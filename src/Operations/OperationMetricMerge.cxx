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

#include "OperationMetricMerge.h"
#include "OperationException.h"
#include "CaretLogger.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationMetricMerge::getCommandSwitch()
{
    return "-metric-merge";
}

AString OperationMetricMerge::getShortDescription()
{
    return "MERGE METRIC FILES INTO A NEW FILE";
}

OperationParameters* OperationMetricMerge::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addMetricOutputParameter(1, "metric-out", "the output metric");
    
    ParameterComponent* metricOpt = ret->createRepeatableParameter(2, "-metric", "specify an input metric");
    metricOpt->addMetricParameter(1, "metric-in", "a metric file to use columns from");
    OptionalParameter* columnOpt = metricOpt->createOptionalParameter(2, "-column", "select a single column to use");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("Takes one or more metric files and constructs a new metric file by concatenating columns from them.  ") +
        "The input metric files must have the same number of vertices and same structure.\n\n" +
        "Example: wb_command -metric-merge out.func.gii -metric first.func.gii -column 1 -metric second.func.gii\n\n" +
        "This example would take the first column from first.func.gii, followed by all columns from second.func.gii, " +
        "and write these columns to out.func.gii."
    );
    return ret;
}

void OperationMetricMerge::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    MetricFile* myMetricOut = myParams->getOutputMetric(1);
    const vector<ParameterComponent*>& myInputs = *(myParams->getRepeatableParameterInstances(2));
    int numInputs = (int)myInputs.size();
    if (numInputs == 0) throw OperationException("no inputs specified");
    int numOutColumns = 0;
    bool first = true;
    int numNodes = -1;
    StructureEnum::Enum myStruct = StructureEnum::INVALID;
    for (int i = 0; i < numInputs; ++i)
    {
        MetricFile* inputMetric = myInputs[i]->getMetric(1);
        if (first)
        {
            numNodes = inputMetric->getNumberOfNodes();
            myStruct = inputMetric->getStructure();
            if (myStruct == StructureEnum::INVALID)
            {
                CaretLogWarning("the first input file has no valid structure, use -set-structure to fix this");
            }
        } else {
            if (numNodes != inputMetric->getNumberOfNodes()) throw OperationException("file '" + inputMetric->getFileName() + "' has a different number of nodes than the first");
            if (myStruct != inputMetric->getStructure()) throw OperationException("file '" + inputMetric->getFileName() + "' has a different structure than the first");
        }
        first = false;
        OptionalParameter* columnOpt = myInputs[i]->getOptionalParameter(2);
        if (columnOpt->m_present)
        {
            int column = inputMetric->getMapIndexFromNameOrNumber(columnOpt->getString(1));
            if (column == -1) throw OperationException("column '" + columnOpt->getString(1) + "' not found in file '" + inputMetric->getFileName() + "'");
            numOutColumns += 1;
        } else {
            numOutColumns += inputMetric->getNumberOfColumns();
        }
    }
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numOutColumns);
    myMetricOut->setStructure(myStruct);
    int curColumn = 0;
    for (int i = 0; i < numInputs; ++i)
    {
        MetricFile* inputMetric = myInputs[i]->getMetric(1);
        OptionalParameter* columnOpt = myInputs[i]->getOptionalParameter(2);
        if (columnOpt->m_present)
        {
            int column = inputMetric->getMapIndexFromNameOrNumber(columnOpt->getString(1));
            myMetricOut->setValuesForColumn(curColumn, inputMetric->getValuePointerForColumn(column));
            myMetricOut->setColumnName(curColumn, inputMetric->getColumnName(column));
            *(myMetricOut->getMapPaletteColorMapping(curColumn)) = *(inputMetric->getMapPaletteColorMapping(column));
            ++curColumn;
        } else {
            int numColumns = inputMetric->getNumberOfColumns();
            for (int i = 0; i < numColumns; ++i)
            {
                myMetricOut->setValuesForColumn(curColumn, inputMetric->getValuePointerForColumn(i));
                myMetricOut->setColumnName(curColumn, inputMetric->getColumnName(i));
                *(myMetricOut->getMapPaletteColorMapping(curColumn)) = *(inputMetric->getMapPaletteColorMapping(i));
                ++curColumn;
            }
        }
    }
}
