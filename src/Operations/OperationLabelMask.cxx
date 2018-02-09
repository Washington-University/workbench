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

#include "OperationLabelMask.h"
#include "OperationException.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "MetricFile.h"

using namespace caret;
using namespace std;

AString OperationLabelMask::getCommandSwitch()
{
    return "-label-mask";
}

AString OperationLabelMask::getShortDescription()
{
    return "MASK A LABEL FILE";
}

OperationParameters* OperationLabelMask::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label", "the label file to mask");
    ret->addMetricParameter(2, "mask", "the mask metric");
    ret->addLabelOutputParameter(3, "label-out", "the output label file");
    OptionalParameter* columnSelect = ret->createOptionalParameter(4, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    ret->setHelpText(
        AString("By default, the output label is a copy of the input label, but with the 'unused' label wherever the mask metric is zero or negative.  ") +
        "if -column is specified, the output contains only one column, the masked version of the specified input column."
    );
    return ret;
}

void OperationLabelMask::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    LabelFile* myLabel = myParams->getLabel(1);
    MetricFile* myMask = myParams->getMetric(2);
    int32_t numNodes = myLabel->getNumberOfNodes(), numCols = myLabel->getNumberOfColumns();
    if (myMask->getNumberOfNodes() != numNodes)
    {
        throw OperationException("mask metric must have the same number of vertices");
    }
    LabelFile* myLabelOut = myParams->getOutputLabel(3);//gets the output metric with key 2
    OptionalParameter* columnSelect = myParams->getOptionalParameter(4);//gets optional parameter with key 3
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myLabel->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0 || columnNum >= numCols)
        {
            throw OperationException("invalid column specified");
        }
    }
    vector<int32_t> columnScratch(numNodes);
    int32_t unusedKey = myLabel->getLabelTable()->getUnassignedLabelKey();
    if (columnNum == -1)
    {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, numCols);
        myLabelOut->setStructure(myLabel->getStructure());
        *(myLabelOut->getLabelTable()) = *(myLabel->getLabelTable());
        const float* roiColumn = myMask->getValuePointerForColumn(0);
        for (int i = 0; i < numCols; ++i)
        {
            myLabelOut->setColumnName(i, myLabel->getColumnName(i));
            const int32_t* keyColumn = myLabel->getLabelKeyPointerForColumn(i);
            for (int j = 0; j < numNodes; ++j)
            {
                if (roiColumn[j] > 0.0f)
                {
                    columnScratch[j] = keyColumn[j];
                } else {
                    columnScratch[j] = unusedKey;
                }
            }
            myLabelOut->setLabelKeysForColumn(i, columnScratch.data());
        }
    } else {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, 1);
        myLabelOut->setStructure(myLabel->getStructure());
        *(myLabelOut->getLabelTable()) = *(myLabel->getLabelTable());
        myLabelOut->setColumnName(0, myLabel->getColumnName(columnNum));
        const float* roiColumn = myMask->getValuePointerForColumn(0);
        const int32_t* keyColumn = myLabel->getLabelKeyPointerForColumn(columnNum);
        for (int j = 0; j < numNodes; ++j)
        {
            if (roiColumn[j] > 0.0f)
            {
                columnScratch[j] = keyColumn[j];
            } else {
                columnScratch[j] = unusedKey;
            }
        }
        myLabelOut->setLabelKeysForColumn(0, columnScratch.data());
    }
}
