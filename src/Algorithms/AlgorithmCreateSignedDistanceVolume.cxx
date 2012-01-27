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

#include "AlgorithmCreateSignedDistanceVolume.h"
#include "AlgorithmException.h"
#include "SurfaceFile.h"
#include "BoundingBox.h"
#include "VolumeFile.h"
#include "CaretOMP.h"
#include "CaretHeap.h"
#include "MathFunctions.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <iostream>

using namespace caret;
using namespace std;

AString AlgorithmCreateSignedDistanceVolume::getCommandSwitch()
{
    return "-create-signed-distance-volume";
}

AString AlgorithmCreateSignedDistanceVolume::getShortDescription()
{
    return "CREATE SIGNED DISTANCE VOLUME FROM SURFACE";
}

OperationParameters* AlgorithmCreateSignedDistanceVolume::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the input surface");
    
    ret->addStringParameter(2, "refspace", "a volume in the desired output space (dims, spacing, origin)");
    
    ret->addVolumeOutputParameter(3, "outvol", "the output volume");
    
    OptionalParameter* exactDistOpt = ret->createOptionalParameter(4, "-exact-limit", "specify distance for exact output");
    exactDistOpt->addDoubleParameter(1, "dist", "distance in mm (default 5)");
    
    OptionalParameter* approxDistOpt = ret->createOptionalParameter(5, "-approx-limit", "specify distance for approximate output");
    approxDistOpt->addDoubleParameter(1, "dist", "distance in mm (default 20)");
    
    OptionalParameter* approxNeighborhoodOpt = ret->createOptionalParameter(6, "-approx-neighborhood", "voxel neighborhood for approximate calculation");
    approxNeighborhoodOpt->addIntegerParameter(1, "num", "size of neighborhood cube measured from center to face, in voxels (default 2 = 5x5x5)");
    
    OptionalParameter* windingMethodOpt = ret->createOptionalParameter(7, "-winding", "winding method for point inside surface test");
    windingMethodOpt->addStringParameter(1, "method", "name of the method");
    
    ret->setHelpText(
        AString("Computes the signed distance function of the surface.  Exact distance is calculated by finding the closest point on any surface triangle ") +
        "to the center of the voxel.  Approximate distance is calculated starting with these distances, using dijkstra's method with a neighborhood of voxels.  " +
        "Specifying too small of an exact distance may produce unexpected results.  Valid specifiers for winding methods are as follows:\n\n" +
        "EVEN_ODD (default)\nNEGATIVE\nNONZERO\nNORMALS\n\nThe NORMALS method uses the normals of triangles and edges, or the closest triangle hit by a ray from the point.  " +
        "This method may be slightly faster, but is only reliable for a closed surface that does not cross through itself.  All other methods count entry (positive) and " +
        "exit (negative) crossings of a vertical ray from the point, then counts as inside if the total is odd, negative, or nonzero, respectively."
    );
    return ret;
}

void AlgorithmCreateSignedDistanceVolume::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    AString myRefName = myParams->getString(2);
    vector<vector<float> > volSpace;
    vector<int64_t> volDims;
    {
        VolumeFile myRefSpace;
        myRefSpace.readFile(myRefName);
        volSpace = myRefSpace.getVolumeSpace();
        myRefSpace.getDimensions(volDims);
    }
    volDims.resize(3);
    VolumeFile* myVolOut = myParams->getOutputVolume(3);
    myVolOut->reinitialize(volDims, volSpace);
    float exactLim = 5.0f;
    OptionalParameter* exactDistOpt = myParams->getOptionalParameter(4);
    if (exactDistOpt->m_present)
    {
        exactLim = (float)exactDistOpt->getDouble(1);
    }
    float approxLim = 20.0f;
    OptionalParameter* approxDistOpt = myParams->getOptionalParameter(5);
    if (approxDistOpt->m_present)
    {
        approxLim = (float)approxDistOpt->getDouble(1);//don't sanity check it, less than exact limit simply turns it off, specify extremely large to do entire volume
    }
    int approxNeighborhood = 2;
    OptionalParameter* approxNeighborhoodOpt = myParams->getOptionalParameter(6);
    if (approxNeighborhoodOpt->m_present)
    {
        approxNeighborhood = (int)approxNeighborhoodOpt->getInteger(1);
    }
    WindingLogic myWinding = EVEN_ODD;
    OptionalParameter* windingMethodOpt = myParams->getOptionalParameter(7);
    if (windingMethodOpt->m_present)
    {
        AString methodName = windingMethodOpt->getString(1);
        if (methodName == "EVEN_ODD")
        {
            myWinding = EVEN_ODD;
        } else if (methodName == "NEGATIVE") {
            myWinding = NEGATIVE;
        } else if (methodName == "NONZERO") {
            myWinding = NONZERO;
        } else if (methodName == "NORMALS") {
            myWinding = NORMALS;
        } else {
            throw AlgorithmException("unrecognized winding method");
        }
    }
    AlgorithmCreateSignedDistanceVolume(myProgObj, mySurf, myVolOut, exactLim, approxLim, approxNeighborhood, myWinding);//executes the algorithm
}

