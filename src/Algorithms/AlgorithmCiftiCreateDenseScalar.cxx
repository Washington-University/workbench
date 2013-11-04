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

#include "AlgorithmCiftiCreateDenseScalar.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiCreateDenseTimeseries.h" //for making the dense mapping from metric files
#include "CiftiFile.h"
#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "MetricFile.h"
#include "StructureEnum.h"
#include "VolumeFile.h"

#include <map>
#include <vector>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCreateDenseScalar::getCommandSwitch()
{
    return "-cifti-create-dense-scalar";
}

AString AlgorithmCiftiCreateDenseScalar::getShortDescription()
{
    return "CREATE A CIFTI DENSE SCALAR FILE";
}

OperationParameters* AlgorithmCiftiCreateDenseScalar::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiOutputParameter(1, "cifti-out", "the output cifti file");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(2, "-volume", "volume component");
    volumeOpt->addVolumeParameter(1, "volume-data", "volume file containing all voxel data for all volume structures");
    volumeOpt->addVolumeParameter(2, "label-volume", "label volume file containing labels for cifti structures");
    
    OptionalParameter* leftMetricOpt = ret->createOptionalParameter(3, "-left-metric", "metric for left surface");
    leftMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* leftRoiOpt = leftMetricOpt->createOptionalParameter(2, "-roi-left", "roi of vertices to use from left surface");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* rightMetricOpt = ret->createOptionalParameter(4, "-right-metric", "metric for left surface");
    rightMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* rightRoiOpt = rightMetricOpt->createOptionalParameter(2, "-roi-right", "roi of vertices to use from right surface");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    OptionalParameter* cerebMetricOpt = ret->createOptionalParameter(5, "-cerebellum-metric", "metric for the cerebellum");
    cerebMetricOpt->addMetricParameter(1, "metric", "the metric file");
    OptionalParameter* cerebRoiOpt = cerebMetricOpt->createOptionalParameter(2, "-roi-cerebellum", "roi of vertices to use from right surface");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the ROI as a metric file");
    
    AString myText = AString("All input files must have the same number of columns/subvolumes.  Only the specified components will be in the output cifti.  ") +
        "Map names will be taken from one of the input files.  " +
        "At least one component must be specified.  The label volume should have some of the label names from this list, all other label names will be ignored:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        myText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

void AlgorithmCiftiCreateDenseScalar::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVol = NULL, *myVolLabel = NULL;
    CiftiFile* myCiftiOut = myParams->getOutputCifti(1);
    OptionalParameter* volumeOpt = myParams->getOptionalParameter(2);
    if (volumeOpt->m_present)
    {
        myVol = volumeOpt->getVolume(1);
        myVolLabel = volumeOpt->getVolume(2);
    }
    MetricFile* leftData = NULL, *leftRoi = NULL, *rightData = NULL, *rightRoi = NULL, *cerebData = NULL, *cerebRoi = NULL;
    OptionalParameter* leftMetricOpt = myParams->getOptionalParameter(3);
    if (leftMetricOpt->m_present)
    {
        leftData = leftMetricOpt->getMetric(1);
        OptionalParameter* leftRoiOpt = leftMetricOpt->getOptionalParameter(2);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
        }
    }
    OptionalParameter* rightMetricOpt = myParams->getOptionalParameter(4);
    if (rightMetricOpt->m_present)
    {
        rightData = rightMetricOpt->getMetric(1);
        OptionalParameter* rightRoiOpt = rightMetricOpt->getOptionalParameter(2);
        if (rightRoiOpt->m_present)
        {
            rightRoi = rightRoiOpt->getMetric(1);
        }
    }
    OptionalParameter* cerebMetricOpt = myParams->getOptionalParameter(5);
    if (cerebMetricOpt->m_present)
    {
        cerebData = cerebMetricOpt->getMetric(1);
        OptionalParameter* cerebRoiOpt = cerebMetricOpt->getOptionalParameter(2);
        if (cerebRoiOpt->m_present)
        {
            cerebRoi = cerebRoiOpt->getMetric(1);
        }
    }
    AlgorithmCiftiCreateDenseScalar(myProgObj, myCiftiOut, myVol, myVolLabel, leftData, leftRoi, rightData, rightRoi, cerebData, cerebRoi);//executes the algorithm
}

