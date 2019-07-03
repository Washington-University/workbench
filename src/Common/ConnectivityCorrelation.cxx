
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include <cmath>

#define __CONNECTIVITY_CORRELATION_DECLARE__
#include "ConnectivityCorrelation.h"
#undef __CONNECTIVITY_CORRELATION_DECLARE__

#include "CaretAssert.h"
#include "CaretOMP.h"
#include "dot_wrapper.h"

using namespace caret;



/**
 * \class caret::ConnectivityCorrelation
 * \brief Computes connectivity correlations
 * \ingroup Common
 */

/**
 * Constructor for data in one 'chunk' of memory
 *
 * @param data
 *    Points to first data element in first group
 * @param groupCount
 *    Number of groups
 * @param groupStride
 *    Element offset between two consecutive groups (from first element in first
 *    group to first element in second group, and so on)
 * @param groupDataCount
 *    Number of data elements in a group
 * @param groupDataStride
 *    Element offset between to consecutive data values within a group
 *    Use a value of "1" for contiguous data
 *
 * Example: Cifti file with data in a matrix.  Each row contains
 * data (timepoints) for one group
 *     data            = pointer to data
 *     groupCount      = number of rows
 *     groupStride     = number of columns
 *     groupDataCount  = number of columns
 *     groupDataStride = 1
 *
 * Example: Nifti file with time-series data.  Each 'brick' contains one timepoint
 *          for all groups (voxels).   4D matrix
 *     data            = pointer to data
 *     groupCount      = dim[0] * dim[1] * dim[2] (sizeof a 'brick')
 *     groupStride     = 1
 *     groupDataCount  = number of timepoints
 *     groupDataStride = dim[0] * dim[1] * dim[3] (sizeof a 'brick')
 */
ConnectivityCorrelation::ConnectivityCorrelation(const float* data,
                                                 const int64_t groupCount,
                                                 const int64_t groupStride,
                                                 const int64_t groupDataCount,
                                                 const int64_t groupDataStride)
: m_dataCount(groupDataCount)
{
    CaretAssert(data);
    CaretAssert(groupStride >= 1);
    CaretAssert(groupDataStride >= 1);
    
    for (int64_t i = 0; i < groupCount; i++) {
        const float* dataPointer = data + (i * groupStride);
        addDataGroup(dataPointer,
                     groupDataCount,
                     groupDataStride);
    }
    CaretAssert(static_cast<int64_t>(m_groups.size()) == groupCount);
    
    m_contiguousDataFlag = (groupDataStride == 1);
}

/**
 * Constructor for data in separate 'chunk' for each data group
 *
 * @param dataGroups
 *    Each element is pointer to first element in each group
 * @param groupCount
 *    Number of groups (number of elements in 'dataGroups')
 * @param groupDataCount
 *    Number of data elements in a group
 * @param groupDataStrides
 *    Each element is offset between consecutive data values within each group
 *    Must contain 'groupCount' elements.
 *    OR may also be NULL which indicates elements are contigous in all groups
 *
 * Example: GIFTI file that stores each 'map' in a separate chunk of memory
 *     dataGroups       = pointers to data in each GIFTI data array
 *     groupCount       = number of rows
 *     groupStride      = number of columns
 *     groupDataCount   = number of columns
 *     groupDataStrides = NULL (data elements are consecutive)
 */
ConnectivityCorrelation::ConnectivityCorrelation(const float* dataGroups[],
                                                 const int64_t groupCount,
                                                 const int64_t groupDataCount,
                                                 const int64_t groupDataStrides[])
: m_dataCount(groupDataCount)
{
    CaretAssert(dataGroups);
    m_contiguousDataFlag = true;
    
    const bool haveStride(groupDataStrides != NULL);
    for (int64_t i = 0; i < groupCount; i++) {
        const int64_t dataStride(haveStride
                                 ? groupDataStrides[i]
                                 : 1);
        CaretAssert(dataStride >= 1);
        CaretAssert(dataGroups[i]);
        
        addDataGroup(dataGroups[i],
                     groupDataCount,
                     dataStride);
        
        if (dataStride != 1) {
            m_contiguousDataFlag = false;
        }
    }
    CaretAssert(static_cast<int64_t>(m_groups.size()) == groupCount);
}

/**
 * Destructor.
 */
ConnectivityCorrelation::~ConnectivityCorrelation()
{
}

/**
 * Compute data's mean and sum-squared
 *
 * @param data
 *     Data on which mean and sum-squared are calculated
 * @param dataCount
 *     Number of elements in data.
 * @param dataStride
 *     Element offset between consecutive data values
 */
void
ConnectivityCorrelation::addDataGroup(const float* data,
                                      const int64_t dataCount,
                                      const int64_t dataStride)
{
    CaretAssert(data);
    
    float mean(0.0f);
    float sqrtSumSquared(0.0f);
    computeDataMeanAndSumSquared(data,
                                 dataCount,
                                 dataStride,
                                 mean,
                                 sqrtSumSquared);
    
    std::unique_ptr<GroupData> gd(new GroupData(data,
                                                dataStride,
                                                mean,
                                                sqrtSumSquared));
    m_groups.push_back(std::move(gd));
}

