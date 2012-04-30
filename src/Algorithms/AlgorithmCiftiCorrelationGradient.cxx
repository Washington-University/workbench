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

#include "AlgorithmCiftiCorrelationGradient.h"
#include "AlgorithmException.h"
#include "AlgorithmMetricGradient.h"
#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmVolumeGradient.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"
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
    
    ret->addDoubleParameter(2, "volume-kernel", "the sigma for the gaussian volume smoothing kernel, in mm");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "the output cifti");
    
    OptionalParameter* leftSurfOpt = ret->createOptionalParameter(4, "-left-surface", "specify the left surface to use");
    leftSurfOpt->addSurfaceParameter(1, "surface", "the left surface file");
    
    OptionalParameter* rightSurfOpt = ret->createOptionalParameter(5, "-right-surface", "specify the right surface to use");
    rightSurfOpt->addSurfaceParameter(1, "surface", "the right surface file");
    
    OptionalParameter* cerebSurfaceOpt = ret->createOptionalParameter(6, "-cereb-surface", "specify the cerebellum surface to use");
    cerebSurfaceOpt->addSurfaceParameter(1, "surface", "the cerebellum surface file");
    
    OptionalParameter* presmoothOpt = ret->createOptionalParameter(7, "-surface-presmooth", "smooth on the surface before computing the gradient");
    presmoothOpt->addDoubleParameter(1, "presmooth-kernel", "the sigma for the gaussian surface smoothing kernel, in mm");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(8, "-mem-limit", "restrict memory usage");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes (default unlimited)");
    
    //ret->createOptionalParameter(9, "-local-method", "use the local gradient at each node instead of averaging all gradients");
    
    ret->setHelpText(
        AString("For each structure, compute the correlation of the rows in the structure, and take the gradients of ") +
        "the resulting rows, then average them.  If -local-method is specified, take only the gradient value at the " +
        "node/voxel being correlated with when generating output, and don't compute data that doesn't have an effect on the output."
    );
    return ret;
}

void AlgorithmCiftiCorrelationGradient::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float volKern = (float)myParams->getDouble(2);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(3);
    SurfaceFile* myLeftSurf = NULL, *myRightSurf = NULL, *myCerebSurf = NULL;
    OptionalParameter* leftSurfOpt = myParams->getOptionalParameter(4);
    if (leftSurfOpt->m_present)
    {
        myLeftSurf = leftSurfOpt->getSurface(1);
    }
    OptionalParameter* rightSurfOpt = myParams->getOptionalParameter(5);
    if (rightSurfOpt->m_present)
    {
        myRightSurf = rightSurfOpt->getSurface(1);
    }
    OptionalParameter* cerebSurfOpt = myParams->getOptionalParameter(6);
    if (cerebSurfOpt->m_present)
    {
        myCerebSurf = cerebSurfOpt->getSurface(1);
    }
    float surfKern = -1.0f;
    OptionalParameter* presmoothOpt = myParams->getOptionalParameter(7);
    if (presmoothOpt->m_present)
    {
        surfKern = (float)presmoothOpt->getDouble(1);
    }
    float memLimitGB = -1.0f;
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(8);
    if (memLimitOpt->m_present)
    {
        memLimitGB = (float)memLimitOpt->getDouble(1);
        if (memLimitGB < 0.0f)
        {
            throw AlgorithmException("memory limit cannot be negative");
        }
    }
    //bool localMethod = myParams->getOptionalParameter(9)->m_present;
    AlgorithmCiftiCorrelationGradient(myProgObj, myCifti, volKern, myCiftiOut, myLeftSurf, myRightSurf, myCerebSurf, surfKern, memLimitGB);
}

