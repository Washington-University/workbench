/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AlgorithmCiftiResample.h"
#include "AlgorithmException.h"

#include "AffineFile.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmLabelDilate.h"
#include "AlgorithmLabelResample.h"
#include "AlgorithmMetricDilate.h"
#include "AlgorithmMetricResample.h"
#include "AlgorithmVolumeAffineResample.h"
#include "AlgorithmVolumeDilate.h"
#include "AlgorithmVolumeWarpfieldResample.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "VolumePaddingHelper.h"
#include "WarpfieldFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiResample::getCommandSwitch()
{
    return "-cifti-resample";
}

AString AlgorithmCiftiResample::getShortDescription()
{
    return "RESAMPLE A CFTI FILE TO A NEW CIFTI SPACE";
}

OperationParameters* AlgorithmCiftiResample::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the cifti file to resample");
    
    ret->addStringParameter(2, "direction", "the direction of the input that should be resampled");
    
    ret->addCiftiParameter(3, "cifti-template", "a cifti file containing the cifti space to resample to");
    
    ret->addStringParameter(4, "template-direction", "the direction of the template to use as the resampling space");
    
    ret->addStringParameter(5, "surface-method", "specify a surface resampling method");
    
    ret->addStringParameter(6, "volume-method", "specify a volume interpolation method");
    
    ret->addCiftiOutputParameter(7, "cifti-out", "the output cifti file");
    
    ret->createOptionalParameter(8, "-surface-largest", "use largest weight instead of weighted average when doing surface resampling");
    
    OptionalParameter* volDilateOpt = ret->createOptionalParameter(9, "-volume-predilate", "dilate the volume components before resampling");
    volDilateOpt->addDoubleParameter(1, "dilate-mm", "distance, in mm, to dilate");
    
    OptionalParameter* surfDilateOpt = ret->createOptionalParameter(10, "-surface-postdilate", "dilate the surface components after resampling");
    surfDilateOpt->addDoubleParameter(1, "dilate-mm", "distance, in mm, to dilate");
    
    OptionalParameter* affineOpt = ret->createOptionalParameter(11, "-affine", "use an affine transformation on the volume components");
    affineOpt->addStringParameter(1, "affine-file", "the affine file to use");
    OptionalParameter* flirtOpt = affineOpt->createOptionalParameter(2, "-flirt", "MUST be used if affine is a flirt affine");
    flirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the affine");
    flirtOpt->addStringParameter(2, "target-volume", "the target volume used when generating the affine");
    
    OptionalParameter* warpfieldOpt = ret->createOptionalParameter(12, "-warpfield", "use a warpfield on the volume components");
    warpfieldOpt->addStringParameter(1, "warpfield", "the warpfield to use");
    OptionalParameter* fnirtOpt = warpfieldOpt->createOptionalParameter(2, "-fnirt", "MUST be used if using a fnirt warpfield");
    fnirtOpt->addStringParameter(1, "source-volume", "the source volume used when generating the warpfield");
    
    OptionalParameter* leftSpheresOpt = ret->createOptionalParameter(13, "-left-spheres", "specify spheres for left surface resampling");
    leftSpheresOpt->addSurfaceParameter(1, "current-sphere", "a sphere with the same mesh as the current left surface");
    leftSpheresOpt->addSurfaceParameter(2, "new-sphere", "a sphere with the new left mesh that is in register with the current sphere");
    OptionalParameter* leftAreaSurfsOpt = leftSpheresOpt->createOptionalParameter(3, "-left-area-surfs", "specify left surfaces to do vertex area correction based on");
    leftAreaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant left anatomical surface with current mesh");
    leftAreaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant left anatomical surface with new mesh");
    
    OptionalParameter* rightSpheresOpt = ret->createOptionalParameter(14, "-right-spheres", "specify spheres for right surface resampling");
    rightSpheresOpt->addSurfaceParameter(1, "current-sphere", "a sphere with the same mesh as the current right surface");
    rightSpheresOpt->addSurfaceParameter(2, "new-sphere", "a sphere with the new right mesh that is in register with the current sphere");
    OptionalParameter* rightAreaSurfsOpt = rightSpheresOpt->createOptionalParameter(3, "-right-area-surfs", "specify right surfaces to do vertex area correction based on");
    rightAreaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant right anatomical surface with current mesh");
    rightAreaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant right anatomical surface with new mesh");
    
    OptionalParameter* cerebSpheresOpt = ret->createOptionalParameter(15, "-cerebellum-spheres", "specify spheres for cerebellum surface resampling");
    cerebSpheresOpt->addSurfaceParameter(1, "current-sphere", "a sphere with the same mesh as the current cerebellum surface");
    cerebSpheresOpt->addSurfaceParameter(2, "new-sphere", "a sphere with the new cerebellum mesh that is in register with the current sphere");
    OptionalParameter* cerebAreaSurfsOpt = cerebSpheresOpt->createOptionalParameter(3, "-cerebellum-area-surfs", "specify cerebellum surfaces to do vertex area correction based on");
    cerebAreaSurfsOpt->addSurfaceParameter(1, "current-area", "a relevant cerebellum anatomical surface with current mesh");
    cerebAreaSurfsOpt->addSurfaceParameter(2, "new-area", "a relevant cerebellum anatomical surface with new mesh");
    
    AString myHelpText =
        AString("Resample cifti data to a different brainordinate space.  Use COLUMN to resample dscalar, dlabel, or dtseries.  ") +
        "Resampling a dconn requires running the command twice, once for each direction.  " +
        "Dilation is done with the 'nearest' method, and is done on <new-sphere> for surface data.  " +
        "Volume components are padded before dilation so that dilation doesn't run into the edge of the component bounding box.\n\n" +
        "The <volume-method> argument must be one of the following:\n\n" +
        "CUBIC\nENCLOSING_VOXEL\nTRILINEAR\n\n" +
        "The <surface-method> argument must be one of the following:\n\n";
    vector<SurfaceResamplingMethodEnum::Enum> allEnums;
    SurfaceResamplingMethodEnum::getAllEnums(allEnums);
    for (int i = 0; i < (int)allEnums.size(); ++i)
    {
        myHelpText += SurfaceResamplingMethodEnum::toName(allEnums[i]) + "\n";
    }
    ret->setHelpText(myHelpText);
    return ret;
}

