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

#include "OperationConvertWarpfield.h"
#include "OperationException.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString OperationConvertWarpfield::getCommandSwitch()
{
    return "-convert-warpfield";
}

AString OperationConvertWarpfield::getShortDescription()
{
    return "CONVERT A WARPFIELD BETWEEN CONVENTIONS";
}

OperationParameters* OperationConvertWarpfield::getParameters()
{
    OperationParameters* ret = new OperationParameters();

    OptionalParameter* fromWorld = ret->createOptionalParameter(1, "-from-world", "input is a NIFTI 'world' warpfield");
    fromWorld->addStringParameter(1, "input", "the input warpfield");
    
    OptionalParameter* fromFnirt = ret->createOptionalParameter(2, "-from-fnirt", "input is a fnirt warpfield");
    fromFnirt->addStringParameter(1, "input", "the input warpfield");
    fromFnirt->addStringParameter(2, "source-volume", "the source volume used when generating the input warpfield");
    
    OptionalParameter* toWorld = ret->createOptionalParameter(3, "-to-world", "write output as a NIFTI 'world' warpfield");
    toWorld->addStringParameter(1, "output", "output - the output warpfield");//HACK: fake the output formatting, since we don't have a parameter for affine file (hard to do due to multiple on-disk formats)
    
    ParameterComponent* toFnirt = ret->createRepeatableParameter(4, "-to-fnirt", "write output as a fnirt warpfield");
    toFnirt->addStringParameter(1, "output", "output - the output warpfield");//this argument order is different than other commands, yes - but don't change it, so scripts will still work
    toFnirt->addStringParameter(2, "source-volume", "the volume you want to apply the warpfield to");
    
    ret->setHelpText(
        AString("NIFTI world warpfields can be used directly on mm coordinates via sampling the three subvolumes at the coordinate ") +
        "and adding the sampled values to the coordinate vector, they use the NIFTI coordinate system, that is, " +
        "X is left to right, Y is posterior to anterior, and Z is inferior to superior.\n\n" +
        "NOTE: this command does not invert the warpfield, and to warp a surface, you must use the inverse of the warpfield that warps the corresponding volume.\n\n" +
        "You must specify exactly one -from option, but you may specify multiple -to options, and -to-fnirt may be specified more than once."
    );
    return ret;
}

void OperationConvertWarpfield::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    WarpfieldFile myWarp;
    bool haveInput = false;
    OptionalParameter* fromWorld = myParams->getOptionalParameter(1);
    OptionalParameter* fromFnirt = myParams->getOptionalParameter(2);
    if (fromWorld->m_present)
    {
        haveInput = true;
    }
    if (fromFnirt->m_present)
    {
        if (haveInput) throw OperationException("only one -from option may be specified");
        haveInput = true;
    }
    if (!haveInput) throw OperationException("you must specify a -from option");
    if (fromWorld->m_present)
    {
        myWarp.readWorld(fromWorld->getString(1));
    }
    if (fromFnirt->m_present)
    {
        myWarp.readFnirt(fromFnirt->getString(1), fromFnirt->getString(2));
    }
    OptionalParameter* toWorld = myParams->getOptionalParameter(3);
    if (toWorld->m_present)
    {
        myWarp.writeWorld(toWorld->getString(1));
    }
    const vector<ParameterComponent*>& toFnirt = *(myParams->getRepeatableParameterInstances(4));//the return of this is a pointer so that it can return NULL if the key is wrong, after asserting
    int numFnirt = (int)toFnirt.size();//so, dereference immediately since it should be caught in debug via assert
    for (int i = 0; i < numFnirt; ++i)
    {
        myWarp.writeFnirt(toFnirt[i]->getString(1), toFnirt[i]->getString(2));
    }
}