/**
 * Get correlation from the given group to all other groups
 *
 * @param groupIndex
 *     Index of group to correlate to all other groups
 * @param dataOut
 *     Output with correlation values. 
 */
void
ConnectivityCorrelation::getCorrelationForGroup(const int64_t groupIndex,
                                                std::vector<float>& dataOut)
{
    CaretAssertVectorIndex(m_groups, groupIndex);
    
    if (m_dataCount > 0) {
        const int64_t numGroups = static_cast<int64_t>(m_groups.size());
        dataOut.resize(numGroups);
#pragma omp CARET_PARFOR schedule(dynamic)
        for (int64_t iGroup = 0; iGroup < numGroups; iGroup++) {
            CaretAssertVectorIndex(dataOut, iGroup);
            if (m_contiguousDataFlag) {
                dataOut[iGroup] = correlationContiguousData(groupIndex,
                                                            iGroup);
            }
            else {
                dataOut[iGroup] = correlationNonContiguousData(groupIndex,
                                                               iGroup);
            }
        }
    }
}

/**
 * Get the correlation coefficient for the two given groups
 * that contain CONTIGOUS data
 *
 * @param fromGroupIndex
 *     Index of a group
 * @param toGroupIndex
 *     Index of a second group.
 */
float
ConnectivityCorrelation::correlationContiguousData(const int64_t fromGroupIndex,
                                                   const int64_t toGroupIndex) const
{
    CaretAssertVectorIndex(m_groups, fromGroupIndex);
    CaretAssertVectorIndex(m_groups, toGroupIndex);
    
    const GroupData* fromGroup = m_groups[fromGroupIndex].get();
    const GroupData* toGroup   = m_groups[toGroupIndex].get();
    
    double xySum = dsdot(fromGroup->m_data,
                         toGroup->m_data,
                         m_dataCount);
    
    const double ssxy = xySum - (m_dataCount * fromGroup->m_mean * toGroup->m_mean);
    
    float correlationCoefficient = 0.0;
    if ((fromGroup->m_sqrt_ssxx > 0.0)
        && (toGroup->m_sqrt_ssxx > 0.0)) {
        correlationCoefficient = (ssxy / (fromGroup->m_sqrt_ssxx * toGroup->m_sqrt_ssxx));
    }
    return correlationCoefficient;
}

/**
 * Get the correlation coefficient for the two given groups
 * that contain NON-CONTIGOUS data
 *
 * @param fromGroupIndex
 *     Index of a group
 * @param toGroupIndex
 *     Index of a second group.
 */
float
ConnectivityCorrelation::correlationNonContiguousData(const int64_t fromGroupIndex,
                                                      const int64_t toGroupIndex) const
{
    CaretAssertVectorIndex(m_groups, fromGroupIndex);
    CaretAssertVectorIndex(m_groups, toGroupIndex);
    
    const GroupData* fromGroup = m_groups[fromGroupIndex].get();
    const GroupData* toGroup   = m_groups[toGroupIndex].get();
    
    const float* fromData    = fromGroup->m_data;
    const int64_t fromStride = fromGroup->m_dataStride;
    const float* toData      = toGroup->m_data;
    const int64_t toStride   = toGroup->m_dataStride;
    
    int64_t fromOffset(0);
    int64_t toOffset(0);
    double xySum(0.0);
    for (int32_t i = 0; i < m_dataCount; i++) {
        xySum += (fromData[fromOffset] * toData[toOffset]);
        fromOffset += fromStride;
        toOffset   += toStride;
    }
    
    const double ssxy = xySum - (m_dataCount * fromGroup->m_mean * toGroup->m_mean);
    
    float correlationCoefficient = 0.0;
    if ((fromGroup->m_sqrt_ssxx > 0.0)
        && (toGroup->m_sqrt_ssxx > 0.0)) {
        correlationCoefficient = (ssxy / (fromGroup->m_sqrt_ssxx * toGroup->m_sqrt_ssxx));
    }
    return correlationCoefficient;
}

/**
 * Compute data's mean and sum-squared
 *
 * @param data
 *     Data on which mean and sum-squared are calculated
 * @param dataCount
 *     Number of elements in data.
 * @param dataStride
 *     Element offset between consecutive data values
 * @param meanOut
 *     Output with mean of data.
 * @param sqrtSquaredOut
 *     Output with square root of (sum-squared).
 */
void
ConnectivityCorrelation::computeDataMeanAndSumSquared(const float* data,
                                                      const int64_t dataCount,
                                                      const int64_t dataStride,
                                                      float& meanOut,
                                                      float& sqrtSquaredOut) const
{
    CaretAssert(data);
    CaretAssert(dataStride >= 1);
    
    meanOut = 0.0;
    sqrtSquaredOut = 0.0;
    if (dataCount <= 0) {
        return;
    }
    
    double sum = 0.0;
    double sumSquared = 0.0;
    
    int64_t offset = 0;
    for (int64_t i = 0; i < dataCount; i++) {
//        const float d = data[i];
        const float d = data[offset];
        sum        += d;
        sumSquared += (d * d);
        offset += dataStride;
    }
    
    meanOut = (sum / dataCount);
    const float ssxx = (sumSquared - (dataCount * meanOut * meanOut));
    sqrtSquaredOut = std::sqrt(ssxx);
}