void AlgorithmCiftiResample::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiIn = myParams->getCifti(1);
    AString myDirString = myParams->getString(2);
    int direction = -1;
    if (myDirString == "ROW")
    {
        direction = CiftiXML::ALONG_ROW;
    } else {
        if (myDirString == "COLUMN")
        {
            direction = CiftiXML::ALONG_COLUMN;
        } else {
            throw AlgorithmException("unrecognized direction string, use ROW or COLUMN");
        }
    }
    CiftiFile* myTemplate = myParams->getCifti(3);
    AString myTemplDirString = myParams->getString(4);
    int templateDir = -1;
    if (myTemplDirString == "ROW")
    {
        templateDir = CiftiXML::ALONG_ROW;
    } else {
        if (myTemplDirString == "COLUMN")
        {
            templateDir = CiftiXML::ALONG_COLUMN;
        } else {
            throw AlgorithmException("unrecognized template direction string, use ROW or COLUMN");
        }
    }
    bool ok = false;
    SurfaceResamplingMethodEnum::Enum mySurfMethod = SurfaceResamplingMethodEnum::fromName(myParams->getString(5), &ok);
    if (!ok)
    {
        throw AlgorithmException("invalid surface resampling method name");
    }
    AString myVolMethodString = myParams->getString(6);
    VolumeFile::InterpType myVolMethod = VolumeFile::CUBIC;
    if (myVolMethodString == "CUBIC")
    {
        myVolMethod = VolumeFile::CUBIC;
    } else if (myVolMethodString == "TRILINEAR") {
        myVolMethod = VolumeFile::TRILINEAR;
    } else if (myVolMethodString == "ENCLOSING_VOXEL") {
        myVolMethod = VolumeFile::ENCLOSING_VOXEL;
    } else {
        throw AlgorithmException("unrecognized volume interpolation method");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(7);
    bool surfLargest = myParams->getOptionalParameter(8)->m_present;
    float voldilatemm = -1.0f, surfdilatemm = -1.0f;
    OptionalParameter* volDilateOpt = myParams->getOptionalParameter(9);
    if (volDilateOpt->m_present)
    {
        voldilatemm = (float)volDilateOpt->getDouble(1);
        if (voldilatemm <= 0.0f) throw AlgorithmException("dilation amount must be positive");
    }
    OptionalParameter* surfDilateOpt = myParams->getOptionalParameter(10);
    if (surfDilateOpt->m_present)
    {
        surfdilatemm = (float)surfDilateOpt->getDouble(1);
        if (surfdilatemm <= 0.0f) throw AlgorithmException("dilation amount must be positive");
    }
    OptionalParameter* affineOpt = myParams->getOptionalParameter(11);
    OptionalParameter* warpfieldOpt = myParams->getOptionalParameter(12);
    if (affineOpt->m_present && warpfieldOpt->m_present) throw AlgorithmException("you cannot specify both -affine and -warpfield");
    AffineFile myAffine;
    WarpfieldFile myWarpfield;
    if (affineOpt->m_present)
    {
        OptionalParameter* flirtOpt = affineOpt->getOptionalParameter(2);
        if (flirtOpt->m_present)
        {
            myAffine.readFlirt(affineOpt->getString(1), flirtOpt->getString(1), flirtOpt->getString(2));
        } else {
            myAffine.readWorld(affineOpt->getString(1));
        }
    }
    if (warpfieldOpt->m_present)
    {
        OptionalParameter* fnirtOpt = warpfieldOpt->getOptionalParameter(2);
        if (fnirtOpt->m_present)
        {
            myWarpfield.readFnirt(warpfieldOpt->getString(1), fnirtOpt->getString(1));
        } else {
            myWarpfield.readWorld(warpfieldOpt->getString(1));
        }
    }
    SurfaceFile* curLeftSphere = NULL, *newLeftSphere = NULL, *curLeftAreaSurf = NULL, *newLeftAreaSurf = NULL;
    OptionalParameter* leftSpheresOpt = myParams->getOptionalParameter(13);
    if (leftSpheresOpt->m_present)
    {
        curLeftSphere = leftSpheresOpt->getSurface(1);
        newLeftSphere = leftSpheresOpt->getSurface(2);
        OptionalParameter* leftAreaSurfsOpt = leftSpheresOpt->getOptionalParameter(3);
        if (leftAreaSurfsOpt->m_present)
        {
            curLeftAreaSurf = leftAreaSurfsOpt->getSurface(1);
            newLeftAreaSurf = leftAreaSurfsOpt->getSurface(2);
        } else {
            curLeftAreaSurf = curLeftSphere;
            newLeftAreaSurf = newLeftSphere;
        }
    }
    SurfaceFile* curRightSphere = NULL, *newRightSphere = NULL, *curRightAreaSurf = NULL, *newRightAreaSurf = NULL;
    OptionalParameter* rightSpheresOpt = myParams->getOptionalParameter(14);
    if (rightSpheresOpt->m_present)
    {
        curRightSphere = rightSpheresOpt->getSurface(1);
        newRightSphere = rightSpheresOpt->getSurface(2);
        OptionalParameter* rightAreaSurfsOpt = rightSpheresOpt->getOptionalParameter(3);
        if (rightAreaSurfsOpt->m_present)
        {
            curRightAreaSurf = rightAreaSurfsOpt->getSurface(1);
            newRightAreaSurf = rightAreaSurfsOpt->getSurface(2);
        } else {
            curRightAreaSurf = curRightSphere;
            newRightAreaSurf = newRightSphere;
        }
    }
    SurfaceFile* curCerebSphere = NULL, *newCerebSphere = NULL, *curCerebAreaSurf = NULL, *newCerebAreaSurf = NULL;
    OptionalParameter* cerebSpheresOpt = myParams->getOptionalParameter(15);
    if (cerebSpheresOpt->m_present)
    {
        curCerebSphere = cerebSpheresOpt->getSurface(1);
        newCerebSphere = cerebSpheresOpt->getSurface(2);
        OptionalParameter* cerebAreaSurfsOpt = cerebSpheresOpt->getOptionalParameter(3);
        if (cerebAreaSurfsOpt->m_present)
        {
            curCerebAreaSurf = cerebAreaSurfsOpt->getSurface(1);
            newCerebAreaSurf = cerebAreaSurfsOpt->getSurface(2);
        } else {
            curCerebAreaSurf = curCerebSphere;
            newCerebAreaSurf = newCerebSphere;
        }
    }
    if (warpfieldOpt->m_present)
    {
        AlgorithmCiftiResample(myProgObj, myCiftiIn, direction, myTemplate, templateDir, mySurfMethod, myVolMethod, myCiftiOut, surfLargest, voldilatemm, surfdilatemm, myWarpfield.getWarpfield(),
                               curLeftSphere, newLeftSphere, curLeftAreaSurf, newLeftAreaSurf,
                               curRightSphere, newRightSphere, curRightAreaSurf, newRightAreaSurf,
                               curCerebSphere, newCerebSphere, curCerebAreaSurf, newCerebAreaSurf);
    } else {//rely on AffineFile() being the identity transform for if neither option is specified
        AlgorithmCiftiResample(myProgObj, myCiftiIn, direction, myTemplate, templateDir, mySurfMethod, myVolMethod, myCiftiOut, surfLargest, voldilatemm, surfdilatemm, myAffine.getMatrix(),
                               curLeftSphere, newLeftSphere, curLeftAreaSurf, newLeftAreaSurf,
                               curRightSphere, newRightSphere, curRightAreaSurf, newRightAreaSurf,
                               curCerebSphere, newCerebSphere, curCerebAreaSurf, newCerebAreaSurf);
    }
}

