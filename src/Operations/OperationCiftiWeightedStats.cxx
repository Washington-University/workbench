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

#include "OperationCiftiWeightedStats.h"
#include "OperationException.h"

#include "CaretHeap.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationCiftiWeightedStats::getCommandSwitch()
{
    return "-cifti-weighted-stats";
}

AString OperationCiftiWeightedStats::getShortDescription()
{
    return "WEIGHTED STATISTICS ALONG CIFTI COLUMNS";
}

OperationParameters* OperationCiftiWeightedStats::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the input cifti");
    
    OptionalParameter* spatialWeightOpt = ret->createOptionalParameter(2, "-spatial-weights", "use vertex area and voxel volume as weights");
    OptionalParameter* leftAreaSurfOpt = spatialWeightOpt->createOptionalParameter(1, "-left-area-surf", "use a surface for left vertex areas");
    leftAreaSurfOpt->addSurfaceParameter(1, "left-surf", "the left surface to use, areas are in mm^2");
    OptionalParameter* rightAreaSurfOpt = spatialWeightOpt->createOptionalParameter(2, "-right-area-surf", "use a surface for right vertex areas");
    rightAreaSurfOpt->addSurfaceParameter(1, "right-surf", "the right surface to use, areas are in mm^2");
    OptionalParameter* cerebAreaSurfOpt = spatialWeightOpt->createOptionalParameter(3, "-cerebellum-area-surf", "use a surface for cerebellum vertex areas");
    cerebAreaSurfOpt->addSurfaceParameter(1, "cerebellum-surf", "the cerebellum surface to use, areas are in mm^2");
    OptionalParameter* leftAreaMetricOpt = spatialWeightOpt->createOptionalParameter(4, "-left-area-metric", "use a metric file for left vertex areas");
    leftAreaMetricOpt->addMetricParameter(1, "left-metric", "metric file containing left vertex areas");
    OptionalParameter* rightAreaMetricOpt = spatialWeightOpt->createOptionalParameter(5, "-right-area-metric", "use a metric file for right vertex areas");
    rightAreaMetricOpt->addMetricParameter(1, "right-metric", "metric file containing right vertex areas");
    OptionalParameter* cerebAreaMetricOpt = spatialWeightOpt->createOptionalParameter(6, "-cerebellum-area-metric", "use a metric file for cerebellum vertex areas");
    cerebAreaMetricOpt->addMetricParameter(1, "cerebellum-metric", "metric file containing cerebellum vertex areas");
    
    OptionalParameter* ciftiWeightOpt = ret->createOptionalParameter(3, "-cifti-weights", "use a cifti file containing weights");
    ciftiWeightOpt->addCiftiParameter(1, "weight-cifti", "the weights to use, as a cifti file");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "only display output for one column");
    columnOpt->addIntegerParameter(1, "column", "the column to use (1-based)");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "only consider data inside an roi");
    roiOpt->addCiftiParameter(1, "roi-cifti", "the roi, as a cifti file");
    roiOpt->createOptionalParameter(2, "-match-maps", "each column of input uses the corresponding column from the roi file");
    
    ret->createOptionalParameter(6, "-mean", "compute weighted mean");
    
    OptionalParameter* stdevOpt = ret->createOptionalParameter(7, "-stdev", "compute weighted standard deviation");
    stdevOpt->createOptionalParameter(1, "-sample", "estimate population stdev from the sample");
    
    OptionalParameter* percentileOpt = ret->createOptionalParameter(8, "-percentile", "compute weighted percentile");
    percentileOpt->addDoubleParameter(1, "percent", "the percentile to find");
    
    ret->createOptionalParameter(9, "-sum", "compute weighted sum");
    
    ret->createOptionalParameter(10, "-show-map-name", "print map index and name before each output");
    
    ret->setHelpText(
        AString("If the mapping along column is brain models, for each column of the input, the specified operation is done on each surface and across all voxels, and the results are printed separately.  ") +
        "For other mapping types, the operation is done on each column, and one number per map is printed.  " +
        "Exactly one of -spatial-weights or -cifti-weights must be specified.  " +
        "Use -column to only give output for a single column.  " +
        "Use -roi to consider only the data within a region.  " +
        "Exactly one of -mean, -stdev, -percentile or -sum must be specified.\n\n" +
        "Using -sum with -spatial-weights (or with -cifti-weights and a cifti containing weights of similar meaning) is equivalent to integrating with respect to area and volume.  " +
        "When the input is binary ROIs, this will therefore output the area or volume of each ROI."
    );
    return ret;
}

