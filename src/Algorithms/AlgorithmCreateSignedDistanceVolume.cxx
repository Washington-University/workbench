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
    exactDistOpt->addDoubleParameter(1, "dist", "distance in mm (default 3)");
    
    OptionalParameter* approxDistOpt = ret->createOptionalParameter(5, "-approx-limit", "specify distance for approximate output");
    approxDistOpt->addDoubleParameter(1, "dist", "distance in mm (default 20)");
    
    OptionalParameter* approxNeighborhoodOpt = ret->createOptionalParameter(6, "-approx-neighborhood", "voxel neighborhood for approximate calculation");
    approxNeighborhoodOpt->addIntegerParameter(1, "num", "size of neighborhood cube measured from center to face, in voxels (default 3 = 7x7x7)");
    
    ret->setHelpText(
        AString("Computes the signed distance function of the surface.  Exact distance is calculated by finding the closest point on any surface triangle ") +
        "to the center of the voxel.  Approximate distance is calculated starting with these distances, using dijkstra's method with a neighborhood of voxels.  " +
        "Specifying too small of an exact distance may produce unexpected results."
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
    float exactLim = 3.0f;
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
    int approxNeighborhood = 3;
    OptionalParameter* approxNeighborhoodOpt = myParams->getOptionalParameter(6);
    if (approxNeighborhoodOpt->m_present)
    {
        approxNeighborhood = (int)approxNeighborhoodOpt->getInteger(1);
    }
    AlgorithmCreateSignedDistanceVolume(myProgObj, mySurf, myVolOut, exactLim, approxLim, approxNeighborhood);//executes the algorithm
}

AlgorithmCreateSignedDistanceVolume::AlgorithmCreateSignedDistanceVolume(ProgressObject* myProgObj, SurfaceFile* mySurf, VolumeFile* myVolOut, float exactLim, float approxLim, int approxNeighborhood) : AbstractAlgorithm(myProgObj)
{
    if (exactLim <= 0.0f)
    {
        throw AlgorithmException("exact limit must be positive");
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
    if (kOrthHat.dot(jvec) < 0) kOrthHat = -kOrthHat;
    vector<int64_t> myDims;
    myVolOut->getDimensions(myDims);
    //list all voxels to be exactly computed
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    CaretArray<int> volMarked(new int[frameSize]);
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
        myProgress.setTask("generating indexing surface");
        cout << "generating indexing surface" << endl;
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
                myVolOut->setValue(myDist.dist(thisCoord.m_vec), exactVoxelList.data() + i);
                volMarked[myVolOut->getIndex(exactVoxelList.data() + i)] |= 6;//set marked to have valid value, and frozen
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
        CaretArray<int64_t> heapIndexes(new int64_t[frameSize]);
        int numExact = (int)exactVoxelList.size();
        for (int i = 0; i < numExact; i += 3)
        {
            int64_t* thisVoxel = exactVoxelList.data() + i;
            float tempf = myVolOut->getValue(thisVoxel);
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
                            posHeap.push(tempf, VoxelIndex(thisVoxel));//don't need to store the index to change the key, value is frozen
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
                float tempf = myVolOut->getValue(curVoxel.m_ijk) + neighborhood[neigh].m_dist;
                if (myVolOut->indexValid(tempijk))
                {
                    int tempindex = myVolOut->getIndex(tempijk);
                    int tempmark = volMarked[tempindex];
                    if (abs(tempf) <= approxLim && (tempmark & 4) == 0 && ((tempmark & 2) == 0 || myVolOut->getValue(tempijk) > tempf))
                    {//within range, not frozen, and either no value or current value is worse
                        volMarked[tempindex] |= 2;//valid value
                        myVolOut->setValue(tempf, tempijk);
                        if ((volMarked[tempindex] & 8) != 0)//if it is already in the heap, we must update its key (log time worst case, but changes should generally be small)
                        {
                            posHeap.changekey(heapIndexes[tempindex], tempf);
                        } else {
                            if (neighborhood[neigh].m_dist <= maxFaceDist)
                            {//this neatly handles both face neighbors and any other needed neighbors to maintain dijkstra correctness under extreme scenarios
                                heapIndexes[tempindex] = posHeap.push(tempf, VoxelIndex(tempijk));
                                volMarked[tempindex] |= 8;//has a heap index
                            }
                        }
                    }
                }
            }
        }//negatives
        myProgress.reportProgress(markweight + exactweight + approxweight * 0.5f);
        CaretMaxHeap<VoxelIndex, float> negHeap;
        for (int i = 0; i < numExact; i += 3)
        {
            int64_t* thisVoxel = exactVoxelList.data() + i;
            if (myVolOut->getValue(thisVoxel) < 0.0f)
            {//now do negatives
                //check face neighbors for being unmarked
                for (int neigh = 0; neigh < 18; neigh += 3)
                {
                    int64_t tempijk[3];
                    tempijk[0] = thisVoxel[0] + faceNeigh[neigh];
                    tempijk[1] = thisVoxel[1] + faceNeigh[neigh + 1];
                    tempijk[2] = thisVoxel[2] + faceNeigh[neigh + 2];
                    if (myVolOut->indexValid(tempijk))
                    {
                        int64_t thisIndex = myVolOut->getIndex(tempijk);
                        if ((volMarked[thisIndex] & 1) == 0)
                        {//only add this to the heap if it has unmarked face neighbors
                            negHeap.push(myVolOut->getValue(thisVoxel), VoxelIndex(thisVoxel));//don't need to store the index to change the key for a frozen value
                            break;
                        }
                    }
                }
            }
        }//initialization done
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
                float tempf = myVolOut->getValue(curVoxel.m_ijk) - neighborhood[neigh].m_dist;
                if (myVolOut->indexValid(tempijk))
                {
                    int tempindex = myVolOut->getIndex(tempijk);
                    int tempmark = volMarked[tempindex];
                    if (abs(tempf) <= approxLim && (tempmark & 4) == 0 && ((tempmark & 2) == 0 || myVolOut->getValue(tempijk) < tempf))
                    {//within range, not frozen, and either no value or current value is worse
                        volMarked[tempindex] |= 2;//valid value
                        myVolOut->setValue(tempf, tempijk);
                        if ((volMarked[tempindex] & 8) != 0)//if it is already in the heap, we must update its key (log time worst case, but changes should generally be small)
                        {
                            negHeap.changekey(heapIndexes[tempindex], tempf);
                        } else {
                            if (neighborhood[neigh].m_dist <= maxFaceDist)
                            {//this neatly handles both face neighbors and any other needed neighbors to maintain dijkstra correctness under extreme scenarios
                                heapIndexes[tempindex] = negHeap.push(tempf, VoxelIndex(tempijk));
                                volMarked[tempindex] |= 8;//has a heap index
                            }
                        }
                    }
                }
            }
        }
    }//done!
}

