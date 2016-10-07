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

#include "AlgorithmCiftiCorrelation.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiSeparate.h"
#include "CiftiFile.h"
#include "MetricFile.h"
#include "VolumeFile.h"
#include "CaretLogger.h"
#include "MathFunctions.h"
#include "CaretOMP.h"
#include "FileInformation.h"
#include "CaretPointer.h"
#include "dot_wrapper.h"
#include <fstream>
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
    rightRoiOpt->addMetricParameter(1, "roi-metric", "the right roi as a metric file");
    OptionalParameter* cerebRoiOpt = roiOverrideOpt->createOptionalParameter(3, "-cerebellum-roi", "use an roi for cerebellum");
    cerebRoiOpt->addMetricParameter(1, "roi-metric", "the cerebellum roi as a metric file");
    OptionalParameter* volRoiOpt = roiOverrideOpt->createOptionalParameter(4, "-vol-roi", "use an roi for volume");
    volRoiOpt->addVolumeParameter(1, "roi-vol", "the volume roi file");
    OptionalParameter* ciftiRoiOpt = roiOverrideOpt->createOptionalParameter(5, "-cifti-roi", "use a cifti file for combined rois");
    ciftiRoiOpt->addCiftiParameter(1, "roi-cifti", "the cifti roi file");
    
    OptionalParameter* weightsOpt = ret->createOptionalParameter(4, "-weights", "specify column weights");
    weightsOpt->addStringParameter(1, "weight-file", "text file containing one weight per column");
    
    ret->createOptionalParameter(5, "-fisher-z", "apply fisher small z transform (ie, artanh) to correlation");
    
    ret->createOptionalParameter(7, "-no-demean", "instead of correlation, do dot product of rows, then normalize by diagonal");
    
    ret->createOptionalParameter(8, "-covariance", "compute covariance instead of correlation");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(6, "-mem-limit", "restrict memory usage");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("For each row (or each row inside an roi if -roi-override is specified), correlate to all other rows.  ") +
        "The -cifti-roi suboption to -roi-override may not be specified with any other -*-roi suboption, but you may specify the other -*-roi suboptions together.\n\n" +
        "When using the -fisher-z option, the output is NOT a Z-score, it is artanh(r), to do further math on this output, consider using -cifti-math.\n\n" +
        "Restricting the memory usage will make it calculate the output in chunks, and if the input file size is more than 70% of the memory limit, " +
        "it will also read through the input file as rows are required, resulting in several passes through the input file (once per chunk).  " +
        "Memory limit does not need to be an integer, you may also specify 0 to calculate a single output row at a time (this may be very slow)."
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
    CiftiFile* ciftiRoi = NULL;
    bool ciftiRoiMode = true;
    if (roiOverrideMode)
    {
        OptionalParameter* leftRoiOpt = roiOverrideOpt->getOptionalParameter(1);
        if (leftRoiOpt->m_present)
        {
            leftRoi = leftRoiOpt->getMetric(1);
            ciftiRoiMode = false;
        }
        OptionalParameter* rightRoiOpt = roiOverrideOpt->getOptionalParameter(2);
        if (rightRoiOpt->m_present)
        {
            rightRoi = rightRoiOpt->getMetric(1);
            ciftiRoiMode = false;
        }
        OptionalParameter* cerebRoiOpt = roiOverrideOpt->getOptionalParameter(3);
        if (cerebRoiOpt->m_present)
        {
            cerebRoi = cerebRoiOpt->getMetric(1);
            ciftiRoiMode = false;
        }
        OptionalParameter* volRoiOpt = roiOverrideOpt->getOptionalParameter(4);
        if (volRoiOpt->m_present)
        {
            volRoi = volRoiOpt->getVolume(1);
            ciftiRoiMode = false;
        }
        OptionalParameter* ciftiRoiOpt = roiOverrideOpt->getOptionalParameter(5);
        if (ciftiRoiOpt->m_present)
        {
            if (!ciftiRoiMode) throw AlgorithmException("-cifti-roi cannot be specified with any other -*-roi option");
            ciftiRoi = ciftiRoiOpt->getCifti(1);
        } else {
            if (ciftiRoiMode) throw AlgorithmException("-roi-override requires a -*-roi suboption");
        }
    }
    OptionalParameter* weightsOpt = myParams->getOptionalParameter(4);
    vector<float>* weights = NULL, realweights;//NOTE: realweights is NOT a pointer
    if (weightsOpt->m_present)
    {
        weights = &realweights;//point it to the actual vector to signify the option is present
        AString weightFileName = weightsOpt->getString(1);
        FileInformation textFileInfo(weightFileName);
        if (!textFileInfo.exists())
        {
            throw AlgorithmException("weight list file doesn't exist");
        }
        fstream weightListFile(weightFileName.toLocal8Bit().constData(), fstream::in);
        if (!weightListFile.good())
        {
            throw AlgorithmException("error reading weight list file");
        }
        while (weightListFile.good())
        {
            float weight;
            if (!(weightListFile >> weight))//yes, this is how you check fstream for successfully extracted output.  seriously.
            {
                break;
            }
            realweights.push_back(weight);
        }
    }
    bool fisherZ = myParams->getOptionalParameter(5)->m_present;
    float memLimitGB = -1.0f;
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(6);
    if (memLimitOpt->m_present)
    {
        memLimitGB = (float)memLimitOpt->getDouble(1);
        if (memLimitGB < 0.0f)
        {
            throw AlgorithmException("memory limit cannot be negative");
        }
    }
    bool noDemean = myParams->getOptionalParameter(7)->m_present;
    bool covariance = myParams->getOptionalParameter(8)->m_present;
    if (roiOverrideMode)
    {
        if (ciftiRoiMode)
        {
            AlgorithmCiftiCorrelation(myProgObj, myCifti, myCiftiOut, ciftiRoi, weights, fisherZ, memLimitGB, noDemean);
        } else {
            AlgorithmCiftiCorrelation(myProgObj, myCifti, myCiftiOut, leftRoi, rightRoi, cerebRoi, volRoi, weights, fisherZ, memLimitGB, noDemean);
        }
    } else {
        AlgorithmCiftiCorrelation(myProgObj, myCifti, myCiftiOut, weights, fisherZ, memLimitGB, noDemean, covariance);
    }
}