AlgorithmCiftiResample::AlgorithmCiftiResample(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const int& direction, const CiftiFile* myTemplate, const int& templateDir,
                                               const SurfaceResamplingMethodEnum::Enum& mySurfMethod, const VolumeFile::InterpType& myVolMethod, CiftiFile* myCiftiOut,
                                               const bool& surfLargest, const float& voldilatemm, const float& surfdilatemm,
                                               const VolumeFile* warpfield,
                                               const SurfaceFile* curLeftSphere, const SurfaceFile* newLeftSphere, const SurfaceFile* curLeftAreaSurf, const SurfaceFile* newLeftAreaSurf,
                                               const SurfaceFile* curRightSphere, const SurfaceFile* newRightSphere, const SurfaceFile* curRightAreaSurf, const SurfaceFile* newRightAreaSurf,
                                               const SurfaceFile* curCerebSphere, const SurfaceFile* newCerebSphere, const SurfaceFile* curCerebAreaSurf, const SurfaceFile* newCerebAreaSurf) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (direction > 1) throw AlgorithmException("unsupported mapping direction");
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    CiftiXML myOutXML = myInputXML;
    myOutXML.copyMapping(direction, myTemplate->getCiftiXML(), templateDir);
    vector<StructureEnum::Enum> surfList, volList;
    myOutXML.getStructureLists(direction, surfList, volList);
    for (int i = 0; i < (int)surfList.size(); ++i)//ensure existence of resampling spheres before doing any computation
    {
        if (!myInputXML.hasSurfaceData(direction, surfList[i])) throw AlgorithmException("input cifti missing surface information for structure: " + StructureEnum::toGuiName(surfList[i]));
        switch (surfList[i])
        {
            case StructureEnum::CORTEX_LEFT:
                if (curLeftSphere == NULL || newLeftSphere == NULL) throw AlgorithmException("missing left spheres");
                break;
            case StructureEnum::CORTEX_RIGHT:
                if (curRightSphere == NULL || newRightSphere == NULL) throw AlgorithmException("missing right spheres");
                break;
            case StructureEnum::CEREBELLUM:
                if (curCerebSphere == NULL || newCerebSphere == NULL) throw AlgorithmException("missing cerebellum spheres");
                break;
            default:
                throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                break;
        }
    }
    myCiftiOut->setCiftiXML(myOutXML);
    for (int i = 0; i < (int)surfList.size(); ++i)//and now, resampling
    {
        const SurfaceFile* curSphere = NULL, *newSphere = NULL, *curArea = NULL, *newArea = NULL;
        switch (surfList[i])
        {
            case StructureEnum::CORTEX_LEFT:
                curSphere = curLeftSphere;
                newSphere = newLeftSphere;
                curArea = curLeftAreaSurf;
                newArea = newLeftAreaSurf;
                break;
            case StructureEnum::CORTEX_RIGHT:
                curSphere = curRightSphere;
                newSphere = newRightSphere;
                curArea = curRightAreaSurf;
                newArea = newRightAreaSurf;
                break;
            case StructureEnum::CEREBELLUM:
                curSphere = curCerebSphere;
                newSphere = newCerebSphere;
                curArea = curCerebAreaSurf;
                newArea = newCerebAreaSurf;
                break;
            default:
                throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                break;
        }
        processSurfaceComponent(myCiftiIn, direction, surfList[i], mySurfMethod, myCiftiOut, surfLargest, surfdilatemm, curSphere, newSphere, curArea, newArea);
    }
    for (int i = 0; i < (int)volList.size(); ++i)
    {
        processVolumeWarpfield(myCiftiIn, direction, volList[i], myVolMethod, myCiftiOut, voldilatemm, warpfield);
    }
}

