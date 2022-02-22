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

#include "AlgorithmCiftiParcellate.h"
#include "AlgorithmException.h"
#include "CaretLogger.h"
#include "CiftiFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "MetricFile.h"
#include "MultiDimIterator.h"
#include "ReductionOperation.h"
#include "SurfaceFile.h"

#include <cmath>
#include <map>

using namespace caret;
using namespace std;

AString AlgorithmCiftiParcellate::getCommandSwitch()
{
    return "-cifti-parcellate";
}

AString AlgorithmCiftiParcellate::getShortDescription()
{
    return "PARCELLATE A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiParcellate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to parcellate");
    
    ret->addCiftiParameter(2, "cifti-label", "a cifti label file to use for the parcellation");
    
    ret->addStringParameter(3, "direction", "which mapping to parcellate (integer, ROW, or COLUMN)");
    
    ret->addCiftiOutputParameter(4, "cifti-out", "output cifti file");
    
    OptionalParameter* spatialWeightOpt = ret->createOptionalParameter(5, "-spatial-weights", "use voxel volume and either vertex areas or metric files as weights");
    OptionalParameter* leftAreaSurfOpt = spatialWeightOpt->createOptionalParameter(1, "-left-area-surf", "use a surface for left vertex areas");
    leftAreaSurfOpt->addSurfaceParameter(1, "left-surf", "the left surface to use, areas are in mm^2");
    OptionalParameter* rightAreaSurfOpt = spatialWeightOpt->createOptionalParameter(2, "-right-area-surf", "use a surface for right vertex areas");
    rightAreaSurfOpt->addSurfaceParameter(1, "right-surf", "the right surface to use, areas are in mm^2");
    OptionalParameter* cerebAreaSurfOpt = spatialWeightOpt->createOptionalParameter(3, "-cerebellum-area-surf", "use a surface for cerebellum vertex areas");
    cerebAreaSurfOpt->addSurfaceParameter(1, "cerebellum-surf", "the cerebellum surface to use, areas are in mm^2");
    OptionalParameter* leftAreaMetricOpt = spatialWeightOpt->createOptionalParameter(4, "-left-area-metric", "use a metric file for left vertex weights");
    leftAreaMetricOpt->addMetricParameter(1, "left-metric", "metric file containing left vertex weights");
    OptionalParameter* rightAreaMetricOpt = spatialWeightOpt->createOptionalParameter(5, "-right-area-metric", "use a metric file for right vertex weights");
    rightAreaMetricOpt->addMetricParameter(1, "right-metric", "metric file containing right vertex weights");
    OptionalParameter* cerebAreaMetricOpt = spatialWeightOpt->createOptionalParameter(6, "-cerebellum-area-metric", "use a metric file for cerebellum vertex weights");
    cerebAreaMetricOpt->addMetricParameter(1, "cerebellum-metric", "metric file containing cerebellum vertex weights");
    
    OptionalParameter* ciftiWeightOpt = ret->createOptionalParameter(6, "-cifti-weights", "use a cifti file containing weights");
    ciftiWeightOpt->addCiftiParameter(1, "weight-cifti", "the weights to use, as a cifti file");
    
    OptionalParameter* methodOpt = ret->createOptionalParameter(7, "-method", "specify method of parcellation (default MEAN, or MODE if label data)");
    methodOpt->addStringParameter(1, "method", "the method to use to assign parcel values from the values of member brainordinates");

    OptionalParameter* excludeOpt = ret->createOptionalParameter(8, "-exclude-outliers", "exclude non-numeric values and outliers from each parcel by standard deviation");
    excludeOpt->addDoubleParameter(1, "sigma-below", "number of standard deviations below the mean to include");
    excludeOpt->addDoubleParameter(2, "sigma-above", "number of standard deviations above the mean to include");
    
    ret->createOptionalParameter(9, "-only-numeric", "exclude non-numeric values");
    
    OptionalParameter* emptyValOpt = ret->createOptionalParameter(11, "-fill-value", "specify value to use in empty parcels (default 0)");
    emptyValOpt->addDoubleParameter(1, "value", "the value to fill empty parcels with");
    OptionalParameter* emptyRoiOpt = ret->createOptionalParameter(12, "-nonempty-mask-out", "output a matching pscalar file that has 0s in empty parcels, and 1s elsewhere");
    emptyRoiOpt->addCiftiOutputParameter(1, "mask-out", "the output mask file");
    
    ret->createOptionalParameter(13, "-legacy-mode", "use the old behavior, parcels are defined by the intersection between labels and valid data, and empty parcels are discarded");
    
    ret->createOptionalParameter(10, "-include-empty", "deprecated: now the default behavior");

    ret->setHelpText(
        AString("Each label (other than the unlabeled key) in the cifti label file will be treated as a parcel, and all rows or columns of data within the parcel ") +
        "are averaged together to form the parcel's output row or column.  " +
        "If -legacy-mode is specified, parcels will be defined as the overlap between a label and the data, with no errors for missing data vertices or voxels, and empty parcels discarded.  " +
        CiftiXML::directionFromStringExplanation() + "  " +
        "For dtseries or dscalar, use COLUMN.  " +
        "If you are parcellating a dconn in both directions, parcellating by ROW first will use much less memory.\n\n" +
        "NOTE: the parcels in the output file are sorted by the numeric label keys, in ascending order.\n\n" +
        "The parameter to the -method option must be one of the following:\n\n" + ReductionOperation::getHelpInfo() +
        "\nThe -*-weights options are mutually exclusive and may only be used with MEAN (default), SUM, STDEV, SAMPSTDEV, VARIANCE, MEDIAN, or MODE (default for label data)."
    );
    return ret;
}

