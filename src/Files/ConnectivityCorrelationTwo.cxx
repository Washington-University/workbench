
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __CONNECTIVITY_CORRELATION_TWO_DECLARE__
#include "ConnectivityCorrelationTwo.h"
#undef __CONNECTIVITY_CORRELATION_TWO_DECLARE__

#include <algorithm>
#include <cmath>
#include <iostream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "dot_wrapper.h"

using namespace caret;

/**
 * \class caret::ConnectivityCorrelationTwo 
 * \brief Correlation and covariance
 * \ingroup Files
 */

/**
 * Create a new instance for correlation and covariance
 * @param settings
 *    The settings for the various operations
 * @param dataSetPointers
 *    Pointers to the first element for each "row" of data.  Typically each pointer is the first 'timepoint' for one 'brainordinate'.
 * @param numberOfDataElements
 *    The number of elements for each of the dataPointers
 * @param dataStride
 *    The offset of each element in one data pointer.  In most cases, the data is contiguous, this value is one.  In instance
 *    where the data is in the columns of a matrix, this value is the number of columns.
 * @param errorMessageOut
 *    Contains information describing the error
 * @return Pointer to new instance or NULL if there is an error.
 */
ConnectivityCorrelationTwo*
ConnectivityCorrelationTwo::newInstance(const AString& ownerName,
                                        const ConnectivityCorrelationSettings& settings,
                                        const std::vector<const float*>& dataSetPointers,
                                        const int64_t numberOfDataElements,
                                        const int64_t dataStride,
                                        AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (dataSetPointers.size() < 2) {
        errorMessageOut.appendWithNewLine("There must be at least two sets of data");
    }
    if (numberOfDataElements < 2) {
        errorMessageOut.appendWithNewLine("There must be at least two data elements");
    }
    if (dataStride < 1) {
        errorMessageOut.appendWithNewLine("The data stride must be at least one");
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        return NULL;
    }
    
    return new ConnectivityCorrelationTwo(ownerName,
                                          settings,
                                          dataSetPointers,
                                          numberOfDataElements,
                                          dataStride);
}


/**
 * Constructor.
 * @param ownerName
 *    Name of file that owns this instance
 * @param settings
 *    The settings for the various operations
 * @param dataSetPointers
 *    Pointers to the first element for each "row" of data.  Typically each pointer is the first 'timepoint' for one 'brainordinate'.
 * @param numberOfDataElements
 *    The number of elements for each of the dataPointers
 * @param dataStride
 *    The offset of each element in one data pointer.  In most cases, the data is contiguous, this value is one.  In instance
 *    where the data is in the columns of a matrix, this value is the number of columns.
 */
ConnectivityCorrelationTwo::ConnectivityCorrelationTwo(const AString& ownerName,
                                                       const ConnectivityCorrelationSettings& settings,
                                                       const std::vector<const float*>& dataSetPointers,
                                                       const int64_t numberOfDataElements,
                                                       const int64_t dataStride)
: CaretObject(),
m_ownerName(ownerName),
m_settings(settings),
m_numberOfDataSets(dataSetPointers.size()),
m_numberOfDataElements(numberOfDataElements),
m_dataStride(dataStride)
{
    m_dataSets.resize(m_numberOfDataSets,
                      NULL);

#pragma omp CARET_PARFOR schedule(dynamic)
    for (int32_t dataSetIndex = 0; dataSetIndex < m_numberOfDataSets; dataSetIndex++) {
        CaretAssertVectorIndex(dataSetPointers, dataSetIndex);
        const float* dataPtr(dataSetPointers[dataSetIndex]);
        
        float mean(0.0);
        float sqrtSumSquared(0.0);
        
        computeMeanAndSumSquared(dataPtr,
                                 numberOfDataElements,
                                 dataStride,
                                 mean,
                                 sqrtSumSquared);
        
        m_dataSets[dataSetIndex] = new DataSet(dataSetIndex,
                                               dataPtr,
                                               m_numberOfDataElements,
                                               m_dataStride,
                                               mean,
                                               sqrtSumSquared);
    }

    if (m_debugFlag) {
        printDebugData();
    }
}