AlgorithmCiftiCreateDenseScalar::AlgorithmCiftiCreateDenseScalar(ProgressObject* myProgObj, CiftiFile* myCiftiOut, const VolumeFile* myVol,
                                                                 const VolumeFile* myVolLabel, const MetricFile* leftData, const MetricFile* leftRoi,
                                                                 const MetricFile* rightData, const MetricFile* rightRoi, const MetricFile* cerebData,
                                                                 const MetricFile* cerebRoi) : AbstractAlgorithm(myProgObj)
{
    CaretAssert(myCiftiOut != NULL);
    LevelProgress myProgress(myProgObj);
    CiftiXML myXML;
    AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(myXML, CiftiXML::ALONG_COLUMN, myVol, myVolLabel, leftData, leftRoi, rightData, rightRoi, cerebData, cerebRoi);
    int numMaps = -1;
    const CaretMappableDataFile* nameFile = NULL;
    if (leftData != NULL)
    {
        numMaps = leftData->getNumberOfMaps();
        nameFile = leftData;
    }
    if (rightData != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = rightData->getNumberOfMaps();
            nameFile = rightData;
        } else {
            if (numMaps != rightData->getNumberOfMaps())
            {
                throw AlgorithmException("right and left surface data have a different number of maps");
            }
        }
    }
    if (cerebData != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = cerebData->getNumberOfMaps();
            nameFile = cerebData;
        } else {
            if (numMaps != cerebData->getNumberOfMaps())
            {
                throw AlgorithmException("cerebellum surface data has a different number of maps");
            }
        }
    }
    if (myVol != NULL)
    {
        if (numMaps == -1)
        {
            numMaps = myVol->getNumberOfMaps();
            nameFile = myVol;
        } else {
            if (numMaps != myVol->getNumberOfMaps())
            {
                throw AlgorithmException("volume data has a different number of maps");
            }
        }
    }
    if (numMaps == -1)//doubles as checking nameFile for being NULL
    {
        throw AlgorithmException("no models specified");
    }
    myXML.resetDirectionToScalars(CiftiXML::ALONG_ROW, numMaps);
    for (int i = 0; i < numMaps; ++i)
    {
        myXML.setMapNameForIndex(CiftiXML::ALONG_ROW, i, nameFile->getMapName(i));//copy map names
    }
    myCiftiOut->setCiftiXML(myXML);
    CaretArray<float> temprow(numMaps);
    vector<CiftiSurfaceMap> surfMap;
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CORTEX_LEFT))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                temprow[t] = leftData->getValue(surfMap[i].m_surfaceNode, t);
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CORTEX_RIGHT))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                temprow[t] = rightData->getValue(surfMap[i].m_surfaceNode, t);
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    if (myXML.getSurfaceMapForColumns(surfMap, StructureEnum::CEREBELLUM))
    {
        for (int64_t i = 0; i < (int)surfMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                temprow[t] = cerebData->getValue(surfMap[i].m_surfaceNode, t);
            }
            myCiftiOut->setRow(temprow, surfMap[i].m_ciftiIndex);
        }
    }
    vector<CiftiVolumeMap> volMap;
    if (myXML.getVolumeMapForColumns(volMap))//we don't need to know which voxel is from which parcel
    {
        for (int64_t i = 0; i < (int)volMap.size(); ++i)
        {
            for (int t = 0; t < numMaps; ++t)
            {
                temprow[t] = myVol->getValue(volMap[i].m_ijk, t);
            }
            myCiftiOut->setRow(temprow, volMap[i].m_ciftiIndex);
        }
    }
}

float AlgorithmCiftiCreateDenseScalar::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCreateDenseScalar::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
