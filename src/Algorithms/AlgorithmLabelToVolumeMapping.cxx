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

#include "AlgorithmLabelToVolumeMapping.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretOMP.h"
#include "CaretPointLocator.h"
#include "LabelFile.h"
#include "GiftiLabelTable.h"
#include "RibbonMappingHelper.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmLabelToVolumeMapping::getCommandSwitch()
{
    return "-label-to-volume-mapping";
}

AString AlgorithmLabelToVolumeMapping::getShortDescription()
{
    return "MAP LABEL FILE TO VOLUME";
}

OperationParameters* AlgorithmLabelToVolumeMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelParameter(1, "label", "the input label file");
    
    ret->addSurfaceParameter(2, "surface", "the surface to use coordinates from");
    
    ret->addVolumeParameter(3, "volume-space", "a volume file in the desired output volume space");
    
    ret->addVolumeOutputParameter(4, "volume-out", "the output volume file");
    
    OptionalParameter* nearestVertOpt = ret->createOptionalParameter(5, "-nearest-vertex", "use the label from the vertex closest to the voxel center");
    nearestVertOpt->addDoubleParameter(1, "distance", "how far from the surface to map labels to voxels, in mm");
    
    OptionalParameter* ribbonOpt = ret->createOptionalParameter(6, "-ribbon-constrained", "use ribbon constrained mapping algorithm");
    ribbonOpt->addSurfaceParameter(1, "inner-surf", "the inner surface of the ribbon");
    ribbonOpt->addSurfaceParameter(2, "outer-surf", "the outer surface of the ribbon");
    OptionalParameter* ribbonSubdivOpt = ribbonOpt->createOptionalParameter(3, "-voxel-subdiv", "voxel divisions while estimating voxel weights");
    ribbonSubdivOpt->addIntegerParameter(1, "subdiv-num", "number of subdivisions, default 3");
    ribbonOpt->createOptionalParameter(4, "-greedy", "also put labels in voxels with less than 50% partial volume (legacy behavior)");
    ribbonOpt->createOptionalParameter(5, "-thick-columns", "use overlapping columns (legacy method)");
    
    ret->setHelpText(
        AString("Maps labels from a gifti label file into a volume file.  ") +
        "You must specify exactly one mapping method option.  " +
        "The -nearest-vertex method uses the label from the vertex closest to the voxel center.  " +
        "The -ribbon-constrained method uses the same method as in -volume-to-surface-mapping, then uses the weights in reverse, with popularity logic to decide on a label to use."
    );
    return ret;
}

void AlgorithmLabelToVolumeMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
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
            AlgorithmLabelToVolumeMapping(myProgObj, myLabel, mySurf, myTemplateVol->getVolumeSpace(), myVolOut, nearDist);
            break;
        case RIBBON:
            AlgorithmLabelToVolumeMapping(myProgObj, myLabel, mySurf, myTemplateVol->getVolumeSpace(), myVolOut, innerSurf, outerSurf, subDivs, greedy, thick);
            break;
        case INVALID:
            CaretAssert(0);
            throw AlgorithmException("internal error, tell the developers what you just tried to do");
    }
}

AlgorithmLabelToVolumeMapping::AlgorithmLabelToVolumeMapping(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, const VolumeSpace& myVolSpace,
                                                             VolumeFile* myVolOut, const float& nearDist) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myLabel->getNumberOfNodes() != mySurf->getNumberOfNodes())
    {
        throw AlgorithmException("input surface and label file have different number of vertices");
    }
    if (nearDist < 0.0f)
    {
        throw AlgorithmException("invalid distance specified in surface to volume mapping");
    }
    checkStructureMatch(myLabel, mySurf->getStructure(), "input label file", "the surface has");
    int numCols = myLabel->getNumberOfColumns();
    myVolOut->reinitialize(myVolSpace, numCols, 1, SubvolumeAttributes::LABEL);
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
        const int32_t* labelData = myLabel->getLabelKeyPointerForColumn(i);
        for (int64_t v = 0; v < frameSize; ++v)
        {
            if (voxelToVertex[v] >= 0)
            {
                scratchFrame[v] = labelData[voxelToVertex[v]];
            }
        }
        myVolOut->setFrame(scratchFrame.data(), i);
        *(myVolOut->getMapLabelTable(i)) = *(myLabel->getLabelTable());
        myVolOut->setMapName(i, myLabel->getMapName(i));
    }
}

