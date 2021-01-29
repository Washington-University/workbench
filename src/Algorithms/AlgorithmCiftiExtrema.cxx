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

#include "AlgorithmCiftiExtrema.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmMetricExtrema.h"
#include "AlgorithmVolumeExtrema.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiExtrema::getCommandSwitch()
{
    return "-cifti-extrema";
}

AString AlgorithmCiftiExtrema::getShortDescription()
{
    return "FIND EXTREMA IN A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiExtrema::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    ret->addDoubleParameter(2, "surface-distance", "the minimum distance between extrema of the same type, for surface components");
    ret->addDoubleParameter(3, "volume-distance", "the minimum distance between extrema of the same type, for volume components");
    ret->addStringParameter(4, "direction", "which dimension to find extrema along, ROW or COLUMN");
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(6, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(7, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(8, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    OptionalParameter* presmoothSurfOpt = ret->createOptionalParameter(9, "-surface-presmooth", "smooth on the surface before finding extrema");
    presmoothSurfOpt->addDoubleParameter(1, "surface-kernel", "the size of the gaussian surface smoothing kernel in mm, as sigma by default");
    
    OptionalParameter* presmoothVolOpt = ret->createOptionalParameter(10, "-volume-presmooth", "smooth volume components before finding extrema");
    presmoothVolOpt->addDoubleParameter(1, "volume-kernel", "the size of the gaussian volume smoothing kernel in mm, as sigma by default");
    
    ret->createOptionalParameter(17, "-presmooth-fwhm", "smoothing kernel distances are FWHM, not sigma");

    OptionalParameter* thresholdOpt = ret->createOptionalParameter(11, "-threshold", "ignore small extrema");
    thresholdOpt->addDoubleParameter(1, "low", "the largest value to consider for being a minimum");
    thresholdOpt->addDoubleParameter(2, "high", "the smallest value to consider for being a maximum");
    
    ret->createOptionalParameter(12, "-merged-volume", "treat volume components as if they were a single component");
    
    ret->createOptionalParameter(13, "-sum-maps", "output the sum of the extrema maps instead of each map separately");
    
    ret->createOptionalParameter(14, "-consolidate-mode", "use consolidation of local minima instead of a large neighborhood");
    
    ret->createOptionalParameter(15, "-only-maxima", "only find the maxima");
    
    ret->createOptionalParameter(16, "-only-minima", "only find the minima");

    ret->setHelpText(
        AString("Finds spatial locations in a cifti file that have more extreme values than all nearby locations in the same component (surface or volume structure).  ") +
        "The input cifti file must have a brain models mapping along the specified direction.  " +
        "COLUMN is the direction that works on dtseries and dscalar.  For dconn, if it is symmetric use COLUMN, otherwise use ROW."
    );
    return ret;
}

void AlgorithmCiftiExtrema::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float surfDist = (float)myParams->getDouble(2);
    float volDist = (float)myParams->getDouble(3);
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
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(6);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(7);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(8);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
    }
    float surfPresmooth = -1.0f;
    OptionalParameter* presmoothSurfOpt = myParams->getOptionalParameter(9);
    if (presmoothSurfOpt->m_present)
    {
        surfPresmooth = (float)presmoothSurfOpt->getDouble(1);
    }
    float volPresmooth = -1.0f;
    OptionalParameter* presmoothVolOpt = myParams->getOptionalParameter(10);
    if (presmoothVolOpt->m_present)
    {
        volPresmooth = (float)presmoothVolOpt->getDouble(1);
    }
    if (myParams->getOptionalParameter(17)->m_present)
    {
        if (surfPresmooth > 0.0f) surfPresmooth = surfPresmooth / (2.0f * sqrt(2.0f * log(2.0f)));
        if (volPresmooth > 0.0f) volPresmooth = volPresmooth / (2.0f * sqrt(2.0f * log(2.0f)));
    }
    OptionalParameter* thresholdOpt = myParams->getOptionalParameter(11);
    bool thresholdMode = false;
    float lowThresh = 0.0f, highThresh = 0.0f;
    if (thresholdOpt->m_present)
    {
        thresholdMode = true;
        lowThresh = (float)thresholdOpt->getDouble(1);
        highThresh = (float)thresholdOpt->getDouble(2);
    }
    bool mergedVolume = myParams->getOptionalParameter(12)->m_present;
    bool sumMaps = myParams->getOptionalParameter(13)->m_present;
    bool consolidateMode = myParams->getOptionalParameter(14)->m_present;
    bool ignoreMinima = myParams->getOptionalParameter(15)->m_present;
    bool ignoreMaxima = myParams->getOptionalParameter(16)->m_present;
    if (ignoreMinima && ignoreMaxima) throw AlgorithmException("you may not specify both -only-maxima and -only-minima");
    AlgorithmCiftiExtrema(myProgObj, myCifti, surfDist, volDist, myDir, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, surfPresmooth,
                          volPresmooth, thresholdMode, lowThresh, highThresh, mergedVolume, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
}

AlgorithmCiftiExtrema::AlgorithmCiftiExtrema(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfDist, const float& volDist, const int& myDir,
                                             CiftiFile* myCiftiOut, const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf,
                                             const float& surfPresmooth, const float& volPresmooth, const bool& thresholdMode, const float& lowThresh,
                                             const float& highThresh, const bool& mergedVolume, const bool& sumMaps, const bool& consolidateMode,
                                             const bool& ignoreMinima, const bool& ignoreMaxima) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld myXML = myCifti->getCiftiXMLOld(), myOutXML;
    myOutXML = myXML;
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (myDir == CiftiXMLOld::ALONG_COLUMN)
    {
        if (!myXML.getStructureListsForColumns(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
    } else {
        if (!myXML.getStructureListsForRows(surfaceList, volumeList))
        {
            throw AlgorithmException("specified direction does not contain brainordinates");
        }
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        const SurfaceFile* mySurf = NULL;
        AString surfType;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                surfType = "left";
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                surfType = "right";
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
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
    }
    int outDir = myDir;
    if (sumMaps)
    {
        if (myDir == CiftiXMLOld::ALONG_ROW)//NOTE: when using along row and -sum-maps, make a dscalar, not a scalard
        {
            myOutXML.copyMapping(CiftiXMLOld::ALONG_COLUMN, myXML, CiftiXMLOld::ALONG_ROW);//so, transpose the maps if we are using dense along row
            myOutXML.copyMapping(CiftiXMLOld::ALONG_ROW, myXML, CiftiXMLOld::ALONG_COLUMN);
        }
        outDir = CiftiXMLOld::ALONG_COLUMN;
        myOutXML.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, 1);
        myOutXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 0, "sum of extrema");
    }
    myCiftiOut->setCiftiXML(myOutXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        const SurfaceFile* mySurf = NULL;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                break;
            default:
                break;
        }
        MetricFile myMetric, myRoi, myMetricOut;
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &myRoi);
        if (thresholdMode)
        {
            AlgorithmMetricExtrema(NULL, mySurf, &myMetric, surfDist, &myMetricOut, lowThresh, highThresh, &myRoi, surfPresmooth, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
        } else {
            AlgorithmMetricExtrema(NULL, mySurf, &myMetric, surfDist, &myMetricOut, &myRoi, surfPresmooth, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
        }
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, outDir, surfaceList[whichStruct], &myMetricOut);
    }
    if (mergedVolume)
    {
        if (myCifti->getCiftiXMLOld().hasVolumeData(myDir))
        {
            VolumeFile myVol, myRoi, myVolOut;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, &myVol, offset, &myRoi, true);
            if (thresholdMode)
            {
                AlgorithmVolumeExtrema(NULL, &myVol, volDist, &myVolOut, lowThresh, highThresh, &myRoi, volPresmooth, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
            } else {
                AlgorithmVolumeExtrema(NULL, &myVol, volDist, &myVolOut, &myRoi, volPresmooth, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
            }
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, outDir, &myVolOut, true);
        }
    } else {
        for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
        {
            VolumeFile myVol, myRoi, myVolOut;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &myRoi, true);
            if (thresholdMode)
            {
                AlgorithmVolumeExtrema(NULL, &myVol, volDist, &myVolOut, lowThresh, highThresh, &myRoi, volPresmooth, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
            } else {
                AlgorithmVolumeExtrema(NULL, &myVol, volDist, &myVolOut, &myRoi, volPresmooth, sumMaps, consolidateMode, ignoreMinima, ignoreMaxima);
            }
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, outDir, volumeList[whichStruct], &myVolOut, true);
        }
    }
}

float AlgorithmCiftiExtrema::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiExtrema::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
