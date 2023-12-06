
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
#include "ConnectivityCorrelationSettings.h"
#undef __CONNECTIVITY_CORRELATION_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "dot_wrapper.h"

using namespace caret;



/**
 * \class caret::ConnectivityCorrelation
 * \brief Computes connectivity correlations
 * \ingroup Files
 *
 * Correlation from https://en.wikipedia.org/wiki/Pearson_product-moment_correlation_coefficient
 */

/**
 * Create new instance for data that is in one contiguous "chunk" of memory
 *
 * @param settings
 *    The settings for the correlation and covariance operations
 * @param data
 *    Points to first timepoint for first brainordinate
 * @param numberOfBrainordinates
 *    Number of brainordinates
 * @param nextBrainordinateStride
 *    Element offset between two consecutive brainordinates (eg: offset from first value for FIRST brainordinate
 *    to first value for SECOND brainordinate, and so on)
 * @param numberOfTimePoints
 *    Number of timepoints for each brainordinate
 * @param nextTimePointStride
 *    Offset between two consecutive timepoints for a brainordinate
 *    Use a value of "1" for contiguous data
 * @param errorMessageOut
 *    Contains error information if NULL is returned
 * @return
 *    Pointer to new instance or NULL if there is an error.
 *
 * Example: Nifti file with time-series data.  Each 'brick' contains one timepoint
 *          for all brainordinates.   Brainordinates are "interleaved"
 *     data                    = pointer to data
 *     numberOfBrainordinates  = dim[0] * dim[1] * dim[2] (sizeof a 'brick')
 *     nextBrainordinateStride = 1
 *     numberOfTimePoints      = number of timepoints
 *     nextTimePointStride     = dim[0] * dim[1] * dim[3] (sizeof a 'brick')
 */
ConnectivityCorrelation*
ConnectivityCorrelation::newInstance(const ConnectivityCorrelationSettings& settings,
                                     const float* data,
                                     const int64_t numberOfBrainordinates,
                                     const int64_t nextBrainordinateStride,
                                     const int64_t numberOfTimePoints,
                                     const int64_t nextTimePointStride,
                                     AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    std::vector<const float*> brainordinateDataPointers;
    for (int64_t i = 0; i < numberOfBrainordinates; i++) {
        const int64_t offset = i * nextBrainordinateStride;
        brainordinateDataPointers.push_back(&data[offset]);
    }
    
    CaretAssert(numberOfBrainordinates == static_cast<int64_t>(brainordinateDataPointers.size()));
    
    /*
     * While either of the other two factory methods can be called, the new instance
     * for brainordinates is faster when each brainordinates timepoint's are in a
     * contigous section of memory (nextTimePointStride == 1).
     */
    ConnectivityCorrelation* instanceOut = newInstanceBrainordinates(settings,
                                                                     brainordinateDataPointers,
                                                                     numberOfTimePoints,
                                                                     nextTimePointStride,
                                                                     errorMessageOut);
    
    return instanceOut;
}

/**
 * Create a new instance where each chunk of memory contains all timepoints for one brainordinate.
 *
 * @param settings
 *    The settings for the correlation and covariance operations
 * @param brainordinateDataPointers
 *    Each element points to all timepoints for one brainordinate
 * @param numberOfTimePoints
 *    Number of timepoints for each brainordinate
 * @param nextTimePointStride
 *    Offset between two consecutive timepoints for a brainordinate
 *    Use a value of "1" for contiguous data
 * @param errorMessageOut
 *    Contains error information if NULL is returned
 * @return
 *    Pointer to new instance or NULL if there is an error.
 */
ConnectivityCorrelation*
ConnectivityCorrelation::newInstanceBrainordinates(const ConnectivityCorrelationSettings& settings,
                                                   const std::vector<const float*>& brainordinateDataPointers,
                                                   const int64_t numberOfTimePoints,
                                                   const int64_t nextTimePointStride,
                                                   AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    ConnectivityCorrelation* instanceOut = ((nextTimePointStride == 1)
                                            ? new ConnectivityCorrelation(settings,
                                                                          DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA)
                                            : new ConnectivityCorrelation(settings,
                                                                          DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA));
    const bool validFlag = instanceOut->initializeWithBrainordinates(brainordinateDataPointers,
                                                                     numberOfTimePoints,
                                                                     nextTimePointStride,
                                                                     errorMessageOut);
    if ( ! validFlag) {
        delete instanceOut;
        instanceOut = NULL;
    }
    
    return instanceOut;
}

