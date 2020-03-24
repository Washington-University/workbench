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

#include "AffineFile.h"
#include "OperationConvertAffine.h"
#include "OperationException.h"

using namespace caret;
using namespace std;

AString OperationConvertAffine::getCommandSwitch()
{
    return "-convert-affine";
}

AString OperationConvertAffine::getShortDescription()
{
    return "CONVERT AN AFFINE FILE BETWEEN CONVENTIONS";
}

OperationParameters* OperationConvertAffine::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    OptionalParameter* fromWorld = ret->createOptionalParameter(1, "-from-world", "input is a NIFTI 'world' affine");
    fromWorld->addStringParameter(1, "input", "the input affine");
    fromWorld->createOptionalParameter(2, "-inverse", "for files that use 'target to source' convention");
    
    OptionalParameter* fromITK = ret->createOptionalParameter(5, "-from-itk", "input is an ITK matrix");
    fromITK->addStringParameter(1, "input", "the input affine");
    
    OptionalParameter* fromFlirt = ret->createOptionalParameter(2, "-from-flirt", "input is a flirt matrix");
    fromFlirt->addStringParameter(1, "input", "the input affine");
    fromFlirt->addStringParameter(2, "source-volume", "the source volume used when generating the input affine");
    fromFlirt->addStringParameter(3, "target-volume", "the target volume used when generating the input affine");
    
    OptionalParameter* toWorld = ret->createOptionalParameter(3, "-to-world", "write output as a NIFTI 'world' affine");
    toWorld->addStringParameter(1, "output", "output - the output affine");//HACK: fake the output formatting, since we don't have a parameter for affine file (hard to do due to multiple on-disk formats)
    toWorld->createOptionalParameter(2, "-inverse", "write file using 'target to source' convention");
    
    OptionalParameter* toITK = ret->createOptionalParameter(6, "-to-itk", "write output as an ITK affine");
    toITK->addStringParameter(1, "output", "output - the output affine");
    
    ParameterComponent* toFlirt = ret->createRepeatableParameter(4, "-to-flirt", "write output as a flirt matrix");
    toFlirt->addStringParameter(1, "output", "output - the output affine");
    toFlirt->addStringParameter(2, "source-volume", "the volume you want to apply the transform to");
    toFlirt->addStringParameter(3, "target-volume", "the target space you want the transformed volume to match");
    
    ret->setHelpText(
        AString("NIFTI world matrices can be used directly on mm coordinates via matrix multiplication, they use the NIFTI coordinate system, that is, ") +
        "positive X is right, positive Y is anterior, and positive Z is superior.  " +
        "Note that wb_command assumes that world matrices transform source coordinates to target coordinates, " +
        "while other tools may use affines that transform target coordinates to source coordinates.\n\n" +
        "The ITK format is used by ANTS.\n\n" +
        "You must specify exactly one -from option, but you may specify multiple -to options, and -to-flirt may be specified more than once."
    );
    return ret;
}

void OperationConvertAffine::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AffineFile myAffine;
    bool haveInput = false;
    OptionalParameter* fromWorld = myParams->getOptionalParameter(1);
    if (fromWorld->m_present)
    {
        haveInput = true;
        myAffine.readWorld(fromWorld->getString(1), fromWorld->getOptionalParameter(2)->m_present);
    }
    OptionalParameter* fromFlirt = myParams->getOptionalParameter(2);
    if (fromFlirt->m_present)
    {
        if (haveInput) throw OperationException("only one -from option may be specified");
        haveInput = true;
        myAffine.readFlirt(fromFlirt->getString(1), fromFlirt->getString(2), fromFlirt->getString(3));
    }
    OptionalParameter* fromITK = myParams->getOptionalParameter(5);
    if (fromITK->m_present)
    {
        if (haveInput) throw OperationException("only one -from option may be specified");
        haveInput = true;
        myAffine.readITK(fromITK->getString(1));
    }
    if (!haveInput) throw OperationException("you must specify a -from option");
    OptionalParameter* toWorld = myParams->getOptionalParameter(3);
    if (toWorld->m_present)
    {
        myAffine.writeWorld(toWorld->getString(1), toWorld->getOptionalParameter(2)->m_present);
    }
    const vector<ParameterComponent*>& toFlirt = myParams->getRepeatableParameterInstances(4);//the return of this is a pointer so that it can return NULL if the key is wrong, after asserting
    int numFlirt = (int)toFlirt.size();//so, dereference immediately since it should be caught in debug via assert
    for (int i = 0; i < numFlirt; ++i)
    {
        myAffine.writeFlirt(toFlirt[i]->getString(1), toFlirt[i]->getString(2), toFlirt[i]->getString(3));
    }
    OptionalParameter* toITK = myParams->getOptionalParameter(6);
    if (toITK->m_present)
    {
        myAffine.writeITK(toITK->getString(1));
    }
}