AlgorithmCreateSignedDistanceVolume::AlgorithmCreateSignedDistanceVolume(ProgressObject* myProgObj, SurfaceFile* mySurf, VolumeFile* myVolOut, float exactLim, float approxLim, int approxNeighborhood, WindingLogic myWinding) : AbstractAlgorithm(myProgObj)
{
    if (exactLim <= 0.0f)
    {
        throw AlgorithmException("exact limit must be positive");
    }
    if (approxNeighborhood < 1)
    {
        throw AlgorithmException("approximate neighborhood must be at least 1");
    }
    int32_t numNodes = mySurf->getNumberOfNodes();
    float markweight = 0.1f, exactweight = 5.0f * exactLim, approxweight = 0.2f * (approxLim - exactLim);
    if (approxweight < 0.0f) approxweight = 0.0f;
    LevelProgress myProgress(myProgObj, markweight + exactweight + approxweight);
    vector<vector<float> > myVolSpace;
    myVolSpace = myVolOut->getVolumeSpace();
    Vector3D ivec, jvec, kvec;
    ivec[0] = myVolSpace[0][0]; ivec[1] = myVolSpace[1][0]; ivec[2] = myVolSpace[2][0];
    jvec[0] = myVolSpace[0][1]; jvec[1] = myVolSpace[1][1]; jvec[2] = myVolSpace[2][1];
    kvec[0] = myVolSpace[0][2]; kvec[1] = myVolSpace[1][2]; kvec[2] = myVolSpace[2][2];
    Vector3D iOrthHat = jvec.cross(kvec);//these "orth" vectors are used to find the index extremes of a sphere, adding any amount of the other index vectors increases their length
    iOrthHat = iOrthHat.normal();
    if (iOrthHat.dot(ivec) < 0) iOrthHat = -iOrthHat;//make sure it lies with rather than against the i vector
    Vector3D jOrthHat = ivec.cross(kvec);
    jOrthHat = jOrthHat.normal();
    if (jOrthHat.dot(jvec) < 0) jOrthHat = -jOrthHat;
    Vector3D kOrthHat = ivec.cross(jvec);
    kOrthHat = kOrthHat.normal();
    if (kOrthHat.dot(kvec) < 0) kOrthHat = -kOrthHat;
    vector<int64_t> myDims;
    myVolOut->getDimensions(myDims);
    myVolOut->setValueAllVoxels(0.0f);
    //list all voxels to be exactly computed
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    CaretArray<int> volMarked(frameSize);
    for (int64_t i = 0; i < frameSize; ++i)
    {
        volMarked[i] = 0;
    }
    myProgress.setTask("marking voxel to be calculated exactly");
    cout << "marking voxels to be calculated exactly" << endl;
#pragma omp CARET_PARFOR
    for (int node = 0; node < numNodes; ++node)
    {
        int64_t ijk[3];
        Vector3D nodeCoord = mySurf->getCoordinate(node), tempvec;
        float tempf, tempf2, tempf3;
        tempvec = nodeCoord - iOrthHat * exactLim;
        myVolOut->spaceToIndex(tempvec.m_vec, tempf, tempf2, tempf3);//compute bounding box once rather than doing a convoluted sphere loop construct
        int64_t imin = (int64_t)ceil(tempf);
        if (imin < 0) imin = 0;
        tempvec = nodeCoord + iOrthHat * exactLim;
        myVolOut->spaceToIndex(tempvec.m_vec, tempf, tempf2, tempf3);
        int64_t imax = (int64_t)floor(tempf) + 1;
        if (imax > myDims[0]) imax = myDims[0];
        tempvec = nodeCoord - jOrthHat * exactLim;
        myVolOut->spaceToIndex(tempvec.m_vec, tempf, tempf2, tempf3);
        int64_t jmin = (int64_t)ceil(tempf2);
        if (jmin < 0) jmin = 0;
        tempvec = nodeCoord + jOrthHat * exactLim;
        myVolOut->spaceToIndex(tempvec.m_vec, tempf, tempf2, tempf3);
        int64_t jmax = (int64_t)floor(tempf2) + 1;
        if (jmax > myDims[1]) jmax = myDims[1];
        tempvec = nodeCoord - kOrthHat * exactLim;
        myVolOut->spaceToIndex(tempvec.m_vec, tempf, tempf2, tempf3);
        int64_t kmin = (int64_t)ceil(tempf3);
        if (kmin < 0) kmin = 0;
        tempvec = nodeCoord + kOrthHat * exactLim;
        myVolOut->spaceToIndex(tempvec.m_vec, tempf, tempf2, tempf3);
        int64_t kmax = (int64_t)floor(tempf3) + 1;
        if (kmax > myDims[2]) kmax = myDims[2];
        for (ijk[0] = imin; ijk[0] < imax; ++ijk[0])
        {
            for (ijk[1] = jmin; ijk[1] < jmax; ++ijk[1])
            {
                for (ijk[2] = kmin; ijk[2] < kmax; ++ijk[2])
                {
                    myVolOut->indexToSpace(ijk, tempvec.m_vec);
                    tempvec -= nodeCoord;
                    if (tempvec.length() <= exactLim)
                    {
                        volMarked[myVolOut->getIndex(ijk)] = 1;
                    }
                }
            }
        }
    }
    vector<int64_t> exactVoxelList;
    {
        int64_t ijk[3];
        for (ijk[0] = 0; ijk[0] < myDims[0]; ++ijk[0])
        {
            for (ijk[1] = 0; ijk[1] < myDims[1]; ++ijk[1])
            {
                for (ijk[2] = 0; ijk[2] < myDims[2]; ++ijk[2])
                {
                    if (volMarked[myVolOut->getIndex(ijk)] == 1)
                    {
                        exactVoxelList.push_back(ijk[0]);
                        exactVoxelList.push_back(ijk[1]);
                        exactVoxelList.push_back(ijk[2]);
                    }
                }
            }
        }
        myProgress.reportProgress(markweight);
        myProgress.setTask("generating indexing structure");
        cout << "generating indexing structure" << endl;
        CaretPointer<SignedDistToSurfIndexedBase> myDistBase(new SignedDistToSurfIndexedBase(mySurf));
        myProgress.setTask("computing exact distances");
        cout << "computing exact distances" << endl;
#pragma omp CARET_PAR
        {
            SignedDistToSurfIndexed myDist(myDistBase);
            int numExact = (int)exactVoxelList.size();
            Vector3D thisCoord;
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < numExact; i += 3)
            {
                myVolOut->indexToSpace(exactVoxelList.data() + i, thisCoord.m_vec);
                myVolOut->setValue(myDist.dist(thisCoord.m_vec, myWinding), exactVoxelList.data() + i);
                volMarked[myVolOut->getIndex(exactVoxelList.data() + i)] |= 22;//set marked to have valid value (positive and negative), and frozen
            }
        }
    }
    myProgress.reportProgress(markweight + exactweight);
    myProgress.setTask("approximating distances in extended region");
    cout << "approximating distances in extended region" << endl;
    if (approxLim > exactLim)
    {
        int faceNeigh[] = { 1, 0, 0, 
                            -1, 0, 0,
                            0, 1, 0,
                            0, -1, 0,
                            0, 0, 1,
                            0, 0, -1 };
        vector<DistVoxOffset> neighborhood;//this will contain ONLY the shortest voxel offsets with unique 3d slopes within the neighborhood
        DistVoxOffset tempIndex;
        Vector3D tempvec;
        for (int i = -approxNeighborhood; i <= approxNeighborhood; ++i)
        {
            tempIndex.m_offset[0] = i;
            for (int j = -approxNeighborhood; j <= approxNeighborhood; ++j)
            {
                tempIndex.m_offset[1] = j;
                for (int k = -approxNeighborhood; k <= approxNeighborhood; ++k)
                {
                    tempIndex.m_offset[2] = k;
                    tempvec = ivec * i + jvec * j + kvec * k;
                    tempIndex.m_dist = tempvec.length();
                    int low, med, high;
                    low = min(min(abs(i), abs(j)), abs(k));//stupid sort
                    high = max(max(abs(i), abs(j)), abs(k));
                    if (abs(i) != low && abs(i) != high)
                    {
                        med = abs(i);
                    } else if (abs(j) != low && abs(j) != high) {
                        med = abs(j);
                    } else {
                        med = abs(k);
                    }
                    if (low == 0)
                    {
                        if (med == 0)
                        {
                            if (high == 1)
                            {
                                neighborhood.push_back(tempIndex);//face neighbors
                            }
                        } else {
                            if (MathFunctions::gcd(med, high) == 1)
                            {
                                neighborhood.push_back(tempIndex);//unique in-plane
                            }
                        }
                    } else {
                        if (MathFunctions::gcd(MathFunctions::gcd(low, med), high) == 1)
                        {
                            neighborhood.push_back(tempIndex);//unique out of plane
                        }
                    }
                }
            }
        }//positives
        float maxFaceDist = max(max(ivec.length(), jvec.length()), kvec.length()) * 1.01f;//add a fudge factor to make sure rounding error doesn't remove a cardinal direction
        int neighSize = neighborhood.size();//this is provably correct for volumes where there is no diagonal shorter than the longest index vector, so we test this explicitly just in case
        CaretMinHeap<VoxelIndex, float> posHeap;
        CaretArray<int64_t> heapIndexes(frameSize);
        int numExact = (int)exactVoxelList.size();
        for (int i = 0; i < numExact; i += 3)
        {
            int64_t* thisVoxel = exactVoxelList.data() + i;
            float tempf = myVolOut->getValue(thisVoxel);
            for (int neigh = 0; neigh < neighSize; ++neigh)
            {
                int64_t tempijk[3];
                tempijk[0] = thisVoxel[0] + neighborhood[neigh].m_offset[0];
                tempijk[1] = thisVoxel[1] + neighborhood[neigh].m_offset[1];
                tempijk[2] = thisVoxel[2] + neighborhood[neigh].m_offset[2];
                if (myVolOut->indexValid(tempijk))
                {
                    int64_t tempindex = myVolOut->getIndex(tempijk);
                    float tempf2 = tempf + neighborhood[neigh].m_dist;
                    if (abs(tempf2) <= approxLim && (volMarked[tempindex] & 4) == 0 && ((volMarked[tempindex] & 2) == 0 || tempf2 < myVolOut->getValue(tempijk)))
                    {//within approxlim (so no stragglers outside limit), not frozen, and either no value or worse value
                        volMarked[tempindex] |= 2;
                        myVolOut->setValue(tempf2, tempijk);
                    }
                }
            }
            if (tempf > 0.0f)
            {//start only from positive values
                //check face neighbors for being unmarked
                for (int neigh = 0; neigh < 18; neigh += 3)
                {
                    int64_t tempijk[3];
                    tempijk[0] = thisVoxel[0] + faceNeigh[neigh];
                    tempijk[1] = thisVoxel[1] + faceNeigh[neigh + 1];
                    tempijk[2] = thisVoxel[2] + faceNeigh[neigh + 2];
                    if (myVolOut->indexValid(tempijk))
                    {
                        int64_t tempIndex = myVolOut->getIndex(tempijk);
                        if ((volMarked[tempIndex] & 1) == 0)
                        {//only add this to the heap if it has unmarked face neighbors
                            posHeap.push(VoxelIndex(thisVoxel), tempf);//don't need to store the index to change the key, value is frozen
                            break;
                        }
                    }
                }
            }
        }//initialization done, now (sort of) dijkstras - in order to not go to the "inside" (and to run faster), it only adds things that are FACE neighbors to the heap for cubic voxels
        while (!posHeap.isEmpty())
        {
            float curDist;
            VoxelIndex curVoxel = posHeap.pop(&curDist);
            int64_t curIndex = myVolOut->getIndex(curVoxel.m_ijk);
            volMarked[curIndex] |= 4;//frozen
            volMarked[curIndex] &= ~8;//no longer in the heap, don't try to modify by the index it used to have
            for (int neigh = 0; neigh < neighSize; ++neigh)
            {
                int64_t tempijk[3];
                tempijk[0] = curVoxel.m_ijk[0] + neighborhood[neigh].m_offset[0];
                tempijk[1] = curVoxel.m_ijk[1] + neighborhood[neigh].m_offset[1];
                tempijk[2] = curVoxel.m_ijk[2] + neighborhood[neigh].m_offset[2];
                float tempf = curDist + neighborhood[neigh].m_dist;
                if (myVolOut->indexValid(tempijk))
                {
                    int tempindex = myVolOut->getIndex(tempijk);
                    int tempmark = volMarked[tempindex];
                    if (abs(tempf) <= approxLim && (tempmark & 4) == 0 && ((tempmark & 2) == 0 || myVolOut->getValue(tempijk) > tempf))
                    {//within range, not frozen, no value or current value is worse
                        volMarked[tempindex] |= 2;//valid value
                        myVolOut->setValue(tempf, tempijk);
                        if ((tempmark & 8) != 0)//if it is already in the heap, we must update its key (log time worst case, but changes should generally be small)
                        {
                            posHeap.changekey(heapIndexes[tempindex], tempf);
                        }
                    }
                    if ((tempmark & 12) == 0 && (tempmark & 2) != 0 && neighborhood[neigh].m_dist <= maxFaceDist)
                    {//this neatly handles both face neighbors and any other needed neighbors to maintain dijkstra correctness under extreme scenarios
                        heapIndexes[tempindex] = posHeap.push(VoxelIndex(tempijk), myVolOut->getValue(tempijk));
                        volMarked[tempindex] |= 8;//has a heap index
                    }
                }
            }
        }//negatives
        myProgress.reportProgress(markweight + exactweight + approxweight * 0.5f);
        CaretMaxHeap<VoxelIndex, float> negHeap;
        for (int i = 0; i < numExact; i += 3)
        {
            int64_t* thisVoxel = exactVoxelList.data() + i;
            float tempf = myVolOut->getValue(thisVoxel);
            for (int neigh = 0; neigh < neighSize; ++neigh)
            {
                int64_t tempijk[3];
                tempijk[0] = thisVoxel[0] + neighborhood[neigh].m_offset[0];
                tempijk[1] = thisVoxel[1] + neighborhood[neigh].m_offset[1];
                tempijk[2] = thisVoxel[2] + neighborhood[neigh].m_offset[2];
                if (myVolOut->indexValid(tempijk))
                {
                    int64_t tempindex = myVolOut->getIndex(tempijk);
                    float tempf2 = tempf - neighborhood[neigh].m_dist;
                    if (abs(tempf2) <= approxLim && (volMarked[tempindex] & 4) == 0 && ((volMarked[tempindex] & 16) == 0 || tempf2 > myVolOut->getValue(tempijk)))
                    {//within approxlim (so no stragglers outside limit), not frozen, and either no value or worse value
                        volMarked[tempindex] |= 16;
                        myVolOut->setValue(tempf2, tempijk);
                    }
                }
            }
            if (tempf < 0.0f)
            {//start only from negative values
                //check face neighbors for being unmarked
                for (int neigh = 0; neigh < 18; neigh += 3)
                {
                    int64_t tempijk[3];
                    tempijk[0] = thisVoxel[0] + faceNeigh[neigh];
                    tempijk[1] = thisVoxel[1] + faceNeigh[neigh + 1];
                    tempijk[2] = thisVoxel[2] + faceNeigh[neigh + 2];
                    if (myVolOut->indexValid(tempijk))
                    {
                        int64_t tempIndex = myVolOut->getIndex(tempijk);
                        if ((volMarked[tempIndex] & 1) == 0)
                        {//only add this to the heap if it has unmarked face neighbors
                            negHeap.push(VoxelIndex(thisVoxel), tempf);//don't need to store the index to change the key, value is frozen
                            break;
                        }
                    }
                }
            }
        }
        while (!negHeap.isEmpty())
        {
            float curDist;
            VoxelIndex curVoxel = negHeap.pop(&curDist);
            int64_t curIndex = myVolOut->getIndex(curVoxel.m_ijk);
            volMarked[curIndex] |= 4;//frozen
            volMarked[curIndex] &= ~8;//no longer in the heap, don't try to modify by the index it used to have
            for (int neigh = 0; neigh < neighSize; ++neigh)
            {
                int64_t tempijk[3];
                tempijk[0] = curVoxel.m_ijk[0] + neighborhood[neigh].m_offset[0];
                tempijk[1] = curVoxel.m_ijk[1] + neighborhood[neigh].m_offset[1];
                tempijk[2] = curVoxel.m_ijk[2] + neighborhood[neigh].m_offset[2];
                float tempf = curDist - neighborhood[neigh].m_dist;
                if (myVolOut->indexValid(tempijk))
                {
                    int tempindex = myVolOut->getIndex(tempijk);
                    int tempmark = volMarked[tempindex];
                    if (abs(tempf) <= approxLim && (tempmark & 4) == 0 && ((tempmark & 16) == 0 || myVolOut->getValue(tempijk) < tempf))
                    {//within range, not frozen, no value or current value is worse
                        volMarked[tempindex] |= 16;//valid value
                        myVolOut->setValue(tempf, tempijk);
                        if ((tempmark & 8) != 0)//if it is already in the heap, we must update its key (log time worst case, but changes should generally be small)
                        {
                            negHeap.changekey(heapIndexes[tempindex], tempf);
                        }
                    }
                    if ((tempmark & 12) == 0 && (tempmark & 16) != 0 && neighborhood[neigh].m_dist <= maxFaceDist)
                    {//this neatly handles both face neighbors and any other needed neighbors to maintain dijkstra correctness under extreme scenarios
                        heapIndexes[tempindex] = negHeap.push(VoxelIndex(tempijk), myVolOut->getValue(tempijk));
                        volMarked[tempindex] |= 8;//has a heap index
                    }
                }
            }
        }
    }//done!
}