/**
 * Create a new instance where each chunk of memory contains one timepoint for all brainordinates
 *
 * @param settings
 *    The settings for the correlation and covariance operations
 * @param timePointDataPointers
 *    Each element points to all brainordinates for one timepoint
 * @param numberOfBrainordinates
 *    Number of brainordinates
 * @param nextBrainordinateStride
 *    Element offset between two consecutive brainordinates (eg: offset from first value for FIRST brainordinate
 *    to first value for SECOND brainordinate, and so on)
 * @param errorMessageOut
 *    Contains error information if NULL is returned
 * @return
 *    Pointer to new instance or NULL if there is an error.
 *
 * Example: GIFTI functional file (Metric) with time-series data.  Each map contains one timepoint
 *          for all brainordinates.
 *     timePointDataPointers   = pointer to each map's data
 *     numberOfBrainordinates  = Number of vertices in the GIFTI file
 *     nextBrainordinateStride = 1
 *     numberOfTimePoints      = number of maps in file (each map is one time point)
 */
ConnectivityCorrelation*
ConnectivityCorrelation::newInstanceTimePoints(const ConnectivityCorrelationSettings& settings,
                                               const std::vector<const float*>& timePointDataPointers,
                                               const int64_t numberOfBrainordinates,
                                               const int64_t nextBrainordinateStride,
                                               AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    ConnectivityCorrelation* instanceOut = new ConnectivityCorrelation(settings,
                                                                       DataTypeEnum::TIMEPOINTS);
    const bool validFlag = instanceOut->initializeWithTimePoints(timePointDataPointers,
                                                                 numberOfBrainordinates,
                                                                 nextBrainordinateStride,
                                                                 errorMessageOut);
    if ( ! validFlag) {
        delete instanceOut;
        instanceOut = NULL;
    }
    
    return instanceOut;
}

/**
 * Create a new instance for correlating data associated with parcels
 * @param settings
 *    The settings for the correlation and covariance operations
 * @param parcelDataPointers
 *    Pointers to the first data element for each parcel (assumes all elements are consecutive in each parcel)
 * @param numberOfDataElementsPerParcel
 *    Number of data elements for each parcel
 * @param errorMessageOut
 *    Contains error info if failure to create new instance
 * @return
 *    Pointer to new instnace or NULL if there is an error
 */
ConnectivityCorrelation*
ConnectivityCorrelation::newInstanceParcels(const ConnectivityCorrelationSettings& settings,
                                            const std::vector<const float*> parcelDataPointers,
                                            const int64_t numberOfDataElementsPerParcel,
                                            AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    ConnectivityCorrelation* instanceOut = new ConnectivityCorrelation(settings,
                                                                       DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA);
    const int64_t nextTimePointStride(1);
    const bool validFlag(instanceOut->initializeWithBrainordinates(parcelDataPointers,
                                                                   numberOfDataElementsPerParcel,
                                                                   nextTimePointStride,
                                                                   errorMessageOut));
    if ( ! validFlag) {
        delete instanceOut;
        instanceOut = NULL;
    }

    return instanceOut;
}

/**
 * Constructor
 *
 * @param settings
 *    The settings for the correlation and covariance operations
 * @param dataType
 *     Type of data (brainordinate or timepoint)
 *
 */
ConnectivityCorrelation::ConnectivityCorrelation(const ConnectivityCorrelationSettings& settings,
                                                 const DataTypeEnum dataType)
: m_settings(settings),
m_dataType(dataType)
{
    m_fisherZFlag = false;
    m_noDemeanFlag = false;
    switch (m_settings.getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            m_fisherZFlag  = m_settings.isCorrelationFisherZEnabled();
            m_noDemeanFlag = m_settings.isCorrelationNoDemeanEnabled();
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
            break;
    }
}

/**
 * Destructor.
 */
ConnectivityCorrelation::~ConnectivityCorrelation()
{
}

/**
 * Initialize a new instance where a chunk of memory contains all timepoints for one brainordinate.
 *
 * @param brainordinateDataPointers
 *    Each element points to all timepoints for one brainordinate
 * @param numberOfTimePoints
 *    Number of timepoints for each brainordinate
 * @param nextTimePointStride
 *    Offset between two consecutive timepoints for a brainordinate
 *    Use a value of "1" for contiguous data
 * @param errorMessageOut
 *    Contains error information if NULL is returned
 * @return
 *    Pointer to new instance or NULL if there is an error.
 */