float SignedDistToSurfIndexed::dist(float coord[3])
{
    int numIndex = (int)m_base->m_indexing.size();
    Vector3D tempvec;
    float tempf, bestTriDist, absBestTriDist;
    int best = 0;
    bool first = true;//a little slower, but cleaner
    int triMarkChangeCount = 0;
    CaretMutexLocker locked(&m_mutex);
    for (int i = 0; i < numIndex; ++i)
    {
        tempvec = m_base->m_indexing[i].m_loc - Vector3D(coord);
        if (first || tempvec.length() < tempf)
        {
            tempf = tempvec.length();
            best = i;
            first = false;
        }
    }
    {
        first = true;
        vector<int>& vecRef = m_base->m_indexing[best].m_triList;
        int numTris = (int)vecRef.size();
        for (int j = 0; j < numTris; ++j)
        {
            tempf = distToTri(coord, vecRef[j]);
            m_triMarked[vecRef[j]] = 1;
            m_triMarkChanged[triMarkChangeCount++] = vecRef[j];
            if (first || abs(tempf) < absBestTriDist)
            {
                first = false;
                bestTriDist = tempf;
                absBestTriDist = abs(bestTriDist);//because we compare against this a LOT, so remove the abs call from them
            }
        }
    }
    float nodeCutoff = sqrt(bestTriDist * bestTriDist + 13.0f * m_base->m_maxEdge * m_base->m_maxEdge / 36.0f);
    float indexCutoff = nodeCutoff + m_base->m_indexLength;
    for (int i = 0; i < numIndex; ++i)
    {
        tempvec = m_base->m_indexing[i].m_loc - Vector3D(coord);
        if (i != best && (first || tempvec.length() <= indexCutoff))
        {
            vector<int>& vecRef = m_base->m_indexing[i].m_triList;
            int numTris = (int)vecRef.size();
            bool changed = false;
            for (int j = 0; j < numTris; ++j)
            {
                if (m_triMarked[vecRef[j]] == 0)
                {
                    tempf = distToTri(coord, vecRef[j]);
                    m_triMarked[vecRef[j]] = 1;
                    m_triMarkChanged[triMarkChangeCount++] = vecRef[j];
                    if (first || abs(tempf) < absBestTriDist)
                    {
                        changed = true;
                        first = false;
                        bestTriDist = tempf;
                        absBestTriDist = abs(bestTriDist);
                    }
                }
            }
            if (changed)//only change this out here because it only affects whether or not we search an index node's triangles, not anything inside the loop over triangles
            {
                nodeCutoff = sqrt(bestTriDist * bestTriDist + 13.0f * m_base->m_maxEdge * m_base->m_maxEdge / 36.0f);
                indexCutoff = nodeCutoff + m_base->m_indexLength;
            }
        }
    }
    while (triMarkChangeCount)
    {
        m_triMarked[m_triMarkChanged[--triMarkChangeCount]] = 0;//zero our marked array
    }
    return bestTriDist;
}

