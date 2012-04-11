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

#include "AlgorithmCiftiCorrelation.h"
#include "AlgorithmException.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "CaretLogger.h"
#include <utility>
#include <algorithm>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCorrelation::getCommandSwitch()
{
    return "-cifti-correlation";
}

AString AlgorithmCiftiCorrelation::getShortDescription()
{
    return "GENERATE CORRELATION OF ROWS IN A CIFTI FILE";
}

OperationParameters* AlgorithmCiftiCorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti", "input cifti file");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "output cifti file");
    
    OptionalParameter* roiOverrideOpt = ret->createOptionalParameter(3, "-roi-override", "perform correlation from a subset of rows to all rows");
    OptionalParameter* leftRoiOpt = roiOverrideOpt->createOptionalParameter(1, "-left-roi", "use an roi for left hempsphere");
    leftRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    OptionalParameter* rightRoiOpt = roiOverrideOpt->createOptionalParameter(2, "-right-roi", "use an roi for right hempsphere");
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    OptionalParameter* cerebRoiOpt = roiOverrideOpt->createOptionalParameter(3, "-cereb-roi", "use an roi for cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the left roi as a metric file");
    OptionalParameter* volRoiOpt = roiOverrideOpt->createOptionalParameter(4, "-vol-roi", "use an roi for volume");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the roi volume file");
    
    ret->createOptionalParameter(4, "-fisher-z", "apply fisher z transform to correlation");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(5, "-mem-limit", "restrict memory usage");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes (default unlimited)");
    
    ret->setHelpText(
        AString("For each row (or each row inside an roi if -roi-override is specified), correlate to all other rows.  ") +
        "Restricting the memory usage will make it calculate in chunks, causing multiple scans through the file, " +
        "resulting in longer runtime due to more IO bandwidth.  " +
        "Memory limit does not need to be an integer, you may also specify 0 to calculate a single output row at a time."
    );
    return ret;
}

void AlgorithmCiftiCorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(2);
    OptionalParameter* roiOverrideOpt = myParams->getOptionalParameter(3);
    bool roiOverrideMode = roiOverrideOpt->m_present;
    MetricFile* leftRoi = NULL, *rightRoi = NULL, *cerebRoi = NULL;
    VolumeFile* volRoi = NULL;
    if (roiOverrideMode)
    {
        OptionalParameter* leftRoiOpt = roiOverrideOpt->getOptionalParameter(1);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
        }
        OptionalParameter* rightRoiOpt = roiOverrideOpt->getOptionalParameter(2);
        if (rightRoiOpt->m_present)
        {
            rightRoi = rightRoiOpt->getMetric(1);
        }
        OptionalParameter* cerebRoiOpt = roiOverrideOpt->getOptionalParameter(3);
        if (cerebRoiOpt->m_present)
        {
            cerebRoi = cerebRoiOpt->getMetric(1);
        }
        OptionalParameter* volRoiOpt = roiOverrideOpt->getOptionalParameter(4);
        if (volRoiOpt->m_present)
        {
            volRoi = volRoiOpt->getVolume(1);
        }
    }
    float memLimitGB = -1.0f;
    bool fisherZ = myParams->getOptionalParameter(4)->m_present;
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(5);
    if (memLimitOpt->m_present)
    {
        memLimitGB = (float)memLimitOpt->getDouble(1);
        if (memLimitGB < 0.0f)
        {
            throw AlgorithmException("memory limit cannot be negative");
        }
    }
    if (roiOverrideMode)
    {
        AlgorithmCiftiCorrelation(myProgObj, myCifti, myCiftiOut, leftRoi, rightRoi, cerebRoi, volRoi, fisherZ, memLimitGB);
    } else {
        AlgorithmCiftiCorrelation(myProgObj, myCifti, myCiftiOut, fisherZ, memLimitGB);
    }
}