bool
ConnectivityCorrelation::initializeWithBrainordinates(const std::vector<const float*>& brainordinateDataPointers,
                                                      const int64_t numberOfTimePoints,
                                                      const int64_t nextTimePointStride,
                                                      AString& errorMessageOut)
{
    switch (m_dataType) {
        case DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA:
            break;
        case DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA:
            break;
        case DataTypeEnum::INVALID:
            CaretAssert(0);
            break;
        case DataTypeEnum::TIMEPOINTS:
            CaretAssert(0);
            break;
    }
    
    m_numberOfTimePoints = numberOfTimePoints;

    CaretAssert(brainordinateDataPointers.size() >= 2);
    CaretAssert(m_numberOfTimePoints >= 2);
    CaretAssert(nextTimePointStride >= 1);
    
    m_numberOfBrainordinates =  static_cast<int64_t>(brainordinateDataPointers.size());
    if (m_numberOfBrainordinates < 2) {
        errorMessageOut.appendWithNewLine("There must be at least two brainordinates");
    }
    if (numberOfTimePoints < 2) {
        errorMessageOut.appendWithNewLine("There must be at least two time points");
    }
    if (nextTimePointStride < 1) {
        errorMessageOut.appendWithNewLine("TimePoint stride must be at least one");
    }
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    for (int64_t i = 0; i < m_numberOfBrainordinates; i++) {
        CaretAssertVectorIndex(brainordinateDataPointers, i);
        const float* dataPointer = brainordinateDataPointers[i];
        CaretAssert(dataPointer);
        std::unique_ptr<BrainordinateData> gd(new BrainordinateData(dataPointer,
                                                                    nextTimePointStride));
        m_brainordinateData.push_back(std::move(gd));
    }
    CaretAssert(static_cast<int64_t>(m_brainordinateData.size()) == m_numberOfBrainordinates);
    
    computeBrainordinateMeanAndSumSquared();
    
    /*printDebugData();*/
    
    return true;
}

/**
 * Initialize a new instance where a chunk of memory contains all timepoints for one brainordinate.
 *
 * @param timePointDataPointers
 *    Each element points to all brainordinates for one timepoint
 * @param numberOfBrainordinates
 *    Number of brainordinates
 * @param nextBrainordinateStride
 *    Element offset between two consecutive brainordinates (eg: offset from first value for FIRST brainordinate
 *    to first value for SECOND brainordinate, and so on)
 * @param errorMessageOut
 *    Contains error information if NULL is returned
 * @return
 *    Pointer to new instance or NULL if there is an error.
 */
bool
ConnectivityCorrelation::initializeWithTimePoints(const std::vector<const float*>& timePointDataPointers,
                                                  const int64_t numberOfBrainordinates,
                                                  const int64_t nextBrainordinateStride,
                                                  AString& errorMessageOut)
{
    CaretAssert(m_dataType == DataTypeEnum::TIMEPOINTS);
    m_numberOfBrainordinates = numberOfBrainordinates;
    m_numberOfTimePoints = static_cast<int64_t>(timePointDataPointers.size());
    CaretAssert(m_numberOfTimePoints >= 2);
    CaretAssert(nextBrainordinateStride >= 1);
    
    if (m_numberOfBrainordinates < 2) {
        errorMessageOut.appendWithNewLine("There must be at least two brainordinates");
    }
    if (m_numberOfTimePoints < 2) {
        errorMessageOut.appendWithNewLine("There must be at least two time points");
    }
    if (nextBrainordinateStride < 1) {
        errorMessageOut.appendWithNewLine("Brainordinate stride must be at least one");
    }
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    for (int64_t i = 0; i < m_numberOfTimePoints; i++) {
        CaretAssertVectorIndex(timePointDataPointers, i);
        const float* dataPointer = timePointDataPointers[i];
        CaretAssert(dataPointer);
        
        std::unique_ptr<TimePointData> td(new TimePointData(dataPointer,
                                                            nextBrainordinateStride));
        m_timePointData.push_back(std::move(td));
    }
    
    computeBrainordinateMeanAndSumSquared();

    return true;
}

/**
 * Get the correlation for the given brainordinate data
 * @param brainordinateData
 *    The data that should contain "number of timepoints" elements
 * @param dataOut
 *    Output with correlation
 */
