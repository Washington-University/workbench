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

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmException.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "CaretPointer.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiSeparate::getCommandSwitch()
{
    return "-cifti-separate";
}

AString AlgorithmCiftiSeparate::getShortDescription()
{
    return "SEPARATE A CIFTI MODEL INTO METRIC OR VOLUME";
}

OperationParameters* AlgorithmCiftiSeparate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the cifti to separate a component of");
    
    ret->addStringParameter(2, "direction", "which dimension to interpret as a single map, ROW or COLUMN");
    
    OptionalParameter* metricOpt = ret->createOptionalParameter(3, "-metric", "separate a surface model into a metric file");
    metricOpt->addStringParameter(1, "structure", "the structure to output");
    metricOpt->addMetricOutputParameter(2, "metric-out", "the output metric");
    OptionalParameter* metricRoiOpt = metricOpt->createOptionalParameter(3, "-roi", "also output the roi of which nodes have data, in case the structure isn't the full surface");
    metricRoiOpt->addMetricOutputParameter(1, "roi-out", "the roi output metric");
    
    OptionalParameter* volumeOpt = ret->createOptionalParameter(4, "-volume", "separate a volume model into a volume file");
    volumeOpt->addStringParameter(1, "structure", "the structure to output");
    volumeOpt->addVolumeOutputParameter(2, "volume-out", "the output volume");
    OptionalParameter* volumeRoiOpt = volumeOpt->createOptionalParameter(3, "-roi", "also output the roi of which voxels have data");
    volumeRoiOpt->addVolumeOutputParameter(1, "roi-out", "the roi output volume");
    volumeOpt->createOptionalParameter(4, "-crop", "crop volumes to the size of the parcel rather than using the original volume size");
    
    AString helpText = AString("You must specify -metric or -volume for this command to do anything.  Output volumes should line up with their ") +
        "original positions, whether or not they are cropped.  For dtseries, use COLUMN, and if your matrix is fully symmetric, COLUMN is " +
        "more efficient.  The structure argument must be one of the following:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        helpText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

