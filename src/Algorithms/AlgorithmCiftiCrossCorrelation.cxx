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

#include "AlgorithmCiftiCrossCorrelation.h"
#include "AlgorithmException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "dot_wrapper.h"
#include "FileInformation.h"

#include <cmath>
#include <fstream>

using namespace caret;
using namespace std;

AString AlgorithmCiftiCrossCorrelation::getCommandSwitch()
{
    return "-cifti-cross-correlation";
}

AString AlgorithmCiftiCrossCorrelation::getShortDescription()
{
    return "CORRELATE A CIFTI FILE WITH ANOTHER CIFTI FILE";
}

OperationParameters* AlgorithmCiftiCrossCorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addCiftiParameter(1, "cifti-a", "first input cifti file");
    
    ret->addCiftiParameter(2, "cifti-b", "second input cifti file");
    
    ret->addCiftiOutputParameter(3, "cifti-out", "output cifti file");
    
    OptionalParameter* weightsOpt = ret->createOptionalParameter(4, "-weights", "specify column weights");
    weightsOpt->addStringParameter(1, "weight-file", "text file containing one weight per column");
    
    ret->createOptionalParameter(5, "-fisher-z", "apply fisher small z transform (ie, artanh) to correlation");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(6, "-mem-limit", "restrict memory usage");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("Correlates every row in <cifti-a> with every row in <cifti-b>.  ") +
        "The mapping along columns in <cifti-b> becomes the mapping along rows in the output.\n\n" +
        "When using the -fisher-z option, the output is NOT a Z-score, it is artanh(r), to do further math on this output, consider using -cifti-math.\n\n" +
        "Restricting the memory usage will make it calculate the output in chunks, by reading through <cifti-b> multiple times."
    );
    return ret;
}

void AlgorithmCiftiCrossCorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    CiftiFile* myCiftiA = myParams->getCifti(1);
    CiftiFile* myCiftiB = myParams->getCifti(2);
    CiftiFile* myCiftiOut = myParams->getOutputCifti(3);
    OptionalParameter* weightsOpt = myParams->getOptionalParameter(4);
    vector<float>* weights = NULL, realweights;//NOTE: realweights is not a pointer
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
    AlgorithmCiftiCrossCorrelation(myProgObj, myCiftiA, myCiftiB, myCiftiOut, weights, fisherZ, memLimitGB);
}

AlgorithmCiftiCrossCorrelation::AlgorithmCiftiCrossCorrelation(ProgressObject* myProgObj, const CiftiFile* myCiftiA, const CiftiFile* myCiftiB, CiftiFile* myCiftiOut,
                                                               const vector<float>* weights, const bool& fisherZ, const float& memLimitGB) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    init(myCiftiA, myCiftiB, myCiftiOut, weights);
    CiftiXMLOld outXML = myCiftiA->getCiftiXMLOld();
    outXML.copyMapping(CiftiXMLOld::ALONG_ROW, myCiftiB->getCiftiXMLOld(), CiftiXMLOld::ALONG_COLUMN);//(try to) copy B's along column mapping to output's along row mapping
    myCiftiOut->setCiftiXML(outXML);
    int64_t chunkSize = m_numRowsA;
    if (memLimitGB >= 0.0f)
    {
        chunkSize = numRowsForMem(memLimitGB);
    }
    vector<vector<float> > outscratch(chunkSize, vector<float>(m_numRowsB));//allocate output rows
    for (int64_t chunkStart = 0; chunkStart < m_numRowsA; chunkStart += chunkSize)
    {
        int64_t chunkEnd = chunkStart + chunkSize;
        if (chunkEnd > m_numRowsA) chunkEnd = m_numRowsA;
        cacheRowsA(chunkStart, chunkEnd);
        int64_t counter = 0;
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int64_t i = 0; i < m_numRowsB; ++i)
        {
            float rrsB;
            int64_t indB;
            const float* rowB;
#pragma omp critical
            {
                indB = counter;//manually in-order rows because we need to read them from disk, and can't request more than one at a time
                ++counter;
                rowB = getRowB(indB, rrsB);
            }
            for (int indA = chunkStart; indA < chunkEnd; ++indA)
            {
                float rrsA;
                const float* rowA = getCachedRowA(indA, rrsA);
                outscratch[indA - chunkStart][indB] = correlate(rowA, rrsA, rowB, rrsB, fisherZ);
            }
        }
        for (int64_t indA = chunkStart; indA < chunkEnd; ++indA)
        {
            myCiftiOut->setRow(outscratch[indA - chunkStart].data(), indA);
        }
    }
}

