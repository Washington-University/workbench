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

#include "AlgorithmSurfaceFlipLR.h"
#include "AlgorithmException.h"

#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceFlipLR::getCommandSwitch()
{
    return "-surface-flip-lr";
}

AString AlgorithmSurfaceFlipLR::getShortDescription()
{
    return "MIRROR A SURFACE THROUGH THE YZ PLANE";
}

OperationParameters* AlgorithmSurfaceFlipLR::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to flip");
    
    ret->addSurfaceOutputParameter(2, "surface-out", "the output flipped surface");
    
    ret->setHelpText(
        AString("This command negates the x coordinate of each vertex, and flips the surface normals, so that you have a surface of ") +
        "opposite handedness with the same features and vertex correspondence, with normals consistent with the original surface.  " +
        "That is, if the input surface has normals facing outward, the output surface will also have normals facing outward."
    );
    return ret;
}

void AlgorithmSurfaceFlipLR::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    SurfaceFile* mySurfOut = myParams->getOutputSurface(2);
    AlgorithmSurfaceFlipLR(myProgObj, mySurf, mySurfOut);
}

AlgorithmSurfaceFlipLR::AlgorithmSurfaceFlipLR(ProgressObject* myProgObj, const SurfaceFile* mySurf, SurfaceFile* mySurfOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    mySurfOut->setNumberOfNodesAndTriangles(mySurf->getNumberOfNodes(), mySurf->getNumberOfTriangles());
    mySurfOut->setSurfaceType(mySurf->getSurfaceType());
    mySurfOut->setSecondaryType(mySurf->getSecondaryType());
    int numNodes = mySurf->getNumberOfNodes();
    float tempcoord[3];
    for (int i = 0; i < numNodes; ++i)
    {
        mySurf->getCoordinate(i, tempcoord);
        tempcoord[0] = -tempcoord[0];
        mySurfOut->setCoordinate(i, tempcoord);
    }
    int numTiles = mySurf->getNumberOfTriangles();
    for (int i = 0; i < numTiles; ++i)
    {
        const int32_t* temptile = mySurf->getTriangle(i);
        mySurfOut->setTriangle(i, temptile[1], temptile[0], temptile[2]);
    }
}

float AlgorithmSurfaceFlipLR::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceFlipLR::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
