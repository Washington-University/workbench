/*LICENSE_START*/
/*
 *  Copyright (C) 2020  Washington University School of Medicine
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

#include "AlgorithmVolumeResample.h"
#include "AlgorithmException.h"

#include "AffineFile.h"
#include "AffineSeriesFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "NiftiIO.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString AlgorithmVolumeResample::getCommandSwitch()
{
    return "-volume-resample";
}

AString AlgorithmVolumeResample::getShortDescription()
{
    return "TRANSFORM AND RESAMPLE A VOLUME FILE";
}

OperationParameters* AlgorithmVolumeResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "volume to resample");

    ret->addStringParameter(2, "volume-space", "a volume file in the volume space you want for the output");

    ret->addStringParameter(3, "method", "the resampling method");

    ret->addVolumeOutputParameter(4, "volume-out", "the output volume");

    OptionalParameter* backgroundOpt = ret->createOptionalParameter(8, "-background", "use a specified value for locations outside the FoV of the input image or warpfield(s)");
    backgroundOpt->addDoubleParameter(1, "value", "the value to use (default 0)");

    ParameterComponent* affineOpt = ret->createRepeatableParameter(5, "-affine", "add an affine transform");
    affineOpt->addStringParameter(1, "affine", "the affine file to use");
    OptionalParameter* flirtOpt = affineOpt->createOptionalParameter(2, "-flirt", "MUST be used if affine is a flirt affine");
    flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");

    ParameterComponent* affineSeriesOpt = ret->createRepeatableParameter(6, "-affine-series", "add an independent affine per-frame");
    affineSeriesOpt->addStringParameter(1, "affine-series", "text file containing 12 or 16 numbers per line, each being a row-major flattened affine");
    OptionalParameter* seriesFlirtOpt = affineSeriesOpt->createOptionalParameter(2, "-flirt", "MUST be used if the affines are flirt affines");
    seriesFlirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    seriesFlirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");

    ParameterComponent* warpOpt = ret->createRepeatableParameter(7, "-warp", "add a nonlinear warpfield transform");
    warpOpt->addStringParameter(1, "warpfield", "the warpfield file");
    OptionalParameter* fnirtOpt = warpOpt->createOptionalParameter(2, "-fnirt", "MUST be used if using a fnirt warpfield");
    fnirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the warpfield");

    ret->setHelpText(
        AString("Resample a volume file with an arbitrary list of transformations.  ") +
        "You may specify -affine, -warp, and -affine-series multiple times each, and they will be used in the order specified.  "
        "For instance, for rigid motion correction followed by nonlinear atlas registration, specify -affine-series first, then -warp.  "
        "The recommended methods are CUBIC (cubic spline) for most data, and the -volume-label-resample command for label data (the ENCLOSING_VOXEL method in this command will give a rough version).  "
        "The parameter <method> must be one of:\n\n"
        "CUBIC\nENCLOSING_VOXEL\nTRILINEAR"
    );
    return ret;
}

void AlgorithmVolumeResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* inVol = myParams->getVolume(1);
    vector<int64_t> voldims = inVol->getDimensions();
    AString refSpaceName = myParams->getString(2);
    AString methodStr = myParams->getString(3);
    VolumeFile* outVol = myParams->getOutputVolume(4);
    auto& affInstances = myParams->getRepeatableParameterInstances(5);
    auto& affSeriesInstances = myParams->getRepeatableParameterInstances(6);
    auto& warpInstances = myParams->getRepeatableParameterInstances(7);
    float backgroundVal = 0.0f;//the help info says zero, don't use the VolumeFile constant
    OptionalParameter* backgroundOpt = myParams->getOptionalParameter(8);
    if (backgroundOpt->m_present)
    {
        backgroundVal = backgroundOpt->getDouble(1);
    }
    VolumeSpace refSpace;
    {
        NiftiIO myIO;
        myIO.openRead(refSpaceName);
        refSpace = myIO.getHeader().getVolumeSpace();
    }
    VolumeFile::InterpType myMethod = VolumeFile::CUBIC;
    if (methodStr == "CUBIC")
    {
        myMethod = VolumeFile::CUBIC;
    } else if (methodStr == "TRILINEAR") {
        myMethod = VolumeFile::TRILINEAR;
    } else if (methodStr == "ENCLOSING_VOXEL") {
        myMethod = VolumeFile::ENCLOSING_VOXEL;
    } else {
        throw AlgorithmException("unrecognized interpolation method '" + methodStr + "'");
    }
    XfmStack myStack;
    auto xfmOrder = myParams->getRepeatableOrder();//helper for some ugly code to resolve relative order of repeatable options
    vector<WarpfieldFile> warpStorage(warpInstances.size());//need to keep these in scope until after the algorithm completes
    for (auto iter = xfmOrder.rbegin(); iter != xfmOrder.rend(); ++iter)//because this is volume resampling, we need to transform target coords into source coords
    {//so, reverse the transform order and invert affines (warpfields are harder to invert, so they work differently for surfaces)
        switch (iter->key)
        {
            case 5:
            {
                OptionalParameter* flirtOpt = affInstances[iter->index]->getOptionalParameter(2);
                AffineFile myAff;
                if (flirtOpt->m_present)
                {
                    myAff.readFlirt(affInstances[iter->index]->getString(1), flirtOpt->getString(1), flirtOpt->getString(2));
                } else {
                    myAff.readWorld(affInstances[iter->index]->getString(1));
                }
                myStack.push_back(CaretPointer<XfmBase>(new AffineXfm(myAff.getMatrix().inverse())));//invert it
                break;
            }
            case 6:
            {
                OptionalParameter* flirtOpt = affSeriesInstances[iter->index]->getOptionalParameter(2);
                AffineSeriesFile myAffSeries;
                if (flirtOpt->m_present)
                {
                    myAffSeries.readFlirt(affSeriesInstances[iter->index]->getString(1), flirtOpt->getString(1), flirtOpt->getString(2));
                } else {
                    myAffSeries.readWorld(affSeriesInstances[iter->index]->getString(1));
                }
                if (myAffSeries.getMatrixList().size() != size_t(voldims[3]))
                {
                    throw AlgorithmException("affine series file '" + affSeriesInstances[iter->index]->getString(1) + "' has different number of frames than the input volume");
                }
                myStack.push_back(CaretPointer<XfmBase>(new AffineSeriesXfm(myAffSeries.getInverseMatrixList())));//invert it
                break;
            }
            case 7:
            {
                OptionalParameter* fnirtOpt = warpInstances[iter->index]->getOptionalParameter(2);
                WarpfieldFile& myWarp = warpStorage[iter->index];
                if (fnirtOpt->m_present)
                {
                    myWarp.readFnirt(warpInstances[iter->index]->getString(1), fnirtOpt->getString(1));
                } else {
                    myWarp.readWorld(warpInstances[iter->index]->getString(1));
                }
                myStack.push_back(CaretPointer<XfmBase>(new WarpfieldXfm(myWarp.getWarpfield())));//DON'T invert, internal warpfield convention is already inverse
                break;
            }
            default:
                CaretAssert(false);
                throw AlgorithmException("internal error, tell the developers what you just tried to do");
        }
    }
    AlgorithmVolumeResample(myProgObj, inVol, myStack, refSpace, myMethod, outVol, backgroundVal);
}

AlgorithmVolumeResample::AlgorithmVolumeResample(ProgressObject* myProgObj, const VolumeFile* inVol, const XfmStack& myStack, const VolumeSpace refSpace,
                                                 const VolumeFile::InterpType& myMethod, VolumeFile* outVol, const float backgroundVal) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<int64_t> outDims = inVol->getOriginalDimensions();
    const int64_t* refDims = refSpace.getDims();
    if (outDims.size() < 3) throw AlgorithmException("input must have 3 spatial dimensions");
    outDims[0] = refDims[0];
    outDims[1] = refDims[1];
    outDims[2] = refDims[2];
    int64_t numMaps = inVol->getNumberOfMaps(), numComponents = inVol->getNumberOfComponents();
    outVol->reinitialize(outDims, refSpace.getSform(), numComponents, inVol->getType(), inVol->m_header);
    vector<float> scratchFrame(outDims[0] * outDims[1] * outDims[2], 0.0f);
    if (inVol->isMappedWithLabelTable())
    {
        if (myMethod != VolumeFile::ENCLOSING_VOXEL)
        {
            CaretLogWarning("using interpolation type other than ENCLOSING_VOXEL on label volume " + inVol->getFileName());
        }
        for (int64_t i = 0; i < numMaps; ++i)
        {
            *(outVol->getMapLabelTable(i)) = *(inVol->getMapLabelTable(i));
        }
    }
    for (int64_t i = 0; i < numMaps; ++i)
    {
        outVol->setMapName(i, inVol->getMapName(i));
    }
    for (int64_t c = 0; c < numComponents; ++c)
    {
        for (int64_t b = 0; b < numMaps; ++b)
        {
            if (myMethod == VolumeFile::CUBIC)
            {
                inVol->validateSpline(b, c);//because deconvolve is parallel, but won't execute parallel if we are already in a parallel section
            }
#pragma omp CARET_PARFOR schedule(guided, 10)
            for (int64_t k = 0; k < outDims[2]; ++k)
            {
                for (int64_t j = 0; j < outDims[1]; ++j)
                {
                    for (int64_t i = 0; i < outDims[0]; ++i)
                    {
                        Vector3D outCoord;
                        outVol->indexToSpace(i, j, k, outCoord);//start with the coords of the output voxel
                        bool validCoord = false;
                        Vector3D inCoord = myStack.xfmPoint(outCoord, b, &validCoord);//put it through the inverse transforms that are in reverse order
                        if (validCoord)
                        {
                            scratchFrame[outVol->getIndex(i, j, k)] = inVol->interpolateValue(inCoord, myMethod, NULL, b, c, backgroundVal);
                        } else {
                            scratchFrame[outVol->getIndex(i, j, k)] = backgroundVal;
                        }
                    }
                }
            }
            outVol->setFrame(scratchFrame.data(), b, c);
            if (myMethod == VolumeFile::CUBIC)
            {
                inVol->freeSpline(b, c);//release memory we no longer need, if we allocated it
            }
        }
    }
}

float AlgorithmVolumeResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
