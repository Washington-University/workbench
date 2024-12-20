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

#include "AlgorithmCiftiSmoothing.h"
#include "AlgorithmException.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmVolumeSmoothing.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "SurfaceFile.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiSmoothing::getCommandSwitch()
{
    return "-cifti-smoothing";
}

AString AlgorithmCiftiSmoothing::getShortDescription()
{
    return "SMOOTH A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    ret->addDoubleParameter(2, "surface-kernel", "the size of the gaussian surface smoothing kernel in mm, as sigma by default");
    ret->addDoubleParameter(3, "volume-kernel", "the size of the gaussian volume smoothing kernel in mm, as sigma by default");
    ret->addStringParameter(4, "direction", "which dimension to smooth along, ROW or COLUMN");
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti");
    
    ret->createOptionalParameter(13, "-fwhm", "kernel sizes are FWHM, not sigma");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(6, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    OptionalParameter* leftCorrAreasOpt = leftSurfOpt->createOptionalParameter(2, "-left-corrected-areas", "vertex areas to use instead of computing them from the left surface");
    leftCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(7, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    OptionalParameter* rightCorrAreasOpt = rightSurfOpt->createOptionalParameter(2, "-right-corrected-areas", "vertex areas to use instead of computing them from the right surface");
    rightCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* cerebSurfOpt = ret->createOptionalParameter(8, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    OptionalParameter* cerebCorrAreasOpt = cerebSurfOpt->createOptionalParameter(2, "-cerebellum-corrected-areas", "vertex areas to use instead of computing them from the cerebellum surface");
    cerebCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ParameterComponent* genSurfOpt = ret->createRepeatableParameter(13, "-surface", "specify a surface by structure name");
    genSurfOpt->addStringParameter(1, "structure", "the surface structure name");
    genSurfOpt->addSurfaceParameter(2, "surface", "the surface file");
    OptionalParameter* genCorrAreasOpt = genSurfOpt->createOptionalParameter(3, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    genCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(9, "-cifti-roi", "smooth only within regions of interest");
    roiOpt->addCiftiParameter(1, "roi-cifti", "the regions to smooth within, as a cifti file");
    
    ret->createOptionalParameter(10, "-fix-zeros-volume", "treat values of zero in the volume as missing data");
    
    ret->createOptionalParameter(11, "-fix-zeros-surface", "treat values of zero on the surface as missing data");
    
    ret->createOptionalParameter(12, "-merged-volume", "smooth across subcortical structure boundaries");
    
    AString helpText =
        AString("The input cifti file must have a brain models mapping on the chosen dimension, columns for .dtseries, and either for .dconn.  ") +
        "By default, data in different structures is smoothed independently (i.e., \"parcel constrained\" smoothing), so volume structures that touch do not smooth across this boundary.  " +
        "Specify -merged-volume to ignore these boundaries.  " +
        "Surface smoothing uses the GEO_GAUSS_AREA smoothing method.\n\n" +
        "The -*-corrected-areas options are intended for when it is unavoidable to smooth on group average surfaces, it is only an approximate correction " +
        "for the reduction of structure in a group average surface.  It is better to smooth the data on individuals before averaging, when feasible.\n\n" +
        "The -fix-zeros-* options will treat values of zero as lack of data, and not use that value when generating the smoothed values, but will fill zeros with extrapolated values.  " +
        "The ROI should have a brain models mapping along columns, exactly matching the mapping of the chosen direction in the input file.  " +
        "Data outside the ROI is ignored.\n\n" +
        "The <structure> argument of -surface must be one of the following strings:\n";
    vector<StructureEnum::Enum> myStructureEnums;
    StructureEnum::getAllEnums(myStructureEnums);
    for (int i = 0; i < (int)myStructureEnums.size(); ++i)
    {
        helpText += "\n" + StructureEnum::toName(myStructureEnums[i]);
    }
    ret->setHelpText(helpText);
    return ret;
}

void AlgorithmCiftiSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float surfKern = (float)myParams->getDouble(2);
    float volKern = (float)myParams->getDouble(3);
    if (myParams->getOptionalParameter(13)->m_present)
    {
        surfKern = surfKern / (2.0f * sqrt(2.0f * log(2.0f)));
        volKern = volKern / (2.0f * sqrt(2.0f * log(2.0f)));
    }
    AString directionName = myParams->getString(4);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXMLOld::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXMLOld::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(5);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    MetricFile* myLeftAreas = NULL, *myRightAreas = NULL, *myCerebAreas = NULL;
    map<StructureEnum::Enum, SurfParam> surfArgs;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(6);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
        OptionalParameter* leftCorrAreasOpt = leftSurfOpt->getOptionalParameter(2);
        if (leftCorrAreasOpt->m_present)
        {
            myLeftAreas = leftCorrAreasOpt->getMetric(1);
        }
        surfArgs[StructureEnum::CORTEX_LEFT] = SurfParam(myLeftSurf, myLeftAreas);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(7);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
        OptionalParameter* rightCorrAreasOpt = rightSurfOpt->getOptionalParameter(2);
        if (rightCorrAreasOpt->m_present)
        {
            myRightAreas = rightCorrAreasOpt->getMetric(1);
        }
        surfArgs[StructureEnum::CORTEX_RIGHT] = SurfParam(myRightSurf, myRightAreas);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(8);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
        OptionalParameter* cerebCorrAreasOpt = cerebSurfOpt->getOptionalParameter(2);
        if (cerebCorrAreasOpt->m_present)
        {
            myCerebAreas = cerebCorrAreasOpt->getMetric(1);
        }
        surfArgs[StructureEnum::CEREBELLUM] = SurfParam(myCerebSurf, myCerebAreas);
    }
    auto genSurfArgs = myParams->getRepeatableParameterInstances(13);
    for (auto instance : genSurfArgs)
    {
        bool ok = false;
        StructureEnum::Enum structure = StructureEnum::fromName(instance->getString(1), &ok);
        if (!ok) throw AlgorithmException("unrecognized structure identifier: " + instance->getString(1));
        if (surfArgs.find(structure) != surfArgs.end()) throw AlgorithmException("more than one surface argument specified for structure '" + instance->getString(1) + "'");
        auto areasOpt = instance->getOptionalParameter(3);
        if (areasOpt->m_present)
        {
            surfArgs[structure] = SurfParam(instance->getSurface(2), areasOpt->getMetric(1));
        } else {
            surfArgs[structure] = SurfParam(instance->getSurface(2));
        }
    }
    CiftiFile* roiCifti = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(9);
    if (roiOpt->m_present)
    {
        roiCifti = roiOpt->getCifti(1);
    }
    bool fixZerosVol = myParams->getOptionalParameter(10)->m_present;
    bool fixZerosSurf = myParams->getOptionalParameter(11)->m_present;
    bool mergedVolume = myParams->getOptionalParameter(12)->m_present;
    AlgorithmCiftiSmoothing(myProgObj, myCifti, surfKern, volKern, myDir, myCiftiOut,
                            surfArgs,
                            roiCifti, fixZerosVol, fixZerosSurf,
                            mergedVolume);
}

AlgorithmCiftiSmoothing::AlgorithmCiftiSmoothing(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfKern, const float& volKern, const int& myDir, CiftiFile* myCiftiOut,
                                                 const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf,
                                                 const CiftiFile* roiCifti, bool fixZerosVol, bool fixZerosSurf,
                                                 const MetricFile* myLeftAreas, const MetricFile* myRightAreas, const MetricFile* myCerebAreas, const bool& mergedVolume) : AbstractAlgorithm(NULL)
{
    map<StructureEnum::Enum, SurfParam> surfParams;
    if (myLeftSurf != NULL) surfParams[StructureEnum::CORTEX_LEFT] = SurfParam(myLeftSurf, myLeftAreas);
    if (myRightSurf != NULL) surfParams[StructureEnum::CORTEX_RIGHT] = SurfParam(myRightSurf, myRightAreas);
    if (myCerebSurf != NULL) surfParams[StructureEnum::CEREBELLUM] = SurfParam(myCerebSurf, myCerebAreas);
    AlgorithmCiftiSmoothing(myProgObj, myCifti, surfKern, volKern, myDir, myCiftiOut,
                            surfParams,
                            roiCifti, fixZerosVol, fixZerosSurf,
                            mergedVolume);
}

AlgorithmCiftiSmoothing::AlgorithmCiftiSmoothing(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfKern, const float& volKern, const int& myDir, CiftiFile* myCiftiOut,
                                                 const map<StructureEnum::Enum, AlgorithmCiftiSmoothing::SurfParam> surfParams,
                                                 const CiftiFile* roiCifti, bool fixZerosVol, bool fixZerosSurf,
                                                 const bool& mergedVolume) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (!(surfKern > 0.0f) && !(volKern > 0.0f)) throw AlgorithmException("zero smoothing kernels requested for both volume and surface");
    const CiftiXMLOld& myXML = myCifti->getCiftiXMLOld();
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (myDir == CiftiXMLOld::ALONG_COLUMN)
    {
        if (!myXML.getStructureListsForColumns(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
    } else {
        if (myDir != CiftiXMLOld::ALONG_ROW) throw AlgorithmException("direction not supported in AlgorithmCiftiSmoothing");
        if (!myXML.getStructureListsForRows(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
    }
    if (roiCifti != NULL && !myXML.mappingMatches(myDir, roiCifti->getCiftiXMLOld(), CiftiXMLOld::ALONG_COLUMN))
    {
        throw AlgorithmException("along-column mapping of roi cifti does not match the smoothing direction of the input cifti");
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        AString surfType = StructureEnum::toName(surfaceList[whichStruct]);
        auto result = surfParams.find(surfaceList[whichStruct]);
        if (result == surfParams.end()) throw AlgorithmException(surfType + " surface required but not provided");
        const SurfaceFile* mySurf = result->second.surface;
        const MetricFile* myAreas = result->second.correctedAreas;
        if (mySurf == NULL)
        {
            throw AlgorithmException(surfType + " surface required but not provided");
        }
        if (myDir == CiftiXMLOld::ALONG_COLUMN)
        {
            if (mySurf->getNumberOfNodes() != myXML.getColumnSurfaceNumberOfNodes(surfaceList[whichStruct]))
            {
                throw AlgorithmException(surfType + " surface has the wrong number of vertices");
            }
        } else {
            if (mySurf->getNumberOfNodes() != myXML.getRowSurfaceNumberOfNodes(surfaceList[whichStruct]))
            {
                throw AlgorithmException(surfType + " surface has the wrong number of vertices");
            }
        }
        if (myAreas != NULL && myAreas->getNumberOfNodes() != mySurf->getNumberOfNodes())
        {
            throw AlgorithmException(surfType + " surface and vertex area metric have different number of vertices");
        }
    }
    myCiftiOut->setCiftiXML(myXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        auto result = surfParams.find(surfaceList[whichStruct]);
        CaretAssert(result != surfParams.end()); //we already checked that these are all present, but assert anyway
        const SurfaceFile* mySurf = result->second.surface;
        const MetricFile* myAreas = result->second.correctedAreas;
        MetricFile myMetric, myRoi, myMetricOut;
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &myRoi);
        if (surfKern > 0.0f)
        {
            if (roiCifti != NULL)
            {//due to above testing, we know the structure mask is the same, so just overwrite the ROI from the mask
                AlgorithmCiftiSeparate(NULL, roiCifti, CiftiXMLOld::ALONG_COLUMN, surfaceList[whichStruct], &myRoi);
            }
            AlgorithmMetricSmoothing(NULL, mySurf, &myMetric, surfKern, &myMetricOut, &myRoi, false, fixZerosSurf, -1, myAreas);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
        } else {
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetric);
        }
    }
    if (mergedVolume)
    {
        VolumeFile myVol, myRoi, myVolOut;
        int64_t offset[3];
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, &myVol, offset, &myRoi, true);
        if (volKern > 0.0f)
        {
            if (roiCifti != NULL)
            {//due to above testing, we know the structure mask is the same, so just overwrite the ROI from the mask
                AlgorithmCiftiSeparate(NULL, roiCifti, CiftiXMLOld::ALONG_COLUMN, &myRoi, offset, NULL, true);
            }
            AlgorithmVolumeSmoothing(NULL, &myVol, volKern, &myVolOut, &myRoi, fixZerosVol);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, &myVolOut, true);
        } else {
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, &myVol, true);
        }
    } else {
        for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
        {
            VolumeFile myVol, myRoi, myVolOut;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &myRoi, true);
            if (volKern > 0.0f)
            {
                if (roiCifti != NULL)
                {//due to above testing, we know the structure mask is the same, so just overwrite the ROI from the mask
                    AlgorithmCiftiSeparate(NULL, roiCifti, CiftiXMLOld::ALONG_COLUMN, volumeList[whichStruct], &myRoi, offset, NULL, true);
                }
                AlgorithmVolumeSmoothing(NULL, &myVol, volKern, &myVolOut, &myRoi, fixZerosVol);
                AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
            } else {
                AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVol, true);
            }
        }
    }
}

float AlgorithmCiftiSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiSmoothing::getSubAlgorithmWeight()
{
    return 0.0f;//if you use a subalgorithm
}
