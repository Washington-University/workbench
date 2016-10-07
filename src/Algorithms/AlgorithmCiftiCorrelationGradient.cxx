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

#include "AlgorithmCiftiCorrelationGradient.h"
#include "AlgorithmException.h"
#include "AlgorithmMetricGradient.h"
#include "MetricSmoothingObject.h"
#include "AlgorithmVolumeGradient.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"
#include "dot_wrapper.h"
#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCorrelationGradient::getCommandSwitch()
{
    return "-cifti-correlation-gradient";
}

AString AlgorithmCiftiCorrelationGradient::getShortDescription()
{
    return "CORRELATE CIFTI ROWS AND TAKE GRADIENT";
}

OperationParameters* AlgorithmCiftiCorrelationGradient::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "the input cifti");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(3, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    OptionalParameter* leftCorrAreasOpt = leftSurfOpt->createOptionalParameter(2, "-left-corrected-areas", "vertex areas to use instead of computing them from the left surface");
    leftCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(4, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    OptionalParameter* rightCorrAreasOpt = rightSurfOpt->createOptionalParameter(2, "-right-corrected-areas", "vertex areas to use instead of computing them from the right surface");
    rightCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(5, "-cerebellum-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    OptionalParameter* cerebCorrAreasOpt = cerebSurfaceOpt->createOptionalParameter(2, "-cerebellum-corrected-areas", "vertex areas to use instead of computing them from the cerebellum surface");
    cerebCorrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* presmoothSurfOpt = ret->createOptionalParameter(6, "-surface-presmooth", "smooth on the surface before computing the gradient");
    presmoothSurfOpt->addDoubleParameter(1, "surface-kernel", "the sigma for the gaussian surface smoothing kernel, in mm");
    
    OptionalParameter* presmoothVolOpt = ret->createOptionalParameter(7, "-volume-presmooth", "smooth the volume before computing the gradient");
    presmoothVolOpt->addDoubleParameter(1, "volume-kernel", "the sigma for the gaussian volume smoothing kernel, in mm");
    
    ret->createOptionalParameter(8, "-undo-fisher-z", "apply the inverse fisher small z transform to the input");
    
    ret->createOptionalParameter(12, "-fisher-z", "apply the fisher small z transform to the correlations before taking the gradient");
    
    OptionalParameter* surfaceExcludeOpt = ret->createOptionalParameter(9, "-surface-exclude", "exclude vertices near each seed vertex from computation");
    surfaceExcludeOpt->addDoubleParameter(1, "distance", "geodesic distance from seed vertex for the exclusion zone, in mm");
    
    OptionalParameter* volumeExcludeOpt = ret->createOptionalParameter(10, "-volume-exclude", "exclude voxels near each seed voxel from computation");
    volumeExcludeOpt->addDoubleParameter(1, "distance", "distance from seed voxel for the exclusion zone, in mm");
    
    ret->createOptionalParameter(13, "-covariance", "compute covariance instead of correlation");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(11, "-mem-limit", "restrict memory usage");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("For each structure, compute the correlation of the rows in the structure, and take the gradients of ") +
        "the resulting rows, then average them.  " +
        "Memory limit does not need to be an integer, you may also specify 0 to use as little memory as possible (this may be very slow)."
    );
    return ret;
}

void AlgorithmCiftiCorrelationGradient::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    MetricFile* myLeftAreas = NULL, *myRightAreas = NULL, *myCerebAreas = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(3);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
        OptionalParameter* leftCorrAreasOpt = leftSurfOpt->getOptionalParameter(2);
        if (leftCorrAreasOpt->m_present)
        {
            myLeftAreas = leftCorrAreasOpt->getMetric(1);
        }
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(4);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
        OptionalParameter* rightCorrAreasOpt = rightSurfOpt->getOptionalParameter(2);
        if (rightCorrAreasOpt->m_present)
        {
            myRightAreas = rightCorrAreasOpt->getMetric(1);
        }
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(5);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
        OptionalParameter* cerebCorrAreasOpt = cerebSurfOpt->getOptionalParameter(2);
        if (cerebCorrAreasOpt->m_present)
        {
            myCerebAreas = cerebCorrAreasOpt->getMetric(1);
        }
    }
    float surfKern = -1.0f;
    OptionalParameter* presmoothSurfOpt = myParams->getOptionalParameter(6);
    if (presmoothSurfOpt->m_present)
    {
        surfKern = (float)presmoothSurfOpt->getDouble(1);
    }
    float volKern = -1.0f;
    OptionalParameter* presmoothVolOpt = myParams->getOptionalParameter(7);
    if (presmoothVolOpt->m_present)
    {
        volKern = (float)presmoothVolOpt->getDouble(1);
    }
    bool undoFisherInput = myParams->getOptionalParameter(8)->m_present;
    bool applyFisher = myParams->getOptionalParameter(12)->m_present;
    float surfaceExclude = -1.0f;
    OptionalParameter* surfaceExcludeOpt = myParams->getOptionalParameter(9);
    if (surfaceExcludeOpt->m_present)
    {
        surfaceExclude = (float)surfaceExcludeOpt->getDouble(1);
        if (surfaceExclude < 0.0f)
        {
            throw AlgorithmException("surface exclude distance cannot be negative");
        }
    }
    float volumeExclude = -1.0f;
    OptionalParameter* volumeExcludeOpt = myParams->getOptionalParameter(10);
    if (volumeExcludeOpt->m_present)
    {
        volumeExclude = (float)volumeExcludeOpt->getDouble(1);
        if (volumeExclude < 0.0f)
        {
            throw AlgorithmException("volume exclude distance cannot be negative");
        }
    }
    float memLimitGB = -1.0f;
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(11);
    if (memLimitOpt->m_present)
    {
        memLimitGB = (float)memLimitOpt->getDouble(1);
        if (memLimitGB < 0.0f)
        {
            throw AlgorithmException("memory limit cannot be negative");
        }
    }
    bool covariance = myParams->getOptionalParameter(13)->m_present;
    AlgorithmCiftiCorrelationGradient(myProgObj, myCifti, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, myLeftAreas, myRightAreas, myCerebAreas,
                                      surfKern, volKern, undoFisherInput, applyFisher, surfaceExclude, volumeExclude, covariance, memLimitGB);
}

