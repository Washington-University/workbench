/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AlgorithmCiftiAverageROICorrelation.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "FileInformation.h"
#include "MetricFile.h"
#include "VolumeFile.h"

#include <cmath>
#include <fstream>
#include <string>

using namespace caret;
using namespace std;

AString AlgorithmCiftiAverageROICorrelation::getCommandSwitch()
{
    return "-cifti-average-roi-correlation";
}

AString AlgorithmCiftiAverageROICorrelation::getShortDescription()
{
    return "CORRELATE ROI AVERAGE WITH ALL ROWS THEN AVERAGE ACROSS SUBJECTS";
}

OperationParameters* AlgorithmCiftiAverageROICorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "cifti-list-file", "a text file containing a list of cifti filenames to use");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "output cifti file");
    
    OptionalParameter* leftRoiOpt = ret->createOptionalParameter(3, "-left-roi", "vertices to use from left hempsphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    
    OptionalParameter* rightRoiOpt = ret->createOptionalParameter(4, "-right-roi", "vertices to use from right hempsphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    
    OptionalParameter* cerebRoiOpt = ret->createOptionalParameter(5, "-cerebellum-roi", "vertices to use from cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    
    OptionalParameter* volRoiOpt = ret->createOptionalParameter(6, "-vol-roi", "voxels to use");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    ret->setHelpText(
        AString("Averages rows within the ROI(s), takes the correlation of the row average to the rest of the rows in the same file, then averages the results across all files.")
    );
    return ret;
}

void AlgorithmCiftiAverageROICorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString listFileName = myParams->getString(1);
    CiftiFile* ciftiOut = myParams->getOutputCifti(2);
    MetricFile* leftROI = NULL;
    OptionalParameter* leftRoiOpt = myParams->getOptionalParameter(3);
    if (leftRoiOpt->m_present)
    {
        leftROI = leftRoiOpt->getMetric(1);
    }
    MetricFile* rightROI = NULL;
    OptionalParameter* rightRoiOpt = myParams->getOptionalParameter(4);
    if (rightRoiOpt->m_present)
    {
        rightROI = rightRoiOpt->getMetric(1);
    }
    MetricFile* cerebROI = NULL;
    OptionalParameter* cerebRoiOpt = myParams->getOptionalParameter(5);
    if (cerebRoiOpt->m_present)
    {
        cerebROI = cerebRoiOpt->getMetric(1);
    }
    VolumeFile* volROI = NULL;
    OptionalParameter* volRoiOpt = myParams->getOptionalParameter(6);
    if (volRoiOpt->m_present)
    {
        volROI = volRoiOpt->getVolume(1);
    }
    fstream textFile(listFileName.toLocal8Bit().constData(), fstream::in);
    if (!textFile.good())
    {
        throw AlgorithmException("error opening input file for reading");
    }
    vector<const CiftiInterface*> ciftiList;
    try
    {
        string myLine;
        while (textFile.good())
        {
            if (!getline(textFile, myLine))
            {
                break;
            }
            if (myLine == "")
            {
                continue;//skip blank lines
            }
            FileInformation ciftiFileInfo(myLine.c_str());
            if (!ciftiFileInfo.exists())
            {
                throw AlgorithmException(AString("file does not exist: ") + myLine.c_str());//throw inside try block so that the error handling path is the same
            }
            CiftiFile* tempCifti = new CiftiFile(myLine.c_str(), ON_DISK);
            ciftiList.push_back(tempCifti);
        }
        AlgorithmCiftiAverageROICorrelation(myProgObj, ciftiList, ciftiOut, leftROI, rightROI, cerebROI, volROI);
    } catch (CaretException& e) {//catch exceptions to prevent memory leaks
        for (size_t i = 0; i < ciftiList.size(); ++i)
        {
            delete ciftiList[i];
        }
        throw e;
    } catch (std::exception& e) {
        for (size_t i = 0; i < ciftiList.size(); ++i)
        {
            delete ciftiList[i];
        }
        throw AlgorithmException(e.what());
    } catch (...) {
        for (size_t i = 0; i < ciftiList.size(); ++i)
        {
            delete ciftiList[i];
        }
        throw;
    }
    for (size_t i = 0; i < ciftiList.size(); ++i)
    {
        delete ciftiList[i];
    }
}

AlgorithmCiftiAverageROICorrelation::AlgorithmCiftiAverageROICorrelation(ProgressObject* myProgObj, const vector<const CiftiInterface*>& ciftiList, CiftiFile* ciftiOut,
                                                             const MetricFile* leftROI, const MetricFile* rightROI, const MetricFile* cerebROI, const VolumeFile* volROI) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(ciftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    int numCifti = (int)ciftiList.size();
    if (numCifti < 1) throw AlgorithmException("no cifti files specified to average");
    const CiftiXML& baseXML = ciftiList[0]->getCiftiXML();
    int rowSize = baseXML.getNumberOfColumns();
    int colSize = baseXML.getNumberOfRows();
    vector<double> accum(colSize, 0.0);
    vector<float> tempresult(colSize);
    for (int i = 0; i < numCifti; ++i)
    {
        const CiftiXML& thisXML = ciftiList[i]->getCiftiXML();
        if (!thisXML.matchesForRows(baseXML)) throw AlgorithmException("cifti rows do not match between #1 and #" + AString::number(i + 1));
        if (thisXML.getNumberOfColumns() != rowSize) throw AlgorithmException("ERROR: row size doesn't match, but CiftiXML matchesForRows() returned true!");
        if (leftROI != NULL)
        {
            verifySurfaceComponent(i, ciftiList[i], StructureEnum::CORTEX_LEFT, leftROI);
        }
        if (rightROI != NULL)
        {
            verifySurfaceComponent(i, ciftiList[i], StructureEnum::CORTEX_RIGHT, rightROI);
        }
        if (cerebROI != NULL)
        {
            verifySurfaceComponent(i, ciftiList[i], StructureEnum::CEREBELLUM, cerebROI);
        }
        if (volROI != NULL)
        {
            verifyVolumeComponent(i, ciftiList[i], volROI);
        }
    }
    for (int i = 0; i < numCifti; ++i)
    {
        processCifti(ciftiList[i], tempresult, leftROI, rightROI, cerebROI, volROI);
        for (int j = 0; j < colSize; ++j)
        {
            accum[j] += tempresult[j];
        }
    }
    for (int i = 0; i < colSize; ++i)
    {
        tempresult[i] = accum[i] / numCifti;
    }
    CiftiXML newXml = baseXML;
    newXml.resetRowsToTimepoints(1.0f, 1);
    ciftiOut->setCiftiXML(newXml);
    ciftiOut->setColumn(tempresult.data(), 0);
}

void AlgorithmCiftiAverageROICorrelation::verifySurfaceComponent(const int& index, const CiftiInterface* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    if (!myXml.hasColumnSurfaceData(myStruct))
    {
        CaretLogWarning("cifti file #" + AString::number(index + 1) + " missing structure " + StructureEnum::toName(myStruct));
        return;
    }
    if (myRoi->getNumberOfNodes() != myXml.getColumnSurfaceNumberOfNodes(myStruct)) throw AlgorithmException("cifti #" + AString::number(index + 1) + " number of vertices does not match roi");
}

void AlgorithmCiftiAverageROICorrelation::verifyVolumeComponent(const int& index, const CiftiInterface* myCifti, const VolumeFile* volROI)
{
    const CiftiXML& myXml = myCifti->getCiftiXML();
    int64_t dims[3];
    vector<vector<float> > sform;
    myXml.getVolumeDimsAndSForm(dims, sform);
    if (!volROI->matchesVolumeSpace(dims, sform)) throw AlgorithmException("cifti file #" + AString::number(index + 1) + " doesn't match the ROI volume's space");
    vector<CiftiVolumeMap> myMap;
    myXml.getVolumeMapForColumns(myMap);
    int mapSize = (int)myMap.size();
    for (int i = 0; i < mapSize; ++i)
    {
        if (!volROI->indexValid(myMap[i].m_ijk)) throw AlgorithmException("cifti file #" + AString::number(index + 1) + " lists invalid voxels");
    }
}

void AlgorithmCiftiAverageROICorrelation::processCifti(const CiftiInterface* myCifti, vector<float>& output, const MetricFile* leftROI, const MetricFile* rightROI, const MetricFile* cerebROI, const VolumeFile* volROI)
{
    int rowSize = myCifti->getNumberOfColumns();
    int colSize = myCifti->getNumberOfRows();
    vector<double> accumarray(rowSize, 0.0);
    int count = 0;
    addSurface(myCifti, StructureEnum::CORTEX_LEFT, accumarray, count, leftROI);
    addSurface(myCifti, StructureEnum::CORTEX_RIGHT, accumarray, count, rightROI);
    addSurface(myCifti, StructureEnum::CEREBELLUM, accumarray, count, cerebROI);
    addVolume(myCifti, accumarray, count, volROI);
    vector<float> average(rowSize);
    double accum = 0.0;
    for (int i = 0; i < rowSize; ++i)
    {
        average[i] = accumarray[i] / count;
        accum += average[i];
    }
    float mean = accum / rowSize;
    accum = 0.0;
    for (int i = 0; i < rowSize; ++i)
    {
        average[i] -= mean;//remove the mean from the average timeseries to optimize the correlation
        accum += average[i] * average[i];
    }
    float rrs = sqrt(accum);//compute this only once
    int curRow = 0;
#pragma omp CARET_PAR
    {
        vector<float> rowscratch(rowSize);
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < colSize; ++i)
        {
            int myRow;
#pragma omp critical
            {
                myRow = curRow;//force sequential reading
                ++curRow;
                myCifti->getRow(rowscratch.data(), myRow);//but never read multiple rows at once from the same file
            }
            double tempaccum = 0.0;//compute mean of new row
            for (int j = 0; j < rowSize; ++j)
            {
                tempaccum += rowscratch[j];
            }
            float thismean = tempaccum / rowSize;
            tempaccum = 0.0;
            double corraccum = 0.0;//correlate
            for (int j = 0; j < rowSize; ++j)
            {
                float tempf = rowscratch[j] - thismean;
                tempaccum += tempf * tempf;//compute rrs on the fly
                corraccum += tempf * average[j];//gather the correlation
            }
            corraccum /= rrs * sqrt(tempaccum);
            if (corraccum > 0.999999) corraccum = 0.999999;
            if (corraccum < -0.999999) corraccum = -0.999999;
            output[i] = 0.5 * log((1 + corraccum) / (1 - corraccum));
        }
    }
}

