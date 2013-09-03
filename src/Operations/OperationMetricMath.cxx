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
#include "CaretMathExpression.h"
#include "MetricFile.h"
#include "OperationMetricMath.h"
#include "OperationException.h"

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
    
    OptionalParameter* fixNanOpt = ret->createOptionalParameter(4, "-fixnan", "replace NaN results with a value");
    fixNanOpt->addDoubleParameter(1, "replace", "value to replace NaN with");
    
    AString myText = AString("This command evaluates <expression> at each surface vertex independently.  ") +
                        "There must be at least one -var option (to get the structure and number of vertices from), even if the <name> specified in it isn't used in <expression>.  " +
                        "All metrics must have the same number of vertices.  " +
                        "Filenames are not valid in <expression>, use a variable name and a -var option with matching <name> to specify an input file.  " +
                        "If the -column option is given to any -var option, all -var options must specify single column metric files, or have the -column option specified.  " +
                        "If no -var option is given the -column option, all metric files specified must have the same number of columns.  " +
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
    bool allColumnsMode = true;
    if (numInputs == 0) throw OperationException("you must specify at least one input metric (-var), even if the expression doesn't use a variable");
    int numNodes = myVarOpts[0]->getMetric(2)->getNumberOfNodes();
    StructureEnum::Enum myStructure = myVarOpts[0]->getMetric(2)->getStructure();
    for (int i = 0; i < numInputs; ++i)
    {
        AString varName = myVarOpts[i]->getString(1);
        double constVal;
        if (CaretMathExpression::getNamedConstant(varName, constVal))
        {
            throw OperationException("'" + varName + "' is a named constant equal to " + AString::number(constVal, 'g', 15) + ", please use a different variable name");
        }
        for (int j = 0; j < numVars; ++j)
        {
            if (varName == myVarNames[j])
            {
                if (varMetrics[j] != NULL) throw OperationException("variable '" + varName + "' specified more than once");
                varMetrics[j] = myVarOpts[i]->getMetric(2);
                OptionalParameter* columnSelect = myVarOpts[i]->getOptionalParameter(3);
                if (columnSelect->m_present)
                {
                    allColumnsMode = false;
                    metricColumns[j] = varMetrics[j]->getMapIndexFromNameOrNumber(columnSelect->getString(1));
                    if (metricColumns[j] == -1) throw OperationException("could not find map '" + columnSelect->getString(1) +
                                                                        "' in metric file for '" + varName + "'");
                }
                break;
            }
        }
    }
    int numColumns = 1;
    if (numVars > 0 && varMetrics[0] != NULL) numNodes = varMetrics[0]->getNumberOfNodes();//in case the first -var is unused, and has a different number of nodes
    for (int i = 0; i < numVars; ++i)
    {
        if (varMetrics[i] == NULL) throw OperationException("no -var option specified for variable '" + myVarNames[i] + "'");
        if (varMetrics[i]->getNumberOfNodes() != numNodes) throw OperationException("metric file for '" + myVarNames[i] + "' has a different number of vertices than " +
                                                                                    "the metric file for '" + myVarNames[0] + "'");
        if (allColumnsMode)
        {
            if (i == 0)
            {
                numColumns = varMetrics[i]->getNumberOfColumns();
            } else {
                if (varMetrics[i]->getNumberOfColumns() != numColumns) throw OperationException("metric file for '" + myVarNames[i] + "' has a different number of columns than " +
                                                                                                "the metric file for '" + myVarNames[0] + "'");
            }
        } else {
            if (metricColumns[i] == -1)
            {
                if (varMetrics[i]->getNumberOfColumns() == 1)
                {
                    metricColumns[i] = 0;
                } else {
                    throw OperationException("metric file for '" + myVarNames[i] + "' has more than one column, and -column was not specified for it");
                }
            }
        }
    }
    vector<float> values(numVars), rowScratch(numNodes);
    vector<const float*> columnPointers(numVars);
    if (allColumnsMode)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
        myMetricOut->setStructure(myStructure);
        for (int j = 0; j < numColumns; ++j)
        {
            for (int v = 0; v < numVars; ++v)
            {
                columnPointers[v] = varMetrics[v]->getValuePointerForColumn(j);
            }
            for (int i = 0; i < numNodes; ++i)
            {
                for (int v = 0; v < numVars; ++v)
                {
                    values[v] = columnPointers[v][i];
                }
                rowScratch[i] = (float)myExpr.evaluate(values);
                if (nanfix && rowScratch[i] != rowScratch[i])
                {
                    rowScratch[i] = nanfixval;
                }
            }
            myMetricOut->setValuesForColumn(j, rowScratch.data());
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(myStructure);
        for (int v = 0; v < numVars; ++v)
        {
            columnPointers[v] = varMetrics[v]->getValuePointerForColumn(metricColumns[v]);
        }
        for (int i = 0; i < numNodes; ++i)
        {
            for (int v = 0; v < numVars; ++v)
            {
                values[v] = columnPointers[v][i];
            }
            rowScratch[i] = (float)myExpr.evaluate(values);
            if (nanfix && rowScratch[i] != rowScratch[i])
            {
                rowScratch[i] = nanfixval;
            }
        }
        myMetricOut->setValuesForColumn(0, rowScratch.data());
    }
}
