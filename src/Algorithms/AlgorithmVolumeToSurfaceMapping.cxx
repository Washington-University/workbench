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
#include "FloatMatrix.h"

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
    //ribbonOpt->addMetricParameter(1, "thickness", "the metric file containing the ribbon thickness measure");
    //ribbonOpt->addVolumeParameter(2, "ribbon", "the ribbon mask volume");
    //ribbonOpt->addDoubleParameter(3, "ribbon-val", "the value to use from the ribbon mask");
    //ribbonOpt->addDoubleParameter(4, "ribbon-dist-kernel", "the sigma for the gaussian kernel based on distance within connected ribbon voxels");
    //ribbonOpt->createOptionalParameter(5, "-average-normals", "average each node's normals with its neighbors");
    ribbonOpt->addSurfaceParameter(1, "outer-surf", "the outer surface of the ribbon");
    ribbonOpt->addSurfaceParameter(2, "inner-surf", "the inner surface of the ribbon");
    OptionalParameter* ribbonSubdiv = ribbonOpt->createOptionalParameter(3, "-voxel-subdiv", "number of subdivisions of each voxel edge when estimating partial voluming");
    ribbonSubdiv->addIntegerParameter(1, "subdiv-num", "number of subdivisions, default 3");
    
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
        if (ribbonOpt->m_present)
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
                //MetricFile* thickness = ribbonOpt->getMetric(1);
                //VolumeFile* ribbonVol = ribbonOpt->getVolume(2);
                //float ribbonValue = (float)(ribbonOpt->getDouble(3));
                //float kernel = (float)(ribbonOpt->getDouble(4));
                //AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, myMethod, mySubVol, thickness, ribbonVol, ribbonValue, kernel, averageNormals);
                SurfaceFile* outerSurf = ribbonOpt->getSurface(1);
                SurfaceFile* innerSurf = ribbonOpt->getSurface(2);
                int32_t subdivisions = 3;
                OptionalParameter* ribbonSubdiv = ribbonOpt->getOptionalParameter(3);
                if (ribbonSubdiv->m_present)
                {
                    subdivisions = ribbonSubdiv->getInteger(1);
                }
                AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, myMethod, mySubVol, innerSurf, outerSurf, subdivisions);
            }
            break;
        default:
            throw AlgorithmException("this method not yet implemented");
    }
}

AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, VolumeFile* myVolume, SurfaceFile* mySurface, MetricFile* myMetricOut, Method myMethod, int64_t mySubVol, SurfaceFile* innerSurf, SurfaceFile* outerSurf, int32_t subdivisions) : AbstractAlgorithm(myProgObj)
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
                if (outerSurf == NULL || innerSurf == NULL)
                {
                    throw AlgorithmException("missing required parameter for this mapping method");
                }
                if (mySurface->getNumberOfNodes() != outerSurf->getNumberOfNodes() || mySurface->getNumberOfNodes() != innerSurf->getNumberOfNodes())
                {//TODO: also compare topologies?
                    throw AlgorithmException("all surfaces must be in node correspondence");
                }
                vector<vector<VoxelWeight> > myWeights;
                precomputeWeights(myWeights, myVolume, innerSurf, outerSurf, subdivisions);
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
                                    myScratch[node] += thisWeight * myVolume->getValue(myWeights[node][voxel].ijk, i, j);
                                }
                                if (totalWeight != 0.0f)
                                {
                                    myScratch[node] /= totalWeight;
                                } else {
                                    myScratch[node] = 0.0f;
                                }
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
                                    myScratch[node] += thisWeight * myVolume->getValue(myWeights[node][voxel].ijk, mySubVol, j);
                                }
                                if (totalWeight != 0.0f)
                                {
                                    myScratch[node] /= totalWeight;
                                } else {
                                    myScratch[node] = 0.0f;
                                }
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

