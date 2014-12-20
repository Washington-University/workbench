/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "OperationCiftiROIAverage.h"

#include "AlgorithmCiftiSeparate.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "OperationException.h"
#include "VolumeFile.h"

#include <fstream>

using namespace caret;
using namespace std;

AString OperationCiftiROIAverage::getCommandSwitch()
{
    return "-cifti-roi-average";
}

AString OperationCiftiROIAverage::getShortDescription()
{
    return "AVERAGE ROWS IN A SINGLE CIFTI FILE";
}

OperationParameters* OperationCiftiROIAverage::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to average rows from");
    ret->addStringParameter(2, "text-out", "output text file of the average values");
    
    OptionalParameter* ciftiRoiOpt = ret->createOptionalParameter(3, "-cifti-roi", "cifti file containing combined rois");
    ciftiRoiOpt->addCiftiParameter(1, "roi-cifti", "the rois as a cifti file");
    
    OptionalParameter* leftRoiOpt = ret->createOptionalParameter(4, "-left-roi", "vertices to use from left hemisphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    
    OptionalParameter* rightRoiOpt = ret->createOptionalParameter(5, "-right-roi", "vertices to use from right hemisphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    
    OptionalParameter* cerebRoiOpt = ret->createOptionalParameter(6, "-cerebellum-roi", "vertices to use from cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    
    OptionalParameter* volRoiOpt = ret->createOptionalParameter(7, "-vol-roi", "voxels to use");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    ret->setHelpText(
        AString("Average the rows that are within the specified ROIs, and write the resulting average row to a text file, separated by newlines.  ") +
        "If -cifti-roi is specified, -left-roi, -right-roi, -cerebellum-roi, and -vol-roi must not be specified."
    );
    return ret;
}

void OperationCiftiROIAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myCifti = myParams->getCifti(1);
    if (myCifti->getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw OperationException("input cifti file does not have a brain models mapping along column");
    }
    AString textFileName = myParams->getString(2);
    fstream textFile(textFileName.toLocal8Bit().constData(), fstream::out | fstream::trunc);
    if (!textFile.good())
    {
        throw OperationException("error opening output file for writing");
    }
    int numCols = myCifti->getNumberOfColumns();
    vector<double> accum(numCols, 0.0);
    int accumCount = 0;
    CiftiFile* ciftiROI = NULL;
    OptionalParameter* ciftiRoiOpt = myParams->getOptionalParameter(3);
    if (ciftiRoiOpt->m_present)
    {
        ciftiROI = ciftiRoiOpt->getCifti(1);
    }
    OptionalParameter* leftRoiOpt = myParams->getOptionalParameter(4);
    if (leftRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw OperationException("-cifti-roi cannot be used with any other ROI option");
        MetricFile* tempMetric = leftRoiOpt->getMetric(1);
        processSurfaceComponent(myCifti, StructureEnum::CORTEX_LEFT, tempMetric, accum, accumCount);
    }
    OptionalParameter* rightRoiOpt = myParams->getOptionalParameter(5);
    if (rightRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw OperationException("-cifti-roi cannot be used with any other ROI option");
        MetricFile* tempMetric = rightRoiOpt->getMetric(1);
        processSurfaceComponent(myCifti, StructureEnum::CORTEX_RIGHT, tempMetric, accum, accumCount);
    }
    OptionalParameter* cerebRoiOpt = myParams->getOptionalParameter(6);
    if (cerebRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw OperationException("-cifti-roi cannot be used with any other ROI option");
        MetricFile* tempMetric = cerebRoiOpt->getMetric(1);
        processSurfaceComponent(myCifti, StructureEnum::CEREBELLUM, tempMetric, accum, accumCount);
    }
    OptionalParameter* volRoiOpt = myParams->getOptionalParameter(7);
    if (volRoiOpt->m_present)
    {
        if (ciftiROI != NULL) throw OperationException("-cifti-roi cannot be used with any other ROI option");
        VolumeFile* tempVol = volRoiOpt->getVolume(1);
        processVolume(myCifti, tempVol, accum, accumCount);
    }
    if (ciftiROI != NULL)
    {
        const CiftiXML& roiXML = ciftiROI->getCiftiXML();
        const CiftiXML& inputXML = myCifti->getCiftiXML();
        if (!(roiXML.getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(*inputXML.getMap(CiftiXML::ALONG_COLUMN))))
        {
            throw OperationException("dense mappings of input and roi cifti files don't match");
        }
        const CiftiBrainModelsMap& roiDense = roiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
        if (roiDense.hasVolumeData())
        {
            VolumeFile roiVol;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, ciftiROI, CiftiXML::ALONG_COLUMN, &roiVol, offset, NULL, false);//don't crop because there should be only one map, and processVolume doesn't currently accept cropped
            processVolume(myCifti, &roiVol, accum, accumCount);
        }
        vector<StructureEnum::Enum> surfStructs = roiDense.getSurfaceStructureList();
        for (int i = 0; i < (int)surfStructs.size(); ++i)
        {
            MetricFile roiMetric;
            AlgorithmCiftiSeparate(NULL, ciftiROI, CiftiXML::ALONG_COLUMN, surfStructs[i], &roiMetric);
            processSurfaceComponent(myCifti, surfStructs[i], &roiMetric, accum, accumCount);
        }
    }
    if (accumCount == 0)
    {
        throw OperationException("ROI(s) don't match any data");
    }
    for (int i = 0; i < numCols; ++i)
    {
        textFile << accum[i] / accumCount << endl;
    }
}

