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

#include "OperationVolumeMath.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMathExpression.h"
#include "VolumeFile.h"

#include <iostream>

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
    varOpt->createOptionalParameter(4, "-repeat", "reuse a single subvolume for each subvolume of calculation");
    
    OptionalParameter* fixNanOpt = ret->createOptionalParameter(4, "-fixnan", "replace NaN results with a value");
    fixNanOpt->addDoubleParameter(1, "replace", "value to replace NaN with");
    
    AString myText = AString("This command evaluates <expression> at each voxel independently.  ") +
                        "There must be at least one -var option (to get the volume space from), even if the <name> specified in it isn't used in <expression>.  " +
                        "All volumes must have the same volume space.  " +
                        "Filenames are not valid in <expression>, use a variable name and a -var option with matching <name> to specify an input file.  " +
                        "If the -subvolume option is given to any -var option, only one subvolume is used from that file.  " +
                        "If -repeat is specified, the file must either have only one subvolume, or have the -subvolume option specified.  " +
                        "All files that don't use -repeat must have the same number of subvolumes requested to be used.  " +
                        "The format of <expression> is as follows:\n\n";
    myText += CaretMathExpression::getExpressionHelpInfo();
    ret->setHelpText(myText);
    return ret;
}

void OperationVolumeMath::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString expression = myParams->getString(1);
    CaretMathExpression myExpr(expression);
    cout << "parsed '" + expression + "' as '" + myExpr.toString() + "'" << endl;
    vector<AString> myVarNames = myExpr.getVarNames();
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
    vector<VolumeFile*> varVolumes(numVars, NULL);
    vector<int> varSubvolumes(numVars, -1);
    if (numInputs == 0 && numVars == 0) throw OperationException("you must specify at least one input volume (-var), even if the expression doesn't use a variable");
    VolumeFile* first = NULL, *namefile = NULL;
    VolumeSpace mySpace;
    vector<int64_t> outDims;
    int numSubvols = -1;
    for (int i = 0; i < numInputs; ++i)
    {
        if (i == 0)
        {
            first = myVarOpts[0]->getVolume(2);
            mySpace = first->getVolumeSpace();
        }
        AString varName = myVarOpts[i]->getString(1);
        double constVal;
        if (CaretMathExpression::getNamedConstant(varName, constVal))
        {
            throw OperationException("'" + varName + "' is a named constant equal to " + AString::number(constVal, 'g', 15) + ", please use a different variable name");
        }
        VolumeFile* thisVolume = myVarOpts[i]->getVolume(2);
        if (thisVolume->getNumberOfComponents() != 1)
        {
            throw OperationException("volume file for variable '" + varName + "' has multiple components, this is not currently supported in -volume-math");
        }
        int thisSubvols = thisVolume->getNumberOfMaps();
        OptionalParameter* subvolSelect = myVarOpts[i]->getOptionalParameter(3);
        int useSubvolume = -1;
        if (subvolSelect->m_present)
        {
            thisSubvols = 1;
            useSubvolume = thisVolume->getMapIndexFromNameOrNumber(subvolSelect->getString(1));
            if (useSubvolume == -1) throw OperationException("could not find map '" + subvolSelect->getString(1) +
                                                                "' in volume file for '" + varName + "'");
        }
        bool repeat = myVarOpts[i]->getOptionalParameter(4)->m_present;
        if (!thisVolume->matchesVolumeSpace(mySpace))
        {
            throw OperationException("volume file for variable '" + varName + "' has different volume space than the first volume file");
        }
        if (repeat)
        {
            if (thisSubvols != 1)
            {
                throw OperationException("-repeat specified without -subvolume for variable '" + varName + "', but volume file has " + AString::number(thisSubvols) + " subvolumes");
            }
            if (useSubvolume == -1) useSubvolume = 0;//-1 means use same input subvolume as current output subvolume, so we need to fix the special case of -repeat on single subvolume file without -subvolume
        } else {
            if (numSubvols == -1)//then this is the first one that doesn't use -repeat
            {
                numSubvols = thisSubvols;
                outDims = thisVolume->getOriginalDimensions();
                if (useSubvolume == -1)
                {
                    namefile = thisVolume;//if it also doesn't use -subvolume, take map names from it
                } else {
                    outDims.resize(3);//change to output only one subvolume in the simplest way
                }
            } else {
                if (numSubvols != thisSubvols)
                {
                    if (useSubvolume == -1)
                    {
                        throw OperationException("volume file for variable '" + varName + "' has " + AString::number(thisSubvols) + " subvolume(s), but previous volume files have " +
                                                 AString::number(numSubvols) + " subvolume(s) requested to be used");
                    } else {
                        throw OperationException("-subvolume specified without -repeat for variable '" + varName + "', but previous volume files have have " +
                                                 AString::number(numSubvols) + " subvolumes requested to be used");
                    }
                }
            }
        }
        bool found = false;
        for (int j = 0; j < numVars; ++j)
        {
            if (varName == myVarNames[j])
            {
                if (varVolumes[j] != NULL) throw OperationException("variable '" + varName + "' specified more than once");
                varVolumes[j] = thisVolume;
                varSubvolumes[j] = useSubvolume;
                found = true;
                break;
            }
        }
        if (!found && (numVars != 0 || numInputs != 1))//supress warning when a single -var is used with a constant expression, as required per help
        {
            CaretLogWarning("variable '" + varName + "' not used in expression");
        }
    }
    for (int i = 0; i < numVars; ++i)
    {
        if (varVolumes[i] == NULL) throw OperationException("no -var option specified for variable '" + myVarNames[i] + "'");
    }
    if (numSubvols == -1)
    {
        throw OperationException("all -var options used -repeat, there is no file to get number of desired output subvolumes from");
    }
    int64_t frameSize = outDims[0] * outDims[1] * outDims[2];
    vector<float> values(numVars), outFrame(frameSize);
    vector<const float*> inputFrames(numVars);
    myVolOut->reinitialize(outDims, first->getSform());//DO NOT take volume type from first volume, because we don't check for or copy label tables, nor do we want to
    for (int s = 0; s < numSubvols; ++s)
    {
        if (namefile != NULL) myVolOut->setMapName(s, namefile->getMapName(s));
        for (int v = 0; v < numVars; ++v)
        {
            if (varSubvolumes[v] == -1)
            {
                inputFrames[v] = varVolumes[v]->getFrame(s);
            } else {
                inputFrames[v] = varVolumes[v]->getFrame(varSubvolumes[v]);
            }
        }
        for (int64_t i = 0; i < frameSize; ++i)
        {
            for (int v = 0; v < numVars; ++v)
            {
                values[v] = inputFrames[v][i];
            }
            float tempf = (float)myExpr.evaluate(values);
            if (nanfix && tempf != tempf)
            {
                tempf = nanfixval;
            }
            outFrame[i] = tempf;
        }
        myVolOut->setFrame(outFrame.data(), s);
    }
}