AlgorithmCiftiCorrelation::AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut, const vector<float>* weights,
                                                     const bool& fisherZ, const float& memLimitGB, const bool& noDemean, const bool& covariance) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (covariance)
    {
        if (fisherZ) throw AlgorithmException("cannot apply fisher z transformation to covariance");
    }
    init(myCifti, weights, noDemean, covariance);
    int numRows = myCifti->getNumberOfRows();
    CiftiXMLOld newXML = myCifti->getCiftiXMLOld();
    newXML.applyColumnMapToRows();
    myCiftiOut->setCiftiXML(newXML);
    int numCacheRows;
    bool cacheFullInput = true;
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, cacheFullInput);
    } else {
        numCacheRows = numRows;
    }
    if (numCacheRows > numRows) numCacheRows = numRows;
    if (cacheFullInput)
    {
        if (numCacheRows != numRows) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    vector<CaretArray<float> > outRows;
    if (cacheFullInput)
    {
        for (int i = 0; i < numRows; ++i)
        {
            cacheRow(i);
        }
    }
    for (int startrow = 0; startrow < numRows; startrow += numCacheRows)
    {
        int endrow = startrow + numCacheRows;
        if (endrow > numRows) endrow = numRows;
        outRows.resize(endrow - startrow);
        for (int i = startrow; i < endrow; ++i)
        {
            if (!cacheFullInput)
            {
                cacheRow(i);//preload the rows in a range which we will reuse as much as possible during one row by row scan
            }
            if (outRows[i - startrow].size() != numRows)
            {
                outRows[i - startrow] = CaretArray<float>(numRows);
            }
        }
        int curRow = 0;//because we can't trust the order threads hit the critical section
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < numRows; ++i)
        {
            float movingRrs;
            int myrow;
            const float* movingRow;
#pragma omp critical
            {//CiftiFile may explode if we request multiple rows concurrently (needs mutexes), but we should force sequential requests anyway
                myrow = curRow;//so, manually force it to read sequentially
                ++curRow;
                movingRow = getRow(myrow, movingRrs);
            }
            for (int j = startrow; j < endrow; ++j)
            {
                if (myrow >= startrow && myrow < endrow)//check whether we are in the output memory area
                {
                    if (j >= myrow)//if so, only compute one half, and store both places
                    {
                        float cacheRrs;
                        const float* cacheRow = getRow(j, cacheRrs, true);
                        outRows[j - startrow][myrow] = correlate(movingRow, movingRrs, cacheRow, cacheRrs, fisherZ);
                        outRows[myrow - startrow][j] = outRows[j - startrow][myrow];
                    }
                } else {
                    float cacheRrs;
                    const float* cacheRow = getRow(j, cacheRrs, true);
                    outRows[j - startrow][myrow] = correlate(movingRow, movingRrs, cacheRow, cacheRrs, fisherZ);
                }
            }
        }
        for (int i = startrow; i < endrow; ++i)
        {
            myCiftiOut->setRow(outRows[i - startrow], i);
        }
        if (!cacheFullInput)
        {
            clearCache();//tell the cache we are going to preload a different set of rows now
        }
    }
    if (cacheFullInput)
    {
        clearCache();//don't currently need to do this, its just for completeness
    }
}

