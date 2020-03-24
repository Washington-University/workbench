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

#include "OperationLabelMerge.h"
#include "OperationException.h"

#include "GiftiLabelTable.h"
#include "LabelFile.h"

#include <map>
#include <vector>

using namespace caret;
using namespace std;

AString OperationLabelMerge::getCommandSwitch()
{
    return "-label-merge";
}

AString OperationLabelMerge::getShortDescription()
{
    return "MERGE LABEL FILES INTO A NEW FILE";
}

OperationParameters* OperationLabelMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelOutputParameter(1, "label-out", "the output label");
    
    ParameterComponent* labelOpt = ret->createRepeatableParameter(2, "-label", "specify an input label");
    labelOpt->addLabelParameter(1, "label-in", "a label file to use columns from");
    ParameterComponent* columnOpt = labelOpt->createRepeatableParameter(2, "-column", "select a single column to use");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    OptionalParameter* upToOpt = columnOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of columns");
    upToOpt->addStringParameter(1, "last-column", "the number or name of the last column to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
    ret->setHelpText(
        AString("Takes one or more label files and constructs a new label file by concatenating columns from them.  ") +
        "The input files must have the same number of vertices and the same structure.\n\n" +
        "Example: wb_command -label-merge out.label.gii -label first.label.gii -column 1 -label second.label.gii\n\n" +
        "This example would take the first column from first.label.gii and all subvolumes from second.label.gii, " +
        "and write these to out.label.gii."
    );
    return ret;
}

namespace
{//private namespace for helper functions
    void doRemap(const int32_t* input, const map<int32_t, int32_t>& remap, const int numElems, const int32_t& unlabeledValue, int32_t* output)
    {
        for (int i = 0; i < numElems; ++i)
        {
            map<int32_t, int32_t>::const_iterator iter = remap.find(input[i]);
            if (iter != remap.end())
            {
                output[i] = iter->second;
            } else {//values that have no key
                output[i] = unlabeledValue;
            }
        }
    }
}

void OperationLabelMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    LabelFile* myLabelOut = myParams->getOutputLabel(1);
    const vector<ParameterComponent*>& myInputs = myParams->getRepeatableParameterInstances(2);
    int numInputs = (int)myInputs.size();
    if (numInputs == 0) throw OperationException("no inputs specified");
    const LabelFile* firstLabel = myInputs[0]->getLabel(1);
    int numOutColumns = 0;
    int numNodes = firstLabel->getNumberOfNodes();
    StructureEnum::Enum myStruct = firstLabel->getStructure();
    GiftiLabelTable outTable;
    vector<map<int32_t,int32_t> > fileRemap(numInputs);
    for (int i = 0; i < numInputs; ++i)
    {
        LabelFile* inputLabel = myInputs[i]->getLabel(1);
        fileRemap[i] = outTable.append(*(inputLabel->getLabelTable()));//NOTE: does (and must) include identity mappings - anything that doesn't match was invalid in the original file
        if (numNodes != inputLabel->getNumberOfNodes()) throw OperationException("file '" + inputLabel->getFileName() + "' has a different number of nodes than the first");
        if (myStruct != inputLabel->getStructure()) throw OperationException("file '" + inputLabel->getFileName() + "' has a different structure than the first");
        const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numColumnOpts = (int)columnOpts.size();
        if (numColumnOpts > 0)
        {
            for (int j = 0; j < numColumnOpts; ++j)
            {
                int initialColumn = inputLabel->getMapIndexFromNameOrNumber(columnOpts[j]->getString(1));
                if (initialColumn < 0) throw OperationException("column '" + columnOpts[j]->getString(1) + "' not found in file '" + inputLabel->getFileName() + "'");
                OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int finalColumn = inputLabel->getMapIndexFromNameOrNumber(upToOpt->getString(1));
                    if (finalColumn < 0) throw OperationException("ending column '" + upToOpt->getString(1) + "' not found in file '" + inputLabel->getFileName() + "'");
                    if (finalColumn < initialColumn) throw OperationException("ending column '" + upToOpt->getString(1) + "' occurs before starting column '"
                                                                            + columnOpts[j]->getString(1) + "' in file '" + inputLabel->getFileName() + "'");
                    numOutColumns += finalColumn - initialColumn + 1;//inclusive - we don't need to worry about reversing for counting, though
                } else {
                    numOutColumns += 1;
                }
            }
        } else {
            numOutColumns += inputLabel->getNumberOfColumns();
        }
    }
    myLabelOut->setNumberOfNodesAndColumns(numNodes, numOutColumns);
    myLabelOut->setStructure(myStruct);
    *(myLabelOut->getLabelTable()) = outTable;
    vector<int32_t> scratchCol(numNodes);
    int curColumn = 0;
    for (int i = 0; i < numInputs; ++i)
    {
        const LabelFile* inputLabel = myInputs[i]->getLabel(1);
        const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numColumnOpts = (int)columnOpts.size();
        if (numColumnOpts > 0)
        {
            for (int j = 0; j < numColumnOpts; ++j)
            {
                int initialColumn = inputLabel->getMapIndexFromNameOrNumber(columnOpts[j]->getString(1));
                OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int finalColumn = inputLabel->getMapIndexFromNameOrNumber(upToOpt->getString(1));
                    bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                    if (reverse)
                    {
                        for (int c = finalColumn; c >= initialColumn; --c)
                        {
                            doRemap(inputLabel->getLabelKeyPointerForColumn(c), fileRemap[i], numNodes, outTable.getUnassignedLabelKey(), scratchCol.data());
                            myLabelOut->setLabelKeysForColumn(curColumn, scratchCol.data());
                            myLabelOut->setColumnName(curColumn, inputLabel->getColumnName(c));
                            ++curColumn;
                        }
                    } else {
                        for (int c = initialColumn; c <= finalColumn; ++c)
                        {
                            doRemap(inputLabel->getLabelKeyPointerForColumn(c), fileRemap[i], numNodes, outTable.getUnassignedLabelKey(), scratchCol.data());
                            myLabelOut->setLabelKeysForColumn(curColumn, scratchCol.data());
                            myLabelOut->setColumnName(curColumn, inputLabel->getColumnName(c));
                            ++curColumn;
                        }
                    }
                } else {
                    doRemap(inputLabel->getLabelKeyPointerForColumn(initialColumn), fileRemap[i], numNodes, outTable.getUnassignedLabelKey(), scratchCol.data());
                    myLabelOut->setLabelKeysForColumn(curColumn, scratchCol.data());
                    myLabelOut->setColumnName(curColumn, inputLabel->getColumnName(initialColumn));
                    ++curColumn;
                }
            }
        } else {
            int numColumns = inputLabel->getNumberOfColumns();
            for (int j = 0; j < numColumns; ++j)
            {
                doRemap(inputLabel->getLabelKeyPointerForColumn(j), fileRemap[i], numNodes, outTable.getUnassignedLabelKey(), scratchCol.data());
                myLabelOut->setLabelKeysForColumn(curColumn, scratchCol.data());
                myLabelOut->setColumnName(curColumn, inputLabel->getColumnName(j));
                ++curColumn;
            }
        }
    }
    CaretAssert(curColumn == numOutColumns);
}
