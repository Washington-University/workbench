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
    ParameterComponent* columnOpt = metricOpt->createRepeatableParameter(2, "-column", "select a single column to use");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    OptionalParameter* upToOpt = columnOpt->createOptionalParameter(2, "-up-to", "use an inclusive range of columns");
    upToOpt->addStringParameter(1, "last-column", "the number or name of the last column to include");
    upToOpt->createOptionalParameter(2, "-reverse", "use the range in reverse order");
    
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
    const vector<ParameterComponent*>& myInputs = myParams->getRepeatableParameterInstances(2);
    int numInputs = (int)myInputs.size();
    if (numInputs == 0) throw OperationException("no inputs specified");
    const MetricFile* firstMetric = myInputs[0]->getMetric(1);
    int numOutColumns = 0;
    int numNodes = firstMetric->getNumberOfNodes();
    StructureEnum::Enum myStruct = firstMetric->getStructure();
    for (int i = 0; i < numInputs; ++i)
    {
        const MetricFile* inputMetric = myInputs[i]->getMetric(1);
        if (numNodes != inputMetric->getNumberOfNodes()) throw OperationException("file '" + inputMetric->getFileName() + "' has a different number of nodes than the first");
        if (myStruct != inputMetric->getStructure()) throw OperationException("file '" + inputMetric->getFileName() + "' has a different structure than the first");
        const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numColumnOpts = (int)columnOpts.size();
        if (numColumnOpts > 0)
        {
            for (int j = 0; j < numColumnOpts; ++j)
            {
                int initialColumn = inputMetric->getMapIndexFromNameOrNumber(columnOpts[j]->getString(1));
                if (initialColumn < 0) throw OperationException("column '" + columnOpts[j]->getString(1) + "' not found in file '" + inputMetric->getFileName() + "'");
                OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int finalColumn = inputMetric->getMapIndexFromNameOrNumber(upToOpt->getString(1));
                    if (finalColumn < 0) throw OperationException("ending column '" + upToOpt->getString(1) + "' not found in file '" + inputMetric->getFileName() + "'");
                    if (finalColumn < initialColumn) throw OperationException("ending column '" + upToOpt->getString(1) + "' occurs before starting column '"
                                                                            + columnOpts[j]->getString(1) + "' in file '" + inputMetric->getFileName() + "'");
                    numOutColumns += finalColumn - initialColumn + 1;//inclusive - we don't need to worry about reversing for counting, though
                } else {
                    numOutColumns += 1;
                }
            }
        } else {
            numOutColumns += inputMetric->getNumberOfColumns();
        }
    }
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numOutColumns);
    myMetricOut->setStructure(myStruct);
    int curColumn = 0;
    for (int i = 0; i < numInputs; ++i)
    {
        const MetricFile* inputMetric = myInputs[i]->getMetric(1);
        const vector<ParameterComponent*>& columnOpts = myInputs[i]->getRepeatableParameterInstances(2);
        int numColumnOpts = (int)columnOpts.size();
        if (numColumnOpts > 0)
        {
            for (int j = 0; j < numColumnOpts; ++j)
            {
                int initialColumn = inputMetric->getMapIndexFromNameOrNumber(columnOpts[j]->getString(1));
                OptionalParameter* upToOpt = columnOpts[j]->getOptionalParameter(2);
                if (upToOpt->m_present)
                {
                    int finalColumn = inputMetric->getMapIndexFromNameOrNumber(upToOpt->getString(1));
                    bool reverse = upToOpt->getOptionalParameter(2)->m_present;
                    if (reverse)
                    {
                        for (int c = finalColumn; c >= initialColumn; --c)
                        {
                            myMetricOut->setValuesForColumn(curColumn, inputMetric->getValuePointerForColumn(c));
                            myMetricOut->setColumnName(curColumn, inputMetric->getColumnName(c));
                            *(myMetricOut->getMapPaletteColorMapping(curColumn)) = *(inputMetric->getMapPaletteColorMapping(c));
                            ++curColumn;
                        }
                    } else {
                        for (int c = initialColumn; c <= finalColumn; ++c)
                        {
                            myMetricOut->setValuesForColumn(curColumn, inputMetric->getValuePointerForColumn(c));
                            myMetricOut->setColumnName(curColumn, inputMetric->getColumnName(c));
                            *(myMetricOut->getMapPaletteColorMapping(curColumn)) = *(inputMetric->getMapPaletteColorMapping(c));
                            ++curColumn;
                        }
                    }
                } else {
                    myMetricOut->setValuesForColumn(curColumn, inputMetric->getValuePointerForColumn(initialColumn));
                    myMetricOut->setColumnName(curColumn, inputMetric->getColumnName(initialColumn));
                    *(myMetricOut->getMapPaletteColorMapping(curColumn)) = *(inputMetric->getMapPaletteColorMapping(initialColumn));
                    ++curColumn;
                }
            }
        } else {
            int numColumns = inputMetric->getNumberOfColumns();
            for (int j = 0; j < numColumns; ++j)
            {
                myMetricOut->setValuesForColumn(curColumn, inputMetric->getValuePointerForColumn(j));
                myMetricOut->setColumnName(curColumn, inputMetric->getColumnName(j));
                *(myMetricOut->getMapPaletteColorMapping(curColumn)) = *(inputMetric->getMapPaletteColorMapping(j));
                ++curColumn;
            }
        }
    }
    CaretAssert(curColumn == numOutColumns);
}