void
ConnectivityCorrelation::getCorrelationForBrainordinateData(const std::vector<float>& brainordinateData,
                                                            std::vector<float>& dataOut)
{
    dataOut.resize(m_numberOfBrainordinates);
    std::fill(dataOut.begin(), dataOut.end(), 0.0);
    
    const int64_t numTimePointsInData(brainordinateData.size());
    if (numTimePointsInData != m_numberOfTimePoints) {
        CaretLogSevere(AString::number(numTimePointsInData)
                       + " points in data but number of timepoints should be "
                       + AString::number(m_numberOfTimePoints));
        return;
    }
    
    double sum(0.0);
    double sumSquared(0.0);
    for (int64_t j = 0; j < m_numberOfTimePoints; j++) {
        CaretAssertVectorIndex(brainordinateData, j);
        const float d = brainordinateData[j];
        sum        += d;
        sumSquared += (d * d);
    }
    const float mean(computeMean(sum, m_numberOfTimePoints));
    const float ssxxSquared = (sumSquared - (m_numberOfTimePoints * mean * mean));
    const float ssxx = std::sqrt(ssxxSquared);
    
    const float* dataPtr = &brainordinateData[0];
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int32_t iBrain = 0; iBrain < m_numberOfBrainordinates; iBrain++) {
        CaretAssertVectorIndex(dataOut, iBrain);
        switch (m_dataType) {
            case DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA:
                dataOut[iBrain] = correlationBrainordinateContiguousDataAux(dataPtr,
                                                                            mean,
                                                                            ssxx,
                                                                            iBrain);
                break;
            case DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA:
                dataOut[iBrain] = correlationBrainordinateNonContiguousDataAux(dataPtr,
                                                                               mean,
                                                                               ssxx,
                                                                               iBrain);
                break;
            case DataTypeEnum::INVALID:
                CaretAssert(0);
                break;
            case DataTypeEnum::TIMEPOINTS:
                dataOut[iBrain] = correlationTimePointDataAux(dataPtr,
                                                              mean,
                                                              ssxx,
                                                              iBrain);
                break;
        }
    }
}

/**
 * @return The settings
 */
const
ConnectivityCorrelationSettings*
ConnectivityCorrelation::getSettings() const
{
    return &m_settings;
}

/**
 * Compute the mean and perhaps override with no-demean option
 * @param dataSum
 *    Sum of the items
 * @param numberOfItems
 *    Number of items
 * @return Mean value or zero if no-demean
 */
float
ConnectivityCorrelation::computeMean(const float dataSum,
                                     const float numberOfItems) const
{
    float meanOut(0.0);
    if (numberOfItems > 0.0) {
        if ( ! m_noDemeanFlag) {
            meanOut = dataSum / numberOfItems;
        }
    }
    return meanOut;
}

/**
 * Get correlation from the given brainordinate ROI to all other brainordinates
 *
 * @param brainordinateIndices
 *     Index of brainordinates in the ROI to correlate to all other brainordinates
 * @param dataOut
 *     Output with correlation values.
 */
void
ConnectivityCorrelation::getCorrelationForBrainordinateROI(const std::vector<int64_t>& brainordinateIndices,
                                                           std::vector<float>& dataOut)
{
    dataOut.resize(m_numberOfBrainordinates);
    std::fill(dataOut.begin(), dataOut.end(), 0.0);
    
    const int64_t numBrainordinatesInROI = static_cast<int64_t>(brainordinateIndices.size());
    if (m_numberOfTimePoints > 1) {
        if (numBrainordinatesInROI > 0) {
            std::vector<float> dataAverage(m_numberOfTimePoints, 0.0);

            for (int32_t iTime = 0; iTime < m_numberOfTimePoints; iTime++) {
                double timePointSum(0.0);
                for (int64_t jBrain = 0; jBrain < numBrainordinatesInROI; jBrain++) {
                    CaretAssertVectorIndex(brainordinateIndices, jBrain);
                    timePointSum += getDataValue(brainordinateIndices[jBrain], iTime);
                }
                dataAverage[iTime] = timePointSum / static_cast<double>(numBrainordinatesInROI);
            }

            double sum(0.0);
            double sumSquared(0.0);
            for (int64_t j = 0; j < m_numberOfTimePoints; j++) {
                CaretAssertVectorIndex(dataAverage, j);
                const float d = dataAverage[j];
                sum        += d;
                sumSquared += (d * d);
            }
            const float mean(computeMean(sum, m_numberOfTimePoints));
            const float ssxxSquared = (sumSquared - (m_numberOfTimePoints * mean * mean));
            const float ssxx = std::sqrt(ssxxSquared);
            
            const float* dataPtr = &dataAverage[0];
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int32_t iBrain = 0; iBrain < m_numberOfBrainordinates; iBrain++) {
                CaretAssertVectorIndex(dataOut, iBrain);
                switch (m_dataType) {
                    case DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA:
                        dataOut[iBrain] = correlationBrainordinateContiguousDataAux(dataPtr,
                                                                                    mean,
                                                                                    ssxx,
                                                                                    iBrain);
                        break;
                    case DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA:
                        dataOut[iBrain] = correlationBrainordinateNonContiguousDataAux(dataPtr,
                                                                                       mean,
                                                                                       ssxx,
                                                                                       iBrain);
                        break;
                    case DataTypeEnum::INVALID:
                        CaretAssert(0);
                        break;
                    case DataTypeEnum::TIMEPOINTS:
                        dataOut[iBrain] = correlationTimePointDataAux(dataPtr,
                                                                      mean,
                                                                      ssxx,
                                                                      iBrain);
                        break;
                }
            }
        }
    }
}


