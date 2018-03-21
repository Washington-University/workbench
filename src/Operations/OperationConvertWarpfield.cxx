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
    
    OptionalParameter* fromITK = ret->createOptionalParameter(5, "-from-itk", "input is an ITK warpfield");
    fromITK->addStringParameter(1, "input", "the input warpfield");
    
    OptionalParameter* fromFnirt = ret->createOptionalParameter(2, "-from-fnirt", "input is a fnirt warpfield");
    fromFnirt->addStringParameter(1, "input", "the input warpfield");
    fromFnirt->addStringParameter(2, "source-volume", "the source volume used when generating the input warpfield");
    fromFnirt->createOptionalParameter(3, "-absolute", "warpfield was written in absolute convention, rather than relative");
    
    OptionalParameter* toWorld = ret->createOptionalParameter(3, "-to-world", "write output as a NIFTI 'world' warpfield");
    toWorld->addStringParameter(1, "output", "output - the output warpfield");//HACK: fake the output formatting, since the warpfield uses separate calls for writing each output type, and by design doesn't give access to the raw with-quirks volume file
    
    OptionalParameter* toITK = ret->createOptionalParameter(6, "-to-itk", "write output as an ITK warpfield");
    toITK->addStringParameter(1, "output", "output - the output warpfield");
    
    ParameterComponent* toFnirt = ret->createRepeatableParameter(4, "-to-fnirt", "write output as a fnirt warpfield");
    toFnirt->addStringParameter(1, "output", "output - the output warpfield");//this argument order is different than other commands, yes - but don't change it, so scripts will still work
    toFnirt->addStringParameter(2, "source-volume", "the volume you want to apply the warpfield to");
    
    ret->setHelpText(
        AString("NIFTI world warpfields can be used directly on mm coordinates via sampling the three subvolumes at the coordinate ") +
        "and adding the sampled values to the coordinate vector.  " +
        "They use the NIFTI coordinate system, that is, X is left to right, Y is posterior to anterior, and Z is inferior to superior.\n\n" +
        "NOTE: this command does not invert the warpfield, and to warp a surface, you must use the inverse of the warpfield that warps the corresponding volume.\n\n" +
        "The ITK format is used by ANTS.\n\n" +
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
    OptionalParameter* fromITK = myParams->getOptionalParameter(5);
    OptionalParameter* fromFnirt = myParams->getOptionalParameter(2);
    if (fromWorld->m_present)
    {
        haveInput = true;
    }
    if (fromITK->m_present)
    {
        if (haveInput) throw OperationException("only one -from option may be specified");
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
    if (fromITK->m_present)
    {
        myWarp.readITK(fromITK->getString(1));
    }
    if (fromFnirt->m_present)
    {
        myWarp.readFnirt(fromFnirt->getString(1), fromFnirt->getString(2), fromFnirt->getOptionalParameter(3)->m_present);
    }
    OptionalParameter* toWorld = myParams->getOptionalParameter(3);
    OptionalParameter* toITK = myParams->getOptionalParameter(6);
    if (toWorld->m_present)
    {
        myWarp.writeWorld(toWorld->getString(1));
    }
    if (toITK->m_present)
    {
        myWarp.writeITK(toITK->getString(1));
    }
    const vector<ParameterComponent*>& toFnirt = *(myParams->getRepeatableParameterInstances(4));//the return of this is a pointer so that it can return NULL if the key is wrong, after asserting
    int numFnirt = (int)toFnirt.size();//so, dereference immediately since it should be caught in debug via assert
    for (int i = 0; i < numFnirt; ++i)
    {
        myWarp.writeFnirt(toFnirt[i]->getString(1), toFnirt[i]->getString(2));
    }
}
