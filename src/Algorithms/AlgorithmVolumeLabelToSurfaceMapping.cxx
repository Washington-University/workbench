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

#include "AlgorithmVolumeLabelToSurfaceMapping.h"
#include "AlgorithmException.h"

#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeLabelToSurfaceMapping::getCommandSwitch()
{
    return "-volume-label-to-surface-mapping";
}

AString AlgorithmVolumeLabelToSurfaceMapping::getShortDescription()
{
    return "MAP A LABEL VOLUME TO A SURFACE LABEL FILE";
}

OperationParameters* AlgorithmVolumeLabelToSurfaceMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume", "the volume to map data from");
    
    ret->addSurfaceParameter(2, "surface", "the surface to map the data onto");
    
    ret->addLabelOutputParameter(3, "label-out", "the output gifti label file");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(4, "-subvol-select", "select a single subvolume to map");
    subvolumeSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Uses the enclosing voxel mapping method to map label data to a gifti label file.")
    );
    return ret;
}

void AlgorithmVolumeLabelToSurfaceMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVolume = myParams->getVolume(1);
    SurfaceFile* mySurface = myParams->getSurface(2);
    LabelFile* myLabelOut = myParams->getOutputLabel(3);
    int64_t mySubVol = -1;
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(4);
    if (subvolumeSelect->m_present)
    {
        mySubVol = (int)myVolume->getMapIndexFromNameOrNumber(subvolumeSelect->getString(1));
        if (mySubVol < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmVolumeLabelToSurfaceMapping(myProgObj, myVolume, mySurface, myLabelOut, mySubVol);
}

AlgorithmVolumeLabelToSurfaceMapping::AlgorithmVolumeLabelToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface,
                                                                           LabelFile* myLabelOut, const int& mySubVol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myVolume->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("input volume must be a label volume");
    }
    vector<int64_t> myVolDims;
    myVolume->getDimensions(myVolDims);
    if (mySubVol >= myVolDims[3] || mySubVol < -1)
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    int64_t numColumns;
    if (mySubVol == -1)
    {
        numColumns = myVolDims[3] * myVolDims[4];
    } else {
        numColumns = myVolDims[4];
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    myLabelOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myLabelOut->setStructure(mySurface->getStructure());
    vector<int32_t> myArray(numNodes);
    if (mySubVol == -1)
    {
        GiftiLabelTable cumulativeTable = *(myVolume->getMapLabelTable(0));//so we don't run into append issues with the "???" label that gets made by GiftiLabelTable's constructor
        map<int32_t,int32_t> cumulativeRemap;
        for (int64_t i = 0; i < myVolDims[3]; ++i)
        {
            if (i > 0)
            {
                const GiftiLabelTable* tempTable = myVolume->getMapLabelTable(i);
                if (tempTable == NULL)
                {
                    throw AlgorithmException("a subvolume is missing a label table");
                }
                map<int32_t,int32_t> newMap = cumulativeTable.append(*tempTable);
                cumulativeRemap.insert(newMap.begin(), newMap.end());
            }
            for (int64_t j = 0; j < myVolDims[4]; ++j)
            {
                AString mapName = myVolume->getMapName(i);
                if (myVolDims[4] != 1)
                {
                    mapName += " component " + AString::number(j);
                }
                int64_t thisCol = i * myVolDims[4] + j;
                myLabelOut->setColumnName(thisCol, mapName);
#pragma omp CARET_PARFOR
                for (int64_t node = 0; node < numNodes; ++node)
                {
                    int32_t tempKey = (int32_t)floor(myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::ENCLOSING_VOXEL, NULL, i, j) + 0.5f);
                    map<int32_t, int32_t>::iterator iter = cumulativeRemap.find(tempKey);
                    if (iter != cumulativeRemap.end())
                    {
                        myArray[node] = iter->second;
                    } else {
                        myArray[node] = tempKey;//for simplicity, assume all values in the volume file are in the label table
                    }
                }
                myLabelOut->setLabelKeysForColumn(thisCol, myArray.data());
            }
        }
        *(myLabelOut->getLabelTable()) = cumulativeTable;
    } else {
        const GiftiLabelTable* tempTable = myVolume->getMapLabelTable(mySubVol);
        if (tempTable == NULL)
        {
            throw AlgorithmException("specified subvolume is missing a label table");
        }
        *(myLabelOut->getLabelTable()) = *tempTable;
        for (int64_t j = 0; j < myVolDims[4]; ++j)
        {
            AString mapName = myVolume->getMapName(mySubVol);
            if (myVolDims[4] != 1)
            {
                mapName += " component " + AString::number(j);
            }
            int64_t thisCol = j;
            myLabelOut->setColumnName(thisCol, mapName);
#pragma omp CARET_PARFOR
            for (int64_t node = 0; node < numNodes; ++node)
            {//for simplicity, assume all values in the volume file are in the label table
                myArray[node] = (int32_t)floor(myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::ENCLOSING_VOXEL, NULL, mySubVol, j) + 0.5f);
            }
            myLabelOut->setLabelKeysForColumn(thisCol, myArray.data());
        }
    }
}

float AlgorithmVolumeLabelToSurfaceMapping::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeLabelToSurfaceMapping::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
