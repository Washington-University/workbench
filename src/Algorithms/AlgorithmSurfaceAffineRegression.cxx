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

#include "AlgorithmSurfaceAffineRegression.h"
#include "AlgorithmException.h"

#include "AffineFile.h"
#include "SurfaceFile.h"
//in order to do rref on double
#include "MatrixFunctions.h"

using namespace caret;
using namespace std;

AString AlgorithmSurfaceAffineRegression::getCommandSwitch()
{
    return "-surface-affine-regression";
}

AString AlgorithmSurfaceAffineRegression::getShortDescription()
{
    return "REGRESS THE AFFINE TRANSFORM BETWEEN SURFACES ON THE SAME MESH";
}

OperationParameters* AlgorithmSurfaceAffineRegression::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "source", "the surface to warp");
    
    ret->addSurfaceParameter(2, "target", "the surface to match the coordinates of");
    
    ret->addStringParameter(3, "affine-out", "output - the output affine file");
    
    ret->setHelpText(
        AString("Use linear regression to compute an affine that minimizes the sum of squares of the coordinate differences between the target surface and the warped source surface.  ") +
        "Note that this has a bias to shrink the surface that is being warped.  " +
        "The output is written as a NIFTI 'world' matrix, see -convert-affine to convert it for use in other software."
    );
    return ret;
}

void AlgorithmSurfaceAffineRegression::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* sourceSurf = myParams->getSurface(1);
    SurfaceFile* targetSurf = myParams->getSurface(2);
    AString affineOutName = myParams->getString(3);
    FloatMatrix affineMatOut;
    AlgorithmSurfaceAffineRegression(myProgObj, sourceSurf, targetSurf, affineMatOut);
    AffineFile affineOut;
    affineOut.setMatrix(affineMatOut);
    affineOut.writeWorld(affineOutName);
}

AlgorithmSurfaceAffineRegression::AlgorithmSurfaceAffineRegression(ProgressObject* myProgObj, const SurfaceFile* sourceSurf, const SurfaceFile* targetSurf, FloatMatrix& affineMatOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (!targetSurf->hasNodeCorrespondence(*sourceSurf)) throw AlgorithmException("input surfaces must have vertex correspondence");
    affineMatOut = FloatMatrix::identity(4);
    vector<vector<double> > indep(4, vector<double>(4, 0.0)), dep(4, vector<double>(3, 0.0));
    int numNodes = targetSurf->getNumberOfNodes();
    int coordSize = 3 * numNodes;
    const float* targetData = targetSurf->getCoordinateData();
    const float* sourceData = sourceSurf->getCoordinateData();
    for (int i = 0; i < coordSize; i += 3)
    {
        const float* targetCoord = targetData + i;
        const float* sourceCoord = sourceData + i;
        for (int j = 0; j < 3; ++j)//compute X' * X and X' * Y
        {
            for (int k = 0; k < 3; ++k)
            {
                indep[j][k] += sourceCoord[j] * sourceCoord[k];
                dep[k][j] += targetCoord[j] * sourceCoord[k];
            }
            indep[j][3] += sourceCoord[j];
            indep[3][j] += sourceCoord[j];
            dep[3][j] += targetCoord[j];
        }
        indep[3][3] += 1.0;
    }
    vector<vector<double> > rrefMat;
    MatrixFunctions::horizCat(indep, dep, rrefMat);
    MatrixFunctions::rref(rrefMat);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            affineMatOut[i][j] = rrefMat[j][4 + i];
        }
    }
}

float AlgorithmSurfaceAffineRegression::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfaceAffineRegression::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
