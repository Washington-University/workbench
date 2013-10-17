/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "OperationMetricMath.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMathExpression.h"
#include "MetricFile.h"

using namespace caret;
using namespace std;

AString OperationMetricMath::getCommandSwitch()
{
    return "-metric-math";
}

AString OperationMetricMath::getShortDescription()
{
    return "EVALUATE EXPRESSION ON METRIC FILES";
}

OperationParameters* OperationMetricMath::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "expression", "the expression to evaluate, in quotes");
    
    ret->addMetricOutputParameter(2, "metric-out", "the output metric");
    
    ParameterComponent* varOpt = ret->createRepeatableParameter(3, "-var", "a metric to use as a variable");
    varOpt->addStringParameter(1, "name", "the name of the variable, as used in the expression");
    varOpt->addMetricParameter(2, "metric", "the metric file to use as this variable");
    OptionalParameter* columnSelect = varOpt->createOptionalParameter(3, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    varOpt->createOptionalParameter(4, "-repeat", "reuse a single column for each column of calculation");
    
    OptionalParameter* fixNanOpt = ret->createOptionalParameter(4, "-fixnan", "replace NaN results with a value");
    fixNanOpt->addDoubleParameter(1, "replace", "value to replace NaN with");
    
    AString myText = AString("This command evaluates <expression> at each surface vertex independently.  ") +
                        "There must be at least one -var option (to get the structure, number of vertices, and number of columns from), even if the <name> specified in it isn't used in <expression>.  " +
                        "All metrics must have the same number of vertices.  " +
                        "Filenames are not valid in <expression>, use a variable name and a -var option with matching <name> to specify an input file.  " +
                        "If the -column option is given to any -var option, only one column is used from that file.  " +
                        "If -repeat is specified, the file must either be a single column, or have the -column option specified.  " +
                        "All files that don't use -repeat must have the same number of columns requested to be used.  " +
                        "The format of <expression> is as follows:\n\n";
    myText += CaretMathExpression::getExpressionHelpInfo();
    ret->setHelpText(myText);
    return ret;
}

void OperationMetricMath::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString expression = myParams->getString(1);
    CaretMathExpression myExpr(expression);
    const vector<AString>& myVarNames = myExpr.getVarNames();
    MetricFile* myMetricOut = myParams->getOutputMetric(2);
    const vector<ParameterComponent*>& myVarOpts = *(myParams->getRepeatableParameterInstances(3));
    OptionalParameter* fixNanOpt = myParams->getOptionalParameter(4);
    bool nanfix = false;
    float nanfixval = 0;
    if (fixNanOpt->m_present)
    {
        nanfix = true;
        nanfixval = (float)fixNanOpt->getDouble(1);
    }
    int numInputs = myVarOpts.size();
    int numVars = myVarNames.size();
    vector<MetricFile*> varMetrics(numVars, (MetricFile*)NULL);
    vector<int> metricColumns(numVars, -1);
    if (numInputs == 0) throw OperationException("you must specify at least one input metric (-var), even if the expression doesn't use a variable");
    int numNodes = myVarOpts[0]->getMetric(2)->getNumberOfNodes();
    StructureEnum::Enum myStructure = myVarOpts[0]->getMetric(2)->getStructure();
    int numColumns = -1;
    bool firstNonrepeat = true;
    for (int i = 0; i < numInputs; ++i)
    {
        AString varName = myVarOpts[i]->getString(1);
        double constVal;
        if (CaretMathExpression::getNamedConstant(varName, constVal))
        {
            throw OperationException("'" + varName + "' is a named constant equal to " + AString::number(constVal, 'g', 15) + ", please use a different variable name");
        }
        MetricFile* thisMetric = myVarOpts[i]->getMetric(2);
        int thisColumns = thisMetric->getNumberOfColumns();
        OptionalParameter* columnSelect = myVarOpts[i]->getOptionalParameter(3);
        int useColumn = -1;
        if (columnSelect->m_present)
        {
            thisColumns = 1;
            useColumn = thisMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
            if (useColumn == -1) throw OperationException("could not find map '" + columnSelect->getString(1) +
                                                                "' in metric file for '" + varName + "'");
        }
        bool repeat = myVarOpts[i]->getOptionalParameter(4)->m_present;
        if (thisMetric->getNumberOfNodes() != numNodes)
        {
            throw OperationException("metric file for variable '" + varName + "' has different number of vertices than the first metric file");
        }
        if (repeat)
        {
            if (thisColumns != 1)
            {
                throw OperationException("-repeat specified without -column for variable '" + varName + "', but metric file has " + AString::number(thisColumns) + " columns");
            }
            if (useColumn == -1) useColumn = 0;//-1 means use same input column as current output column, so we need to fix the special case of -repeat on single column file without -column
        } else {
            if (firstNonrepeat)
            {
                numColumns = thisColumns;
                firstNonrepeat = false;
            } else {
                if (numColumns != thisColumns)
                {
                    if (useColumn == -1)
                    {
                        throw OperationException("metric file for variable '" + varName + "' has " + AString::number(thisColumns) + " column(s), but previous metric files have " +
                                                 AString::number(numColumns) + " column(s) requested to be used");
                    } else {
                        throw OperationException("-column specified without -repeat for variable '" + varName + "', but previous metric files have have " +
                                                 AString::number(numColumns) + " columns requested to be used");
                    }
                }
            }
        }
        bool found = false;
        for (int j = 0; j < numVars; ++j)
        {
            if (varName == myVarNames[j])
            {
                if (varMetrics[j] != NULL) throw OperationException("variable '" + varName + "' specified more than once");
                varMetrics[j] = thisMetric;
                metricColumns[j] = useColumn;
                found = true;
                break;
            }
        }
        if (!found && (numVars != 0 || numInputs != 1))//supress warning when a single -var is used with a constant expression, as required per help
        {
            CaretLogWarning("variable '" + varName + "' not used in expression");
        }
    }
    CaretAssert(numColumns != -1);
    if (numVars > 0 && varMetrics[0] != NULL) numNodes = varMetrics[0]->getNumberOfNodes();//in case the first -var is unused, and has a different number of nodes
    for (int i = 0; i < numVars; ++i)
    {
        if (varMetrics[i] == NULL) throw OperationException("no -var option specified for variable '" + myVarNames[i] + "'");
    }
    vector<float> values(numVars), colScratch(numNodes);
    vector<const float*> columnPointers(numVars);
    myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
    myMetricOut->setStructure(myStructure);
    for (int j = 0; j < numColumns; ++j)
    {
        for (int v = 0; v < numVars; ++v)
        {
            if (metricColumns[v] == -1)
            {
                columnPointers[v] = varMetrics[v]->getValuePointerForColumn(j);
            } else {
                columnPointers[v] = varMetrics[v]->getValuePointerForColumn(metricColumns[v]);
            }
        }
        for (int i = 0; i < numNodes; ++i)
        {
            for (int v = 0; v < numVars; ++v)
            {
                values[v] = columnPointers[v][i];
            }
            colScratch[i] = (float)myExpr.evaluate(values);
            if (nanfix && colScratch[i] != colScratch[i])
            {
                colScratch[i] = nanfixval;
            }
        }
        myMetricOut->setValuesForColumn(j, colScratch.data());
    }
}