/**
 * Get correlation from the given brainordinate to all other brainordinates
 *
 * @param brainordinateIndex
 *     Index of brainordinate to correlate to all other brainordinates
 * @param dataOut
 *     Output with correlation values. 
 */
void
ConnectivityCorrelation::getCorrelationForBrainordinate(const int64_t brainordinateIndex,
                                                        std::vector<float>& dataOut)
{
    CaretAssert(m_numberOfBrainordinates > 1);
    CaretAssert(m_numberOfTimePoints > 1);
    if (m_numberOfTimePoints > 0) {
        if (m_numberOfBrainordinates > 0) {
            dataOut.resize(m_numberOfBrainordinates);
#pragma omp CARET_PARFOR schedule(dynamic)
            for (int64_t i = 0; i < m_numberOfBrainordinates; i++) {
                CaretAssertVectorIndex(dataOut, i);
                switch (m_dataType) {
                    case DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA:
                        dataOut[i] = correlationBrainordinateContiguousData(brainordinateIndex,
                                                                            i);
                        break;
                    case DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA:
                        dataOut[i] = correlationBrainordinateNonContiguousData(brainordinateIndex,
                                                                               i);
                        break;
                    case DataTypeEnum::INVALID:
                        CaretAssert(0);
                        break;
                    case DataTypeEnum::TIMEPOINTS:
                        dataOut[i] = correlationTimePointData(brainordinateIndex,
                                                              i);
                        break;
                }
            }
        }
    }
}

/**
 * Get the correlation coefficient for the two given brainordinates
 * that contain CONTIGOUS data
 *
 * @param fromBrainordinateIndex
 *     Index of a brainordinate
 * @param toBrainordinateIndex
 *     Index of a second brainordinate.
 */
float
ConnectivityCorrelation::correlationBrainordinateContiguousData(const int64_t fromBrainordinateIndex,
                                                                const int64_t toBrainordinateIndex) const
{
    CaretAssert(m_dataType == DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA);
    
    CaretAssertVectorIndex(m_brainordinateData, fromBrainordinateIndex);
    const BrainordinateData* fromData = m_brainordinateData[fromBrainordinateIndex].get();
    CaretAssertVectorIndex(m_meanSSData, fromBrainordinateIndex);
    const BrainordinateMeanSS* fromMeanSS = m_meanSSData[fromBrainordinateIndex].get();
    
    return correlationBrainordinateContiguousDataAux(fromData->m_data,
                                                     fromMeanSS->m_mean,
                                                     fromMeanSS->m_sqrt_ssxx,
                                                     toBrainordinateIndex);
}

/**
 * Get the correlation coefficient for the two given brainordinates
 * that contain CONTIGOUS data
 *
 * @param fromBrainordinateData
 *     Data for the 'from' brainordinate
 * @param fromBrainordinateMean
 *     Mean of the data for the 'from' brainordinate.
 * @param fromBrainordinateSSXX
 *     Sum-squared of the data for the 'from' brainordinate.
 * @param toBrainordinateIndex
 *     Index of the 'to' brainordinate.
 */