AlgorithmCiftiCorrelationGradient::AlgorithmCiftiCorrelationGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& volKern,
                                                                     CiftiFile* myCiftiOut, SurfaceFile* myLeftSurf, SurfaceFile* myRightSurf,
                                                                     SurfaceFile* myCerebSurf, const float& surfKern, const float& memLimitGB) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    init(myCifti);
    const CiftiXML& myXML = myCifti->getCiftiXML();
    CiftiXML myNewXML = myXML;
    myNewXML.resetRowsToTimepoints(1.0f, 1);
    myCiftiOut->setCiftiXML(myNewXML);
    vector<StructureEnum::Enum> surfaceList, volumeList;
    myXML.getStructureListsForColumns(surfaceList, volumeList);
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {//sanity check surfaces
        SurfaceFile* mySurf = NULL;
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
        if (mySurf->getNumberOfNodes() != myCifti->getColumnSurfaceNumberOfNodes(surfaceList[whichStruct]))
        {
            throw AlgorithmException(surfType + " surface has the wrong number of nodes");
        }
    }
    for (int whichStruct = 0; whichStruct < (int)surfaceList.size(); ++whichStruct)
    {
        SurfaceFile* mySurf = NULL;
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
        processSurfaceComponent(surfaceList[whichStruct], surfKern, memLimitGB, mySurf);
    }
    for (int whichStruct = 0; whichStruct < (int)volumeList.size(); ++whichStruct)
    {
        processVolumeComponent(volumeList[whichStruct], volKern, memLimitGB);
    }
    myCiftiOut->setColumn(m_outColumn.data(), 0);
}

