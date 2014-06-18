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

/*
 * For the function vertRayHit():
 * Original copyright for PNPOLY, though my version is entirely rewritten and modified
 * Source: http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
 */
/*
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

#include "AlgorithmVolumeToSurfaceMapping.h"
#include "AlgorithmException.h"

#include "CaretOMP.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"
#include "VolumeFile.h"

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
    
    ret->createOptionalParameter(5, "-enclosing", "use value of the enclosing voxel");
    
    ret->createOptionalParameter(8, "-cubic", "use cubic splines");
    
    OptionalParameter* ribbonOpt = ret->createOptionalParameter(6, "-ribbon-constrained", "use ribbon constrained mapping algorithm");
    ribbonOpt->addSurfaceParameter(1, "inner-surf", "the inner surface of the ribbon");
    ribbonOpt->addSurfaceParameter(2, "outer-surf", "the outer surface of the ribbon");
    OptionalParameter* roiVol = ribbonOpt->createOptionalParameter(3, "-volume-roi", "use a volume roi");
    roiVol->addVolumeParameter(1, "roi-volume", "the volume file");
    OptionalParameter* ribbonSubdiv = ribbonOpt->createOptionalParameter(4, "-voxel-subdiv", "voxel divisions while estimating voxel weights");
    ribbonSubdiv->addIntegerParameter(1, "subdiv-num", "number of subdivisions, default 3");
    OptionalParameter* ribbonWeights = ribbonOpt->createOptionalParameter(5, "-output-weights", "write the voxel weights for a vertex to a volume file");
    ribbonWeights->addIntegerParameter(1, "vertex", "the vertex number to get the voxel weights for, 0-based");
    ribbonWeights->addVolumeOutputParameter(2, "weights-out", "volume to write the weights to");
    
    OptionalParameter* myelinStyleOpt = ret->createOptionalParameter(9, "-myelin-style", "use the method from myelin mapping");
    myelinStyleOpt->addVolumeParameter(1, "ribbon-roi", "an roi volume of the cortical ribbon for this hemisphere");
    myelinStyleOpt->addMetricParameter(2, "thickness", "a metric file of cortical thickness");
    myelinStyleOpt->addDoubleParameter(3, "sigma", "guassian kernel in mm for weighting voxels within range");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(7, "-subvol-select", "select a single subvolume to map");
    subvolumeSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("You must specify exactly one mapping method.  Enclosing voxel uses the value from the voxel the vertex lies inside, while trilinear does a 3D ") + 
        "linear interpolation based on the voxels immediately on each side of the vertex's position.\n\n" +
        "The ribbon mapping method constructs a polyhedron from the vertex's neighbors on each " +
        "surface, and estimates the amount of this polyhedron's volume that falls inside any nearby voxels, to use as the weights for sampling.  " +
        "The volume ROI is useful to exclude partial volume effects of voxels the surfaces pass through, and will cause the mapping to ignore " +
        "voxels that don't have a positive value in the mask.  The subdivision number specifies how it approximates the amount of the volume the polyhedron " +
        "intersects, by splitting each voxel into NxNxN pieces, and checking whether the center of each piece is inside the polyhedron.  If you have very large " +
        "voxels, consider increasing this if you get zeros in your output.\n\n" +
        "The myelin style method uses part of the caret5 myelin mapping command to do the mapping: for each surface vertex, take all voxels closer than the thickness at the vertex " +
        "that are within the ribbon ROI, and less than half the thickness value away from the vertex along the direction of the surface normal, and apply a gaussian kernel " +
        "with the specified sigma to them to get the weights to use."
    );
    return ret;
}

void AlgorithmVolumeToSurfaceMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVolume = myParams->getVolume(1);
    SurfaceFile* mySurface = myParams->getSurface(2);
    MetricFile* myMetricOut = myParams->getOutputMetric(3);
    OptionalParameter* trilinearOpt = myParams->getOptionalParameter(4);
    OptionalParameter* enclosingOpt = myParams->getOptionalParameter(5);
    OptionalParameter* cubicOpt = myParams->getOptionalParameter(8);
    OptionalParameter* ribbonOpt = myParams->getOptionalParameter(6);
    OptionalParameter* myelinStyleOpt = myParams->getOptionalParameter(9);
    int64_t mySubVol = -1;
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(7);
    if (subvolumeSelect->m_present)
    {
        mySubVol = (int)myVolume->getMapIndexFromNameOrNumber(subvolumeSelect->getString(1));
        if (mySubVol < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    bool haveMethod = false;
    Method myMethod = CUBIC;//this tracks which constructor to call
    VolumeFile::InterpType volInterpMethod = VolumeFile::CUBIC;
    if (trilinearOpt->m_present)
    {
        haveMethod = true;
        myMethod = TRILINEAR;
        volInterpMethod = VolumeFile::TRILINEAR;
    }
    if (enclosingOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        haveMethod = true;
        myMethod = ENCLOSING_VOXEL;
        volInterpMethod = VolumeFile::ENCLOSING_VOXEL;
    }
    if (ribbonOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        haveMethod = true;
        myMethod = RIBBON_CONSTRAINED;
    }
    if (cubicOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        haveMethod = true;
        myMethod = CUBIC;
        volInterpMethod = VolumeFile::CUBIC;
    }
    if (myelinStyleOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        haveMethod = true;
        myMethod = MYELIN_STYLE;
    }
    if (!haveMethod)
    {
        throw AlgorithmException("no mapping method specified");
    }
    switch (myMethod)
    {
        case TRILINEAR:
        case ENCLOSING_VOXEL:
        case CUBIC:
            AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, volInterpMethod, mySubVol);//because we have separate constructors
            break;
        case RIBBON_CONSTRAINED:
        {
            SurfaceFile* innerSurf = ribbonOpt->getSurface(1);
            SurfaceFile* outerSurf = ribbonOpt->getSurface(2);
            OptionalParameter* roiVol = ribbonOpt->getOptionalParameter(3);
            VolumeFile* myRoiVol = NULL;
            if (roiVol->m_present)
            {
                myRoiVol = roiVol->getVolume(1);
            }
            int32_t subdivisions = 3;
            OptionalParameter* ribbonSubdiv = ribbonOpt->getOptionalParameter(4);
            if (ribbonSubdiv->m_present)
            {
                subdivisions = ribbonSubdiv->getInteger(1);
                if (subdivisions < 1)
                {
                    throw AlgorithmException("invalid number of subdivisions specified");
                }
            }
            int weightsOutVertex = -1;
            VolumeFile* weightsOut = NULL;
            OptionalParameter* ribbonWeights = ribbonOpt->getOptionalParameter(5);
            if (ribbonWeights->m_present)
            {
                weightsOutVertex = (int)ribbonWeights->getInteger(1);
                weightsOut = ribbonWeights->getOutputVolume(2);
            }
            AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, innerSurf, outerSurf, myRoiVol, subdivisions, mySubVol, weightsOutVertex, weightsOut);
            break;
        }
        case MYELIN_STYLE:
        {
            VolumeFile* roi = myelinStyleOpt->getVolume(1);
            MetricFile* thickness = myelinStyleOpt->getMetric(2);
            float sigma = (float)myelinStyleOpt->getDouble(3);
            AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, roi, thickness, sigma, mySubVol);
            break;
        }
        default:
            throw AlgorithmException("this method not yet implemented");
    }
}

//interpolation mapping
AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                                                 const VolumeFile::InterpType& myMethod, const int64_t& mySubVol) : AbstractAlgorithm(myProgObj)
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
        numColumns = myVolDims[3] * myVolDims[4];
    } else {
        numColumns = myVolDims[4];
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myMetricOut->setStructure(mySurface->getStructure());
    vector<float> myArray(numNodes);
    AString methodName;
    switch (myMethod)
    {
        case VolumeFile::CUBIC:
            methodName = " cubic";
            break;
        case VolumeFile::TRILINEAR:
            methodName = " trilinear";
            break;
        case VolumeFile::ENCLOSING_VOXEL:
            methodName = " enclosing voxel";
            break;
    }
    if (mySubVol == -1)
    {
        for (int64_t i = 0; i < myVolDims[3]; ++i)
        {
            for (int64_t j = 0; j < myVolDims[4]; ++j)
            {
                if (myMethod == VolumeFile::CUBIC)
                {
                    myVolume->validateSpline(i, j);//to do spline deconvolution in parallel
                }
                AString metricLabel = myVolume->getMapName(i);
                if (myVolDims[4] != 1)
                {
                    metricLabel += " component " + AString::number(j);
                }
                metricLabel += methodName;
                int64_t thisCol = i * myVolDims[4] + j;
                myMetricOut->setColumnName(thisCol, metricLabel);
#pragma omp CARET_PARFOR
                for (int64_t node = 0; node < numNodes; ++node)
                {
                    myArray[node] = myVolume->interpolateValue(mySurface->getCoordinate(node), myMethod, NULL, i, j);
                }
                if (myMethod == VolumeFile::CUBIC)
                {
                    myVolume->freeSpline(i, j);//release memory we no longer need, if we allocated it
                }
                myMetricOut->setValuesForColumn(thisCol, myArray.data());
            }
        }
    } else {
        for (int64_t j = 0; j < myVolDims[4]; ++j)
        {
            if (myMethod == VolumeFile::CUBIC)
            {
                myVolume->validateSpline(mySubVol, j);//to do spline deconvolution in parallel
            }
            AString metricLabel = myVolume->getMapName(mySubVol);
            if (myVolDims[4] != 1)
            {
                metricLabel += " component " + AString::number(j);
            }
            metricLabel += methodName;
            int64_t thisCol = j;
            myMetricOut->setColumnName(thisCol, metricLabel);
#pragma omp CARET_PARFOR
            for (int64_t node = 0; node < numNodes; ++node)
            {
                myArray[node] = myVolume->interpolateValue(mySurface->getCoordinate(node), myMethod, NULL, mySubVol, j);
            }
            if (myMethod == VolumeFile::CUBIC)
            {
                myVolume->freeSpline(mySubVol, j);//release memory we no longer need, if we allocated it
            }
            myMetricOut->setValuesForColumn(thisCol, myArray.data());
        }
    }
}

//ribbon mapping
AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                                                 const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const VolumeFile* roiVol,
                                                                 const int32_t& subdivisions, const int64_t& mySubVol, const int& weightsOutVertex, VolumeFile* weightsOut) : AbstractAlgorithm(myProgObj)
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
        numColumns = myVolDims[3] * myVolDims[4];
    } else {
        numColumns = myVolDims[4];
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myMetricOut->setStructure(mySurface->getStructure());
    if (!mySurface->hasNodeCorrespondence(*outerSurf) || !mySurface->hasNodeCorrespondence(*innerSurf))
    {
        throw AlgorithmException("all surfaces must be in vertex correspondence");
    }
    if (roiVol != NULL && !roiVol->matchesVolumeSpace(myVolume))
    {
        throw AlgorithmException("roi volume is not in the same volume space as input volume");
    }
    if (weightsOut != NULL)
    {
        if (weightsOutVertex < 0 || weightsOutVertex >= numNodes) throw AlgorithmException("invalid vertex for voxel weights output");
        vector<int64_t> weightDims = myVolDims;
        weightDims.resize(3);
        weightsOut->reinitialize(weightDims, myVolume->getSform());
    }
    vector<vector<VoxelWeight> > myWeights;
    precomputeWeightsRibbon(myWeights, myVolume, innerSurf, outerSurf, roiVol, subdivisions);
    if (weightsOut != NULL)
    {
        weightsOut->setValueAllVoxels(0.0f);
        const vector<VoxelWeight>& vertexWeights = myWeights[weightsOutVertex];
        int numWeights = (int)vertexWeights.size();
        for (int i = 0; i < numWeights; ++i)
        {
            weightsOut->setValue(vertexWeights[i].weight, vertexWeights[i].ijk);
        }
    }
    CaretArray<float> myScratchArray(numNodes);
    float* myScratch = myScratchArray.getArray();
    if (mySubVol == -1)
    {
        for (int64_t i = 0; i < myVolDims[3]; ++i)
        {
            for (int64_t j = 0; j < myVolDims[4]; ++j)
            {
                int64_t thisCol = i * myVolDims[4] + j;
                AString metricLabel = myVolume->getMapName(i);
                if (myVolDims[4] != 1)
                {
                    metricLabel += " component " + AString::number(j);
                }
                metricLabel += " ribbon constrained";
                myMetricOut->setColumnName(thisCol, metricLabel);
#pragma omp CARET_PARFOR schedule(dynamic)
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
#pragma omp CARET_PARFOR schedule(dynamic)
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

//myelin style mapping
AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                                                 const VolumeFile* roiVol, const MetricFile* thickness, const float& sigma, const int64_t& mySubVol): AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> myVolDims;
    myVolume->getDimensions(myVolDims);
    if (mySubVol >= myVolDims[3] || mySubVol < -1)
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    if (!roiVol->matchesVolumeSpace(myVolume))
    {
        throw AlgorithmException("roi volume is not in the same volume space as input volume");
    }
    int64_t numColumns;
    if (mySubVol == -1)
    {
        numColumns = myVolDims[3] * myVolDims[4];
    } else {
        numColumns = myVolDims[4];
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myMetricOut->setStructure(mySurface->getStructure());
    vector<vector<VoxelWeight> > myWeights;
    precomputeWeightsMyelin(myWeights, mySurface, roiVol, thickness, sigma);
    vector<float> myScratch(numNodes);
    if (mySubVol == -1)
    {
        for (int64_t i = 0; i < myVolDims[3]; ++i)
        {
            for (int64_t j = 0; j < myVolDims[4]; ++j)
            {
                int64_t thisCol = i * myVolDims[4] + j;
                AString metricLabel = myVolume->getMapName(i);
                if (myVolDims[4] != 1)
                {
                    metricLabel += " component " + AString::number(j);
                }
                metricLabel += " ribbon constrained";
                myMetricOut->setColumnName(thisCol, metricLabel);
#pragma omp CARET_PARFOR schedule(dynamic)
                for (int64_t node = 0; node < numNodes; ++node)
                {
                    double accum = 0.0;
                    int numVoxels = (int)myWeights[node].size();
                    for (int voxel = 0; voxel < numVoxels; ++voxel)
                    {
                        accum += myWeights[node][voxel].weight * myVolume->getValue(myWeights[node][voxel].ijk, i, j);//weights have already been normalized in precompute, for this method
                    }
                    myScratch[node] = accum;
                }
                myMetricOut->setValuesForColumn(thisCol, myScratch.data());
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
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int64_t node = 0; node < numNodes; ++node)
            {
                double accum = 0.0;
                int numVoxels = (int)myWeights[node].size();
                for (int voxel = 0; voxel < numVoxels; ++voxel)
                {
                    accum += myWeights[node][voxel].weight * myVolume->getValue(myWeights[node][voxel].ijk, mySubVol, j);//weights have already been normalized in precompute, for this method
                }
                myScratch[node] = accum;
            }
            myMetricOut->setValuesForColumn(thisCol, myScratch.data());
        }
    }
}

void AlgorithmVolumeToSurfaceMapping::precomputeWeightsRibbon(vector<vector<VoxelWeight> >& myWeights, const VolumeFile* myVolume, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf,
                                                              const VolumeFile* roiVol, const int& numDivisions)
{
    int64_t numNodes = outerSurf->getNumberOfNodes();
    myWeights.resize(numNodes);
    const vector<vector<float> >& myVolSpace = myVolume->getSform();
    Vector3D origin, ivec, jvec, kvec;//these are the spatial projections of the ijk unit vectors (also, the offset that specifies the origin)
    ivec[0] = myVolSpace[0][0]; jvec[0] = myVolSpace[0][1]; kvec[0] = myVolSpace[0][2]; origin[0] = myVolSpace[0][3];
    ivec[1] = myVolSpace[1][0]; jvec[1] = myVolSpace[1][1]; kvec[1] = myVolSpace[1][2]; origin[1] = myVolSpace[1][3];
    ivec[2] = myVolSpace[2][0]; jvec[2] = myVolSpace[2][1]; kvec[2] = myVolSpace[2][2]; origin[2] = myVolSpace[2][3];
    const float* outerCoords = outerSurf->getCoordinateData();
    const float* innerCoords = innerSurf->getCoordinateData();
    vector<int64_t> myDims;
    myVolume->getDimensions(myDims);
#pragma omp CARET_PAR
    {
        int maxVoxelCount = 10;//guess for preallocating vectors
        CaretPointer<TopologyHelper> myTopoHelp = innerSurf->getTopologyHelper();
#pragma omp CARET_FOR schedule(dynamic)
        for (int64_t node = 0; node < numNodes; ++node)
        {
            myWeights[node].clear();
            myWeights[node].reserve(maxVoxelCount);
            float tempf;
            int64_t node3 = node * 3;
            PolyInfo myPoly(innerSurf, outerSurf, node);//build the polygon
            Vector3D minIndex, maxIndex, tempvec;
            myVolume->spaceToIndex(innerCoords + node3, minIndex);//find the bounding box in VOLUME INDEX SPACE, starting with the center nodes
            maxIndex = minIndex;
            myVolume->spaceToIndex(outerCoords + node3, tempvec);
            for (int i = 0; i < 3; ++i)
            {
                if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
            }
            int numNeigh;
            const int* myNeighList = myTopoHelp->getNodeNeighbors(node, numNeigh);//and now the neighbors
            for (int j = 0; j < numNeigh; ++j)
            {
                int neigh3 = myNeighList[j] * 3;
                myVolume->spaceToIndex(outerCoords + neigh3, tempvec);
                for (int i = 0; i < 3; ++i)
                {
                    if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                    if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
                }
                myVolume->spaceToIndex(innerCoords + neigh3, tempvec);
                for (int i = 0; i < 3; ++i)
                {
                    if (tempvec[i] < minIndex[i]) minIndex[i] = tempvec[i];
                    if (tempvec[i] > maxIndex[i]) maxIndex[i] = tempvec[i];
                }
            }
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
                        if (roiVol == NULL || roiVol->getValue(ijk) > 0.0f)
                        {
                            tempf = computeVoxelFraction(myVolume, ijk, myPoly, numDivisions, ivec, jvec, kvec);
                            if (tempf != 0.0f)
                            {
                                myWeights[node].push_back(VoxelWeight(tempf, ijk));
                            }
                        }
                    }
                }
            }
            if ((int)myWeights[node].size() > maxVoxelCount)
            {//capacity() would use more memory
                maxVoxelCount = myWeights[node].size();
            }
        }
    }
}

void AlgorithmVolumeToSurfaceMapping::precomputeWeightsMyelin(vector<vector<VoxelWeight> >& myWeights, const SurfaceFile* mySurface, const VolumeFile* roiVol,
                                                              const MetricFile* thickness, const float& sigma)
{
    int64_t numNodes = mySurface->getNumberOfNodes();
    myWeights.clear();
    myWeights.resize(numNodes);
    vector<int64_t> myDims;
    roiVol->getDimensions(myDims);
    vector<vector<float> > volSpace = roiVol->getSform();//same as input volume
    Vector3D ivec, jvec, kvec, origin, ijorth, jkorth, kiorth;
    ivec[0] = volSpace[0][0]; jvec[0] = volSpace[0][1]; kvec[0] = volSpace[0][2]; origin[0] = volSpace[0][3];
    ivec[1] = volSpace[1][0]; jvec[1] = volSpace[1][1]; kvec[1] = volSpace[1][2]; origin[1] = volSpace[1][3];
    ivec[2] = volSpace[2][0]; jvec[2] = volSpace[2][1]; kvec[2] = volSpace[2][2]; origin[2] = volSpace[2][3];
    ijorth = ivec.cross(jvec).normal();//find the box in index space that encloses a sphere of radius 1
    jkorth = jvec.cross(kvec).normal();
    kiorth = kvec.cross(ivec).normal();
    float range[3];
    range[0] = abs(1.0f / ivec.dot(jkorth));//we can multiply these by thickness to get the range of voxels to check
    range[1] = abs(1.0f / jvec.dot(kiorth));
    range[2] = abs(1.0f / kvec.dot(ijorth));
    const float* thicknessData = thickness->getValuePointerForColumn(0);
    const float* normals = mySurface->getNormalData();
    float invnegsigmasquaredx2 = -1.0f / (2.0f * sigma * sigma);
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int node = 0; node < numNodes; ++node)
    {
        Vector3D nodeCoord = mySurface->getCoordinate(node), nodeIndices, nodenormal = normals + (node * 3);
        roiVol->spaceToIndex(nodeCoord, nodeIndices);
        float nodeThick = thicknessData[node];
        int64_t min[3], max[3];
        for (int i = 0; i < 3; ++i)
        {
            min[i] = (int)ceil(nodeIndices[i] - range[i] * nodeThick);
            if (min[i] < 0) min[i] = 0;
            max[i] = (int)floor(nodeIndices[i] + range[i] * nodeThick) + 1;
            if (max[i] > myDims[i]) max[i] = myDims[i];
        }
        double weightTotal = 0.0;
        int64_t ijk[3];
        for (ijk[2] = min[2]; ijk[2] < max[2]; ++ijk[2])
        {
            for (ijk[1] = min[1]; ijk[1] < max[1]; ++ijk[1])
            {
                for (ijk[0] = min[0]; ijk[0] < max[0]; ++ijk[0])
                {
                    Vector3D voxelCoord;
                    roiVol->indexToSpace(ijk, voxelCoord);
                    Vector3D delta = voxelCoord - nodeCoord;
                    if (abs(nodenormal.dot(delta)) < 0.5f * nodeThick && roiVol->getValue(ijk) > 0.0f)
                    {
                        float weight = exp(delta.lengthsquared() * invnegsigmasquaredx2);
                        myWeights[node].push_back(VoxelWeight(weight, ijk));
                        weightTotal += weight;
                    }
                }
            }
        }
        int numWeights = (int)myWeights[node].size();
        for (int i = 0; i < numWeights; ++i)
        {
            myWeights[node][i].weight /= weightTotal;//normalize weights to eliminate the need to divide
        }
    }
}

float AlgorithmVolumeToSurfaceMapping::computeVoxelFraction(const VolumeFile* myVolume, const int64_t* ijk, PolyInfo& myPoly, const int divisions,
                                                            const Vector3D& ivec, const Vector3D& jvec, const Vector3D& kvec)
{
    Vector3D myLowCorner;
    myVolume->indexToSpace(ijk[0] - 0.5f, ijk[1] - 0.5f, ijk[2] - 0.5f, myLowCorner);
    int inside = 0;
    Vector3D istep = ivec / divisions;
    Vector3D jstep = jvec / divisions;
    Vector3D kstep = kvec / divisions;
    myLowCorner += istep * 0.5f + jstep * 0.5f + kstep * 0.5f;
    for (int i = 0; i < divisions; ++i)
    {
        Vector3D tempVeci = myLowCorner + istep * i;
        for (int j = 0; j < divisions; ++j)
        {
            Vector3D tempVecj = tempVeci + jstep * j;
            for (int k = 0; k < divisions; ++k)
            {
                Vector3D thisPoint = tempVecj + kstep * k;
                inside += myPoly.isInside(thisPoint);
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
    int numTiles;
    const int* myTiles = myTopoHelp->getNodeTiles(node, numTiles);
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
    //below logic copied from PNPOLY by Wm. Randolph Franklin, swapped x for y, and slightly rewritten, for the special case of 3 vertices
    bool inside = false;
    for (int j = 2, i = 0; i < 3; ++i)//start with the wraparound case
    {
        if ((m_xyz[i][0] < xyz[0]) != (m_xyz[j][0] < xyz[0]))
        {//if one vertex is on one side of the point in the x direction, and the other is on the other side (equal case is treated as greater)
            int ti, tj;
            if (m_xyz[i][0] < m_xyz[j][0])//reorient the segment consistently to get a consistent answer
            {
                ti = i; tj = j;
            } else {
                ti = j; tj = i;
            }
            if ((m_xyz[ti][1] - m_xyz[tj][1]) / (m_xyz[ti][0] - m_xyz[tj][0]) * (xyz[0] - m_xyz[tj][0]) + m_xyz[tj][1] > xyz[1])
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