void AlgorithmCiftiParcellate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiIn = myParams->getCifti(1);
    CiftiFile* myCiftiLabel = myParams->getCifti(2);
    int direction = CiftiXML::directionFromString(myParams->getString(3));
    CiftiFile* myCiftiOut = myParams->getOutputCifti(4);
    const CiftiXML& myXML = myCiftiIn->getCiftiXML();
    vector<int64_t> dims = myXML.getDimensions();
    ReductionEnum::Enum method = ReductionEnum::MEAN;
    for (int i = 0; i < (int)dims.size(); ++i)
    {
        if (myXML.getMappingType(i) == CiftiMappingType::LABELS)
        {
            method = ReductionEnum::MODE;
            break;
        }
    }
    OptionalParameter* methodOpt = myParams->getOptionalParameter(7);
    if (methodOpt->m_present)
    {
        bool ok = false;
        method = ReductionEnum::fromName(methodOpt->getString(1), &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized method string '" + methodOpt->getString(1) + "'");
        }
    }
    bool onlyNumeric = myParams->getOptionalParameter(9)->m_present;
    float excludeLow = -1.0f, excludeHigh = -1.0f;
    OptionalParameter* excludeOpt = myParams->getOptionalParameter(8);
    if (excludeOpt->m_present)
    {
        if (onlyNumeric) CaretLogWarning("-only-numeric is redundant when -exclude-outliers is specified");
        excludeLow = (float)excludeOpt->getDouble(1);
        excludeHigh = (float)excludeOpt->getDouble(2);
        if (!(excludeLow > 0.0f && excludeHigh > 0.0f)) throw AlgorithmException("exclusion sigmas must be positive");
    }
    /*OptionalParameter* emptyOpt = */myParams->getOptionalParameter(10);//deprecated, but we need to "get" it to avoid a debug warning of an ignored option
    float emptyFillValue = 0.0f;
    OptionalParameter* emptyValOpt = myParams->getOptionalParameter(11);
    if (emptyValOpt->m_present)
    {
        emptyFillValue = emptyValOpt->getDouble(1);
    }
    CiftiFile* emptyMaskOut = NULL;
    OptionalParameter* emptyRoiOpt = myParams->getOptionalParameter(12);
    if (emptyRoiOpt->m_present)
    {
        emptyMaskOut = emptyRoiOpt->getOutputCifti(1);
    }
    bool legacyMode = myParams->getOptionalParameter(13)->m_present;
    OptionalParameter* spatialWeightOpt = myParams->getOptionalParameter(5);
    OptionalParameter* ciftiWeightOpt = myParams->getOptionalParameter(6);
    if (spatialWeightOpt->m_present && ciftiWeightOpt->m_present)
    {
        throw AlgorithmException("only one of -spatial-weights and -cifti-weights may be specified");
    }
    if (spatialWeightOpt->m_present)
    {
        if (direction >= myXML.getNumberOfDimensions()) throw AlgorithmException("input cifti file does not have the specified dimension");
        if (myXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("input cifti file does not have brain models mapping type in specified direction");
        CiftiBrainModelsMap myDenseMap = myXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
        OptionalParameter* leftSurfOpt = spatialWeightOpt->getOptionalParameter(1);
        OptionalParameter* rightSurfOpt = spatialWeightOpt->getOptionalParameter(2);
        OptionalParameter* cerebSurfOpt = spatialWeightOpt->getOptionalParameter(3);
        OptionalParameter* leftMetricOpt = spatialWeightOpt->getOptionalParameter(4);
        OptionalParameter* rightMetricOpt = spatialWeightOpt->getOptionalParameter(5);
        OptionalParameter* cerebMetricOpt = spatialWeightOpt->getOptionalParameter(6);
        if (leftSurfOpt->m_present && leftMetricOpt->m_present) throw AlgorithmException("only one of -left-area-surf and -left-area-metric may be specified");
        if (rightSurfOpt->m_present && rightMetricOpt->m_present) throw AlgorithmException("only one of -right-area-surf and -right-area-metric may be specified");
        if (cerebSurfOpt->m_present && cerebMetricOpt->m_present) throw AlgorithmException("only one of -cerebellum-area-surf and -cerebellum-area-metric may be specified");
        MetricFile leftStore, rightStore, cerebStore;
        const MetricFile* leftWeights = NULL, *rightWeights = NULL, *cerebWeights = NULL;
        for (int i = 0; i < 3; ++i)
        {
            MetricFile* thisStore = NULL;
            const MetricFile** thisWeights = NULL;
            OptionalParameter* thisSurfOpt = NULL, *thisMetricOpt = NULL;
            switch (i)
            {
                case 0:
                    thisStore = &leftStore;
                    thisWeights = &leftWeights;
                    thisSurfOpt = leftSurfOpt;
                    thisMetricOpt = leftMetricOpt;
                    break;
                case 1:
                    thisStore = &rightStore;
                    thisWeights = &rightWeights;
                    thisSurfOpt = rightSurfOpt;
                    thisMetricOpt = rightMetricOpt;
                    break;
                case 2:
                    thisStore = &cerebStore;
                    thisWeights = &cerebWeights;
                    thisSurfOpt = cerebSurfOpt;
                    thisMetricOpt = cerebMetricOpt;
                    break;
            }
            if (thisMetricOpt->m_present)
            {
                *thisWeights = thisMetricOpt->getMetric(1);
            }
            if (thisSurfOpt->m_present)
            {
                SurfaceFile* thisSurf = thisSurfOpt->getSurface(1);
                thisStore->setNumberOfNodesAndColumns(thisSurf->getNumberOfNodes(), 1);
                thisStore->setStructure(thisSurf->getStructure());
                vector<float> vertAreas;
                thisSurf->computeNodeAreas(vertAreas);
                thisStore->setValuesForColumn(0, vertAreas.data());
                *thisWeights = thisStore;
            }
        }
        AlgorithmCiftiParcellate(myProgObj, myCiftiIn, myCiftiLabel, direction, myCiftiOut,
                                 leftWeights, rightWeights, cerebWeights,
                                 method, excludeLow, excludeHigh, onlyNumeric,
                                 legacyMode, emptyFillValue, emptyMaskOut);
        return;
    }
    if (ciftiWeightOpt->m_present)
    {
        AlgorithmCiftiParcellate(myProgObj, myCiftiIn, myCiftiLabel, direction, myCiftiOut,
                                 ciftiWeightOpt->getCifti(1),
                                 method, excludeLow, excludeHigh, onlyNumeric,
                                 legacyMode, emptyFillValue, emptyMaskOut);
        return;
    }
    AlgorithmCiftiParcellate(myProgObj, myCiftiIn, myCiftiLabel, direction, myCiftiOut,
                             method, excludeLow, excludeHigh, onlyNumeric,
                             legacyMode, emptyFillValue, emptyMaskOut);
}

