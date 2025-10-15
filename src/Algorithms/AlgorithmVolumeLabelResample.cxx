/*LICENSE_START*/
/*
 *  Copyright (C) 2025  Washington University School of Medicine
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

#include "AlgorithmVolumeLabelResample.h"
#include "AlgorithmException.h"

#include "AlgorithmVolumeResample.h"
#include "AlgorithmVolumeSmoothing.h"

#include "AffineFile.h"
#include "AffineSeriesFile.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "NiftiIO.h"
#include "WarpfieldFile.h"

#include <limits>

using namespace caret;
using namespace std;

AString AlgorithmVolumeLabelResample::getCommandSwitch()
{
    return "-volume-label-resample";
}

AString AlgorithmVolumeLabelResample::getShortDescription()
{
    return "TRANSFORM AND RESAMPLE A LABEL VOLUME FILE";
}

OperationParameters* AlgorithmVolumeLabelResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addVolumeParameter(1, "volume-in", "volume to resample");

    ret->addStringParameter(2, "volume-space", "a volume file in the volume space you want for the output");

    ret->addVolumeOutputParameter(3, "volume-out", "the output volume");

    OptionalParameter* smoothOpt = ret->createOptionalParameter(8, "-smooth-edges", "apply smoothing to the ROIs between resampling and indexmax operations, increases boundary smoothness at the cost of fidelity");
    smoothOpt->addDoubleParameter(1, "kernel-size", "smoothing amount to use, gaussian sigma in mm");
    smoothOpt->createOptionalParameter(2, "-fwhm", "use specified kernel size as full width at half maximum, rather than sigma");

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
        AString("Resample a label volume file with an arbitrary list of transformations.  ") +
        "You may specify -affine, -warp, and -affine-series multiple times each, and they will be used in the order specified.  "
        "For instance, for rigid motion correction followed by nonlinear atlas registration, specify -affine-series first, then -warp.  "
    );
    return ret;
}

void AlgorithmVolumeLabelResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    VolumeFile* inVol = myParams->getVolume(1);
    vector<int64_t> voldims = inVol->getDimensions();
    AString refSpaceName = myParams->getString(2);
    VolumeFile* outVol = myParams->getOutputVolume(3);
    auto& affInstances = myParams->getRepeatableParameterInstances(5);
    auto& affSeriesInstances = myParams->getRepeatableParameterInstances(6);
    auto& warpInstances = myParams->getRepeatableParameterInstances(7);
    float smoothVal = 0.0f;
    OptionalParameter* smoothOpt = myParams->getOptionalParameter(8);
    if (smoothOpt->m_present)
    {
        smoothVal = float(smoothOpt->getDouble(1));
        if (smoothOpt->getOptionalParameter(2)->m_present)
        {
            smoothVal = smoothVal / (2.0f * sqrt(2.0f * log(2.0f)));
        }
    }
    VolumeSpace refSpace;
    {
        NiftiIO myIO;
        myIO.openRead(refSpaceName);
        refSpace = myIO.getHeader().getVolumeSpace();
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
    AlgorithmVolumeLabelResample(myProgObj, inVol, myStack, refSpace, outVol, smoothVal);
}

AlgorithmVolumeLabelResample::AlgorithmVolumeLabelResample(ProgressObject* myProgObj, const VolumeFile* inVol, const XfmStack& myStack, const VolumeSpace refSpace,
                                                           VolumeFile* outVol, const float smoothVal) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (!(smoothVal >= 0.0f) || MathFunctions::isInf(smoothVal)) throw AlgorithmException("smoothing kernel size must be numeric and not negative");
    const vector<int64_t> inDims = inVol->getDimensions();
    const int64_t* refDims = refSpace.getDims();
    if (inVol->getType() != SubvolumeAttributes::LABEL) throw AlgorithmException("input to volume label resample must be a label volume, see -volume-label-import");
    if (inVol->getOriginalDimensions().size() < 3) throw AlgorithmException("input must have 3 spatial dimensions");
    if (inVol->getNumberOfComponents() > 1) throw AlgorithmException("input must not use a multi-component datatype");
    int32_t numMaps = inVol->getNumberOfMaps();
    outVol->reinitialize(refSpace, numMaps, 1, SubvolumeAttributes::LABEL, inVol->m_header);
    VolumeFile tempInFrame(inVol->getVolumeSpace()), tempOutFrame(refSpace), tempSmoothFrame; //for extracting ROIs, resampling, and smoothing
    if (smoothVal > 0.0f) tempSmoothFrame.reinitialize(refSpace);
    const int64_t outFrameVoxels = refDims[0] * refDims[1] * refDims[2];
    const int64_t inFrameVoxels = inDims[0] * inDims[1] * inDims[2];
    vector<float> inScratchFrame(inFrameVoxels), outScratchFrame(outFrameVoxels), outBestValue(outFrameVoxels);
    for (int32_t frame = 0; frame < numMaps; ++frame)
    {
        const GiftiLabelTable* thisTable = inVol->getMapLabelTable(frame);
        const int64_t unlabeledKey = thisTable->getUnassignedLabelKey();
        const float* thisFrame = inVol->getFrame(frame);
        for (int64_t i = 0; i < inFrameVoxels; ++i)
        {
            const int32_t thisVoxKey = int(thisFrame[i] + 0.5f);
            if (thisVoxKey == unlabeledKey || thisTable->getLabel(thisVoxKey) == NULL) //merge voxels that don't match a label into the unlabeled key
            {
                inScratchFrame[i] = 1.0f;
            } else {
                inScratchFrame[i] = 0.0f;
            }
        }
        tempInFrame.setFrame(inScratchFrame.data());
        AlgorithmVolumeResample(NULL, &tempInFrame, myStack, refSpace, VolumeFile::TRILINEAR, &tempOutFrame, unlabeledKey);
        VolumeFile* toUse = &tempOutFrame;
        if (smoothVal > 0.0f)
        {
            AlgorithmVolumeSmoothing(NULL, &tempOutFrame, smoothVal, &tempSmoothFrame);
            toUse = &tempSmoothFrame;
        }
        const float* toUseFrame = toUse->getFrame();
        for (int64_t i = 0; i < outFrameVoxels; ++i)
        {
            outScratchFrame[i] = unlabeledKey; //this is the first "label", so it wins regardless, don't bother with a pretend conditional
            outBestValue[i] = toUseFrame[i]; //also lets us skip a -inf initialization each loop
        }
        //now the normal labels
        auto labelKeys = thisTable->getKeys();
        for (auto key : labelKeys)
        {
            for (int64_t i = 0; i < inFrameVoxels; ++i)
            {
                const int32_t thisVoxKey = int(thisFrame[i] + 0.5f);
                if (thisVoxKey == key)
                {
                    inScratchFrame[i] = 1.0f;
                } else {
                    inScratchFrame[i] = 0.0f;
                }
            }
            tempInFrame.setFrame(inScratchFrame.data());
            AlgorithmVolumeResample(NULL, &tempInFrame, myStack, refSpace, VolumeFile::TRILINEAR, &tempOutFrame, unlabeledKey);
            VolumeFile* toUse = &tempOutFrame;
            if (smoothVal > 0.0f)
            {
                AlgorithmVolumeSmoothing(NULL, &tempOutFrame, smoothVal, &tempSmoothFrame);
                toUse = &tempSmoothFrame;
            }
            const float* toUseFrame = toUse->getFrame();
            for (int64_t i = 0; i < outFrameVoxels; ++i)
            {
                if (toUseFrame[i] > outBestValue[i])
                {
                    outScratchFrame[i] = key;
                    outBestValue[i] = toUseFrame[i];
                }
            }
        }
        outVol->setFrame(outScratchFrame.data(), frame);
        *(outVol->getMapLabelTable(frame)) = *thisTable;
    }
}

float AlgorithmVolumeLabelResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmVolumeLabelResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