float
ConnectivityCorrelation::correlationBrainordinateContiguousDataAux(const float* fromBrainordinateData,
                                                                   const float fromBrainordinateMean,
                                                                   const float fromBrainordinateSSXX,
                                                                   const int64_t toBrainordinateIndex) const
{
    CaretAssert(m_dataType == DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA);
    
    CaretAssertVectorIndex(m_brainordinateData, toBrainordinateIndex);
    const BrainordinateData* toGroup   = m_brainordinateData[toBrainordinateIndex].get();
    CaretAssert(toGroup->m_dataStride == 1);
    
    CaretAssertVectorIndex(m_meanSSData, toBrainordinateIndex);
    const BrainordinateMeanSS* toMeanSS   = m_meanSSData[toBrainordinateIndex].get();
    
    double xySum = dsdot(fromBrainordinateData,
                         toGroup->m_data,
                         m_numberOfTimePoints);
    
    const double ssxy = (m_noDemeanFlag
                         ? xySum
                         : (xySum - (m_numberOfTimePoints * fromBrainordinateMean * toMeanSS->m_mean)));
    
    float correlationCoefficient = 0.0;
    switch (m_settings.getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            if ((fromBrainordinateSSXX > 0.0)
                && (toMeanSS->m_sqrt_ssxx > 0.0)) {
                correlationCoefficient = (ssxy / (fromBrainordinateSSXX * toMeanSS->m_sqrt_ssxx));
            }
            correlationCoefficient = finalizeCoefficient(correlationCoefficient);
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
        {
            xySum = 0.0;
            for (int32_t i = 0; i < m_numberOfTimePoints; i++) {
                xySum += ((fromBrainordinateData[i] - fromBrainordinateMean)
                          * (toGroup->m_data[i] - toMeanSS->m_mean));
            }
            correlationCoefficient = (xySum / static_cast<float>(m_numberOfTimePoints));
        }
//            correlationCoefficient = (xySum / static_cast<float>(m_numberOfTimePoints));
//            CaretAssert(m_numberOfBrainordinates >= 1);
//            correlationCoefficient = (xySum / static_cast<float>(m_numberOfBrainordinates));
            break;
    }
    return correlationCoefficient;
}

/**
 * Get the correlation coefficient for the two given brainordinates
 * that contain NON-CONTIGOUS data
 *
 * @param fromBrainordinateIndex
 *     Index of a brainordinate
 * @param toBrainordinateIndex
 *     Index of a second brainordinate.
 */
float
ConnectivityCorrelation::correlationBrainordinateNonContiguousData(const int64_t fromBrainordinateIndex,
                                                                   const int64_t toBrainordinateIndex) const
{
    CaretAssertVectorIndex(m_brainordinateData, fromBrainordinateIndex);
    
    std::vector<float> data(m_numberOfTimePoints);
    for (int32_t iTime = 0; iTime < m_numberOfTimePoints; iTime++) {
        data[iTime] = getDataValue(fromBrainordinateIndex, iTime);
    }
    
    CaretAssertVectorIndex(m_meanSSData, fromBrainordinateIndex);
    const BrainordinateMeanSS* fromMeanSS = m_meanSSData[fromBrainordinateIndex].get();
    
    const float fromMean(m_noDemeanFlag
                         ? 0.0
                         : fromMeanSS->m_mean);
    return correlationBrainordinateNonContiguousDataAux(&data[0],
                                                        fromMean,
                                                        fromMeanSS->m_sqrt_ssxx,
                                                        toBrainordinateIndex);
}

/**
 * Get the correlation coefficient for the two given brainordinates
 * that contain NON-CONTIGOUS data
 *
 * @param fromBrainordinateData
 *     Data for the 'from' brainordinate
 * @param fromBrainordinateMean
 *     Mean of the data for the 'from' brainordinate.
 * @param fromBrainordinateSSXX
 *     Sum-squared of the data for the 'from' brainordinate.
 * @param toBrainordinateIndex
 *     Index of the 'to' brainordinate.
 */