AlgorithmCiftiParcellate::AlgorithmCiftiParcellate(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const CiftiFile* myCiftiLabel, const int& direction, CiftiFile* myCiftiOut,
                                                   const ReductionEnum::Enum& method, const float& excludeLow, const float& excludeHigh, const bool& onlyNumeric,
                                                   const bool& legacyMode, const float& emptyFillVal, CiftiFile* emptyMaskOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    const CiftiXML& myLabelXML = myCiftiLabel->getCiftiXML();
    vector<int64_t> dims = myInputXML.getDimensions();
    if (direction >= (int)dims.size()) throw AlgorithmException("specified direction doesn't exist in input file");
    if (myInputXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti file does not have brain models mapping type in specified direction");
    }
    if (myLabelXML.getNumberOfDimensions() != 2 ||
        myLabelXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS ||
        myLabelXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti label file has the wrong mapping types");
    }
    const CiftiBrainModelsMap& inputDense = myInputXML.getBrainModelsMap(direction);
    const CiftiBrainModelsMap& labelDense = myLabelXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    if (inputDense.hasVolumeData())
    {//don't check volume space if direction doesn't have volume data
        if (labelDense.hasVolumeData() && !inputDense.getVolumeSpace().matches(labelDense.getVolumeSpace()))
        {
            throw AlgorithmException("input cifti files must have the same volume space");
        }
    }
    vector<int> indexToParcel;
    CiftiXML myOutXML = myInputXML;
    CiftiParcelsMap outParcelMap = parcellateMapping(myCiftiLabel, inputDense, indexToParcel, legacyMode);
    int numParcels = outParcelMap.getLength();
    if (numParcels < 1)
    {
        throw AlgorithmException("no parcels found, output file would be empty, aborting");
    }
    myOutXML.setMap(direction, outParcelMap);
    myCiftiOut->setCiftiXML(myOutXML);
    int64_t numCols = myInputXML.getDimensionLength(CiftiXML::ALONG_ROW);
    vector<float> scratchRow(numCols);
    vector<int64_t> parcelCounts(numParcels, 0);
    for (int64_t j = 0; j < (int64_t)indexToParcel.size(); ++j)
    {
        int parcel = indexToParcel[j];
        CaretAssert(parcel > -2 && parcel < numParcels);
        if (parcel != -1)
        {
            ++parcelCounts[parcel];
        }
    }
    if (emptyMaskOut != NULL)
    {
        CiftiXML maskOutXML;
        maskOutXML.setNumberOfDimensions(2);
        maskOutXML.setMap(CiftiXML::ALONG_COLUMN, outParcelMap);
        CiftiScalarsMap maskNameMap;
        maskNameMap.setLength(1);
        maskNameMap.setMapName(0, "parcel not empty");
        maskOutXML.setMap(CiftiXML::ALONG_ROW, maskNameMap);
        emptyMaskOut->setCiftiXML(maskOutXML);
        vector<float> emptyMaskData(numParcels, 1.0f);
        for (int i = 0; i < numParcels; ++i)
        {
            if (parcelCounts[i] == 0)
            {
                emptyMaskData[i] = 0.0f;
            }
        }
        emptyMaskOut->setColumn(emptyMaskData.data(), 0);
    }
    bool isLabel = false;
    int labelDir = -1;
    for (int i = 0; i < (int)dims.size(); ++i)
    {
        if (myInputXML.getMappingType(i) == CiftiMappingType::LABELS)
        {
            isLabel = true;
            labelDir = i;
            break;//there should never be more than one dimension with LABEL type, and if there is, just use the first one, i guess...
        }
    }
    if (isLabel && method != ReductionEnum::MODE)
    {
        CaretLogWarning(ReductionEnum::toName(method) + " reduction requested while parcellating label data");
    }
    if (direction == CiftiXML::ALONG_ROW)
    {
        vector<float> scratchOutRow(numParcels);
        vector<vector<float> > parcelData(numParcels);//float so we can use ReductionOperation
        for (int j = 0; j < numParcels; ++j)
        {
            parcelData[j].reserve(parcelCounts[j]);
        }
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(dims.begin() + 1, dims.end())); !iter.atEnd(); ++iter)
        {
            for (int j = 0; j < numParcels; ++j)
            {
                parcelData[j].clear();//doesn't change allocation
            }
            myCiftiIn->getRow(scratchRow.data(), *iter);
            for (int64_t j = 0; j < numCols; ++j)
            {
                int parcel = indexToParcel[j];
                if (parcel != -1)
                {
                    if (isLabel)
                    {
                        parcelData[parcel].push_back(floor(scratchRow[j] + 0.5f));//round to nearest integer to be safe
                    } else {
                        parcelData[parcel].push_back(scratchRow[j]);
                    }
                }
            }
            for (int j = 0; j < numParcels; ++j)
            {
                CaretAssert(parcelCounts[j] == (int64_t)parcelData[j].size());
                if (parcelCounts[j] > 0 && (method != ReductionEnum::SAMPSTDEV || parcelCounts[j] > 1))
                {
                    if (excludeLow > 0.0f && excludeHigh > 0.0f)
                    {
                        scratchOutRow[j] = ReductionOperation::reduceExcludeDev(parcelData[j].data(), parcelData[j].size(), method, excludeLow, excludeHigh);
                    } else {
                        if (onlyNumeric)
                        {
                            scratchOutRow[j] = ReductionOperation::reduceOnlyNumeric(parcelData[j].data(), parcelData[j].size(), method);
                        } else {
                            scratchOutRow[j] = ReductionOperation::reduce(parcelData[j].data(), parcelData[j].size(), method);
                        }
                    }
                } else {//labelDir can't be 0 (row) because we are parcellating along row, so row must be dense
                    if (isLabel)
                    {
                        scratchOutRow[j] = myOutXML.getLabelsMap(labelDir).getMapLabelTable((*iter)[labelDir - 1])->getUnassignedLabelKey();
                    } else {
                        scratchOutRow[j] = emptyFillVal;//odd corner case, but probably fine: with nonzero empty fill value and SAMPSTDEV, parcels with only one element get the fill value, but aren't technically empty
                    }
                }
            }
            myCiftiOut->setRow(scratchOutRow.data(), *iter);
        }
    } else {
        vector<float> scratchOutRow(numCols);
        vector<int64_t> otherDims = dims;
        otherDims.erase(otherDims.begin() + direction);//direction being parcellated
        otherDims.erase(otherDims.begin());//row
        vector<vector<vector<float> > > parcelData(numParcels, vector<vector<float> >(numCols));//float so we can use ReductionOperation
        for (int i = 0; i < numParcels; ++i)
        {
            for (int j = 0; j < numCols; ++j)
            {
                parcelData[i][j].reserve(parcelCounts[i]);
            }
        }
        for (MultiDimIterator<int64_t> iter(otherDims); !iter.atEnd(); ++iter)
        {
            vector<int64_t> indices(dims.size() - 1);//we need to add the parcellated direction index back into the index list to use it in getRow/setRow
            for (int i = 0; i < (int)otherDims.size(); ++i)
            {
                if (i < direction - 1)
                {
                    indices[i] = (*iter)[i];
                } else {
                    indices[i + 1] = (*iter)[i];
                }
            }//indices[direction - 1] is uninitialized, as it is the dimension to be parcellated
            for (int i = 0; i < numParcels; ++i)
            {
                for (int j = 0; j < numCols; ++j)
                {
                    parcelData[i][j].clear();//doesn't change allocation
                }
            }
            for (int64_t i = 0; i < dims[direction]; ++i)
            {
                int parcel = indexToParcel[i];
                if (parcel != -1)
                {
                    indices[direction - 1] = i;
                    myCiftiIn->getRow(scratchRow.data(), indices);
                    vector<vector<float> >& parcelRef = parcelData[parcel];
                    for (int j = 0; j < numCols; ++j)
                    {
                        if (isLabel)
                        {
                            parcelRef[j].push_back(floor(scratchRow[j] + 0.5f));
                        } else {
                            parcelRef[j].push_back(scratchRow[j]);
                        }
                    }
                }
            }
            for (int i = 0; i < numParcels; ++i)
            {
                indices[direction - 1] = i;
                int64_t count = parcelCounts[i];
                vector<vector<float> >& parcelRef = parcelData[i];
                if (count > 0 && (method != ReductionEnum::SAMPSTDEV || count > 1))
                {
                    for (int j = 0; j < numCols; ++j)
                    {
                        CaretAssert((int64_t)parcelRef[j].size() == count);
                        if (excludeLow > 0.0f && excludeHigh > 0.0f)
                        {
                            scratchOutRow[j] = ReductionOperation::reduceExcludeDev(parcelRef[j].data(), parcelRef[j].size(), method, excludeLow, excludeHigh);
                        } else {
                            if (onlyNumeric)
                            {
                                scratchOutRow[j] = ReductionOperation::reduceOnlyNumeric(parcelRef[j].data(), parcelRef[j].size(), method);
                            } else {
                                scratchOutRow[j] = ReductionOperation::reduce(parcelRef[j].data(), parcelRef[j].size(), method);
                            }
                        }
                    }
                } else {
                    for (int j = 0; j < numCols; ++j)
                    {
                        CaretAssert((int64_t)parcelRef[j].size() == count);
                        if (isLabel)
                        {
                            if (labelDir == CiftiXML::ALONG_ROW)
                            {
                                scratchOutRow[j] = myOutXML.getLabelsMap(CiftiXML::ALONG_ROW).getMapLabelTable(j)->getUnassignedLabelKey();
                            } else {
                                scratchOutRow[j] = myOutXML.getLabelsMap(labelDir).getMapLabelTable(indices[labelDir - 1])->getUnassignedLabelKey();
                            }
                        } else {
                            scratchOutRow[j] = emptyFillVal;
                        }
                    }
                }
                myCiftiOut->setRow(scratchOutRow.data(), indices);
            }
        }
    }
}

