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

#include "OperationCiftiMerge.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CaretCommandGlobalOptions.h"
#include "CiftiFile.h"

#include <algorithm>
#include <vector>
#include <utility>

using namespace caret;
using namespace std;

AString OperationCiftiMerge::getCommandSwitch()
{
    return "-cifti-merge";
}

AString OperationCiftiMerge::getShortDescription()
{
    return "MERGE OR SPLIT ON SERIES, SCALAR, OR LABEL DIMENSIONS";
}

OperationParameters* OperationCiftiMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti file");
    
    OptionalComponent* ciftiOpt = ret->createRepeatableParameter(2, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to use data from");
    OptionalComponent* indexOpt = ciftiOpt->createRepeatableParameter(2, "-index", "select a single index to use");
    indexOpt->addLegacySwitch("-column");
    indexOpt->addStringParameter(1, "index", "the index number (starting from 1), or name");
    OptionalParameter* upToOpt = indexOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of indices");
    upToOpt->addStringParameter(1, "last-index", "the number or name of the last index to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
    OptionalParameter* directionOpt = ret->createOptionalParameter(4, "-direction", "merge in a direction other than along rows");
    directionOpt->addStringParameter(1, "direction", "the dimension to split/concatenate along, default ROW");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(3, "-mem-limit", "restrict memory used for file reading efficiency");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("Given input CIFTI files for which mappings along the selected direction are the same type, all either series, scalars, or labels, ") +
        "and the other dimensions are index-compatible, this command concatenates the data in the specified indices/ranges along the selected direction " +
        "(by default, on typical 2D cifti, concatenate horizontally, so rows become longer).  " + CiftiXML::directionFromStringExplanation() +
        "\n\nExample: wb_command -cifti-merge out.dtseries.nii -cifti first.dtseries.nii -index 1 -cifti second.dtseries.nii\n\n" +
        "This example would take the first column from first.dtseries.nii, followed by all columns from second.dtseries.nii, " +
        "and write these columns to out.dtseries.nii.  "
    );
    return ret;
}

void OperationCiftiMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const vector<ParameterComponent*>& myInputs = myParams->getRepeatableParameterInstances(2);
    int direction = CiftiXML::ALONG_ROW;
    OptionalParameter* directionOpt = myParams->getOptionalParameter(4);
    if (directionOpt->m_present)
    {
        direction = CiftiXML::directionFromString(directionOpt->getString(1));
    }
    float memLimitGB = -1.0f;
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(3);
    if (memLimitOpt->m_present)
    {
        memLimitGB = float(memLimitOpt->getDouble(1));
        if (memLimitGB < 0.0f)
        {
            throw OperationException("memory limit must be positive");
        }
    }
    int numInputs = (int)myInputs.size();
    if (numInputs == 0) throw OperationException("no inputs specified");
    const CiftiFile* firstCifti = myInputs[0]->getCifti(1);
    const CiftiXML& baseXML = firstCifti->getCiftiXML();
    CiftiScalarsMap defaultMap;
    const CiftiMappingType* baseMergeMapping = &defaultMap;
    if (direction < baseXML.getNumberOfDimensions())
        baseMergeMapping = baseXML.getMap(direction);
    switch (baseMergeMapping->getType())
    {
        case CiftiMappingType::SCALARS:
        case CiftiMappingType::LABELS:
        case CiftiMappingType::SERIES:
            break;
        default:
            throw OperationException("merge-direction mapping type must be series, scalars, or labels");
    }
    int64_t numOutIndices = 0;//output merge dimension length
    int numOutDims = max(baseXML.getNumberOfDimensions(), direction + 1); //allow -direction to invent dimensions
    exception_ptr exPtr;
    int64_t exceptedFile = -1;
    //NOTE: throwing inside omp parallel causes an uninformative abort, so catch, skip the rest, and rethrow later
    //our build/processing setup seems to bottleneck on lots of multithreaded memory allocation, so limit to 4 threads for now
    //windows compiler doesn't like unsigned omp loop variables