AlgorithmCiftiResample::AlgorithmCiftiResample(ProgressObject* myProgObj, const CiftiFile* myCiftiIn, const int& direction, const CiftiFile* myTemplate, const int& templateDir,
                                               const SurfaceResamplingMethodEnum::Enum& mySurfMethod, const VolumeFile::InterpType& myVolMethod, CiftiFile* myCiftiOut,
                                               const bool& surfLargest, const float& voldilatemm, const float& surfdilatemm,
                                               const FloatMatrix& affine,
                                               const SurfaceFile* curLeftSphere, const SurfaceFile* newLeftSphere, const SurfaceFile* curLeftAreaSurf, const SurfaceFile* newLeftAreaSurf,
                                               const SurfaceFile* curRightSphere, const SurfaceFile* newRightSphere, const SurfaceFile* curRightAreaSurf, const SurfaceFile* newRightAreaSurf,
                                               const SurfaceFile* curCerebSphere, const SurfaceFile* newCerebSphere, const SurfaceFile* curCerebAreaSurf, const SurfaceFile* newCerebAreaSurf) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (direction > 1) throw AlgorithmException("unsupported mapping direction");
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    CiftiXML myOutXML = myInputXML;
    myOutXML.copyMapping(direction, myTemplate->getCiftiXML(), templateDir);
    vector<StructureEnum::Enum> surfList, volList;
    myOutXML.getStructureLists(direction, surfList, volList);
    for (int i = 0; i < (int)surfList.size(); ++i)//ensure existence of resampling spheres before doing any computation
    {
        if (!myInputXML.hasSurfaceData(direction, surfList[i])) throw AlgorithmException("input cifti missing surface information for structure: " + StructureEnum::toGuiName(surfList[i]));
        switch (surfList[i])
        {
            case StructureEnum::CORTEX_LEFT:
                if (curLeftSphere == NULL || newLeftSphere == NULL) throw AlgorithmException("missing left spheres");
                break;
            case StructureEnum::CORTEX_RIGHT:
                if (curRightSphere == NULL || newRightSphere == NULL) throw AlgorithmException("missing right spheres");
                break;
            case StructureEnum::CEREBELLUM:
                if (curCerebSphere == NULL || newCerebSphere == NULL) throw AlgorithmException("missing cerebellum spheres");
                break;
            default:
                throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                break;
        }
    }
    myCiftiOut->setCiftiXML(myOutXML);
    for (int i = 0; i < (int)surfList.size(); ++i)//and now, resampling
    {
        const SurfaceFile* curSphere = NULL, *newSphere = NULL, *curArea = NULL, *newArea = NULL;
        switch (surfList[i])
        {
            case StructureEnum::CORTEX_LEFT:
                curSphere = curLeftSphere;
                newSphere = newLeftSphere;
                curArea = curLeftAreaSurf;
                newArea = newLeftAreaSurf;
                break;
            case StructureEnum::CORTEX_RIGHT:
                curSphere = curRightSphere;
                newSphere = newRightSphere;
                curArea = curRightAreaSurf;
                newArea = newRightAreaSurf;
                break;
            case StructureEnum::CEREBELLUM:
                curSphere = curCerebSphere;
                newSphere = newCerebSphere;
                curArea = curCerebAreaSurf;
                newArea = newCerebAreaSurf;
                break;
            default:
                throw AlgorithmException("unsupported surface structure: " + StructureEnum::toGuiName(surfList[i]));
                break;
        }
        processSurfaceComponent(myCiftiIn, direction, surfList[i], mySurfMethod, myCiftiOut, surfLargest, surfdilatemm, curSphere, newSphere, curArea, newArea);
    }
    for (int i = 0; i < (int)volList.size(); ++i)
    {
        processVolumeAffine(myCiftiIn, direction, volList[i], myVolMethod, myCiftiOut, voldilatemm, affine);
    }
}