AlgorithmLabelToVolumeMapping::AlgorithmLabelToVolumeMapping(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, const VolumeSpace& myVolSpace,
                                                                 VolumeFile* myVolOut, const SurfaceFile* innerSurf, const SurfaceFile* outerSurf, const int& subDivs,
                                                                 const bool& greedy, const bool& thickColumn) : AbstractAlgorithm(myProgObj)
{
    int numNodes = mySurf->getNumberOfNodes();
    if (myLabel->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("label file and input surfaces have different number of vertices");
    }
    if (!mySurf->hasNodeCorrespondence(*outerSurf) || !mySurf->hasNodeCorrespondence(*innerSurf))
    {
        throw AlgorithmException("all surfaces must have vertex correspondence");
    }
    if (subDivs < 0.0f)
    {
        throw AlgorithmException("invalid number of subdivisions specified in surface to volume mapping");
    }
    checkStructureMatch(myLabel, mySurf->getStructure(), "input label file", "the surface has");
    checkStructureMatch(innerSurf, myLabel->getStructure(), "inner surface file", "the label file has");
    checkStructureMatch(outerSurf, myLabel->getStructure(), "outer surface file", "the label file has");
    int numCols = myLabel->getNumberOfColumns();
    myVolOut->reinitialize(myVolSpace, numCols, 1, SubvolumeAttributes::LABEL);
    vector<vector<VoxelWeight> > forwardWeights;
    RibbonMappingHelper::computeWeightsRibbon(forwardWeights, myVolSpace, innerSurf, outerSurf, NULL, subDivs);
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
    const int32_t unlabeledVal = myLabel->getLabelTable()->getUnassignedLabelKey();
    vector<float> scratchFrame(frameSize, unlabeledVal);
    for (int m = 0; m < numCols; ++m)
    {
        const int32_t* colData = myLabel->getLabelKeyPointerForColumn(m);
        for (map<VoxelIJK, vector<pair<int, float> > >::const_iterator iter = reverseWeights.begin(); iter != reverseWeights.end(); ++iter)
        {
            double totalWeight = 0.0;
            map<int32_t, float> totals;
            const vector<pair<int, float> >& vertWeightsRef = iter->second;
            for (int i = 0; i < (int)vertWeightsRef.size(); ++i)
            {
                totalWeight += vertWeightsRef[i].second;
                map<int32_t, float>::iterator iter2 = totals.find(colData[vertWeightsRef[i].first]);
                if (iter2 == totals.end())
                {//because floats don't initialize to 0
                    totals[colData[vertWeightsRef[i].first]] = vertWeightsRef[i].second;
                } else {
                    iter2->second += vertWeightsRef[i].second;
                }
            }
            float bestWeight = -1.0f;
            int32_t bestLabel = unlabeledVal;
            bool skipLoop = false;
            if (!greedy)
            {
                if (thickColumn)
                {
                    skipLoop = (totalWeight < 1.5);//slight hack: the thick column method basically counts every triangle three times
                } else {
                    skipLoop = (totalWeight < 0.5);
                }
            }
            if (!skipLoop)
            {
                for (map<int32_t, float>::iterator iter2 = totals.begin(); iter2 != totals.end(); ++iter2)
                {
                    if (iter2->second > bestWeight)
                    {
                        bestWeight = iter2->second;
                        bestLabel = iter2->first;
                    }
                }
            }
            scratchFrame[myVolSpace.getIndex(iter->first.m_ijk)] = bestLabel;
        }
        myVolOut->setFrame(scratchFrame.data(), m);
        *(myVolOut->getMapLabelTable(m)) = *(myLabel->getLabelTable());
        myVolOut->setMapName(m, myLabel->getMapName(m));
    }
}

float AlgorithmLabelToVolumeMapping::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelToVolumeMapping::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
