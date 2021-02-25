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
    return "MERGE CIFTI TIMESERIES, SCALAR, OR LABEL FILES";
}

OperationParameters* OperationCiftiMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti file");
    
    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(2, "-cifti", "specify an input cifti file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "a cifti file to use columns from");
    ParameterComponent* columnOpt = ciftiOpt->createRepeatableParameter(2, "-column", "select a single column to use");
    columnOpt->addStringParameter(1, "column", "the column number (starting from 1) or name");
    OptionalParameter* upToOpt = columnOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of columns");
    upToOpt->addStringParameter(1, "last-column", "the number or name of the last column to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(3, "-mem-limit", "restrict memory used for file reading efficiency");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("Given input CIFTI files which have matching mappings along columns, and for which mappings along rows ") +
        "are the same type, all either series, scalars, or labels, this command concatenates the specified columns horizontally (rows become longer).\n\n" +
        "Example: wb_command -cifti-merge out.dtseries.nii -cifti first.dtseries.nii -column 1 -cifti second.dtseries.nii\n\n" +
        "This example would take the first column from first.dtseries.nii, followed by all columns from second.dtseries.nii, " +
        "and write these columns to out.dtseries.nii."
    );
    return ret;
}

void OperationCiftiMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const vector<ParameterComponent*>& myInputs = myParams->getRepeatableParameterInstances(2);
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
    if (baseXML.getNumberOfDimensions() != 2) throw OperationException("only 2D cifti are supported");
    const CiftiMappingType& baseColMapping = *(baseXML.getMap(CiftiXML::ALONG_COLUMN)), &baseRowMapping = *(baseXML.getMap(CiftiXML::ALONG_ROW));
    int64_t numRows = baseColMapping.getLength();
    switch (baseRowMapping.getType())
    {
        case CiftiMappingType::SCALARS:
        case CiftiMappingType::LABELS:
        case CiftiMappingType::SERIES:
            break;
        default:
            throw OperationException("row mapping type must be series, scalars, or labels");
    }
    int64_t numOutColumns = 0;//output row length
    exception_ptr exPtr;
    int64_t exceptedFile = -1;
    //NOTE: throwing inside omp parallel causes an uninformative abort, so catch, skip the rest, and rethrow later
    //our build/processing setup seems to bottleneck on lots of multithreaded memory allocation, so limit to 4 threads for now
    //windows compiler doesn't like unsigned omp loop variables
