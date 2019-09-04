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

#include "AlgorithmVolumeRemoveIslands.h"
#include "AlgorithmException.h"

#include "VolumeFile.h"

#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmVolumeRemoveIslands::getCommandSwitch()
{
    return "-volume-remove-islands";
}

AString AlgorithmVolumeRemoveIslands::getShortDescription()
{
    return "REMOVE ISLANDS FROM AN ROI VOLUME";
}

OperationParameters* AlgorithmVolumeRemoveIslands::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input ROI volume");
    
    ret->addVolumeOutputParameter(2, "volume-out", "the output ROI volume");
    
    ret->setHelpText(
        AString("Finds all face-connected parts of the ROI, and zeros out all but the largest one.")
    );
    return ret;
}

void AlgorithmVolumeRemoveIslands::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVolIn = myParams->getVolume(1);
    VolumeFile* myVolOut = myParams->getOutputVolume(2);
    AlgorithmVolumeRemoveIslands(myProgObj, myVolIn, myVolOut);
}

AlgorithmVolumeRemoveIslands::AlgorithmVolumeRemoveIslands(ProgressObject* myProgObj, const VolumeFile* myVolIn, VolumeFile* myVolOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const int STENCIL_SIZE = 18;//the easy way, and prepare for different stencils if we ever need them
    const int stencil[STENCIL_SIZE] = { 0, 0, -1,
                                  0, -1, 0,
                                  -1, 0, 0,
                                  1, 0, 0,
                                  0, 1, 0,
                                  0, 0, 1 };
    vector<int64_t> dims;
    myVolIn->getDimensions(dims);
    myVolOut->reinitialize(myVolIn->getOriginalDimensions(), myVolIn->getSform(), myVolIn->getNumberOfComponents(), myVolIn->getType(), myVolIn->m_header);
    for (int s = 0; s < dims[3]; ++s)
    {
        myVolOut->setMapName(s, myVolIn->getMapName(s));
        for (int c = 0; c < dims[4]; ++c)
        {
            int64_t ijk[3];
            vector<vector<int64_t> > parts;
            vector<int> used(dims[0] * dims[1] * dims[2], 0);
            const float* frame = myVolIn->getFrame(s, c);
            for (ijk[2] = 0; ijk[2] < dims[2]; ++ijk[2])
            {
                for (ijk[1] = 0; ijk[1] < dims[1]; ++ijk[1])
                {
                    for (ijk[0] = 0; ijk[0] < dims[0]; ++ijk[0])
                    {
                        int64_t index = myVolIn->getIndex(ijk);
                        if (used[index] == 0 && frame[index] > 0.0f)
                        {
                            parts.push_back(vector<int64_t>());
                            vector<int64_t>& thispart = parts.back();
                            thispart.push_back(ijk[0]);
                            thispart.push_back(ijk[1]);
                            thispart.push_back(ijk[2]);
                            used[index] = 1;
                            vector<int64_t> mystack;
                            mystack.push_back(ijk[0]);
                            mystack.push_back(ijk[1]);
                            mystack.push_back(ijk[2]);
                            while (!mystack.empty())
                            {
                                int64_t curSize = (int64_t)mystack.size();
                                int64_t curvox[3] = { mystack[curSize - 3], mystack[curSize - 2], mystack[curSize - 1] };
                                mystack.resize(curSize - 3);//basically pop_back
                                for (int i = 0; i < STENCIL_SIZE; i += 3)
                                {
                                    int64_t neighbor[3] = { curvox[0] + stencil[i], curvox[1] + stencil[i + 1], curvox[2] + stencil[i + 2] };
                                    if (myVolIn->indexValid(neighbor))
                                    {
                                        int64_t neighindex = myVolIn->getIndex(neighbor);
                                        if (used[neighindex] == 0 && frame[neighindex] > 0.0f)
                                        {
                                            thispart.push_back(neighbor[0]);
                                            thispart.push_back(neighbor[1]);
                                            thispart.push_back(neighbor[2]);
                                            used[neighindex] = 1;
                                            mystack.push_back(neighbor[0]);
                                            mystack.push_back(neighbor[1]);
                                            mystack.push_back(neighbor[2]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            int64_t bestCount = -1, bestPart = -1, numParts = (int64_t)parts.size();
            for (int64_t i = 0; i < numParts; ++i)
            {
                int64_t thisCount = (int64_t)parts[i].size();
                if (thisCount > bestCount)
                {
                    bestCount = thisCount;
                    bestPart = i;
                }
            }
            vector<float> outFrame(dims[0] * dims[1] * dims[2], 0.0f);
            if (bestPart != -1)
            {
                vector<int64_t>& myPart = parts[bestPart];
                for (int64_t i = 0; i < bestCount; i += 3)
                {
                    int64_t myIndex = myVolIn->getIndex(myPart.data() + i);
                    outFrame[myIndex] = 1.0f;//make it a simple 0/1 volume, even if it wasn't before
                }
            }
            myVolOut->setFrame(outFrame.data(), s, c);
        }
    }
}

float AlgorithmVolumeRemoveIslands::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeRemoveIslands::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