void OperationCiftiROIAverage::processSurfaceComponent(const CiftiFile* myCifti, const StructureEnum::Enum& myStruct, const MetricFile* myRoi, vector<double>& accum, int& accumCount)
{
    int numCols = myCifti->getNumberOfColumns();
    int numNodes = myRoi->getNumberOfNodes();
    const CiftiBrainModelsMap& myDenseMap = myCifti->getCiftiXML().getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (myDenseMap.getSurfaceNumberOfNodes(myStruct) != numNodes)
    {
        throw OperationException("roi number of vertices doesn't match for structure " + StructureEnum::toName(myStruct));
    }
    vector<float> scratch(numCols);
    vector<CiftiBrainModelsMap::SurfaceMap> myMap = myDenseMap.getSurfaceMap(myStruct);
    int mapSize = myMap.size();
    for (int i = 0; i < mapSize; ++i)
    {
        if (myRoi->getValue(myMap[i].m_surfaceNode, 0) > 0.0f)
        {
            ++accumCount;
            myCifti->getRow(scratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < numCols; ++j)
            {
                accum[j] += scratch[j];
            }
        }
    }
}

void OperationCiftiROIAverage::processVolume(const CiftiFile* myCifti, const VolumeFile* myRoi, vector<double>& accum, int& accumCount)
{
    int numCols = myCifti->getNumberOfColumns();
    const CiftiXMLOld& myXml = myCifti->getCiftiXMLOld();
    int64_t dims[3];
    vector<vector<float> > sform;
    if (!myXml.getVolumeDimsAndSForm(dims, sform))
    {
        throw OperationException("no volume data in cifti file");
    }
    if (!myRoi->matchesVolumeSpace(dims, sform))
    {
        throw OperationException("volume roi doesn't match cifti volume space");
    }
    vector<float> scratch(numCols);
    vector<CiftiVolumeMap> myMap;
    myXml.getVolumeMapForColumns(myMap);
    int mapSize = myMap.size();
    for (int i = 0; i < mapSize; ++i)
    {
        if (myRoi->getValue(myMap[i].m_ijk) > 0.0f)
        {
            ++accumCount;
            myCifti->getRow(scratch.data(), myMap[i].m_ciftiIndex);
            for (int j = 0; j < numCols; ++j)
            {
                accum[j] += scratch[j];
            }
        }
    }
}
