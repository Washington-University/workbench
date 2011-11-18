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

#include "AlgorithmVolumeToSurfaceMapping.h"
#include "AlgorithmException.h"
#include "VolumeFile.h"
#include "SurfaceFile.h"
#include "MetricFile.h"
#include "MathFunctions.h"
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeToSurfaceMapping::getCommandSwitch()
{
    return "-volume-to-surface-mapping";
}

AString AlgorithmVolumeToSurfaceMapping::getShortDescription()
{
    return "MAP VOLUME TO SURFACE";
}

OperationParameters* AlgorithmVolumeToSurfaceMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume", "the volume to map data from");
    ret->addSurfaceParameter(2, "surface", "the surface to map the data onto");
    ret->addMetricOutputParameter(3, "metric-out", "the output metric file");
    ret->createOptionalParameter(4, "-trilinear", "use trilinear volume interpolation");
    
    ret->createOptionalParameter(5, "-nearest", "use nearest neighbor volume interpolation");
    
    OptionalParameter* ribbonOpt = ret->createOptionalParameter(6, "-ribbon-constrained", "use ribbon constrained mapping algorithm");
    ribbonOpt->addMetricParameter(1, "thickness", "the metric file containing the ribbon thickness measure");
    ribbonOpt->addVolumeParameter(2, "ribbon", "the ribbon mask volume");
    ribbonOpt->addDoubleParameter(3, "ribbon-val", "the value to use from the ribbon mask");
    ribbonOpt->addDoubleParameter(4, "ribbon-dist-kernel", "the sigma for the gaussian kernel based on distance within connected ribbon voxels");
    ribbonOpt->createOptionalParameter(5, "-average-normals", "average each node's normals with its neighbors");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(7, "-subvol-select", "select a single subvolume to map");
    subvolumeSelect->addIntegerParameter(1, "subvol-num", "the index of the subvolume");
    
    ret->setHelpText(
        AString("You must specify exactly one mapping method.  The ribbon mapping method only takes data from within regions where ") +
        "the ribbon volume is equal to <ribbon-val> and weights them according to how far they are within the ribbon mask from " +
        "the surface normal of the node, extended outwards half the value of thickness.  This restricts the method from grabbing " +
        "data from a different part of the ribbon because the distance is calculated by only walking through connected voxels in the mask."
    );
    return ret;
}

void AlgorithmVolumeToSurfaceMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVolume = myParams->getVolume(1);
    SurfaceFile* mySurface = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    OptionalParameter* trilinearOpt = myParams->getOptionalParameter(4);
    OptionalParameter* nearestOpt = myParams->getOptionalParameter(5);
    OptionalParameter* ribbonOpt = myParams->getOptionalParameter(6);
    int64_t mySubVol = -1;
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(7);
    if (subvolumeSelect->m_present)
    {
        mySubVol = subvolumeSelect->getInteger(1);
        if (mySubVol < 0)//don't let them use the special value -1
        {
            throw AlgorithmException("invalid subvolume specified");
        }
    }
    bool haveMethod = false;
    bool averageNormals = false;
    Method myMethod;
    if (trilinearOpt->m_present)
    {
        haveMethod = true;
        myMethod = TRILINEAR;
    }
    if (nearestOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        haveMethod = true;
        myMethod = NEAREST_NEIGHBOR;
    }
    if (ribbonOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        haveMethod = true;
        myMethod = RIBBON_CONSTRAINED;
        if (ribbonOpt->getOptionalParameter(5)->m_present)
        {
            averageNormals = true;
        }
    }
    if (!haveMethod)
    {
        throw AlgorithmException("no mapping method specified");
    }
    switch (myMethod)
    {
        case TRILINEAR:
        case NEAREST_NEIGHBOR:
            AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, myMethod, mySubVol);//in case we want to separate constructors rather than just having defaults
            break;
        case RIBBON_CONSTRAINED:
            {
                MetricFile* thickness = ribbonOpt->getMetric(1);
                VolumeFile* ribbonVol = ribbonOpt->getVolume(2);
                float ribbonValue = (float)(ribbonOpt->getDouble(3));
                float kernel = (float)(ribbonOpt->getDouble(4));
                AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, myMethod, mySubVol, thickness, ribbonVol, ribbonValue, kernel, averageNormals);
            }
            break;
        default:
            throw AlgorithmException("this method not yet implemented");
    }
}

AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, VolumeFile* myVolume, SurfaceFile* mySurface, MetricFile* myMetricOut, Method myMethod, int64_t mySubVol, MetricFile* thickness, VolumeFile* ribbonVol, float ribbonValue, float kernel, bool averageNormals) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> myVolDims;
    myVolume->getDimensions(myVolDims);
    if (mySubVol >= myVolDims[3] || mySubVol < -1)
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    int64_t numColumns;
    if (mySubVol == -1)
    {
        numColumns = myVolDims[4];
    } else {
        numColumns = myVolDims[3] * myVolDims[4];
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myMetricOut->setStructure(mySurface->getStructure());
    switch (myMethod)
    {
        case TRILINEAR:
            {
                CaretArray<float> myCaretArray(new float[numNodes]);
                float* myArray = myCaretArray.getArray();
                if (mySubVol == -1)
                {
                    for (int64_t i = 0; i < myVolDims[3]; ++i)
                    {
                        for (int64_t j = 0; j < myVolDims[4]; ++j)
                        {
                            AString metricLabel = myVolume->getMapName(i);
                            if (myVolDims[4] != 1)
                            {
                                metricLabel += " component " + AString::number(j);
                            }
                            metricLabel += " trilinear";
                            int64_t thisCol = i * myVolDims[4] + j;
                            myMetricOut->setColumnName(thisCol, metricLabel);
                            for (int64_t node = 0; node < numNodes; ++node)
                            {
                                myArray[node] = myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::TRILINEAR, NULL, i, j);
                            }
                            myMetricOut->setValuesForColumn(thisCol, myArray);
                        }
                    }
                } else {
                    for (int64_t j = 0; j < myVolDims[4]; ++j)
                    {
                        AString metricLabel = myVolume->getMapName(mySubVol);
                        if (myVolDims[4] != 1)
                        {
                            metricLabel += " component " + AString::number(j);
                        }
                        metricLabel += " trilinear";
                        int64_t thisCol = j;
                        myMetricOut->setColumnName(thisCol, metricLabel);
                        for (int64_t node = 0; node < numNodes; ++node)
                        {
                            myArray[node] = myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::TRILINEAR, NULL, mySubVol, j);
                        }
                        myMetricOut->setValuesForColumn(thisCol, myArray);
                    }
                }
            }
            break;
        case NEAREST_NEIGHBOR:
            {
                CaretArray<float> myCaretArray(new float[numNodes]);
                float* myArray = myCaretArray.getArray();
                if (mySubVol == -1)
                {
                    for (int64_t i = 0; i < myVolDims[3]; ++i)
                    {
                        for (int64_t j = 0; j < myVolDims[4]; ++j)
                        {
                            AString metricLabel = myVolume->getMapName(i);
                            if (myVolDims[4] != 1)
                            {
                                metricLabel += " component " + AString::number(j);
                            }
                            metricLabel += " nearest neighbor";
                            int64_t thisCol = i * myVolDims[4] + j;
                            myMetricOut->setColumnName(thisCol, metricLabel);
                            for (int64_t node = 0; node < numNodes; ++node)
                            {
                                myArray[node] = myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::NEAREST_NEIGHBOR, NULL, i, j);
                            }
                            myMetricOut->setValuesForColumn(thisCol, myArray);
                        }
                    }
                } else {
                    for (int64_t j = 0; j < myVolDims[4]; ++j)
                    {
                        AString metricLabel = myVolume->getMapName(mySubVol);
                        if (myVolDims[4] != 1)
                        {
                            metricLabel += " component " + AString::number(j);
                        }
                        metricLabel += " nearest neighbor";
                        int64_t thisCol = j;
                        myMetricOut->setColumnName(thisCol, metricLabel);
                        for (int64_t node = 0; node < numNodes; ++node)
                        {
                            myArray[node] = myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::NEAREST_NEIGHBOR, NULL, mySubVol, j);
                        }
                        myMetricOut->setValuesForColumn(thisCol, myArray);
                    }
                }
            }
            break;
        case RIBBON_CONSTRAINED:
            {
                if (thickness == NULL || ribbonVol == NULL)
                {
                    throw AlgorithmException("missing required parameter for this mapping method");
                }
                if (mySurface->getNumberOfNodes() != thickness->getNumberOfNodes())
                {
                    throw AlgorithmException("thickness metric has the wrong number of nodes");
                }
                if (myVolume->compareVolumeSpace(ribbonVol) == false)
                {
                    throw AlgorithmException("volume spaces must match");
                }
                mySurface->computeNormals(averageNormals);
                vector<vector<VoxelWeight> > myWeights;
                precomputeWeights(myWeights, ribbonVol, thickness, mySurface, ribbonValue, kernel);
                CaretArray<float> myScratchArray(new float[numNodes]);
                float* myScratch = myScratchArray.getArray();
                if (mySubVol == -1)
                {
                    for (int64_t i = 0; i < myVolDims[3]; ++i)
                    {
                        for (int64_t j = 0; j < myVolDims[4]; ++j)
                        {
                            AString metricLabel = myVolume->getMapName(i);
                            if (myVolDims[4] != 1)
                            {
                                metricLabel += " component " + AString::number(j);
                            }
                            metricLabel += " ribbon constrained";
                            int64_t thisCol = i * myVolDims[4] + j;
                            myMetricOut->setColumnName(thisCol, metricLabel);
                            for (int64_t node = 0; node < numNodes; ++node)
                            {
                                myScratch[node] = 0.0f;
                                float totalWeight = 0.0f;
                                int numVoxels = (int)myWeights[node].size();
                                for (int voxel = 0; voxel < numVoxels; ++voxel)
                                {
                                    float thisWeight = myWeights[node][voxel].weight;
                                    totalWeight += thisWeight;
                                    myScratch[node] += myWeights[node][voxel].weight * myVolume->getValue(myWeights[node][voxel].ijk, i, j);
                                }
                                if (totalWeight > 0.0f)
                                {
                                    myScratch[node] /= totalWeight;
                                }//should already be zero due to initialization above
                            }
                            myMetricOut->setValuesForColumn(thisCol, myScratch);
                        }
                    }
                } else {
                    for (int64_t j = 0; j < myVolDims[4]; ++j)
                    {
                        AString metricLabel = myVolume->getMapName(mySubVol);
                        if (myVolDims[4] != 1)
                        {
                            metricLabel += " component " + AString::number(j);
                        }
                        metricLabel += " ribbon constrained";
                        int64_t thisCol = j;
                        myMetricOut->setColumnName(thisCol, metricLabel);
                        for (int64_t node = 0; node < numNodes; ++node)
                        {
                                myScratch[node] = 0.0f;
                                float totalWeight = 0.0f;
                                int numVoxels = (int)myWeights[node].size();
                                for (int voxel = 0; voxel < numVoxels; ++voxel)
                                {
                                    float thisWeight = myWeights[node][voxel].weight;
                                    totalWeight += thisWeight;
                                    myScratch[node] += myWeights[node][voxel].weight * myVolume->getValue(myWeights[node][voxel].ijk, mySubVol, j);
                                }
                                myScratch[node] /= totalWeight;
                        }
                        myMetricOut->setValuesForColumn(thisCol, myScratch);
                    }
                }
            }
            break;
        default:
            throw AlgorithmException("this method not yet implemented");
    }
}

