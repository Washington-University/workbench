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

#include "AlgorithmCiftiFindClusters.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmCiftiReplaceStructure.h"
#include "AlgorithmMetricFindClusters.h"
#include "AlgorithmVolumeFindClusters.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiFindClusters::getCommandSwitch()
{
    return "-cifti-find-clusters";
}

AString AlgorithmCiftiFindClusters::getShortDescription()
{
    return "FILTER CLUSTERS BY AREA/VOLUME";
}

OperationParameters* AlgorithmCiftiFindClusters::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    ret->addDoubleParameter(2, "surface-value-threshold", "threshold for surface data values");
    ret->addDoubleParameter(3, "surface-minimum-area", "threshold for surface cluster area, in mm^2");
    ret->addDoubleParameter(4, "volume-value-threshold", "threshold for volume data values");
    ret->addDoubleParameter(5, "volume-minimum-size", "threshold for volume cluster size, in mm^3");
    ret->addStringParameter(6, "direction", "which dimension to use for spatial information, ROW or COLUMN");
    ret->addCiftiOutputParameter(7, "cifti-out", "the output cifti");
    
    ret->createOptionalParameter(8, "-less-than", "find values less than <value-threshold>, rather than greater");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(9, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    OptionalParameter* leftCorrAreasOpt = leftSurfOpt->createOptionalParameter(2, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    leftCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(10, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    OptionalParameter* rightCorrAreasOpt = rightSurfOpt->createOptionalParameter(2, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    rightCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(11, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    OptionalParameter* cerebCorrAreasOpt = cerebSurfaceOpt->createOptionalParameter(2, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    cerebCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(12, "-cifti-roi", "search only within regions of interest");
    roiOpt->addCiftiParameter(1, "roi-cifti", "the regions to search within, as a cifti file");
    
    ret->createOptionalParameter(13, "-merged-volume", "treat volume components as if they were a single component");
    
    OptionalParameter* sizeRatioOpt = ret->createOptionalParameter(15, "-size-ratio", "ignore clusters smaller than a given fraction of the largest cluster in the structure");
    sizeRatioOpt->addDoubleParameter(1, "surface-ratio", "fraction of the structure's largest cluster area");
    sizeRatioOpt->addDoubleParameter(2, "volume-ratio", "fraction of the structure's largest cluster volume");
    
    OptionalParameter* distanceOpt = ret->createOptionalParameter(16, "-distance", "ignore clusters further than a given distance from the largest cluster in the structure");
    distanceOpt->addDoubleParameter(1, "surface-distance", "how far from the largest cluster a cluster can be, edge to edge, in mm");
    distanceOpt->addDoubleParameter(2, "volume-distance", "how far from the largest cluster a cluster can be, edge to edge, in mm");
    
    OptionalParameter* startOpt = ret->createOptionalParameter(14, "-start", "start labeling clusters from a value other than 1");
    startOpt->addIntegerParameter(1, "startval", "the value to give the first cluster found");
    
    ret->setHelpText(
        AString("Outputs a cifti file with nonzero integers for all brainordinates within a large enough cluster, and zeros elsewhere.  ") +
        "The integers denote cluster membership (by default, first cluster found will use value 1, second cluster 2, etc).  " +
        "Cluster values are not reused across maps of the output, but instead keep counting up.  " +
        "The input cifti file must have a brain models mapping on the chosen dimension, columns for .dtseries, and either for .dconn.  " +
        "The ROI should have a brain models mapping along columns, exactly matching the mapping of the chosen direction in the input file.  " +
        "Data outside the ROI is ignored."
    );
    return ret;
}

void AlgorithmCiftiFindClusters::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float surfThresh = (float)myParams->getDouble(2);
    float surfSize = (float)myParams->getDouble(3);
    float volThresh = (float)myParams->getDouble(4);
    float volSize = (float)myParams->getDouble(5);
    AString directionName = myParams->getString(6);
    int myDir;
    if (directionName == "ROW")
    {
        myDir = CiftiXML::ALONG_ROW;
    } else if (directionName == "COLUMN") {
        myDir = CiftiXML::ALONG_COLUMN;
    } else {
        throw AlgorithmException("incorrect string for direction, use ROW or COLUMN");
    }
    CiftiFile* myCiftiOut = myParams->getOutputCifti(7);
    bool lessThan = myParams->getOptionalParameter(8)->m_present;
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    MetricFile* myLeftAreas = NULL, *myRightAreas = NULL, *myCerebAreas = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(9);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
        OptionalParameter* leftCorrAreasOpt = leftSurfOpt->getOptionalParameter(2);
        if (leftCorrAreasOpt->m_present)
        {
            myLeftAreas = leftCorrAreasOpt->getMetric(1);
        }
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(10);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
        OptionalParameter* rightCorrAreasOpt = rightSurfOpt->getOptionalParameter(2);
        if (rightCorrAreasOpt->m_present)
        {
            myRightAreas = rightCorrAreasOpt->getMetric(1);
        }
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(11);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
        OptionalParameter* cerebCorrAreasOpt = cerebSurfOpt->getOptionalParameter(2);
        if (cerebCorrAreasOpt->m_present)
        {
            myCerebAreas = cerebCorrAreasOpt->getMetric(1);
        }
    }
    CiftiFile* roiCifti = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(12);
    if (roiOpt->m_present)
    {
        roiCifti = roiOpt->getCifti(1);
    }
    bool mergedVol = myParams->getOptionalParameter(13)->m_present;
    OptionalParameter* startOpt = myParams->getOptionalParameter(14);
    int startVal = 1;
    if (startOpt->m_present)
    {
        startVal = (int)startOpt->getInteger(1);
    }
    OptionalParameter* sizeRatioOpt = myParams->getOptionalParameter(15);
    float surfSizeRatio = -1.0f, volSizeRatio = -1.0f;
    if (sizeRatioOpt->m_present)
    {
        surfSizeRatio = sizeRatioOpt->getDouble(1);
        volSizeRatio = sizeRatioOpt->getDouble(2);
        if (surfSizeRatio <= 0.0f && volSizeRatio <= 0.0f)
        {
            throw AlgorithmException("at least one size ratio must be positive");
        }
    }
    OptionalParameter* distanceOpt = myParams->getOptionalParameter(16);
    float surfDistCutoff = -1.0f, volDistCutoff = -1.0f;
    if (distanceOpt->m_present)
    {
        surfDistCutoff = distanceOpt->getDouble(1);
        volDistCutoff = distanceOpt->getDouble(2);
        if (surfDistCutoff <= 0.0f && volDistCutoff <= 0.0f)
        {
            throw AlgorithmException("at least one distance cutoff must be positive");
        }
    }
    AlgorithmCiftiFindClusters(myProgObj, myCifti, surfThresh, surfSize, volThresh, volSize, myDir, myCiftiOut, lessThan,
                               myLeftSurf, myLeftAreas, myRightSurf, myRightAreas, myCerebSurf, myCerebAreas,
                               roiCifti, mergedVol, startVal, NULL, surfSizeRatio, volSizeRatio, surfDistCutoff, volDistCutoff);
}

AlgorithmCiftiFindClusters::AlgorithmCiftiFindClusters(ProgressObject* myProgObj, const CiftiFile* myCifti,
                                                       const float& surfThresh, const float& surfSize, const float& volThresh, const float& volSize,
                                                       const int& myDir, CiftiFile* myCiftiOut, const bool& lessThan,
                                                       const SurfaceFile* myLeftSurf, const MetricFile* myLeftAreas,
                                                       const SurfaceFile* myRightSurf, const MetricFile* myRightAreas,
                                                       const SurfaceFile* myCerebSurf, const MetricFile* myCerebAreas,
                                                       const CiftiFile* roiCifti, const bool& mergedVol, const int& startVal, int* endVal,
                                                       const float& surfSizeRatio, const float& volSizeRatio, const float& surfDistCutoff, const float& volDistCutoff) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (startVal == 0)
    {
        throw AlgorithmException("0 is not a valid cluster marking start value");
    }
    const CiftiXML& myXML = myCifti->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw AlgorithmException("cifti separate only supported on 2D cifti");
    if (myDir >= myXML.getNumberOfDimensions() || myDir < 0) throw AlgorithmException("direction invalid for input cifti");
    if (myXML.getMappingType(myDir) != CiftiMappingType::BRAIN_MODELS)
    {
        throw AlgorithmException("specified direction does not contain brainordinates");
    }
    const CiftiBrainModelsMap& myBrainMap = myXML.getBrainModelsMap(myDir);
    if (roiCifti != NULL && myBrainMap != *(roiCifti->getCiftiXML().getMap(CiftiXML::ALONG_COLUMN)))
    {
        throw AlgorithmException("along-column mapping of roi cifti does not match the smoothing direction of the input cifti");
    }
    vector<StructureEnum::Enum> surfaceList = myBrainMap.getSurfaceStructureList();
    int markVal = startVal;
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        const SurfaceFile* mySurf = NULL;
        const MetricFile* myAreas = NULL;
        AString surfType;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                myAreas = myLeftAreas;
                surfType = "left";
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                myAreas = myRightAreas;
                surfType = "right";
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                myAreas = myCerebAreas;
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
        if (mySurf->getNumberOfNodes() != myBrainMap.getSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
        if (myAreas != NULL && myAreas->getNumberOfNodes() != mySurf->getNumberOfNodes())
        {
            throw AlgorithmException(surfType + " corrected areas metric has the wrong number of vertices");
        }
    }
    myCiftiOut->setCiftiXML(myXML);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        const SurfaceFile* mySurf = NULL;
        const MetricFile* myAreas = NULL;
        switch (surfaceList[whichStruct])
        {
            case StructureEnum::CORTEX_LEFT:
                mySurf = myLeftSurf;
                myAreas = myLeftAreas;
                break;
            case StructureEnum::CORTEX_RIGHT:
                mySurf = myRightSurf;
                myAreas = myRightAreas;
                break;
            case StructureEnum::CEREBELLUM:
                mySurf = myCerebSurf;
                myAreas = myCerebAreas;
                break;
            default:
                break;
        }
        MetricFile myMetric, myRoi, myMetricOut;
        AlgorithmCiftiSeparate(NULL, myCifti, myDir, surfaceList[whichStruct], &myMetric, &myRoi);
        if (roiCifti != NULL)
        {//due to above testing, we know the structure mask is the same, so just overwrite the ROI from the mask
            AlgorithmCiftiSeparate(NULL, roiCifti, CiftiXML::ALONG_COLUMN, surfaceList[whichStruct], &myRoi);
        }
        AlgorithmMetricFindClusters(NULL, mySurf, &myMetric, surfThresh, surfSize, &myMetricOut, lessThan, &myRoi, myAreas, -1, markVal, &markVal, surfSizeRatio, surfDistCutoff);
        AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, surfaceList[whichStruct], &myMetricOut);
    }
    if (mergedVol)
    {
        if (myBrainMap.hasVolumeData())
        {
            VolumeFile myVol, myRoi, myVolOut;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, &myVol, offset, &myRoi, true);
            if (roiCifti != NULL)
            {//due to above testing, we know the structure mask is the same, so just overwrite the ROI from the mask
                AlgorithmCiftiSeparate(NULL, roiCifti, CiftiXMLOld::ALONG_COLUMN, &myRoi, offset, NULL, true);
            }
            AlgorithmVolumeFindClusters(NULL, &myVol, volThresh, volSize, &myVolOut, lessThan, &myRoi, -1, markVal, &markVal, volSizeRatio, volDistCutoff);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, &myVolOut, true);
        }
    } else {
        vector<StructureEnum::Enum> volumeList = myBrainMap.getVolumeStructureList();
        for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
        {
            VolumeFile myVol, myRoi, myVolOut;
            int64_t offset[3];
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, offset, &myRoi, true);
            if (roiCifti != NULL)
            {//due to above testing, we know the structure mask is the same, so just overwrite the ROI from the mask
                AlgorithmCiftiSeparate(NULL, roiCifti, CiftiXML::ALONG_COLUMN, volumeList[whichStruct], &myRoi, offset, NULL, true);
            }
            AlgorithmVolumeFindClusters(NULL, &myVol, volThresh, volSize, &myVolOut, lessThan, &myRoi, -1, markVal, &markVal, volSizeRatio, volDistCutoff);
            AlgorithmCiftiReplaceStructure(NULL, myCiftiOut, myDir, volumeList[whichStruct], &myVolOut, true);
        }
    }
    if (endVal != NULL) *endVal = markVal;
}

float AlgorithmCiftiFindClusters::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiFindClusters::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