float SignedDistToSurfIndexed::dist(float coord[3], AlgorithmCreateSignedDistanceVolume::WindingLogic myWinding)
{
    CaretSimpleMinHeap<Oct<SignedDistToSurfIndexedBase::TriVector>*, float> myHeap;
    myHeap.push(m_base->m_indexRoot, m_base->m_indexRoot->distToPoint(coord));
    ClosestPointInfo tempInfo, bestInfo;
    float tempf = -1.0f, bestTriDist = -1.0f;
    bool first = true;
    int numChanged = 0;
    while (!myHeap.isEmpty())
    {
        Oct<SignedDistToSurfIndexedBase::TriVector>* curOct = myHeap.pop();
        if (curOct->m_leaf)
        {
            vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
            int numTris = (int)myVecRef.size();
            for (int i = 0; i < numTris; ++i)
            {
                if (m_triMarked[myVecRef[i]] != 1)
                {
                    m_triMarked[myVecRef[i]] = 1;
                    m_triMarkChanged[numChanged++] = myVecRef[i];
                    tempf = unsignedDistToTri(coord, myVecRef[i], tempInfo);
                    if (first || tempf < bestTriDist)
                    {
                        bestInfo = tempInfo;
                        bestTriDist = tempf;
                        first = false;
                    }
                }
            }
        } else {
            for (int ci = 0; ci < 2; ++ci)
            {
                for (int cj = 0; cj < 2; ++cj)
                {
                    for (int ck = 0; ck < 2; ++ck)
                    {
                        tempf = curOct->m_children[ci][cj][ck]->distToPoint(coord);
                        if (first || tempf < bestTriDist)
                        {
                            myHeap.push(curOct->m_children[ci][cj][ck], tempf);
                        }
                    }
                }
            }
        }
    }
    while (numChanged)
    {
        m_triMarked[m_triMarkChanged[--numChanged]] = 0;//need to do this before computeSign
    }
    return bestTriDist * computeSign(coord, bestInfo, myWinding);
}