void AlgorithmVolumeToSurfaceMapping::precomputeWeights(vector<vector<VoxelWeight> >& myWeights, VolumeFile* myVolume, SurfaceFile* innerSurf, SurfaceFile* outerSurf, int numDivisions)
{
    int64_t numNodes = outerSurf->getNumberOfNodes();
    myWeights.resize(numNodes);
    const vector<vector<float> >& myVolSpace = myVolume->getVolumeSpace();
    Vector3d origin, ivec, jvec, kvec;//these are the spatial projections of the ijk unit vectors (also, the offset that specifies the origin)
    ivec[0] = myVolSpace[0][0]; jvec[0] = myVolSpace[0][1]; kvec[0] = myVolSpace[0][2]; origin[0] = myVolSpace[0][3];
    ivec[1] = myVolSpace[1][0]; jvec[0] = myVolSpace[1][1]; kvec[0] = myVolSpace[1][2]; origin[0] = myVolSpace[1][3];
    ivec[2] = myVolSpace[2][0]; jvec[0] = myVolSpace[2][1]; kvec[0] = myVolSpace[2][2]; origin[0] = myVolSpace[2][3];
    CaretPointer<TopologyHelper> myTopoHelp = innerSurf->getTopologyHelper();
    vector<int> myNeighList, myTileList;
    //const float* mySurfCoords = mySurface->getCoordinateData();
    const float* outerCoords = outerSurf->getCoordinateData();
    const float* innerCoords = innerSurf->getCoordinateData();
    int maxVoxelCount = 10;//guess for preallocating vectors
    for (int64_t node = 0; node < numNodes; ++node)
    {
        myWeights[node].clear();
        myWeights[node].reserve(maxVoxelCount);
        float tempf;
        int64_t node3 = node * 3;
        PolyInfo myPoly(innerSurf, outerSurf, node);//build the polygon
        Vector3d minIndex, maxIndex, tempvec;
        myVolume->spaceToIndex(innerCoords + node3, minIndex.m_vec);//find the bounding box in VOLUME INDEX SPACE, starting with the center nodes
        maxIndex = minIndex;
        myVolume->spaceToIndex(outerCoords + node3, tempvec.m_vec);
        for (int i = 0; i < 3; ++i)
        {
            if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
            if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
        }
        myTopoHelp->getNodeNeighbors(node, myNeighList);//and now the neighbors
        int numNeigh = (int)myNeighList.size();
        for (int j = 0; j < numNeigh; ++j)
        {
            int neigh3 = myNeighList[j] * 3;
            myVolume->spaceToIndex(outerCoords + neigh3, tempvec.m_vec);
            for (int i = 0; i < 3; ++i)
            {
                if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
            }
            myVolume->spaceToIndex(innerCoords + neigh3, tempvec.m_vec);
            for (int i = 0; i < 3; ++i)
            {
                if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
            }
        }
        vector<int64_t> myDims;
        myVolume->getDimensions(myDims);
        int startIndex[3], endIndex[3];
        for (int i = 0; i < 3; ++i)
        {
            startIndex[i] = (int)ceil(minIndex[i] - 0.5f);//give an extra half voxel in order to get anything which could have some polygon in it
            endIndex[i] = (int)floor(maxIndex[i] + 0.5f) + 1;//ditto, plus the one-after end convention
            if (startIndex[i] < 0) startIndex[i] = 0;//keep it inside the volume boundaries
            if (endIndex[i] > myDims[i]) endIndex[i] = myDims[i];
        }
        int64_t ijk[3];
        for (ijk[0] = startIndex[0]; ijk[0] < endIndex[0]; ++ijk[0])
        {
            for (ijk[1] = startIndex[1]; ijk[1] < endIndex[1]; ++ijk[1])
            {
                for (ijk[2] = startIndex[2]; ijk[2] < endIndex[2]; ++ijk[2])
                {
                    tempf = computeVoxelFraction(myVolume, ijk, myPoly, numDivisions, ivec, jvec, kvec);
                    if (tempf != 0.0f)
                    {
                        myWeights[node].push_back(VoxelWeight(tempf, ijk));
                    }
                }
            }
        }
        /*if (node == 100634)//debug - get the "kernel" for any single node
        {
            vector<int64_t> myDims;
            myVolume->getDimensions(myDims);
            myDims.resize(3);
            VolumeFile debugVol(myDims, myVolSpace);
            debugVol.setValueAllVoxels(0.0f);
            for (int i = 0; i < (int)myWeights[node].size(); ++i)
            {
                debugVol.setValue(myWeights[node][i].weight, myWeights[node][i].ijk);
            }
            debugVol.writeFile("debug.nii");
        }//*/
        if ((int)myWeights[node].size() > maxVoxelCount)
        {//capacity() might result in a faster stabilization, but would use more memory
            maxVoxelCount = myWeights[node].size();
        }
        //sphere based code for finding all voxels that could be within range, UNTESTED, wrote it before I realized a box based version can work without orthogonal axes, and might be more efficient
        /*float maxDist = MathFunctions::distance3D(mySurfCoords + node3, outerCoords + node3);//find the maximum sphere we need to test voxel centers inside of
        float tempf = MathFunctions::distance3D(mySurfCoords + node3, innerCoords + node3);//start with the corresponding nodes
        if (tempf > maxDist)
        {
            maxDist = tempf;
        }
        int numNeigh = (int)myNeighList.size();
        for (int i = 0; i < numNeigh; ++i)
        {
            int myNeigh = myNeighList[i];
            int myNeigh3 = myNeigh * 3;
            tempf = MathFunctions::distance3D(mySurfCoords + node3, outerCoords + myNeigh3);//search neighbors
            if (tempf > maxDist)
            {
                maxDist = tempf;
            }
            tempf = MathFunctions::distance3D(mySurfCoords + node3, innerCoords + myNeigh3);
            if (tempf > maxDist)
            {
                maxDist = tempf;
            }
        }//now, add half the length of the longest voxel diagonal
        Vector3d tempvec, tempvec2;
        float tempf2;
        tempvec = ivec + jvec;//the brute force way, because there are only 4 cases that need to be tested
        tempvec2 = tempvec + kvec;
        tempf = tempvec2.length();
        tempvec2 = tempvec - kvec;
        tempf2 = tempvec2.length();
        if (tempf2 > tempf)
        {
            tempf = tempf2;
        }
        tempvec = ivec - jvec;
        tempvec2 = tempvec + kvec;
        tempf2 = tempvec2.length();
        if (tempf2 > tempf)
        {
            tempf = tempf2;
        }
        tempvec2 = tempvec - kvec;
        tempf2 = tempvec2.length();
        if (tempf2 > tempf)
        {
            tempf = tempf2;
        }
        maxDist += tempf * 0.5f;
        Vector3d khat = kvec.normal(), iorth, jorth, iorthhat, jorthhat;//generate an orthogonal basis that will let us find the voxels inside a sphere
        if (myVolume->isPlumb())
        {
            iorth = ivec;
            jorth = jvec;
            iorthhat = iorth.normal();
            jorthhat = jorth.normal();
        } else {
            jorth = jvec - jvec.dot(khat) * khat;
            jorthhat = jorth.normal();
            iorth = ivec - ivec.dot(khat) * khat;
            iorth -= iorth.dot(jorthhat) * jorthhat;//due to jorthhat being orthogonal to khat, we could probably combine these into one step
            iorthhat = iorth.normal();//k will use the given vector, since we need it to not disturb other indexes
        }
        tempvec = iorthhat + jorthhat + khat;//sanity check
        tempf = tempvec.length();
        if (tempf != tempf || (abs(tempf) > 0.0f && tempf * 2.0f == tempf))
        {
            throw AlgorithmException("volume spacing is singular");
        }
        Vector3d myCenter = mySurfCoords + node3;//this is overloaded to accept assignment/construction from a plain float triplet
        vector<int64_t> myDims;
        myVolume->getDimensions(myDims);
        tempvec = myCenter - maxDist * iorthhat;//this will give us our lowest first "index" in our orthogonal space
        myVolume->spaceToIndex(tempvec.m_vec, tempvec2.m_vec);//which was crafted to match the i index, but letting j and k wander (since we subtracted some of each of their normals)
        int64_t istart = (int64_t)ceil(tempvec2[0]);
        if (istart < 0) istart = 0;
        tempvec = myCenter + maxDist * iorthhat;//and our highest first "index"
        myVolume->spaceToIndex(tempvec.m_vec, tempvec2.m_vec);
        int64_t iend = (int64_t)floor(tempvec2[0]) + 1;//use the 1 after idiom
        if (iend > myDims[0]) iend = myDims[0];
        int64_t ijk[3];
        float maxDistSquare = maxDist * maxDist;
        Vector3d tempvec3, tempvec4;//need even more temporaries for volume space interaction
        for (ijk[0] = istart; ijk[0] < iend; ++ijk[0])
        {//now, the j orthogonal basis is crafted to keep i constant, but k can wander (since we only subtracted some of the k normal, and moving along the k normal changes only k)
            tempf = ijk[0] * iorth.length();
            float remDistSquarei = maxDistSquare - tempf * tempf;
            if (remDistSquarei < 0.0f) continue;//because floats aren't exact
            Vector3d myCenteri = myCenter + iorth * ijk[0];
            tempvec2 = sqrt(remDistSquarei) * jorthhat;//avoid repeat calculation
            tempvec3 = myCenteri - tempvec2;//get our minimum j
            myVolume->spaceToIndex(tempvec3.m_vec, tempvec4.m_vec);
            int64_t jstart = (int64_t)ceil(tempvec4[1]);
            if (jstart < 0) jstart = 0;
            tempvec3 = myCenteri + tempvec2;//get our maximum j
            myVolume->spaceToIndex(tempvec3.m_vec, tempvec4.m_vec);
            int64_t jend = (int64_t)floor(tempvec4[1]) + 1;
            if (jend > myDims[1]) jend = myDims[1];
            for (ijk[1] = jstart; ijk[1] < jend; ++ijk[1])
            {
                tempf = ijk[1] * jorth.length();
                float remDistSquarej = remDistSquarei - tempf * tempf;
                if (remDistSquarej < 0.0f) continue;
                Vector3d myCenterj = myCenteri + jorth * ijk[1];
                tempvec2 = sqrt(remDistSquarej) * khat;
                tempvec3 = myCenterj - tempvec2;//minimum k
                myVolume->spaceToIndex(tempvec3.m_vec, tempvec4.m_vec);
                int64_t kstart = (int64_t)ceil(tempvec4.m_vec[2]);//same as tempvec4[2], but without the function call, using it here because it is more of an inner loop
                if (kstart < 0) kstart = 0;
                tempvec3 = myCenterj + tempvec2;//maximum j
                myVolume->spaceToIndex(tempvec3.m_vec, tempvec4.m_vec);
                int64_t kend = (int64_t)floor(tempvec4.m_vec[2]) + 1;
                if (kend > myDims[2]) kend = myDims[2];
                for (ijk[2] = kstart; ijk[2] < kend; ++ijk[2])
                {//compute partial volume of this voxel inside the polygon - estimation by point tests
                    tempf = computeVoxelFraction(myVolume, ijk, myPoly, numDivisions, ivec, jvec, kvec);
                    myWeights[node].push_back(VoxelWeight(tempf, ijk));
                }
            }
        }//*/
    }
}

