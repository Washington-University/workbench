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
#include "OperationVolumeMath.h"
#include "OperationException.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumeMath::getCommandSwitch()
{
    return "-volume-math";
}

AString OperationVolumeMath::getShortDescription()
{
    return "EVALUATE EXPRESSION ON VOLUME FILES";
}

OperationParameters* OperationVolumeMath::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "expression", "the expression to evaluate, in quotes");
    
    ret->addVolumeOutputParameter(2, "volume-out", "the output volume");
    
    ParameterComponent* varOpt = ret->createRepeatableParameter(3, "-var", "a volume file to use as a variable");
    varOpt->addStringParameter(1, "name", "the name of the variable, as used in the expression");
    varOpt->addVolumeParameter(2, "volume", "the volume file to use as this variable");
    OptionalParameter* subvolSelect = varOpt->createOptionalParameter(3, "-subvolume", "select a single subvolume");
    subvolSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    OptionalParameter* fixNanOpt = ret->createOptionalParameter(4, "-fixnan", "replace NaN results with a value");
    fixNanOpt->addDoubleParameter(1, "replace", "value to replace NaN with");
    
    AString myText = AString("This command evaluates the expression given at each voxel independently.  ") +
                        "There must be at least one input volume (to get the volume space from), and all volumes must have " +
                        "the same volume space.  " +
                        "If the -subvolume option is given to any -var option, all -var options must specify single single subvolume volume files, or have the -subvolume option specified.  " +
                        "If no -var option is given the -subvolume option, all volume files specified must have the same number of subvolumes.  " +
                        "The format of the expressions is as follows:\n\n";
    myText += CaretMathExpression::getExpressionHelpInfo();
    ret->setHelpText(myText);
    return ret;
}

void OperationVolumeMath::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString expression = myParams->getString(1);
    CaretMathExpression myExpr(expression);
    const vector<AString>& myVarNames = myExpr.getVarNames();
    VolumeFile* myVolOut = myParams->getOutputVolume(2);
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
    vector<VolumeFile*> varVolumes(numVars, (VolumeFile*)NULL);
    vector<int> varSubvolumes(numVars, -1);
    bool allSubvolsMode = true;
    if (numInputs == 0) throw OperationException("you must specify at least one input volume (-var), even if the expression doesn't use a variable");
    VolumeFile* first = myVarOpts[0]->getVolume(2);
    for (int i = 0; i < numInputs; ++i)
    {
        AString varName = myVarOpts[i]->getString(1);
        for (int j = 0; j < numVars; ++j)
        {
            if (varName == myVarNames[j])
            {
                if (varVolumes[j] != NULL) throw OperationException("variable '" + varName + "' specified more than once");
                varVolumes[j] = myVarOpts[i]->getVolume(2);
                OptionalParameter* subvolSelect = myVarOpts[i]->getOptionalParameter(3);
                if (subvolSelect->m_present)
                {
                    allSubvolsMode = false;
                    varSubvolumes[j] = varVolumes[j]->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
                    if (varSubvolumes[j] == -1) throw OperationException("could not find map '" + subvolSelect->getString(1) +
                                                                        "' in volume file for '" + varName + "'");
                }
                break;
            }
        }
    }
    int numSubvols = 1;
    if (numVars > 0) first = varVolumes[0];
    for (int i = 0; i < numVars; ++i)
    {
        if (varVolumes[i] == NULL) throw OperationException("no -var option specified for variable '" + myVarNames[i] + "'");
        if (!varVolumes[i]->matchesVolumeSpace(first)) throw OperationException("volume file for '" + myVarNames[i] + "' has a different volume space than " +
                                                                                    "the volume file for '" + myVarNames[0] + "'");
        if (allSubvolsMode)
        {
            if (i == 0)
            {
                numSubvols = varVolumes[i]->getNumberOfMaps();
            } else {
                if (varVolumes[i]->getNumberOfMaps() != numSubvols) throw OperationException("volume file for '" + myVarNames[i] + "' has a different number of subvolumes than " +
                                                                                                "the volume file for '" + myVarNames[0] + "'");
            }
        } else {
            if (varSubvolumes[i] == -1)
            {
                if (varVolumes[i]->getNumberOfMaps() == 1)
                {
                    varSubvolumes[i] = 0;
                } else {
                    throw OperationException("volume file for '" + myVarNames[i] + "' has more than one subvolume, and -subvolume was not specified for it");
                }
            }
        }
    }
    vector<float> values(numVars);
    vector<int64_t> origDims = first->getOriginalDimensions();
    if (allSubvolsMode)
    {
        myVolOut->reinitialize(origDims, first->getVolumeSpace(), 1, first->getType());
        for (int s = 0; s < numSubvols; ++s)
        {
            for (int k = 0; k < origDims[2]; ++k)
            {
                for (int j = 0; j < origDims[1]; ++j)
                {
                    for (int i = 0; i < origDims[0]; ++i)
                    {
                        for (int v = 0; v < numVars; ++v)
                        {
                            values[v] = varVolumes[v]->getValue(i, j, k, s);
                        }
                        float tempf = (float)myExpr.evaluate(values);
                        if (nanfix && tempf != tempf)
                        {
                            tempf = nanfixval;
                        }
                        myVolOut->setValue(tempf, i, j, k, s);
                    }
                }
            }
        }
    } else {
        origDims.resize(3);
        myVolOut->reinitialize(origDims, first->getVolumeSpace(), 1, first->getType());
        for (int k = 0; k < origDims[2]; ++k)
        {
            for (int j = 0; j < origDims[1]; ++j)
            {
                for (int i = 0; i < origDims[0]; ++i)
                {
                    for (int v = 0; v < numVars; ++v)
                    {
                        values[v] = varVolumes[v]->getValue(i, j, k, varSubvolumes[v]);
                    }
                    float tempf = (float)myExpr.evaluate(values);
                    if (nanfix && tempf != tempf)
                    {
                        tempf = nanfixval;
                    }
                    myVolOut->setValue(tempf, i, j, k);
                }
            }
        }
    }
}