/**
 * Compute the mean and the square root of sum squared for the given data
 * @param dataPtr
 *    Pointer to data
 * @param numberOfDataElements
 *    Number of elements in data
 * @param dataStride
 *    The offset of each element in one data pointer.  In most cases, the data is contiguous, this value is one.  In instance
 *    where the data is in the columns of a matrix, this value is the number of columns.
 * @param meanOut
 *    Output with mean
 * @param sqrtSumSquaredOut
 *    Output with square root of sum squared
 */
void
ConnectivityCorrelationTwo::computeMeanAndSumSquared(const float* dataPtr,
                                                     const int64_t numberOfDataElements,
                                                     const int64_t dataStride,
                                                     float& meanOut,
                                                     float& sqrtSumSquaredOut) const
{
    /*
     * NOTE: Do not use OpenMP here.  OpenMP is used
     * in the method that calls this method.
     */
    double sum(0.0);
    double sumSQ(0.0);
    for (int64_t j = 0; j < numberOfDataElements; j++) {
        const int64_t offset(j * dataStride);
        const float d(dataPtr[offset]);
        sum   += d;
        sumSQ += (d * d);
    }
    
    meanOut = (sum / static_cast<float>(numberOfDataElements));
    const float sumSquared(m_settings.isCorrelationNoDemeanEnabled()
                           ? sumSQ
                           : (sumSQ - (m_numberOfDataElements * meanOut * meanOut)));
    sqrtSumSquaredOut = (std::sqrt(sumSquared));
}


/**
 * Destructor.
 */
ConnectivityCorrelationTwo::~ConnectivityCorrelationTwo()
{
    for (DataSet* ds : m_dataSets) {
        delete ds;
    }
    m_dataSets.clear();
}

/**
 * @retrurn The correlation settings
 */
const ConnectivityCorrelationSettings*
ConnectivityCorrelationTwo::getSettings() const
{
    return &m_settings;
}

/**
 * Compute correlation/covariance for the given data set indices and output the
 * average of the computations.
 * @param dataSetIndices
 *    Index of the data set
 * @param dataOut
 *    Output with computed data.  Number of elements is same length as the
 *    Number of data sets.
 */
void
ConnectivityCorrelationTwo::computeAverageForDataSetIndices(const std::vector<int64_t> dataSetIndices,
                                                            std::vector<float>& dataOut) const
{
    const int64_t numData(dataOut.size());
    if ((numData != m_numberOfDataSets)
        || (numData < 1)) {
        CaretLogSevere("Incorrection number of data="
                       + AString::number(numData)
                       + " but should be="
                       + AString::number(m_numberOfDataSets)
                       + " for "
                       + m_ownerName);
        std::fill(dataOut.begin(),
                  dataOut.end(),
                  0.0);
        return;
    }
    
    if (dataSetIndices.size() < 1) {
        std::fill(dataOut.begin(),
                  dataOut.end(),
                  0.0);
        return;
    }

    /*
     * Note: OpenMP is not used here.
     * OpenMP is used in 'computeForDataSet()'
     */
    std::vector<double> sum(numData, 0.0);
    std::vector<float> data(numData, 0.0);
    for (int64_t index : dataSetIndices) {
        CaretAssertVectorIndex(m_dataSets,
                               index);
        const DataSet* dataSet(m_dataSets[index]);
        CaretAssert(dataSet);
        computeForDataSet(*dataSet,
                          data);
        
        /*
         * Using OpenMP on a loop like this showed
         * no reduction in time
         */
        for (int64_t i = 0; i < numData; i++) {
            CaretAssertVectorIndex(data, i);
            CaretAssertVectorIndex(sum, i);
            sum[i] += data[i];
        }
    }
    
    const double numIndicesFloat(dataSetIndices.size());
    for (int64_t i = 0; i < numData; i++) {
        CaretAssertVectorIndex(dataOut, i);
        CaretAssertVectorIndex(sum, i);
        dataOut[i] = sum[i] / numIndicesFloat;
    }
}


/**
 * Compute correlation/covariance for the given data set index to all other data sets
 * @param dataSetIndex
 *    Index of the data set
 * @param dataOut
 *    Output with computed data.  Number of elements is same length as the
 *    Number of data sets.
 */