float AlgorithmVolumeToSurfaceMapping::computeVoxelFraction(const caret::VolumeFile* myVolume, const int64_t* ijk, PolyInfo& myPoly, const int divisions, const Vector3d& ivec, const Vector3d& jvec, const Vector3d& kvec)
{
    Vector3d myLowCorner;
    myVolume->indexToSpace(ijk[0] - 0.5f, ijk[1] - 0.5f, ijk[2] - 0.5f, myLowCorner.m_vec);
    int inside = 0;
    Vector3d istep = ivec / divisions;
    Vector3d jstep = jvec / divisions;
    Vector3d kstep = kvec / divisions;
    myLowCorner += istep * 0.5f + jstep * 0.5f + kstep * 0.5f;
    for (int i = 0; i < divisions; ++i)
    {
        Vector3d tempVeci = myLowCorner + istep * i;
        for (int j = 0; j < divisions; ++j)
        {
            Vector3d tempVecj = tempVeci + jstep * j;
            for (int k = 0; k < divisions; ++k)
            {
                Vector3d thisPoint = tempVecj + kstep * k;
                inside += myPoly.isInside(thisPoint.m_vec);
            }
        }
    }
    return ((float)inside) / (divisions * divisions * divisions * 2);
}

void PolyInfo::addTri(const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int32_t root, const int32_t node2, const int32_t node3)
{
    TriInfo myTriInfo;
    myTriInfo.initialize(innerSurf->getCoordinate(root), innerSurf->getCoordinate(node2), innerSurf->getCoordinate(node3));
    m_tris.push_back(myTriInfo);
    myTriInfo.initialize(outerSurf->getCoordinate(root), outerSurf->getCoordinate(node2), outerSurf->getCoordinate(node3));
    m_tris.push_back(myTriInfo);
    QuadInfo myQuadInfo;
    myQuadInfo.initialize(innerSurf->getCoordinate(node2), innerSurf->getCoordinate(node3), outerSurf->getCoordinate(node3), outerSurf->getCoordinate(node2));
    m_quads.push_back(myQuadInfo);
}

