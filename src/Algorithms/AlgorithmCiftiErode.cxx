/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AlgorithmCiftiErode.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmLabelErode.h"
#include "AlgorithmMetricErode.h"
#include "AlgorithmVolumeErode.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiErode::getCommandSwitch()
{
    return "-cifti-erode";
}

AString AlgorithmCiftiErode::getShortDescription()
{
    return "ERODE A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiErode::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the input cifti file");
    
    ret->addStringParameter(2, "direction", "which dimension to dilate along, ROW or COLUMN");
    
    ret->addDoubleParameter(3, "surface-distance", "the distance to dilate on surfaces, in mm");
    
    ret->addDoubleParameter(4, "volume-distance", "the distance to dilate in the volume, in mm");
    
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti file");
    
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
    
    ret->createOptionalParameter(9, "-merged-volume", "treat volume components as if they were a single component");

    ret->setHelpText(
        AString("For all data values that are empty (for label data, unlabeled, for other data, zero), set the surrounding values to empty.  ") +
        "The surrounding values are defined as the immediate neighbors and all values in the same structure within the specified distance " +
        "(-merged-volume treats all voxels as one structure).\n\n" +
        "The -*-corrected-areas options are intended for eroding on group average surfaces, but it is only an approximate correction."
    );
    return ret;
}

void AlgorithmCiftiErode::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    AString directionName = myParams->getString(2);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXML::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXML::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    float surfDist = (float)myParams->getDouble(3);
    float volDist = (float)myParams->getDouble(4);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(5);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    MetricFile* myLeftAreas = NULL, *myRightAreas = NULL, *myCerebAreas = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(6);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
        OptionalParameter* leftCorrAreasOpt = leftSurfOpt->getOptionalParameter(2);
        if (leftCorrAreasOpt->m_present)
        {
            myLeftAreas = leftCorrAreasOpt->getMetric(1);
        }
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
    }
    bool mergedVolume = myParams->getOptionalParameter(9)->m_present;
    AlgorithmCiftiErode(myProgObj, myCifti, myDir, surfDist, volDist, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, myLeftAreas, myRightAreas, myCerebAreas, mergedVolume);
}

AlgorithmCiftiErode::AlgorithmCiftiErode(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir, const float& surfDist, const float& volDist, CiftiFile* myCiftiOut,
                                         const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf,
                                         const MetricFile* myLeftAreas, const MetricFile* myRightAreas, const MetricFile* myCerebAreas, const bool& mergedVolume) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = myCifti->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2)
    {
        throw AlgorithmException("cifti erode only supports 2D cifti");
    }
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("specified direction does not contain brainordinates");
    }
    const CiftiBrainModelsMap& myDenseMap = myXML.getBrainModelsMap(myDir);
    vector<StructureEnum::Enum> surfaceList = myDenseMap.getSurfaceStructureList();
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        const SurfaceFile* mySurf = NULL;
        const MetricFile* myCorrAreas = NULL;
        AString surfType;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                myCorrAreas = myLeftAreas;
                surfType = "left";
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                myCorrAreas = myRightAreas;
                surfType = "right";
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                myCorrAreas = myCerebAreas;
                surfType = "cerebellum";
                break;
            default:
                throw AlgorithmException("found surface model with incorrect type: " + StructureEnum::toName(surfaceList[whichStruct]));
                break;
        }
        if (mySurf == NULL)
        {
            throw AlgorithmException(surfType + " surface required but not provided");
        }
        if (mySurf->getNumberOfNodes() != myDenseMap.getSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
        if (myCorrAreas != NULL && myCorrAreas->getNumberOfNodes() != mySurf->getNumberOfNodes())
        {
            throw AlgorithmException(surfType + " surface and corrected areas metric have different numbers of vertices");
        }
    }
    myCiftiOut->setCiftiXML(myXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        const SurfaceFile* mySurf = NULL;
        const MetricFile* myCorrAreas = NULL;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                myCorrAreas = myLeftAreas;
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                myCorrAreas = myRightAreas;
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                myCorrAreas = myCerebAreas;
                break;
            default:
                break;
        }
        MetricFile dataRoiMetric;
        if (myXML.getMappingType(1 - myDir) == CiftiMappingType::LABELS)
        {
            LabelFile myLabel, myLabelOut;
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myLabel, &dataRoiMetric);
            AlgorithmLabelErode(NULL, &myLabel, mySurf, surfDist, &myLabelOut, &dataRoiMetric, -1, myCorrAreas);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myLabelOut);
        } else {
            MetricFile myMetric, myMetricOut;
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &dataRoiMetric);
            AlgorithmMetricErode(NULL, &myMetric, mySurf, surfDist, &myMetricOut, &dataRoiMetric, -1, myCorrAreas);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
        }
    }
    if (mergedVolume)
    {
        if (myDenseMap.hasVolumeData())
        {
            VolumeFile myVol, roiVol, myVolOut;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, &myVol, offset, &roiVol, true);
            AlgorithmVolumeErode(NULL, &myVol, volDist, &myVolOut, &roiVol);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, &myVolOut, true);
        }
    } else {
        vector<StructureEnum::Enum> volumeList = myDenseMap.getVolumeStructureList();
        for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
        {
            VolumeFile myVol, myVolOut, dataRoiVol;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &dataRoiVol, true);
            AlgorithmVolumeErode(NULL, &myVol, volDist, &myVolOut, &dataRoiVol);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
        }
    }
}

float AlgorithmCiftiErode::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiErode::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
