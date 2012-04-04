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

#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmException.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "CiftiFile.h"
#include "CaretPointer.h"
#include "Vector3D.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiReplaceStructure::getCommandSwitch()
{
    return "-cifti-replace-structure";
}

AString AlgorithmCiftiReplaceStructure::getShortDescription()
{
    return "REPLACE DATA IN A STRUCTURE IN A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiReplaceStructure::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "cifti", "the cifti to modify");//in place read/write is not a case in OperationParameters currently, could hack it by using a CiftiParameter...
    
    ret->addStringParameter(2, "direction", "which dimension to interpret as a single map, ROW or COLUMN");
    
    OptionalParameter* metricOpt = ret->createOptionalParameter(3, "-metric", "replace the data in a surface component");
    metricOpt->addStringParameter(1, "structure", "the structure to replace the data of");
    metricOpt->addMetricParameter(2, "metric", "the input metric");

    OptionalParameter* volumeOpt = ret->createOptionalParameter(4, "-volume", "replace the data in a volume component");
    volumeOpt->addStringParameter(1, "structure", "the structure to replace the data of");
    volumeOpt->addVolumeOutputParameter(2, "volume", "the input volume");
    volumeOpt->createOptionalParameter(3, "-from-cropped", "interpret the input as a volume cropped to the size of the parcel");

    AString helpText = AString("You must specify -metric or -volume for this command to do anything.  Input volumes must line up with the ") +
        "output of -cifti-separate.  For dtseries, use COLUMN, and if your matrix is fully symmetric, COLUMN is " +
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

void AlgorithmCiftiReplaceStructure::useParameters(OperationParameters* myParams, ProgressObject* /*myProgObj*/)
{
    AString ciftiName = myParams->getString(1);
    CiftiFile myCifti(ciftiName, ON_DISK);
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
        MetricFile* metricIn = metricOpt->getMetric(2);
        AlgorithmCiftiReplaceStructure(NULL, &myCifti, myDir, myStruct, metricIn);
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
        VolumeFile* volIn = volumeOpt->getOutputVolume(2);
        bool fromCropVol = volumeOpt->getOptionalParameter(3)->m_present;
        AlgorithmCiftiReplaceStructure(NULL, &myCifti, myDir, myStruct, volIn, fromCropVol);
    }
}

AlgorithmCiftiReplaceStructure::AlgorithmCiftiReplaceStructure(ProgressObject* myProgObj, CiftiFile* ciftiInOut, const CiftiInterface::CiftiDirection& myDir,
                                                               const StructureEnum::Enum& myStruct, const MetricFile* metricIn) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<CiftiSurfaceMap> myMap;
    int rowSize = ciftiInOut->getNumberOfColumns(), colSize = ciftiInOut->getNumberOfRows();
    if (myDir == CiftiInterface::ALONG_COLUMN)
    {
        if (!ciftiInOut->getSurfaceMapForColumns(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiInOut->getColumnSurfaceNumberOfNodes(myStruct);
        if (metricIn->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("input metric has the wrong number of nodes");
        }
        if (metricIn->getNumberOfColumns() != rowSize)
        {
            throw AlgorithmException("input metric has the wrong number of columns");
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        for (int i = 0; i < mapSize; ++i)
        {
            for (int j = 0; j < rowSize; ++j)
            {
                rowScratch[j] = metricIn->getValue(myMap[i].m_surfaceNode, j);
            }
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
    } else {
        if (!ciftiInOut->getSurfaceMapForRows(myMap, myStruct))
        {
            throw AlgorithmException("structure not found in specified dimension");
        }
        int64_t numNodes = ciftiInOut->getRowSurfaceNumberOfNodes(myStruct);
        if (metricIn->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("input metric has the wrong number of nodes");
        }
        if (metricIn->getNumberOfColumns() != colSize)
        {
            throw AlgorithmException("input metric has the wrong number of columns");
        }
        int mapSize = (int)myMap.size();
        CaretArray<float> rowScratch(rowSize);
        for (int i = 0; i < colSize; ++i)
        {
            ciftiInOut->getRow(rowScratch, i);
            for (int j = 0; j < mapSize; ++j)
            {
                rowScratch[myMap[j].m_ciftiIndex] = metricIn->getValue(myMap[j].m_surfaceNode, i);
            }
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
    }
}

AlgorithmCiftiReplaceStructure::AlgorithmCiftiReplaceStructure(ProgressObject* myProgObj, CiftiFile* ciftiInOut, const CiftiInterface::CiftiDirection& myDir,
                                                               const StructureEnum::Enum& myStruct, const VolumeFile* volIn, const bool& fromCropped) : AbstractAlgorithm(myProgObj)
{
    const CiftiXML& myXML = ciftiInOut->getCiftiXML();
    LevelProgress myProgress(myProgObj);
    int64_t myDims[3], offset[3];
    vector<vector<float> > mySform;
    vector<CiftiVolumeMap> myMap;
    vector<int64_t> newdims;
    int rowSize = ciftiInOut->getNumberOfColumns(), colSize = ciftiInOut->getNumberOfRows();
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
    if (fromCropped)
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
            throw AlgorithmException("cropped volume specified, but no voxels exist in this structure");
        }
    } else {
        newdims.push_back(myDims[0]);
        newdims.push_back(myDims[1]);
        newdims.push_back(myDims[2]);
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = 0;
    }
    if (!volIn->matchesVolumeSpace(mySform, newdims.data()))
    {
        throw AlgorithmException("input volume doesn't match volume space and dimensions in CIFTI");
    }
    CaretArray<float> rowScratch(rowSize);
    vector<int64_t> volDims;
    volIn->getDimensions(volDims);
    if (myDir == CiftiInterface::ALONG_COLUMN)
    {
        if (volDims[3] != rowSize)
        {
            throw AlgorithmException("volume has the wrong number of subvolumes");
        }
        for (int64_t i = 0; i < numVoxels; ++i)
        {
            int64_t thisvoxel[3] = { myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2] };
            for (int j = 0; j < rowSize; ++j)
            {
                rowScratch[j] = volIn->getValue(thisvoxel, j);
            }
            ciftiInOut->setRow(rowScratch, myMap[i].m_ciftiIndex);
        }
    } else {
        if (volDims[3] != colSize)
        {
            throw AlgorithmException("volume has the wrong number of subvolumes");
        }
        for (int64_t i = 0; i < colSize; ++i)
        {
            ciftiInOut->getRow(rowScratch, i);
            for (int64_t j = 0; j < numVoxels; ++j)
            {
                int64_t thisvoxel[3] = { myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2] };
                rowScratch[myMap[j].m_ciftiIndex] = volIn->getValue(thisvoxel, i);
            }
            ciftiInOut->setRow(rowScratch, i);
        }
    }
}

float AlgorithmCiftiReplaceStructure::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiReplaceStructure::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
