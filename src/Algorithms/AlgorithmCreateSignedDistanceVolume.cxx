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

#include "AlgorithmCreateSignedDistanceVolume.h"

#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CaretHeap.h"
#include "FastStatistics.h"
#include "MathFunctions.h"
#include "NiftiIO.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

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
    
    OptionalParameter* roiOutOpt = ret->createOptionalParameter(9, "-roi-out", "output an roi volume of where the output has a computed value");
    roiOutOpt->addVolumeOutputParameter(1, "roi-vol", "the output roi volume");
    
    OptionalParameter* fillValOpt = ret->createOptionalParameter(4, "-fill-value", "specify a value to put in all voxels that don't get assigned a distance");
    fillValOpt->addDoubleParameter(1, "value", "value to fill with (default 0)");
    
    OptionalParameter* exactDistOpt = ret->createOptionalParameter(5, "-exact-limit", "specify distance for exact output");
    exactDistOpt->addDoubleParameter(1, "dist", "distance in mm (default 5)");
    
    OptionalParameter* approxDistOpt = ret->createOptionalParameter(6, "-approx-limit", "specify distance for approximate output");
    approxDistOpt->addDoubleParameter(1, "dist", "distance in mm (default 20)");
    
    OptionalParameter* approxNeighborhoodOpt = ret->createOptionalParameter(7, "-approx-neighborhood", "voxel neighborhood for approximate calculation");
    approxNeighborhoodOpt->addIntegerParameter(1, "num", "size of neighborhood cube measured from center to face, in voxels (default 2 = 5x5x5)");
    
    OptionalParameter* windingMethodOpt = ret->createOptionalParameter(8, "-winding", "winding method for point inside surface test");
    windingMethodOpt->addStringParameter(1, "method", "name of the method (default EVEN_ODD)");
    
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
        NiftiIO refSpaceIO;
        refSpaceIO.openRead(myRefName);
        volDims = refSpaceIO.getDimensions();
        if (volDims.size() < 3) volDims.resize(3, 1);
        volSpace = refSpaceIO.getHeader().getSForm();
    }
    volDims.resize(3);
    VolumeFile* myVolOut = myParams->getOutputVolume(3);
    myVolOut->reinitialize(volDims, volSpace);
    float fillValue = 0.0f;
    OptionalParameter* fillValOpt = myParams->getOptionalParameter(4);
    if (fillValOpt->m_present)
    {
        fillValue = (float)fillValOpt->getDouble(1);
    }
    float exactLim = 5.0f;
    OptionalParameter* exactDistOpt = myParams->getOptionalParameter(5);
    if (exactDistOpt->m_present)
    {
        exactLim = (float)exactDistOpt->getDouble(1);
    }
    float approxLim = 20.0f;
    OptionalParameter* approxDistOpt = myParams->getOptionalParameter(6);
    if (approxDistOpt->m_present)
    {
        approxLim = (float)approxDistOpt->getDouble(1);//don't sanity check it, less than exact limit simply turns it off, specify extremely large to do entire volume
    }
    int approxNeighborhood = 2;
    OptionalParameter* approxNeighborhoodOpt = myParams->getOptionalParameter(7);
    if (approxNeighborhoodOpt->m_present)
    {
        approxNeighborhood = (int)approxNeighborhoodOpt->getInteger(1);
    }
    SignedDistanceHelper::WindingLogic myWinding = SignedDistanceHelper::EVEN_ODD;
    OptionalParameter* windingMethodOpt = myParams->getOptionalParameter(8);
    if (windingMethodOpt->m_present)
    {
        AString methodName = windingMethodOpt->getString(1);
        if (methodName == "EVEN_ODD")
        {
            myWinding = SignedDistanceHelper::EVEN_ODD;
        } else if (methodName == "NEGATIVE") {
            myWinding = SignedDistanceHelper::NEGATIVE;
        } else if (methodName == "NONZERO") {
            myWinding = SignedDistanceHelper::NONZERO;
        } else if (methodName == "NORMALS") {
            myWinding = SignedDistanceHelper::NORMALS;
        } else {
            throw AlgorithmException("unrecognized winding method");
        }
    }
    VolumeFile* myRoiOut = NULL;
    OptionalParameter* roiOutOpt = myParams->getOptionalParameter(9);
    if (roiOutOpt->m_present)
    {
        myRoiOut = roiOutOpt->getOutputVolume(1);
    }
    AlgorithmCreateSignedDistanceVolume(myProgObj, mySurf, myVolOut, myRoiOut, fillValue, exactLim, approxLim, approxNeighborhood, myWinding);
}

