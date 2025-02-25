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

#include "AlgorithmVolumeToSurfaceMapping.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretOMP.h"
#include "FloatMatrix.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include "AlgorithmMetricDilate.h"

//for gaussian weighting
#include "AlgorithmSurfaceToSurface3dDistance.h"
#include "AlgorithmCreateSignedDistanceVolume.h"

#include <cmath>
#include <fstream>

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
    OptionalParameter* roiVolOpt = ribbonOpt->createOptionalParameter(3, "-volume-roi", "use a volume roi");
    roiVolOpt->addVolumeParameter(1, "roi-volume", "the roi volume file");
    roiVolOpt->createOptionalParameter(2, "-weighted", "treat the roi values as weightings rather than binary");
    OptionalParameter* ribbonDilOpt = ribbonOpt->createOptionalParameter(11, "-dilate-missing", "use dilation for small vertices that 'missed' the geometry tests");
    ribbonDilOpt->addDoubleParameter(1, "dist", "distance in mm for dilation (can be small, like 1mm)");
    ribbonDilOpt->createOptionalParameter(2, "-nearest", "use nearest neighbor dilation (mainly useful for integer data)");
    OptionalParameter* ribbonSubdivOpt = ribbonOpt->createOptionalParameter(4, "-voxel-subdiv", "voxel divisions while estimating voxel weights");
    ribbonSubdivOpt->addIntegerParameter(1, "subdiv-num", "number of subdivisions, default 3");
    ribbonOpt->createOptionalParameter(7, "-thin-columns", "use non-overlapping polyhedra");
    OptionalParameter* gaussianOpt = ribbonOpt->createOptionalParameter(8, "-gaussian", "reduce weight to voxels that aren't near <surface>");
    gaussianOpt->addDoubleParameter(1, "scale", "value to multiply the local thickness by, to get the gaussian sigma");
    OptionalParameter* ribbonInterpOpt = ribbonOpt->createOptionalParameter(10, "-interpolate", "instead of a weighted average of voxels, interpolate at subpoints inside the ribbon");
    ribbonInterpOpt->addStringParameter(1, "method", "interpolation method, must be CUBIC, ENCLOSING_VOXEL, or TRILINEAR");
    OptionalParameter* badVertOpt = ribbonOpt->createOptionalParameter(9, "-bad-vertices-out", "output an ROI of which vertices didn't intersect any valid voxels");
    badVertOpt->addMetricOutputParameter(1, "roi-out", "the output metric file of vertices that have no data");
    OptionalParameter* ribbonWeightsOpt = ribbonOpt->createOptionalParameter(5, "-output-weights", "write the voxel weights for a vertex to a volume file");
    ribbonWeightsOpt->addIntegerParameter(1, "vertex", "the vertex number to get the voxel weights for, 0-based");
    ribbonWeightsOpt->addVolumeOutputParameter(2, "weights-out", "volume to write the weights to");
    OptionalParameter* ribbonWeightsTextOpt = ribbonOpt->createOptionalParameter(6, "-output-weights-text", "write the voxel weights for all vertices to a text file");
    ribbonWeightsTextOpt->addStringParameter(1, "text-out", "output - the output text filename");//fake the output formatting
    
    OptionalParameter* myelinStyleOpt = ret->createOptionalParameter(9, "-myelin-style", "use the method from myelin mapping");
    myelinStyleOpt->addVolumeParameter(1, "ribbon-roi", "an roi volume of the cortical ribbon for this hemisphere");
    myelinStyleOpt->addMetricParameter(2, "thickness", "a metric file of cortical thickness");
    myelinStyleOpt->addDoubleParameter(3, "sigma", "gaussian kernel in mm for weighting voxels within range");
    myelinStyleOpt->createOptionalParameter(4, "-legacy-bug", "emulate old v1.2.3 and earlier code that didn't follow a cylinder cutoff");
    
    OptionalParameter* subvolumeSelectOpt = ret->createOptionalParameter(7, "-subvol-select", "select a single subvolume to map");
    subvolumeSelectOpt->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("You must specify exactly one mapping method.  Enclosing voxel uses the value from the voxel the vertex lies inside, while trilinear does a 3D ") + 
        "linear interpolation based on the voxels immediately on each side of the vertex's position." +
        "\n\n" +
        "The ribbon mapping method constructs a polyhedron from the vertex's neighbors on each " +
        "surface, and estimates the amount of this polyhedron's volume that falls inside any nearby voxels, to use as the weights for sampling.  " +
        "If -thin-columns is specified, the polyhedron uses the edge midpoints and triangle centroids, so that neighboring vertices do not have overlapping polyhedra.  " +
        "This may require increasing -voxel-subdiv to get enough samples in each voxel to reliably land inside these smaller polyhedra.  " +
        "The volume ROI is useful to exclude partial volume effects of voxels the surfaces pass through, and will cause the mapping to ignore " +
        "voxels that don't have a positive value in the mask.  The subdivision number specifies how it approximates the amount of the volume the polyhedron " +
        "intersects, by splitting each voxel into NxNxN pieces, and checking whether the center of each piece is inside the polyhedron.  If you have very large " +
        "voxels, consider increasing this if you get zeros in your output.  " +
        "The -gaussian option makes it act more like the myelin method, where the distance of a voxel from <surface> is used to downweight the voxel.  " +
        "The -interpolate suboption, instead of doing a weighted average of voxels, interpolates from the volume at the subdivided points inside the ribbon.  " +
        "If using both -interpolate and the -weighted suboption to -volume-roi, the roi volume weights are linearly interpolated, " +
        "unless the -interpolate method is ENCLOSING_VOXEL, in which case ENCLOSING_VOXEL is also used for sampling the roi volume weights." +
        "\n\n" +
        "The myelin style method uses part of the caret5 myelin mapping command to do the mapping: for each surface vertex, take all voxels that are in a cylinder " +
        "with radius and height equal to cortical thickness, centered on the vertex and aligned with the surface normal, and that are also within the ribbon ROI, " +
        "and apply a gaussian kernel with the specified sigma to them to get the weights to use.  " +
        "The -legacy-bug flag reverts to the unintended behavior present from the initial implementation up to and including v1.2.3, which had only the tangential cutoff " +
        "and a bounding box intended to be larger than where the cylinder cutoff should have been."
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
            bool weightedRoi = false;
            if (roiVol->m_present)
            {
                myRoiVol = roiVol->getVolume(1);
                weightedRoi = roiVol->getOptionalParameter(2)->m_present;
            }
            float dilate = -1.0f;
            bool dilateNearest = false;
            OptionalParameter* ribbonDilOpt = ribbonOpt->getOptionalParameter(11);
            if (ribbonDilOpt->m_present)
            {
                dilate = float(ribbonDilOpt->getDouble(1));
                if (!(dilate >= 0.0f))
                {
                    throw AlgorithmException("dilate distance must not be negative or NaN");
                }
                dilateNearest = ribbonDilOpt->getOptionalParameter(2)->m_present;
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
            bool thinColumns = ribbonOpt->getOptionalParameter(7)->m_present;
            float gaussScale = -1.0f;
            OptionalParameter* gaussianOpt = ribbonOpt->getOptionalParameter(8);
            if (gaussianOpt->m_present)
            {
                gaussScale = (float)gaussianOpt->getDouble(1);
                if (!(gaussScale > 0.0f)) throw AlgorithmException("gaussian scale must be positive");
            }
            bool ribbonInterp = false;
            OptionalParameter* ribbonInterpOpt = ribbonOpt->getOptionalParameter(10);
            if (ribbonInterpOpt->m_present)
            {
                ribbonInterp = true;
                AString interpMethod = ribbonInterpOpt->getString(1);
                if (interpMethod == "CUBIC")
                {
                    volInterpMethod = VolumeFile::CUBIC;
                } else if (interpMethod == "TRILINEAR") {
                    volInterpMethod = VolumeFile::TRILINEAR;
                } else if (interpMethod == "ENCLOSING_VOXEL") {
                    volInterpMethod = VolumeFile::ENCLOSING_VOXEL;
                } else {
                    throw AlgorithmException("unrecognized volume interpolation method");
                }
            }
            MetricFile* badVertices = NULL;
            OptionalParameter* badVertOpt = ribbonOpt->getOptionalParameter(9);
            if (badVertOpt->m_present)
            {
                badVertices = badVertOpt->getOutputMetric(1);
            }
            int weightsOutVertex = -1;
            VolumeFile* weightsOut = NULL;
            OptionalParameter* ribbonWeights = ribbonOpt->getOptionalParameter(5);
            if (ribbonWeights->m_present)
            {
                weightsOutVertex = (int)ribbonWeights->getInteger(1);
                weightsOut = ribbonWeights->getOutputVolume(2);
            }
            OptionalParameter* ribbonWeightsText = ribbonOpt->getOptionalParameter(6);
            if (ribbonInterp)
            {
                if (ribbonWeightsText->m_present || weightsOut != NULL)
                {
                    throw AlgorithmException("-output-weights options are incompatible with -interpolate");
                }
                AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, innerSurf, outerSurf, volInterpMethod, myRoiVol, weightedRoi, subdivisions, thinColumns,
                                                mySubVol, gaussScale, badVertices, dilate, dilateNearest);
            } else {
                AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, innerSurf, outerSurf, myRoiVol, weightedRoi, subdivisions, thinColumns,
                                                mySubVol, gaussScale, badVertices, weightsOutVertex, weightsOut, dilate, dilateNearest);
            }
            if (ribbonWeightsText->m_present)
            {//do this after the algorithm, to let it do the error condition checking
                ofstream outFile(ribbonWeightsText->getString(1).toLocal8Bit().constData());
                if (!outFile) throw AlgorithmException("failed to open output textfile '" + ribbonWeightsText->getString(1) + "'");
                vector<vector<VoxelWeight> > myWeights;
                const float* roiFrame = NULL;
                if (myRoiVol != NULL) roiFrame = myRoiVol->getFrame();
                AlgorithmVolumeToSurfaceMapping::precomputeWeightsRibbon(myWeights, myVolume->getVolumeSpace(), innerSurf, outerSurf, roiFrame, weightedRoi, subdivisions, thinColumns, mySurface, gaussScale);
                for (int i = 0; i < (int)myWeights.size(); ++i)
                {
                    outFile << i << ", " << myWeights[i].size();
                    for (int j = 0; j < (int)myWeights[i].size(); ++j)
                    {
                        for (int v = 0; v < 3; ++v)
                        {
                            outFile << ", " << myWeights[i][j].ijk[v];
                        }
                        outFile << ", " << myWeights[i][j].weight;
                    }
                    outFile << endl;
                }
            }
            break;
        }
        case MYELIN_STYLE:
        {
            VolumeFile* roi = myelinStyleOpt->getVolume(1);
            MetricFile* thickness = myelinStyleOpt->getMetric(2);
            float sigma = (float)myelinStyleOpt->getDouble(3);
            bool oldCutoffBug = myelinStyleOpt->getOptionalParameter(4)->m_present;
            AlgorithmVolumeToSurfaceMapping(myProgObj, myVolume, mySurface, myMetricOut, roi, thickness, sigma, mySubVol, oldCutoffBug);
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
                                                                 const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const VolumeFile* roiVol, const bool roiWeights,
                                                                 const int32_t& subdivisions, const bool& thinColumns, const int64_t& mySubVol, const float& gaussScale, MetricFile* badVertices,
                                                                 const int& weightsOutVertex, VolumeFile* weightsOut, float dilateDist, bool dilateNearest) : AbstractAlgorithm(myProgObj)
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
    MetricFile rawMappingTemp;
    MetricFile* rawMapping = myMetricOut;
    if (dilateDist >= 0.0f)
    {
        rawMapping = &rawMappingTemp;
    }
    rawMapping->setNumberOfNodesAndColumns(numNodes, numColumns);
    rawMapping->setStructure(mySurface->getStructure());
    AlgorithmMetricDilate::Method dilMethod = AlgorithmMetricDilate::WEIGHTED;
    if (dilateNearest)
    {
        dilMethod = AlgorithmMetricDilate::AlgorithmMetricDilate::NEAREST;
    }
    vector<float> badVertScratch;
    MetricFile badVertTemp; //we will need a metric file if we do the dilate step, so always make a metric file
    MetricFile* badVertCompute = &badVertTemp;
    if (badVertices != NULL)
    {
        badVertCompute = badVertices;
    }
    badVertCompute->setNumberOfNodesAndColumns(numNodes, 1);
    badVertCompute->setStructure(mySurface->getStructure());
    badVertScratch.resize(numNodes, 0.0f);
    if (!mySurface->hasNodeCorrespondence(*outerSurf) || !mySurface->hasNodeCorrespondence(*innerSurf))
    {
        throw AlgorithmException("all surfaces must have vertex correspondence");
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
    const float* roiFrame = NULL;
    if (roiVol != NULL) roiFrame = roiVol->getFrame();
    precomputeWeightsRibbon(myWeights, myVolume->getVolumeSpace(), innerSurf, outerSurf, roiFrame, roiWeights, subdivisions, thinColumns, mySurface, gaussScale);
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
                rawMapping->setColumnName(thisCol, metricLabel);
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
                        if (thisCol == 0)
                        {
                            badVertScratch[node] = 1.0f;
                        }
                    }
                }
                rawMapping->setValuesForColumn(thisCol, myScratch);
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
            rawMapping->setColumnName(thisCol, metricLabel);
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
                        badVertScratch[node] = 1.0f;
                    }
            }
            rawMapping->setValuesForColumn(thisCol, myScratch);
        }
    }
    badVertCompute->setValuesForColumn(0, badVertScratch.data());
    if (dilateDist >= 0.0f)
    {
        CaretAssert(rawMapping != myMetricOut);
        AlgorithmMetricDilate(NULL, rawMapping, mySurface, dilateDist, myMetricOut, badVertCompute, NULL, -1, dilMethod);
    }
}

