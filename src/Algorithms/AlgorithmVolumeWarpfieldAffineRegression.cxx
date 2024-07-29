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

#include "AlgorithmVolumeWarpfieldAffineRegression.h"
#include "AlgorithmException.h"

#include "AffineFile.h"
#include "MatrixFunctions.h"
#include "Vector3D.h"
#include "VolumeFile.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeWarpfieldAffineRegression::getCommandSwitch()
{
    return "-volume-warpfield-affine-regression";
}

AString AlgorithmVolumeWarpfieldAffineRegression::getShortDescription()
{
    return "REGRESS AFFINE FROM WARPFIELD";
}

OperationParameters* AlgorithmVolumeWarpfieldAffineRegression::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "warpfield", "the input warpfield");
    
    ret->addStringParameter(2, "affine-out", "output - the output affine file");//HACK: fake the output help formatting
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(3, "-roi", "only consider voxels within a mask (e.g., a brain mask)");
    roiOpt->addVolumeParameter(1, "roi-vol", "the mask volume");
    
    OptionalParameter* fnirtInOpt = ret->createOptionalParameter(4, "-fnirt", "input is a fnirt warpfield");
    fnirtInOpt->addStringParameter(1, "source-volume", "the source volume used when generating the fnirt warpfield");
    
    OptionalParameter* flirtOutOpt = ret->createOptionalParameter(5, "-flirt-out", "write output as a flirt matrix rather than a world coordinate transform");
    flirtOutOpt->addStringParameter(1, "source-volume", "the volume you want to apply the transform to");
    flirtOutOpt->addStringParameter(2, "target-volume", "the target space you want the transformed volume to match");
    
    ret->setHelpText(
        AString("For all voxels in the warpfield, do a regression that predicts the post-warp coordinate from the source coordinate.  ") +
        "When -roi is specified, only consider voxels with a value greater than 0 in <roi-vol>.\n\n" +
        "The default is to expect the warpfield to be in relative world coordinates (mm space), and to write the output as a world affine (mm space to mm space).  " +
        "If you are using FSL-created files and utilities, specify -fnirt and -flirt-out as needed, as their coordinate conventions are different."
    );
    return ret;
}

void AlgorithmVolumeWarpfieldAffineRegression::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString warpfieldName = myParams->getString(1);
    AString affineOutName = myParams->getString(2);
    VolumeFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(3);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getVolume(1);
    }
    WarpfieldFile myWarpfield;
    OptionalParameter* fnirtInOpt = myParams->getOptionalParameter(4);
    if (fnirtInOpt->m_present)
    {
        myWarpfield.readFnirt(warpfieldName, fnirtInOpt->getString(1));
    } else {
        myWarpfield.readWorld(warpfieldName);
    }
    FloatMatrix affineMatOut;
    AlgorithmVolumeWarpfieldAffineRegression(myProgObj, myWarpfield.getWarpfield(), affineMatOut, myRoi);
    AffineFile affineOut;
    affineOut.setMatrix(affineMatOut);
    OptionalParameter* flirtOutOpt = myParams->getOptionalParameter(5);
    if (flirtOutOpt->m_present)
    {
        affineOut.writeFlirt(affineOutName, flirtOutOpt->getString(1), flirtOutOpt->getString(2));
    } else {
        affineOut.writeWorld(affineOutName);
    }
}

AlgorithmVolumeWarpfieldAffineRegression::AlgorithmVolumeWarpfieldAffineRegression(ProgressObject* myProgObj, const VolumeFile* warpVol, FloatMatrix& affineMatOut, const VolumeFile* myRoi) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (myRoi != NULL && !warpVol->matchesVolumeSpace(myRoi))
    {
        throw AlgorithmException("roi volume does not match the volume space of the warpfield");
    }
    vector<int64_t> voldims = warpVol->getDimensions();
    if (voldims[3] != 3)
    {
        throw AlgorithmException("warpfield volume does not have 3 subvolumes");
    }
    //three regressions with the same predictors and different target values
    vector<vector<double> > indep(4, vector<double>(4, 0.0)), dep(4, vector<double>(3, 0.0));
    for (int64_t k = 0; k < voldims[2]; ++k)
    {
        for (int64_t j = 0; j < voldims[1]; ++j)
        {
            for (int64_t i = 0; i < voldims[0]; ++i)
            {
                if (myRoi == NULL || myRoi->getValue(i, j, k) > 0.0f)
                {
                    Vector3D outCoord, displacement, inCoord;//outCoord is the post-warp coordinate
                    warpVol->indexToSpace(i, j, k, outCoord);
                    displacement[0] = warpVol->getValue(i, j, k, 0);
                    displacement[1] = warpVol->getValue(i, j, k, 1);
                    displacement[2] = warpVol->getValue(i, j, k, 2);
                    inCoord = outCoord + displacement;
                    for (int dim1 = 0; dim1 < 3; ++dim1)//compute X' * X and X' * Y
                    {
                        for (int dim2 = 0; dim2 < 3; ++dim2)
                        {
                            indep[dim1][dim2] += inCoord[dim1] * inCoord[dim2];
                            dep[dim2][dim1] += outCoord[dim1] * inCoord[dim2];
                        }
                        indep[dim1][3] += inCoord[dim1];
                        indep[3][dim1] += inCoord[dim1];
                        dep[3][dim1] += outCoord[dim1];
                    }
                    indep[3][3] += 1.0;
                }
            }
        }
    }
    vector<vector<double> > rrefMat;
    MatrixFunctions::horizCat(indep, dep, rrefMat);
    MatrixFunctions::rref(rrefMat);
    affineMatOut = FloatMatrix::identity(4);
    for (int dim1 = 0; dim1 < 3; ++dim1)
    {
        for (int dim2 = 0; dim2 < 4; ++dim2)
        {
            affineMatOut[dim1][dim2] = rrefMat[dim2][4 + dim1];
        }
    }
}

float AlgorithmVolumeWarpfieldAffineRegression::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeWarpfieldAffineRegression::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