namespace
{
    enum OperationType
    {
        MEAN,
        STDEV,
        SAMPSTDEV,
        PERCENTILE,
        SUM
    };
    
    float doOperation(const float* data, const float* weights, const int64_t& numElements, const OperationType& myop, const float* roiData, const float& argument)
    {//argument is only used for percentile currently
        if (roiData != NULL)
        {
            bool haveData = false;
            for (int64_t i = 0; i < numElements; ++i)
            {
                if (weights[i] > 0.0f)
                {
                    haveData = true;
                    break;
                }
            }
            if (!haveData) throw OperationException("roi column is empty");
        }
        switch(myop)
        {
            case SUM:
            case MEAN:
            case STDEV:
            case SAMPSTDEV://these all start the same way
            {
                double accum = 0.0, weightsum = 0.0;
                for (int64_t i = 0; i < numElements; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        accum += data[i] * weights[i];
                        weightsum += weights[i];
                    }
                }
                if (myop == SUM) return accum;
                const float mean = accum / weightsum;
                if (myop == MEAN) return mean;
                accum = 0.0;
                double weightsum2 = 0.0;//for weighted sample stdev
                for (int64_t i = 0; i < numElements; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        float tempf = data[i] - mean;
                        accum += weights[i] * tempf * tempf;
                        weightsum2 += weights[i] * weights[i];
                    }
                }
                if (myop == STDEV) return sqrt(accum / weightsum);
                CaretAssert(myop == SAMPSTDEV);
                return sqrt(accum / (weightsum - weightsum2 / weightsum));//http://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Weighted_sample_variance
            }
            case PERCENTILE:
            {
                CaretAssert(argument >= 0.0f && argument <= 100.0f);
                CaretSimpleMinHeap<float, float> sorter;
                double weightaccum = 0.0;//double will usually prevent adding weights in a different order from getting a different answer
                for (int64_t i = 0; i < numElements; ++i)
                {
                    if (roiData == NULL || roiData[i] > 0.0f)
                    {
                        if (weights[i] < 0.0f) throw OperationException("negative weights not allowed in weighted percentile");
                        weightaccum += weights[i];
                        sorter.push(weights[i], data[i]);//sort by value, so the key is the data
                    }
                }
                int64_t numUse = sorter.size();
                if (numUse == 1)//would need special handling anyway, so get it early
                {
                    float ret;
                    sorter.top(&ret);
                    return ret;
                }
                float targetWeight = argument / 100.0f * weightaccum;
                float lastData, nextData;
                float lastWeight = sorter.pop(&lastData);
                weightaccum = lastWeight;
                float nextWeight = sorter.top(&nextData);
                int64_t position = 1;//because the first and last sections get special treatment to not have flat ends on the function
                while (weightaccum + nextWeight * 0.5f < targetWeight && sorter.size() > 1)
                {
                    ++position;
                    sorter.pop();
                    weightaccum += nextWeight;
                    lastWeight = nextWeight;
                    lastData = nextData;
                    nextWeight = sorter.top(&nextData);
                }
                if (targetWeight < weightaccum)
                {
                    if (position == 1)
                    {//stretch interpolation at first position to the edge
                        return lastData + (nextData - lastData) * 0.5f * ((targetWeight - weightaccum) / lastWeight + 1.0f);
                    } else {
                        return lastData + (nextData - lastData) * 0.5f * ((targetWeight - weightaccum) / (lastWeight * 0.5f) + 1.0f);
                    }
                } else {
                    if (position == numUse - 1)
                    {//ditto
                        return (lastData + nextData) * 0.5f + (nextData - lastData) * 0.5f * (targetWeight - weightaccum) / nextWeight;
                    } else {
                        return (lastData + nextData) * 0.5f + (nextData - lastData) * 0.5f * (targetWeight - weightaccum) / (nextWeight * 0.5f);
                    }
                }
            }
        }
        CaretAssert(false);//make sure execution never actually reaches end of function
        throw OperationException("internal error in weighted stats");
    }
}

