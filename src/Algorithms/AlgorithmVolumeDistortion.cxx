/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "AlgorithmVolumeDistortion.h"
#include "AlgorithmException.h"

#include "AlgorithmVolumeGradient.h"
#include "CaretOMP.h"
#include "FloatMatrix.h"
#include "VolumeFile.h"
#include "WarpfieldFile.h"

#include <algorithm>
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmVolumeDistortion::getCommandSwitch()
{
    return "-volume-distortion";
}

AString AlgorithmVolumeDistortion::getShortDescription()
{
    return "CALCULATE VOLUME WARPFIELD DISTORTION";
}

OperationParameters* AlgorithmVolumeDistortion::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "warpfield", "the warpfield to compute the distortion of");
    
    ret->addVolumeOutputParameter(2, "volume-out", "the output distortion measures");
    
    OptionalParameter* fnirtOpt = ret->createOptionalParameter(3, "-fnirt", "MUST be used if using a fnirt warpfield");
    fnirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the warpfield");
    
    ret->createOptionalParameter(4, "-circular", "use the circle-based formula for the anisotropic measure");
    
    ret->createOptionalParameter(5, "-log2", "apply base-2 log transform");
    
    ret->setHelpText(
        AString("Calculates isotropic and anisotropic distortions in the volume warpfield.  ") +
        "At each voxel, the gradient of the absolute warpfield is computed to obtain the local affine transforms for each voxel (jacobian matrices), and strain tensors are derived from them.  " +
        "The isotropic component (volumetric expansion ratio) is the product of the three principal strains.  " +
        "The default measure ('elongation') for the anisotropic component is the largest principal strain divided by the smallest.\n\n" +
        "The -circular option instead calculates the anisotropic component by transforming the principal strains into log space, " +
        "considering them as x-values of points on a circle 120 degrees apart, finds the circle's diameter, and transforms that back to a ratio."
    );
    return ret;
}

void AlgorithmVolumeDistortion::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString warpName = myParams->getString(1);
    VolumeFile* distortionOut = myParams->getOutputVolume(2);
    OptionalParameter* fnirtOpt = myParams->getOptionalParameter(3);
    Method myMethod = ELONGATION;
    if (myParams->getOptionalParameter(4)->m_present) myMethod = CIRCULAR;
    bool dolog2 = myParams->getOptionalParameter(5)->m_present;
    WarpfieldFile myWarp;
    if (fnirtOpt->m_present)
    {
        myWarp.readFnirt(warpName, fnirtOpt->getString(1));
    } else {
        myWarp.readWorld(warpName);
    }
    AlgorithmVolumeDistortion(myProgObj, myWarp, distortionOut, myMethod, dolog2);
}

