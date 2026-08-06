/*LICENSE_START*/
/*
 *  Copyright (C) 2026  Washington University School of Medicine
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

#include "OperationWbsparseAverage.h"
#include "OperationException.h"

#include "CaretSparseFile.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

using namespace caret;
using namespace std;

AString OperationWbsparseAverage::getCommandSwitch()
{
    return "-wbsparse-average";
}

AString OperationWbsparseAverage::getShortDescription()
{
    return "AVERAGE WBSPARSE FILES";
}

OperationParameters* OperationWbsparseAverage::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addStringParameter(1, "wbsparse-out", "output - the output wbsparse file"); //HACK: fake the output formatting
    
    OptionalParameter* thresholdOpt = ret->createOptionalParameter(2, "-threshold", "increase sparseness by removing output values below this threshold");
    thresholdOpt->addDoubleParameter(1, "value", "the threshold value");

    OptionalParameter* memLimitOpt = ret->createOptionalParameter(3, "-mem-limit", "limit how much memory to use for averaging values (more memory does less random IO, less memory may be faster at averaging the sparse values)");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "approximate memory limit in gigabytes (default is 0.1)");
    
    ParameterComponent* wbsparseOpt = ret->createRepeatableParameter(4, "-wbsparse", "specify an input wbsparse file");
    wbsparseOpt->addStringParameter(1, "wbsparse-in", "a wbsparse file to include in the average");
    
    ret->setHelpText(
        AString("Adds the values in the wbsparse files together by index and divides by the number of files.")
    );
    return ret;
}

namespace
{
    //std::map has substantial memory overhead, and we are doing entire rows at a time, so roll our own
    template<typename T>
    struct SparseVector
    {
        vector<T> m_values;
        vector<int64_t> m_indices;
        
        SparseVector<T>() = default;
        
        template <typename T2>
        SparseVector<T>(const map<int64_t, T2>& rhs)
        { //WARNING: assumes standard less() comparator
            m_indices.reserve(rhs.size());
            m_values.reserve(rhs.size());
            int64_t prevIndex = -1;
            for (auto iter : rhs)
            {
                CaretAssert(iter.first > prevIndex);
                prevIndex = iter.first;
                m_indices.push_back(iter.first);
                m_values.push_back(iter.second);
            }
            (void)prevIndex;
        }
        void append(const int64_t index, const T value)
        {
            CaretAssert(m_indices.empty() || index > m_indices.back());
            m_indices.push_back(index);
            m_values.push_back(value);
        }
        template <typename T2>
        SparseVector<T> operator+(const SparseVector<T2>& rhs) const
        {
            SparseVector<T> ret;
            ret.m_indices.reserve(m_indices.size() + rhs.m_indices.size()); //won't reallocate during the loop, worst case is double the needed storage, which is vector behavior anyway
            ret.m_values.reserve(m_indices.size() + rhs.m_indices.size());
            size_t thisIndex = 0, rindex = 0; //after what has already been inserted
            while (thisIndex < m_indices.size() || rindex < rhs.m_indices.size())
            { //seesaw until done, to use insert() ranges maximally - runs are likely, particularly when asymmetric
                size_t nextIndex = thisIndex;
                while (nextIndex < m_indices.size() && (rindex >= rhs.m_indices.size() || m_indices[nextIndex] < rhs.m_indices[rindex])) ++nextIndex;
                if (nextIndex > thisIndex)
                {
                    ret.m_indices.insert(ret.m_indices.end(), m_indices.begin() + thisIndex, m_indices.begin() + nextIndex);
                    ret.m_values.insert(ret.m_values.end(), m_values.begin() + thisIndex, m_values.begin() + nextIndex);
                }
                thisIndex = nextIndex;
                
                //add until indices don't match
                while (thisIndex < m_indices.size() && rindex < rhs.m_indices.size() && m_indices[thisIndex] == rhs.m_indices[rindex])
                {
                    ret.m_indices.push_back(m_indices[thisIndex]);
                    ret.m_values.push_back(m_values[thisIndex] + rhs.m_values[rindex]);
                    ++thisIndex; ++rindex;
                }
                
                //other side
                size_t RnextIndex = rindex;
                while (RnextIndex < rhs.m_indices.size() && (thisIndex >= m_indices.size() || rhs.m_indices[RnextIndex] < m_indices[thisIndex])) ++RnextIndex;
                if (RnextIndex > rindex)
                {
                    ret.m_indices.insert(ret.m_indices.end(), rhs.m_indices.begin() + rindex, rhs.m_indices.begin() + RnextIndex);
                    ret.m_values.insert(ret.m_values.end(), rhs.m_values.begin() + rindex, rhs.m_values.begin() + RnextIndex);
                }
                rindex = RnextIndex;
                
                //add again
                while (thisIndex < m_indices.size() && rindex < rhs.m_indices.size() && m_indices[thisIndex] == rhs.m_indices[rindex])
                {
                    ret.m_indices.push_back(m_indices[thisIndex]);
                    ret.m_values.push_back(m_values[thisIndex] + rhs.m_values[rindex]);
                    ++thisIndex; ++rindex;
                }
            }
            return ret;
        }
        template <typename T2>
        SparseVector<T>& operator+=(const SparseVector<T2>& rhs)
        {
            return *this = *this + rhs; //yes, we want the addition to create a temporary, rather than repeatedly inserting into our existing vector
        }
    };
    
    struct FiberVals
    {
        double m_vals[4]; //weighted distance and 3 counts
        FiberVals operator+(const FiberVals& rhs) const
        {
            FiberVals ret;
            for (int i = 0; i < 4; ++i) ret.m_vals[i] = m_vals[i] + rhs.m_vals[i]; //just so we can use SparseVector's +=
            return ret;
        }
    };
    
    template <typename T, typename A>
    void averageSparse(vector<CaretSparseFile>& inFiles, CaretSparseFileWriter& outFile, const double threshold, const float memLimitGB)
    {
        CaretAssert(!inFiles.empty());
        CaretAssert(inFiles[0].getDatatype() != CaretSparseFile::Fibers);
        const int64_t numRows = inFiles[0].getDimensions()[1], rowLength = inFiles[0].getDimensions()[0];
        const int64_t memLimitBytes = size_t(memLimitGB * 1000000000);
        const float bytesPerElem = (sizeof(T) + sizeof(int64_t)) * 1.5f; //fudge factor for...runtime keeping old allocs around?
        float sparsityMax = -1.0f;
        int64_t chunkSize = 1; //this value will never get used
        for (int64_t chunkStart = 0; chunkStart < numRows; chunkStart += chunkSize) //increment expression isn't evaluated until after the loop body
        {
            double predictedElements = 0.0;
            for (chunkSize = 0; (memLimitBytes < 1 || predictedElements < (memLimitBytes / bytesPerElem)) && chunkSize < numRows - chunkStart; ++chunkSize)
            {
                int64_t rowBestCase = 0;
                double rowRandomPred = 0.0;
                for (auto& thisFile : inFiles)
                {
                    int64_t thisElems = thisFile.getCountOfRowValues(chunkStart + chunkSize);
                    rowRandomPred = rowRandomPred + (rowLength - rowRandomPred) * double(thisElems) / rowLength; //assume remaining sparse count is decreased by this row's sparsity - should still be pessimistic for large numbers of non-adversarial inputs
                    if (thisElems > rowBestCase) rowBestCase = thisElems;
                }
                //cap random prediction at worst-case chunk output sparsity seen so far times 1.2, or a minimum of 10%, whichever is worse
                //so, when averaging across 1000 subjects, it won't just predict "full map" every row
                const float sparsityMin = 0.1f;
                double cappedPred = rowRandomPred;
                if (sparsityMax > 0.0f)
                    cappedPred = min(rowRandomPred, double(max(sparsityMax * 1.2f, sparsityMin) * rowLength));
                predictedElements += max(double(rowBestCase), cappedPred); //if a row is proven to be much denser than any previous chunk, believe it
            }
            CaretAssert(chunkSize > 0);
            CaretAssert(chunkSize <= numRows - chunkStart);
            vector<SparseVector<A> > accum(chunkSize);
            map<int64_t, T> thisRow;
            for (auto& thisFile : inFiles)
            {
                for (int64_t chunkIndex = 0; chunkIndex < chunkSize; ++chunkIndex)
                {
                    int64_t i = chunkStart + chunkIndex;
                    thisRow = thisFile.getRowSparse<T>(i);
                    accum[chunkIndex] += SparseVector<T>(thisRow);
                }
            }
            double sparsityAccum = 0.0f; //we want raw sparsity before threshold, because that is what the memory used is
            for (int64_t chunkIndex = 0; chunkIndex < chunkSize; ++chunkIndex)
            {
                int64_t i = chunkStart + chunkIndex;
                map<int64_t, T> outRow;
                sparsityAccum += float(accum[chunkIndex].m_indices.size()) / rowLength;
                for (int64_t j = 0; j < int64_t(accum[chunkIndex].m_indices.size()); ++j)
                {
                    A tempVal = accum[chunkIndex].m_values[j] / inFiles.size(); //accum is floating-point type
                    if (tempVal >= threshold) //exclude nans
                    {
                        if (numeric_limits<T>::is_integer)
                        {
                            outRow[accum[chunkIndex].m_indices[j]] = T(floor(0.5f + tempVal)); //since it is an average, we shouldn't need to do fancy clamping
                        } else {
                            outRow[accum[chunkIndex].m_indices[j]] = T(tempVal);
                        }
                    }
                }
                outFile.writeRowSparse(i, outRow); //file should be T output type
            }
            float thisSparsity = sparsityAccum / chunkSize;
            if (thisSparsity > sparsityMax) sparsityMax = thisSparsity;
        }
    }
    
    //specialize for fiber type
    template <>
    void averageSparse<FiberFractions, FiberFractions>(vector<CaretSparseFile>& inFiles, CaretSparseFileWriter& outFile, const double threshold, const float memLimitGB)
    {
        CaretAssert(!inFiles.empty());
        CaretAssert(inFiles[0].getDatatype() == CaretSparseFile::Fibers);
        const int64_t numRows = inFiles[0].getDimensions()[1], rowLength = inFiles[0].getDimensions()[0];
        const int64_t memLimitBytes = size_t(memLimitGB * 1000000000);
        const float bytesPerElem = (sizeof(FiberVals) + sizeof(int64_t)) * 1.5f; //runtime keeping old allocs?
        float sparsityMax = -1.0f;
        int64_t chunkSize = 1; //this value will never get used
        for (int64_t chunkStart = 0; chunkStart < numRows; chunkStart += chunkSize) //increment expression isn't evaluated until after the loop body
        {
            double predictedElements = 0.0;
            for (chunkSize = 0; (memLimitBytes < 1 || predictedElements < (memLimitBytes / bytesPerElem)) && chunkSize < numRows - chunkStart; ++chunkSize)
            {
                int64_t rowBestCase = 0;
                double rowRandomPred = 0.0;
                for (auto& thisFile : inFiles)
                {
                    int64_t thisElems = thisFile.getCountOfRowValues(chunkStart + chunkSize);
                    rowRandomPred = rowRandomPred + (rowLength - rowRandomPred) * double(thisElems) / rowLength; //assume remaining sparse count is decreased by this row's sparsity - should still be pessimistic for large numbers of non-adversarial inputs
                    if (thisElems > rowBestCase) rowBestCase = thisElems;
                }
                //cap random prediction at worst-case chunk output sparsity seen so far times 1.2, or a minimum of 10%, whichever is worse
                //so, when averaging across 1000 subjects, it won't just predict "full map" every row
                const float sparsityMin = 0.1f;
                double cappedPred = rowRandomPred;
                if (sparsityMax > 0.0f)
                    cappedPred = min(rowRandomPred, double(max(sparsityMax * 1.2f, sparsityMin) * rowLength));
                predictedElements += max(double(rowBestCase), cappedPred); //if a row is proven to be much denser than any previous chunk, believe it
            }
            CaretAssert(chunkSize > 0);
            CaretAssert(chunkSize <= numRows - chunkStart);
            vector<SparseVector<FiberVals> > accum(chunkSize); //distance and 3 counts
            map<int64_t, FiberFractions> thisRow;
            for (auto& thisFile : inFiles)
            {
                for (int64_t chunkIndex = 0; chunkIndex < chunkSize; ++chunkIndex)
                {
                    int64_t i = chunkStart + chunkIndex;
                    thisRow = thisFile.getRowSparse<FiberFractions>(i);
                    SparseVector<FiberVals> rowVals; //need to do some math before we are ready to do sparse add
                    for (auto& iter : thisRow)
                    {
                        int numFibers = int(iter.second.fiberFractions.size()); //really, this should always be 3, for now
                        CaretAssert(numFibers == 3);
                        if (numFibers != 3) throw OperationException("wbsparse average currently only works with 3 fibers");
                        FiberVals tempVal;
                        for (int j = 0; j < 3; ++j)
                        {
                            tempVal.m_vals[j + 1] = iter.second.totalCount * iter.second.fiberFractions[j];
                        }
                        tempVal.m_vals[0] = iter.second.distance * iter.second.totalCount; //we don't want 0s in distance to implicitly contribute by dividing by number of files, so weighted average by count
                        rowVals.append(iter.first, tempVal);
                    }
                    accum[chunkIndex] += rowVals;
                }
            }
            double sparsityAccum = 0.0f; //we want raw sparsity before threshold, because that is what the memory used is
            for (int64_t chunkIndex = 0; chunkIndex < chunkSize; ++chunkIndex)
            {
                int64_t i = chunkStart + chunkIndex;
                map<int64_t, FiberFractions> outRow;
                sparsityAccum += float(accum[chunkIndex].m_indices.size()) / rowLength;
                for (int64_t j = 0; j < int64_t(accum[chunkIndex].m_indices.size()); ++j)
                {
                    double countSum = 0.0f;
                    for (int k = 0; k < 3; ++k)
                        countSum += accum[chunkIndex].m_values[j].m_vals[k + 1];
                    if (countSum / inFiles.size() >= threshold)
                    {
                        auto& outVoxel = outRow[accum[chunkIndex].m_indices[j]];
                        outVoxel.distance = accum[chunkIndex].m_values[j].m_vals[0] / countSum;
                        outVoxel.fiberFractions.resize(3);
                        outVoxel.totalCount = floor(countSum / inFiles.size() + 0.5);
                        for (int k = 0; k < 3; ++k)
                            outVoxel.fiberFractions[k] = accum[chunkIndex].m_values[j].m_vals[k + 1] / countSum;
                    }
                }
                outFile.writeRowSparse(i, outRow);
            }
            float thisSparsity = sparsityAccum / chunkSize;
            if (thisSparsity > sparsityMax) sparsityMax = thisSparsity;
        }
    }
}

void OperationWbsparseAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString outFileName = myParams->getString(1);
    double threshold = -INFINITY; //wbsparse can store negatives, even though tractography won't generate them
    auto thresholdOpt = myParams->getOptionalParameter(2);
    if (thresholdOpt->m_present)
    {
        threshold = thresholdOpt->getDouble(1);
    }
    float memLimitGB = 0.1f; //100MB
    auto memLimitOpt = myParams->getOptionalParameter(3);
    if (memLimitOpt->m_present)
    {
        memLimitGB = float(memLimitOpt->getDouble(1));
        if (memLimitGB <= 0.0f) throw OperationException("memory limit must be positive");
    }
    auto wbsparseInOpts = myParams->getRepeatableParameterInstances(4);
    if (wbsparseInOpts.empty()) throw OperationException("at least one input file is required");
    vector<CaretSparseFile> inFiles(wbsparseInOpts.size());
    CiftiXML outXML;
    CaretSparseFile::ValueType outType = CaretSparseFile::Int32; //quiet the compiler, though it is a false positive
    float fileSparsityMax = -1.0f; //actually means "most non-sparse"...
    //TODO: parallel read (XML parse)?
    for (int64_t i = 0; i < int64_t(wbsparseInOpts.size()); ++i)
    {
        inFiles[i].readFile(wbsparseInOpts[i]->getString(1));
        float thisSparsity = inFiles[i].getFileSparsity();
        if (thisSparsity > fileSparsityMax) fileSparsityMax = thisSparsity;
        if (i == 0)
        {
            outXML = inFiles[i].getCiftiXML();
            outType = inFiles[i].getDatatype();
        } else {
            if (!outXML.approximateMatch(inFiles[i].getCiftiXML()))
            {
                throw OperationException("XML does not match between first file and '" + wbsparseInOpts[i]->getString(1) + "'");
            }
            auto thisType = inFiles[i].getDatatype();
            if (outType == CaretSparseFile::Fibers)
            {
                if (thisType != CaretSparseFile::Fibers)
                    throw OperationException("first file is fibers type, but '" + wbsparseInOpts[i]->getString(1) + "' is not");
            } else {
                if (thisType == CaretSparseFile::Fibers)
                    throw OperationException("first file is a numeric type type, but '" + wbsparseInOpts[i]->getString(1) + "' is fibers type");
                switch (outType)
                {
                    case CaretSparseFile::Float64:
                        break; //can't further promote
                    case CaretSparseFile::Float32:
                        if (thisType == CaretSparseFile::Float64) outType = thisType;
                        break;
                    case CaretSparseFile::Int64:
                        if (thisType != CaretSparseFile::Int32) outType = thisType;
                        break;
                    case CaretSparseFile::Int32:
                        outType = thisType;
                        break;
                    case CaretSparseFile::Fibers:
                        CaretAssert(0);
                        break;
                }
            }
        }
        inFiles[i].forgetMapping(CiftiXML::ALONG_ROW);
        inFiles[i].forgetMapping(CiftiXML::ALONG_COLUMN);
    }
    CaretSparseFileWriter outFile(outFileName, outXML, outType);
    switch (outType)
    {
        case CaretSparseFile::Fibers:
            averageSparse<FiberFractions, FiberFractions>(inFiles, outFile, threshold, memLimitGB);
            break;
        case CaretSparseFile::Float32:
            averageSparse<float, double>(inFiles, outFile, threshold, memLimitGB);
            break;
        case CaretSparseFile::Float64:
            averageSparse<double, long double>(inFiles, outFile, threshold, memLimitGB);
            break;
        case CaretSparseFile::Int32:
            averageSparse<int32_t, double>(inFiles, outFile, threshold, memLimitGB); //because we take the sum first, we want to handle overflow gracefully before we divide
            break;
        case CaretSparseFile::Int64:
            averageSparse<int64_t, long double>(inFiles, outFile, threshold, memLimitGB);
            break;
    }
}
