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

#include "OperationSurfaceFlipNormals.h"
#include "OperationException.h"

#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString OperationSurfaceFlipNormals::getCommandSwitch()
{
    return "-surface-flip-normals";
}

AString OperationSurfaceFlipNormals::getShortDescription()
{
    return "FLIP ALL TILES ON A SURFACE";
}

OperationParameters* OperationSurfaceFlipNormals::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to flip the normals of");
    
    ret->addSurfaceOutputParameter(2, "surface-out", "the output surface");
    
    ret->setHelpText(
        AString("Flips all triangles on a surface, resulting in surface normals being flipped the other direction (inward vs outward).  ") +
        "If you transform a surface with an affine that has negative determinant, or a warpfield that similarly flips the surface, you may end up " +
        "with a surface that has normals pointing inwards, which may have display problems.  " +
        "Using this command will solve that problem."
    );
    return ret;
}

void OperationSurfaceFlipNormals::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    SurfaceFile* mySurf = myParams->getSurface(1);
    SurfaceFile* mySurfOut = myParams->getOutputSurface(2);
    *mySurfOut = *mySurf;
    mySurfOut->flipNormals();
}