namespace
{
    void doWeightedParcellation(const CiftiFile* myCiftiIn, const int& direction, CiftiFile* myCiftiOut, const vector<int>& indexToParcel,
                                const vector<vector<float> >& parcelWeights, const ReductionEnum::Enum& method, const float& excludeLow, const float& excludeHigh, const bool& onlyNumeric,
                                const float& emptyFillVal, CiftiFile* emptyMaskOut)
    {
        const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
        const CiftiXML& myOutXML = myCiftiOut->getCiftiXML();
        vector<int64_t> dims = myInputXML.getDimensions();
        CaretAssert(direction < (int)dims.size());
        bool isLabel = false;
        int labelDir = -1;
        for (int i = 0; i < (int)dims.size(); ++i)
        {
            if (myInputXML.getMappingType(i) == CiftiMappingType::LABELS)
            {
                isLabel = true;
                labelDir = i;
                break;//there should never be more than one dimension with LABEL type, and if there is, just use the first one, i guess...
            }
        }
        if (isLabel && method != ReductionEnum::MODE)
        {
            CaretLogWarning(ReductionEnum::toName(method) + " reduction requested while parcellating label data");
        }
        int numParcels = myOutXML.getDimensionLength(direction);
        if (emptyMaskOut != NULL)
        {
            CiftiXML maskOutXML;
            maskOutXML.setNumberOfDimensions(2);
            maskOutXML.setMap(CiftiXML::ALONG_COLUMN, myOutXML.getParcelsMap(direction));
            CiftiScalarsMap maskNameMap;
            maskNameMap.setLength(1);
            maskNameMap.setMapName(0, "parcel not empty");
            maskOutXML.setMap(CiftiXML::ALONG_ROW, maskNameMap);
            emptyMaskOut->setCiftiXML(maskOutXML);
            vector<float> emptyMaskData(numParcels, 1.0f);
            for (int i = 0; i < numParcels; ++i)
            {
                if (parcelWeights[i].size() == 0)
                {
                    emptyMaskData[i] = 0.0f;
                }
            }
            emptyMaskOut->setColumn(emptyMaskData.data(), 0);
        }
        int64_t numCols = myInputXML.getDimensionLength(CiftiXML::ALONG_ROW);
        vector<float> scratchRow(numCols);
        if (direction == CiftiXML::ALONG_ROW)
        {
            vector<float> scratchOutRow(numParcels);
            vector<vector<float> > parcelData(numParcels);//float so we can use ReductionOperation
            for (int j = 0; j < numParcels; ++j)
            {
                parcelData[j].reserve(parcelWeights[j].size());
            }
            for (MultiDimIterator<int64_t> iter(vector<int64_t>(dims.begin() + 1, dims.end())); !iter.atEnd(); ++iter)
            {
                for (int j = 0; j < numParcels; ++j)
                {
                    parcelData[j].clear();//doesn't change allocation
                }
                myCiftiIn->getRow(scratchRow.data(), *iter);
                for (int64_t j = 0; j < numCols; ++j)
                {
                    int parcel = indexToParcel[j];
                    if (parcel != -1)
                    {
                        if (isLabel)
                        {
                            parcelData[parcel].push_back(floor(scratchRow[j] + 0.5f));//round to nearest integer to be safe
                        } else {
                            parcelData[parcel].push_back(scratchRow[j]);
                        }
                    }
                }
                for (int j = 0; j < numParcels; ++j)
                {
                    CaretAssert(parcelWeights[j].size() == parcelData[j].size());
                    if (parcelData[j].size() > 0 && (method != ReductionEnum::SAMPSTDEV || parcelData[j].size() > 1))
                    {
                        if (excludeLow > 0.0f && excludeHigh > 0.0f)
                        {
                            scratchOutRow[j] = ReductionOperation::reduceWeightedExcludeDev(parcelData[j].data(), parcelWeights[j].data(), parcelData[j].size(), method, excludeLow, excludeHigh);
                        } else {
                            if (onlyNumeric)
                            {
                                scratchOutRow[j] = ReductionOperation::reduceWeightedOnlyNumeric(parcelData[j].data(), parcelWeights[j].data(), parcelData[j].size(), method);
                            } else {
                                scratchOutRow[j] = ReductionOperation::reduceWeighted(parcelData[j].data(), parcelWeights[j].data(), parcelData[j].size(), method);
                            }
                        }
                    } else {//labelDir can't be 0 (row) because we are parcellating along row, so row must be dense
                        if (isLabel)
                        {
                            scratchOutRow[j] = myOutXML.getLabelsMap(labelDir).getMapLabelTable((*iter)[labelDir - 1])->getUnassignedLabelKey();
                        } else {
                            scratchOutRow[j] = emptyFillVal;
                        }
                    }
                }
                myCiftiOut->setRow(scratchOutRow.data(), *iter);
            }
        } else {
            vector<float> scratchOutRow(numCols);
            vector<int64_t> otherDims = dims;
            otherDims.erase(otherDims.begin() + direction);//direction being parcellated
            otherDims.erase(otherDims.begin());//row
            vector<vector<vector<float> > > parcelData(numParcels, vector<vector<float> >(numCols));//float so we can use ReductionOperation
            for (int i = 0; i < numParcels; ++i)
            {
                for (int j = 0; j < numCols; ++j)
                {
                    parcelData[i][j].reserve(parcelWeights[i].size());
                }
            }
            for (MultiDimIterator<int64_t> iter(otherDims); !iter.atEnd(); ++iter)
            {
                vector<int64_t> indices(dims.size() - 1);//we need to add the parcellated direction index back into the index list to use it in getRow/setRow
                for (int i = 0; i < (int)otherDims.size(); ++i)
                {
                    if (i < direction - 1)
                    {
                        indices[i] = (*iter)[i];
                    } else {
                        indices[i + 1] = (*iter)[i];
                    }
                }//indices[direction - 1] is uninitialized, as it is the dimension to be parcellated
                for (int i = 0; i < numParcels; ++i)
                {
                    for (int j = 0; j < numCols; ++j)
                    {
                        parcelData[i][j].clear();//doesn't change allocation
                    }
                }
                for (int64_t i = 0; i < dims[direction]; ++i)
                {
                    int parcel = indexToParcel[i];
                    if (parcel != -1)
                    {
                        indices[direction - 1] = i;
                        myCiftiIn->getRow(scratchRow.data(), indices);
                        vector<vector<float> >& parcelRef = parcelData[parcel];
                        for (int j = 0; j < numCols; ++j)
                        {
                            if (isLabel)
                            {
                                parcelRef[j].push_back(floor(scratchRow[j] + 0.5f));
                            } else {
                                parcelRef[j].push_back(scratchRow[j]);
                            }
                        }
                    }
                }
                for (int i = 0; i < numParcels; ++i)
                {
                    indices[direction - 1] = i;
                    int64_t count = (int64_t)parcelWeights[i].size();
                    vector<vector<float> >& parcelRef = parcelData[i];
                    if (count > 0 && (method != ReductionEnum::SAMPSTDEV || count > 1))
                    {
                        for (int j = 0; j < numCols; ++j)
                        {
                            CaretAssert((int64_t)parcelRef[j].size() == count);
                            if (excludeLow > 0.0f && excludeHigh > 0.0f)
                            {
                                scratchOutRow[j] = ReductionOperation::reduceWeightedExcludeDev(parcelRef[j].data(), parcelWeights[i].data(), parcelRef[j].size(), method, excludeLow, excludeHigh);
                            } else {
                                if (onlyNumeric)
                                {
                                    scratchOutRow[j] = ReductionOperation::reduceWeightedOnlyNumeric(parcelRef[j].data(), parcelWeights[i].data(), parcelRef[j].size(), method);
                                } else {
                                    scratchOutRow[j] = ReductionOperation::reduceWeighted(parcelRef[j].data(), parcelWeights[i].data(), parcelRef[j].size(), method);
                                }
                            }
                        }
                    } else {
                        for (int j = 0; j < numCols; ++j)
                        {
                            CaretAssert((int64_t)parcelRef[j].size() == count);
                            if (isLabel)
                            {
                                if (labelDir == CiftiXML::ALONG_ROW)
                                {
                                    scratchOutRow[j] = myOutXML.getLabelsMap(CiftiXML::ALONG_ROW).getMapLabelTable(j)->getUnassignedLabelKey();
                                } else {
                                    scratchOutRow[j] = myOutXML.getLabelsMap(labelDir).getMapLabelTable(indices[labelDir - 1])->getUnassignedLabelKey();
                                }
                            } else {
                                scratchOutRow[j] = emptyFillVal;
                            }
                        }
                    }
                    myCiftiOut->setRow(scratchOutRow.data(), indices);
                }
            }
        }
    }
}