AlgorithmCiftiCorrelation::AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                                     const MetricFile* leftRoi, const MetricFile* rightRoi, const MetricFile* cerebRoi,
                                                     const VolumeFile* volRoi, const vector<float>* weights, const bool& fisherZ, const float& memLimitGB,
                                                     const bool& noDemean, const bool& covariance) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (covariance)
    {
        if (fisherZ) throw AlgorithmException("cannot apply fisher z transformation to covariance");
    }
    init(myCifti, weights, noDemean, covariance);
    const CiftiXMLOld& origXML = myCifti->getCiftiXMLOld();
    if (origXML.getColumnMappingType() != CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        throw AlgorithmException("cannot use ROIs on this cifti, columns are not brain models");
    }
    CiftiXMLOld newXML = origXML;
    vector<StructureEnum::Enum> surfList, volList;
    origXML.getStructureListsForColumns(surfList, volList);
    newXML.applyColumnMapToRows();
    newXML.resetColumnsToBrainModels();
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
            if (myRoi->getNumberOfNodes() != origXML.getColumnSurfaceNumberOfNodes(surfList[i]))
            {
                throw AlgorithmException("surface roi has the wrong number of vertices for structure " + StructureEnum::toName(surfList[i]));
            }
            const CiftiBrainModelsMap& myDenseMap = myCifti->getCiftiXML().getBrainModelsMap(CiftiXML::ALONG_COLUMN);
            vector<CiftiBrainModelsMap::SurfaceMap> myMap = myDenseMap.getSurfaceMap(surfList[i]);
            int numNodes = myDenseMap.getSurfaceNumberOfNodes(surfList[i]);
            int mapsize = (int)myMap.size();
            vector<int64_t> tempNodeList;
            for (int j = 0; j < mapsize; ++j)
            {
                int myNode = myMap[j].m_surfaceNode;
                if (myRoi->getValue(myNode, 0) > 0.0f)
                {
                    tempNodeList.push_back(myNode);
                    ciftiIndexList.push_back(std::pair<int,int>(myMap[j].m_ciftiIndex, newCiftiIndex));
                    ++newCiftiIndex;
                }
            }
            if (tempNodeList.size() > 0)//don't add it if it is empty
            {
                newXML.addSurfaceModelToColumns(numNodes, surfList[i], tempNodeList);
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
            origXML.getVolumeStructureMapForColumns(myMap, volList[i]);
            vector<voxelIndexType> tempVoxList;
            int64_t numVoxels = (int64_t)myMap.size();
            for (int64_t j = 0; j < numVoxels; ++j)
            {
                if (volRoi->getValue(myMap[j].m_ijk) > 0.0f)
                {
                    tempVoxList.push_back(myMap[j].m_ijk[0]);
                    tempVoxList.push_back(myMap[j].m_ijk[1]);
                    tempVoxList.push_back(myMap[j].m_ijk[2]);
                    ciftiIndexList.push_back(std::pair<int, int>(myMap[j].m_ciftiIndex, newCiftiIndex));
                    ++newCiftiIndex;
                }
            }
            if (tempVoxList.size() > 0)
            {
                newXML.addVolumeModelToColumns(tempVoxList, volList[i]);
            }
        }
    }
    myCiftiOut->setCiftiXML(newXML);
    int numSelected = (int)ciftiIndexList.size(), numRows = myCifti->getNumberOfRows();
    int numCacheRows;
    bool cacheFullInput = true;
    if (memLimitGB >= 0.0f)
    {
        numCacheRows = numRowsForMem(memLimitGB, cacheFullInput);
    } else {
        numCacheRows = numSelected;
    }
    if (numCacheRows > numSelected) numCacheRows = numSelected;
    if (cacheFullInput)
    {
        if (numCacheRows != numSelected) CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time");
    } else {
        CaretLogInfo("computing " + AString::number(numCacheRows) + " rows at a time, reading rows as needed during processing");
    }
    vector<CaretArray<float> > outRows;
    if (cacheFullInput)
    {
        for (int i = 0; i < numRows; ++i)
        {
            cacheRow(i);
        }
    }
    CaretArray<int> indexReverse(numRows, -1);
    for (int startrow = 0; startrow < numSelected; startrow += numCacheRows)
    {
        int endrow = startrow + numCacheRows;
        if (endrow > numSelected) endrow = numSelected;
        outRows.resize(endrow - startrow);
        int curRow = 0;//because we can't trust the order threads hit the critical section
        for (int i = startrow; i < endrow; ++i)
        {
            if (!cacheFullInput)
            {
                cacheRow(ciftiIndexList[i].first);//preload the rows in a range which we will reuse as much as possible during one row by row scan
            }
            if (outRows[i - startrow].size() != numRows)
            {
                outRows[i - startrow] = CaretArray<float>(numRows);
            }
            indexReverse[ciftiIndexList[i].first] = i;
        }
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int i = 0; i < numRows; ++i)
        {
            float movingRrs;
            int myrow;
            const float* movingRow;
#pragma omp critical
            {//CiftiFile may explode if we request multiple rows concurrently (needs mutexes), but we should force sequential requests anyway
                myrow = curRow;//so, manually force it to read sequentially
                ++curRow;
                movingRow = getRow(myrow, movingRrs);
            }
            for (int j = startrow; j < endrow; ++j)
            {
                if (indexReverse[myrow] != -1)//check if we are on a row that is in the output memory range
                {
                    if (indexReverse[myrow] <= j)//if so, only compute one of the elements, then store it both places
                    {
                        float cacheRrs;
                        const float* cacheRow = getRow(ciftiIndexList[j].first, cacheRrs, true);
                        outRows[j - startrow][myrow] = correlate(movingRow, movingRrs, cacheRow, cacheRrs, fisherZ);
                        outRows[indexReverse[myrow] - startrow][ciftiIndexList[j].first] = outRows[j - startrow][myrow];
                    }
                } else {
                    float cacheRrs;
                    const float* cacheRow = getRow(ciftiIndexList[j].first, cacheRrs, true);
                    outRows[j - startrow][myrow] = correlate(movingRow, movingRrs, cacheRow, cacheRrs, fisherZ);
                }
            }
        }
        for (int i = startrow; i < endrow; ++i)
        {
            myCiftiOut->setRow(outRows[i - startrow], ciftiIndexList[i].second);
            indexReverse[ciftiIndexList[i].first] = -1;
        }
        if (!cacheFullInput)
        {
            clearCache();//tell the cache we are going to preload a different set of rows now
        }
    }
    if (cacheFullInput)
    {
        clearCache();//don't currently need to do this, its just for completeness
    }
}