float SignedDistToSurfIndexed::distToTri(float coord[3], int triangle)
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
    int32_t node1, node2;//tracks which nodes are involved
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
    int distSign = 1;
    switch (type)
    {
        case 0://node
            {
                Vector3D nodeNormal = m_base->m_surface->getNormalVector(node1);
                if (nodeNormal.dot(result) < 0.0f)
                {
                    distSign = -1;
                }
            }
            break;
        case 1://edge
            {
                set<TopologyEdgeInfo>::const_iterator myEdge = m_topoHelp->getEdgeInfo().find(TopologyEdgeInfo(-1, node1, node2));
                CaretAssert(myEdge != m_topoHelp->getEdgeInfo().end());
                int tile1, tile2;
                myEdge->getTiles(tile1, tile2);
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
                    distSign = -1;
                }
            }
            break;
        case 2://face
            {
                Vector3D triNormal;
                MathFunctions::normalVector(vert1.m_vec, vert2.m_vec, vert3.m_vec, triNormal.m_vec);
                if (triNormal.dot(result) < 0.0f)
                {
                    distSign = -1;
                }
            }
            break;
    }
    return result.length() * distSign;
}

SignedDistToSurfIndexed::SignedDistToSurfIndexed(CaretPointer<SignedDistToSurfIndexedBase> myBase)
{
    m_base = myBase;
    m_topoHelp = myBase->m_surface->getTopologyHelper();
    int32_t numTris = m_base->m_surface->getNumberOfTriangles();
    m_triMarked = CaretArray<int>(new int[numTris]);
    m_triMarkChanged = CaretArray<int>(new int[numTris]);
    for (int32_t i = 0; i < numTris; ++i)
    {
        m_triMarked[i] = 0;
    }
}

SignedDistToSurfIndexedBase::SignedDistToSurfIndexedBase(SurfaceFile* mySurf)
{
    m_avgEdge = 0.0f;
    m_maxEdge = 0.0f;
    m_surface = mySurf;
    m_surface->computeNormals(false);//need unsmoothed normals to get guarantees of getting the right sign
    CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
    const set<TopologyEdgeInfo> myEdgeInfo = myTopoHelp->getEdgeInfo();
    set<TopologyEdgeInfo>::const_iterator myiter, myend = myEdgeInfo.end();
    for (myiter = myEdgeInfo.begin(); myiter != myend; ++myiter)
    {
        int node1, node2;
        myiter->getNodes(node1, node2);
        Vector3D diff = Vector3D(mySurf->getCoordinate(node1)) - Vector3D(mySurf->getCoordinate(node2));
        float tempf = diff.length();
        m_avgEdge += tempf;
        if (tempf > m_maxEdge)
        {
            m_maxEdge = tempf;
        }
    }
    m_avgEdge /= myEdgeInfo.size();
    int32_t numNodes = mySurf->getNumberOfNodes();
    m_indexLength = m_avgEdge * pow(numNodes / 6.0f, 0.16667f);//formula worked out to (approximately) minimize the number of comparisons needed for finding the distance from one point
    int numTris = mySurf->getNumberOfTriangles();
    CaretArray<int> triMarked(numTris);
    for (int i = 0; i < numTris; ++i)
    {
        triMarked[i] = -1;//because we use the node number to mark, so we never have to clear it
    }
    for (int i = 0; i < numNodes; ++i)
    {
        Vector3D thisCoord = mySurf->getCoordinate(i);
        bool inRange = false;
        int numIndexes = (int)m_indexing.size();
        for (int j = 0; j < numIndexes; ++j)
        {
            Vector3D diff = thisCoord - m_indexing[j].m_loc;
            if (diff.length() < m_indexLength)
            {
                inRange = true;
                break;
            }
        }
        if (!inRange)
        {
            m_indexing.push_back(triIndexPoint(thisCoord));
            int thisIndex = (int)m_indexing.size() - 1;
            for (int k = 0; k < numNodes; ++k)
            {//yes, this will take a while, but it doesn't compare to having to test every triangle on the surface for every voxel within the signed distance ribbon
                Vector3D diff = Vector3D(mySurf->getCoordinate(k)) - thisCoord;
                if (diff.length() < m_indexLength)
                {
                    vector<int> myTiles;
                    myTopoHelp->getNodeTiles(k, myTiles);
                    int numTiles = (int)myTiles.size();
                    for (int m = 0; m < numTiles; ++m)
                    {
                        if (triMarked[myTiles[m]] != i)
                        {
                            m_indexing[thisIndex].m_triList.push_back(myTiles[m]);
                            triMarked[myTiles[m]] = i;
                        }
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