int PolyInfo::isInside(const float* xyz)
{
    int i, temp, numQuads = (int)m_quads.size();
    bool toggle = false;
    for (i = 0; i < numQuads; ++i)
    {
        temp = m_quads[i].vertRayHit(xyz);
        if (temp == 1) return 1;//means only one of the two triangulations has a hit, therefore, we can return early
        if (temp) toggle = !toggle;//even/odd winding rule
    }
    int numTris = (int)m_tris.size();
    for (i = 0; i < numTris; ++i)
    {
        if (m_tris[i].vertRayHit(xyz)) toggle = !toggle;
    }
    if (toggle) return 2;
    return 0;
}

PolyInfo::PolyInfo(const caret::SurfaceFile* innerSurf, const caret::SurfaceFile* outerSurf, const int32_t node)
{
    CaretPointer<TopologyHelper> myTopoHelp = innerSurf->getTopologyHelper();
    vector<int> myTiles;
    myTopoHelp->getNodeTiles(node, myTiles);
    int numTiles = (int)myTiles.size();
    for (int i = 0; i < numTiles; ++i)
    {
        const int32_t* myTri = innerSurf->getTriangle(myTiles[i]);
        if (myTri[0] == node)
        {
            addTri(innerSurf, outerSurf, myTri[0], myTri[1], myTri[2]);
        } else {
            if (myTri[1] == node)
            {
                addTri(innerSurf, outerSurf, myTri[1], myTri[2], myTri[0]);
            } else {
                addTri(innerSurf, outerSurf, myTri[2], myTri[0], myTri[1]);
            }
        }
    }
}

