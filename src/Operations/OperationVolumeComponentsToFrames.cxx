/*LICENSE_START*/
/*
 *  Copyright (C) 2023  Washington University School of Medicine
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

#include "OperationVolumeComponentsToFrames.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString OperationVolumeComponentsToFrames::getCommandSwitch()
{
    return "-volume-components-to-frames";
}

AString OperationVolumeComponentsToFrames::getShortDescription()
{
    return "CONVERT RGB/COMPLEX VOLUME TO FRAMES";
}

OperationParameters* OperationVolumeComponentsToFrames::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "input", "the RGB/complex-type volume");
    
    ret->addVolumeOutputParameter(2, "output", "the input volume converted to multiple frames of scalar type");
    
    ret->setHelpText(
        AString("RGB and complex datatypes are not always well supported, this command allows separating them into standard subvolumes for better support.")
    );
    return ret;
}

void OperationVolumeComponentsToFrames::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const VolumeFile* invol = myParams->getVolume(1);
    VolumeFile* outvol = myParams->getOutputVolume(2);
    vector<int64_t> indims = invol->getDimensions();
    if (indims[4] == 1) CaretLogWarning("file '" + invol->getFileName() + "' is already single-component");
    outvol->reinitialize(invol, indims[3] * indims[4]);
    for (int64_t s = 0; s < indims[3]; ++s)
    {
        for (int64_t c = 0; c < indims[4]; ++c)
        {
            const int64_t outmap = c + s * indims[4];
            outvol->setFrame(invol->getFrame(s, c), outmap, 0);
            outvol->setMapName(outmap, invol->getMapName(s));
        }
    }
}