void AlgorithmVolumeToSurfaceMapping::precomputeWeights(vector<vector<VoxelWeight> >& myWeights, VolumeFile* ribbonVol, MetricFile* thickness, SurfaceFile* mySurface, float ribbonValue, float kernel)
{
    int numNodes = mySurface->getNumberOfNodes();
    myWeights.resize(numNodes);
    vector<int64_t> myDims;
    ribbonVol->getDimensions(myDims);
    myDims.resize(4);
    myDims[3] = 1;
    VolumeFile tempMask;//so that we don't have to test equal to value with tolerance everywhere
    vector<vector<float> > myVolSpace = ribbonVol->getVolumeSpace();
    tempMask.reinitialize(myDims, myVolSpace);
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    const float* ribbonFrame = ribbonVol->getFrame(0, 0);
    float* tempFrame = new float[frameSize];
    const float TOLERANCE = 1.000001f;//need 1,000,000 labels before it will grab adjacent labels
    for (int64_t i = 0; i < frameSize; ++i)
    {
        if (ribbonFrame[i] == ribbonValue)
        {
            tempFrame[i] = 1.0f;
        } else {
            if (ribbonFrame[i] != 0.0f && ribbonValue != 0.0f && ribbonFrame[i] / ribbonValue < TOLERANCE && ribbonValue / ribbonFrame[i] < TOLERANCE)
            {
                tempFrame[i] = 1.0f;
            } else {
                tempFrame[i] = 0.0f;
            }
        }
    }
    tempMask.setFrame(tempFrame);
    delete[] tempFrame;
    float ihat[3], jhat[3], khat[3], tempvec[3];
    ihat[0] = myVolSpace[0][0]; ihat[1] = myVolSpace[1][0]; ihat[2] = myVolSpace[2][0];
    jhat[0] = myVolSpace[0][1]; jhat[1] = myVolSpace[1][1]; jhat[2] = myVolSpace[2][1];
    khat[0] = myVolSpace[0][2]; khat[1] = myVolSpace[1][2]; khat[2] = myVolSpace[2][2];
    float voxNeighDist[3][3][3];//precompute to save a bunch of flops in dijkstras
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                tempvec[0] = ihat[0] * (i - 1);
                tempvec[1] = ihat[1] * (i - 1);
                tempvec[2] = ihat[2] * (i - 1);
                tempvec[0] += jhat[0] * (j - 1);
                tempvec[1] += jhat[1] * (j - 1);
                tempvec[2] += jhat[2] * (j - 1);
                tempvec[0] += khat[0] * (k - 1);
                tempvec[1] += khat[1] * (k - 1);
                tempvec[2] += khat[2] * (k - 1);
                voxNeighDist[i][j][k] = MathFunctions::vectorLength(tempvec);
            }
        }
    }
    const float* myCoordRef = mySurface->getCoordinateData();
    const float* myNormalRef = mySurface->getNormalData();
    const float* myThickRef = thickness->getValuePointerForColumn(0);
    int* marked = new int[frameSize];//temp storage for dijkstra
    int* changed = new int[frameSize];
    float* distances = new float[frameSize];
    VoxelDistMinHeap myHeap;
    vector<VoxelDist> myVoxels;
    for (int64_t i = 0; i < frameSize; ++i)
    {
        marked[i] = 0;
    }
    for (int i = 0; i < numNodes; ++i)
    {
        myHeap.clear();
        int i3 = i * 3;
        float curCoord[3];
        curCoord[0] = myCoordRef[i3] - myNormalRef[i3] * myThickRef[i] * 0.5f;//the normal is guaranteed to be a unit vector, thickness units and coord units must match
        curCoord[1] = myCoordRef[i3 + 1] - myNormalRef[i3 + 1] * myThickRef[i] * 0.5f;
        curCoord[2] = myCoordRef[i3 + 2] - myNormalRef[i3 + 2] * myThickRef[i] * 0.5f;
        float curDist = 0.0f;
        float endDist = myThickRef[i];
        float crossingDists[3];
        float curIndex[3];
        int64_t curVoxel[3];
        tempMask.spaceToIndex(curCoord, curIndex);
        tempMask.closestVoxel(curCoord, curVoxel);
        float ijksteps[3];
        ijksteps[0] = 1.0f / MathFunctions::dotProduct(ihat, myNormalRef + i3);
        ijksteps[1] = 1.0f / MathFunctions::dotProduct(jhat, myNormalRef + i3);
        ijksteps[2] = 1.0f / MathFunctions::dotProduct(khat, myNormalRef + i3);
        while (curDist < endDist)
        {
            if (tempMask.indexValid(curVoxel) && tempMask.getValue(curVoxel) > 0.5f)
            {//obey the mask even if the voxel is on the normal within thickness
                float tempCoord[3];
                tempMask.indexToSpace(curVoxel, tempCoord);
                float diffVec[3];
                MathFunctions::subtractVectors(myCoordRef + i3, tempCoord, diffVec);
                float normAmount = MathFunctions::dotProduct(myNormalRef + i3, diffVec);
                diffVec[0] -= myNormalRef[i3] * normAmount;
                diffVec[1] -= myNormalRef[i3 + 1] * normAmount;
                diffVec[2] -= myNormalRef[i3 + 2] * normAmount;
                float distFromNorm = MathFunctions::vectorLength(diffVec);
                myHeap.push(VoxelDist(distFromNorm, curVoxel));
            }//compute positive fractions of the normal required to change the index of the closest voxel in each direction
            int direction = -1;
            float tempf = -1.0f;
            for (int j = 0; j < 3; ++j)
            {
                if (ijksteps[j] > 0.0f)
                {
                    crossingDists[j] = (0.5f - (curIndex[j] - curVoxel[j])) * ijksteps[j];
                } else {
                    crossingDists[j] = ((curVoxel[j] - curIndex[j]) - 0.5f) * ijksteps[j];//because step is negative, reverse the subtraction
                }//find smallest of these, and use it
                if (crossingDists[j] < tempf || (direction == -1 && crossingDists[j] == crossingDists[j]))
                {//keep NaNs out, +inf will take care of itself, -inf shouldn't happen
                    direction = j;
                    tempf = crossingDists[j];
                }
            }
            if (direction == -1 || !(tempf == tempf) || (tempf != 0.0f && tempf * 2.0f == tempf))
            {//panic!
                throw AlgorithmException("volume spacing produced an error initializing shortest path sources");
            }
            curDist += tempf;
            for (int j = 0; j < 3; ++j)
            {
                curCoord[j] += tempf * myNormalRef[i3 + j];
            }
            tempMask.spaceToIndex(curCoord, curIndex);
            if (ijksteps[direction] > 0.0f)
            {
                ++curVoxel[direction];
            } else {
                --curVoxel[direction];
            }
        }
        dijkstra(&tempMask, myHeap, voxNeighDist, marked, changed, distances, myVoxels, kernel * 3.0f);//triple the kernel, we don't need more accuracy
        int myEnd = (int)myVoxels.size();
        myWeights[i].resize(myEnd);
        float gaussDenom = -0.5f / (kernel * kernel);
        for (int j = 0; j < myEnd; ++j)
        {
            myWeights[i][j].ijk[0] = myVoxels[j].ijk[0];
            myWeights[i][j].ijk[1] = myVoxels[j].ijk[1];
            myWeights[i][j].ijk[2] = myVoxels[j].ijk[2];
            myWeights[i][j].weight = exp(myVoxels[j].dist * myVoxels[j].dist * gaussDenom);
        }
    }
}