//interpolation ribbon mapping
AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                                                 const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const VolumeFile::InterpType interpType, const VolumeFile* roiVol, const bool roiWeights,
                                                                 const int32_t& subdivisions, const bool& thinColumns, const int64_t& mySubVol, const float& gaussScale,
                                                                 MetricFile* badVertices, float dilateDist, bool dilateNearest) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> myVolDims;
    myVolume->getDimensions(myVolDims);
    if (mySubVol >= myVolDims[3] || mySubVol < -1)
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    if (!mySurface->hasNodeCorrespondence(*outerSurf) || !mySurface->hasNodeCorrespondence(*innerSurf))
    {
        throw AlgorithmException("all surfaces must have vertex correspondence");
    }
    if (roiVol != NULL && !roiVol->matchesVolumeSpace(myVolume))
    {
        throw AlgorithmException("roi volume is not in the same volume space as input volume");
    }
    int64_t startVol = 0, endVol = myVolDims[3];
    if (mySubVol > -1)
    {
        startVol = mySubVol;
        endVol = mySubVol + 1;
    }
    int64_t numColumns = (endVol - startVol) * myVolDims[4];
    int64_t numNodes = mySurface->getNumberOfNodes();
    MetricFile rawMappingTemp;
    MetricFile* rawMapping = myMetricOut;
    if (dilateDist >= 0.0f)
    {
        rawMapping = &rawMappingTemp;
    }
    rawMapping->setNumberOfNodesAndColumns(numNodes, numColumns);
    rawMapping->setStructure(mySurface->getStructure());
    AlgorithmMetricDilate::Method dilMethod = AlgorithmMetricDilate::WEIGHTED;
    if (dilateNearest)
    {
        dilMethod = AlgorithmMetricDilate::AlgorithmMetricDilate::NEAREST;
    }
    vector<float> badVertScratch, myScratchArray(numNodes);
    MetricFile badVertTemp; //we will need a metric file if we do the dilate step, so always make a metric file
    MetricFile* badVertCompute = &badVertTemp;
    if (badVertices != NULL)
    {
        badVertCompute = badVertices;
    }
    badVertCompute->setNumberOfNodesAndColumns(numNodes, 1);
    badVertCompute->setStructure(mySurface->getStructure());
    badVertScratch.resize(numNodes, 0.0f);
    const float* roiFrame = NULL;
    if (roiVol != NULL) roiFrame = roiVol->getFrame();
    vector<vector<PointWeight> > pointList = RibbonMappingHelper::computePointsRibbon(myVolume->getVolumeSpace(), innerSurf, outerSurf, roiFrame, subdivisions, thinColumns);
    CaretPointer<SignedDistanceHelperBase> myHelpBase;
    if (gaussScale > 0.0f)
    {
        myHelpBase.grabNew(new SignedDistanceHelperBase(mySurface));
    }
    MetricFile thickness;
    VolumeFile::InterpType roiInterp = VolumeFile::TRILINEAR;//TODO: should this change based on interpType? be explicit?
    if (interpType == VolumeFile::ENCLOSING_VOXEL)
    {
        roiInterp = VolumeFile::ENCLOSING_VOXEL;
    }
    AlgorithmSurfaceToSurface3dDistance(NULL, innerSurf, outerSurf, &thickness);
    for (int64_t i = startVol; i < endVol; ++i)
    {
        for (int64_t j = 0; j < myVolDims[4]; ++j)
        {
            if (interpType == VolumeFile::CUBIC)
            {
                myVolume->validateSpline(i, j);//to do spline deconvolution in parallel
            }
            int64_t thisCol = (i - startVol) * myVolDims[4] + j;
            AString metricLabel = myVolume->getMapName(i);
            if (myVolDims[4] != 1)
            {
                metricLabel += " component " + AString::number(j);
            }
            rawMapping->setColumnName(thisCol, metricLabel);
#pragma omp CARET_PAR
            {
                CaretPointer<SignedDistanceHelper> distHelp;
                if (gaussScale > 0.0f)
                {
                    distHelp.grabNew(new SignedDistanceHelper(myHelpBase));
                }
#pragma omp CARET_FOR
                for (int64_t node = 0; node < numNodes; ++node)
                {
                    double accum = 0.0, weightAccum = 0.0;
                    float thisThick = thickness.getValue(node, 0);
                    for (auto& point : pointList[node])
                    {
                        float value = myVolume->interpolateValue(point.coord, interpType, NULL, i, j);
                        float weight = point.weight;
                        if (roiWeights)
                        {
                            weight = weight * roiVol->interpolateValue(point.coord, roiInterp);
                        }
                        if (gaussScale > 0.0f)
                        {
                            float dist = distHelp->dist(point.coord, SignedDistanceHelper::EVEN_ODD);//could make a function for unsigned distance to speed this up some
                            float toSquare = dist / (thisThick * gaussScale);
                            weight = weight * exp(-toSquare * toSquare / 2);
                        }
                        accum += value * weight;
                        weightAccum += weight;
                    }
                    if (weightAccum > 0.0)
                    {
                        myScratchArray[node] = accum / weightAccum;
                    } else {
                        myScratchArray[node] = 0.0f;
                        badVertScratch[node] = 1.0f;
                    }
                }
            }
            rawMapping->setValuesForColumn(thisCol, myScratchArray.data());
            if (interpType == VolumeFile::CUBIC)
            {
                myVolume->freeSpline(i, j);//release memory we no longer need, if we allocated it
            }
        }
    }
    badVertCompute->setValuesForColumn(0, badVertScratch.data());
    if (dilateDist >= 0.0f)
    {
        CaretAssert(rawMapping != myMetricOut);
        AlgorithmMetricDilate(NULL, rawMapping, mySurface, dilateDist, myMetricOut, badVertCompute, NULL, -1, dilMethod);
    }
}