AlgorithmCiftiParcellate::AlgorithmCiftiParcellate(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const CiftiFile* myCiftiLabel, const int& direction, CiftiFile* myCiftiOut,
                                                   const MetricFile* leftWeights, const MetricFile* rightWeights, const MetricFile* cerebWeights, const ReductionEnum::Enum& method,
                                                   const float& excludeLow, const float& excludeHigh, const bool& onlyNumeric,
                                                   const bool& legacyMode, const float& emptyFillVal, CiftiFile* emptyMaskOut): AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    const CiftiXML& myLabelXML = myCiftiLabel->getCiftiXML();
    vector<int64_t> dims = myInputXML.getDimensions();
    if (direction >= (int)dims.size()) throw AlgorithmException("specified direction doesn't exist in input file");
    if (myInputXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti file does not have brain models mapping type in specified direction");
    }
    if (myLabelXML.getNumberOfDimensions() != 2 ||
        myLabelXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS ||
        myLabelXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti label file has the wrong mapping types");
    }
    const CiftiBrainModelsMap& inputDense = myInputXML.getBrainModelsMap(direction);
    const CiftiBrainModelsMap& labelDense = myLabelXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    float voxelVolume = 1.0f;
    if (inputDense.hasVolumeData())
    {//don't check volume space if direction doesn't have volume data
        if (labelDense.hasVolumeData() && !inputDense.getVolumeSpace().matches(labelDense.getVolumeSpace()))
        {
            throw AlgorithmException("input cifti files must have the same volume space");
        }
        Vector3D ivec, jvec, kvec, origin;//compute the volume of a voxel in case a parcel spans both surface and volume
        inputDense.getVolumeSpace().getSpacingVectors(ivec, jvec, kvec, origin);
        voxelVolume = abs(ivec.dot(jvec.cross(kvec)));
    }
    vector<StructureEnum::Enum> surfStructs = inputDense.getSurfaceStructureList();
    for (int i = 0; i < (int)surfStructs.size(); ++i)
    {
        const MetricFile* toCheck = NULL;
        switch (surfStructs[i])
        {
            case StructureEnum::CORTEX_LEFT:
                toCheck = leftWeights;
                break;
            case StructureEnum::CORTEX_RIGHT:
                toCheck = rightWeights;
                break;
            case StructureEnum::CEREBELLUM:
                toCheck = cerebWeights;
                break;
            default:
                throw AlgorithmException("unsupported surface structure: " + StructureEnum::toName(surfStructs[i]));
        }
        if (toCheck == NULL) throw AlgorithmException("weight metric required but not provided for structure " + StructureEnum::toName(surfStructs[i]));
        if (toCheck->getNumberOfNodes() != inputDense.getSurfaceNumberOfNodes(surfStructs[i]))
        {
            throw AlgorithmException("weight metric has incorrect number of vertices for structure " + StructureEnum::toName(surfStructs[i]));
        }
        checkStructureMatch(toCheck, surfStructs[i], "weight metric", "it is provided as the argument for");
    }
    vector<int> indexToParcel;
    CiftiXML myOutXML = myInputXML;
    CiftiParcelsMap outParcelMap = parcellateMapping(myCiftiLabel, inputDense, indexToParcel, legacyMode);
    int numParcels = outParcelMap.getLength();
    if (numParcels < 1)
    {
        throw AlgorithmException("no parcels found, output file would be empty, aborting");
    }
    myOutXML.setMap(direction, outParcelMap);
    myCiftiOut->setCiftiXML(myOutXML);
    vector<vector<float> > parcelWeights(numParcels);
    for (int64_t j = 0; j < (int64_t)indexToParcel.size(); ++j)
    {
        int parcel = indexToParcel[j];
        if (parcel != -1)
        {
            const CiftiBrainModelsMap::IndexInfo myDenseInfo = inputDense.getInfoForIndex(j);
            if (myDenseInfo.m_type == CiftiBrainModelsMap::VOXELS)
            {
                parcelWeights[parcel].push_back(voxelVolume);
            } else {
                const MetricFile* toUse = NULL;
                switch (myDenseInfo.m_structure)
                {
                    case StructureEnum::CORTEX_LEFT:
                        toUse = leftWeights;
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        toUse = rightWeights;
                        break;
                    case StructureEnum::CEREBELLUM:
                        toUse = cerebWeights;
                        break;
                    default:
                        CaretAssert(0);
                }
                parcelWeights[parcel].push_back(toUse->getValue(myDenseInfo.m_surfaceNode, 0));
            }
        }
    }
    doWeightedParcellation(myCiftiIn, direction, myCiftiOut, indexToParcel, parcelWeights, method, excludeLow, excludeHigh, onlyNumeric, emptyFillVal, emptyMaskOut);
}