AlgorithmCiftiCorrelation::AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut, const CiftiFile* ciftiRoi,
                                                     const vector<float>* weights, const bool& fisherZ, const float& memLimitGB,
                                                     const bool& noDemean, const bool& covariance): AbstractAlgorithm(NULL)//HACK: get around the sentinel by passing a null, because this implementation calls another
{
    const CiftiXML& roiXML = ciftiRoi->getCiftiXML();//roi is not optional in this variant
    if (roiXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("cifti roi does not have brain models mapping along column");
    const CiftiBrainModelsMap myDenseMap = roiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN);
    MetricFile leftRoi, rightRoi, cerebRoi;
    MetricFile* leftRoiPtr = NULL, *rightRoiPtr = NULL, *cerebRoiPtr = NULL;
    VolumeFile volRoi;
    VolumeFile* volRoiPtr = NULL;
    vector<StructureEnum::Enum> surfStructs = myDenseMap.getSurfaceStructureList();
    for (int i = 0; i < (int)surfStructs.size(); ++i)
    {
        MetricFile* thisRoi = NULL;
        switch (surfStructs[i])
        {
            case StructureEnum::CORTEX_LEFT:
                thisRoi = &leftRoi;
                leftRoiPtr = thisRoi;
                break;
            case StructureEnum::CORTEX_RIGHT:
                thisRoi = &rightRoi;
                rightRoiPtr = thisRoi;
                break;
            case StructureEnum::CEREBELLUM:
                thisRoi = &cerebRoi;
                cerebRoiPtr = thisRoi;
                break;
            default:
                throw AlgorithmException("structure not supported for surface type: " + StructureEnum::toName(surfStructs[i]));
        }
        AlgorithmCiftiSeparate(NULL, ciftiRoi, CiftiXML::ALONG_COLUMN, surfStructs[i], thisRoi);
    }
    if (myDenseMap.hasVolumeData())
    {
        int64_t offsetOut[3];
        AlgorithmCiftiSeparate(NULL, ciftiRoi, CiftiXML::ALONG_COLUMN, &volRoi, offsetOut, NULL, false);//don't crop, because it needs to match the original volume space in the input
        volRoiPtr = &volRoi;
    }
    AlgorithmCiftiCorrelation(myProgObj, myCifti, myCiftiOut, leftRoiPtr, rightRoiPtr, cerebRoiPtr, volRoiPtr, weights, fisherZ, memLimitGB, noDemean, covariance);//HACK: pass through our progress object
}