float
ConnectivityCorrelation::correlationBrainordinateNonContiguousDataAux(const float* fromBrainordinateData,
                                                                      const float fromBrainordinateMean,
                                                                      const float fromBrainordinateSSXX,
                                                                      const int64_t toBrainordinateIndex) const
{
    CaretAssert(m_dataType == DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA);
    
    CaretAssertVectorIndex(m_brainordinateData, toBrainordinateIndex);
    
    const BrainordinateData* toGroup   = m_brainordinateData[toBrainordinateIndex].get();
    
    CaretAssertVectorIndex(m_meanSSData, toBrainordinateIndex);
    const BrainordinateMeanSS* toMeanSS   = m_meanSSData[toBrainordinateIndex].get();
    
    const float* toData      = toGroup->m_data;
    const int64_t toStride   = toGroup->m_dataStride;
    
    int64_t toOffset(0);
    double xySum(0.0);
    for (int32_t i = 0; i < m_numberOfTimePoints; i++) {
        xySum += (fromBrainordinateData[i] * toData[toOffset]);
        toOffset   += toStride;
    }
    
    const double ssxy = (m_noDemeanFlag
                         ? xySum
                         : (xySum - (m_numberOfTimePoints * fromBrainordinateMean * toMeanSS->m_mean)));
    
    float correlationCoefficient = 0.0;
    switch (m_settings.getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            if ((fromBrainordinateSSXX > 0.0)
                && (toMeanSS->m_sqrt_ssxx > 0.0)) {
                correlationCoefficient = (ssxy / (fromBrainordinateSSXX * toMeanSS->m_sqrt_ssxx));
            }
            correlationCoefficient = finalizeCoefficient(correlationCoefficient);
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
            correlationCoefficient = (ssxy / static_cast<float>(m_numberOfTimePoints));
            CaretAssert(m_numberOfBrainordinates >= 1);
            correlationCoefficient = (xySum / static_cast<float>(m_numberOfBrainordinates));
            break;
    }
    return correlationCoefficient;
}

/**
 * Get the correlation coefficient for the two given brainordinates
 * that contain NON-CONTIGOUS data
 *
 * @param fromBrainordinateIndex
 *     Index of a brainordinate
 * @param toBrainordinateIndex
 *     Index of a second brainordinate.
 */
float
ConnectivityCorrelation::correlationTimePointData(const int64_t fromBrainordinateIndex,
                                                  const int64_t toBrainordinateIndex) const
{
    CaretAssert(m_dataType == DataTypeEnum::TIMEPOINTS);

    CaretAssert((fromBrainordinateIndex >= 0) && (fromBrainordinateIndex < m_numberOfBrainordinates));
    CaretAssert((toBrainordinateIndex >= 0) && (toBrainordinateIndex < m_numberOfBrainordinates));
    
    std::vector<float> data(m_numberOfTimePoints);
    for (int32_t iTime = 0; iTime < m_numberOfTimePoints; iTime++) {
        data[iTime] = getDataValue(fromBrainordinateIndex, iTime);
    }
    
    CaretAssertVectorIndex(m_meanSSData, fromBrainordinateIndex);
    const BrainordinateMeanSS* fromMeanSS = m_meanSSData[fromBrainordinateIndex].get();
    
    
    return correlationTimePointDataAux(&data[0],
                                       fromMeanSS->m_mean,
                                       fromMeanSS->m_sqrt_ssxx,
                                       toBrainordinateIndex);
}

/**
 * Get the correlation coefficient for the two given brainordinates
 * that contain NON-CONTIGOUS data
 *
 * @param fromBrainordinateData
 *     Data for the 'from' brainordinate
 * @param fromBrainordinateMean
 *     Mean of the data for the 'from' brainordinate.
 * @param fromBrainordinateSSXX
 *     Sum-squared of the data for the 'from' brainordinate.
 * @param toBrainordinateIndex
 *     Index of the 'to' brainordinate.
 */
float
ConnectivityCorrelation::correlationTimePointDataAux(const float* fromBrainordinateData,
                                                     const float fromBrainordinateMean,
                                                     const float fromBrainordinateSSXX,
                                                     const int64_t toBrainordinateIndex) const
{
    CaretAssert(m_dataType == DataTypeEnum::TIMEPOINTS);
    
    CaretAssert((toBrainordinateIndex >= 0) && (toBrainordinateIndex < m_numberOfBrainordinates));
    
    CaretAssertVectorIndex(m_meanSSData, toBrainordinateIndex);
    const BrainordinateMeanSS* toMeanSS   = m_meanSSData[toBrainordinateIndex].get();
    
    double xySum(0.0);
    for (int32_t i = 0; i < m_numberOfTimePoints; i++) {
        CaretAssertVectorIndex(m_timePointData, i);
        const TimePointData* tpd = m_timePointData[i].get();
        const int64_t toOffset   = (toBrainordinateIndex   * tpd->m_dataStride);
        
        xySum += (fromBrainordinateData[i] * tpd->m_data[toOffset]);
    }
    
    const double ssxy = (m_noDemeanFlag
                         ? xySum
                         : (xySum - (m_numberOfTimePoints * fromBrainordinateMean * toMeanSS->m_mean)));
    
    float correlationCoefficient = 0.0;
    switch (m_settings.getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            if ((fromBrainordinateSSXX > 0.0)
                && (toMeanSS->m_sqrt_ssxx > 0.0)) {
                correlationCoefficient = (ssxy / (fromBrainordinateSSXX * toMeanSS->m_sqrt_ssxx));
                correlationCoefficient = finalizeCoefficient(correlationCoefficient);
            }
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
            correlationCoefficient = (ssxy / static_cast<float>(m_numberOfTimePoints));
            CaretAssert(m_numberOfBrainordinates >= 1);
            correlationCoefficient = (xySum / static_cast<float>(m_numberOfBrainordinates));
            break;
    }
    return correlationCoefficient;
}