void AlgorithmCiftiResample::processSurfaceComponent(const CiftiFile* myCiftiIn, const int& direction, const StructureEnum::Enum& myStruct, const SurfaceResamplingMethodEnum::Enum& mySurfMethod,
                                                     CiftiFile* myCiftiOut, const bool& surfLargest, const float& surfdilatemm, const SurfaceFile* curSphere, const SurfaceFile* newSphere,
                                                     const SurfaceFile* curArea, const SurfaceFile* newArea)
{
    const CiftiXML& myInputXML = myCiftiIn->getCiftiXML();
    if (myInputXML.getMappingType(1 - direction) == CIFTI_INDEX_TYPE_LABELS)
    {
        LabelFile origLabel;
        AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origLabel);
        LabelFile newLabel, newDilate, *newUse;
        newUse = &newLabel;
        AlgorithmLabelResample(NULL, &origLabel, curSphere, newSphere, mySurfMethod, &newLabel, curArea, newArea, surfLargest);
        if (surfdilatemm > 0.0f)
        {
            AlgorithmLabelDilate(NULL, &newLabel, newSphere, surfdilatemm, &newDilate);
            newUse = &newDilate;
        }
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, newUse);
    } else {
        MetricFile origMetric, origROI;
        AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origMetric, &origROI);
        origMetric.writeFile("debug.orig." + StructureEnum::toName(myStruct) + ".func.gii");
        MetricFile newMetric, newDilate, resampleROI, *newUse;
        newUse = &origMetric;
        AlgorithmMetricResample(NULL, &origMetric, curSphere, newSphere, mySurfMethod, &newMetric, curArea, newArea, &origROI, &resampleROI, surfLargest);
        if (surfdilatemm > 0.0f)
        {
            MetricFile invertResampleROI;
            invertResampleROI.setNumberOfNodesAndColumns(resampleROI.getNumberOfNodes(), 1);
            for (int j = 0; j < resampleROI.getNumberOfNodes(); ++j)
            {
                float tempf = (resampleROI.getValue(j, 0) > 0.0f) ? 0.0f : 1.0f;//make an inverse ROI
                invertResampleROI.setValue(j, 0, tempf);
            }
            AlgorithmMetricDilate(NULL, &newMetric, newSphere, surfdilatemm, &newDilate, &invertResampleROI, -1, true);
            newUse = &newDilate;
        }
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, newUse);
    }
}