#pragma omp CARET_PARFOR schedule(dynamic) num_threads(4)
    for (int64_t i = 0; i < int64_t(myInputs.size()); ++i)
    {
        if (exceptedFile > -1) continue;//"abort" checking any more files
        try
        {
            CiftiFile* ciftiIn = myInputs[i]->getCifti(1);
            vector<int64_t> thisDims = ciftiIn->getDimensions();
            const CiftiXML& thisXML = ciftiIn->getCiftiXML();
            if (thisXML.getNumberOfDimensions() != 2) throw OperationException("only 2D cifti are supported");
            if (!thisXML.getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(baseColMapping)) throw OperationException("file '" + ciftiIn->getFileName() + "' has non-matching mapping along columns");
            if (thisXML.getMappingType(CiftiXML::ALONG_ROW) != baseRowMapping.getType()) throw OperationException("file '" + ciftiIn->getFileName() + "' has different mapping type along rows");
            const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
            int numColumnOpts = (int)columnOpts.size();
#pragma omp critical
            {
                if (numColumnOpts > 0)
                {
                    for (int j = 0; j < numColumnOpts; ++j)
                    {
                        int64_t initialColumn = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(columnOpts[j]->getString(1));//this function has the 1-indexing convention built in
                        if (initialColumn < 0 || initialColumn >= thisDims[0]) throw OperationException("column '" + columnOpts[j]->getString(1) + "' not valid in file '" + ciftiIn->getFileName() + "'");
                        OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);
                        if (upToOpt->m_present)
                        {
                            int64_t finalColumn = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                            if (finalColumn < 0 || finalColumn >= thisDims[0]) throw OperationException("ending column '" + columnOpts[j]->getString(1) + "' not valid in file '" + ciftiIn->getFileName() + "'");
                            if (finalColumn < initialColumn) throw OperationException("ending column occurs before starting column in file '" + ciftiIn->getFileName() + "'");
                            numOutColumns += finalColumn - initialColumn + 1;//inclusive - we don't need to worry about reversing for counting, though
                        } else {
                            numOutColumns += 1;
                        }
                    }
                } else {
                    numOutColumns += thisDims[0];
                }
            }
            ciftiIn->getCiftiXML().getFileMetaData()->clear();//don't need the metadata anymore, so free its memory too
            if (i != 0)//don't mess with the first file, we use its mapping for the output file
            {
                ciftiIn->forgetMapping(CiftiXML::ALONG_COLUMN);//HACK: release the memory being used to store the dense or parcel mapping, to deal with thousands of inputs
            }
        } catch (...) {
#pragma omp critical
            {
                if (exceptedFile < 0 || i < exceptedFile)
                {//emulate serial order or processing, because why not
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
    switch (baseRowMapping.getType())
    {
        case CiftiMappingType::SCALARS:
            outScalarMap.setLength(numOutColumns);
            break;
        case CiftiMappingType::LABELS:
            outLabelMap.setLength(numOutColumns);
            isLabel = true;
            break;
        case CiftiMappingType::SERIES:
            outSeriesMap.setLength(numOutColumns);
            outSeriesMap.setUnit(((const CiftiSeriesMap&)baseRowMapping).getUnit());
            outSeriesMap.setStart(((const CiftiSeriesMap&)baseRowMapping).getStart());
            outSeriesMap.setStep(((const CiftiSeriesMap&)baseRowMapping).getStep());
            doLoop = false;
            break;
        default:
            CaretAssert(false);
    }
    int64_t curCol = 0, scratchRowLength = 0, shortestRow = -1;
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
        const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numColumnOpts = (int)columnOpts.size();
        if (numColumnOpts > 0)
        {
            scratchRowLength = max(scratchRowLength, thisRowLength);//if we use the entire row, we don't need a separate scratch row for it
            if (doLoop)
            {
                for (int j = 0; j < numColumnOpts; ++j)
                {
                    int64_t initialColumn = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(columnOpts[j]->getString(1));//this function has the 1-indexing convention built in
                    OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);//we already checked that these strings give a valid column
                    if (upToOpt->m_present)
                    {
                        int finalColumn = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                        bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                        if (reverse)
                        {
                            for (int c = finalColumn; c >= initialColumn; --c)
                            {
                                if (isLabel)
                                {
                                    const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(CiftiXML::ALONG_ROW);
                                    outLabelMap.setMapName(curCol, thisLabelMap.getMapName(c));
                                    *(outLabelMap.getMapLabelTable(curCol)) = *(thisLabelMap.getMapLabelTable(c));
                                    *(outLabelMap.getMapMetadata(curCol)) = *(thisLabelMap.getMapMetadata(c));
                                } else {
                                    const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(CiftiXML::ALONG_ROW);
                                    outScalarMap.setMapName(curCol, thisScalarMap.getMapName(c));
                                    *(outScalarMap.getMapPalette(curCol)) = *(thisScalarMap.getMapPalette(c));
                                    *(outScalarMap.getMapMetadata(curCol)) = *(thisScalarMap.getMapMetadata(c));
                                }
                                ++curCol;
                            }
                        } else {
                            for (int c = initialColumn; c <= finalColumn; ++c)
                            {
                                if (isLabel)
                                {
                                    const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(CiftiXML::ALONG_ROW);
                                    outLabelMap.setMapName(curCol, thisLabelMap.getMapName(c));
                                    *(outLabelMap.getMapLabelTable(curCol)) = *(thisLabelMap.getMapLabelTable(c));
                                    *(outLabelMap.getMapMetadata(curCol)) = *(thisLabelMap.getMapMetadata(c));
                                } else {
                                    const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(CiftiXML::ALONG_ROW);
                                    outScalarMap.setMapName(curCol, thisScalarMap.getMapName(c));
                                    *(outScalarMap.getMapPalette(curCol)) = *(thisScalarMap.getMapPalette(c));
                                    *(outScalarMap.getMapMetadata(curCol)) = *(thisScalarMap.getMapMetadata(c));
                                }
                                ++curCol;
                            }
                        }
                    } else {
                        if (isLabel)
                        {
                            const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(CiftiXML::ALONG_ROW);
                            outLabelMap.setMapName(curCol, thisLabelMap.getMapName(initialColumn));
                            *(outLabelMap.getMapLabelTable(curCol)) = *(thisLabelMap.getMapLabelTable(initialColumn));
                            *(outLabelMap.getMapMetadata(curCol)) = *(thisLabelMap.getMapMetadata(initialColumn));
                        } else {
                            const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(CiftiXML::ALONG_ROW);
                            outScalarMap.setMapName(curCol, thisScalarMap.getMapName(initialColumn));
                            *(outScalarMap.getMapPalette(curCol)) = *(thisScalarMap.getMapPalette(initialColumn));
                            *(outScalarMap.getMapMetadata(curCol)) = *(thisScalarMap.getMapMetadata(initialColumn));
                        }
                        ++curCol;
                    }
                }
            }
        } else {
            if (doLoop)
            {
                for (int64_t j = 0; j < thisDims[0]; ++j)
                {
                    if (isLabel)
                    {
                        const CiftiLabelsMap& thisLabelMap = thisXML.getLabelsMap(CiftiXML::ALONG_ROW);
                        outLabelMap.setMapName(curCol, thisLabelMap.getMapName(j));
                        *(outLabelMap.getMapLabelTable(curCol)) = *(thisLabelMap.getMapLabelTable(j));
                        *(outLabelMap.getMapMetadata(curCol)) = *(thisLabelMap.getMapMetadata(j));
                    } else {
                        const CiftiScalarsMap& thisScalarMap = thisXML.getScalarsMap(CiftiXML::ALONG_ROW);
                        outScalarMap.setMapName(curCol, thisScalarMap.getMapName(j));
                        *(outScalarMap.getMapPalette(curCol)) = *(thisScalarMap.getMapPalette(j));
                        *(outScalarMap.getMapMetadata(curCol)) = *(thisScalarMap.getMapMetadata(j));
                    }
                    ++curCol;
                }
            }
        }
    }
    CaretAssert(!doLoop || curCol == numOutColumns);
    CiftiXML outXML;
    outXML.setNumberOfDimensions(2);
    outXML.setMap(CiftiXML::ALONG_COLUMN, baseColMapping);
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);//NOTE: get this after all the inputs, so that parent provenance works
    switch (baseRowMapping.getType())
    {
        case CiftiMappingType::LABELS:
            outXML.setMap(CiftiXML::ALONG_ROW, outLabelMap);
            break;
        case CiftiMappingType::SCALARS:
            outXML.setMap(CiftiXML::ALONG_ROW, outScalarMap);
            break;
        case CiftiMappingType::SERIES:
            outXML.setMap(CiftiXML::ALONG_ROW, outSeriesMap);
            break;
        default:
            CaretAssert(false);
    }
    ciftiOut->setCiftiXML(outXML);
    int64_t chunkRows = -1;//invalid value
    //checking first cifti for "in memory" should catch both -cifti-read-memory and possible future GUI-based operation
    if (firstCifti->isInMemory())
    {
        chunkRows = 1;//chunking is for reading (and memory) efficiency, if we have already read all cifti into memory, reading efficiency is moot, so minimize additional memory
    } else {
        if (memLimitGB > 0.0f)
        {
            int64_t chunkMaxBytes = int64_t(memLimitGB * (1<<30));
            int64_t computeBytes = sizeof(float) * numRows * numOutColumns;
            int64_t numPasses = (computeBytes - 1) / chunkMaxBytes + 1;
            chunkRows = (numRows - 1) / numPasses + 1;
        } else {//by default, do enough rows to read at least 10MB (assuming float) from each file before moving to the next
            int64_t rowBytes = sizeof(float) * shortestRow;
            chunkRows = ((10<<20) - 1) / rowBytes + 1;
            int64_t numPasses = numRows / chunkRows;//make sure we never make chunks smaller, so different fenceposting
            chunkRows = (numRows - 1) / numPasses + 1;
        }
    }
    CaretAssert(chunkRows > 0);
    vector<vector<float> > outRows(chunkRows, vector<float>(numOutColumns));
    vector<float> scratchRow(scratchRowLength);
    for (int64_t chunkStart = 0; chunkStart < numRows; chunkStart += chunkRows)
    {
        int64_t chunkEnd = min(chunkStart + chunkRows, numRows);
        int64_t chunkCol = 0;
        for (int i = 0; i < numInputs; ++i)
        {
            const CiftiFile* ciftiIn = myInputs[i]->getCifti(1);
            vector<int64_t> thisDims = ciftiIn->getDimensions();
            const CiftiXML& thisXML = ciftiIn->getCiftiXML();
            const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
            int numColumnOpts = (int)columnOpts.size();
            for (int64_t row = chunkStart; row < chunkEnd; ++row)
            {
                int64_t chunkIndex = row - chunkStart;
                curCol = chunkCol;
                if (numColumnOpts > 0)
                {
                    ciftiIn->getRow(scratchRow.data(), row);
                    for (int j = 0; j < numColumnOpts; ++j)
                    {
                        int64_t initialColumn = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(columnOpts[j]->getString(1));//this function has the 1-indexing convention built in
                        OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);//we already checked that these strings give a valid column
                        if (upToOpt->m_present)
                        {
                            int finalColumn = thisXML.getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(upToOpt->getString(1));//ditto
                            bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                            if (reverse)
                            {
                                for (int c = finalColumn; c >= initialColumn; --c)
                                {
                                    outRows[chunkIndex][curCol] = scratchRow[c];
                                    ++curCol;
                                }
                            } else {
                                for (int c = initialColumn; c <= finalColumn; ++c)
                                {
                                    outRows[chunkIndex][curCol] = scratchRow[c];
                                    ++curCol;
                                }
                            }
                        } else {
                            outRows[chunkIndex][curCol] = scratchRow[initialColumn];
                            ++curCol;
                        }
                    }
                } else {
                    ciftiIn->getRow(outRows[chunkIndex].data() + curCol, row);
                    curCol += thisDims[0];
                }
            }
            chunkCol = curCol;//done with chunk for this file, update for next file
        }
        CaretAssert(curCol == numOutColumns);
        for (int64_t row = chunkStart; row < chunkEnd; ++row)
        {
            int64_t chunkIndex = row - chunkStart;
            ciftiOut->setRow(outRows[chunkIndex].data(), row);
        }
    }
}