void AlgorithmVolumeToSurfaceMapping::dijkstra(VolumeFile* mask, VoxelDistMinHeap& myHeap, float voxNeighDist[3][3][3], int* marked, int* changed, float* distances, vector<VoxelDist>& myVoxelsOut, float maxDist)
{
    int numChanged = 0;
    myVoxelsOut.clear();
    const float* maskFrame = mask->getFrame();
    float tempf;
    while (!myHeap.isEmpty())
    {
        VoxelDist thisVoxel = myHeap.pop();
        int64_t thisIndex = mask->getIndex(thisVoxel.ijk);
        if (!(marked[thisIndex] & 1))//already visited, happens when same voxel has multiple paths to it (which is nearly always)
        {
            myVoxelsOut.push_back(thisVoxel);
            if (marked[thisIndex] == 0) changed[numChanged++] = thisIndex;//happens because initialization code in precompute doesn't mark the starting set or use the changed array
            marked[thisIndex] |= 1;//mark as visited
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    for (int k = 0; k < 3; ++k)
                    {
                        int64_t neighijk[3];
                        neighijk[0] = thisVoxel.ijk[0] + i - 1;
                        neighijk[1] = thisVoxel.ijk[1] + j - 1;
                        neighijk[2] = thisVoxel.ijk[2] + k - 1;
                        if (mask->indexValid(neighijk))
                        {
                            int64_t neighIndex = mask->getIndex(neighijk);
                            if (!(marked[neighIndex] & 1) && maskFrame[neighIndex] > 0.5f)
                            {//also stops the middle "neighbor", only happens 1/27 of the time so explicit tests for it would probably be more overhead
                                tempf = thisVoxel.dist + voxNeighDist[i][j][k];
                                if (tempf <= maxDist)
                                {
                                    if (marked[neighIndex] & 2)//denotes has a value in the distances array
                                    {
                                        if (distances[neighIndex] > tempf)
                                        {
                                            distances[neighIndex] = tempf;
                                            myHeap.push(VoxelDist(tempf, thisVoxel.ijk[0] + i - 1, thisVoxel.ijk[1] + j - 1, thisVoxel.ijk[2] + k - 1));
                                        }
                                    } else {
                                        changed[numChanged++] = neighIndex;//setting a valid value where none was before is always a new change
                                        marked[neighIndex] |= 2;
                                        distances[neighIndex] = tempf;
                                        myHeap.push(VoxelDist(tempf, thisVoxel.ijk[0] + i - 1, thisVoxel.ijk[1] + j - 1, thisVoxel.ijk[2] + k - 1));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < numChanged; ++i)
    {//reset the marked array
        marked[changed[i]] = 0;
    }
}

float AlgorithmVolumeToSurfaceMapping::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeToSurfaceMapping::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
