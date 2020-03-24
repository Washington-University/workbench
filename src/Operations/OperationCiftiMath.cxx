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

#include "OperationCiftiMath.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMathExpression.h"
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "MultiDimIterator.h"

#include <iostream>

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
    
    ret->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    
    ParameterComponent* varOpt = ret->createRepeatableParameter(3, "-var", "a cifti file to use as a variable");
    varOpt->addStringParameter(1, "name", "the name of the variable, as used in the expression");
    varOpt->addCiftiParameter(2, "cifti", "the cifti file to use as this variable");
    ParameterComponent* selectOpt = varOpt->createRepeatableParameter(3, "-select", "select a single index from a dimension");//repeatable option to repeatable option
    selectOpt->addIntegerParameter(1, "dim", "the dimension to select from (1-based)");
    selectOpt->addIntegerParameter(2, "index", "the index to use (1-based)");
    selectOpt->createOptionalParameter(3, "-repeat", "repeat the selected values for each index of output in this dimension");//with a repeat option
    
    OptionalParameter* fixNanOpt = ret->createOptionalParameter(4, "-fixnan", "replace NaN results with a value");
    fixNanOpt->addDoubleParameter(1, "replace", "value to replace NaN with");
    
    ret->createOptionalParameter(5, "-override-mapping-check", "don't check the mappings for compatibility, only check length");
    
    AString myText = AString("This command evaluates <expression> at each matrix element independently.  ") +
                             "There must be at least one -var option (to get the output layout from), even if the <name> specified in it isn't used in <expression>.\n\n" +
                             "To select a single column from a 2D file (most cifti files are 2D), use -select 1 <index>, where <index> is 1-based.  " +
                             "To select a single row from a 2D file, use -select 2 <index>.  " +
                             "Where -select is not used, the cifti files must have compatible mappings (e.g., brain models and parcels mappings must match exactly except for parcel names).  " +
                             "Use -override-mapping-check to skip this checking.\n\n" +
                             "Filenames are not valid in <expression>, use a variable name and a -var option with matching <name> to specify an input file.  " +
                             "The format of <expression> is as follows:\n\n";
    myText += CaretMathExpression::getExpressionHelpInfo();
    ret->setHelpText(myText);
    return ret;
}

