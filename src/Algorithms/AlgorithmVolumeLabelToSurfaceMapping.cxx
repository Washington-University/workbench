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

#include "AlgorithmVolumeLabelToSurfaceMapping.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "RibbonMappingHelper.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include "AlgorithmLabelDilate.h"

#include <cmath>
#include <map>

using namespace caret;
using namespace std;

AString AlgorithmVolumeLabelToSurfaceMapping::getCommandSwitch()
{
    return "-volume-label-to-surface-mapping";
}

AString AlgorithmVolumeLabelToSurfaceMapping::getShortDescription()
{
    return "MAP A LABEL VOLUME TO A SURFACE LABEL FILE";
}

OperationParameters* AlgorithmVolumeLabelToSurfaceMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume", "the volume to map data from");
    
    ret->addSurfaceParameter(2, "surface", "the surface to map the data onto");
    
    ret->addLabelOutputParameter(3, "label-out", "the output gifti label file");
    
    OptionalParameter* ribbonOpt = ret->createOptionalParameter(4, "-ribbon-constrained", "use ribbon constrained mapping algorithm");
    ribbonOpt->addSurfaceParameter(1, "inner-surf", "the inner surface of the ribbon");
    ribbonOpt->addSurfaceParameter(2, "outer-surf", "the outer surface of the ribbon");
    OptionalParameter* roiVol = ribbonOpt->createOptionalParameter(3, "-volume-roi", "use a volume roi");
    roiVol->addVolumeParameter(1, "roi-volume", "the volume file");
    OptionalParameter* ribbonDilOpt = ribbonOpt->createOptionalParameter(6, "-dilate-missing", "use dilation for small vertices that 'missed' the geometry tests");
    ribbonDilOpt->addDoubleParameter(1, "dist", "distance in mm for dilation (can be small, like 1mm)");
    OptionalParameter* ribbonSubdiv = ribbonOpt->createOptionalParameter(4, "-voxel-subdiv", "voxel divisions while estimating voxel weights");
    ribbonSubdiv->addIntegerParameter(1, "subdiv-num", "number of subdivisions, default 3");
    ribbonOpt->createOptionalParameter(5, "-thin-columns", "use non-overlapping polyhedra");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(5, "-subvol-select", "select a single subvolume to map");
    subvolumeSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->setHelpText(
        AString("Map label volume data to a surface.  If -ribbon-constrained is not specified, uses the enclosing voxel method.  ") +
        "The ribbon mapping method constructs a polyhedron from the vertex's neighbors on each " +
        "surface, and estimates the amount of this polyhedron's volume that falls inside any nearby voxels, to use as the weights for a popularity comparison.  " +
        "If -thin-columns is specified, the polyhedron uses the edge midpoints and triangle centroids, so that neighboring vertices do not have overlapping polyhedra.  " +
        "This may require increasing -voxel-subdiv to get enough samples in each voxel to reliably land inside these smaller polyhedra.  "
        "The volume ROI is useful to exclude partial volume effects of voxels the surfaces pass through, and will cause the mapping to ignore " +
        "voxels that don't have a positive value in the mask.  The subdivision number specifies how it approximates the amount of the volume the polyhedron " +
        "intersects, by splitting each voxel into NxNxN pieces, and checking whether the center of each piece is inside the polyhedron.  If you have very large " +
        "voxels, consider increasing this if you get unexpected unlabeled vertices in your output."
    );
    return ret;
}

void AlgorithmVolumeLabelToSurfaceMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* myVolume = myParams->getVolume(1);
    SurfaceFile* mySurface = myParams->getSurface(2);
    LabelFile* myLabelOut = myParams->getOutputLabel(3);
    int64_t mySubVol = -1;
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(5);
    if (subvolumeSelect->m_present)
    {
        mySubVol = myVolume->getMapIndexFromNameOrNumber(subvolumeSelect->getString(1));
        if (mySubVol < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    OptionalParameter* ribbonOpt = myParams->getOptionalParameter(4);//method options last for now, for simplicity
    if (ribbonOpt->m_present)
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
        bool thinColumns = ribbonOpt->getOptionalParameter(5)->m_present;
        float dilateDist = -1.0f;
        OptionalParameter* dilOpt = ribbonOpt->getOptionalParameter(6);
        if (dilOpt->m_present)
        {
            dilateDist = float(dilOpt->getDouble(1));
            if (!(dilateDist >= 0.0f))
            {
                throw AlgorithmException("dilate distance must not be negative or NaN");
            }
        }
        AlgorithmVolumeLabelToSurfaceMapping(myProgObj, myVolume, mySurface, myLabelOut, innerSurf, outerSurf, myRoiVol, subdivisions, thinColumns, mySubVol, dilateDist);
    } else {
        AlgorithmVolumeLabelToSurfaceMapping(myProgObj, myVolume, mySurface, myLabelOut, mySubVol);
    }
}