float AlgorithmCiftiCorrelation::correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2, const bool& fisherZ)
{
    double r;
    if (row1 == row2 && !m_covariance)
    {
        r = 1.0;//short circuit for same row
    } else {
        if (m_weightedMode)
        {
            int numWeights = (int)m_weightIndexes.size();//because we compacted the data in the row to not include any zero weights
            double accum = sddot(row1, row2, numWeights);//these have already had the weighted row means subtracted out, and weights applied
            if (m_covariance)
            {
                if (m_binaryWeights)
                {
                    r = accum / numWeights;
                } else {
                    r = accum / rrs1;//NOTE: will equal rrs2 as it only depends on weights, and is not square root
                }
            } else {
                r = accum / (rrs1 * rrs2);//as do these
            }
        } else {
            double accum = sddot(row1, row2, m_numCols);//these have already had the row means subtracted out
            if (m_covariance)
            {
                r = accum / m_numCols;
            } else {
                r = accum / (rrs1 * rrs2);
            }
        }
    }
    if (!m_covariance)
    {
        if (fisherZ)
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

void AlgorithmCiftiCorrelation::init(const CiftiFile* input, const vector<float>* weights, const bool& noDemean, const bool& covariance)
{
    m_noDemean = noDemean;
    m_covariance = covariance;
    m_inputCifti = input;
    m_rowInfo.resize(m_inputCifti->getNumberOfRows());
    m_cacheUsed = 0;
    m_numCols = m_inputCifti->getNumberOfColumns();
    if (weights != NULL)
    {
        m_weightedMode = true;
        int numWeights = (int)weights->size();
        if (numWeights != m_numCols)
        {
            throw AlgorithmException("number of weights doesn't match length of a row, number of weights given: " + AString::number(weights->size()));
        }
        m_binaryWeights = true;
        for (int i = 0; i < numWeights; ++i)
        {
            float val = (*weights)[i];
            if (val != 0.0f)
            {
                if (val < 0.0f)
                {
                    throw AlgorithmException("weights cannot be negative");
                }
                m_weights.push_back(val);
                m_weightIndexes.push_back(i);
                if (val != 1.0f)
                {
                    m_binaryWeights = false;
                }
            }
        }
        if (m_binaryWeights && m_weights.size() == weights->size())
        {
            m_weightedMode = false;//all weights were 1, so switch back to normal mode
        }
    } else {
        m_weightedMode = false;
    }
}

void AlgorithmCiftiCorrelation::cacheRow(const int& ciftiIndex)
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

void AlgorithmCiftiCorrelation::clearCache()
{
    for (int i = 0; i < m_cacheUsed; ++i)
    {
        m_rowInfo[m_rowCache[i].m_ciftiIndex].m_cacheIndex = -1;
    }
    m_cacheUsed = 0;
}

const float* AlgorithmCiftiCorrelation::getRow(const int& ciftiIndex, float& rootResidSqr, const bool& mustBeCached)
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

void AlgorithmCiftiCorrelation::computeRowStats(const float* row, float& mean, float& rootResidSqr)
{
    double accum = 0.0;//double, for numerical stability
    if (m_noDemean)
    {
        mean = 0.0f;
    } else {
        if (m_weightedMode)
        {
            int weightsize = (int)m_weightIndexes.size();
            if (m_binaryWeights)//because should be a little faster without multiplies or a second sum
            {
                for (int i = 0; i < weightsize; ++i)
                {
                    accum += row[m_weightIndexes[i]];
                }
                mean = accum / weightsize;
            } else {
                double accum2 = 0.0;
                for (int i = 0; i < weightsize; ++i)
                {
                    float weight = m_weights[i];
                    accum += row[m_weightIndexes[i]] * weight;
                    accum2 += weight;
                }
                mean = accum / accum2;
            }
        } else {
            for (int i = 0; i < m_numCols; ++i)//two pass, for numerical stability
            {
                accum += row[i];
            }
            mean = accum / m_numCols;
        }
    }
    accum = 0.0;
    if (m_covariance)
    {
        int weightsize = (int)m_weightIndexes.size();
        rootResidSqr = 0.0f;
        if (m_weightedMode && !m_binaryWeights)
        {
            for (int i = 0; i < weightsize; ++i)
            {
                accum += m_weights[i];
            }
            rootResidSqr = accum;//repurpose this variable to store the weight sum - NOTE: don't take sqrt in case negative sum (whatever that means), so must not divide by both in correlate() in covariance mode
        }
    } else {
        if (m_weightedMode)
        {
            int weightsize = (int)m_weightIndexes.size();
            if (m_binaryWeights)
            {
                for (int i = 0; i < weightsize; ++i)
                {
                    float tempf = row[m_weightIndexes[i]] - mean;
                    accum += tempf * tempf;
                }
            rootResidSqr = sqrt(accum);
            } else {
                for (int i = 0; i < weightsize; ++i)
                {
                    float tempf = row[m_weightIndexes[i]] - mean;
                    accum += tempf * tempf * m_weights[i];
                }
                rootResidSqr = sqrt(accum);
            }
        } else {
            for (int i = 0; i < m_numCols; ++i)
            {
                float tempf = row[i] - mean;
                accum += tempf * tempf;
            }
            rootResidSqr = sqrt(accum);
        }
    }
}

void AlgorithmCiftiCorrelation::doSubtract(float* row, const float& mean)
{
    if (m_noDemean) return;//skip subtracting zero from everything
    if (m_weightedMode)
    {
        int weightsize = (int)m_weightIndexes.size();
        if (m_binaryWeights)
        {
            for (int i = 0; i < weightsize; ++i)
            {
                row[i] = row[m_weightIndexes[i]] - mean;
            }
        } else {
            for (int i = 0; i < weightsize; ++i)
            {
                row[i] = sqrt(m_weights[i]) * (row[m_weightIndexes[i]] - mean);//multiply by square root of weight, so that the numerator of correlation doesn't get the square of the weight
            }
        }
    } else {
        for (int i = 0; i < m_numCols; ++i)
        {
            row[i] -= mean;
        }
    }
}

float* AlgorithmCiftiCorrelation::getTempRow()
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

int AlgorithmCiftiCorrelation::numRowsForMem(const float& memLimitGB, bool& cacheFullInput)
{
    int numRows = m_inputCifti->getNumberOfRows();
    int inrowBytes = m_numCols * sizeof(float), outrowBytes = numRows * sizeof(float);
    int64_t targetBytes = (int64_t)(memLimitGB * 1024 * 1024 * 1024);
    if (m_inputCifti->isInMemory()) targetBytes -= numRows * m_numCols * 4;//count in-memory input against the total too
#ifdef CARET_OMP
    targetBytes -= inrowBytes * omp_get_max_threads();
#else
    targetBytes -= inrowBytes;//1 row in memory that isn't a reference to cache
#endif
    targetBytes -= numRows * sizeof(RowInfo);//storage for mean, stdev, and info about caching
    int64_t perRowBytes = inrowBytes + outrowBytes;//cache and memory collation for output rows
    if (numRows * m_numCols * 4 < targetBytes * 0.7f)//if caching the entire input file would take less than 70% of remaining allotted memory, do it to reduce IO
    {
        cacheFullInput = true;//precache the entire input file, rather than caching it synchronously with the in-memory output rows
        targetBytes -= numRows * m_numCols * 4;//reduce the remaining total by the memory used
        perRowBytes = outrowBytes;//don't need to count input rows against the remaining memory total
    } else {
        cacheFullInput = false;
    }
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