AlgorithmCiftiCorrelationGradient::AlgorithmCiftiCorrelationGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                                                     SurfaceFile* myLeftSurf, SurfaceFile* myRightSurf, SurfaceFile* myCerebSurf,
                                                                     const MetricFile* myLeftAreas, const MetricFile* myRightAreas, const MetricFile* myCerebAreas,
                                                                     const float& surfKern, const float& volKern, const bool& undoFisherInput, const bool& applyFisher,
                                                                     const float& surfaceExclude, const float& volumeExclude,
                                                                     const bool& covariance,
                                                                     const float& memLimitGB) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    init(myCifti, undoFisherInput, applyFisher, covariance);
    const CiftiXMLOld& myXML = myCifti->getCiftiXMLOld();
    CiftiXMLOld myNewXML = myXML;
    myNewXML.resetDirectionToScalars(CiftiXMLOld::ALONG_ROW, 1);
    myNewXML.setMapNameForIndex(CiftiXMLOld::ALONG_ROW, 0, "gradient");
    myCiftiOut->setCiftiXML(myNewXML);
    vector<StructureEnum::Enum> surfaceList, volumeList;
    myXML.getStructureListsForColumns(surfaceList, volumeList);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        SurfaceFile* mySurf = NULL;
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
        if (mySurf->getNumberOfNodes() != myCifti->getCiftiXML().getBrainModelsMap(CiftiXML::ALONG_COLUMN).getSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of vertices");
        }
        if (myAreas != NULL && myAreas->getNumberOfNodes() != mySurf->getNumberOfNodes())
        {
            throw AlgorithmException(surfType + " corrected vertex areas metric has the wrong number of vertices");
        }
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        SurfaceFile* mySurf = NULL;
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
        if (surfaceExclude > 0.0f)
        {
            processSurfaceComponent(surfaceList[whichStruct], surfKern, surfaceExclude, memLimitGB, mySurf, myAreas);
        } else {
            processSurfaceComponent(surfaceList[whichStruct], surfKern, memLimitGB, mySurf, myAreas);
        }
    }
    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
    {
        if (volumeExclude > 0.0f)
        {
            processVolumeComponent(volumeList[whichStruct], volKern, volumeExclude, memLimitGB);
        } else {
            processVolumeComponent(volumeList[whichStruct], volKern, memLimitGB);
        }
    }
    myCiftiOut->setColumn(m_outColumn.data(), 0);
}

