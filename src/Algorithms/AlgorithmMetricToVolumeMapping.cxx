/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AlgorithmMetricToVolumeMapping.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretOMP.h"
#include "CaretPointLocator.h"
#include "MetricFile.h"
#include "RibbonMappingHelper.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"
#include "VoxelIJK.h"

#include <map>

using namespace caret;
using namespace std;

AString AlgorithmMetricToVolumeMapping::getCommandSwitch()
{
    return "-metric-to-volume-mapping";
}

AString AlgorithmMetricToVolumeMapping::getShortDescription()
{
    return "MAP METRIC FILE TO VOLUME";
}

OperationParameters* AlgorithmMetricToVolumeMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addMetricParameter(1, "metric", "the input metric file");
    
    ret->addSurfaceParameter(2, "surface", "the surface to use coordinates from");
    
    ret->addVolumeParameter(3, "volume-space", "a volume file in the desired output volume space");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output volume file");
    
    OptionalParameter* nearestVertOpt = ret->createOptionalParameter(5, "-nearest-vertex", "use the value from the vertex closest to the voxel center");
    nearestVertOpt->addDoubleParameter(1, "distance", "how far from the surface to map values to voxels, in mm");
    
    OptionalParameter* ribbonOpt = ret->createOptionalParameter(6, "-ribbon-constrained", "use ribbon constrained mapping algorithm");
    ribbonOpt->addSurfaceParameter(1, "inner-surf", "the inner surface of the ribbon");
    ribbonOpt->addSurfaceParameter(2, "outer-surf", "the outer surface of the ribbon");
    OptionalParameter* ribbonSubdivOpt = ribbonOpt->createOptionalParameter(3, "-voxel-subdiv", "voxel divisions while estimating voxel weights");
    ribbonSubdivOpt->addIntegerParameter(1, "subdiv-num", "number of subdivisions, default 3");
    ribbonOpt->createOptionalParameter(4, "-greedy", "instead of antialiasing partial-volumed voxels, put full metric values (legacy behavior)");
    ribbonOpt->createOptionalParameter(5, "-thick-columns", "use overlapping columns (legacy method)");
    
    ret->setHelpText(
        AString("Maps values from a metric file into a volume file.  ") +
        "You must specify exactly one mapping method option.  " +
        "The -nearest-vertex method uses the value from the vertex closest to the voxel center (useful for integer values).  " +
        "The -ribbon-constrained method uses the same method as in -volume-to-surface-mapping, then uses the weights in reverse.  " +
        "Mapping to lower resolutions than the mesh may require a larger -voxel-subdiv value in order to have all of the surface data participate."
    );
    return ret;
}

void AlgorithmMetricToVolumeMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* myMetric = myParams->getMetric(1);
    SurfaceFile* mySurf = myParams->getSurface(2);
    VolumeFile* myTemplateVol = myParams->getVolume(3);
    VolumeFile* myVolOut = myParams->getOutputVolume(4);
    enum Method
    {
        INVALID,
        NEAREST,
        RIBBON
    };
    bool haveMethod = false;
    Method myMethod = INVALID;
    float nearDist = -1.0f;
    OptionalParameter* nearestVertOpt = myParams->getOptionalParameter(5);
    if (nearestVertOpt->m_present)
    {
        myMethod = NEAREST;
        haveMethod = true;
        nearDist = (float)nearestVertOpt->getDouble(1);
        if (nearDist < 0.0f)
        {
            throw AlgorithmException("invalid distance specified");
        }
    }
    SurfaceFile* innerSurf = NULL, *outerSurf = NULL;
    int subDivs = 3;
    bool greedy = false, thick = false;
    OptionalParameter* ribbonOpt = myParams->getOptionalParameter(6);
    if (ribbonOpt->m_present)
    {
        if (haveMethod)
        {
            throw AlgorithmException("more than one mapping method specified");
        }
        myMethod = RIBBON;
        haveMethod = true;
        innerSurf = ribbonOpt->getSurface(1);
        outerSurf = ribbonOpt->getSurface(2);
        OptionalParameter* ribbonSubdivOpt = ribbonOpt->getOptionalParameter(3);
        if (ribbonSubdivOpt->m_present)
        {
            subDivs = (int)ribbonSubdivOpt->getInteger(1);
            if (subDivs < 1)
            {
                throw AlgorithmException("invalid number of subdivisions specified");
            }
        }
        greedy = ribbonOpt->getOptionalParameter(4)->m_present;
        thick = ribbonOpt->getOptionalParameter(5)->m_present;
    }
    if (!haveMethod)
    {
        throw AlgorithmException("no mapping method specified");
    }
    switch (myMethod)
    {
        case NEAREST:
            AlgorithmMetricToVolumeMapping(myProgObj, myMetric, mySurf, myTemplateVol->getVolumeSpace(), myVolOut, nearDist);
            break;
        case RIBBON:
            AlgorithmMetricToVolumeMapping(myProgObj, myMetric, mySurf, myTemplateVol->getVolumeSpace(), myVolOut, innerSurf, outerSurf, subDivs, greedy, thick);
            break;
        case INVALID:
            CaretAssert(0);
            throw AlgorithmException("internal error, tell the developers what you just tried to do");
    }
}