/**
 * Compute the mean and sum-squared for all brainordinates
 */
void
ConnectivityCorrelation::computeBrainordinateMeanAndSumSquared()
{
    const float numTimePointsFloat(m_numberOfTimePoints);
    
    /*
     * Set the size of the vector so that loop can run in parallel
     */
    m_meanSSData.resize(m_numberOfBrainordinates);
    
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int64_t i = 0; i < m_numberOfBrainordinates; i++) {
        double sum(0.0);
        double sumSquared(0.0);
        
        switch (m_dataType) {
            case DataTypeEnum::BRAINORDINATES_CONTIGUOUS_DATA:
            case DataTypeEnum::BRAINORDINATES_NON_CONTIGUOUS_DATA:
            {
                CaretAssertVectorIndex(m_brainordinateData, i);
                const float* data = m_brainordinateData[i]->m_data;
                CaretAssert(data);
                const int64_t stride = m_brainordinateData[i]->m_dataStride;
                for (int64_t j = 0; j < m_numberOfTimePoints; j++) {
                    const int64_t offset = (j * stride);
                    const float d = data[offset];
                    sum        += d;
                    sumSquared += (d * d);
                }
            }
                break;
            case DataTypeEnum::INVALID:
                CaretAssert(0);
                break;
            case DataTypeEnum::TIMEPOINTS:
            {
                for (int64_t j = 0; j < m_numberOfTimePoints; j++) {
                    CaretAssertVectorIndex(m_timePointData, j);
                    const int64_t offset = (i * m_timePointData[j]->m_dataStride);
                    const float d = m_timePointData[j]->m_data[offset];
                    sum        += d;
                    sumSquared += (d * d);
                }
            }
                break;
        }


        const float mean(computeMean(sum, numTimePointsFloat));
        const float ssxxSquared = (sumSquared - (numTimePointsFloat * mean * mean));
        const float ssxx = std::sqrt(ssxxSquared);
        
        BrainordinateMeanSS* bmss = new BrainordinateMeanSS(mean,
                                                            ssxx);
        CaretAssertVectorIndex(m_meanSSData, i);
        m_meanSSData[i].reset(bmss);
    }
    
    CaretAssert(static_cast<int64_t>(m_meanSSData.size()) == m_numberOfBrainordinates);
}

/**
 * Finalize the coefficient by limiting +/- one and possibly apply Fisher-Z
 */
float
ConnectivityCorrelation::finalizeCoefficient(const float coefficient) const
{
    float r(coefficient);

    if (m_fisherZFlag) {
        if (r > 0.999999) r = 0.999999;//prevent inf
        if (r < -0.999999) r = -0.999999;//prevent -inf
        r = 0.5 * std::log((1 + r) / (1 - r));
    }
    else {
        if (r > 1.0) r = 1.0;//don't output anything silly
        if (r < -1.0) r = -1.0;
    }

    return r;
}

void
ConnectivityCorrelation::printDebugData()
{
    std::cout << "ConnectivityCorrelation:" << std::endl;
    std::cout << "   Number of data sets: " << m_meanSSData.size() << std::endl;
    const int32_t numToPrint(std::min(50,
                                      static_cast<int32_t>(m_meanSSData.size())));
    for (int32_t i = 0; i < numToPrint; i++) {
        const auto& m(m_meanSSData[i]);
        std::cout << "   " << i << ": u=" << m->m_mean
        << ", SS=" << m->m_sqrt_ssxx << std::endl;
    }
}