void OperationCiftiWeightedStats::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myInput = myParams->getCifti(1);
    const CiftiXML& myXML = myInput->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw OperationException("only 2D cifti are supported in this command");
    int64_t numCols = myXML.getDimensionLength(CiftiXML::ALONG_ROW);
    int64_t colLength = myXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
    OptionalParameter* spatialWeightOpt = myParams->getOptionalParameter(2);
    OptionalParameter* ciftiWeightOpt = myParams->getOptionalParameter(3);
    if (spatialWeightOpt->m_present == ciftiWeightOpt->m_present)//use == as logical xnor
    {
        throw OperationException("you must use exactly one of -spatial-weights or -cifti-weights");
    }
    vector<float> combinedWeights(colLength);
    if (spatialWeightOpt->m_present)
    {
        if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw OperationException("spatial weights require a brain models mapping");
        CiftiBrainModelsMap myDenseMap = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
        OptionalParameter* leftSurfOpt = spatialWeightOpt->getOptionalParameter(1);
        OptionalParameter* rightSurfOpt = spatialWeightOpt->getOptionalParameter(2);
        OptionalParameter* cerebSurfOpt = spatialWeightOpt->getOptionalParameter(3);
        OptionalParameter* leftMetricOpt = spatialWeightOpt->getOptionalParameter(4);
        OptionalParameter* rightMetricOpt = spatialWeightOpt->getOptionalParameter(5);
        OptionalParameter* cerebMetricOpt = spatialWeightOpt->getOptionalParameter(6);
        if (leftSurfOpt->m_present && leftMetricOpt->m_present) throw OperationException("only one of -left-area-surf and -left-area-metric may be specified");
        if (rightSurfOpt->m_present && rightMetricOpt->m_present) throw OperationException("only one of -right-area-surf and -right-area-metric may be specified");
        if (cerebSurfOpt->m_present && cerebMetricOpt->m_present) throw OperationException("only one of -cerebellum-area-surf and -cerebellum-area-metric may be specified");
        vector<StructureEnum::Enum> surfStructs = myDenseMap.getSurfaceStructureList();
        int numSurf = (int)surfStructs.size();
        for (int i = 0; i < numSurf; ++i)
        {
            OptionalParameter* surfOpt = NULL, *metricOpt = NULL;
            AString structName;
            switch (surfStructs[i])
            {
                case StructureEnum::CORTEX_LEFT:
                    surfOpt = leftSurfOpt;
                    metricOpt = leftMetricOpt;
                    structName = "left";
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    surfOpt = rightSurfOpt;
                    metricOpt = rightMetricOpt;
                    structName = "right";
                    break;
                case StructureEnum::CEREBELLUM:
                    surfOpt = cerebSurfOpt;
                    metricOpt = cerebMetricOpt;
                    structName = "cerebellum";
                    break;
                default:
                    throw OperationException("unsupported surface structure: " + StructureEnum::toName(surfStructs[i]));
            }
            vector<CiftiBrainModelsMap::SurfaceMap> myMap = myDenseMap.getSurfaceMap(surfStructs[i]);
            int mapSize = (int)myMap.size();
            if (surfOpt->m_present)
            {
                SurfaceFile* mySurf = surfOpt->getSurface(1);
                if (mySurf->getNumberOfNodes() != myDenseMap.getSurfaceNumberOfNodes(surfStructs[i]))
                {
                    throw OperationException(structName + " area surface has different number of vertices than cifti file");
                }
                vector<float> surfAreas;
                mySurf->computeNodeAreas(surfAreas);
                for (int i = 0; i < mapSize; ++i)
                {
                    combinedWeights[myMap[i].m_ciftiIndex] = surfAreas[myMap[i].m_surfaceNode];
                }
            } else if (metricOpt->m_present) {
                MetricFile* myMetric = metricOpt->getMetric(1);
                if (myMetric->getNumberOfNodes() != myDenseMap.getSurfaceNumberOfNodes(surfStructs[i]))
                {
                    throw OperationException(structName + " area metric has different number of vertices than cifti file");
                }
                for (int i = 0; i < mapSize; ++i)
                {
                    combinedWeights[myMap[i].m_ciftiIndex] = myMetric->getValue(myMap[i].m_surfaceNode, 0);
                }
            } else {
                throw OperationException("no area data specified for " + structName + " surface");
            }
        }
        if (myDenseMap.hasVolumeData())
        {
            float voxelVolume = myDenseMap.getVolumeSpace().getVoxelVolume();
            vector<CiftiBrainModelsMap::VolumeMap> volMap = myDenseMap.getFullVolumeMap();
            int64_t volMapSize = (int64_t)volMap.size();
            for (int64_t i = 0; i < volMapSize; ++i)
            {
                combinedWeights[volMap[i].m_ciftiIndex] = voxelVolume;
            }
        }
    }
    if (ciftiWeightOpt->m_present)
    {
        CiftiFile* weightCifti = ciftiWeightOpt->getCifti(1);
        if (!myXML.getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(*(weightCifti->getCiftiXML().getMap(CiftiXML::ALONG_COLUMN))))
        {
            throw OperationException("weighting cifti has incompatible mapping along column");
        }
        weightCifti->getColumn(combinedWeights.data(), 0);//since we are only using one column, go ahead and call getColumn while it is on disk
    }
    int64_t useColumn = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(4);
    if (columnOpt->m_present)
    {
        useColumn = columnOpt->getInteger(1) - 1;//1-based indexing convention
        if (useColumn < 0 || useColumn >= numCols) throw OperationException("invalid column specified");
    }
    vector<float> roiData;
    bool matchColumnMode = false;
    CiftiFile* myRoi = NULL;
    int64_t numRoiCols = -1;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getCifti(1);
        if (!myXML.getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(*(myRoi->getCiftiXML().getMap(CiftiXML::ALONG_COLUMN))))
        {
            throw OperationException("roi cifti has incompatible mapping along column");
        }
        roiData.resize(colLength);
        if (roiOpt->getOptionalParameter(2)->m_present)
        {
            if (myXML.getMap(CiftiXML::ALONG_ROW)->getLength() != myRoi->getCiftiXML().getMap(CiftiXML::ALONG_ROW)->getLength())
            {
                throw OperationException("-match-maps specified, but roi has different number of columns than input");
            }
            matchColumnMode = true;
        }
        numRoiCols = myRoi->getCiftiXML().getDimensionLength(CiftiXML::ALONG_ROW);
    }
    bool haveOp = false;
    OperationType myop;
    if (myParams->getOptionalParameter(6)->m_present)
    {
        haveOp = true;
        myop = MEAN;
    }
    OptionalParameter* stdevOpt = myParams->getOptionalParameter(7);
    if (stdevOpt->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        if (stdevOpt->getOptionalParameter(1)->m_present)
        {
            myop = SAMPSTDEV;
        } else {
            myop = STDEV;
        }
    }
    float argument = -1.0f;
    OptionalParameter* percentileOpt = myParams->getOptionalParameter(8);
    if (percentileOpt->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        myop = PERCENTILE;
        argument = percentileOpt->getDouble(1);
        if (!(argument >= 0.0f && argument <= 100.0f)) throw OperationException("percentile must be between 0 and 100");
    }
    if (myParams->getOptionalParameter(9)->m_present)
    {
        if (haveOp) throw OperationException("you may only specify one operation");
        haveOp = true;
        myop = SUM;
    }
    if (!haveOp) throw OperationException("you must specify an operation");
    bool showMapName = myParams->getOptionalParameter(10)->m_present;
    const CiftiMappingType* rowMap = myXML.getMap(CiftiXML::ALONG_ROW);
    vector<float> inColumn(colLength);
    int64_t columnStart, columnEnd;
    if (useColumn == -1)
    {
        myInput->convertToInMemory();//we will be getting all columns, so read it all in first
        if (myRoi != NULL) myRoi->convertToInMemory();//ditto
        columnStart = 0;
        columnEnd = numCols;
    } else {
        if (!matchColumnMode && myRoi != NULL) myRoi->convertToInMemory();//matching maps with one column selected is the only time we don't need the whole ROI file
        columnStart = useColumn;
        columnEnd = useColumn + 1;
    }
    if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::BRAIN_MODELS)
    {
        const CiftiBrainModelsMap& myDenseMap = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
        vector<CiftiBrainModelsMap::ModelInfo> myModels = myDenseMap.getModelInfo();//use this so that we get the start and end indices, so we don't have to use cifti separate
        int numModels = (int)myModels.size();
        for (int64_t i = columnStart; i < columnEnd; ++i)
        {
            myInput->getColumn(inColumn.data(), i);
            if (showMapName)
            {
                cout << AString::number(i + 1) << ":\t" << rowMap->getIndexName(i) << ":" << endl;
            }
            if (matchColumnMode)
            {
                myRoi->getColumn(roiData.data(), i);//retrieve the roi data once per map with match maps
            }
            for (int j = 0; j < numModels; ++j)
            {//first, do the surfaces individually
                if (myModels[j].m_type == CiftiBrainModelsMap::SURFACE)
                {
                    cout << StructureEnum::toName(myModels[j].m_structure) << ":\t";
                    if (roiOpt->m_present)
                    {
                        if (matchColumnMode)
                        {
                            float result = doOperation(inColumn.data() + myModels[j].m_indexStart,
                                                combinedWeights.data() + myModels[j].m_indexStart,
                                                myModels[j].m_indexCount,
                                                myop,
                                                roiData.data() + myModels[j].m_indexStart,
                                                argument);
                            stringstream resultsstr;
                            resultsstr << setprecision(7) << result;
                            cout << resultsstr.str();
                        } else {
                            for (int k = 0; k < numRoiCols; ++k)
                            {
                                myRoi->getColumn(roiData.data(), k);
                                float result = doOperation(inColumn.data() + myModels[j].m_indexStart,
                                                    combinedWeights.data() + myModels[j].m_indexStart,
                                                    myModels[j].m_indexCount,
                                                    myop,
                                                    roiData.data() + myModels[j].m_indexStart,
                                                    argument);
                                stringstream resultsstr;
                                resultsstr << setprecision(7) << result;
                                if (k != 0) cout << "\t";
                                cout << resultsstr.str();
                            }
                        }
                    } else {
                        float result = doOperation(inColumn.data() + myModels[j].m_indexStart,
                                            combinedWeights.data() + myModels[j].m_indexStart,
                                            myModels[j].m_indexCount,
                                            myop,
                                            NULL,
                                            argument);
                        stringstream resultsstr;
                        resultsstr << setprecision(7) << result;
                        cout << resultsstr.str();
                    }
                    cout << endl;
                }
            }//now do volume as one chunk
            vector<CiftiBrainModelsMap::VolumeMap> volMap = myDenseMap.getFullVolumeMap();
            int64_t mapSize = (int64_t)volMap.size();
            if (mapSize > 0)
            {
                vector<float> volData(mapSize), weightVolData(mapSize);
                for (int64_t j = 0; j < mapSize; ++j)
                {
                    volData[j] = inColumn[volMap[j].m_ciftiIndex];
                    weightVolData[j] = combinedWeights[volMap[j].m_ciftiIndex];
                }
                cout << "VOLUME:\t";
                if (roiOpt->m_present)
                {
                    vector<float> roiVolData(mapSize);
                    if (matchColumnMode)
                    {
                        for (int64_t j = 0; j < mapSize; ++j)
                        {
                            roiVolData[j] = roiData[volMap[j].m_ciftiIndex];
                        }
                        float result = doOperation(volData.data(), weightVolData.data(), mapSize, myop, roiVolData.data(), argument);
                        stringstream resultsstr;
                        resultsstr << setprecision(7) << result;
                        cout << resultsstr.str();
                    } else {
                        for (int k = 0; k < numRoiCols; ++k)
                        {
                            myRoi->getColumn(roiData.data(), k);
                            for (int64_t j = 0; j < mapSize; ++j)
                            {
                                roiVolData[j] = roiData[volMap[j].m_ciftiIndex];
                            }
                            float result = doOperation(volData.data(), weightVolData.data(), mapSize, myop, roiVolData.data(), argument);
                            stringstream resultsstr;
                            resultsstr << setprecision(7) << result;
                            if (k != 0) cout << "\t";
                            cout << resultsstr.str();
                        }
                    }
                } else {
                    float result = doOperation(volData.data(), weightVolData.data(), mapSize, myop, NULL, argument);
                    stringstream resultsstr;
                    resultsstr << setprecision(7) << result;
                    cout << resultsstr.str();
                }
                cout << endl;
            }
        }
    } else {
        for (int64_t i = columnStart; i < columnEnd; ++i)
        {
            myInput->getColumn(inColumn.data(), i);
            if (showMapName)
            {
                cout << AString::number(i + 1) << ":\t" << rowMap->getIndexName(i) << ":\t";
            }
            if (roiOpt->m_present)
            {
                if (matchColumnMode)
                {
                    myRoi->getColumn(roiData.data(), i);//retrieve the roi data once per map with match maps
                    float result = doOperation(inColumn.data(), combinedWeights.data(), colLength, myop, roiData.data(), argument);
                    stringstream resultsstr;
                    resultsstr << setprecision(7) << result;
                    cout << resultsstr.str();
                } else {
                    for (int k = 0; k < numRoiCols; ++k)
                    {
                        myRoi->getColumn(roiData.data(), k);
                        float result = doOperation(inColumn.data(), combinedWeights.data(), colLength, myop, roiData.data(), argument);
                        stringstream resultsstr;
                        resultsstr << setprecision(7) << result;
                        if (k != 0) cout << "\t";
                        cout << resultsstr.str();
                    }
                }
            } else {
                float result = doOperation(inColumn.data(), combinedWeights.data(), colLength, myop, NULL, argument);
                stringstream resultsstr;
                resultsstr << setprecision(7) << result;
                cout << resultsstr.str();
            }
            cout << endl;
        }
    }
}