AlgorithmMetricToVolumeMapping::AlgorithmMetricToVolumeMapping(ProgressObject* myProgObj, const MetricFile* myMetric, const SurfaceFile* mySurf, const VolumeSpace& myVolSpace,
                                                                 VolumeFile* myVolOut, const float& nearDist) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myMetric->getNumberOfNodes() != mySurf->getNumberOfNodes())
    {
        throw AlgorithmException("input surface and metric have different number of vertices");
    }
    if (nearDist < 0.0f)
    {
        throw AlgorithmException("invalid distance specified in surface to volume mapping");
    }
    checkStructureMatch(myMetric, mySurf->getStructure(), "input metric file", "the surface has");
    int numCols = myMetric->getNumberOfColumns();
    myVolOut->reinitialize(myVolSpace, numCols);
    const int64_t* dims = myVolSpace.getDims();
    const int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<int32_t> voxelToVertex(frameSize);
    CaretPointer<const CaretPointLocator> myLocator = mySurf->getPointLocator();
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t k = 0; k < dims[2]; ++k)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                float voxelCoord[3];
                myVolSpace.indexToSpace(i, j, k, voxelCoord);
                voxelToVertex[myVolSpace.getIndex(i, j, k)] = myLocator->closestPointLimited(voxelCoord, nearDist);
            }
        }
    }
    vector<float> scratchFrame(frameSize, 0.0f);
    for (int i = 0; i < numCols; ++i)
    {
        const float* metricData = myMetric->getValuePointerForColumn(i);
        for (int64_t v = 0; v < frameSize; ++v)
        {
            if (voxelToVertex[v] >= 0)
            {
                scratchFrame[v] = metricData[voxelToVertex[v]];
            }
        }
        myVolOut->setFrame(scratchFrame.data(), i);
        myVolOut->setMapName(i, myMetric->getMapName(i));
    }
}

AlgorithmMetricToVolumeMapping::AlgorithmMetricToVolumeMapping(ProgressObject* myProgObj, const MetricFile* myMetric, const SurfaceFile* mySurf, const VolumeSpace& myVolSpace,
                                                               VolumeFile* myVolOut, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int& subDivs,
                                                               const bool& greedy, const bool& thickColumn) : AbstractAlgorithm(myProgObj)
{
    int numNodes = mySurf->getNumberOfNodes();
    if (myMetric->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("metric and input surfaces have different number of vertices");
    }
    if (!mySurf->hasNodeCorrespondence(*outerSurf) || !mySurf->hasNodeCorrespondence(*innerSurf))
    {
        throw AlgorithmException("all surfaces must have vertex correspondence");
    }
    if (subDivs < 0.0f)
    {
        throw AlgorithmException("invalid number of subdivisions specified in surface to volume mapping");
    }
    checkStructureMatch(myMetric, mySurf->getStructure(), "input metric file", "the surface has");
    checkStructureMatch(innerSurf, myMetric->getStructure(), "inner surface file", "the metric file has");
    checkStructureMatch(outerSurf, myMetric->getStructure(), "outer surface file", "the metric file has");
    int numCols = myMetric->getNumberOfColumns();
    myVolOut->reinitialize(myVolSpace, numCols);
    vector<vector<VoxelWeight> > forwardWeights;
    RibbonMappingHelper::computeWeightsRibbon(forwardWeights, myVolSpace, innerSurf, outerSurf, NULL, subDivs, !thickColumn);
    map<VoxelIJK, vector<pair<int, float> > > reverseWeights;
    for (int i = 0; i < numNodes; ++i)
    {
        for (int v = 0; v < (int)forwardWeights[i].size(); ++v)
        {//vectors initialize to empty
            reverseWeights[VoxelIJK(forwardWeights[i][v].ijk)].push_back(pair<int, float>(i, forwardWeights[i][v].weight));
        }
    }
    const int64_t* dims = myVolSpace.getDims();
    const int64_t frameSize = dims[0] * dims[1] * dims[2];
    vector<float> scratchFrame(frameSize, 0.0f);
    for (int m = 0; m < numCols; ++m)
    {
        const float* colData = myMetric->getValuePointerForColumn(m);
        for (map<VoxelIJK, vector<pair<int, float> > >::const_iterator iter = reverseWeights.begin(); iter != reverseWeights.end(); ++iter)
        {
            double accum = 0.0, totalWeight = 0.0;
            for (int i = 0; i < (int)iter->second.size(); ++i)
            {
                totalWeight += iter->second[i].second;
                accum += colData[iter->second[i].first] * iter->second[i].second;
            }
            CaretAssert(totalWeight > 0.0);//ribbon mapping should never add weights of 0 to lists
            if (greedy)
            {
                scratchFrame[myVolSpace.getIndex(iter->first.m_ijk)] = accum / totalWeight;
            } else {
                float denom = 1.0f;//thin weights are intended to be space filling without overlapping
                if (thickColumn) denom = 3.0f;//a bit of a hack - ideally, in thick mode every fully covered voxel should have weights sum to 3
                if (totalWeight > denom)
                {
                    denom = totalWeight;//however, the surface contours occasionally occasionally turn the polyhedrons partly inside out, so voxels can sum to more than they theoretically should
                }
                scratchFrame[myVolSpace.getIndex(iter->first.m_ijk)] = accum / denom;
            }
        }
        myVolOut->setFrame(scratchFrame.data(), m);
        myVolOut->setMapName(m, myMetric->getMapName(m));
    }
}

float AlgorithmMetricToVolumeMapping::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricToVolumeMapping::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