#pragma omp CARET_PARFOR schedule(dynamic) num_threads(min(4, omp_get_max_threads()))
    for (int64_t i = 0; i < int64_t(myInputs.size()); ++i)
    {
        if (exceptedFile > -1) continue;//"abort" checking any more files, "break" isn't allowed
        try
        {
            CiftiFile* ciftiIn = myInputs[i]->getCifti(1); //this is the line that needs to be parallel so that the XML parsing goes multicore
            const CiftiXML& thisXML = ciftiIn->getCiftiXML();
            {
                int j;
                for (j = 0; j < baseXML.getNumberOfDimensions(); ++j)
                {
                    if (j != direction)
                    {
                        if (j < thisXML.getNumberOfDimensions())
                        {
                            AString explanation;
                            if (!thisXML.getMap(j)->approximateMatch(*(baseXML.getMap(j)), &explanation))
                                throw OperationException("file '" + ciftiIn->getFileName() + "' has non-matching mapping along dimension " + AString::number(j + 1) + ": " + explanation);
                        } else {
                            if (baseXML.getDimensionLength(j) != 1)
                                throw OperationException("file '" + ciftiIn->getFileName() + "' has " + AString::number(thisXML.getNumberOfDimensions()) +
                                                         " dimensions, while the first input file has length " + AString::number(baseXML.getDimensionLength(j)) + " on dimension " + AString::number(j + 1));
                        }
                    }
                }
                //continue into the remaining dimensions of this file (which are missing from the first file), if applicable
                for (; j < thisXML.getNumberOfDimensions(); ++j)
                {
                    if (j != direction)
                    {
                        if (thisXML.getDimensionLength(j) > 1)
                        {
                            throw OperationException("file '" + ciftiIn->getFileName() + "' has length " + AString::number(thisXML.getDimensionLength(j)) + " on dimension " +
                                                     AString::number(j + 1) + ", where the first file has only " + AString::number(baseXML.getNumberOfDimensions()) + " dimensions");
                        }
                    }
                }
            }
            if (direction >= thisXML.getNumberOfDimensions() && baseMergeMapping->getType() == CiftiMappingType::LABELS)
            {
                throw OperationException("file '" + ciftiIn->getFileName() + "' is missing the merging direction, but the first input file has a LABELS mapping on that direction");
            }
            //this check could be relaxed, if we want an option to allow merging of series and scalars
            if (direction < thisXML.getNumberOfDimensions() && thisXML.getMappingType(direction) != baseMergeMapping->getType())
            {
                if (direction < baseXML.getNumberOfDimensions())
                {
                    throw OperationException("file '" + ciftiIn->getFileName() + "' has different mapping type along the merging direction");
                } else {
                    throw OperationException("file '" + ciftiIn->getFileName() + "' has a mapping type other than scalar for the merging direction, but the first input file has only " +
                                             AString::number(baseXML.getNumberOfDimensions()) + " dimensions");
                }
            }
            vector<int64_t> thisDims = ciftiIn->getDimensions();
            const vector<ParameterComponent*>& indexOpts = myInputs[i]->getRepeatableParameterInstances(2);
            int numIndexOpts = (int)indexOpts.size();
            if (thisXML.getNumberOfDimensions() > numOutDims)
            {
                numOutDims = thisXML.getNumberOfDimensions();
            }
            if (numIndexOpts > 0)
            {//could consider accepting "-index 1" on a missing dimension...but wait until someone complains before being permissive about sloppy 3D cifti commands
                if (direction >= int(thisXML.getNumberOfDimensions())) throw OperationException("unable to select index along missing merge dimension in file '" + ciftiIn->getFileName() + "'");
                for (int j = 0; j < numIndexOpts; ++j)
                {
                    int64_t initialIndex = thisXML.getMap(direction)->getIndexFromNumberOrName(indexOpts[j]->getString(1));//this function has the 1-indexing convention built in and checks against actual length
                    if (initialIndex < 0) throw OperationException("index '" + indexOpts[j]->getString(1) + "' not valid in file '" + ciftiIn->getFileName() + "'");
                    OptionalParameter* upToOpt = indexOpts[j]->getOptionalParameter(2);
                    if (upToOpt->m_present)
                    {
                        int64_t finalIndex = thisXML.getMap(direction)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                        if (finalIndex < 0) throw OperationException("ending column '" + indexOpts[j]->getString(1) + "' not valid in file '" + ciftiIn->getFileName() + "'");
                        if (finalIndex < initialIndex) throw OperationException("ending column occurs before starting column in file '" + ciftiIn->getFileName() + "'");
#pragma omp critical
                        {//NOTE: throw inside critical can't be caught outside critical, so don't do it
                            numOutIndices += finalIndex - initialIndex + 1;//inclusive - we don't need to worry about reversing for counting, though
                        }
                    } else {
#pragma omp critical
                        {
                            numOutIndices += 1;
                        }
                    }
                }
            } else {
#pragma omp critical
                {
                    if (direction < int(thisXML.getNumberOfDimensions()))
                    {
                        numOutIndices += thisXML.getDimensionLength(direction);
                    } else {
                        numOutIndices += 1;
                    }
                }
            }
            ciftiIn->getCiftiXML().getFileMetaData()->clear();//don't need the metadata anymore, so free its memory too
            if (i != 0)//don't mess with the first file, we use its mapping for the output file
            {
                for (int j = 0; j < thisXML.getNumberOfDimensions(); ++j)
                {
                    if (j != direction)
                        ciftiIn->forgetMapping(CiftiXML::ALONG_COLUMN);//HACK: release the memory being used to store the dense or parcel mapping, to deal with thousands of inputs
                }
            }
        } catch (...) {
#pragma omp critical
            {
                if (exceptedFile < 0 || i < exceptedFile)
                {//emulate serial order of processing, because why not
                    exceptedFile = i;
                    exPtr = current_exception();
                }
            }//exit from critical flushes everything
        }
    }
    if (exceptedFile > -1)
    {
        rethrow_exception(exPtr);
    }
    CiftiScalarsMap outScalarMap;//we only use one of these
    CiftiLabelsMap outLabelMap;
    CiftiSeriesMap outSeriesMap;
    bool isLabel = false, doLoop = true;//whether we need to set attributes on each column
    switch (baseMergeMapping->getType())
    {
        case CiftiMappingType::SCALARS:
            outScalarMap.setLength(numOutIndices);
            break;
        case CiftiMappingType::LABELS:
            outLabelMap.setLength(numOutIndices);
            isLabel = true;
            break;
        case CiftiMappingType::SERIES:
            outSeriesMap.setLength(numOutIndices);
            outSeriesMap.setUnit(((const CiftiSeriesMap*)baseMergeMapping)->getUnit());
            outSeriesMap.setStart(((const CiftiSeriesMap*)baseMergeMapping)->getStart());
            outSeriesMap.setStep(((const CiftiSeriesMap*)baseMergeMapping)->getStep());
            doLoop = false;
            break;
        default:
            CaretAssert(false);
    }
    int64_t curMergeIndex = 0, scratchRowLength = 0, shortestRow = -1; //shortest row is needed to figure out how many to read to reduce seek penalty from switching between files (for row merge)
    for (int i = 0; i < numInputs; ++i)
    {
        const CiftiFile* ciftiIn = myInputs[i]->getCifti(1);
        vector<int64_t> thisDims = ciftiIn->getDimensions();
        const CiftiXML& thisXML = ciftiIn->getCiftiXML();
        int64_t thisRowLength = thisXML.getDimensionLength(CiftiXML::ALONG_ROW);
        if (thisRowLength < shortestRow || shortestRow < 0)
        {
            shortestRow = thisRowLength;
        }
        const vector<ParameterComponent*>& indexOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numIndexOpts = (int)indexOpts.size();
        if (numIndexOpts > 0)
        {//already checked that this input has this mapping direction, above, not allowing "-index 1" on missing dimension
            scratchRowLength = max(scratchRowLength, thisRowLength);//if we use the entire row, we don't need a separate scratch row for it
            if (doLoop)
            {
                for (int j = 0; j < numIndexOpts; ++j)
                {
                    int64_t initialIndex = thisXML.getMap(direction)->getIndexFromNumberOrName(indexOpts[j]->getString(1));//this function has the 1-indexing convention built in
                    OptionalParameter* upToOpt = indexOpts[j]->getOptionalParameter(2);//we already checked that these strings give a valid column
                    if (upToOpt->m_present)
                    {
                        int64_t finalIndex = thisXML.getMap(direction)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                        bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                        if (reverse)
                        {
                            for (int64_t c = finalIndex; c >= initialIndex; --c)
                            {
                                if (isLabel)
                                {
                                    const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(direction);
                                    outLabelMap.setMapName(curMergeIndex, thisLabelMap.getMapName(c));
                                    *(outLabelMap.getMapLabelTable(curMergeIndex)) = *(thisLabelMap.getMapLabelTable(c));
                                    *(outLabelMap.getMapMetadata(curMergeIndex)) = *(thisLabelMap.getMapMetadata(c));
                                } else {
                                    const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(direction);
                                    outScalarMap.setMapName(curMergeIndex, thisScalarMap.getMapName(c));
                                    *(outScalarMap.getMapPalette(curMergeIndex)) = *(thisScalarMap.getMapPalette(c));
                                    *(outScalarMap.getMapMetadata(curMergeIndex)) = *(thisScalarMap.getMapMetadata(c));
                                }
                                ++curMergeIndex;
                            }
                        } else {
                            for (int64_t c = initialIndex; c <= finalIndex; ++c)
                            {
                                if (isLabel)
                                {
                                    const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(direction);
                                    outLabelMap.setMapName(curMergeIndex, thisLabelMap.getMapName(c));
                                    *(outLabelMap.getMapLabelTable(curMergeIndex)) = *(thisLabelMap.getMapLabelTable(c));
                                    *(outLabelMap.getMapMetadata(curMergeIndex)) = *(thisLabelMap.getMapMetadata(c));
                                } else {
                                    const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(direction);
                                    outScalarMap.setMapName(curMergeIndex, thisScalarMap.getMapName(c));
                                    *(outScalarMap.getMapPalette(curMergeIndex)) = *(thisScalarMap.getMapPalette(c));
                                    *(outScalarMap.getMapMetadata(curMergeIndex)) = *(thisScalarMap.getMapMetadata(c));
                                }
                                ++curMergeIndex;
                            }
                        }
                    } else {
                        if (isLabel)
                        {
                            const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(direction);
                            outLabelMap.setMapName(curMergeIndex, thisLabelMap.getMapName(initialIndex));
                            *(outLabelMap.getMapLabelTable(curMergeIndex)) = *(thisLabelMap.getMapLabelTable(initialIndex));
                            *(outLabelMap.getMapMetadata(curMergeIndex)) = *(thisLabelMap.getMapMetadata(initialIndex));
                        } else {
                            const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(direction);
                            outScalarMap.setMapName(curMergeIndex, thisScalarMap.getMapName(initialIndex));
                            *(outScalarMap.getMapPalette(curMergeIndex)) = *(thisScalarMap.getMapPalette(initialIndex));
                            *(outScalarMap.getMapMetadata(curMergeIndex)) = *(thisScalarMap.getMapMetadata(initialIndex));
                        }
                        ++curMergeIndex;
                    }
                }
            }
        } else {
            if (doLoop)
            {
                if (direction < thisXML.getNumberOfDimensions())
                {
                    for (int64_t j = 0; j < thisXML.getDimensionLength(direction); ++j)
                    {
                        if (isLabel)
                        {
                            const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(direction);
                            outLabelMap.setMapName(curMergeIndex, thisLabelMap.getMapName(j));
                            *(outLabelMap.getMapLabelTable(curMergeIndex)) = *(thisLabelMap.getMapLabelTable(j));
                            *(outLabelMap.getMapMetadata(curMergeIndex)) = *(thisLabelMap.getMapMetadata(j));
                        } else {
                            const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(direction);
                            outScalarMap.setMapName(curMergeIndex, thisScalarMap.getMapName(j));
                            *(outScalarMap.getMapPalette(curMergeIndex)) = *(thisScalarMap.getMapPalette(j));
                            *(outScalarMap.getMapMetadata(curMergeIndex)) = *(thisScalarMap.getMapMetadata(j));
                        }
                    }
                    ++curMergeIndex;
                } else {//act like it has a singular dimension here - nothing to copy
                    ++curMergeIndex;
                }
            }
        }
    }
    CaretAssert(!doLoop || curMergeIndex == numOutIndices);
    CiftiXML outXML;
    outXML.setNumberOfDimensions(numOutDims);
    for (int j = 0; j < numOutDims; ++j)
    {
        if (j == direction)
        {
            switch (baseMergeMapping->getType())
            {
                case CiftiMappingType::LABELS:
                    outXML.setMap(j, outLabelMap);
                    break;
                case CiftiMappingType::SCALARS:
                    outXML.setMap(j, outScalarMap);
                    break;
                case CiftiMappingType::SERIES:
                    outXML.setMap(j, outSeriesMap);
                    break;
                default:
                    CaretAssert(false);
            }
        } else {
            if (j < baseXML.getNumberOfDimensions())
            {
                outXML.setMap(j, *baseXML.getMap(j));
            } else {//missing dimensions are required to be singular and compatible with scalar, so we can use length-1 scalar on the output to fill
                outXML.setMap(j, CiftiScalarsMap(1));
            }
        }
    }
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);//NOTE: get this after all the inputs, so that parent provenance works
    ciftiOut->setCiftiXML(outXML);
    if (direction == CiftiXML::ALONG_ROW)
    {
        int64_t curRowIndex = 0;
        int64_t chunkRows = -1;//invalid value
        //checking first cifti for "in memory" should catch both -cifti-read-memory and possible future GUI-based operation
        int64_t numRows = 1;
        for (int i = 1; i < outXML.getNumberOfDimensions(); ++i)
        {
            numRows *= outXML.getDimensionLength(i);
        }
        if (firstCifti->isInMemory())
        {
            chunkRows = 1;//chunking is for reading (and memory) efficiency, if we have already read all cifti into memory, reading efficiency is moot, so minimize additional memory
        } else {
            if (memLimitGB > 0.0f)
            {
                int64_t chunkMaxBytes = int64_t(memLimitGB * (1<<30));
                int64_t computeBytes = sizeof(float) * numRows * numOutIndices;
                int64_t numPasses = (computeBytes - 1) / chunkMaxBytes + 1;
                chunkRows = (numRows - 1) / numPasses + 1;
            } else {//by default, do enough rows to read at least 10MB (assuming float) from each file before moving to the next
                int64_t rowBytes = sizeof(float) * shortestRow;
                chunkRows = ((10<<20) - 1) / rowBytes + 1;
                int64_t numPasses = max(int64_t(1), numRows / chunkRows);//make sure we never make chunks smaller, so different fenceposting
                chunkRows = (numRows - 1) / numPasses + 1;
            }
        }
        CaretAssert(chunkRows > 0);
        vector<vector<float> > outRows(chunkRows, vector<float>(numOutIndices));
        vector<float> scratchRow(scratchRowLength);
        auto outputIterator = ciftiOut->getIteratorOverRows(); //starts at beginning
        for (int64_t chunkStart = 0; chunkStart < numRows; chunkStart += chunkRows)
        {
            int64_t chunkEnd = min(chunkStart + chunkRows, numRows);
            int64_t chunkRowIndex = 0; //track position within the row, to update while looping through input files
            for (int i = 0; i < numInputs; ++i)
            {
                auto inputIterator = outputIterator; //start wherever we don't yet have output for
                const CiftiFile* ciftiIn = myInputs[i]->getCifti(1);
                const CiftiXML& thisXML = ciftiIn->getCiftiXML();
                const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
                int numColumnOpts = (int)columnOpts.size();
                for (int64_t chunkIndex = 0; chunkIndex < chunkEnd - chunkStart; ++chunkIndex)
                {
                    curRowIndex = chunkRowIndex;
                    if (numColumnOpts > 0)
                    {
                        auto inputSelect = *inputIterator;
                        inputSelect.erase(inputSelect.begin() + (thisXML.getNumberOfDimensions() - 1), inputSelect.end()); //deal with files that are missing a dimension
                        ciftiIn->getRow(scratchRow.data(), inputSelect); //get a row and...
                        ++inputIterator; //advance
                        for (int j = 0; j < numColumnOpts; ++j)
                        {
                            int64_t initialRowIndex = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(columnOpts[j]->getString(1));//this function has the 1-indexing convention built in
                            OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);//we already checked that these strings give a valid index
                            if (upToOpt->m_present)
                            {
                                int64_t finalRowIndex = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                                bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                                if (reverse)
                                {
                                    for (int64_t c = finalRowIndex; c >= initialRowIndex; --c)
                                    {
                                        outRows[chunkIndex][curRowIndex] = scratchRow[c];
                                        ++curRowIndex;
                                    }
                                } else {
                                    for (int64_t c = initialRowIndex; c <= finalRowIndex; ++c)
                                    {
                                        outRows[chunkIndex][curRowIndex] = scratchRow[c];
                                        ++curRowIndex;
                                    }
                                }
                            } else {
                                outRows[chunkIndex][curRowIndex] = scratchRow[initialRowIndex];
                                ++curRowIndex;
                            }
                        }
                    } else {
                        ciftiIn->getRow(outRows[chunkIndex].data() + curRowIndex, *inputIterator); //get a row and...
                        ++inputIterator; //advance
                        curRowIndex += thisXML.getDimensionLength(CiftiXML::ALONG_ROW);
                    }
                }
                chunkRowIndex = curRowIndex;//done with chunk for this file, update position within row for next file
            }
            CaretAssert(curRowIndex == numOutIndices);
            for (int64_t row = chunkStart; row < chunkEnd; ++row)
            {
                int64_t chunkIndex = row - chunkStart;
                ciftiOut->setRow(outRows[chunkIndex].data(), *outputIterator); //set a row and...
                ++outputIterator; //advance
            }
        }
        CaretAssert(outputIterator.atEnd()); //make sure we wrote the whole file
    } else {
        int64_t fileMergeIndex = 0;
        vector<float> scratchRow(outXML.getDimensionLength(CiftiXML::ALONG_ROW));
        //want to read input files as linearly as possible, so use inner, merge, and outer loops
        vector<int64_t> outDims = ciftiOut->getDimensions();
        const MultiDimIterator<int64_t> innerStart(vector<int64_t>(outDims.begin() + 1, outDims.begin() + direction));//NOTE: empty vector behaves like a single length-1 vector
        const MultiDimIterator<int64_t> outerStart(vector<int64_t>(outDims.begin() + direction + 1, outDims.end()));
        for (int i = 0; i < numInputs; ++i)
        {
            const CiftiFile* ciftiIn = myInputs[i]->getCifti(1);
            const CiftiXML& thisXML = ciftiIn->getCiftiXML();
            const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
            int numColumnOpts = (int)columnOpts.size();
            int64_t outerMergeIndex = -1; //initialize to invalid value, compiler doesn't understand that the loop will execute at least once
            for (auto outerIter = outerStart; !outerIter.atEnd(); ++outerIter)
            {
                outerMergeIndex = fileMergeIndex; //avoid overwriting the file-level merge index due to outer loop
                if (numColumnOpts > 0)
                {
                    for (int j = 0; j < numColumnOpts; ++j)
                    {
                        int64_t initialInputIndex = thisXML.getMap(direction)->getIndexFromNumberOrName(columnOpts[j]->getString(1));//this function has the 1-indexing convention built in
                        OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);//we already checked that these strings give a valid index
                        if (upToOpt->m_present)
                        {
                            int64_t finalInputIndex = thisXML.getMap(direction)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                            bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                            if (reverse)
                            {
                                for (int64_t backCount = 0; backCount <= finalInputIndex - initialInputIndex; ++backCount)
                                {//read forwards, write backwards, to make the filesystem happier
                                    for (auto innerIter = innerStart; !innerIter.atEnd(); ++innerIter)
                                    {
                                        auto inputSelect = *innerIter;//concatenate inner, direction, and outer selections
                                        inputSelect.push_back(backCount + initialInputIndex);
                                        auto outerSelect = *outerIter;
                                        inputSelect.insert(inputSelect.end(), outerSelect.begin(), outerSelect.end());
                                        
                                        auto outputSelect = inputSelect;//copy and overwrite direction index for output
                                        outputSelect[direction - 1] = outerMergeIndex + (finalInputIndex - initialInputIndex) - backCount;
                                        
                                        inputSelect.erase(inputSelect.begin() + (thisXML.getNumberOfDimensions() - 1), inputSelect.end()); //deal with files that are missing a dimension
                                        ciftiIn->getRow(scratchRow.data(), inputSelect);
                                        ciftiOut->setRow(scratchRow.data(), outputSelect);
                                    }
                                }
                                outerMergeIndex += finalInputIndex - initialInputIndex + 1;
                            } else {
                                for (int64_t c = initialInputIndex; c <= finalInputIndex; ++c)
                                {
                                    for (auto innerIter = innerStart; !innerIter.atEnd(); ++innerIter)
                                    {
                                        auto inputSelect = *innerIter;
                                        inputSelect.push_back(c);
                                        auto outerSelect = *outerIter;
                                        inputSelect.insert(inputSelect.end(), outerSelect.begin(), outerSelect.end());
                                        
                                        auto outputSelect = inputSelect;
                                        outputSelect[direction - 1] = outerMergeIndex;
                                        
                                        inputSelect.erase(inputSelect.begin() + (thisXML.getNumberOfDimensions() - 1), inputSelect.end());
                                        ciftiIn->getRow(scratchRow.data(), inputSelect);
                                        ciftiOut->setRow(scratchRow.data(), outputSelect);
                                    }
                                    ++outerMergeIndex;
                                }
                            }
                        } else {
                            for (auto innerIter = innerStart; !innerIter.atEnd(); ++innerIter)
                            {
                                auto inputSelect = *innerIter;
                                inputSelect.push_back(initialInputIndex);
                                auto outerSelect = *outerIter;
                                inputSelect.insert(inputSelect.end(), outerSelect.begin(), outerSelect.end());
                                
                                auto outputSelect = inputSelect;
                                outputSelect[direction - 1] = outerMergeIndex;
                                
                                inputSelect.erase(inputSelect.begin() + (thisXML.getNumberOfDimensions() - 1), inputSelect.end());
                                ciftiIn->getRow(scratchRow.data(), inputSelect);
                                ciftiOut->setRow(scratchRow.data(), outputSelect);
                            }
                            ++outerMergeIndex;
                        }
                    }
                } else { //all indices
                    int64_t mergeDimLength = 1; //deal with missing merge dimension
                    if (direction < thisXML.getNumberOfDimensions()) mergeDimLength = thisXML.getDimensionLength(direction);
                    for (int64_t j = 0; j < mergeDimLength; ++j)
                    {
                        for (auto innerIter = innerStart; !innerIter.atEnd(); ++innerIter)
                        {
                            auto inputSelect = *innerIter;
                            inputSelect.push_back(j);
                            auto outerSelect = *outerIter;
                            inputSelect.insert(inputSelect.end(), outerSelect.begin(), outerSelect.end());
                            
                            auto outputSelect = inputSelect;
                            outputSelect[direction - 1] = outerMergeIndex;
                            
                            inputSelect.erase(inputSelect.begin() + (thisXML.getNumberOfDimensions() - 1), inputSelect.end());
                            ciftiIn->getRow(scratchRow.data(), inputSelect);
                            ciftiOut->setRow(scratchRow.data(), outputSelect);
                        }
                        ++outerMergeIndex;
                    }
                }
            }
            //done with this file, update file-level merge index
            fileMergeIndex = outerMergeIndex;
        }
    }
}