void AlgorithmCiftiResample::processVolumeWarpfield(const CiftiFile* myCiftiIn, const int& direction, const StructureEnum::Enum& myStruct, const VolumeFile::InterpType& myVolMethod,
                                                    CiftiFile* myCiftiOut, const float& voldilatemm, const VolumeFile* warpfield)
{
    VolumeFile origData, origROI, origDilate, *origProcess;
    origProcess = &origData;
    int64_t offset[3];
    AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origData, offset, &origROI, true);
    if (voldilatemm > 0.0f)
    {
        VolumeFile invertROI;
        invertROI.reinitialize(origROI.getOriginalDimensions(), origROI.getSform());
        vector<int64_t> dims;
        origROI.getDimensions(dims);
        int64_t frameSize = dims[0] * dims[1] * dims[2];
        const float* roiFrame = origROI.getFrame();
        vector<float> invertFrame(frameSize);
        for (int64_t j = 0; j < frameSize; ++j)
        {
            invertFrame[j] = (roiFrame[j] > 0.0f) ? 0.0f : 1.0f;
        }
        invertROI.setFrame(invertFrame.data());
        VolumePaddingHelper mypadding = VolumePaddingHelper::padMM(&origData, voldilatemm);
        VolumeFile origPad, invertROIPad;
        mypadding.doPadding(&origData, &origPad);
        mypadding.doPadding(&invertROI, &invertROIPad, 1.0f);//pad with ones since this is an inverted ROI
        AlgorithmVolumeDilate(NULL, &origPad, voldilatemm, AlgorithmVolumeDilate::NEAREST, &origDilate, &invertROIPad);
        origProcess = &origDilate;
    }
    VolumeFile newVolume;
    int64_t refdims[3], refoffset[3];
    vector<vector<float> > refsform;
    AlgorithmCiftiSeparate::getCroppedVolSpace(myCiftiOut, direction, myStruct, refdims, refsform, refoffset);
    AlgorithmVolumeWarpfieldResample(NULL, origProcess, warpfield, refdims, refsform, myVolMethod, &newVolume);
    AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, &newVolume, true);
}

