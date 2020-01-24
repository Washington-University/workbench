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

#include "AlgorithmCiftiDilate.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmLabelDilate.h"
#include "AlgorithmMetricDilate.h"
#include "AlgorithmVolumeDilate.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiDilate::getCommandSwitch()
{
    return "-cifti-dilate";
}

AString AlgorithmCiftiDilate::getShortDescription()
{
    return "DILATE A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiDilate::getParameters()
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
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(9, "-bad-brainordinate-roi", "specify an roi of brainordinates to overwrite, rather than zeros");
    roiOpt->addCiftiParameter(1, "roi-cifti", "cifti dscalar or dtseries file, positive values denote brainordinates to have their values replaced");
    
    ret->createOptionalParameter(10, "-nearest", "use nearest good value instead of a weighted average");
    
    ret->createOptionalParameter(11, "-merged-volume", "treat volume components as if they were a single component");
    
    ret->createOptionalParameter(12, "-legacy-mode", "use the math from v1.3.2 and earlier for weighted dilation");
    
    ret->setHelpText(
        AString("For all data values designated as bad, if they neighbor a good value or are within the specified distance of a good value in the same kind of model, ") +
        "replace the value with a distance weighted average of nearby good values, otherwise set the value to zero.  " +
        "If -nearest is specified, it will use the value from the closest good value within range instead of a weighted average.  " +
        "When the input file contains label data, nearest dilation is used on the surface, and weighted popularity is used in the volume.\n\n" +
        "The -*-corrected-areas options are intended for dilating on group average surfaces, but it is only an approximate correction " +
        "for the reduction of structure in a group average surface.\n\n" +
        "If -bad-brainordinate-roi is specified, all values, including those with value zero, are good, except for locations with a positive value in the ROI.  " +
        "If it is not specified, only values equal to zero are bad."
    );
    return ret;
}

void AlgorithmCiftiDilate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    AString directionName = myParams->getString(2);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXMLOld::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXMLOld::ALONG_COLUMN;
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
    CiftiFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(9);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getCifti(1);
    }
    bool nearest = myParams->getOptionalParameter(10)->m_present;
    bool mergedVolume = myParams->getOptionalParameter(11)->m_present;
    bool legacyMode = myParams->getOptionalParameter(12)->m_present;
    AlgorithmCiftiDilate(myProgObj, myCifti, myDir, surfDist, volDist, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, myLeftAreas, myRightAreas, myCerebAreas, myRoi, nearest, mergedVolume, legacyMode);
}

AlgorithmCiftiDilate::AlgorithmCiftiDilate(ProgressObject* myProgObj, const CiftiFile* myCifti, const int& myDir, const float& surfDist, const float& volDist, CiftiFile* myCiftiOut,
                                           const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf,
                                           const MetricFile* myLeftAreas, const MetricFile* myRightAreas, const MetricFile* myCerebAreas,
                                           const CiftiFile* myBadRoi, const bool& nearest, const bool& mergedVolume, const bool legacyMode) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld myXML = myCifti->getCiftiXMLOld();
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (myDir != CiftiXMLOld::ALONG_ROW && myDir != CiftiXMLOld::ALONG_COLUMN) throw AlgorithmException("direction not supported by cifti dilate");
    if (myBadRoi != NULL && !myXML.mappingMatches(myDir, myBadRoi->getCiftiXMLOld(), CiftiXMLOld::ALONG_COLUMN)) throw AlgorithmException("bad brainordinate roi has different brainordinate space than input");
    if (!myXML.getStructureLists(myDir, surfaceList, volumeList))
    {
        throw AlgorithmException("specified direction does not contain brainordinates");
    }
    float volExponent = 7.0f, surfExponent = 6.0f;
    if (legacyMode)
    {
        volExponent = 2.0f;
        surfExponent = 2.0f;
    }
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
        if (mySurf->getNumberOfNodes() != myXML.getSurfaceNumberOfNodes(myDir, surfaceList[whichStruct]))
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
        MetricFile badRoiMetric, dataRoiMetric;
        MetricFile* badRoiPtr = NULL;
        if (myBadRoi != NULL)
        {
            AlgorithmCiftiSeparate(NULL, myBadRoi, CiftiXMLOld::ALONG_COLUMN, surfaceList[whichStruct], &badRoiMetric);
            badRoiPtr = &badRoiMetric;
        }
        if (myXML.getMappingType(1 - myDir) == CIFTI_INDEX_TYPE_LABELS)
        {
            LabelFile myLabel, myLabelOut;
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myLabel);
            AlgorithmLabelDilate(NULL, &myLabel, mySurf, surfDist, &myLabelOut, badRoiPtr, -1, myCorrAreas);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myLabelOut);
        } else {
            MetricFile myMetric, myMetricOut;
            AlgorithmMetricDilate::Method myMethod = AlgorithmMetricDilate::WEIGHTED;
            if (nearest) myMethod = AlgorithmMetricDilate::NEAREST;
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &dataRoiMetric);
            AlgorithmMetricDilate(NULL, &myMetric, mySurf, surfDist, &myMetricOut, badRoiPtr, &dataRoiMetric, -1, myMethod, surfExponent, myCorrAreas, legacyMode);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
        }
    }
    if (mergedVolume)
    {
        if (myXML.hasVolumeData(myDir))
        {
            VolumeFile myVol, roiVol, myVolOut, dataRoi, junkVol;
            VolumeFile* roiPtr = NULL, *dataRoiPtr = &dataRoi;
            if (legacyMode) dataRoiPtr = NULL;
            int64_t offset[3];
            AlgorithmVolumeDilate::Method myMethod = AlgorithmVolumeDilate::WEIGHTED;
            if (nearest)
            {
                myMethod = AlgorithmVolumeDilate::NEAREST;
            }
            if (myBadRoi != NULL)
            {
                AlgorithmCiftiSeparate(NULL, myBadRoi, CiftiXMLOld::ALONG_COLUMN, &roiVol, offset, NULL, true);
                roiPtr = &roiVol;
            }
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, &myVol, offset, &dataRoi, true);
            AlgorithmVolumeDilate(NULL, &myVol, volDist, myMethod, &myVolOut, roiPtr, dataRoiPtr, -1, volExponent, legacyMode);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, &myVolOut, true);
        }
    } else {
        AlgorithmVolumeDilate::Method myMethod = AlgorithmVolumeDilate::WEIGHTED;
        if (nearest)
        {
            myMethod = AlgorithmVolumeDilate::NEAREST;
        }
        for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
        {
            VolumeFile myVol, badRoiVol, myVolOut, dataRoiVol;
            VolumeFile* roiPtr = NULL;
            int64_t offset[3];
            if (myBadRoi != NULL)
            {
                AlgorithmCiftiSeparate(NULL, myBadRoi, CiftiXMLOld::ALONG_COLUMN, volumeList[whichStruct], &badRoiVol, offset, NULL, true);
                roiPtr = &badRoiVol;
            }
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &dataRoiVol, true);
            AlgorithmVolumeDilate(NULL, &myVol, volDist, myMethod, &myVolOut, roiPtr, &dataRoiVol);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
        }
    }
}

float AlgorithmCiftiDilate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiDilate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
