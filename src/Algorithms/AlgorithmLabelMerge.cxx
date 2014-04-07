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

#include "AlgorithmLabelMerge.h"
#include "AlgorithmException.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"

#include <map>

using namespace caret;
using namespace std;

AString AlgorithmLabelMerge::getCommandSwitch()
{
    return "-label-merge";
}

AString AlgorithmLabelMerge::getShortDescription()
{
    return "MERGE LABEL FILES INTO A NEW FILE";
}

OperationParameters* AlgorithmLabelMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelOutputParameter(1, "label-out", "the output label file");
    
    ParameterComponent* labelOpt = ret->createRepeatableParameter(2, "-label", "specify an input label file");
    labelOpt->addLabelParameter(1, "label-in", "a label file to use columns from");
    OptionalParameter* columnOpt = labelOpt->createOptionalParameter(2, "-column", "select a single column to use");
    columnOpt->addStringParameter(1, "column", "the column number or name");

    ret->setHelpText(
        AString("Takes one or more label files and constructs a new label file by concatenating columns from them.  ") +
        "The input files must have the same number of nodes.\n\n" +
        "Example: wb_command -label-merge out.label.gii -label first.label.gii -column 1 -label second.label.gii\n\n" +
        "This example would take the first column from first.label.gii and all subvolumes from second.label.gii, " +
        "and write these to out.label.gii."
    );
    return ret;
}

void AlgorithmLabelMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* labelOut = myParams->getOutputLabel(1);
    const vector<ParameterComponent*>& myInputs = *(myParams->getRepeatableParameterInstances(2));
    int numInputs = (int)myInputs.size();
    vector<const LabelFile*> labelList;
    vector<int64_t> indexList;
    for (int i = 0; i < numInputs; ++i)
    {
        LabelFile* myLabel = myInputs[i]->getLabel(1);
        labelList.push_back(myLabel);
        OptionalParameter* columnOpt = myInputs[i]->getOptionalParameter(2);
        if (columnOpt->m_present)
        {
            int index = myLabel->getMapIndexFromNameOrNumber(columnOpt->getString(1));
            if (index == -1) throw AlgorithmException("column '" + columnOpt->getString(1) + "' not found in file '" + myLabel->getFileName() + "'");
            indexList.push_back(index);
        } else {
            indexList.push_back(-1);
        }
    }
    AlgorithmLabelMerge(myProgObj, labelList, indexList, labelOut);
}

AlgorithmLabelMerge::AlgorithmLabelMerge(ProgressObject* myProgObj, const vector<const LabelFile*>& labelList, const vector<int64_t>& indexList, LabelFile* labelOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (labelList.size() == 0)
    {
        throw AlgorithmException("no files specified");
    }
    int numNodes = labelList[0]->getNumberOfNodes();
    CaretAssert(labelList.size() == indexList.size());
    int numInputs = (int)labelList.size();
    int64_t numOutCols = 0;
    GiftiLabelTable outTable = *(labelList[0]->getLabelTable());//this works because there is only one label table per label file
    map<int32_t, int32_t> translate, temptrans;//NOTE: the keys in the first label file will NOT be in the translate map!
    for (int i = 0; i < numInputs; ++i)
    {
        if (labelList[i]->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("label file '" + labelList[i]->getFileName() + "' has a different number of vertices");
        }
        if (indexList[i] < -1) throw AlgorithmException("found invalid (less than -1) index in indexList in AlgorithmLabelMerge");
        if (i != 0)//append the label table
        {
            temptrans = outTable.append(*(labelList[i]->getLabelTable()));
            translate.insert(temptrans.begin(), temptrans.end());
        }
        if (indexList[i] == -1)
        {
            numOutCols += labelList[i]->getNumberOfColumns();
        } else {
            if (indexList[i] >= labelList[i]->getNumberOfColumns()) throw AlgorithmException("index too large for input label file #" + AString::number(i + 1));
            numOutCols += 1;
        }
    }
    labelOut->setNumberOfNodesAndColumns(numNodes, numOutCols);
    labelOut->setStructure(labelList[0]->getStructure());
    *(labelOut->getLabelTable()) = outTable;
    int64_t curCol = 0;
    vector<int32_t> colScratch(numNodes);
    for (int i = 0; i < numInputs; ++i)
    {
        if (indexList[i] == -1)
        {
            int numCols = labelList[i]->getNumberOfColumns();
            for (int j = 0; j < numCols; ++j)
            {
                const int32_t* colData = labelList[i]->getLabelKeyPointerForColumn(j);
                for (int n = 0; n < numNodes; ++n)
                {
                    map<int32_t, int32_t>::iterator iter = translate.find(colData[n]);
                    if (iter == translate.end())//will be true for first file
                    {
                        colScratch[n] = colData[n];
                    } else {
                        colScratch[n] = iter->second;
                    }
                }
                labelOut->setColumnName(curCol, labelList[i]->getColumnName(j));
                labelOut->setLabelKeysForColumn(curCol, colScratch.data());
                ++curCol;
            }
        } else {
            const int32_t* colData = labelList[i]->getLabelKeyPointerForColumn(indexList[i]);
            for (int n = 0; n < numNodes; ++n)
            {
                map<int32_t, int32_t>::iterator iter = translate.find(colData[n]);
                if (iter == translate.end())//will be true for first file
                {
                    colScratch[n] = colData[n];
                } else {
                    colScratch[n] = iter->second;
                }
            }
            labelOut->setColumnName(curCol, labelList[i]->getColumnName(indexList[i]));
            labelOut->setLabelKeysForColumn(curCol, colScratch.data());
            ++curCol;
        }
    }
}

float AlgorithmLabelMerge::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelMerge::getSubAlgorithmWeight()
{
    return 0.0f;
}