AlgorithmVolumeDistortion::AlgorithmVolumeDistortion(ProgressObject* myProgObj, const WarpfieldFile& myWarp, VolumeFile* distortionOut, const Method myMethod, const bool dolog2) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    switch (myMethod)
    {
        case ELONGATION:
        case CIRCULAR:
            break;
        default:
            throw AlgorithmException("invalid method value passed to AlgorithmVolumeDistortion");
    }
    CaretPointer<VolumeFile> absWarp = myWarp.generateAbsolute();
    VolumeFile gradMag, gradVec;//don't need gradMag, but it is a mandatory argument to algorithm
    AlgorithmVolumeGradient(NULL, absWarp, &gradMag, -1, NULL, &gradVec);//vector order is dx'/dx, dx'/dy, dx'/dz, dy'/dx, etc - these are the local affines, we don't need the translation
    vector<int64_t> dims = gradVec.getDimensions();
    CaretAssert(dims[3] == 9);//make sure it has the expected components for a 3D tranform
    dims.resize(4);//drop components so we can use it to initialize output
    dims[3] = 2;//set it to 2 maps
    distortionOut->reinitialize(dims, absWarp->getSform());
    distortionOut->setMapName(0, "isotropic (expansion)");
    switch (myMethod)
    {
        case ELONGATION:
            distortionOut->setMapName(1, "anisotropic (elongation)");
            break;
        case CIRCULAR:
            distortionOut->setMapName(1, "anisotropic (circular)");
            break;
    }
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t k = 0; k < dims[2]; ++k)
    {
        for (int64_t j = 0; j < dims[1]; ++j)
        {
            for (int64_t i = 0; i < dims[0]; ++i)
            {
                FloatMatrix localAff(3, 3);
                for (int dim1 = 0; dim1 < 3; ++dim1)
                {
                    for (int dim2 = 0; dim2 < 3; ++dim2)
                    {
                        localAff[dim1][dim2] = gradVec.getValue(i, j, k, dim1 * 3 + dim2);
                    }
                }
                FloatMatrix squaredtensor = localAff * localAff.transpose();//this cancels the inner rotations, aligning the principal strains resulting in squaring them, allowing us to use tensor simplifications
                float isotropic = sqrt(squaredtensor.determinant());//square root of i3 for squared tensor
                float i1 = (squaredtensor[0][0] + squaredtensor[1][1] + squaredtensor[2][2]);//sum of squares of principal strains
                float i2 = squaredtensor[0][0] * squaredtensor[1][1] + squaredtensor[0][0] * squaredtensor[2][2] + squaredtensor[1][1] * squaredtensor[2][2] -
                            squaredtensor[0][1] * squaredtensor[0][1] - squaredtensor[0][2] * squaredtensor[0][2] - squaredtensor[1][2] * squaredtensor[1][2];
                float q = (3 * i2 - i1 * i1) / 9.0f;//now we need to solve the cubic for the three principal strains, so we can make whatever formula we want
                if (q > 0) 
                {
                    q = 0.0f;//just in case, avoid pow/sqrt giving NaN for tiny positive q, we should always have 3 solutions
                }
                float r = (i1 * (2 * i1 * i1 - 9 * i2) + 27 * isotropic * isotropic) / 54.0f;//remember, the tensor is squared
                const float PI = 3.141592654f;//trivially avoid the _USE_MATH_DEFINES that isn't actually required by the standard - float has only ~7 decimal digits of precision
                float triratio = r / pow(-q, 3.0f / 2.0f);//actually part of the cubic root equation, but we need to sanity check it
                if (triratio > 1.0f) triratio = 1.0f;//because we don't want acos to give a nan
                if (triratio < -1.0f) triratio = -1.0f;
                float theta = acos(triratio);
                float shapes[3];//these will be ordinary, not squared, strains
                float isoroot3 = pow(isotropic, 1.0f / 3.0f);
                for (int a = 0; a < 3; ++a)
                {
                    shapes[a] = sqrt(2 * sqrt(-q) * cos((theta + a * 2 * PI) / 3.0f) + i1 / 3.0f) / isoroot3;//square root because squared tensor, and also divide out isotropic component
                }
                sort(shapes, shapes + 3);
                float anisotropic = -1.0f;//initialize with obviously-wrong value to silence compiler and make bugs obvious
                switch (myMethod)
                {
                    case ELONGATION:
                        anisotropic = shapes[2] / shapes[0];//deceptively simple: largest strain divided by smallest strain
                        break;
                    case CIRCULAR:
                    {
                        //new solution found by coincidence: square and sum the x coordinates of the vertices an equilateral triangle centered on the origin, you get (3r^2)/2, where r is the distance from the origin to a vertex
                        float logshapes[3];
                        for (int a = 0; a < 3; ++a)
                        {
                            logshapes[a] = log(shapes[a]);
                        }
                        anisotropic = exp(sqrt(8.0f / 3.0f * (logshapes[0] * logshapes[0] + logshapes[1] * logshapes[1] + logshapes[2] * logshapes[2])));
                        break;//explanation time: per the help info, put the shape ratios into log space, so that geometry can be useful
                        //now consider an equilateral triangle centered at the origin, and these log-space shape values as the x coordinates of the vertices
                        //now, find the distance from the origin to any vertex, and multiply by 2 to get the circumscribing circle's diameter
                        //the old solution relied on the the most-horizontal edge of the triangle being split by the y-axis at the same ratio as the magnitudes of the two extreme log-shape values (don't forget the negative sign)
                    }
                }
                if (dolog2)
                {
                    isotropic = log2(isotropic);
                    anisotropic = log2(anisotropic);
                }
                distortionOut->setValue(isotropic, i, j, k, 0);
                distortionOut->setValue(anisotropic, i, j, k, 1);
            }
        }
    }
}

float AlgorithmVolumeDistortion::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeDistortion::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
