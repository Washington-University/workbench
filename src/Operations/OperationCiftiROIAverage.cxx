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

#include "CiftiFile.h"
#include "MetricFile.h"
#include "OperationCiftiROIAverage.h"
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
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to average in");
    ret->addStringParameter(2, "text-out", "output text file of the average values");
    
    OptionalParameter* leftRoiOpt = ret->createOptionalParameter(3, "-left-roi", "vertices to use from left hempsphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    
    OptionalParameter* rightRoiOpt = ret->createOptionalParameter(4, "-right-roi", "vertices to use from right hempsphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    
    OptionalParameter* cerebRoiOpt = ret->createOptionalParameter(5, "-cerebellum-roi", "vertices to use from cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    
    OptionalParameter* volRoiOpt = ret->createOptionalParameter(6, "-vol-roi", "voxels to use");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    ret->setHelpText(
        AString("Average the rows that are within the specified ROIs, and write the resulting average row to a text file, seperated by newlines.")
    );
    return ret;
}

void OperationCiftiROIAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myCifti = myParams->getCifti(1);
    AString textFileName = myParams->getString(2);
    fstream textFile(textFileName.toLocal8Bit().constData(), fstream::out | fstream::trunc);
    if (!textFile.good())
    {
        throw OperationException("error opening output file for writing");
    }
    int numCols = myCifti->getNumberOfColumns();
    vector<double> accum(numCols, 0.0);
    int accumCount = 0;
    OptionalParameter* leftRoiOpt = myParams->getOptionalParameter(3);
    if (leftRoiOpt->m_present)
    {
        MetricFile* tempMetric = leftRoiOpt->getMetric(1);
        processSurfaceComponent(myCifti, StructureEnum::CORTEX_LEFT, tempMetric, accum, accumCount);
    }
    OptionalParameter* rightRoiOpt = myParams->getOptionalParameter(4);
    if (rightRoiOpt->m_present)
    {
        MetricFile* tempMetric = rightRoiOpt->getMetric(1);
        processSurfaceComponent(myCifti, StructureEnum::CORTEX_RIGHT, tempMetric, accum, accumCount);
    }
    OptionalParameter* cerebRoiOpt = myParams->getOptionalParameter(5);
    if (cerebRoiOpt->m_present)
    {
        MetricFile* tempMetric = cerebRoiOpt->getMetric(1);
        processSurfaceComponent(myCifti, StructureEnum::CEREBELLUM, tempMetric, accum, accumCount);
    }
    OptionalParameter* volRoiOpt = myParams->getOptionalParameter(6);
    if (volRoiOpt->m_present)
    {
        VolumeFile* tempVol = volRoiOpt->getVolume(1);
        processVolume(myCifti, tempVol, accum, accumCount);
    }
    if (accumCount == 0)
    {
        throw OperationException("ROIs don't match any data");
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
    if (myCifti->getColumnSurfaceNumberOfNodes(myStruct) != numNodes)
    {
        throw OperationException("roi number of vertices doesn't match for structure " + StructureEnum::toName(myStruct));
    }
    vector<float> scratch(numCols);
    vector<CiftiBrainModelsMap::SurfaceMap> myMap;
    myCifti->getSurfaceMapForColumns(myMap, myStruct);
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