AlgorithmCiftiParcellate::AlgorithmCiftiParcellate(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const CiftiFile* myCiftiLabel, const int& direction, CiftiFile* myCiftiOut,
                                                   const CiftiFile* ciftiWeights, const ReductionEnum::Enum& method, const float& excludeLow, const float& excludeHigh, const bool& onlyNumeric,
                                                   const bool& legacyMode, const float& emptyFillVal, CiftiFile* emptyMaskOut): AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CaretAssert(direction >= 0);
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    const CiftiXML& myLabelXML = myCiftiLabel->getCiftiXML();
    const CiftiXML& weightsXML = ciftiWeights->getCiftiXML();
    vector<int64_t> dims = myInputXML.getDimensions();
    if (direction >= (int)dims.size()) throw AlgorithmException("specified direction doesn't exist in input file");
    if (myInputXML.getMappingType(direction) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti file does not have brain models mapping type in specified direction");
    }
    if (weightsXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("cifti weight file does not have brain models along column");
    }
    if (myLabelXML.getNumberOfDimensions() != 2 ||
        myLabelXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS ||
        myLabelXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("input cifti label file has the wrong mapping types");
    }
    const CiftiBrainModelsMap& inputDense = myInputXML.getBrainModelsMap(direction);
    const CiftiBrainModelsMap& labelDense = myLabelXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    const CiftiBrainModelsMap& weightsDense = weightsXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    vector<StructureEnum::Enum> surfModels = labelDense.getSurfaceStructureList();
    for (int i = 0; i < int(surfModels.size()); ++i)
    {
        if (weightsDense.hasSurfaceData(surfModels[i]))
        {
            if (labelDense.getSurfaceNumberOfNodes(surfModels[i]) != weightsDense.getSurfaceNumberOfNodes(surfModels[i]))
            {
                throw AlgorithmException("cifti weight file has wrong number of vertices for surface structure " + StructureEnum::toName(surfModels[i]));
            }
        }
    }
    if (labelDense.hasVolumeData())
    {//don't check volume space if direction doesn't have volume data
        if (inputDense.hasVolumeData() && !labelDense.getVolumeSpace().matches(inputDense.getVolumeSpace()))
        {
            throw AlgorithmException("input cifti file has a different volume space");
        }
        if (weightsDense.hasVolumeData() && !labelDense.getVolumeSpace().matches(weightsDense.getVolumeSpace()))
        {
            throw AlgorithmException("cifti weight file has a different volume space");
        }
    }
    vector<int> indexToParcel;
    CiftiXML myOutXML = myInputXML;
    CiftiParcelsMap outParcelMap = parcellateMapping(myCiftiLabel, inputDense, indexToParcel, legacyMode);
    int numParcels = outParcelMap.getLength();
    if (numParcels < 1)
    {
        throw AlgorithmException("no parcels found, output file would be empty, aborting");
    }
    myOutXML.setMap(direction, outParcelMap);
    myCiftiOut->setCiftiXML(myOutXML);
    vector<float> weightCol(weightsXML.getDimensionLength(CiftiXML::ALONG_COLUMN));
    ciftiWeights->getColumn(weightCol.data(), 0);
    vector<vector<float> > parcelWeights(numParcels);
    for (int64_t j = 0; j < (int64_t)indexToParcel.size(); ++j)
    {
        int parcel = indexToParcel[j];
        if (parcel != -1)
        {
            int weightIndex = -1;
            CiftiBrainModelsMap::IndexInfo myInfo = inputDense.getInfoForIndex(j);
            switch (myInfo.m_type)
            {
                case CiftiBrainModelsMap::SURFACE:
                    weightIndex = weightsDense.getIndexForNode(myInfo.m_surfaceNode, myInfo.m_structure);
                    break;
                case CiftiBrainModelsMap::VOXELS:
                    weightIndex = weightsDense.getIndexForVoxel(myInfo.m_ijk);
                    break;
            }
            if (weightIndex < 0)
            {
                throw AlgorithmException("cifti weights file does not contain all necessary vertices and voxels");
            }
            parcelWeights[parcel].push_back(weightCol[weightIndex]);
        }
    }
    doWeightedParcellation(myCiftiIn, direction, myCiftiOut, indexToParcel, parcelWeights, method, excludeLow, excludeHigh, onlyNumeric, emptyFillVal, emptyMaskOut);
}