int SignedDistToSurfIndexed::computeSign(float coord[3], SignedDistToSurfIndexed::ClosestPointInfo myInfo, AlgorithmCreateSignedDistanceVolume::WindingLogic myWinding)
{
    Vector3D point = coord;
    Vector3D result = point - myInfo.tempPoint;
    float tempf;
    switch (myWinding)
    {
        case AlgorithmCreateSignedDistanceVolume::EVEN_ODD:
        case AlgorithmCreateSignedDistanceVolume::NEGATIVE:
        case AlgorithmCreateSignedDistanceVolume::NONZERO:
            {
                int numChanged = 0;
                /*const vector<int>& myTiles = m_topoHelp->getNodeTiles(myInfo.node1);
                bool first = true;
                float bestNorm = 0;
                Vector3D tempvec, tempvec2, bestCent;
                for (int i = 0; i < (int)myTiles.size(); ++i)//find the tile of the node with the normal most parallel to the line segment between centroid and point
                {//should be least likely to have an intervening triangle
                    const int32_t* myTileNodes = m_base->m_surface->getTriangle(myTiles[i]);
                    Vector3D vert1 = m_base->m_surface->getCoordinate(myTileNodes[0]);
                    Vector3D vert2 = m_base->m_surface->getCoordinate(myTileNodes[1]);
                    Vector3D vert3 = m_base->m_surface->getCoordinate(myTileNodes[2]);
                    Vector3D centroid = (vert1 + vert2 + vert3) / 3.0f;
                    if (MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, tempvec.m_vec))//make sure the triangle has a valid normal
                    {
                        tempvec2 = point - centroid;
                        tempf = tempvec.dot(tempvec2.normal());
                        if (first || abs(tempf) > abs(bestNorm))
                        {
                            first = false;
                            bestNorm = tempf;
                            bestCent = centroid;
                        }
                    }
                }
                Vector3D mySeg = point - bestCent;//*/
                float positiveZ[3] = {0, 0, 1};
                Vector3D point2 = point + positiveZ;
                int crossCount = 0;
                vector<Oct<SignedDistToSurfIndexedBase::TriVector>*> myStack;
                myStack.push_back(m_base->m_indexRoot);
                while (!myStack.empty())
                {
                    Oct<SignedDistToSurfIndexedBase::TriVector>* curOct = myStack[myStack.size() - 1];
                    myStack.pop_back();
                    if (curOct->m_leaf)
                    {
                        vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
                        int numTris = (int)myVecRef.size();
                        for (int i = 0; i < numTris; ++i)
                        {
                            if (m_triMarked[myVecRef[i]] != 1)
                            {
                                m_triMarked[myVecRef[i]] = 1;
                                m_triMarkChanged[numChanged++] = myVecRef[i];
                                const int32_t* myTileNodes = m_base->m_surface->getTriangle(myVecRef[i]);
                                Vector3D verts[3];
                                verts[0] = m_base->m_surface->getCoordinate(myTileNodes[0]);
                                verts[1] = m_base->m_surface->getCoordinate(myTileNodes[1]);
                                verts[2] = m_base->m_surface->getCoordinate(myTileNodes[2]);
                                Vector3D triNormal;
                                MathFunctions::normalVector(verts[0], verts[1], verts[2], triNormal);
                                float factor = triNormal.dot(positiveZ);
                                if (factor != 0.0f)
                                {
                                    if (triNormal.dot(verts[0] - point) / factor > 0.0f && pointInTri(verts, point, 0, 1))
                                    {
                                        if (triNormal[2] < 0.0f)
                                        {
                                            ++crossCount;
                                        } else {
                                            --crossCount;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        for (int ci = 0; ci < 2; ++ci)
                        {
                            for (int cj = 0; cj < 2; ++cj)
                            {
                                for (int ck = 0; ck < 2; ++ck)
                                {
                                    if (curOct->m_children[ci][cj][ck]->rayIntersects(coord, point2))
                                    {
                                        myStack.push_back(curOct->m_children[ci][cj][ck]);
                                    }
                                }
                            }
                        }
                    }
                }
                while (numChanged)
                {
                    m_triMarked[m_triMarkChanged[--numChanged]] = 0;
                }
                switch (myWinding)
                {
                    case AlgorithmCreateSignedDistanceVolume::EVEN_ODD:
                        if ((abs(crossCount) & 1) == 1) return -1;//& 1 instead of % 2
                        return 1;
                        break;
                    case AlgorithmCreateSignedDistanceVolume::NEGATIVE:
                        if (crossCount < 0) return -1;
                        return 1;
                        break;
                    case AlgorithmCreateSignedDistanceVolume::NONZERO:
                        if (crossCount != 0) return -1;
                        return 1;
                        break;
                    default:
                        return 1;//because compiler can't handle when a switch doesn't accound for an enum value...
                }
            }
            break;
        case AlgorithmCreateSignedDistanceVolume::NORMALS:
            switch (myInfo.type)
            {
                case 0://node
                    {
                        int curSign = 0;
                        int numChanged = 0;
                        const vector<int>& myTiles = m_topoHelp->getNodeTiles(myInfo.node1);
                        bool first = true;
                        float bestNorm = 0;
                        int bestTile = -1;
                        Vector3D tempvec, tempvec2, bestCent;
                        for (int i = 0; i < (int)myTiles.size(); ++i)//find the tile of the node with the normal most parallel to the line segment between centroid and point
                        {//should be least likely to have an intervening triangle
                            const int32_t* myTileNodes = m_base->m_surface->getTriangle(myTiles[i]);
                            Vector3D vert1 = m_base->m_surface->getCoordinate(myTileNodes[0]);
                            Vector3D vert2 = m_base->m_surface->getCoordinate(myTileNodes[1]);
                            Vector3D vert3 = m_base->m_surface->getCoordinate(myTileNodes[2]);
                            Vector3D centroid = (vert1 + vert2 + vert3) / 3.0f;
                            if (MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, tempvec.m_vec))//make sure the triangle has a valid normal
                            {
                                tempvec2 = point - centroid;
                                tempf = tempvec.dot(tempvec2.normal());
                                if (first || abs(tempf) > abs(bestNorm))
                                {
                                    if (tempf > 0.0f)
                                    {
                                        curSign = 1;
                                    } else {
                                        curSign = -1;
                                    }
                                    bestTile = i;
                                    first = false;
                                    bestNorm = tempf;
                                    bestCent = centroid;
                                }
                            }
                        }
                        Vector3D mySeg = point - bestCent;
                        float bestDist = mySeg.length();
                        Vector3D segNormal = mySeg.normal();//from the surface to the point, to match the convention of triangles with normals oriented outwards
                        int majAxis = 0, midAxis = 1;//find the axes to use for projecting the triangles, discard the one most aligned with the line segment
                        if (abs(mySeg[1]) < abs(mySeg[0]))
                        {
                            majAxis = 1;
                            midAxis = 0;
                        }
                        if (abs(mySeg[2]) < abs(mySeg[midAxis]))
                        {
                            midAxis = 2;
                        }
                        vector<Oct<SignedDistToSurfIndexedBase::TriVector>*> myStack;
                        myStack.push_back(m_base->m_indexRoot);
                        while (!myStack.empty())
                        {
                            Oct<SignedDistToSurfIndexedBase::TriVector>* curOct = myStack[myStack.size() - 1];
                            myStack.pop_back();
                            if (curOct->m_leaf)
                            {
                                vector<int32_t>& myVecRef = *(curOct->m_data.m_triList);
                                int numTris = (int)myVecRef.size();
                                for (int i = 0; i < numTris; ++i)
                                {
                                    if (m_triMarked[myVecRef[i]] != 1)
                                    {
                                        m_triMarked[myVecRef[i]] = 1;
                                        m_triMarkChanged[numChanged++] = myVecRef[i];
                                        const int32_t* myTileNodes = m_base->m_surface->getTriangle(myVecRef[i]);
                                        Vector3D verts[3];
                                        verts[0] = m_base->m_surface->getCoordinate(myTileNodes[0]);
                                        verts[1] = m_base->m_surface->getCoordinate(myTileNodes[1]);
                                        verts[2] = m_base->m_surface->getCoordinate(myTileNodes[2]);
                                        Vector3D triNormal;
                                        MathFunctions::normalVector(verts[0], verts[1], verts[2], triNormal);
                                        float factor = triNormal.dot(segNormal);
                                        if (factor == 0.0f)
                                        {
                                            continue;//skip triangles parallel to the line segment
                                        }
                                        float intersectDist = triNormal.dot(point - verts[0]) / factor;
                                        if (intersectDist > 0.0f && intersectDist < bestDist)
                                        {
                                            Vector3D inPlane = point - intersectDist * segNormal;
                                            if (pointInTri(verts, inPlane, majAxis, midAxis))
                                            {
                                                bestDist = intersectDist;
                                                if (triNormal.dot(mySeg) > 0.0f)
                                                {
                                                    curSign = 1;
                                                } else {
                                                    curSign = -1;
                                                }
                                            }
                                        }
                                    }
                                }
                            } else {
                                for (int ci = 0; ci < 2; ++ci)
                                {
                                    for (int cj = 0; cj < 2; ++cj)
                                    {
                                        for (int ck = 0; ck < 2; ++ck)
                                        {
                                            if (curOct->m_children[ci][cj][ck]->lineSegmentIntersects(coord, bestCent))
                                            {
                                                myStack.push_back(curOct->m_children[ci][cj][ck]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        while (numChanged)
                        {
                            m_triMarked[m_triMarkChanged[--numChanged]] = 0;
                        }
                        return curSign;
                    }
                    break;
                case 1://edge
                    {
                        const vector<TopologyEdgeInfo>& edgeInfo = m_topoHelp->getEdgeInfo();
                        const vector<int>& edges = m_topoHelp->getNodeEdges(myInfo.node1);
                        int whichEdge = -1, numEdges = (int)edges.size();
                        for (int i = 0; i < numEdges; ++i)
                        {
                            if (edgeInfo[edges[i]].node1 == myInfo.node2 || edgeInfo[edges[i]].node2 == myInfo.node2)
                            {
                                whichEdge = edges[i];
                            }
                        }
                        CaretAssert(whichEdge != -1);
                        int tile1 = edgeInfo[whichEdge].tiles[0].tile, tile2 = edgeInfo[whichEdge].tiles[1].tile;
                        Vector3D normalaccum, tempvec;//default constructor initializes it to the zero vector
                        if (tile1 > -1)
                        {
                            const int32_t* tile1nodes = m_base->m_surface->getTriangle(tile1);
                            MathFunctions::normalVector(m_base->m_surface->getCoordinate(tile1nodes[0]),
                                                        m_base->m_surface->getCoordinate(tile1nodes[1]),
                                                        m_base->m_surface->getCoordinate(tile1nodes[2]), tempvec.m_vec);
                            normalaccum += tempvec;
                        }
                        if (tile2 > -1)
                        {
                            const int32_t *tile2nodes = m_base->m_surface->getTriangle(tile2);
                            MathFunctions::normalVector(m_base->m_surface->getCoordinate(tile2nodes[0]),
                                                        m_base->m_surface->getCoordinate(tile2nodes[1]),
                                                        m_base->m_surface->getCoordinate(tile2nodes[2]), tempvec.m_vec);
                            normalaccum += tempvec;
                        }
                        if (normalaccum.dot(result) < 0.0f)
                        {
                            return -1;
                        }
                    }
                    break;
                case 2://face
                    {
                        Vector3D triNormal;
                        const int32_t* triNodes = m_base->m_surface->getTriangle(myInfo.triangle);
                        Vector3D vert1 = m_base->m_surface->getCoordinate(triNodes[0]);
                        Vector3D vert2 = m_base->m_surface->getCoordinate(triNodes[1]);
                        Vector3D vert3 = m_base->m_surface->getCoordinate(triNodes[2]);
                        MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, triNormal.m_vec);
                        if (triNormal.dot(result) < 0.0f)
                        {
                            return -1;
                        }
                    }
                    break;
            }
            break;
    }
    return 1;
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

bool SignedDistToSurfIndexed::pointInTri(Vector3D verts[3], Vector3D inPlane, int majAxis, int midAxis)
{
    bool inside = false;
    for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
    {
        if ((verts[i][majAxis] < inPlane[majAxis]) != (verts[j][majAxis] < inPlane[majAxis]))
        {//if one vertex is on one side of the point in the x direction, and the other is on the other side (equal case is treated as greater)
            if ((verts[i][midAxis] - verts[j][midAxis]) / (verts[i][majAxis] - verts[j][majAxis]) * (inPlane[majAxis] - verts[j][majAxis]) + verts[j][midAxis] > inPlane[midAxis])
            {//if the point on the line described by the two vertices with the same x coordinate is above (greater y) than the test point
                inside = !inside;//even/odd winding rule
            }
        }
        j = i;//consecutive vertices, does 2,0 then 0,1 then 1,2
    }
    return inside;
}

float SignedDistToSurfIndexed::unsignedDistToTri(float coord[3], int32_t triangle, ClosestPointInfo& myInfo)
{
    const int32_t* triNodes = m_base->m_surface->getTriangle(triangle);
    Vector3D point = coord, tempPoint;
    Vector3D vert1 = m_base->m_surface->getCoordinate(triNodes[0]);
    Vector3D vert2 = m_base->m_surface->getCoordinate(triNodes[1]);
    Vector3D vert3 = m_base->m_surface->getCoordinate(triNodes[2]);
    Vector3D v21hat = vert2 - vert1;
    float origLength;
    v21hat = v21hat.normal(&origLength);
    int type = 0;//tracks whether it is closest to a node, an edge, or the face
    int32_t node1 = -1, node2 = -1;//tracks which nodes are involved
    float tempf = v21hat.dot(point - vert1);
    if (tempf < 0.0f)
    {
        tempPoint = vert1;
        node1 = triNodes[0];
    } else if (tempf > origLength) {
        tempPoint = vert2;
        node1 = triNodes[1];
    } else {
        tempPoint = vert1 + tempf * v21hat;
        node1 = triNodes[0];
        node2 = triNodes[1];
        ++type;
    }
    Vector3D v3that = vert3 - tempPoint;
    v3that = v3that.normal(&origLength);
    tempf = v3that.dot(point - tempPoint);
    if (tempf > origLength)
    {
        tempPoint = vert3;
        node1 = triNodes[2];
    } else if (tempf > 0.0f) {
        tempPoint += tempf * v3that;
        node2 = triNodes[2];//don't worry, if this puts it on the face, then it doesn't use node1/node2
        ++type;
    }//if less than 0, keep point the same
    Vector3D result = point - tempPoint;
    myInfo.type = type;
    myInfo.node1 = node1;
    myInfo.node2 = node2;
    myInfo.triangle = triangle;
    myInfo.tempPoint = tempPoint;
    return result.length();
}

SignedDistToSurfIndexed::SignedDistToSurfIndexed(CaretPointer<SignedDistToSurfIndexedBase> myBase)
{
    m_base = myBase;
    m_topoHelp = myBase->m_topoHelp;//because we don't need neighborsToDepth, just share the same one
    int32_t numTris = m_base->m_surface->getNumberOfTriangles();
    m_triMarked = CaretArray<int>(numTris);
    m_triMarkChanged = CaretArray<int>(numTris);
    for (int32_t i = 0; i < numTris; ++i)
    {
        m_triMarked[i] = 0;
    }
}

SignedDistToSurfIndexedBase::SignedDistToSurfIndexedBase(SurfaceFile* mySurf)
{
    m_surface = mySurf;
    m_topoHelp = mySurf->getTopologyHelper();
    const float* myBB = mySurf->getBoundingBox()->getBounds();
    Vector3D minCoord, maxCoord;
    minCoord[0] = myBB[0]; maxCoord[0] = myBB[1];
    minCoord[1] = myBB[2]; maxCoord[1] = myBB[3];
    minCoord[2] = myBB[4]; maxCoord[2] = myBB[5];
    m_indexRoot = new Oct<TriVector>(minCoord, maxCoord);
    const float* myCoordData = mySurf->getCoordinateData();
    int32_t numTris = mySurf->getNumberOfTriangles();
    for (int32_t i = 0; i < numTris; ++i)
    {
        const int32_t* thisTri = mySurf->getTriangle(i);
        maxCoord = minCoord = myCoordData + thisTri[0] * 3;//set both to the coordinates of the first node in the triangle
        for (int j = 1; j < 3; ++j)
        {
            int32_t thisNode3 = thisTri[j] * 3;
            if (myCoordData[thisNode3] < minCoord[0]) minCoord[0] = myCoordData[thisNode3];
            if (myCoordData[thisNode3 + 1] < minCoord[1]) minCoord[1] = myCoordData[thisNode3 + 1];
            if (myCoordData[thisNode3 + 2] < minCoord[2]) minCoord[2] = myCoordData[thisNode3 + 2];
            if (myCoordData[thisNode3] > maxCoord[0]) maxCoord[0] = myCoordData[thisNode3];
            if (myCoordData[thisNode3 + 1] > maxCoord[1]) maxCoord[1] = myCoordData[thisNode3 + 1];
            if (myCoordData[thisNode3 + 2] > maxCoord[2]) maxCoord[2] = myCoordData[thisNode3 + 2];
        }
        addTriangle(m_indexRoot, i, minCoord, maxCoord);//use bounding box for now as an easy test to capture any chance of the triangle intersecting the Oct
    }
}

void SignedDistToSurfIndexedBase::addTriangle(Oct<TriVector>* thisOct, int32_t triangle, float minCoord[3], float maxCoord[3])
{
    if (thisOct->m_leaf)
    {
        thisOct->m_data.m_triList->push_back(triangle);
        int numTris = (int)thisOct->m_data.m_triList->size();
        if (numTris >= NUM_TRIS_TO_TEST && numTris % NUM_TRIS_TEST_INCR == NUM_TRIS_TO_TEST % NUM_TRIS_TEST_INCR)//the second modulus should const out
        {
            Vector3D tempMinCoord, tempMaxCoord;
            const float* myCoordData = m_surface->getCoordinateData();
            int totalSize = 0;
            int numSplit = 0;
            for (int i = 0; i < numTris; ++i)//gather data on how it would end up splitting
            {
                const int32_t* tempTri = m_surface->getTriangle((*(thisOct->m_data.m_triList))[i]);
                tempMaxCoord = tempMinCoord = myCoordData + tempTri[0] * 3;//set both to the coordinates of the first node in the triangle
                for (int j = 1; j < 3; ++j)
                {
                    int32_t thisNode3 = tempTri[j] * 3;
                    if (myCoordData[thisNode3] < tempMinCoord[0]) tempMinCoord[0] = myCoordData[thisNode3];
                    if (myCoordData[thisNode3 + 1] < tempMinCoord[1]) tempMinCoord[1] = myCoordData[thisNode3 + 1];
                    if (myCoordData[thisNode3 + 2] < tempMinCoord[2]) tempMinCoord[2] = myCoordData[thisNode3 + 2];
                    if (myCoordData[thisNode3] > tempMaxCoord[0]) tempMaxCoord[0] = myCoordData[thisNode3];
                    if (myCoordData[thisNode3 + 1] > tempMaxCoord[1]) tempMaxCoord[1] = myCoordData[thisNode3 + 1];
                    if (myCoordData[thisNode3 + 2] > tempMaxCoord[2]) tempMaxCoord[2] = myCoordData[thisNode3 + 2];
                }
                int minOct[3], maxOct[3];
                thisOct->containingChild(tempMinCoord, minOct);
                thisOct->containingChild(tempMaxCoord, maxOct);
                int splitSize = 8;
                if (minOct[0] == maxOct[0]) splitSize >>= 1;
                if (minOct[1] == maxOct[1]) splitSize >>= 1;
                if (minOct[2] == maxOct[2]) splitSize >>= 1;
                totalSize += splitSize;
                if (splitSize != 8) ++numSplit;
            }
            if (numSplit > 0 && totalSize < 3.0f * numTris)//don't split if all triangles end up in all child octs, and try to balance speedup with memory usage
            {
                thisOct->makeChildren();//do the split
                for (int i = 0; i < numTris; ++i)//gather data on how it would end up splitting
                {
                    const int32_t* tempTri = m_surface->getTriangle((*(thisOct->m_data.m_triList))[i]);
                    tempMaxCoord = tempMinCoord = myCoordData + tempTri[0] * 3;//set both to the coordinates of the first node in the triangle
                    for (int j = 1; j < 3; ++j)
                    {
                        int32_t thisNode3 = tempTri[j] * 3;
                        if (myCoordData[thisNode3] < tempMinCoord[0]) tempMinCoord[0] = myCoordData[thisNode3];
                        if (myCoordData[thisNode3 + 1] < tempMinCoord[1]) tempMinCoord[1] = myCoordData[thisNode3 + 1];
                        if (myCoordData[thisNode3 + 2] < tempMinCoord[2]) tempMinCoord[2] = myCoordData[thisNode3 + 2];
                        if (myCoordData[thisNode3] > tempMaxCoord[0]) tempMaxCoord[0] = myCoordData[thisNode3];
                        if (myCoordData[thisNode3 + 1] > tempMaxCoord[1]) tempMaxCoord[1] = myCoordData[thisNode3 + 1];
                        if (myCoordData[thisNode3 + 2] > tempMaxCoord[2]) tempMaxCoord[2] = myCoordData[thisNode3 + 2];
                    }
                    for (int ci = 0; ci < 2; ++ci)
                    {
                        for (int cj = 0; cj < 2; ++cj)
                        {
                            for (int ck = 0; ck < 2; ++ck)
                            {
                                if (thisOct->m_children[ci][cj][ck]->boundsOverlaps(tempMinCoord, tempMaxCoord))
                                {
                                    addTriangle(thisOct->m_children[ci][cj][ck], (*(thisOct->m_data.m_triList))[i], tempMinCoord, tempMaxCoord);
                                }
                            }
                        }
                    }
                }
                thisOct->m_data.freeData();//and free up some memory
            }
        }
    } else {
        for (int ci = 0; ci < 2; ++ci)
        {
            for (int cj = 0; cj < 2; ++cj)
            {
                for (int ck = 0; ck < 2; ++ck)
                {
                    if (thisOct->m_children[ci][cj][ck]->boundsOverlaps(minCoord, maxCoord))
                    {
                        addTriangle(thisOct->m_children[ci][cj][ck], triangle, minCoord, maxCoord);
                    }
                }
            }
        }
    }
}

float AlgorithmCreateSignedDistanceVolume::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCreateSignedDistanceVolume::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