void AlgorithmCiftiResample::processVolumeAffine(const CiftiFile* myCiftiIn, const int& direction, const StructureEnum::Enum& myStruct, const VolumeFile::InterpType& myVolMethod,
                                                    CiftiFile* myCiftiOut, const float& voldilatemm, const FloatMatrix& affine)
{
    VolumeFile origData, origROI, origDilate, *origProcess;
    origProcess = &origData;
    int64_t offset[3];
    AlgorithmCiftiSeparate(NULL, myCiftiIn, direction, myStruct, &origData, offset, &origROI, true);
    if (voldilatemm > 0.0f)
    {
        VolumeFile invertROI;
        invertROI.reinitialize(origROI.getOriginalDimensions(), origROI.getSform());
        vector<int64_t> dims;
        origROI.getDimensions(dims);
        int64_t frameSize = dims[0] * dims[1] * dims[2];
        const float* roiFrame = origROI.getFrame();
        vector<float> invertFrame(frameSize);
        for (int64_t j = 0; j < frameSize; ++j)
        {
            invertFrame[j] = (roiFrame[j] > 0.0f) ? 0.0f : 1.0f;
        }
        invertROI.setFrame(invertFrame.data());
        VolumePaddingHelper mypadding = VolumePaddingHelper::padMM(&origData, voldilatemm);
        VolumeFile origPad, invertROIPad;
        mypadding.doPadding(&origData, &origPad);
        mypadding.doPadding(&invertROI, &invertROIPad, 1.0f);//pad with ones since this is an inverted ROI
        AlgorithmVolumeDilate(NULL, &origPad, voldilatemm, AlgorithmVolumeDilate::NEAREST, &origDilate, &invertROIPad);
        origProcess = &origDilate;
    }
    VolumeFile newVolume;
    int64_t refdims[3], refoffset[3];
    vector<vector<float> > refsform;
    AlgorithmCiftiSeparate::getCroppedVolSpace(myCiftiOut, direction, myStruct, refdims, refsform, refoffset);
    AlgorithmVolumeAffineResample(NULL, origProcess, affine, refdims, refsform, myVolMethod, &newVolume);
    AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, direction, myStruct, &newVolume, true);
}

float AlgorithmCiftiResample::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiResample::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