AlgorithmCiftiCorrelation::AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                                     const bool& fisherZ, const float& memLimitGB) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    init(myCifti);
    int numRows = myCifti->getNumberOfRows();
    CiftiXML newXML = myCifti->getCiftiXML();
    newXML.applyRowMapToColumns();
    myCiftiOut->setCiftiXML(newXML);
    if (myCifti->isInMemory())
    {
        CaretArray<float> outrow(numRows);
        for (int i = 0; i < numRows; ++i)
        {
            float fixedMean, fixedDev;
            CaretArray<float> fixedRow = getRow(i, fixedMean, fixedDev);
            for (int j = 0; j < numRows; ++j)
            {
                float movingMean, movingDev;
                CaretArray<float> movingRow = getRow(j, movingMean, movingDev);
                outrow[j] = correlate(movingRow, movingMean, movingDev, fixedRow, fixedMean, fixedDev, fisherZ);
            }
            myCiftiOut->setRow(outrow, i);
        }
    } else {
        int numCacheRows;
        if (memLimitGB >= 0.0f)
        {
            numCacheRows = numRowsForMem(memLimitGB);
        } else {
            numCacheRows = numRows;
        }
        if (numCacheRows > numRows) numCacheRows = numRows;
        CaretLogInfo("using " + AString::number(numCacheRows) + " rows at a time");
        vector<CaretArray<float> > outRows;
        for (int startrow = 0; startrow < numRows; startrow += numCacheRows)
        {
            int endrow = startrow + numCacheRows;
            if (endrow > numRows) endrow = numRows;
            outRows.resize(endrow - startrow);
            for (int i = startrow; i < endrow; ++i)
            {
                cacheRow(i);//preload the rows in a range which we will reuse as much as possible during one row by row scan
                if (outRows[i - startrow].size() != numRows)
                {
                    outRows[i - startrow] = CaretArray<float>(numRows);
                }
            }
            for (int i = 0; i < numRows; ++i)
            {
                float movingMean, movingDev;
                CaretArray<float> movingRow = getRow(i, movingMean, movingDev);
                for (int j = startrow; j < endrow; ++j)
                {
                    if (j < i && i >= startrow && i < endrow)//if on the upper triangle, and i is within the current caching range, so we have the outRow allocated 
                    {
                        outRows[j - startrow][i] = outRows[i - startrow][j];//copy from other half of the triangle
                    } else {
                        float cacheMean, cacheDev;
                        CaretArray<float> cacheRow = getRow(j, cacheMean, cacheDev);//this bit isn't obvious: CaretArray acts like a float*, so this isn't a copy
                        outRows[j - startrow][i] = correlate(movingRow, movingMean, movingDev, cacheRow, cacheMean, cacheDev, fisherZ);
                    }
                }
            }
            for (int i = startrow; i < endrow; ++i)
            {
                myCiftiOut->setRow(outRows[i - startrow], i);
            }
            clearCache();//tell the cache we are going to preload a different set of rows now
        }
    }
}