void AlgorithmCiftiCrossCorrelation::init(const CiftiFile* myCiftiA, const CiftiFile* myCiftiB, const CiftiFile* myCiftiOut, const vector<float>* weights)
{
    m_numCols = myCiftiA->getNumberOfColumns();
    if (myCiftiB->getNumberOfColumns() != m_numCols) throw AlgorithmException("input cifti files have different row lengths");
    m_numRowsA = myCiftiA->getNumberOfRows();
    m_numRowsB = myCiftiB->getNumberOfRows();
    m_ciftiA = myCiftiA;
    m_ciftiB = myCiftiB;
    m_ciftiOut = myCiftiOut;
    m_rowInfoA.resize(m_numRowsA);//calls default constructors, setting m_haveCalculated and m_cacheIndex
    m_rowInfoB.resize(m_numRowsB);
    if (weights != NULL)
    {
        m_weightSum = 0.0;
        m_weightedMode = true;
        int64_t numWeights = (int64_t)weights->size();
        if (numWeights != m_numCols) throw AlgorithmException("input weights do not match row length");
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
                m_weightSum += val;
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

int64_t AlgorithmCiftiCrossCorrelation::numRowsForMem(const float& memLimitGB)
{
    int64_t targetBytes = (int64_t)(memLimitGB * 1024 * 1024 * 1024);
    if (m_ciftiOut->isInMemory()) targetBytes -= sizeof(float) * m_numRowsA * m_numRowsB;//count only in-memory output against total, the only time inputs might be in memory is in the GUI
    int64_t bytesPerInputRow = sizeof(float) * m_numCols;//this means we expect the user to give "current free memory" as the limit
    int64_t bytesPerOutputRow = sizeof(float) * m_numRowsB;
#ifdef CARET_OMP
    targetBytes -= bytesPerInputRow * omp_get_max_threads();//subtract the temporary row memory
#else
    targetBytes -= bytesPerInputRow;
#endif
    int64_t ret = 1;
    if (targetBytes < 1)
    {
        ret = 1;//assume the user knows what they are doing when they request minimum memory usage, even if we do yell at them
    } else {
        int64_t numRowsMax = targetBytes / (bytesPerInputRow + bytesPerOutputRow);//calculate max that can fit in given memory
        if (numRowsMax < 1) numRowsMax = 1;
        int64_t numPasses = (m_numRowsA - 1) / numRowsMax + 1;//figure the number of passes that makes
        if (numPasses < 1)
        {
            CaretLogWarning("memory usage calculation found zero/negative pass solution, report to developers (it may use a lot of memory this run)");
            numPasses = 1;
        }
        ret = (m_numRowsA - 1) / numPasses + 1;//and then figure the most even distribution for that number of passes
    }
    if (ret == 1)
    {
        if (!m_ciftiA->isInMemory() || !m_ciftiB->isInMemory())
        {
            CaretLogWarning("requested memory usage is too low, and at least one input is not in memory, using only 1 row at a time - this may be extremely slow");
        }
    }
    return ret;
}

float AlgorithmCiftiCrossCorrelation::correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2, const bool& fisherZ)
{
    double r;
    if (m_weightedMode)
    {
        int numWeights = (int)m_weightIndexes.size();//because we compacted the data in the row to not include any zero weights
        double accum = sddot(row1, row2, numWeights);//these have already had the weighted row means subtracted out, and weights applied
        r = accum / (rrs1 * rrs2);//as do these
    } else {
        double accum = sddot(row1, row2, m_numCols);//these have already had the row means subtracted out
        r = accum / (rrs1 * rrs2);
    }
    if (fisherZ)
    {
        if (r > 0.999999) r = 0.999999;//prevent inf
        if (r < -0.999999) r = -0.999999;//prevent -inf
        return 0.5 * log((1 + r) / (1 - r));
    } else {
        if (r > 1.0) r = 1.0;//don't output anything silly
        if (r < -1.0) r = -1.0;
        return r;
    }
}

const float* AlgorithmCiftiCrossCorrelation::getCachedRowA(const int64_t& ciftiIndex, float& rootResidSqr)
{
    CaretAssertVectorIndex(m_rowInfoA, ciftiIndex);
    CaretAssert(m_rowInfoA[ciftiIndex].m_cacheIndex != -1);
    rootResidSqr = m_rowInfoA[ciftiIndex].m_rootResidSqr;
    return m_rowCacheA[m_rowInfoA[ciftiIndex].m_cacheIndex].m_row.data();
}

const float* AlgorithmCiftiCrossCorrelation::getRowB(const int64_t& ciftiIndex, float& rootResidSqr)
{
    CaretAssertVectorIndex(m_rowInfoB, ciftiIndex);
    float* ret = getTempRowB();//purely allocation, one array per thread
    m_ciftiB->getRow(ret, ciftiIndex);
    adjustRow(ret, m_rowInfoB[ciftiIndex]);
    rootResidSqr = m_rowInfoB[ciftiIndex].m_rootResidSqr;//NOTE: must do this AFTER adjustRow, because it is not computed before it on the first chunk
    return ret;
}

float* AlgorithmCiftiCrossCorrelation::getTempRowB()
{
#ifdef CARET_OMP
    int oldsize = (int)m_tempRowsB.size();
    int threadNum = omp_get_thread_num();
    if (threadNum >= oldsize)
    {
        m_tempRowsB.resize(threadNum + 1);
        for (int i = oldsize; i <= threadNum; ++i)
        {
            m_tempRowsB[i] = CaretArray<float>(m_numCols);
        }
    }
    return m_tempRowsB[threadNum].getArray();
#else
    if (m_tempRowsB.size() == 0)
    {
        m_tempRowsB.resize(1);
        m_tempRowsB[0] = CaretArray<float>(m_numCols);
    }
    return m_tempRowsB[0].getArray();
#endif
}

void AlgorithmCiftiCrossCorrelation::cacheRowsA(const int64_t& begin, const int64_t& end)
{
    CaretAssert(begin > -1);
    CaretAssert(end <= m_numRowsA);
    CaretAssert(begin < end);//takes care of end <= 0 and being >= numrows
    int64_t cacheSize = (int64_t)m_rowCacheA.size();//clear the existing cache references
    for (int64_t i = 0; i < cacheSize; ++i)
    {
        m_rowInfoA[m_rowCacheA[i].m_ciftiIndex].m_cacheIndex = -1;
    }
    m_rowCacheA.resize(end - begin);//set to exactly the size needed
    int64_t counter = begin;//force in-order row reading via critical and counter
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t i = begin; i < end; ++i)
    {
        int64_t myindex;
#pragma omp critical
        {
            myindex = counter;
            ++counter;
            m_rowCacheA[myindex - begin].m_row.resize(m_numCols);
            m_ciftiA->getRow(m_rowCacheA[myindex - begin].m_row.data(), myindex);
        }
        CacheRow& myRow = m_rowCacheA[myindex - begin];
        myRow.m_ciftiIndex = myindex;
        m_rowInfoA[myindex].m_cacheIndex = myindex - begin;
        adjustRow(myRow.m_row.data(), m_rowInfoA[myindex]);
    }
}

