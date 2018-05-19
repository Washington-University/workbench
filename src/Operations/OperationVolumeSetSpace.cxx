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

#include "OperationVolumeSetSpace.h"
#include "OperationException.h"

#include "FloatMatrix.h"
#include "NiftiIO.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumeSetSpace::getCommandSwitch()
{
    return "-volume-set-space";
}

AString OperationVolumeSetSpace::getShortDescription()
{
    return "CHANGE VOLUME SPACE INFORMATION";
}

OperationParameters* OperationVolumeSetSpace::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume-in", "the input volume");
    
    ret->addStringParameter(2, "volume-out", "output - the output volume");//fake the "out" parameter formatting, because copying a volume file in memory is currently a problem
    
    OptionalParameter* plumbOpt = ret->createOptionalParameter(3, "-plumb", "set via axis order and spacing/offset");
    plumbOpt->addStringParameter(1, "axis-order", "a string like 'XYZ' that specifies which index is along which spatial dimension");
    plumbOpt->addDoubleParameter(2, "x-spacing", "change in x-coordinate from incrementing the relevant index");
    plumbOpt->addDoubleParameter(3, "y-spacing", "change in y-coordinate from incrementing the relevant index");
    plumbOpt->addDoubleParameter(4, "z-spacing", "change in z-coordinate from incrementing the relevant index");
    plumbOpt->addDoubleParameter(5, "x-offset", "the x-coordinate of the first voxel");
    plumbOpt->addDoubleParameter(6, "y-offset", "the y-coordinate of the first voxel");
    plumbOpt->addDoubleParameter(7, "z-offset", "the z-coordinate of the first voxel");
    
    OptionalParameter* sformOpt = ret->createOptionalParameter(4, "-sform", "set via a nifti sform");
    char axisNames[] = "xyz", indexNames[] = "ijk";
    for (int axis = 0; axis < 3; ++axis)
    {
        for (int index = 0; index < 3; ++index)
        {
            sformOpt->addDoubleParameter(axis * 4 + index, AString(axisNames[axis]) + indexNames[index] + "-spacing",
                                         "increase in " + AString(axisNames[axis]) + " coordinate from incrementing the " + indexNames[index] + " index");
        }
        sformOpt->addDoubleParameter(axis * 4 + 3, AString(axisNames[axis]) + "-offset", AString(axisNames[axis]) + " coordinate of first voxel");
    }
    
    OptionalParameter* fileOpt = ret->createOptionalParameter(5, "-file", "copy spacing info from volume file with matching dimensions");
    fileOpt->addStringParameter(1, "volume-ref", "volume file to use for reference space");
    fileOpt->createOptionalParameter(2, "-ignore-dims", "copy the spacing info even if the dimensions don't match");
    
    ret->setHelpText(
        AString("Writes a copy of the volume file, with the spacing information changed as specified.  ") +
        "No reordering of the voxel data occurs, see -volume-reorient to change the volume indexing order and reorder the voxels to match.  " +
        "Exactly one of -plumb, -sform, or -file must be specified."
    );
    return ret;
}

void OperationVolumeSetSpace::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* orig = myParams->getVolume(1);
    FloatMatrix newSform = FloatMatrix::zeros(4, 4);
    newSform[3][3] = 1.0f;//not needed, but just for sanity
    bool haveSpace = false;
    OptionalParameter* plumbOpt = myParams->getOptionalParameter(3);
    if (plumbOpt->m_present)
    {
        haveSpace = true;
        bool used[3] = {false, false, false};
        int revorder[3] = {-1, -1, -1};
        AString orient = plumbOpt->getString(1);
        if (orient.size() < 3) throw OperationException("<axis-order> must have 3 characters");
        for (int i = 0; i < 3; ++i)
        {
            int dir = -1;
            switch (orient[i].toLatin1())
            {
                case 'X':
                case 'x':
                    dir = 0;
                    break;
                case 'Y':
                case 'y':
                    dir = 1;
                    break;
                case 'Z':
                case 'z':
                    dir = 2;
                    break;
                default:
                    throw OperationException("<axis-order> must use the characters X, Y, and Z");
            }
            if (used[dir]) throw OperationException("<axis-order> may not repeat an axis");
            used[dir] = true;
            revorder[dir] = i;//construct the reversed order, because thats what we need
        }
        newSform[0][revorder[0]] = (float)plumbOpt->getDouble(2);
        newSform[0][3] = (float)plumbOpt->getDouble(5);
        newSform[1][revorder[1]] = (float)plumbOpt->getDouble(3);
        newSform[1][3] = (float)plumbOpt->getDouble(6);
        newSform[2][revorder[2]] = (float)plumbOpt->getDouble(4);
        newSform[2][3] = (float)plumbOpt->getDouble(7);
    }
    OptionalParameter* sformOpt = myParams->getOptionalParameter(4);
    if (sformOpt->m_present)
    {
        if (haveSpace) throw OperationException("only one of -plumb, -sform, or -file may be specified");
        haveSpace = true;
        for (int axis = 0; axis < 3; ++axis)
        {
            for (int index = 0; index < 3; ++index)
            {
                newSform[axis][index] = (float)sformOpt->getDouble(axis * 4 + index);
            }
            newSform[axis][3] = (float)sformOpt->getDouble(axis * 4 + 3);
        }
    }
    OptionalParameter* fileOpt = myParams->getOptionalParameter(5);
    if (fileOpt->m_present)
    {
        if (haveSpace) throw OperationException("only one of -plumb, -sform, or -file may be specified");
        haveSpace = true;
        NiftiIO myIO;
        myIO.openRead(fileOpt->getString(1));
        newSform = myIO.getHeader().getSForm();
        if (!fileOpt->getOptionalParameter(2)->m_present)
        {
            //check dimensions and throw
            vector<int64_t> origDims = orig->getDimensions(), templateDims = myIO.getDimensions();
            origDims.resize(3, 1); templateDims.resize(3, 1);
            if (origDims != templateDims) throw OperationException("template has different dimensions than input file, use -ignore-dims to copy the volume space info anyway");
        }
    }
    if (!haveSpace) throw OperationException("you must specify -plumb, -sform, or -file");
    orig->setVolumeSpace(newSform.getMatrix());
    orig->writeFile(myParams->getString(2));
}