CiftiParcelsMap AlgorithmCiftiParcellate::parcellateMapping(const CiftiFile* myCiftiLabel, const CiftiBrainModelsMap& toParcellate, vector<int>& indexToParcelOut, const bool& legacyMode)
{
    const CiftiXML& myLabelXML = myCiftiLabel->getCiftiXML();
    if (myLabelXML.getNumberOfDimensions() != 2 ||
        myLabelXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS ||
        myLabelXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("AlgorithmCiftiParcellate::parcellateMapping requires a cifti dlabel file as input");
    }
    const CiftiLabelsMap& myLabelsMap = myLabelXML.getLabelsMap(CiftiXML::ALONG_ROW);
    const CiftiBrainModelsMap& labelDenseMap = myLabelXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    CiftiParcelsMap ret;
    if (labelDenseMap.hasVolumeData() && toParcellate.hasVolumeData())
    {//if only one has voxel data, don't error until we know it is used
        if(!toParcellate.getVolumeSpace().matches(labelDenseMap.getVolumeSpace()))
        {
            throw AlgorithmException("data file to parcellate has a different voxel space than the dlabel file");
        }
        ret.setVolumeSpace(toParcellate.getVolumeSpace());
    }
    const GiftiLabelTable* myLabelTable = myLabelsMap.getMapLabelTable(0);
    vector<float> labelData(myLabelXML.getDimensionLength(CiftiXML::ALONG_COLUMN));
    int unusedKey = myLabelTable->getUnassignedLabelKey();
    myCiftiLabel->getColumn(labelData.data(), 0);
    indexToParcelOut.clear();
    indexToParcelOut.resize(toParcellate.getLength(), -1);
    if (!legacyMode)
    {//by default, the parcel definitions are based only on the dlabel file
        const vector<StructureEnum::Enum> labelSurfList = labelDenseMap.getSurfaceStructureList();
        const set<int32_t> allLabelKeys = myLabelTable->getKeys();
        map<int32_t, int32_t> keyToParcel;
        int32_t count = 0;
        vector<CiftiParcelsMap::Parcel> parcelList;
        for (set<int32_t>::const_iterator iter = allLabelKeys.begin(); iter != allLabelKeys.end(); ++iter)
        {
            if (*iter != unusedKey)
            {
                keyToParcel[*iter] = count;
                parcelList.push_back(CiftiParcelsMap::Parcel());
                parcelList.back().m_name = myLabelTable->getLabelName(*iter);
                ++count;
            }
        }
        for (int i = 0; i < (int)labelSurfList.size(); ++i)
        {
            StructureEnum::Enum myStruct = labelSurfList[i];
            if (toParcellate.hasSurfaceData(myStruct))
            {//if a surface is missing from the data, don't error until we know it is used by the parcellation
                if (labelDenseMap.getSurfaceNumberOfNodes(myStruct) != toParcellate.getSurfaceNumberOfNodes(myStruct))
                {//if both have a surface, it must match
                    throw AlgorithmException("mismatch in number of surface vertices between data and dlabel for structure " + StructureEnum::toName(myStruct));
                }
            }
            ret.addSurface(labelDenseMap.getSurfaceNumberOfNodes(myStruct), myStruct);
            vector<CiftiBrainModelsMap::SurfaceMap> labelSurfMap = labelDenseMap.getSurfaceMap(myStruct);
            for (int64_t j = 0; j < (int64_t)labelSurfMap.size(); ++j)
            {
                int labelKey = (int)floor(labelData[labelSurfMap[j].m_ciftiIndex] + 0.5f);
                map<int32_t, int32_t>::iterator found = keyToParcel.find(labelKey);//could be unlabeled, or wild key value
                if (found != keyToParcel.end())
                {
                    int32_t whichParcel = found->second;
                    parcelList[whichParcel].m_surfaceNodes[myStruct].insert(labelSurfMap[j].m_surfaceNode);
                    int64_t dataIndex = toParcellate.getIndexForNode(labelSurfMap[j].m_surfaceNode, myStruct);
                    if (dataIndex < 0)
                    {
                        throw AlgorithmException("data file is missing vertex " + AString::number(labelSurfMap[j].m_surfaceNode) + " in structure " +
                                                 StructureEnum::toName(myStruct) + ", which is used by label '" + parcelList[whichParcel].m_name + "'");
                    }
                    indexToParcelOut[dataIndex] = whichParcel;
                }
            }
        }
        const vector<CiftiBrainModelsMap::VolumeMap> labelVolMap = labelDenseMap.getFullVolumeMap();
        for (int64_t i = 0; i < (int64_t)labelVolMap.size(); ++i)
        {
            int labelKey = (int)floor(labelData[labelVolMap[i].m_ciftiIndex] + 0.5f);
            map<int32_t, int32_t>::iterator found = keyToParcel.find(labelKey);//could be unlabeled, or wild key value
            if (found != keyToParcel.end())
            {
                int32_t whichParcel = found->second;
                parcelList[whichParcel].m_voxelIndices.insert(labelVolMap[i].m_ijk);
                int64_t dataIndex = toParcellate.getIndexForVoxel(labelVolMap[i].m_ijk);
                if (dataIndex < 0)
                {
                    throw AlgorithmException("data file is missing voxel (" + AString::fromNumbers(labelVolMap[i].m_ijk, 3, ", ") + "), which is used by label '" +
                                             parcelList[whichParcel].m_name + "'");
                }
                indexToParcelOut[dataIndex] = whichParcel;
            }
        }
        for (int i = 0; i < (int)parcelList.size(); ++i)
        {
            ret.addParcel(parcelList[i]);
        }
    } else {//legacy mode: parcels are defined by overlap between labels and the data ROI, any parcels that don't overlap any data are discarded
        vector<StructureEnum::Enum> surfList = toParcellate.getSurfaceStructureList();
        map<int, pair<CiftiParcelsMap::Parcel, int> > usedKeys;//the keys from the label table that actually overlap with data in the input file
        for (int i = 0; i < (int)surfList.size(); ++i)
        {
            StructureEnum::Enum myStruct = surfList[i];
            if (labelDenseMap.hasSurfaceData(myStruct))
            {//if a surface is missing from the label file, don't error
                if (labelDenseMap.getSurfaceNumberOfNodes(myStruct) != toParcellate.getSurfaceNumberOfNodes(myStruct))
                {
                    throw AlgorithmException("mismatch in number of surface vertices between input and dlabel for structure " + StructureEnum::toName(myStruct));
                }
                ret.addSurface(toParcellate.getSurfaceNumberOfNodes(myStruct), myStruct);
                vector<CiftiBrainModelsMap::SurfaceMap> surfMap = toParcellate.getSurfaceMap(myStruct);
                int64_t mapSize = (int64_t)surfMap.size();
                for (int64_t j = 0; j < mapSize; ++j)
                {
                    int64_t labelIndex = labelDenseMap.getIndexForNode(surfMap[j].m_surfaceNode, myStruct);
                    if (labelIndex != -1)
                    {
                        int labelKey = (int)floor(labelData[labelIndex] + 0.5f);
                        if (labelKey != unusedKey)
                        {
                            int tempVal = -1;
                            map<int, pair<CiftiParcelsMap::Parcel, int> >::iterator iter = usedKeys.find(labelKey);
                            if (iter == usedKeys.end())
                            {
                                const GiftiLabel* myLabel = myLabelTable->getLabel(labelKey);
                                if (myLabel != NULL)//ignore values that aren't in the label table
                                {
                                    tempVal = usedKeys.size();
                                    CiftiParcelsMap::Parcel tempParcel;
                                    tempParcel.m_name = myLabel->getName();
                                    tempParcel.m_surfaceNodes[myStruct].insert(surfMap[j].m_surfaceNode);
                                    usedKeys[labelKey] = pair<CiftiParcelsMap::Parcel, int>(tempParcel, tempVal);
                                }
                            } else {
                                tempVal = iter->second.second;
                                CiftiParcelsMap::Parcel& tempParcel = iter->second.first;
                                tempParcel.m_surfaceNodes[myStruct].insert(surfMap[j].m_surfaceNode);
                            }
                            indexToParcelOut[surfMap[j].m_ciftiIndex] = tempVal;//we will remap these to be in order of label keys later
                        }
                    }
                }
            }
        }
        vector<CiftiBrainModelsMap::VolumeMap> volMap = toParcellate.getFullVolumeMap();
        for (int64_t i = 0; i < (int64_t)volMap.size(); ++i)
        {
            int64_t labelIndex = labelDenseMap.getIndexForVoxel(volMap[i].m_ijk);
            if (labelIndex != -1)
            {
                int labelKey = (int)floor(labelData[labelIndex] + 0.5f);
                if (labelKey != unusedKey)
                {
                    int tempVal = -1;
                    map<int, pair<CiftiParcelsMap::Parcel, int> >::iterator iter = usedKeys.find(labelKey);
                    if (iter == usedKeys.end())
                    {
                        const GiftiLabel* myLabel = myLabelTable->getLabel(labelKey);
                        if (myLabel != NULL)//ignore values that aren't in the label table
                        {
                            tempVal = usedKeys.size();
                            CiftiParcelsMap::Parcel tempParcel;
                            tempParcel.m_name = myLabel->getName();
                            tempParcel.m_voxelIndices.insert(VoxelIJK(volMap[i].m_ijk));
                            usedKeys[labelKey] = pair<CiftiParcelsMap::Parcel, int>(tempParcel, tempVal);
                        }
                    } else {
                        tempVal = iter->second.second;
                        CiftiParcelsMap::Parcel& tempParcel = iter->second.first;
                        tempParcel.m_voxelIndices.insert(VoxelIJK(volMap[i].m_ijk));
                    }
                    indexToParcelOut[volMap[i].m_ciftiIndex] = tempVal;//we will remap these to be in order of label keys later
                }
            }
        }
        int numParcels = (int)usedKeys.size();
        vector<int> valRemap(numParcels, -1);
        int count = 0;
        for (map<int, pair<CiftiParcelsMap::Parcel, int> >::const_iterator iter = usedKeys.begin(); iter != usedKeys.end(); ++iter)
        {
            valRemap[iter->second.second] = count;//build a lookup from temp values to label key rank
            ret.addParcel(iter->second.first);
            ++count;
        }
        int64_t lookupSize = (int64_t)indexToParcelOut.size();
        for (int64_t i = 0; i < lookupSize; ++i)//finally, remap the temporary values to the key order of the labels
        {
            if (indexToParcelOut[i] != -1)
            {
                indexToParcelOut[i] = valRemap[indexToParcelOut[i]];
            }
        }
    }
    return ret;
}

float AlgorithmCiftiParcellate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiParcellate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