void AlgorithmCiftiCorrelationGradient::processSurfaceComponent(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf, const MetricFile* myAreas)
{
    const CiftiXMLOld& myXML = m_inputCifti->getCiftiXMLOld();
    vector<CiftiSurfaceMap> myMap;
    myXML.getSurfaceMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    vector<double> accum(mapSize, 0.0);
    int numCacheRows = mapSize;
    bool cacheFullInput = true;
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, m_numCols * sizeof(float), (mySurf->getNumberOfNodes() * (sizeof(float) * 8 + 1)) / 8, mapSize, cacheFullInput);
    }
    if (numCacheRows > mapSize)
    {
        cacheFullInput = true;
        numCacheRows = mapSize;
    }
    if (cacheFullInput)
    {
        if (numCacheRows != mapSize) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    const float* areaData = NULL;
    if (myAreas != NULL)
    {
        areaData = myAreas->getValuePointerForColumn(0);
    }
    MetricFile myRoi;
    myRoi.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    myRoi.initializeColumn(0);
    vector<int> rowsToCache;
    for (int i = 0; i < mapSize; ++i)
    {
        myRoi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
        if (cacheFullInput)
        {
            rowsToCache.push_back(myMap[i].m_ciftiIndex);
        }
    }
    if (cacheFullInput)
    {
        cacheRows(rowsToCache);
    }
    CaretPointer<MetricSmoothingObject> mySmooth;
    if (surfKern > 0.0f)
    {
        mySmooth.grabNew(new MetricSmoothingObject(mySurf, surfKern, &myRoi, MetricSmoothingObject::GEO_GAUSS_AREA, areaData));//computes the smoothing weights only once per surface
    }
    for (int startpos = 0; startpos < mapSize; startpos += numCacheRows)
    {
        int endpos = startpos + numCacheRows;
        if (endpos > mapSize) endpos = mapSize;
        if (!cacheFullInput)
        {
            rowsToCache.clear();
            for (int i = startpos; i < endpos; ++i)
            {
                rowsToCache.push_back(myMap[i].m_ciftiIndex);
            }
            cacheRows(rowsToCache);
        }
        int curRow = 0;//because we can't trust the order threads hit the critical section
        MetricFile computeMetric;
        computeMetric.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), endpos - startpos);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < mapSize; ++i)
        {
            float movingRrs;
            const float* movingRow;
            int myrow;
#pragma omp critical
            {//CiftiFile may explode if we request multiple rows concurrently (needs mutexes), but we should force sequential requests anyway
                myrow = curRow;//so, manually force it to read sequentially
                ++curRow;
                movingRow = getRow(myMap[myrow].m_ciftiIndex, movingRrs);
            }
            for (int j = startpos; j < endpos; ++j)
            {
                if (myrow >= startpos && myrow < endpos)
                {
                    if (j >= myrow)
                    {
                        float cacheRrs;
                        const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                        float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                        computeMetric.setValue(myMap[myrow].m_surfaceNode, j - startpos, result);
                        computeMetric.setValue(myMap[j].m_surfaceNode, myrow - startpos, result);
                    }
                } else {
                    float cacheRrs;
                    const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                    float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                    computeMetric.setValue(myMap[myrow].m_surfaceNode, j - startpos, result);
                }
            }
        }
        int numMetricCols = endpos - startpos;
        MetricFile outputMetric, outputMetric2;
        for (int j = 0; j < numMetricCols; ++j)
        {
            const float* myCol;
            if (surfKern > 0.0f)
            {
                mySmooth->smoothColumn(&computeMetric, j, &outputMetric);
                AlgorithmMetricGradient(NULL, mySurf, &outputMetric, &outputMetric2, NULL, -1.0f, &myRoi, false, -1, myAreas);
                myCol = outputMetric2.getValuePointerForColumn(0);
            } else {
                AlgorithmMetricGradient(NULL, mySurf, &computeMetric, &outputMetric, NULL, -1.0f, &myRoi, false, j, myAreas);
                myCol = outputMetric.getValuePointerForColumn(0);
            }
            for (int i = 0; i < mapSize; ++i)
            {
                const float* roiColumn = myRoi.getValuePointerForColumn(0);
                if (roiColumn[myMap[i].m_surfaceNode] > 0.0f)
                {
                    accum[i] += myCol[myMap[i].m_surfaceNode];
                }
            }
        }
    }
    for (int i = 0; i < mapSize; ++i)
    {
        m_outColumn[myMap[i].m_ciftiIndex] = accum[i] / mapSize;
    }
}