void AlgorithmCiftiSeparate::useParameters(OperationParameters* myParams, ProgressObject* /*myProgObj*/)
{//ignore the progress object for now, and allow specifying both options at once
    CiftiFile* ciftiIn = myParams->getCifti(1);
    AString dirName = myParams->getString(2);
    CiftiInterface::CiftiDirection myDir;
    if (dirName == "ROW")
    {
        myDir = CiftiInterface::ALONG_ROW;
    } else if (dirName == "COLUMN") {
        myDir = CiftiInterface::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    OptionalParameter* metricOpt = myParams->getOptionalParameter(3);
    if (metricOpt->m_present)
    {
        AString structName = metricOpt->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type");
        }
        MetricFile* metricOut = metricOpt->getOutputMetric(2);
        MetricFile* roiOut = NULL;
        OptionalParameter* metricRoiOpt = metricOpt->getOptionalParameter(3);
        if (metricRoiOpt->m_present)
        {
            roiOut = metricRoiOpt->getOutputMetric(1);
        }
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, metricOut, roiOut);
    }
    OptionalParameter* volumeOpt = myParams->getOptionalParameter(4);
    if (volumeOpt->m_present)
    {
        AString structName = volumeOpt->getString(1);
        bool ok = false;
        StructureEnum::Enum myStruct = StructureEnum::fromName(structName, &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized structure type");
        }
        VolumeFile* volOut = volumeOpt->getOutputVolume(2);
        VolumeFile* roiOut = NULL;
        OptionalParameter* volumeRoiOpt = volumeOpt->getOptionalParameter(3);
        if (volumeRoiOpt->m_present)
        {
            roiOut = volumeRoiOpt->getOutputVolume(1);
        }
        bool cropVol = volumeOpt->getOptionalParameter(4)->m_present;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, ciftiIn, myDir, myStruct, volOut, offset, roiOut, cropVol);
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const CiftiInterface::CiftiDirection& myDir,
                                               const StructureEnum::Enum& myStruct, MetricFile* metricOut, MetricFile* roiOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<CiftiSurfaceMap> myMap;
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (myDir == CiftiInterface::ALONG_COLUMN)
    {
        if (!ciftiIn->getSurfaceMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiIn->getColumnSurfaceNumberOfNodes(myStruct);
        metricOut->setNumberOfNodesAndColumns(numNodes, rowSize);
        if (roiOut != NULL)
        {
            roiOut->setNumberOfNodesAndColumns(numNodes, 1);
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        CaretArray<float> nodeUsed(numNodes, 0.0f);
        for (int i = 0; i < mapSize; ++i)
        {
            ciftiIn->getRow(rowScratch, myMap[i].m_ciftiIndex);
            nodeUsed[myMap[i].m_surfaceNode] = 1.0f;
            for (int j = 0; j < rowSize; ++j)
            {
                metricOut->setValue(myMap[i].m_surfaceNode, j, rowScratch[j]);
            }
        }
        if (roiOut != NULL)
        {
            roiOut->setValuesForColumn(0, nodeUsed);
        }
        for (int i = 0; i < numNodes; ++i)//zero unused columns
        {
            if (nodeUsed[i] == 0.0f)
            {
                for (int j = 0; j < rowSize; ++j)
                {
                    metricOut->setValue(i, j, 0.0f);
                }
            }
        }
    } else {
        if (!ciftiIn->getSurfaceMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiIn->getRowSurfaceNumberOfNodes(myStruct);
        metricOut->setNumberOfNodesAndColumns(numNodes, colSize);
        if (roiOut != NULL)
        {
            roiOut->setNumberOfNodesAndColumns(numNodes, 1);
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize), metricScratch(numNodes, 0.0f);
        if (roiOut != NULL)
        {
            CaretArray<float> nodeUsed(numNodes, 0.0f);
            for (int i = 0; i < mapSize; ++i)
            {
                nodeUsed[myMap[i].m_surfaceNode] = 1.0f;
            }
            roiOut->setValuesForColumn(0, nodeUsed);
        }
        for (int i = 0; i < colSize; ++i)
        {
            ciftiIn->getRow(rowScratch, i);
            for (int j = 0; j < mapSize; ++j)
            {
                metricScratch[myMap[j].m_surfaceNode] = rowScratch[myMap[j].m_ciftiIndex];
            }
            metricOut->setValuesForColumn(i, metricScratch);
        }
    }
}

AlgorithmCiftiSeparate::AlgorithmCiftiSeparate(ProgressObject* myProgObj, const CiftiFile* ciftiIn, const CiftiInterface::CiftiDirection& myDir,
                                               const StructureEnum::Enum& myStruct, VolumeFile* volOut, int64_t offset[3],
                                               VolumeFile* roiOut, const bool& cropVol) : AbstractAlgorithm(myProgObj)
{
    const CiftiXML& myXML = ciftiIn->getCiftiXML();
    LevelProgress myProgress(myProgObj);
    int64_t myDims[3];
    vector<vector<float> > mySform;
    vector<CiftiVolumeMap> myMap;
    vector<int64_t> newdims;
    int rowSize = ciftiIn->getNumberOfColumns(), colSize = ciftiIn->getNumberOfRows();
    if (!myXML.getVolumeDimsAndSForm(myDims, mySform))
    {
        throw AlgorithmException("input cifti has no volume space information");
    }
    if (myDir == CiftiInterface::ALONG_COLUMN)
    {
        if (!myXML.getVolumeStructureMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
    } else {
        if (!myXML.getVolumeStructureMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
    }
    int64_t numVoxels = (int64_t)myMap.size();
    if (cropVol)
    {
        if (numVoxels > 0)
        {//make a voxel bounding box to minimize memory usage
            int extrema[6] = { myMap[0].m_ijk[0],
                myMap[0].m_ijk[0],
                myMap[0].m_ijk[1],
                myMap[0].m_ijk[1],
                myMap[0].m_ijk[2],
                myMap[0].m_ijk[2]
            };
            for (int64_t i = 1; i < numVoxels; ++i)
            {
                if (myMap[i].m_ijk[0] < extrema[0]) extrema[0] = myMap[i].m_ijk[0];
                if (myMap[i].m_ijk[0] > extrema[1]) extrema[1] = myMap[i].m_ijk[0];
                if (myMap[i].m_ijk[1] < extrema[2]) extrema[2] = myMap[i].m_ijk[1];
                if (myMap[i].m_ijk[1] > extrema[3]) extrema[3] = myMap[i].m_ijk[1];
                if (myMap[i].m_ijk[2] < extrema[4]) extrema[4] = myMap[i].m_ijk[2];
                if (myMap[i].m_ijk[2] > extrema[5]) extrema[5] = myMap[i].m_ijk[2];
            }
            newdims.push_back(extrema[1] - extrema[0] + 1);
            newdims.push_back(extrema[3] - extrema[2] + 1);
            newdims.push_back(extrema[5] - extrema[4] + 1);
            offset[0] = extrema[0];
            offset[1] = extrema[2];
            offset[2] = extrema[4];
            Vector3D ivec, jvec, kvec, shift;
            ivec[0] = mySform[0][0]; ivec[1] = mySform[1][0]; ivec[2] = mySform[2][0];
            jvec[0] = mySform[0][1]; jvec[1] = mySform[1][1]; jvec[2] = mySform[2][1];
            kvec[0] = mySform[0][2]; kvec[1] = mySform[1][2]; kvec[2] = mySform[2][2];
            shift = offset[0] * ivec + offset[1] * jvec + offset[2] * kvec;
            mySform[0][3] += shift[0];//fix the sform to align to the old position with the new dimensions
            mySform[1][3] += shift[1];
            mySform[2][3] += shift[2];
        } else {
            throw AlgorithmException("cropped volume requested, but no voxels exist in this structure");
        }
    } else {
        newdims.push_back(myDims[0]);
        newdims.push_back(myDims[1]);
        newdims.push_back(myDims[2]);
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = 0;
    }
    if (roiOut != NULL)
    {
        roiOut->reinitialize(newdims, mySform);
        roiOut->setValueAllVoxels(0.0f);
    }
    CaretArray<float> rowScratch(rowSize);
    if (myDir == CiftiInterface::ALONG_COLUMN)
    {
        if (rowSize > 1) newdims.push_back(rowSize);
        volOut->reinitialize(newdims, mySform);
        volOut->setValueAllVoxels(0.0f);
        for (int64_t i = 0; i < numVoxels; ++i)
        {
            int64_t thisvoxel[3] = { myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2] };
            if (roiOut != NULL)
            {
                roiOut->setValue(1.0f, thisvoxel);
            }
            ciftiIn->getRow(rowScratch, myMap[i].m_ciftiIndex);
            for (int j = 0; j < rowSize; ++j)
            {
                volOut->setValue(rowScratch[j], thisvoxel, j);
            }
        }
    } else {
        if (rowSize > 1) newdims.push_back(colSize);
        volOut->reinitialize(newdims, mySform);
        volOut->setValueAllVoxels(0.0f);
        for (int64_t i = 0; i < colSize; ++i)
        {
            ciftiIn->getRow(rowScratch, i);
            for (int64_t j = 0; j < numVoxels; ++j)
            {
                int64_t thisvoxel[3] = { myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2] };
                if (i == 0 && roiOut != NULL)
                {
                    roiOut->setValue(1.0f, thisvoxel);
                }
                volOut->setValue(rowScratch[myMap[j].m_ciftiIndex], thisvoxel, i);
            }
        }
    }
}

float AlgorithmCiftiSeparate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiSeparate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