void
ConnectivityCorrelationTwo::computeForDataSetIndex(const int64_t dataSetIndex,
                                                   std::vector<float>& dataOut) const
{
    CaretAssertVectorIndex(m_dataSets,
                           dataSetIndex);
    const DataSet* dataSet(m_dataSets[dataSetIndex]);
    CaretAssert(dataSet);
    computeForDataSet(*dataSet,
                      dataOut);
}

/**
 * Compute correlation/covariance for the given data set to all other data sets
 * @param dataSet
 *    The data set
 * @param dataOut
 *    Output with computed data.  Number of elements is same length as the
 *    Number of data sets.
 */
void
ConnectivityCorrelationTwo::computeForDataSet(const DataSet& dataSet,
                                              std::vector<float>& dataOut) const
{
    if (m_numberOfDataSets < static_cast<int64_t>(dataOut.size())) {
        CaretAssertMessage(0, "Shrinking dataOut, this is probably wrong");
    }
        
    dataOut.resize(m_numberOfDataSets);
    std::fill(dataOut.begin(),
              dataOut.end(),
              0.0);
     
    bool correlationModeFlag(false);
    switch (m_settings.getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            correlationModeFlag = true;
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
            break;
    }

#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t i = 0; i < m_numberOfDataSets; i++) {
        if (correlationModeFlag
            && (i == dataSet.m_dataSetIndex)) {
            /* Don't need to compute correlation with 'self' */
            dataOut[i] = 1.0;
        }
        else {
            const DataSet* otherDataSet(m_dataSets[i]);
            CaretAssert(otherDataSet);
            dataOut[i] = computeForDataSets(dataSet,
                                            *otherDataSet);
        }
    }
}

/**
 * Compute between the two given data sets
 * @param a
 *    First data set
 * @param b
 *    Second data set
 * @return
 *    The data value computed from the two given data sets
 */
float
ConnectivityCorrelationTwo::computeForDataSets(const DataSet& a,
                                               const DataSet& b) const
{
    CaretAssert(a.m_numDataElements == b.m_numDataElements);
    CaretAssert(a.m_numDataElements >= 1);

    float value(0.0);
    
    switch (m_settings.getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
        {
            double xySum(0.0);
            
            if ((a.m_dataStride == 1)
                && (b.m_dataStride == 1)) {
                /*
                 * "dsdot" requires contiguous data
                 */
                xySum = dsdot(a.m_dataElements,
                              b.m_dataElements,
                              m_numberOfDataElements);
            }
            else {
                for (int64_t i = 0; i < a.m_numDataElements; i++) {
                    xySum += (a.get(i) * b.get(i));
                }
            }
            
            const double ssxy(m_settings.isCorrelationNoDemeanEnabled()
                              ? xySum
                              : (xySum - (m_numberOfDataElements
                                          * a.m_mean
                                          * b.m_mean)));
            
            const double denom(a.m_sqrtSumSquared * b.m_sqrtSumSquared);
            if (denom != 0.0) {
                value = (ssxy / denom);

                if (m_settings.isCorrelationFisherZEnabled()) {
                    if (value > 0.999999) value = 0.999999;   /*prevent inf */
                    if (value < -0.999999) value = -0.999999; /*prevent -inf*/
                    value = 0.5 * std::log((1 + value) / (1 - value));
                }
                else {
                    if (value > 1.0) value = 1.0; /*don't output anything silly*/
                    if (value < -1.0) value = -1.0;
                }
            }
        }
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
        {
            double sum(0.0);
            for (int64_t i = 0; i < a.m_numDataElements; i++) {
                sum += ((a.get(i) - a.m_mean)
                        * (b.get(i) - b.m_mean));
            }

            value = (sum / static_cast<double>(a.m_numDataElements));
        }
            break;
    }
    
    return value;
}

void
ConnectivityCorrelationTwo::printDebugData()
{
    std::cout << "ConnectivityCorrelationTwo:" << std::endl;
    std::cout << "   Number of data sets: " << m_dataSets.size() << std::endl;
    const int32_t numToPrint(std::min(50,
                                      static_cast<int32_t>(m_dataSets.size())));
    for (int32_t i = 0; i < numToPrint; i++) {
        const DataSet* ds(m_dataSets[i]);
        CaretAssert(ds);
        std::cout << "   " << i << "u=" << ds->m_mean
        << ", SS=" << ds->m_sqrtSumSquared << std::endl;
    }
}