void AlgorithmCiftiCorrelationGradient::processSurfaceComponent(StructureEnum::Enum& myStructure, const float& surfKern, const float& surfExclude, const float& memLimitGB, SurfaceFile* mySurf, const MetricFile* myAreas)
{
    const CiftiXMLOld& myXML = m_inputCifti->getCiftiXMLOld();
    vector<CiftiSurfaceMap> myMap;
    myXML.getSurfaceMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    vector<double> accum(mapSize, 0.0);
    vector<int32_t> accumCount(mapSize, 0);
    int numCacheRows = mapSize;
    bool cacheFullInput = true;
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, m_numCols * sizeof(float), (mySurf->getNumberOfNodes() * (sizeof(float) * 8 + 1)) / 8, mapSize, cacheFullInput);
    }
    if (numCacheRows > mapSize)
    {
        cacheFullInput = true;
        numCacheRows = mapSize;
    }
    if (cacheFullInput)
    {
        if (numCacheRows != mapSize) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    const float* areaData = NULL;
    if (myAreas != NULL)
    {
        areaData = myAreas->getValuePointerForColumn(0);
    }
    CaretPointer<GeodesicHelperBase> myGeoBase(new GeodesicHelperBase(mySurf, areaData));//can't really have SurfaceFile cache ones with corrected areas
    MetricFile myRoi;
    myRoi.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    myRoi.initializeColumn(0);
    vector<vector<bool> > roiLookup(numCacheRows);//this gets bit compressed
    vector<bool> origRoi(mySurf->getNumberOfNodes());
    vector<vector<int32_t> > excludeNodes(numCacheRows);
    vector<int> rowsToCache;
    for (int i = 0; i < mapSize; ++i)
    {
        myRoi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
        if (cacheFullInput)
        {
            rowsToCache.push_back(myMap[i].m_ciftiIndex);
        }
    }
    if (cacheFullInput)
    {
        cacheRows(rowsToCache);
    }
    CaretPointer<MetricSmoothingObject> mySmooth;
    if (surfKern > 0.0f)
    {
        mySmooth.grabNew(new MetricSmoothingObject(mySurf, surfKern, &myRoi, MetricSmoothingObject::GEO_GAUSS_AREA, areaData));//computes the smoothing weights only once per surface
    }
    for (int startpos = 0; startpos < mapSize; startpos += numCacheRows)
    {
        int endpos = startpos + numCacheRows;
        if (endpos > mapSize) endpos = mapSize;
        if (!cacheFullInput)
        {
            rowsToCache.clear();
            for (int i = startpos; i < endpos; ++i)
            {
                rowsToCache.push_back(myMap[i].m_ciftiIndex);
            }
            cacheRows(rowsToCache);
        }
        int numSurfNodes = mySurf->getNumberOfNodes();
#pragma omp CARET_PAR
        {
            vector<float> distances;
            CaretPointer<GeodesicHelper> myGeoHelp(new GeodesicHelper(myGeoBase));
#pragma omp CARET_FOR
            for (int i = startpos; i < endpos; ++i)
            {
                vector<int32_t>& excludeRef = excludeNodes[i - startpos];
                myGeoHelp->getNodesToGeoDist(myMap[i].m_surfaceNode, surfExclude, excludeRef, distances);
                vector<bool>& lookupRef = roiLookup[i - startpos];
                lookupRef.resize(numSurfNodes);
                for (int j = 0; j < numSurfNodes; ++j)
                {
                    lookupRef[j] = (myRoi.getValue(j, 0) > 0.0f);
                }
                int numExclude = excludeRef.size();
                for (int j = 0; j < numExclude; ++j)
                {
                    lookupRef[excludeRef[j]] = false;
                }
            }
        }
        int curRow = 0;//because we can't trust the order threads hit the critical section
        MetricFile computeMetric;
        computeMetric.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), endpos - startpos);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < mapSize; ++i)
        {
            float movingRrs;
            const float* movingRow;
            int myrow;
#pragma omp critical
            {//CiftiFile may explode if we request multiple rows concurrently (needs mutexes), but we should force sequential requests anyway
                myrow = curRow;//so, manually force it to read sequentially
                ++curRow;
                movingRow = getRow(myMap[myrow].m_ciftiIndex, movingRrs);
            }
            for (int j = startpos; j < endpos; ++j)
            {
                if (roiLookup[j - startpos][myMap[myrow].m_surfaceNode])
                {
                    if (myrow >= startpos && myrow < endpos)
                    {
                        if (j >= myrow)
                        {
                            float cacheRrs;
                            const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                            float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                            computeMetric.setValue(myMap[myrow].m_surfaceNode, j - startpos, result);
                            computeMetric.setValue(myMap[j].m_surfaceNode, myrow - startpos, result);
                        }
                    } else {
                        float cacheRrs;
                        const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                        float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                        computeMetric.setValue(myMap[myrow].m_surfaceNode, j - startpos, result);
                    }
                }
            }
        }
        int numMetricCols = endpos - startpos;
        MetricFile outputMetric, outputMetric2;
        MetricFile excludeRoi = myRoi;
        for (int j = 0; j < numMetricCols; ++j)
        {
            int numExclude = (int)excludeNodes[j].size();
            const float* myCol;
            for (int k = 0; k < numExclude; ++k)
            {
                excludeRoi.setValue(excludeNodes[j][k], 0, 0.0f);//exclude the nodes near the seed node
            }
            if (surfKern > 0.0f)
            {
                mySmooth->smoothColumn(&computeMetric, j, &outputMetric, &excludeRoi);
                AlgorithmMetricGradient(NULL, mySurf, &outputMetric, &outputMetric2, NULL, -1.0f, &excludeRoi, false, -1, myAreas);
                myCol = outputMetric2.getValuePointerForColumn(0);
            } else {
                AlgorithmMetricGradient(NULL, mySurf, &computeMetric, &outputMetric, NULL, -1.0f, &excludeRoi, false, j, myAreas);
                myCol = outputMetric.getValuePointerForColumn(0);
            }
            for (int i = 0; i < mapSize; ++i)
            {
                const float* roiColumn = excludeRoi.getValuePointerForColumn(0);
                if (roiColumn[myMap[i].m_surfaceNode] > 0.0f)
                {
                    accum[i] += myCol[myMap[i].m_surfaceNode];
                    accumCount[i] += 1;//less dubious looking than ++accumCount[i]
                }
            }
            for (int k = 0; k < numExclude; ++k)
            {
                excludeRoi.setValue(excludeNodes[j][k], 0, myRoi.getValue(excludeNodes[j][k], 0));//and set them back to original roi afterwards, instead of a full reinitialize
            }
        }
    }
    for (int i = 0; i < mapSize; ++i)
    {
        if (accumCount[i] != 0)
        {
            m_outColumn[myMap[i].m_ciftiIndex] = accum[i] / accumCount[i];
        } else {
            m_outColumn[myMap[i].m_ciftiIndex] = 0.0f;
        }
    }
}