void AlgorithmCiftiCrossCorrelation::adjustRow(float* row, RowInfo& info)
{
    if (!info.m_haveCalculated)//ensure statistics are calculated
    {
        info.m_haveCalculated = true;
        double accum = 0.0;
        if (m_weightedMode)
        {
            int64_t mycount = (int64_t)m_weightIndexes.size();
            if (m_binaryWeights)
            {
                for (int64_t i = 0; i < mycount; ++i)
                {
                    accum += row[m_weightIndexes[i]];
                }
                info.m_mean = accum / mycount;
                accum = 0.0;
                for (int64_t i = 0; i < mycount; ++i)
                {
                    float tempf = row[m_weightIndexes[i]] - info.m_mean;
                    accum += tempf * tempf;
                }
                info.m_rootResidSqr = sqrt(accum);
            } else {
                for (int64_t i = 0; i < mycount; ++i)
                {
                    accum += m_weights[i] * row[m_weightIndexes[i]];
                }
                info.m_mean = accum / m_weightSum;
                accum = 0.0;
                for (int64_t i = 0; i < mycount; ++i)
                {
                    float tempf = row[m_weightIndexes[i]] - info.m_mean;
                    accum += m_weights[i] * tempf * tempf;
                }
                info.m_rootResidSqr = sqrt(accum);
            }
        } else {
            for (int64_t i = 0; i < m_numCols; ++i)
            {
                accum += row[i];
            }
            info.m_mean = accum / m_numCols;
            accum = 0.0;
            for (int64_t i = 0; i < m_numCols; ++i)
            {
                float tempf = row[i] - info.m_mean;
                accum += tempf * tempf;
            }
            info.m_rootResidSqr = sqrt(accum);
        }
    }
    if (m_weightedMode)//COMPACT data, subtract mean, multiply by square root of weights if applicable
    {
        int64_t mycount = (int64_t)m_weightIndexes.size();
        if (m_binaryWeights)
        {
            for (int64_t i = 0; i < mycount; ++i)
            {
                row[i] = row[m_weightIndexes[i]] - info.m_mean;
            }
        } else {
            for (int64_t i = 0; i < mycount; ++i)
            {
                row[i] = sqrt(m_weights[i]) * (row[m_weightIndexes[i]] - info.m_mean);//this is so the numerator doesn't get squared weights applied, since this happens to both rows
            }
        }
    } else {
        for (int64_t i = 0; i < m_numCols; ++i)
        {
            row[i] -= info.m_mean;
        }
    }
}

float AlgorithmCiftiCrossCorrelation::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiCrossCorrelation::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