void AlgorithmCiftiCorrelationGradient::processSurfaceComponent(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf)
{
    const CiftiXML& myXML = m_inputCifti->getCiftiXML();
    vector<CiftiSurfaceMap> myMap;
    myXML.getSurfaceMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    vector<double> accum(mapSize, 0.0);
    int numCacheRows = mapSize;
    bool cacheFullInput = true;
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, mySurf->getNumberOfNodes(), cacheFullInput);
    }
    if (numCacheRows > mapSize) numCacheRows = mapSize;
    if (cacheFullInput)
    {
        if (numCacheRows != mapSize) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    MetricFile myRoi;
    myRoi.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    myRoi.initializeColumn(0);
    for (int i = 0; i < mapSize; ++i)
    {
        myRoi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
        if (cacheFullInput)
        {
            cacheRow(myMap[i].m_ciftiIndex);
        }
    }
    for (int startpos = 0; startpos < mapSize; startpos += numCacheRows)
    {
        int endpos = startpos + numCacheRows;
        if (endpos > mapSize) endpos = mapSize;
        if (!cacheFullInput)
        {
            for (int i = startpos; i < endpos; ++i)
            {
                cacheRow(myMap[i].m_ciftiIndex);
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
        MetricFile outputMetric, *finalMetric;
        if (surfKern > 0.0f)
        {
            AlgorithmMetricSmoothing(NULL, mySurf, &computeMetric, surfKern, &outputMetric, &myRoi);
            AlgorithmMetricGradient(NULL, mySurf, &outputMetric, &computeMetric, NULL, -1.0f, &myRoi);
            finalMetric = &computeMetric;
        } else {
            AlgorithmMetricGradient(NULL, mySurf, &computeMetric, &outputMetric, NULL, -1.0f, &myRoi);
            finalMetric = &outputMetric;
        }
        int numMetricCols = endpos - startpos;
        for (int j = 0; j < numMetricCols; ++j)
        {
            const float* myCol = finalMetric->getValuePointerForColumn(j);
            for (int i = 0; i < mapSize; ++i)
            {
                accum[i] += myCol[myMap[i].m_surfaceNode];
            }
        }
        if (!cacheFullInput)
        {
            clearCache();
        }
    }
    if (cacheFullInput)
    {
        clearCache();
    }
    for (int i = 0; i < mapSize; ++i)
    {
        m_outColumn[myMap[i].m_ciftiIndex] = accum[i] / mapSize;
    }
}

void AlgorithmCiftiCorrelationGradient::processVolumeComponent(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB)
{
    const CiftiXML& myXML = m_inputCifti->getCiftiXML();
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
        numCacheRows = numRowsForMem(memLimitGB, newdims[0] * newdims[1] * newdims[2], cacheFullInput);
    }
    if (numCacheRows > mapSize) numCacheRows = mapSize;
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
    for (int i = 0; i < mapSize; ++i)
    {
        volRoi.setValue(1.0f, myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
        if (cacheFullInput)
        {
            cacheRow(myMap[i].m_ciftiIndex);
        }
    }
    for (int startpos = 0; startpos < mapSize; startpos += numCacheRows)
    {
        int endpos = startpos + numCacheRows;
        if (endpos > mapSize) endpos = mapSize;
        if (!cacheFullInput)
        {
            for (int i = startpos; i < endpos; ++i)
            {
                cacheRow(myMap[i].m_ciftiIndex);
            }
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
        //computeVol.writeFile("debug." + StructureEnum::toName(myStructure) + ".corr.nii");
        AlgorithmVolumeGradient(NULL, &computeVol, volKern, &outputVol, &volRoi);
        //outputVol.writeFile("debug." + StructureEnum::toName(myStructure) + ".grad.nii");
        int numSubvols = endpos - startpos;
        for (int j = 0; j < numSubvols; ++j)
        {
            for (int i = 0; i < mapSize; ++i)
            {
                accum[i] += outputVol.getValue(myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2], j);
            }
        }
        if (!cacheFullInput)
        {
            clearCache();
        }
    }
    if (cacheFullInput)
    {
        clearCache();
    }
    for (int i = 0; i < mapSize; ++i)
    {
        m_outColumn[myMap[i].m_ciftiIndex] = accum[i] / mapSize;
    }
}

/*void AlgorithmCiftiCorrelationGradient::processSurfaceComponentLocal(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf)
{
    const CiftiXML& myXML = m_inputCifti->getCiftiXML();
    vector<CiftiSurfaceMap> myMap;
    myXML.getSurfaceMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    //int numCacheRows = myXML.getNumberOfRows();
    //bool cacheFullInput = true;
    //if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, mySurf->getNumberOfNodes(), cacheFullInput);//TODO: figure out how to deal with caching and parallelism with ROIs
    }//this may be one-off code that gets removed, just ignore memory limit and cache all input rows for now
    MetricFile myRoi;
    myRoi.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    myRoi.initializeColumn(0);
    float kernBox = surfKern * 3.5f;//FIXME: add average edge length, after multiplying by 3
    for (int i = 0; i < mapSize; ++i)
    {
        myRoi.setValue(myMap[i].m_surfaceNode, 0, 1.0f);
        //if (cacheFullInput)
        //{
            cacheRow(myMap[i].m_ciftiIndex);
        //}
    }
    MetricFile nodeRoi, processMetric, outputMetric;
    vector<int32_t> neighbors;
    vector<float> dists;
    nodeRoi.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    processMetric.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
    CaretPointer<GeodesicHelper> myGeoHelp = mySurf->getGeodesicHelper();
    CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
    for (int i = 0; i < mapSize; ++i)
    {
        int myNode = myMap[i].m_surfaceNode;
        nodeRoi.initializeColumn(0);
        if (surfKern > 0.0f)
        {
            myGeoHelp->getNodesToGeoDist(myNode, kernBox, neighbors, dists);
            if (neighbors.size() < 19)//1+6+12, neighbor depth 2
            {
                myTopoHelp->getNodeNeighborsToDepth(myNode, 2, neighbors);
                neighbors.push_back(myNode);
            }
        } else {
            neighbors = myTopoHelp->getNodeNeighbors(myNode);
            neighbors.push_back(myNode);
        }
        int numNeighbors = (int)neighbors.size();
        float myrrs;
        const float* myrow = getRow(myMap[i].m_ciftiIndex, myrrs, true);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int j = 0; j < numNeighbors; ++j)
        {
            if (myRoi.getValue(neighbors[j], 0) > 0.0f)
            {
                nodeRoi.setValue(neighbors[j], 0, 1.0f);
                float secondrrs;
                const float* secondrow = getRow(myXML.getColumnIndexForNode(neighbors[j], myStructure), secondrrs, true);
                float value = correlate(myrow, myrrs, secondrow, secondrrs);
                processMetric.setValue(neighbors[j], 0, value);
            }
        }
        if (surfKern > 0.0f)
        {
            AlgorithmMetricSmoothing(NULL, mySurf, &processMetric, surfKern, &outputMetric, &nodeRoi);
            nodeRoi.initializeColumn(0);
            const vector<int32_t>& tempneighbors = myTopoHelp->getNodeNeighbors(myNode);
            int tempnum = (int)tempneighbors.size();
            for (int j = 0; j < tempnum; ++j)
            {
                nodeRoi.setValue(tempneighbors[j], 0, 1.0f);
            }
            nodeRoi.setValue(myNode, 0, 1.0f);
            AlgorithmMetricGradient(NULL, mySurf, &outputMetric, &processMetric, NULL, -1.0f, &nodeRoi);
            m_outColumn[myMap[i].m_ciftiIndex] = processMetric.getValue(myMap[i].m_surfaceNode, 0);
        } else {
            AlgorithmMetricGradient(NULL, mySurf, &processMetric, &outputMetric, NULL, -1.0f, &nodeRoi);
            m_outColumn[myMap[i].m_ciftiIndex] = outputMetric.getValue(myMap[i].m_surfaceNode, 0);
        }
    }
}

void AlgorithmCiftiCorrelationGradient::processVolumeComponentLocal(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB)
{
    const CiftiXML& myXML = m_inputCifti->getCiftiXML();
    vector<CiftiVolumeMap> myMap;
    myXML.getVolumeStructureMapForColumns(myMap, myStructure);
    int mapSize = (int)myMap.size();
    //int numCacheRows = myXML.getNumberOfRows();
    //bool cacheFullInput = true;
    //if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, mySurf->getNumberOfNodes(), cacheFullInput);//TODO: figure out how to deal with caching and parallelism with ROIs
    }//this may be one-off code that gets removed, just ignore memory limit and cache all input rows for now
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
    int64_t ciftiDims[3];
    vector<vector<float> > ciftiSform;
    myXML.getVolumeDimsAndSForm(ciftiDims, ciftiSform);
    VolumeFile volRoi(newdims, ciftiSform);
    volRoi.setValueAllVoxels(0.0f);
    Vector3D ivec, jvec, kvec;
    ivec[0] = ciftiSform[0][0]; jvec[0] = ciftiSform[0][1]; kvec[0] = ciftiSform[0][2];
    ivec[1] = ciftiSform[1][0]; jvec[1] = ciftiSform[1][1]; kvec[0] = ciftiSform[1][2];
    ivec[2] = ciftiSform[2][0]; jvec[2] = ciftiSform[2][1]; kvec[0] = ciftiSform[2][2];
    float kernBox = volKern * 3.0f;
    for (int i = 0; i < mapSize; ++i)
    {
        volRoi.setValue(1.0f, myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
        //if (cacheFullInput)
        //{
            cacheRow(myMap[i].m_ciftiIndex);
        //}
    }
    VolumeFile voxelRoi(newdims, ciftiSform), processVol(newdims, ciftiSform), outputVol;
    for (int i = 0; i < mapSize; ++i)
    {
        voxelRoi.setValueAllVoxels(0.0f);
        vector<int> indexList;
        for (int j = 0; j < mapSize; ++j)
        {
            if ((ivec * (myMap[i].m_ijk[0] - myMap[j].m_ijk[0]) + jvec * (myMap[i].m_ijk[1] - myMap[j].m_ijk[1]) + kvec * (myMap[i].m_ijk[2] - myMap[j].m_ijk[2])).length() <= kernBox)
            {
                indexList.push_back(j);
                voxelRoi.setValue(1.0f, myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2]);
            }
        }
        int listSize = (int)indexList.size();
        float myrrs;
        const float* myrow = getRow(myMap[i].m_ciftiIndex, myrrs, true);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int j = 0; j < listSize; ++j)
        {
            float secondrrs;
            const float* secondrow = getRow(myMap[indexList[j]].m_ciftiIndex, secondrrs, true);
            processVol.setValue(correlate(myrow, myrrs, secondrow, secondrrs), myMap[j].m_ijk[0] - offset[0], myMap[j].m_ijk[1] - offset[1], myMap[j].m_ijk[2] - offset[2]);
        }
        AlgorithmVolumeGradient(NULL, &processVol, volKern, &outputVol, &voxelRoi);
        m_outColumn[myMap[i].m_ciftiIndex] = outputVol.getValue(myMap[i].m_ijk[0] - offset[0], myMap[i].m_ijk[1] - offset[1], myMap[i].m_ijk[2] - offset[2]);
    }
}//*/

float AlgorithmCiftiCorrelationGradient::correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2)
{
    double r;
    if (row1 == row2)
    {
        r = 1.0;//short circuit for same row
    } else {
        double accum = 0.0;
        for (int i = 0; i < m_numCols; ++i)
        {
            accum += row1[i] * row2[i];//these have already had the row means subtracted out
        }
        r = accum / (rrs1 * rrs2);
    }
    if (r > 1.0) r = 1.0;//don't output anything silly
    if (r < -1.0) r = -1.0;
    return r;
}

void AlgorithmCiftiCorrelationGradient::init(const CiftiFile* input)
{
    m_inputCifti = input;
    m_rowInfo.resize(m_inputCifti->getNumberOfRows());
    m_cacheUsed = 0;
    m_numCols = m_inputCifti->getNumberOfColumns();
    m_outColumn.resize(m_inputCifti->getNumberOfRows());
}

void AlgorithmCiftiCorrelationGradient::cacheRow(const int& ciftiIndex)
{
    CaretAssertVectorIndex(m_rowInfo, ciftiIndex);
    if (m_rowInfo[ciftiIndex].m_cacheIndex != -1) return;//shouldn't happen, but hey
    if (m_cacheUsed >= (int)m_rowCache.size())
    {
        m_rowCache.push_back(CacheRow());
        m_rowCache[m_cacheUsed].m_row.resize(m_numCols);
    }
    m_rowCache[m_cacheUsed].m_ciftiIndex = ciftiIndex;
    float* myPtr = m_rowCache[m_cacheUsed].m_row.data();
    m_inputCifti->getRow(myPtr, ciftiIndex);
    if (!m_rowInfo[ciftiIndex].m_haveCalculated)
    {
        computeRowStats(myPtr, m_rowInfo[ciftiIndex].m_mean, m_rowInfo[ciftiIndex].m_rootResidSqr);
        m_rowInfo[ciftiIndex].m_haveCalculated = true;
    }
    doSubtract(myPtr, m_rowInfo[ciftiIndex].m_mean);
    m_rowInfo[ciftiIndex].m_cacheIndex = m_cacheUsed;
    ++m_cacheUsed;
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
        if (!m_rowInfo[ciftiIndex].m_haveCalculated)
        {
            computeRowStats(ret, m_rowInfo[ciftiIndex].m_mean, m_rowInfo[ciftiIndex].m_rootResidSqr);
            m_rowInfo[ciftiIndex].m_haveCalculated = true;
        }
        doSubtract(ret, m_rowInfo[ciftiIndex].m_mean);
    }
    rootResidSqr = m_rowInfo[ciftiIndex].m_rootResidSqr;
    return ret;
}