void AlgorithmCiftiCorrelationGradient::processVolumeComponent(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB)
{
    const CiftiXMLOld& myXML = m_inputCifti->getCiftiXMLOld();
    vector<CiftiVolumeMap> myMap;
    myXML.getVolumeStructureMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    vector<double> accum(mapSize, 0.0);
    int numCacheRows = mapSize;
    bool cacheFullInput = true;
    vector<int64_t> newdims;
    int64_t offset[3];
    if (mapSize > 0)
    {//make a voxel bounding box to minimize memory usage
        int extrema[6] = { myMap[0].m_ijk[0],
            myMap[0].m_ijk[0],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[2],
            myMap[0].m_ijk[2]
        };
        for (int64_t i = 1; i < mapSize; ++i)
        {
            if (myMap[i].m_ijk[0] < extrema[0]) extrema[0] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[0] > extrema[1]) extrema[1] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[1] < extrema[2]) extrema[2] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[1] > extrema[3]) extrema[3] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[2] < extrema[4]) extrema[4] = myMap[i].m_ijk[2];
            if (myMap[i].m_ijk[2] > extrema[5]) extrema[5] = myMap[i].m_ijk[2];
        }
        newdims.push_back(extrema[1] - extrema[0] + 1);
        newdims.push_back(extrema[3] - extrema[2] + 1);
        newdims.push_back(extrema[5] - extrema[4] + 1);
        offset[0] = extrema[0];
        offset[1] = extrema[2];
        offset[2] = extrema[4];
    } else {
        return;
    }
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, m_numCols * sizeof(float), newdims[0] * newdims[1] * newdims[2] * sizeof(float), mapSize, cacheFullInput);
    }
    if (numCacheRows > mapSize)
    {
        cacheFullInput = true;
        numCacheRows = mapSize;
    }
    if (cacheFullInput)
    {
        if (numCacheRows != mapSize) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    int64_t ciftiDims[3];
    vector<vector<float> > ciftiSform;
    myXML.getVolumeDimsAndSForm(ciftiDims, ciftiSform);
    VolumeFile volRoi(newdims, ciftiSform);
    volRoi.setValueAllVoxels(0.0f);
    vector<int> rowsToCache;
    for (int i = 0; i < mapSize; ++i)
    {
        volRoi.setValue(1.0f, myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
        if (cacheFullInput)
        {
            rowsToCache.push_back(myMap[i].m_ciftiIndex);
        }
    }
    if (cacheFullInput)
    {
        cacheRows(rowsToCache);
    }
    for (int startpos = 0; startpos < mapSize; startpos += numCacheRows)
    {
        int endpos = startpos + numCacheRows;
        if (endpos > mapSize) endpos = mapSize;
        if (!cacheFullInput)
        {
            rowsToCache.clear();
            for (int i = startpos; i < endpos; ++i)
            {
                rowsToCache.push_back(myMap[i].m_ciftiIndex);
            }
            cacheRows(rowsToCache);
        }
        int curRow = 0;//because we can't trust the order threads hit the critical section
        vector<int64_t> computeDims = newdims;
        computeDims.push_back(endpos - startpos);
        VolumeFile computeVol(computeDims, ciftiSform);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < mapSize; ++i)
        {
            float movingRrs;
            const float* movingRow;
            int myrow;
#pragma omp critical
            {//CiftiFile may explode if we request multiple rows concurrently (needs mutexes), but we should force sequential requests anyway
                myrow = curRow;//so, manually force it to read sequentially
                ++curRow;
                movingRow = getRow(myMap[myrow].m_ciftiIndex, movingRrs);
            }
            for (int j = startpos; j < endpos; ++j)
            {
                if (myrow >= startpos && myrow < endpos)
                {
                    if (j >= myrow)
                    {
                        float cacheRrs;
                        const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                        float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                        computeVol.setValue(result, myMap[myrow].m_ijk[0] - offset[0], myMap[myrow].m_ijk[1] - offset[1], myMap[myrow].m_ijk[2] - offset[2], j - startpos);
                        computeVol.setValue(result, myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2], myrow - startpos);
                    }
                } else {
                    float cacheRrs;
                    const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                    float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                    computeVol.setValue(result, myMap[myrow].m_ijk[0] - offset[0], myMap[myrow].m_ijk[1] - offset[1], myMap[myrow].m_ijk[2] - offset[2], j - startpos);
                }
            }
        }
        VolumeFile outputVol;
        int numSubvols = endpos - startpos;
        for (int j = 0; j < numSubvols; ++j)
        {
            AlgorithmVolumeGradient(NULL, &computeVol, &outputVol, volKern, &volRoi, NULL, j);
            for (int i = 0; i < mapSize; ++i)
            {
                accum[i] += outputVol.getValue(myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
            }
        }
    }
    for (int i = 0; i < mapSize; ++i)
    {
        m_outColumn[myMap[i].m_ciftiIndex] = accum[i] / mapSize;
    }
}