void QuadInfo::initialize(const float* xyz1, const float* xyz2, const float* xyz3, const float* xyz4)
{
    m_tris[0][0].initialize(xyz1, xyz2, xyz3);
    m_tris[0][1].initialize(xyz1, xyz3, xyz4);
    m_tris[1][0].initialize(xyz1, xyz2, xyz4);
    m_tris[1][1].initialize(xyz2, xyz3, xyz4);
}

int QuadInfo::vertRayHit(const float* xyz)
{
    int ret = 0;
    if (m_tris[0][0].vertRayHit(xyz) != m_tris[0][1].vertRayHit(xyz)) ++ret;
    if (m_tris[1][0].vertRayHit(xyz) != m_tris[1][1].vertRayHit(xyz)) ++ret;
    return ret;
}

void TriInfo::initialize(const float* xyz1, const float* xyz2, const float* xyz3)
{
    m_xyz[0] = xyz1; m_xyz[1] = xyz2; m_xyz[2] = xyz3;
    FloatMatrix myRref;
    myRref.resize(3, 4);
    for (int i = 0; i < 3; ++i)//ax + by + c = z
    {
        myRref[i][0] = m_xyz[i][0];//coefficient of a
        myRref[i][1] = m_xyz[i][1];//coefficient of b
        myRref[i][2] = 1;//coefficient of c
        myRref[i][3] = m_xyz[i][2];//what it equals
    }
    FloatMatrix myResult = myRref.reducedRowEchelon();
    m_planeEq[0] = myResult[0][3];//a
    m_planeEq[1] = myResult[1][3];//b
    m_planeEq[2] = myResult[2][3];//c
    float sanity = m_planeEq[0] + m_planeEq[1] + m_planeEq[2];
    if (sanity != sanity || (sanity != 0.0f && sanity * 2.0f == sanity))
    {
        m_planeEq[0] = sanity;//make sure the first element easily identifies vertical triangles
    }
}

//Original copyright for PNPOLY, even though my version is entirely rewritten
//Source: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
/**
Copyright (c) 1970-2003, Wm. Randolph Franklin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

1. Redistributions of source code must retain the above copyright notice, this list of conditions
and the following disclaimers.
2. Redistributions in binary form must reproduce the above copyright notice in the documentation
and/or other materials provided with the distribution.
3. The name of W. Randolph Franklin may not be used to endorse or promote products derived from this
Software without specific prior written permission.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

bool TriInfo::vertRayHit(const float* xyz)
{
    if (m_planeEq[0] != m_planeEq[0] || (m_planeEq[0] != 0.0f && m_planeEq[0] * 2.0f == m_planeEq[0]))
    {//plane is vertical, nothing can hit it
        return false;
    }
    float planeZ = xyz[0] * m_planeEq[0] + xyz[1] * m_planeEq[1] + m_planeEq[2];//ax + by + c = z
    if (xyz[2] >= planeZ)
    {//point is above the plane
        return false;
    }//test if the x, y projection has the point inside the triangle
    //below code inspired by (lifted from) PNPOLY by Wm. Randolph Franklin, swapped x for y, and slightly rewritten, for the special case of 3 vertices
    bool inside = false;
    for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
    {
        if ((m_xyz[i][0] < xyz[0]) != (m_xyz[j][0] < xyz[0]))
        {//if one vertex is on one side of the point in the x direction, and the other is on the other side (equal case is treated as greater)
            if ((m_xyz[i][1] - m_xyz[j][1]) / (m_xyz[i][0] - m_xyz[j][0]) * (xyz[0] - m_xyz[j][0]) + m_xyz[j][1] > xyz[1])
            {//if the point on the line described by the two vertices with the same x coordinate is above (greater y) than the test point
                inside = !inside;//even/odd winding rule again
            }
        }
        j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
    }
    return inside;
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
