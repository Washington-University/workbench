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

#include "OperationVolumeReorient.h"
#include "OperationException.h"

#include "CaretCommandGlobalOptions.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumeReorient::getCommandSwitch()
{
    return "-volume-reorient";
}

AString OperationVolumeReorient::getShortDescription()
{
    return "CHANGE VOXEL ORDER OF A VOLUME FILE";
}

OperationParameters* OperationVolumeReorient::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "volume", "the volume to reorient");
    ret->addStringParameter(2, "orient-string", "the desired orientation");
    ret->addStringParameter(3, "volume-out", "out - the reoriented volume");//fake the "out" parameter formatting, because copying a volume file in memory is currently a problem
    ret->setHelpText(
        AString("Changes the voxel order and the header spacing/origin information such that the value of any spatial point is unchanged.  ") +
        "Orientation strings look like 'LPI', which means first index is left to right, second is posterior to anterior, and third is inferior to superior.  " +
        "The valid characters are:\n\nL      left to right\nR      right to left\nP      posterior to anterior\nA      anterior to posterior\nI      inferior to superior\nS      superior to inferior"
    );
    return ret;
}

void OperationVolumeReorient::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    VolumeFile* myVol = myParams->getVolume(1);
    AString orientString = myParams->getString(2);
    AString outName = myParams->getString(3);
    if (orientString.length() < 3)
    {
        throw OperationException("orient-string must have 3 characters");
    }
    bool used[3] = {false, false, false};
    VolumeSpace::OrientTypes orient[3];
    for (int i = 0; i < 3; ++i)
    {
        char id = orientString[i].toLatin1();
        switch (id)
        {
            case 'L':
            case 'l':
                if (used[0]) throw OperationException("X axis (L, R) specified more than once");
                used[0] = true;
                orient[i] = VolumeSpace::LEFT_TO_RIGHT;
                break;
            case 'R':
            case 'r':
                if (used[0]) throw OperationException("X axis (L, R) specified more than once");
                used[0] = true;
                orient[i] = VolumeSpace::RIGHT_TO_LEFT;
                break;
            case 'P':
            case 'p':
                if (used[1]) throw OperationException("Y axis (P, A) specified more than once");
                used[1] = true;
                orient[i] = VolumeSpace::POSTERIOR_TO_ANTERIOR;
                break;
            case 'A':
            case 'a':
                if (used[1]) throw OperationException("Y axis (P, A) specified more than once");
                used[1] = true;
                orient[i] = VolumeSpace::ANTERIOR_TO_POSTERIOR;
                break;
            case 'I':
            case 'i':
                if (used[2]) throw OperationException("Z axis (I, S) specified more than once");
                used[2] = true;
                orient[i] = VolumeSpace::INFERIOR_TO_SUPERIOR;
                break;
            case 'S':
            case 's':
                if (used[2]) throw OperationException("Z axis (I, S) specified more than once");
                used[2] = true;
                orient[i] = VolumeSpace::SUPERIOR_TO_INFERIOR;
                break;
            default:
                throw OperationException(AString("unrecognized character '") + id + "'");
        }
    }
    myVol->reorient(orient);
    if (caret_global_command_options.m_volumeScale) //we aren't currently using an output volume parameter, but we should probably obey the datatype options...
    {
        myVol->setWritingDataTypeAndScaling(caret_global_command_options.m_volumeDType, caret_global_command_options.m_volumeMin, caret_global_command_options.m_volumeMax);
    } else {
        myVol->setWritingDataTypeNoScaling(caret_global_command_options.m_volumeDType);
    }
    myVol->writeFile(outName);
}