void OperationCiftiMath::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString expression = myParams->getString(1);
    CaretMathExpression myExpr(expression);
    cout << "parsed '" + expression + "' as '" + myExpr.toString() + "'" << endl;
    vector<AString> myVarNames = myExpr.getVarNames();
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
    const vector<ParameterComponent*>& myVarOpts = myParams->getRepeatableParameterInstances(3);
    OptionalParameter* fixNanOpt = myParams->getOptionalParameter(4);
    bool nanfix = false;
    float nanfixval = 0;
    if (fixNanOpt->m_present)
    {
        nanfix = true;
        nanfixval = (float)fixNanOpt->getDouble(1);
    }
    bool overrideMapCheck = myParams->getOptionalParameter(5)->m_present;
    int numInputs = myVarOpts.size();
    int numVars = myVarNames.size();
    vector<CiftiFile*> varCiftiFiles(numVars, (CiftiFile*)NULL);
    if (numInputs == 0 && numVars == 0) throw OperationException("you must specify at least one input file (-var), even if the expression doesn't use a variable");
    CiftiXML outXML;
    QString xmlText;
    vector<int64_t> outDims;//don't even assume 2 dimensions, in case someone makes a 1-d cifti
    vector<vector<int64_t> > selectInfo(numVars);
    for (int i = 0; i < numInputs; ++i)
    {
        AString varName = myVarOpts[i]->getString(1);
        double constVal;
        if (CaretMathExpression::getNamedConstant(varName, constVal))
        {
            throw OperationException("'" + varName + "' is a named constant equal to " + AString::number(constVal, 'g', 15) + ", please use a different variable name");
        }
        const CiftiXML& tempXML = myVarOpts[i]->getCifti(2)->getCiftiXML();
        int thisNumDims = tempXML.getNumberOfDimensions();
        vector<int64_t> thisSelectInfo(thisNumDims, -1);
        vector<bool> thisRepeat(thisNumDims, false);
        const vector<ParameterComponent*>& thisSelectOpts = myVarOpts[i]->getRepeatableParameterInstances(3);
        for (int j = 0; j < (int)thisSelectOpts.size(); ++j)
        {
            int dim = (int)thisSelectOpts[j]->getInteger(1) - 1;
            int64_t thisIndex = thisSelectOpts[j]->getInteger(2) - 1;
            if (dim >= (int)thisSelectInfo.size())
            {
                if (thisIndex != 0) throw OperationException("-select used  for variable '" + varName + "' with index other than 1 on nonexistent dimension");
                thisSelectInfo.resize(dim + 1, -1);
                thisRepeat.resize(dim + 1, false);
            }
            thisSelectInfo[dim] = thisIndex;
            thisRepeat[dim] = thisSelectOpts[j]->getOptionalParameter(3)->m_present;
        }
        bool found = false;
        for (int j = 0; j < numVars; ++j)
        {
            if (varName == myVarNames[j])
            {
                if (varCiftiFiles[j] != NULL) throw OperationException("variable '" + varName + "' specified more than once");
                found = true;
                varCiftiFiles[j] = myVarOpts[i]->getCifti(2);
                selectInfo[j] = thisSelectInfo;//copy selection info
                break;
            }
        }
        if (!found && (numVars != 0 || numInputs != 1))//supress warning when a single -var is used with a constant expression, as required per help
        {
            CaretLogWarning("variable '" + varName + "' not used in expression");
        }
        int newNumDims = (int)max(thisSelectInfo.size(), outDims.size());//now, to figure out the output dimensions with -select and -repeat
        for (int j = 0; j < newNumDims; ++j)
        {
            if (j >= (int)outDims.size())//need to expand output dimensions
            {
                outXML.setNumberOfDimensions(j + 1);//does not clear existing mappings
                outDims.push_back(-1);//unknown length
            }
            if (j >= (int)thisSelectInfo.size())//need to expand input info
            {
                thisSelectInfo.push_back(-1);//use "all" indices, but there is only 1 (virtual) index, pushing 0 should have same effect
                thisRepeat.push_back(false);//repeat not specified
            }
            if (outDims[j] == -1)//if we don't know the output length yet, put it in if we have it (-repeat not specified)
            {
                if (thisSelectInfo[j] == -1)//no -select for this dimension, use all maps
                {
                    if (j < thisNumDims)
                    {
                        outDims[j] = tempXML.getDimensionLength(j);
                        outXML.setMap(j, *(tempXML.getMap(j)));//copy the mapping type, since this input defines this dimension
                    } else {//if higher dimension than the file has, transparently say it is of length 1, and don't use the mapping
                        outDims[j] = 1;
                    }
                } else {//-select was used
                    if (!thisRepeat[j])//if -repeat wasn't used, output length is 1
                    {
                        outDims[j] = 1;
                    }
                }
            } else {
                if (thisSelectInfo[j] == -1)//-select was not used
                {
                    if (j < thisNumDims)
                    {
                        if (outDims[j] != tempXML.getDimensionLength(j))
                        {
                            throw OperationException("variable '" + varName + "' has length " + AString::number(tempXML.getDimensionLength(j)) +
                                                    " for dimension " + AString::number(j + 1) + " while previous -var options require a length of " + AString::number(outDims[j]));
                        }
                        if (outXML.getMap(j) == NULL)//dimension was set to 1 by -select, but didn't set a mapping, so borrow from here
                        {
                            outXML.setMap(j, *(tempXML.getMap(j)));
                        } else {//test mapping types for compatibility since -select wasn't used
                            AString explanation;
                            if (!overrideMapCheck && !outXML.getMap(j)->approximateMatch(*(tempXML.getMap(j)), &explanation))
                            {
                                throw OperationException("variable " + varName + "'s " + CiftiMappingType::mappingTypeToName(tempXML.getMap(j)->getType()) +
                                                         " mapping on dimension " + AString::number(j + 1) + " doesn't match mappings from earlier -var options: '" + explanation + "'");
                            }
                        }
                    } else {
                        if (outDims[j] != 1)
                        {
                            throw OperationException(AString("variable '" + varName + "' is of lower dimensionality than output, ") +
                                                            "and the length of output dimension " + AString::number(j + 1) + " is " +
                                                            AString::number(outDims[j]) + ", you might want to use -select with -repeat");
                        }
                    }
                } else {
                    if (!thisRepeat[j])
                    {
                        if (outDims[j] != 1)
                        {
                            throw OperationException("variable '" + varName + "' uses -select for dimension " + AString::number(j + 1) +
                                                     ", but previous -var options require a length of " + AString::number(outDims[j]));
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < numVars; ++i)
    {
        if (varCiftiFiles[i] == NULL) throw OperationException("no -var option specified for variable '" + myVarNames[i] + "'");
    }
    CiftiScalarsMap dummyMap;//make an empty length-1 scalar map for dimensions we don't have a mapping for
    dummyMap.setLength(1);
    for (int i = 0; i < outXML.getNumberOfDimensions(); ++i)
    {
        if (outDims[i] == -1) throw OperationException("all -var options used -select and -repeat for dimension " +
                                                       AString::number(i + 1) + ", there is no file to get the dimension length from");
        if (outXML.getMap(i) == NULL)//-select was used in all variables for this dimension, so we don't have a mapping
        {
            outXML.setMap(i, dummyMap);//so, make it a length-1 scalar with no name and empty metadata
        }
        CaretAssert(outDims[i] == outXML.getDimensionLength(i));
    }
    if (outXML.getNumberOfDimensions() < 1) throw OperationException("output must have at least 1 dimension");
    myCiftiOut->setCiftiXML(outXML);
    vector<float> values(numVars), scratchRow(outDims[0]);
    vector<vector<float> > inputRows(numVars);
    vector<vector<int64_t> > loadedRow(numVars);//to detect and prevent rereading the same row
    for (int v = 0; v < numVars; ++v)
    {
        inputRows[v].resize(varCiftiFiles[v]->getCiftiXML().getDimensionLength(CiftiXML::ALONG_ROW));
        loadedRow[v].resize(varCiftiFiles[v]->getCiftiXML().getNumberOfDimensions() - 1, -1);//we always load a full row, so ignore first dim
    }
    for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
    {
        for (int v = 0; v < numVars; ++v)//first, retrieve whichever rows are needed
        {
            bool needToLoad = false;
            for (int dim = 0; dim < (int)loadedRow[v].size(); ++dim)
            {
                int64_t indexNeeded = -1;
                if (selectInfo[v][dim + 1] == -1)
                {
                    CaretAssert(dim + 1 < (int)outDims.size());//"match to output index" can't work past output dimensionality
                    indexNeeded = (*iter)[dim];//NOTE: iter also doesn't include the first dim
                } else {
                    indexNeeded = selectInfo[v][dim + 1];
                }
                if (indexNeeded != loadedRow[v][dim])
                {
                    needToLoad = true;
                    loadedRow[v][dim] = indexNeeded;
                }
            }
            if (needToLoad)
            {
                varCiftiFiles[v]->getRow(inputRows[v].data(), loadedRow[v]);
            }
        }
        for (int j = 0; j < outDims[0]; ++j)
        {
            for (int v = 0; v < numVars; ++v)//now we check for select along row
            {
                if (selectInfo[v][0] == -1)
                {
                    values[v] = inputRows[v][j];
                } else {
                    values[v] = inputRows[v][selectInfo[v][0]];
                }
            }
            scratchRow[j] = (float)myExpr.evaluate(values);
            if (nanfix && scratchRow[j] != scratchRow[j])
            {
                scratchRow[j] = nanfixval;
            }
        }
        myCiftiOut->setRow(scratchRow.data(), *iter);
    }
}