AlgorithmCiftiCorrelation::AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                                     const MetricFile* leftRoi, const MetricFile* rightRoi, const MetricFile* cerebRoi,
                                                     const VolumeFile* volRoi, const bool& fisherZ, const float& memLimitGB) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    init(myCifti);
    const CiftiXML& origXML = myCifti->getCiftiXML();
    if (origXML.getRowMappingType() != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        throw AlgorithmException("cannot use ROIs on this cifti, rows are not brain models");
    }
    CiftiXML newXML = origXML;
    vector<StructureEnum::Enum> surfList, volList;
    newXML.getStructureListsForRows(surfList, volList);
    newXML.applyRowMapToColumns();
    newXML.resetRowsToBrainModels();
    vector<pair<int, int> > ciftiIndexList;
    int newCiftiIndex = 0;
    for (int i = 0; i < (int)surfList.size(); ++i)
    {
        const MetricFile* myRoi = NULL;
        switch (surfList[i])
        {
            case StructureEnum::CORTEX_LEFT:
                myRoi = leftRoi;
                break;
            case StructureEnum::CORTEX_RIGHT:
                myRoi = rightRoi;
                break;
            case StructureEnum::CEREBELLUM:
                myRoi = cerebRoi;
                break;
            default:
                break;
        }
        if (myRoi != NULL)
        {
            if (myRoi->getNumberOfNodes() != origXML.getRowSurfaceNumberOfNodes(surfList[i]))
            {
                throw AlgorithmException("surface roi has the wrong number of nodes for structure " + StructureEnum::toName(surfList[i]));
            }
            vector<CiftiSurfaceMap> myMap;
            myCifti->getSurfaceMapForRows(myMap, surfList[i]);
            int numNodes = myCifti->getRowSurfaceNumberOfNodes(surfList[i]);
            int mapsize = (int)myMap.size();
            vector<int64_t> tempNodeList;
            for (int j = 0; j < mapsize; ++j)
            {
                int myNode = myMap[j].m_surfaceNode;
                if (myRoi->getValue(myNode, 0) > 0.0f)
                {
                    tempNodeList.push_back(myNode);
                    ciftiIndexList.push_back(make_pair<int, int>(myMap[j].m_ciftiIndex, newCiftiIndex));
                    ++newCiftiIndex;
                }
            }
            if (tempNodeList.size() > 0)//don't add it if it is empty
            {
                newXML.addSurfaceModelToRows(numNodes, surfList[i], tempNodeList);
            }
        }
    }
    if (volRoi != NULL)
    {
        int64_t origDims[3];
        vector<vector<float> > origSForm;
        origXML.getVolumeDimsAndSForm(origDims, origSForm);
        if (!volRoi->matchesVolumeSpace(origDims, origSForm))
        {
            throw AlgorithmException("roi volume space doesn't match cifti volume space");
        }
        for (int i = 0; i < (int)volList.size(); ++i)
        {
            vector<CiftiVolumeMap> myMap;
            origXML.getVolumeStructureMapForRows(myMap, volList[i]);
            vector<voxelIndexType> tempVoxList;
            int64_t numVoxels = (int64_t)myMap.size();
            for (int64_t j = 0; j < numVoxels; ++j)
            {
                if (volRoi->getValue(myMap[j].m_ijk) > 0.0f)
                {
                    tempVoxList.push_back(myMap[j].m_ijk[0]);
                    tempVoxList.push_back(myMap[j].m_ijk[1]);
                    tempVoxList.push_back(myMap[j].m_ijk[2]);
                    ciftiIndexList.push_back(make_pair<int, int>(myMap[j].m_ciftiIndex, newCiftiIndex));
                    ++newCiftiIndex;
                }
            }
            if (tempVoxList.size() > 0)
            {
                newXML.addVolumeModelToRows(tempVoxList, volList[i]);
            }
        }
    }
    myCiftiOut->setCiftiXML(newXML);
    int numSelected = (int)ciftiIndexList.size(), numRows = myCifti->getNumberOfRows();
    if (myCifti->isInMemory())
    {
        CaretArray<float> outrow(numRows);
        for (int i = 0; i < numSelected; ++i)
        {
            float fixedMean, fixedDev;
            CaretArray<float> fixedRow = getRow(ciftiIndexList[i].first, fixedMean, fixedDev);
            for (int j = 0; j < numRows; ++j)
            {
                float movingMean, movingDev;
                CaretArray<float> movingRow = getRow(j, movingMean, movingDev);
                outrow[j] = correlate(movingRow, movingMean, movingDev, fixedRow, fixedMean, fixedDev, fisherZ);
            }
            myCiftiOut->setRow(outrow, ciftiIndexList[i].second);
        }
    } else {
        int numCacheRows;
        if (memLimitGB >= 0.0f)
        {
            numCacheRows = numRowsForMem(memLimitGB);
        } else {
            numCacheRows = numSelected;
        }
        if (numCacheRows > numSelected) numCacheRows = numSelected;
        CaretLogInfo("using " + AString::number(numCacheRows) + " rows at a time");
        vector<CaretArray<float> > outRows;
        for (int startrow = 0; startrow < numRows; startrow += numCacheRows)
        {
            int endrow = startrow + numCacheRows;
            if (endrow > numSelected) endrow = numSelected;
            outRows.resize(endrow - startrow);
            for (int i = startrow; i < endrow; ++i)
            {
                cacheRow(ciftiIndexList[i].first);//preload the rows in a range which we will reuse as much as possible during one row by row scan
                if (outRows[i - startrow].size() != numRows)
                {
                    outRows[i - startrow] = CaretArray<float>(numRows);
                }
            }
            for (int i = 0; i < numRows; ++i)
            {
                float movingMean, movingDev;
                CaretArray<float> movingRow = getRow(i, movingMean, movingDev);
                for (int j = startrow; j < endrow; ++j)
                {
                    float cacheMean, cacheDev;
                    CaretArray<float> cacheRow = getRow(ciftiIndexList[j].first, cacheMean, cacheDev);//this bit isn't obvious: CaretArray acts like a float*, so this isn't a copy
                    outRows[j - startrow][i] = correlate(movingRow, movingMean, movingDev, cacheRow, cacheMean, cacheDev, fisherZ);
                }
            }
            for (int i = startrow; i < endrow; ++i)
            {
                myCiftiOut->setRow(outRows[i - startrow], ciftiIndexList[i].second);
            }
            clearCache();//tell the cache we are going to preload a different set of rows now
        }
    }
}

float AlgorithmCiftiCorrelation::correlate(const float* row1, const float& mean1, const float& dev1, const float* row2, const float& mean2, const float& dev2, const bool& fisherZ)
{
    double accum = 0.0;
    for (int i = 0; i < m_numCols; ++i)
    {
        accum += (row1[i] - mean1) * (row2[i] - mean2);
    }
    if (fisherZ)
    {
        double r = accum / m_numCols / dev1 / dev2;
        return 0.5 * log((1 + r) / (1 - r));
    } else {
        return accum / m_numCols / dev1 / dev2;
    }
}

void AlgorithmCiftiCorrelation::init(const CiftiFile* input)
{
    m_inputCifti = input;
    m_rowInfo.resize(m_inputCifti->getNumberOfRows());
    m_tempRowPos = 0;
    m_cacheUsed = 0;
    m_numCols = m_inputCifti->getNumberOfColumns();
}