void AlgorithmCiftiCorrelationGradient::computeRowStats(const float* row, float& mean, float& rootResidSqr)
{
    double accum = 0.0;//double, for numerical stability
    for (int i = 0; i < m_numCols; ++i)//two pass, for numerical stability
    {
        accum += row[i];
    }
    mean = accum / m_numCols;
    accum = 0.0;
    for (int i = 0; i < m_numCols; ++i)
    {
        float tempf = row[i] - mean;
        accum += tempf * tempf;
    }
    rootResidSqr = sqrt(accum);
}

void AlgorithmCiftiCorrelationGradient::doSubtract(float* row, const float& mean)
{
    for (int i = 0; i < m_numCols; ++i)
    {
        row[i] -= mean;
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
            m_tempRows[i].resize(m_numCols);
        }
    }
    return m_tempRows[threadNum].data();
#else
    if (m_tempRows.size() == 0)
    {
        m_tempRows.resize(1);
        m_tempRows[0].resize(m_numCols);
    }
    return m_tempRows[0].data();
#endif
}

int AlgorithmCiftiCorrelationGradient::numRowsForMem(const float& memLimitGB, const int& numComponentFloats, bool& cacheFullInput)
{
    int64_t numRows = m_inputCifti->getNumberOfRows();
    int64_t inrowBytes = m_numCols * sizeof(float), outrowBytes = numComponentFloats * sizeof(float);
    int64_t targetBytes = (int64_t)(memLimitGB * 1024 * 1024 * 1024);
    if (m_inputCifti->isInMemory()) targetBytes -= numRows * inrowBytes;//count in-memory input against the total too
    targetBytes -= numRows * sizeof(RowInfo);//storage for mean, stdev, and info about caching
    int64_t perRowBytes = inrowBytes + outrowBytes * 2;//cache and memory for processing and output structures
    if (numComponentFloats * inrowBytes < targetBytes * 0.7f)//if caching the entire input file would take less than 70% of remaining allotted memory, do it to reduce IO
    {
        cacheFullInput = true;//precache the entire input file, rather than caching it synchronously with the in-memory output rows
        targetBytes -= numComponentFloats * inrowBytes;//reduce the remaining total by the memory used
        perRowBytes = outrowBytes * 2;//don't need to count input rows against the remaining memory total
    } else {
        cacheFullInput = false;
#ifdef CARET_OMP
        targetBytes -= inrowBytes * omp_get_max_threads();
#else
        targetBytes -= inrowBytes;//1 row in memory that isn't a reference to cache
#endif
    }
    if (perRowBytes == 0) return 1;//protect against integer div by zero
    int ret = targetBytes / perRowBytes;//integer divide rounds down
    if (ret < 1) return 1;//always return at least one
    return ret;
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