void AlgorithmCiftiCorrelationGradient::processVolumeComponent(StructureEnum::Enum& myStructure, const float& volKern, const float& volExclude, const float& memLimitGB)
{
    const CiftiXMLOld& myXML = m_inputCifti->getCiftiXMLOld();
    vector<CiftiVolumeMap> myMap;
    myXML.getVolumeStructureMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    vector<double> accum(mapSize, 0.0);
    vector<int32_t> accumCount(mapSize, 0);
    int numCacheRows = mapSize;
    bool cacheFullInput = true;
    vector<int64_t> newdims;
    int64_t offset[3];
    if (mapSize > 0)
    {//make a voxel bounding box to minimize memory usage
        int extrema[6] = { myMap[0].m_ijk[0],
            myMap[0].m_ijk[0],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[1],
            myMap[0].m_ijk[2],
            myMap[0].m_ijk[2]
        };
        for (int64_t i = 1; i < mapSize; ++i)
        {
            if (myMap[i].m_ijk[0] < extrema[0]) extrema[0] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[0] > extrema[1]) extrema[1] = myMap[i].m_ijk[0];
            if (myMap[i].m_ijk[1] < extrema[2]) extrema[2] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[1] > extrema[3]) extrema[3] = myMap[i].m_ijk[1];
            if (myMap[i].m_ijk[2] < extrema[4]) extrema[4] = myMap[i].m_ijk[2];
            if (myMap[i].m_ijk[2] > extrema[5]) extrema[5] = myMap[i].m_ijk[2];
        }
        newdims.push_back(extrema[1] - extrema[0] + 1);
        newdims.push_back(extrema[3] - extrema[2] + 1);
        newdims.push_back(extrema[5] - extrema[4] + 1);
        offset[0] = extrema[0];
        offset[1] = extrema[2];
        offset[2] = extrema[4];
    } else {
        return;
    }
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, m_numCols * sizeof(float), newdims[0] * newdims[1] * newdims[2] * sizeof(float), mapSize, cacheFullInput);
    }
    if (numCacheRows > mapSize)
    {
        cacheFullInput = true;
        numCacheRows = mapSize;
    }
    if (cacheFullInput)
    {
        if (numCacheRows != mapSize) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    int64_t ciftiDims[3];
    vector<vector<float> > ciftiSform;
    myXML.getVolumeDimsAndSForm(ciftiDims, ciftiSform);
    VolumeFile volRoi(newdims, ciftiSform);
    volRoi.setValueAllVoxels(0.0f);
    vector<int> rowsToCache;
    for (int i = 0; i < mapSize; ++i)
    {
        volRoi.setValue(1.0f, myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
        if (cacheFullInput)
        {
            rowsToCache.push_back(myMap[i].m_ciftiIndex);
        }
    }
    if (cacheFullInput)
    {
        cacheRows(rowsToCache);
    }
    for (int startpos = 0; startpos < mapSize; startpos += numCacheRows)
    {
        int endpos = startpos + numCacheRows;
        if (endpos > mapSize) endpos = mapSize;
        if (!cacheFullInput)
        {
            rowsToCache.clear();
            for (int i = startpos; i < endpos; ++i)
            {
                rowsToCache.push_back(myMap[i].m_ciftiIndex);
            }
            cacheRows(rowsToCache);
        }
        int curRow = 0;//because we can't trust the order threads hit the critical section
        vector<int64_t> computeDims = newdims;
        computeDims.push_back(endpos - startpos);
        VolumeFile computeVol(computeDims, ciftiSform);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < mapSize; ++i)
        {
            float movingRrs;
            const float* movingRow;
            int myrow;
#pragma omp critical
            {//CiftiFile may explode if we request multiple rows concurrently (needs mutexes), but we should force sequential requests anyway
                myrow = curRow;//so, manually force it to read sequentially
                ++curRow;
                movingRow = getRow(myMap[myrow].m_ciftiIndex, movingRrs);
            }
            Vector3D movingLoc;
            volRoi.indexToSpace(myMap[myrow].m_ijk, movingLoc);//NOTE: this is outside the cropped volume, but matches the real location in the full volume, because we didn't fix the center
            for (int j = startpos; j < endpos; ++j)
            {
                Vector3D seedLoc;
                volRoi.indexToSpace(myMap[j].m_ijk, seedLoc);//ditto
                if ((movingLoc - seedLoc).length() > volExclude)//don't correlate if closer than the exclude range
                {
                    if (myrow >= startpos && myrow < endpos)
                    {
                        if (j >= myrow)
                        {
                            float cacheRrs;
                            const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                            float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                            computeVol.setValue(result, myMap[myrow].m_ijk[0] - offset[0], myMap[myrow].m_ijk[1] - offset[1], myMap[myrow].m_ijk[2] - offset[2], j - startpos);
                            computeVol.setValue(result, myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2], myrow - startpos);
                        }
                    } else {
                        float cacheRrs;
                        const float* cacheRow = getRow(myMap[j].m_ciftiIndex, cacheRrs, true);
                        float result = correlate(movingRow, movingRrs, cacheRow, cacheRrs);
                        computeVol.setValue(result, myMap[myrow].m_ijk[0] - offset[0], myMap[myrow].m_ijk[1] - offset[1], myMap[myrow].m_ijk[2] - offset[2], j - startpos);
                    }
                }
            }
        }
        VolumeFile outputVol, excludeRoi(newdims, ciftiSform);
        excludeRoi.setFrame(volRoi.getFrame());
        int numSubvols = endpos - startpos;
        for (int j = 0; j < numSubvols; ++j)
        {
            Vector3D seedLoc;
            volRoi.indexToSpace(myMap[j + startpos].m_ijk, seedLoc);
            for (int i = 0; i < mapSize; ++i)
            {
                Vector3D otherLoc;
                volRoi.indexToSpace(myMap[i].m_ijk, otherLoc);
                if ((otherLoc - seedLoc).length() <= volExclude)
                {
                    excludeRoi.setValue(0.0f, myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
                }
            }
            AlgorithmVolumeGradient(NULL, &computeVol, &outputVol, volKern, &excludeRoi, NULL, j);
            for (int i = 0; i < mapSize; ++i)
            {
                if (excludeRoi.getValue(myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]) > 0.0f)
                {
                    float val = outputVol.getValue(myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
                    accum[i] += val;
                    accumCount[i] += 1;
                } else {
                    excludeRoi.setValue(1.0f, myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);//reset the ROI
                }
            }
        }
    }
    for (int i = 0; i < mapSize; ++i)
    {
        if (accumCount[i] != 0)
        {
            m_outColumn[myMap[i].m_ciftiIndex] = accum[i] / accumCount[i];
        } else {
            m_outColumn[myMap[i].m_ciftiIndex] = 0.0f;
        }
    }
}