void AlgorithmCiftiCorrelation::cacheRow(const int& ciftiIndex)
{
    CaretAssertVectorIndex(m_rowInfo, ciftiIndex);
    if (m_rowInfo[ciftiIndex].m_cacheIndex != -1) return;//shouldn't happen, but hey
    if (m_cacheUsed >= (int)m_rowCache.size())
    {
        m_rowCache.push_back(CacheRow());
        m_rowCache[m_cacheUsed].m_row = CaretArray<float>(m_numCols);
    }
    m_rowCache[m_cacheUsed].m_ciftiIndex = ciftiIndex;
    m_inputCifti->getRow(m_rowCache[m_cacheUsed].m_row, ciftiIndex);
    m_rowInfo[ciftiIndex].m_cacheIndex = m_cacheUsed;
    ++m_cacheUsed;
}

void AlgorithmCiftiCorrelation::clearCache()
{
    for (int i = 0; i < m_cacheUsed; ++i)
    {
        m_rowInfo[m_rowCache[i].m_ciftiIndex].m_cacheIndex = -1;
    }
    m_cacheUsed = 0;
}

CaretArray<float> AlgorithmCiftiCorrelation::getRow(const int& ciftiIndex, float& mean, float& stdev)
{
    CaretAssertVectorIndex(m_rowInfo, ciftiIndex);
    CaretArray<float> ret;
    if (m_rowInfo[ciftiIndex].m_cacheIndex != -1)
    {
        ret = m_rowCache[m_rowInfo[ciftiIndex].m_cacheIndex].m_row;
    } else {
        ret = getTempRow();
        m_inputCifti->getRow(ret, ciftiIndex);
    }
    if (!m_rowInfo[ciftiIndex].m_haveCalculated)
    {
        int datasize = (int)ret.size();
        double accum = 0.0f;//double, for numerical stability
        for (int i = 0; i < datasize; ++i)//two pass, for numerical stability
        {
            accum += ret[i];
        }
        m_rowInfo[ciftiIndex].m_mean = accum / datasize;
        accum = 0.0;
        for (int i = 0; i < datasize; ++i)
        {
            float tempf = ret[i] - m_rowInfo[ciftiIndex].m_mean;
            accum += tempf * tempf;
        }
        m_rowInfo[ciftiIndex].m_stddev = sqrt(accum / datasize);//should this be sample standard deviation?
        m_rowInfo[ciftiIndex].m_haveCalculated = true;
    }
    mean = m_rowInfo[ciftiIndex].m_mean;
    stdev = m_rowInfo[ciftiIndex].m_stddev;
    return ret;
}

CaretArray<float> AlgorithmCiftiCorrelation::getTempRow()
{
    const int MAX_TEMP_ROWS = 5;
    int numTempRows = (int)m_tempRows.size();
    for (int i = 0; i < numTempRows; ++i)
    {
        ++m_tempRowPos;//usually points to previous return value, so cycle it first
        if (m_tempRowPos >= numTempRows) m_tempRowPos = 0;
        if (m_tempRows[m_tempRowPos].getReferenceCount() == 1)//check for not in use
        {
            return m_tempRows[m_tempRowPos];
        }
    }
    if (numTempRows < MAX_TEMP_ROWS)//all temp rows in use, try to add one
    {
        m_tempRowPos = numTempRows;
        m_tempRows.push_back(CaretArray<float>(m_numCols));
        return m_tempRows[m_tempRowPos];
    }
    CaretArray<float> ret = CaretArray<float>(m_numCols);//temp rows full, forget an old one and allocate a new one
    m_tempRows[m_tempRowPos] = ret;
    ++m_tempRowPos;//cycle the position so that if it is found full next time also, it overwrites another old instead of this new one
    if (m_tempRowPos >= numTempRows) m_tempRowPos = 0;
    return ret;
}

int AlgorithmCiftiCorrelation::numRowsForMem(const float& memLimitGB)
{
    int numRows = m_inputCifti->getNumberOfRows();
    int inrowBytes = m_numCols * sizeof(float), outrowBytes = numRows * sizeof(float);
    int64_t targetBytes = (int64_t)(memLimitGB * 1024 * 1024 * 1024);
    targetBytes -= inrowBytes;//1 row in memory that isn't a reference to cache
    targetBytes -= numRows * sizeof(RowInfo);//storage for mean, stdev, and info about caching
    int64_t perRowBytes = inrowBytes + outrowBytes;//cache and memory collation for output rows
    if (perRowBytes == 0) return 1;//protect against integer div by zero
    int ret = targetBytes / perRowBytes;//integer divide rounds down
    if (ret < 1) return 1;//always return at least one
    return ret;
}

float AlgorithmCiftiCorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCorrelation::getSubAlgorithmWeight()
{
    return 0.0f;
}