void AlgorithmVolumeToSurfaceMapping::precomputeWeightsRibbon(vector<vector<VoxelWeight> >& myWeights, const VolumeSpace& volSpace,
                                                              const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const float* roiFrame, const bool roiWeights,
                                                              const int& subdivisions, const bool& thinColumns, const SurfaceFile* gaussSurf, const float& gaussScale)
{
    RibbonMappingHelper::computeWeightsRibbon(myWeights, volSpace, innerSurf, outerSurf, roiFrame, subdivisions, thinColumns);
    const int numNodes = innerSurf->getNumberOfNodes();
    if (roiWeights)
    {
        CaretAssert(roiFrame != NULL);
        for (int i = 0; i < numNodes; ++i)
        {//modify the weights from the ribbon helper
            vector<VoxelWeight>& nodeWeights = myWeights[i];
            for (int j = 0; j < (int)nodeWeights.size(); ++j)
            {
                nodeWeights[j].weight *= roiFrame[volSpace.getIndex(nodeWeights[j].ijk)];
            }
        }
    }
    if (gaussScale > 0.0f)
    {
        VolumeFile signedDistVol;
        MetricFile thickness;
        AlgorithmSurfaceToSurface3dDistance(NULL, innerSurf, outerSurf, &thickness);
        float maxThick = thickness.getValue(0, 0);
        for (int i = 1; i < numNodes; ++i)
        {
            float thisThick = thickness.getValue(i, 0);
            if (thisThick > maxThick) maxThick = thisThick;
        }
        signedDistVol.reinitialize(volSpace);
        AlgorithmCreateSignedDistanceVolume(NULL, gaussSurf, &signedDistVol, NULL, maxThick * gaussScale * 3.0f, maxThick * gaussScale * 3.0f);//if we somehow have a voxel really far away compared to thickness, treat it as at least 3 sigma
        for (int i = 0; i < numNodes; ++i)
        {//modify the weights from the ribbon helper
            vector<VoxelWeight>& nodeWeights = myWeights[i];
            for (int j = 0; j < (int)nodeWeights.size(); ++j)
            {
                float toSquare = signedDistVol.getValue(nodeWeights[j].ijk) / (thickness.getValue(i, 0) * gaussScale);//negatives are fine, we are going to square it
                nodeWeights[j].weight *= exp(-toSquare * toSquare / 2);
            }
        }
    }
}

