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

#include "OperationSurfaceCutResample.h"
#include "OperationException.h"

#include "SurfaceResamplingHelper.h"

using namespace caret;
using namespace std;

AString OperationSurfaceCutResample::getCommandSwitch()
{
    return "-surface-cut-resample";
}

AString OperationSurfaceCutResample::getShortDescription()
{
    return "RESAMPLE A CUT SURFACE";
}

OperationParameters* OperationSurfaceCutResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface-in", "the surface file to resample, with cuts");
    
    ret->addSurfaceParameter(2, "current-sphere", "a sphere surface with the mesh that the input surface is currently on (no cuts)");
    
    ret->addSurfaceParameter(3, "new-sphere", "a sphere surface that is in register with <current-sphere> and has the desired output mesh (no cuts)");
    
    ret->addSurfaceOutputParameter(4, "surface-out", "the output surface file");
    
    ret->setHelpText(
        AString("Resamples a cut surface file, given two spherical surfaces that are in register.  ") +
        "Barycentric resampling is used, because it is usually better for resampling surfaces, and because it is needed to figure out the new topology anyway."
    );
    return ret;
}

void OperationSurfaceCutResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* surfaceIn = myParams->getSurface(1);
    SurfaceFile* curSphere = myParams->getSurface(2);
    SurfaceFile* newSphere = myParams->getSurface(3);
    SurfaceFile* surfaceOut = myParams->getOutputSurface(4);
    SurfaceResamplingHelper::resampleCutSurface(surfaceIn, curSphere, newSphere, surfaceOut);
}