//enclosing
AlgorithmVolumeLabelToSurfaceMapping::AlgorithmVolumeLabelToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface,
                                                                           LabelFile* myLabelOut, const int64_t& mySubVol) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myVolume->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("input volume must be a label volume");
    }
    if (myVolume->getNumberOfComponents() != 1) throw AlgorithmException("label volumes must not have multiple components per map");
    vector<int64_t> myVolDims;
    myVolume->getDimensions(myVolDims);
    if (mySubVol >= myVolDims[3] || mySubVol < -1)
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    int64_t numColumns;
    if (mySubVol == -1)
    {
        numColumns = myVolDims[3];
    } else {
        numColumns = 1;
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    myLabelOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myLabelOut->setStructure(mySurface->getStructure());
    vector<int32_t> myArray(numNodes);
    if (mySubVol == -1)
    {
        GiftiLabelTable cumulativeTable = *(myVolume->getMapLabelTable(0));//so we don't run into append issues with the "???" label that gets made by GiftiLabelTable's constructor
        for (int64_t i = 0; i < myVolDims[3]; ++i)
        {
            map<int32_t, int32_t> frameRemap;
            if (i > 0)
            {
                const GiftiLabelTable* tempTable = myVolume->getMapLabelTable(i);
                if (tempTable == NULL)
                {
                    throw AlgorithmException("a subvolume is missing a label table");
                }
                frameRemap = cumulativeTable.append(*tempTable);
            }
            AString mapName = myVolume->getMapName(i);
            myLabelOut->setColumnName(i, mapName);
#pragma omp CARET_PARFOR
            for (int64_t node = 0; node < numNodes; ++node)
            {
                int32_t tempKey = (int32_t)floor(myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::ENCLOSING_VOXEL, NULL, i) + 0.5f);
                map<int32_t, int32_t>::iterator iter = frameRemap.find(tempKey);
                if (iter != frameRemap.end())
                {
                    myArray[node] = iter->second;
                } else {
                    myArray[node] = tempKey;//for simplicity, assume all values in the volume file are in the label table
                }
            }
            myLabelOut->setLabelKeysForColumn(i, myArray.data());
        }
        cumulativeTable.setHierarchy(myVolume->getMapLabelTable(0)->getHierarchy()); //just grab the first one instead of trying to merge them, for now
        *(myLabelOut->getLabelTable()) = cumulativeTable;
    } else {
        const GiftiLabelTable* tempTable = myVolume->getMapLabelTable(mySubVol);
        if (tempTable == NULL)
        {
            throw AlgorithmException("specified subvolume is missing a label table");
        }
        *(myLabelOut->getLabelTable()) = *tempTable;
        myLabelOut->getLabelTable()->setHierarchy(myVolume->getMapLabelTable(mySubVol)->getHierarchy());
        myLabelOut->setColumnName(0, myVolume->getMapName(mySubVol));
#pragma omp CARET_PARFOR
        for (int64_t node = 0; node < numNodes; ++node)
        {//for simplicity, assume all values in the volume file are in the label table
            myArray[node] = (int32_t)floor(myVolume->interpolateValue(mySurface->getCoordinate(node), VolumeFile::ENCLOSING_VOXEL, NULL, mySubVol) + 0.5f);
        }
        myLabelOut->setLabelKeysForColumn(0, myArray.data());
    }
}