float AlgorithmCiftiCorrelationGradient::correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2)
{
    double r;
    if (row1 == row2 && !m_covariance)
    {
        r = 1.0;//short circuit for same row
    } else {
        double accum = sddot(row1, row2, m_numCols);//these have already had the row means subtracted out
        if (m_covariance)
        {
            r = accum / m_numCols;
        } else {
            r = accum / (rrs1 * rrs2);
        }
    }
    if (!m_covariance)
    {
        if (m_applyFisher)
        {
            if (r > 0.999999) r = 0.999999;//prevent inf
            if (r < -0.999999) r = -0.999999;//prevent -inf
            r = 0.5 * log((1 + r) / (1 - r));
        } else {
            if (r > 1.0) r = 1.0;//don't output anything silly
            if (r < -1.0) r = -1.0;
        }
    }
    return r;
}

void AlgorithmCiftiCorrelationGradient::init(const CiftiFile* input, const bool& undoFisherInput, const bool& applyFisher,
                                             const bool& covariance)
{
    if (input->getCiftiXML().getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("input cifti file must have brain models mapping along column");
    if (covariance)
    {
        if (applyFisher) throw AlgorithmException("cannot apply fisher z transformation to covariance");
    }
    m_undoFisherInput = undoFisherInput;
    m_applyFisher = applyFisher;
    m_covariance = covariance;
    m_inputCifti = input;
    m_rowInfo.resize(m_inputCifti->getNumberOfRows());
    m_cacheUsed = 0;
    m_numCols = m_inputCifti->getNumberOfColumns();
    m_outColumn.resize(m_inputCifti->getNumberOfRows());
}

void AlgorithmCiftiCorrelationGradient::cacheRows(const vector<int>& ciftiIndices)
{
    clearCache();//clear first, to be sure we never keep a cache around too long
    int curIndex = 0, numIndices = (int)ciftiIndices.size();//manually in-order
    m_rowCache.reserve(m_cacheUsed + numIndices);//so that pointers to members don't change
#pragma omp CARET_PAR
    {
        int myIndex;
        float* myPtr;
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < numIndices; ++i)
        {
            myPtr = NULL;
#pragma omp critical
            {
                myIndex = curIndex;
                ++curIndex;
                CaretAssertVectorIndex(m_rowInfo, ciftiIndices[myIndex]);
                if (m_rowInfo[ciftiIndices[myIndex]].m_cacheIndex == -1)
                {
                    if (m_cacheUsed >= (int)m_rowCache.size())
                    {
                        m_rowCache.push_back(CacheRow());
                        m_rowCache[m_cacheUsed].m_row.resize(m_numCols);
                    }
                    m_rowCache[m_cacheUsed].m_ciftiIndex = ciftiIndices[myIndex];
                    myPtr = m_rowCache[m_cacheUsed].m_row.data();
                    m_inputCifti->getRow(myPtr, ciftiIndices[myIndex]);
                    m_rowInfo[ciftiIndices[myIndex]].m_cacheIndex = m_cacheUsed;
                    ++m_cacheUsed;
                }
            }//end critical, now compute while the next thread reads
            if (myPtr != NULL)
            {
                adjustRow(myPtr, ciftiIndices[myIndex]);
            }
        }
    }
}

void AlgorithmCiftiCorrelationGradient::clearCache()
{
    for (int i = 0; i < m_cacheUsed; ++i)
    {
        m_rowInfo[m_rowCache[i].m_ciftiIndex].m_cacheIndex = -1;
    }
    m_cacheUsed = 0;
}

const float* AlgorithmCiftiCorrelationGradient::getRow(const int& ciftiIndex, float& rootResidSqr, const bool& mustBeCached)
{
    float* ret;
    CaretAssertVectorIndex(m_rowInfo, ciftiIndex);
    if (m_rowInfo[ciftiIndex].m_cacheIndex != -1)
    {
        ret = m_rowCache[m_rowInfo[ciftiIndex].m_cacheIndex].m_row.data();
    } else {
        CaretAssert(!mustBeCached);
        if (mustBeCached)//largely so it doesn't give warning about unused when compiled in release
        {
            throw AlgorithmException("something very bad happened, notify the developers");
        }
        ret = getTempRow();
        m_inputCifti->getRow(ret, ciftiIndex);
        adjustRow(ret, ciftiIndex);
    }
    rootResidSqr = m_rowInfo[ciftiIndex].m_rootResidSqr;
    return ret;
}

