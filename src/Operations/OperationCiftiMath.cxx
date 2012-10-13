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
#include "CiftiFile.h"
#include "OperationCiftiMath.h"
#include "OperationException.h"

using namespace caret;
using namespace std;

AString OperationCiftiMath::getCommandSwitch()
{
    return "-cifti-math";
}

AString OperationCiftiMath::getShortDescription()
{
    return "EVALUATE EXPRESSION ON CIFTI FILES";
}

OperationParameters* OperationCiftiMath::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "expression", "the expression to evaluate, in quotes");
    
    ret->addCiftiOutputParameter(2, "volume-out", "the output cifti file");
    
    ParameterComponent* varOpt = ret->createRepeatableParameter(3, "-var", "a cifti file to use as a variable");
    varOpt->addStringParameter(1, "name", "the name of the variable, as used in the expression");
    varOpt->addCiftiParameter(2, "cifti", "the cifti file to use as this variable");
    
    OptionalParameter* fixNanOpt = ret->createOptionalParameter(4, "-fixnan", "replace NaN results with a value");
    fixNanOpt->addDoubleParameter(1, "replace", "value to replace NaN with");
    
    AString myText = AString("This command evaluates the expression given at each brainordinate independently.  ") +
                        "There must be at least one input cifti file (to get the output layout from), and all cifti files must have " +
                        "the same layout, number of timesteps, etc.  " +
                        "The format of the expressions is as follows:\n\n";
    myText += CaretMathExpression::getExpressionHelpInfo();
    ret->setHelpText(myText);
    return ret;
}

void OperationCiftiMath::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString expression = myParams->getString(1);
    CaretMathExpression myExpr(expression);
    const vector<AString>& myVarNames = myExpr.getVarNames();
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
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
    vector<CiftiFile*> varCiftiFiles(numVars, NULL);
    if (numInputs == 0) throw OperationException("you must specify at least one input volume (-var), even if the expression doesn't use a variable");
    CiftiFile* first = myVarOpts[0]->getCifti(2);
    for (int i = 0; i < numInputs; ++i)
    {
        AString varName = myVarOpts[i]->getString(1);
        for (int j = 0; j < numVars; ++j)
        {
            if (varName == myVarNames[j])
            {
                if (varCiftiFiles[j] != NULL) throw OperationException("variable '" + varName + "' specified more than once");
                varCiftiFiles[j] = myVarOpts[i]->getCifti(2);
                break;
            }
        }
    }
    if (numVars > 0 && varCiftiFiles[0] != NULL) first = varCiftiFiles[0];
    const CiftiXML& firstXML = first->getCiftiXML();
    for (int i = 0; i < numVars; ++i)
    {
        if (varCiftiFiles[i] == NULL) throw OperationException("no -var option specified for variable '" + myVarNames[i] + "'");
        const CiftiXML& thisXML = varCiftiFiles[i]->getCiftiXML();
        if (firstXML != thisXML) throw OperationException("cifti file for '" + myVarNames[i] + "' has a different layout than " +
                                                                                    "the cifti file for '" + myVarNames[0] + "'");
    }
    myCiftiOut->setCiftiXML(firstXML);
    int numRows = firstXML.getNumberOfRows(), numCols = firstXML.getNumberOfColumns();
    vector<float> values(numVars), scratchRow(numCols);
    vector<vector<float> > inputRows(numVars, vector<float>(numCols));
    for (int i = 0; i < numRows; ++i)
    {
        for (int v = 0; v < numVars; ++v)
        {
            varCiftiFiles[v]->getRow(inputRows[v].data(), i);
        }
        for (int j = 0; j < numCols; ++j)
        {
            for (int v = 0; v < numVars; ++v)
            {
                values[v] = inputRows[v][j];
            }
            scratchRow[j] = (float)myExpr.evaluate(values);
            if (nanfix && scratchRow[j] != scratchRow[j])
            {
                scratchRow[j] = nanfixval;
            }
        }
        myCiftiOut->setRow(scratchRow.data(), i);
    }
}
