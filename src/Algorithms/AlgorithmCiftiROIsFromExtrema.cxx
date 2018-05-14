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

#include "AlgorithmCiftiROIsFromExtrema.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmMetricROIsFromExtrema.h"
#include "AlgorithmVolumeROIsFromExtrema.h"
#include "CaretAssert.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "OverlapLogicEnum.h"
#include "SurfaceFile.h"

#include <limits>
#include <vector>

using namespace caret;
using namespace std;

AString AlgorithmCiftiROIsFromExtrema::getCommandSwitch()
{
    return "-cifti-rois-from-extrema";
}

AString AlgorithmCiftiROIsFromExtrema::getShortDescription()
{
    return "CREATE CIFTI ROI MAPS FROM EXTREMA MAPS";
}

OperationParameters* AlgorithmCiftiROIsFromExtrema::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    
    ret->addDoubleParameter(2, "surf-limit", "geodesic distance limit from vertex, in mm");
    
    ret->addDoubleParameter(3, "vol-limit", "euclidean distance limit from voxel center, in mm");

    ret->addStringParameter(4, "direction", "which dimension an extrema map is along, ROW or COLUMN");

    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(6, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(7, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(8, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    OptionalParameter* gaussOpt = ret->createOptionalParameter(9, "-gaussian", "generate gaussian kernels instead of flat ROIs");
    gaussOpt->addDoubleParameter(1, "surf-sigma", "the sigma for the surface gaussian kernel, in mm");
    gaussOpt->addDoubleParameter(2, "vol-sigma", "the sigma for the volume gaussian kernel, in mm");
    
    OptionalParameter* overlapOpt = ret->createOptionalParameter(10, "-overlap-logic", "how to handle overlapping ROIs, default ALLOW");
    overlapOpt->addStringParameter(1, "method", "the method of resolving overlaps");
    
    ret->createOptionalParameter(11, "-merged-volume", "treat volume components as if they were a single component");
    
    ret->setHelpText(
        AString("For each nonzero value in each map, make a map with an ROI around that location.  ") +
        "If the -gaussian option is specified, then normalized gaussian kernels are output instead of ROIs.  " +
        "The <method> argument to -overlap-logic must be one of ALLOW, CLOSEST, or EXCLUDE.  " +
        "ALLOW is the default, and means that ROIs are treated independently and may overlap.  " +
        "CLOSEST means that ROIs may not overlap, and that no ROI contains vertices that are closer to a different seed vertex.  " +
        "EXCLUDE means that ROIs may not overlap, and that any vertex within range of more than one ROI does not belong to any ROI."
    );
    return ret;
}

void AlgorithmCiftiROIsFromExtrema::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float surfLimit = (float)myParams->getDouble(2);
    float volLimit = (float)myParams->getDouble(3);
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
    float surfSigma = -1.0f, volSigma = -1.0f;
    OptionalParameter* gaussOpt = myParams->getOptionalParameter(9);
    if (gaussOpt->m_present)
    {
        surfSigma = (float)gaussOpt->getDouble(1);
        volSigma = (float)gaussOpt->getDouble(2);
        if (surfSigma <= 0.0f || volSigma <= 0.0f)
        {
            throw AlgorithmException("sigma values must be positive");
        }
    }
    OverlapLogicEnum::Enum myLogic = OverlapLogicEnum::ALLOW;
    OptionalParameter* overlapOpt = myParams->getOptionalParameter(10);
    if (overlapOpt->m_present)
    {
        bool ok = false;
        myLogic = OverlapLogicEnum::fromName(overlapOpt->getString(1), &ok);
        if (!ok)
        {
            throw AlgorithmException("unrecognized overlap method");
        }
    }
    bool mergedVolume = myParams->getOptionalParameter(11)->m_present;
    AlgorithmCiftiROIsFromExtrema(myProgObj, myCifti, surfLimit, volLimit, myDir, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, surfSigma, volSigma, myLogic, mergedVolume);
}

AlgorithmCiftiROIsFromExtrema::AlgorithmCiftiROIsFromExtrema(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfLimit, const float& volLimit, const int& myDir,
                                                             CiftiFile* myCiftiOut, const SurfaceFile* myLeftSurf, const SurfaceFile* myRightSurf, const SurfaceFile* myCerebSurf,
                                                             const float& surfSigma, const float& volSigma, const OverlapLogicEnum::Enum& myLogic, const bool& mergedVolume) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiXMLOld myXML = myCifti->getCiftiXMLOld();
    vector<StructureEnum::Enum> surfaceList, volumeList;
    if (myDir != CiftiXMLOld::ALONG_ROW && myDir != CiftiXMLOld::ALONG_COLUMN) throw AlgorithmException("direction not supported by cifti rois from extrema");
    if (!myXML.getStructureLists(myDir, surfaceList, volumeList))
    {
        throw AlgorithmException("columns do not contain brainordinates");
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
        if (mySurf->getNumberOfNodes() != myXML.getColumnSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
    }
    vector<CaretPointer<MetricFile> > surfROIs;
    vector<CaretPointer<VolumeFile> > volROIs;
    vector<int64_t> volOffsets;
    int64_t mapCount = 0;
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
        surfROIs.push_back(CaretPointer<MetricFile>(new MetricFile()));
        AlgorithmMetricROIsFromExtrema(NULL, mySurf, &myMetric, surfLimit, surfROIs.back(), surfSigma, &myRoi, myLogic);
        mapCount += surfROIs.back()->getNumberOfMaps();
    }
    if (mergedVolume)
    {
        if (myCifti->getCiftiXMLOld().hasVolumeData(myDir))
        {
            VolumeFile myVol, myRoi, myVolOut;
            volOffsets.resize(3);
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, &myVol, volOffsets.data(), &myRoi, true);//without structure, it returns a combined volume
            volROIs.push_back(CaretPointer<VolumeFile>(new VolumeFile()));
            AlgorithmVolumeROIsFromExtrema(NULL, &myVol, volLimit, volROIs.back(), volSigma, &myRoi, myLogic);
            vector<int64_t> tempDims;
            volROIs.back()->getDimensions(tempDims);
            mapCount += tempDims[3];//because getNumberOfMaps only returns int32
        }
    } else {
        volOffsets.resize(volumeList.size() * 3);
        for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
        {
            VolumeFile myVol, myRoi, myVolOut;
            AlgorithmCiftiSeparate(NULL, myCifti, myDir, volumeList[whichStruct], &myVol, volOffsets.data() + 3 * whichStruct, &myRoi, true);
            volROIs.push_back(CaretPointer<VolumeFile>(new VolumeFile()));
            AlgorithmVolumeROIsFromExtrema(NULL, &myVol, volLimit, volROIs.back(), volSigma, &myRoi, myLogic);
            vector<int64_t> tempDims;
            volROIs.back()->getDimensions(tempDims);
            mapCount += tempDims[3];//because getNumberOfMaps only returns int32
        }
    }
    if (mapCount == 0) throw AlgorithmException("no nonzero values in input cifti file");
    if (mapCount > numeric_limits<int>::max()) throw AlgorithmException("result has too many ROIs");
    myXML.resetDirectionToScalars(1 - myDir, mapCount);
    myCiftiOut->setCiftiXML(myXML);
    if (myDir == CiftiXMLOld::ALONG_ROW)
    {
        int64_t curRow = 0;
        for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
        {
            vector<float> scratchrow(myXML.getNumberOfColumns(), 0.0f);//so that it is initially zeroed for each structure
            vector<CiftiSurfaceMap> myMap;
            myXML.getSurfaceMap(myDir, myMap, surfaceList[whichStruct]);
            for (int i = 0; i < surfROIs[whichStruct]->getNumberOfColumns(); ++i)
            {
                const float* data = surfROIs[whichStruct]->getValuePointerForColumn(i);
                for (int j = 0; j < (int)myMap.size(); ++j)
                {
                    scratchrow[myMap[j].m_ciftiIndex] = data[myMap[j].m_surfaceNode];
                }
                myCiftiOut->setRow(scratchrow.data(), curRow);
                ++curRow;
            }
        }
        if (mergedVolume)
        {
            if (myXML.hasVolumeData(myDir))
            {
                vector<float> scratchrow(myXML.getNumberOfColumns(), 0.0f);
                vector<CiftiVolumeMap> myMap;
                myXML.getVolumeMap(myDir, myMap);
                vector<int64_t> tempDims;
                volROIs[0]->getDimensions(tempDims);
                for (int64_t b = 0; b < tempDims[3]; ++b)
                {
                    for (int64_t j = 0; j < (int64_t)myMap.size(); ++j)
                    {
                        int64_t myijk[3] = { myMap[j].m_ijk[0] - volOffsets[0], myMap[j].m_ijk[1] - volOffsets[1], myMap[j].m_ijk[2] - volOffsets[2] };
                        scratchrow[myMap[j].m_ciftiIndex] = volROIs[0]->getValue(myijk, b);
                    }
                    myCiftiOut->setRow(scratchrow.data(), curRow);
                    ++curRow;
                }
            }
        } else {
            for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
            {
                vector<float> scratchrow(myXML.getNumberOfColumns(), 0.0f);
                vector<CiftiVolumeMap> myMap;
                myXML.getVolumeStructureMapForRows(myMap, volumeList[whichStruct]);
                vector<int64_t> tempDims;
                volROIs[whichStruct]->getDimensions(tempDims);
                for (int64_t b = 0; b < tempDims[3]; ++b)
                {
                    for (int64_t j = 0; j < (int64_t)myMap.size(); ++j)
                    {
                        int64_t myijk[3] = { myMap[j].m_ijk[0] - volOffsets[whichStruct * 3], myMap[j].m_ijk[1] - volOffsets[whichStruct * 3 + 1], myMap[j].m_ijk[2] - volOffsets[whichStruct * 3 + 2] };
                        scratchrow[myMap[j].m_ciftiIndex] = volROIs[whichStruct]->getValue(myijk, b);
                    }
                    myCiftiOut->setRow(scratchrow.data(), curRow);
                    ++curRow;
                }
            }
        }
    } else {
        vector<int64_t> surfaceStart, volumeStart;
        int64_t mystart = 0;
        for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
        {
            surfaceStart.push_back(mystart);
            mystart += surfROIs[whichStruct]->getNumberOfMaps();
        }
        if (mergedVolume)
        {
            volumeStart.push_back(mystart);
        } else {
            for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
            {
                volumeStart.push_back(mystart);
                vector<int64_t> tempDims;
                volROIs[whichStruct]->getDimensions(tempDims);
                mystart += tempDims[3];
            }
        }
        int64_t curRow = 0;
        while (curRow < myXML.getNumberOfRows())
        {
            bool found = false;
            for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
            {
                vector<CiftiSurfaceMap> myMap;
                myXML.getSurfaceMap(myDir, myMap, surfaceList[whichStruct]);
                if (myMap.size() > 0 && myMap[0].m_ciftiIndex == curRow)//NOTE: cifti indexes in structure maps are always linear ascending
                {
                    vector<float> scratchrow(myXML.getNumberOfColumns(), 0.0f);
                    for (int64_t j = 0; j < (int64_t)myMap.size(); ++j)
                    {
                        CaretAssert(curRow == myMap[j].m_ciftiIndex);
                        for (int k = 0; k < surfROIs[whichStruct]->getNumberOfColumns(); ++k)
                        {
                            scratchrow[surfaceStart[whichStruct] + k] = surfROIs[whichStruct]->getValue(myMap[j].m_surfaceNode, k);
                        }
                        myCiftiOut->setRow(scratchrow.data(), curRow);
                        ++curRow;
                    }
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                if (mergedVolume)
                {
                    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)//NOTE: merged map may not be in ascending order, or contiguous, so don't use it
                    {
                        vector<CiftiVolumeMap> myMap;
                        myXML.getVolumeStructureMapForColumns(myMap, volumeList[whichStruct]);
                        if (myMap.size() > 0 && myMap[0].m_ciftiIndex == curRow)
                        {
                            vector<int64_t> tempDims;
                            volROIs[0]->getDimensions(tempDims);
                            vector<float> scratchrow(myXML.getNumberOfColumns(), 0.0f);
                            for (int64_t j = 0; j < (int64_t)myMap.size(); ++j)
                            {
                                CaretAssert(curRow == myMap[j].m_ciftiIndex);
                                int64_t myijk[3] = { myMap[j].m_ijk[0] - volOffsets[0], myMap[j].m_ijk[1] - volOffsets[1], myMap[j].m_ijk[2] - volOffsets[2] };
                                for (int64_t b = 0; b < tempDims[3]; ++b)
                                {
                                    scratchrow[volumeStart[0] + b] = volROIs[0]->getValue(myijk, b);
                                }
                                myCiftiOut->setRow(scratchrow.data(), curRow);
                                ++curRow;
                            }
                            found = true;
                            break;
                        }
                    }
                } else {
                    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
                    {
                        vector<CiftiVolumeMap> myMap;
                        myXML.getVolumeStructureMapForColumns(myMap, volumeList[whichStruct]);
                        vector<int64_t> tempDims;
                        volROIs[whichStruct]->getDimensions(tempDims);
                        if (myMap.size() > 0 && myMap[0].m_ciftiIndex == curRow)
                        {
                            vector<float> scratchrow(myXML.getNumberOfColumns(), 0.0f);
                            for (int64_t j = 0; j < (int64_t)myMap.size(); ++j)
                            {
                                CaretAssert(curRow == myMap[j].m_ciftiIndex);
                                int64_t myijk[3] = { myMap[j].m_ijk[0] - volOffsets[whichStruct * 3], myMap[j].m_ijk[1] - volOffsets[whichStruct * 3 + 1], myMap[j].m_ijk[2] - volOffsets[whichStruct * 3 + 2] };
                                for (int64_t b = 0; b < tempDims[3]; ++b)
                                {
                                    scratchrow[volumeStart[whichStruct] + b] = volROIs[whichStruct]->getValue(myijk, b);
                                }
                                myCiftiOut->setRow(scratchrow.data(), curRow);
                                ++curRow;
                            }
                            found = true;
                            break;
                        }
                    }
                }
            }
            CaretAssert(found);
        }
    }
}

float AlgorithmCiftiROIsFromExtrema::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiROIsFromExtrema::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