void AlgorithmCiftiCorrelationGradient::adjustRow(float* rowOut, const int& ciftiIndex)
{
    if (m_undoFisherInput)
    {
        for (int i = 0; i < m_numCols; ++i)
        {
            double temp = exp(2 * rowOut[i]);
            rowOut[i] = (float)((temp - 1)/(temp + 1));
        }
    }
    if (!m_rowInfo[ciftiIndex].m_haveCalculated)
    {
        double accum = 0.0;//double, for numerical stability
        for (int i = 0; i < m_numCols; ++i)//two pass, for numerical stability
        {
            accum += rowOut[i];
        }
        float mean = accum / m_numCols;
        float rootResidSqr = 0.0f;//not used in covariance
        if (!m_covariance)
        {
            accum = 0.0;
            for (int i = 0; i < m_numCols; ++i)
            {
                float tempf = rowOut[i] - mean;
                accum += tempf * tempf;
            }
            rootResidSqr = sqrt(accum);
        }
        m_rowInfo[ciftiIndex].m_mean = mean;
        m_rowInfo[ciftiIndex].m_rootResidSqr = rootResidSqr;
        m_rowInfo[ciftiIndex].m_haveCalculated = true;
    }
    float mean = m_rowInfo[ciftiIndex].m_mean;
    for (int i = 0; i < m_numCols; ++i)
    {
        rowOut[i] -= mean;
    }
}

float* AlgorithmCiftiCorrelationGradient::getTempRow()
{
#ifdef CARET_OMP
    int oldsize = (int)m_tempRows.size();
    int threadNum = omp_get_thread_num();
    if (threadNum >= oldsize)
    {
        m_tempRows.resize(threadNum + 1);
        for (int i = oldsize; i <= threadNum; ++i)
        {
            m_tempRows[i] = CaretArray<float>(m_numCols);
        }
    }
    return m_tempRows[threadNum].getArray();
#else
    if (m_tempRows.size() == 0)
    {
        m_tempRows.resize(1);
        m_tempRows[0] = CaretArray<float>(m_numCols);
    }
    return m_tempRows[0].getArray();
#endif
}

int AlgorithmCiftiCorrelationGradient::numRowsForMem(const float& memLimitGB, const int64_t& inrowBytes, const int64_t& outrowBytes, const int& numRows, bool& cacheFullInput)
{
    int64_t targetBytes = (int64_t)(memLimitGB * 1024 * 1024 * 1024);
    if (m_inputCifti->isInMemory()) targetBytes -= numRows * inrowBytes;//count in-memory input against the total too
    targetBytes -= numRows * sizeof(RowInfo) + 2 * outrowBytes;//storage for mean, stdev, and info about caching, output structures
    if (targetBytes < 1)
    {
        cacheFullInput = false;//the most memory conservation possible, though it will take a LOT of time and do a LOT of IO
        return 1;
    }
    if (inrowBytes * numRows < targetBytes)//if we can cache the full input, compute the number of passes needed and compare
    {
        int64_t div = max(outrowBytes, (int64_t)1);//make sure it is never zero or negative
        int64_t numRowsFull = (targetBytes - inrowBytes * numRows) / div;
        if (numRowsFull < 1) numRowsFull = 1;
        int64_t fullPasses = numRows / numRowsFull;
        int64_t fullCorrSkip = (fullPasses * numRowsFull * (numRowsFull - 1) + (numRows - fullPasses * numRowsFull) * (numRows - fullPasses * numRowsFull - 1)) / 2;
#ifdef CARET_OMP
        targetBytes -= inrowBytes * omp_get_max_threads();
#else
        targetBytes -= inrowBytes;//1 row in memory that isn't a reference to cache
#endif
        int64_t numPassesPartial = ((outrowBytes + inrowBytes) * numRows + targetBytes - 1) / targetBytes;//break the partial cached passes up equally, to use less memory, and so we don't get an anemic pass at the end
        if (numPassesPartial < 1)
        {
            numPassesPartial = 1;
            CaretLogWarning("memory usage calculation found zero/negative pass solution, report to developers (it may use a lot of memory this run)");
        }
        int64_t numRowsPartial = (numRows + numPassesPartial - 1) / numPassesPartial;
        fullPasses = numPassesPartial - 1;
        int64_t partialCorrSkip = (fullPasses * numRowsPartial * (numRowsPartial - 1) + (numRows - fullPasses * numRowsPartial) * (numRows - fullPasses * numRowsPartial - 1)) / 2;
        int ret;
        if (partialCorrSkip > fullCorrSkip * 1.05f)//prefer full caching slightly - include a bias factor in options?
        {//assume IO and row adjustment (unfisher, subtract mean) won't be the limiting factor, since IO should be balanced during correlation due to evenly sized passes when not fully cached
            cacheFullInput = false;
            ret = numRowsPartial;
        } else {
            cacheFullInput = true;
            ret = numRowsFull;
        }
        if (ret < 1) ret = 1;//sanitize, just in case
        if (ret > numRows) ret = numRows;
        return ret;
    } else {//if we can't cache the whole thing, split passes evenly
        cacheFullInput = false;
        int64_t div = max((int64_t)1, (outrowBytes + inrowBytes) * numRows);
#ifdef CARET_OMP
        targetBytes -= inrowBytes * omp_get_max_threads();
#else
        targetBytes -= inrowBytes;//1 row in memory that isn't a reference to cache
#endif
        int64_t numPassesPartial = (targetBytes + div - 1) / targetBytes;
        int ret = (numRows + numPassesPartial - 1) / numPassesPartial;
        if (ret < 1) ret = 1;//sanitize, just in case
        if (ret > numRows) ret = numRows;
        return ret;
    }
}

float AlgorithmCiftiCorrelationGradient::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCorrelationGradient::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