void AlgorithmCiftiAverageROICorrelation::addSurface(const CiftiInterface* myCifti, StructureEnum::Enum myStruct, vector<double>& accum, int& count, const MetricFile* myRoi)
{
    if (myRoi == NULL) return;
    vector<CiftiSurfaceMap> myMap;
    myCifti->getSurfaceMapForColumns(myMap, myStruct);
    int mapSize = (int)myMap.size();
    int rowSize = myCifti->getNumberOfColumns();
    vector<float> rowscratch(rowSize);
    for (int i = 0; i < mapSize; ++i)
    {
        if (myRoi->getValue(myMap[i].m_surfaceNode, 0) > 0.0f)
        {
            ++count;
            myCifti->getRow(rowscratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                accum[j] += rowscratch[j];
            }
        }
    }
}

void AlgorithmCiftiAverageROICorrelation::addVolume(const CiftiInterface* myCifti, vector<double>& accum, int& count, const VolumeFile* myRoi)
{
    if (myRoi == NULL) return;
    vector<CiftiVolumeMap> myMap;
    myCifti->getVolumeMapForColumns(myMap);
    int mapSize = (int)myMap.size();
    int rowSize = myCifti->getNumberOfColumns();
    vector<float> rowscratch(rowSize);
    for (int i = 0; i < mapSize; ++i)
    {
        if (myRoi->getValue(myMap[i].m_ijk) > 0.0f)
        {
            ++count;
            myCifti->getRow(rowscratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                accum[j] += rowscratch[j];
            }
        }
    }
}

float AlgorithmCiftiAverageROICorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiAverageROICorrelation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