//myelin style mapping
AlgorithmVolumeToSurfaceMapping::AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, MetricFile* myMetricOut,
                                                                 const VolumeFile* roiVol, const MetricFile* thickness, const float& sigma, const int64_t& mySubVol, const bool& oldCutoffBug): AbstractAlgorithm(myProgObj)
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
    precomputeWeightsMyelin(myWeights, mySurface, roiVol, thickness, sigma, oldCutoffBug);
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
                metricLabel += " myelin style";
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
            metricLabel += " myelin style";
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

void AlgorithmVolumeToSurfaceMapping::precomputeWeightsMyelin(vector<vector<VoxelWeight> >& myWeights, const SurfaceFile* mySurface, const VolumeFile* roiVol,
                                                              const MetricFile* thickness, const float& sigma, const bool& oldCutoffBug)
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
        float cylinderCorner = nodeThick * sqrt(5.0f) / 2.0f;
        if (oldCutoffBug)
        {
            cylinderCorner = nodeThick;//old code didn't use a large enough box to always cover the cylinder corner
        }
        int64_t min[3], max[3];
        for (int i = 0; i < 3; ++i)
        {
            min[i] = (int)ceil(nodeIndices[i] - range[i] * cylinderCorner);
            if (min[i] < 0) min[i] = 0;
            max[i] = (int)floor(nodeIndices[i] + range[i] * cylinderCorner) + 1;
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
                    if (abs(nodenormal.dot(delta)) < 0.5f * nodeThick && (oldCutoffBug || nodenormal.cross(delta).length() < nodeThick) && roiVol->getValue(ijk) > 0.0f)
                    {//old code ignored the tangential distance, only using the box edge
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

float AlgorithmVolumeToSurfaceMapping::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeToSurfaceMapping::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