//ribbon
AlgorithmVolumeLabelToSurfaceMapping::AlgorithmVolumeLabelToSurfaceMapping(ProgressObject* myProgObj, const VolumeFile* myVolume, const SurfaceFile* mySurface, LabelFile* myLabelOut,
                                                                           const SurfaceFile* innerSurf, const SurfaceFile* outerSurf,
                                                                           const VolumeFile* myRoiVol, const int32_t& subdivisions, const bool& thinColumns,
                                                                           const int64_t& mySubVol, const float dilateDist): AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myVolume->getType() != SubvolumeAttributes::LABEL)
    {
        throw AlgorithmException("input volume must be a label volume");
    }
    if (myVolume->getNumberOfComponents() != 1) throw AlgorithmException("label volumes must not have multiple components per map");
    vector<int64_t> myVolDims;
    myVolume->getDimensions(myVolDims);
    if (mySubVol >= myVolDims[3] || mySubVol < -1)
    {
        throw AlgorithmException("invalid subvolume specified");
    }
    int64_t numColumns;
    if (mySubVol == -1)
    {
        numColumns = myVolDims[3];
    } else {
        numColumns = 1;
    }
    int64_t numNodes = mySurface->getNumberOfNodes();
    LabelFile outScratch;
    LabelFile* rawMapping = myLabelOut;
    if (dilateDist >= 0.0f)
    {
        rawMapping = &outScratch;
    }
    rawMapping->setNumberOfNodesAndColumns(numNodes, numColumns);
    rawMapping->setStructure(mySurface->getStructure());
    vector<int32_t> myArray(numNodes);
    vector<float> badVerts(numNodes, 0.0f);
    vector<vector<VoxelWeight> > myWeights;
    const float* roiFrame = NULL;
    if (myRoiVol != NULL) roiFrame = myRoiVol->getFrame();
    RibbonMappingHelper::computeWeightsRibbon(myWeights, myVolume->getVolumeSpace(), innerSurf, outerSurf, roiFrame, subdivisions, thinColumns);
    if (mySubVol == -1)
    {
        GiftiLabelTable cumulativeTable = *(myVolume->getMapLabelTable(0));//so we don't run into append issues with the "???" label that gets made by GiftiLabelTable's constructor
        int32_t unlabeledKey = cumulativeTable.getUnassignedLabelKey();//but, assume the first table has one, because we may need it
        for (int64_t i = 0; i < myVolDims[3]; ++i)
        {
            map<int32_t, int32_t> frameRemap;
            if (i > 0)
            {
                const GiftiLabelTable* tempTable = myVolume->getMapLabelTable(i);
                if (tempTable == NULL)
                {
                    throw AlgorithmException("a subvolume is missing a label table");
                }
                frameRemap = cumulativeTable.append(*tempTable);
            }
            rawMapping->setColumnName(i, myVolume->getMapName(i));
#pragma omp CARET_PARFOR
            for (int64_t node = 0; node < numNodes; ++node)
            {
                const vector<VoxelWeight>& weightRef = myWeights[node];
                map<int32_t, float> totals;
                for (int v = 0; v < (int)weightRef.size(); ++v)
                {
                    int32_t voxKey = (int32_t)floor(myVolume->getValue(weightRef[v].ijk, i) + 0.5f);
                    map<int32_t, float>::iterator iter = totals.find(voxKey);
                    if (iter == totals.end())
                    {//floats don't initialize to 0
                        totals[voxKey] = weightRef[v].weight;
                    } else {
                        totals[voxKey] += weightRef[v].weight;
                    }
                }
                int32_t tempKey = unlabeledKey;
                float bestSum = -1.0f;
                for (map<int32_t, float>::iterator iter = totals.begin(); iter != totals.end(); ++iter)
                {
                    if (iter->second > bestSum)
                    {
                        tempKey = iter->first;
                        bestSum = iter->second;
                    }
                }
                if (i == 0 && weightRef.empty())
                {
                    badVerts[node] = 1.0f;
                }
                map<int32_t, int32_t>::iterator iter = frameRemap.find(tempKey);
                if (iter != frameRemap.end())
                {
                    myArray[node] = iter->second;
                } else {
                    myArray[node] = tempKey;//for simplicity, assume all values in the frame are in the label table
                }
            }
            rawMapping->setLabelKeysForColumn(i, myArray.data());
        }
        *(rawMapping->getLabelTable()) = cumulativeTable;
    } else {
        const GiftiLabelTable* tempTable = myVolume->getMapLabelTable(mySubVol);
        int unlabeledKey = tempTable->getUnassignedLabelKey();
        if (tempTable == NULL)
        {
            throw AlgorithmException("specified subvolume is missing a label table");
        }
        *(rawMapping->getLabelTable()) = *tempTable;
        rawMapping->setColumnName(0, myVolume->getMapName(mySubVol));
#pragma omp CARET_PARFOR
        for (int64_t node = 0; node < numNodes; ++node)
        {//for simplicity, assume all values in the volume file are in the label table
            const vector<VoxelWeight>& weightRef = myWeights[node];
            map<int32_t, float> totals;
            for (int v = 0; v < (int)weightRef.size(); ++v)
            {
                int32_t voxKey = (int32_t)floor(myVolume->getValue(weightRef[v].ijk, mySubVol) + 0.5f);
                map<int32_t, float>::iterator iter = totals.find(voxKey);
                if (iter == totals.end())
                {//floats don't initialize to 0
                    totals[voxKey] = weightRef[v].weight;
                } else {
                    totals[voxKey] += weightRef[v].weight;
                }
            }
            int32_t tempKey = unlabeledKey;
            float bestSum = -1.0f;
            for (map<int32_t, float>::iterator iter = totals.begin(); iter != totals.end(); ++iter)
            {
                if (iter->second > bestSum)
                {
                    tempKey = iter->first;
                    bestSum = iter->second;
                }
            }
            if (weightRef.empty())
            {
                badVerts[node] = 1.0f;
            }
            myArray[node] = tempKey;//for simplicity, assume all values in the frame are in the label table
        }
        rawMapping->setLabelKeysForColumn(0, myArray.data());
    }
    if (dilateDist >= 0.0f)
    {
        CaretAssert(rawMapping != myLabelOut);
        MetricFile badVertsMetric;
        badVertsMetric.setNumberOfNodesAndColumns(numNodes, 1);
        badVertsMetric.setValuesForColumn(0, badVerts.data());
        AlgorithmLabelDilate(NULL, rawMapping, mySurface, dilateDist, myLabelOut, &badVertsMetric);
    }
}

float AlgorithmVolumeLabelToSurfaceMapping::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeLabelToSurfaceMapping::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