AlgorithmCreateSignedDistanceVolume::AlgorithmCreateSignedDistanceVolume(ProgressObject* myProgObj, const SurfaceFile* mySurf, VolumeFile* myVolOut, VolumeFile* myRoiOut, const float& fillValue,
                                                                         const float& exactLim, const float& approxLim, const int& approxNeighborhood, const SignedDistanceHelper::WindingLogic& myWinding) : AbstractAlgorithm(myProgObj)
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
    myVolSpace = myVolOut->getSform();
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
    //list all voxels to be exactly computed
    int64_t frameSize = myDims[0] * myDims[1] * myDims[2];
    vector<int> volMarked(frameSize, 0);
    vector<float> scratchFrame (frameSize, fillValue);
    const float outThresh = max(exactLim, approxLim); //don't output values beyond the specified limits
    vector<int64_t> exactVoxelList;
    VolumeSpace outSpace = myVolOut->getVolumeSpace();
    const int faceNeigh[] = { 1, 0, 0, 
                            -1, 0, 0,
                            0, 1, 0,
                            0, -1, 0,
                            0, 0, 1,
                            0, 0, -1 };
    
    //fudge factor for mark and fixup method
    const float smallestSpacing = min(min(ivec.length(), jvec.length()), kvec.length());
    const float exactLimFudge = exactLim + 1.0f * smallestSpacing; //spend some extra computation in the parallel section to reduce total wall time (by reducing fixup work)
    //compare expected runtimes of kernel based and locator based marking methods
    const bool useStencil = 2.9 * myDims[0] * myDims[1] * myDims[2] > (numNodes * exactLimFudge * exactLimFudge * exactLimFudge / iOrthHat.dot(ivec) / jOrthHat.dot(jvec) / kOrthHat.dot(kvec));
    
    //correction for point locator screening for direct method
    FastStatistics edgeStats;
    mySurf->getNodesSpacingStatistics(edgeStats);
    const float longestEdge = edgeStats.getMostPositiveValue(); //can bound the error from distance to node based on edge length
    const float edgeCorrection = longestEdge / sqrt(3.0f); //worst case is equilateral triangle with longest edges, this is distance from vertex to barycenter
    const float nodeDistFudge = sqrt(exactLim * exactLim + edgeCorrection * edgeCorrection); //vectors from barycenter to farthest point and barycenter to vertex are orthogonal
    const bool uselocator = (nodeDistFudge < 2.0f * exactLim); //take a guess at the crossover point where point locator isn't specific enough to be helpful
    
    //fixup method is faster when it chooses to use stencil (high resolution voxels, small exact distance)
    //direct method is faster on decently-shaped surfaces at larger limits (when stencil would be slow), and is far simpler code
    const bool useDirect = !useStencil;
    //yes, this means the fixup method never gets used in non-stencil mode
    //fixup is single-threaded, like the approximation, but calls signed distance, so the fully parallel direct method generally finishes faster unless stencil saves a lot of time
    if (useDirect)
    {
        //exclude the easy calls with point locator, slightly faster when the surface is well-behaved, slower when the edges are long
#pragma omp CARET_PAR
        {
            CaretPointer<SignedDistanceHelper> myDist = mySurf->getSignedDistanceHelper();
#pragma omp CARET_FOR schedule(dynamic)
            for (int64_t k = 0; k < myDims[2]; ++k)
            {
                for (int64_t j = 0; j < myDims[1]; ++j)
                {
                    for (int64_t i = 0; i < myDims[0]; ++i)
                    {
                        Vector3D voxCoord = outSpace.indexToSpace(i, j, k);
                        if (!uselocator || mySurf->closestNode(voxCoord, nodeDistFudge) != -1)
                        {
                            bool valid = false;
                            float tempf = myDist->distLimited(voxCoord, exactLim, valid, myWinding);
                            if (valid)
                            {
                                int64_t tempindex = myVolOut->getIndex(i, j, k);
                                scratchFrame[tempindex] = tempf;
                                volMarked[tempindex] = 23; //frozen, is in exact list, has valid value for pos and neg
                            }
                        }
                    }
                }
            }
        }
        int64_t ijk[3];
        for (ijk[2] = 0; ijk[2] < myDims[2]; ++ijk[2])
        {
            for (ijk[1] = 0; ijk[1] < myDims[1]; ++ijk[1])
            {
                for (ijk[0] = 0; ijk[0] < myDims[0]; ++ijk[0])
                {
                    if (volMarked[myVolOut->getIndex(ijk)] != 0)
                    {
                        exactVoxelList.push_back(ijk[0]);
                        exactVoxelList.push_back(ijk[1]);
                        exactVoxelList.push_back(ijk[2]);
                    }
                }
            }
        }
    } else {
        myProgress.setTask("marking voxels to be calculated exactly");
        if (!useStencil)
        {
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int64_t k = 0; k < myDims[2]; ++k)
            {
                for (int64_t j = 0; j < myDims[1]; ++j)
                {
                    for (int64_t i = 0; i < myDims[0]; ++i)
                    {
                        Vector3D voxCoord;
                        myVolOut->indexToSpace(i, j, k, voxCoord);
                        int32_t ret = mySurf->closestNode(voxCoord, exactLimFudge);
                        if (ret != -1)
                        {
                            volMarked[myVolOut->getIndex(i, j, k)] = 1; //this mark only matters during approximation - if we calculate an exact distance that is beyond the exact limit, use it as long as it is within approx limit
                        }
                    }
                }
            }
        } else { //when you only need to set a very small fraction of the total voxels, it is faster to mark everything near each vertex with a stencil and fix up holes later
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int node = 0; node < numNodes; ++node)
            {
                int64_t ijk[3];
                Vector3D nodeCoord = mySurf->getCoordinate(node), tempvec;
                float tempf, tempf2, tempf3;
                tempvec = nodeCoord - iOrthHat * exactLimFudge;
                myVolOut->spaceToIndex(tempvec, tempf, tempf2, tempf3);//compute bounding box once rather than doing a convoluted sphere loop construct
                int64_t imin = (int64_t)ceil(tempf);
                if (imin < 0) imin = 0;
                tempvec = nodeCoord + iOrthHat * exactLimFudge;
                myVolOut->spaceToIndex(tempvec, tempf, tempf2, tempf3);
                int64_t imax = (int64_t)floor(tempf) + 1;
                if (imax > myDims[0]) imax = myDims[0];
                tempvec = nodeCoord - jOrthHat * exactLimFudge;
                myVolOut->spaceToIndex(tempvec, tempf, tempf2, tempf3);
                int64_t jmin = (int64_t)ceil(tempf2);
                if (jmin < 0) jmin = 0;
                tempvec = nodeCoord + jOrthHat * exactLimFudge;
                myVolOut->spaceToIndex(tempvec, tempf, tempf2, tempf3);
                int64_t jmax = (int64_t)floor(tempf2) + 1;
                if (jmax > myDims[1]) jmax = myDims[1];
                tempvec = nodeCoord - kOrthHat * exactLimFudge;
                myVolOut->spaceToIndex(tempvec, tempf, tempf2, tempf3);
                int64_t kmin = (int64_t)ceil(tempf3);
                if (kmin < 0) kmin = 0;
                tempvec = nodeCoord + kOrthHat * exactLimFudge;
                myVolOut->spaceToIndex(tempvec, tempf, tempf2, tempf3);
                int64_t kmax = (int64_t)floor(tempf3) + 1;
                if (kmax > myDims[2]) kmax = myDims[2];
                for (ijk[2] = kmin; ijk[2] < kmax; ++ijk[2])
                {
                    for (ijk[1] = jmin; ijk[1] < jmax; ++ijk[1])
                    {
                        for (ijk[0] = imin; ijk[0] < imax; ++ijk[0])
                        {
                            myVolOut->indexToSpace(ijk, tempvec);
                            tempvec -= nodeCoord;
                            if (tempvec.length() <= exactLimFudge)
                            {
                                volMarked[myVolOut->getIndex(ijk)] = 1;
                            }
                        }
                    }
                }
            }
        }
        int64_t ijk[3];
        for (ijk[2] = 0; ijk[2] < myDims[2]; ++ijk[2])
        {
            for (ijk[1] = 0; ijk[1] < myDims[1]; ++ijk[1])
            {
                for (ijk[0] = 0; ijk[0] < myDims[0]; ++ijk[0])
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
        myProgress.setTask("computing exact distances");
#pragma omp CARET_PAR
        {
            int64_t numExact = (int64_t)exactVoxelList.size();
            CaretPointer<SignedDistanceHelper> myDist = mySurf->getSignedDistanceHelper();
            Vector3D thisCoord;
#pragma omp CARET_FOR schedule(dynamic)
            for (int64_t i = 0; i < numExact; i += 3)
            {
                const int64_t* thisVox = exactVoxelList.data() + i;
                myVolOut->indexToSpace(thisVox, thisCoord);
                bool valid = false;
                float thisDist = myDist->distLimited(thisCoord, outThresh, valid, myWinding);
                if (valid)
                {
                    int64_t tempindex = myVolOut->getIndex(thisVox);
                    scratchFrame[tempindex] = thisDist;
                    volMarked[tempindex] |= 22;//set marked to have valid value (positive and negative), and frozen
                }
            }
        }
        //fix up missing exact distances due to vertex-based marking, should be faster for extreme cases than increasing the limit based on longest edge
        //crawl neighbors of marked vertices that aren't marked, looking for any below exact threshold
        CaretPointer<SignedDistanceHelper> myDist = mySurf->getSignedDistanceHelper();
        vector<int64_t> toVisit = exactVoxelList;
        for (int64_t i = 0; i < int64_t(toVisit.size()); i += 3)
        {
            int64_t tempindex = myVolOut->getIndex(toVisit.data() + i);
            if ((volMarked[tempindex] & 22) != 0 && abs(scratchFrame[tempindex]) <= exactLim) //don't grow from a voxel with an invalid value, or that is already beyond exactLim
            {
                //check face neighbors for being unmarked
                for (int neigh = 0; neigh < 18; neigh += 3)
                {
                    int64_t tempijk[3] = { toVisit[i] + faceNeigh[neigh],
                                        toVisit[i + 1] + faceNeigh[neigh + 1],
                                        toVisit[i + 2] + faceNeigh[neigh + 2] };
                    if (myVolOut->indexValid(tempijk))
                    {
                        int64_t tempindex = myVolOut->getIndex(tempijk);
                        if ((volMarked[tempindex] & 1) == 0)
                        {
                            volMarked[tempindex] |= 1; //mark it as having had exact signed distance run on it
                            Vector3D thisCoord = outSpace.indexToSpace(tempijk);
                            bool valid = false;
                            float thisDist = myDist->distLimited(thisCoord, outThresh, valid, myWinding);
                            if (valid) //don't throw the distance away unless it is outside the maximum distance requested
                            {
                                scratchFrame[tempindex] = thisDist;
                                volMarked[tempindex] |= 22; //mark it as having a valid value
                                exactVoxelList.push_back(tempijk[0]);
                                exactVoxelList.push_back(tempijk[1]);
                                exactVoxelList.push_back(tempijk[2]);
                                if (abs(thisDist) <= exactLim) //only evaluate exact distances to one neighbor past exact threshold
                                {
                                    toVisit.push_back(tempijk[0]);
                                    toVisit.push_back(tempijk[1]);
                                    toVisit.push_back(tempijk[2]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    myProgress.reportProgress(markweight + exactweight);
    if (approxLim > exactLim)
    {
        myProgress.setTask("approximating distances in extended region");
        vector<DistVoxOffset> neighborhood;//this will contain ONLY the shortest voxel offsets with unique 3d slopes within the neighborhood
        DistVoxOffset tempOffset;
        Vector3D tempvec;
        for (int i = -approxNeighborhood; i <= approxNeighborhood; ++i)
        {
            tempOffset.m_offset[0] = i;
            for (int j = -approxNeighborhood; j <= approxNeighborhood; ++j)
            {
                tempOffset.m_offset[1] = j;
                for (int k = -approxNeighborhood; k <= approxNeighborhood; ++k)
                {
                    tempOffset.m_offset[2] = k;
                    tempvec = ivec * i + jvec * j + kvec * k;
                    tempOffset.m_dist = tempvec.length();
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
                                neighborhood.push_back(tempOffset);//face neighbors
                            }
                        } else {
                            if (MathFunctions::gcd(med, high) == 1)
                            {
                                neighborhood.push_back(tempOffset);//unique in-plane
                            }
                        }
                    } else {
                        if (MathFunctions::gcd(MathFunctions::gcd(low, med), high) == 1)
                        {
                            neighborhood.push_back(tempOffset);//unique out of plane
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
        for (int64_t i = 0; i < numExact; i += 3)
        {
            int64_t* thisVoxel = exactVoxelList.data() + i;
            int64_t thisindex = myVolOut->getIndex(thisVoxel);
            if ((volMarked[thisindex] & 2) != 0) //using fixup method, there are voxels marked as "exact signed distance run" that don't have a value, because it was beyond both limits
            {
                float tempf = scratchFrame[thisindex];
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
                        if (abs(tempf2) <= approxLim && (volMarked[tempindex] & 4) == 0 && ((volMarked[tempindex] & 2) == 0 || tempf2 < scratchFrame[tempindex]))
                        {//within approxlim (so no stragglers outside limit), not frozen, and either no value or worse value
                            volMarked[tempindex] |= 2;
                            scratchFrame[tempindex] = tempf2;
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
                if (myVolOut->indexValid(tempijk))
                {
                    float tempf = curDist + neighborhood[neigh].m_dist;
                    int tempindex = myVolOut->getIndex(tempijk);
                    int& tempmark = volMarked[tempindex];
                    if (abs(tempf) <= approxLim && (tempmark & 4) == 0 && ((tempmark & 2) == 0 || scratchFrame[tempindex] > tempf))
                    {//within range, not frozen, no value or current value is worse
                        tempmark |= 2;//valid value
                        scratchFrame[tempindex] = tempf;
                        if ((tempmark & 8) != 0)//if it is already in the heap, we must update its key (log time worst case, but changes should generally be small)
                        {
                            posHeap.changekey(heapIndexes[tempindex], tempf);
                        }
                    }
                    if ((tempmark & 12) == 0 && (tempmark & 2) != 0 && neighborhood[neigh].m_dist <= maxFaceDist)
                    {//this neatly handles both face neighbors and any other needed neighbors to maintain dijkstra correctness under extreme scenarios
                        heapIndexes[tempindex] = posHeap.push(VoxelIndex(tempijk), scratchFrame[tempindex]);
                        tempmark |= 8;//has a heap index
                    }
                }
            }
        }//negatives
        myProgress.reportProgress(markweight + exactweight + approxweight * 0.5f);
        CaretMaxHeap<VoxelIndex, float> negHeap;
        for (int64_t i = 0; i < numExact; i += 3)
        {
            int64_t* thisVoxel = exactVoxelList.data() + i;
            int64_t thisindex = myVolOut->getIndex(thisVoxel);
            if ((volMarked[thisindex] & 16) != 0) //using fixup method, there are voxels marked as "exact signed distance run" that don't have a valid value, because it was beyond both limits
            {
                float tempf = scratchFrame[thisindex];
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
                        if (abs(tempf2) <= approxLim && (volMarked[tempindex] & 4) == 0 && ((volMarked[tempindex] & 16) == 0 || tempf2 > scratchFrame[tempindex]))
                        {//within approxlim (so no stragglers outside limit), not frozen, and either no value or worse value
                            volMarked[tempindex] |= 16;
                            scratchFrame[tempindex] = tempf2;
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
                if (myVolOut->indexValid(tempijk))
                {
                    float tempf = curDist - neighborhood[neigh].m_dist;
                    int tempindex = myVolOut->getIndex(tempijk);
                    int& tempmark = volMarked[tempindex];
                    if (abs(tempf) <= approxLim && (tempmark & 4) == 0 && ((tempmark & 16) == 0 || scratchFrame[tempindex] < tempf))
                    {//within range, not frozen, no value or current value is worse
                        tempmark |= 16;//valid value
                        scratchFrame[tempindex] = tempf;
                        if ((tempmark & 8) != 0)//if it is already in the heap, we must update its key (log time worst case, but changes should generally be small)
                        {
                            negHeap.changekey(heapIndexes[tempindex], tempf);
                        }
                    }
                    if ((tempmark & 12) == 0 && (tempmark & 16) != 0 && neighborhood[neigh].m_dist <= maxFaceDist)
                    {//this neatly handles both face neighbors and any other needed neighbors to maintain dijkstra correctness under extreme scenarios
                        heapIndexes[tempindex] = negHeap.push(VoxelIndex(tempijk), scratchFrame[tempindex]);
                        tempmark |= 8;//has a heap index
                    }
                }
            }
        }
    }
    myVolOut->setFrame(scratchFrame.data());
    if (myRoiOut != NULL)
    {//now make the roi volume
        myDims.resize(3);
        myRoiOut->reinitialize(myDims, myVolOut->getSform());
        int64_t ijk[3];
        for (ijk[2] = 0; ijk[2] < myDims[2]; ++ijk[2])
        {
            for (ijk[1] = 0; ijk[1] < myDims[1]; ++ijk[1])
            {
                for (ijk[0] = 0; ijk[0] < myDims[0]; ++ijk[0])
                {
                    int64_t tempindex = myVolOut->getIndex(ijk);
                    if ((volMarked[tempindex] & 4) == 0)//only mark "frozen" (4) as valid, though "have value" (2 or 16) should now be the same
                    {
                        scratchFrame[tempindex] = 0.0f;
                    } else {
                        scratchFrame[tempindex] = 1.0f;
                    }
                }
            }
        }
        myRoiOut->setFrame(scratchFrame.data());
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
