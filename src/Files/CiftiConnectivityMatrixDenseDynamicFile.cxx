
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

#include <cmath>
#include <iostream>

#define __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiFile.h"
#include "ConnectivityCorrelation.h"
#include "ConnectivityCorrelationSettings.h"
#include "FileInformation.h"
#include "MathFunctions.h"
#include "SceneClassAssistant.h"
#include "dot_wrapper.h"

using namespace caret;

/**
 * \class caret::CiftiConnectivityMatrixDenseDynamicFile 
 * \brief Connectivity Dynamic Dense x Dense File version of data-series
 * \ingroup Files
 *
 * Contains dynamic connectivity from brainordinates to brainordinates.
 * Internally, the file format is the same as a data series file.  When
 * a row is requested, the row is correlated with all other rows
 * producing the connectivity from that row to all other rows.
 */

/**
 * Constructor.
 *
 * @param parentDataSeriesFile
 *     Parent data series file.
 */
CiftiConnectivityMatrixDenseDynamicFile::CiftiConnectivityMatrixDenseDynamicFile(CiftiBrainordinateDataSeriesFile* parentDataSeriesFile)
: CiftiMappableConnectivityMatrixDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC),
m_parentDataSeriesFile(parentDataSeriesFile),
m_parentDataSeriesCiftiFile(NULL),
m_numberOfBrainordinates(-1),
m_numberOfTimePoints(-1),
m_validDataFlag(false),
m_enabledAsLayer(true),
m_cacheDataFlag(false)
{
    CaretAssert(m_parentDataSeriesFile);

    m_correlationSettings.reset(new ConnectivityCorrelationSettings());
    
    m_sceneAssistant.grabNew(new SceneClassAssistant());
    m_sceneAssistant->add("m_enabledAsLayer",
                          &m_enabledAsLayer);
    m_sceneAssistant->add("m_correlationSettings",
                          "ConnectivityCorrelationSettings",
                          m_correlationSettings.get());
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixDenseDynamicFile::~CiftiConnectivityMatrixDenseDynamicFile()
{
}

/**
 * @return The parent brainordinate data-series file (const method)
 */
const CiftiBrainordinateDataSeriesFile*
CiftiConnectivityMatrixDenseDynamicFile::getParentBrainordinateDataSeriesFile() const
{
    return m_parentDataSeriesFile;
}

/**
 * @return The parent brainordinate data-series file.
 */
CiftiBrainordinateDataSeriesFile*
CiftiConnectivityMatrixDenseDynamicFile::getParentBrainordinateDataSeriesFile()
{
    return m_parentDataSeriesFile;
}

/**
 * @return True if enabled as a layer.
 */
bool
CiftiConnectivityMatrixDenseDynamicFile::isEnabledAsLayer() const
{
    return m_enabledAsLayer;
}

/**
 * Set enabled as a layer.
 *
 * @param True if enabled as a layer.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::setEnabledAsLayer(const bool enabled)
{
    m_enabledAsLayer = enabled;
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Dense files do NOT support writing.
 */
bool
CiftiConnectivityMatrixDenseDynamicFile::supportsWriting() const
{
    return false;
}

/**
 * @return Is the data within the file valid?
 */
bool
CiftiConnectivityMatrixDenseDynamicFile::isDataValid() const
{
    return m_validDataFlag;
}

/**
 * Update the content of this dense dynamic file after the parent 
 * brainordinate data series file is successfully read.
 *
 * @param ciftiFile
 *     Parent's CIFTI file..
 */
void
CiftiConnectivityMatrixDenseDynamicFile::updateAfterReading(const CiftiFile* ciftiFile)
{
    m_validDataFlag = false;
    
    m_parentDataSeriesCiftiFile = const_cast<CiftiFile*>(ciftiFile);
    
    AString path, nameNoExt, ext;
    FileInformation fileInfo(m_parentDataSeriesCiftiFile->getFileName());
    fileInfo.getFileComponents(path, nameNoExt, ext);
    setFileName(FileInformation::assembleFileComponents(path,
                                                        nameNoExt,
                                                        DataFileTypeEnum::toFileExtension(DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC)));
    
    /*
     * Need dimensions of data
     * Note that CIFTI XML in this file is identifical to CIFTI XML in parent data-series file
     */
    const CiftiXML& ciftiXML = getCiftiFile()->getCiftiXML();
    m_numberOfBrainordinates = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getLength();
    m_numberOfTimePoints     = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW).getLength();
    
    m_rowData.clear();
    
    if ((m_numberOfBrainordinates > 0)
        && (m_numberOfTimePoints > 0)) {
        m_rowData.resize(m_numberOfBrainordinates);
        
        if (m_cacheDataFlag) {
            /*
             * Read all of the data.  Time-series type files are not
             * too large and by caching the data, it eliminates
             * numerous calls to read the data when correlation
             * is performed.
             *
             * READ DATA FROM PARENT FILE
             */
            for (int32_t i = 0; i < m_numberOfBrainordinates; i++) {
                CaretAssertVectorIndex(m_rowData, i);
                m_rowData[i].m_data.resize(m_numberOfTimePoints);
                m_parentDataSeriesCiftiFile->getRow(&m_rowData[i].m_data[0],
                                                    i);
            }
        }
        
        preComputeRowMeanAndSumSquared();
        
        m_validDataFlag = true;
    }
}


/**
 * Load data for the given column.
 *
 * @param dataOut
 *     Output with data.
 * @param index
 *     Index of the column.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::getDataForColumn(float* /*dataOut*/,
                                                          const int64_t& /*index*/) const
{
    const AString msg("Should never be called for Dense Dynamic File");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
}

/**
 * Load data for the given row.
 *
 * @param dataOut
 *     Output with data.
 * @param index
 *     Index of the row.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::getDataForRow(float* dataOut,
                                                       const int64_t& index) const
{
    m_parentDataSeriesCiftiFile->getRow(dataOut,
                                        index);
}

/**
 * Load PROCESSED data for the given column.
 *
 * Some file types may have special processing for a column.  This method can be
 * overridden for those types of files.
 *
 * @param dataOut
 *     Output with data.
 * @param index 
 *     Index of the column.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::getProcessedDataForColumn(float* /*dataOut*/,
                                                                   const int64_t& /*index*/) const
{
    const AString msg("Should never be called for Dense Dynamic File");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
}

/**
 * Load PROCESSED data for the given row.
 *
 * Some file types may have special processing for a row.  This method can be
 * overridden for those types of files.
 *
 * @param dataOut
 *     Output with data.
 * @param index 
 *     Index of the row.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::getProcessedDataForRow(float* dataOut,
                                                                const int64_t& index) const
{
    if ((m_numberOfBrainordinates <= 0)
        || (m_numberOfTimePoints <= 0)) {
        return;
    }
    
    std::vector<float> dataVector(m_numberOfBrainordinates);
    ConnectivityCorrelation* connCorrelation(getConnectivityCorrelation());
    if (connCorrelation == NULL) {
        std::fill(dataVector.begin(),
                  dataVector.end(),
                  0.0);
        return;
    }
    
    connCorrelation->getCorrelationForBrainordinate(index,
                                                    dataVector);

    if ( ! m_testConnectivityCorrelationFlag) {
        for (int32_t i = 0; i < m_numberOfBrainordinates; i++) {
            CaretAssertVectorIndex(dataVector, i);
            dataOut[i] = dataVector[i];
        }
        return;
    }

    
    std::vector<float> rowData(m_numberOfTimePoints);
    m_parentDataSeriesCiftiFile->getRow(&rowData[0], index);
    const float mean = m_rowData[index].m_mean;
    const float ssxx = m_rowData[index].m_sqrt_ssxx;
    
    /*
     * TSC: hyperthreading means some cores end up "faster" than others, so "static" scheduling is generally not as fast
     * there is almost no overhead to dynamic scheduling
     */
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int32_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {
        float coefficient = 1.0;
        
        if (iRow != index) {
            coefficient = correlation(rowData, mean, ssxx, iRow, m_numberOfTimePoints);
        }
        
        dataOut[iRow] = coefficient;
    }
    
    const bool sameFlag(MathFunctions::compareArrays(&dataOut[0], &dataVector[0], m_numberOfBrainordinates, 0.001));
    std::cout << "getProcessedDataForRow() comparison " << AString::fromBool(sameFlag) << std::endl;
//    for (int64_t i = 0; i < m_numberOfBrainordinates; i++) {
//        std::cout << dataOut[i] << ", " << dataVector[i] << std::endl;
//    }
}

/**
 * Some file types may perform additional processing of row average data and
 * can override this method.
 *
 * @param rowAverageDataInOut
 *     The row average data.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::processRowAverageData(std::vector<float>& rowAverageDataInOut)
{
    if ((m_numberOfBrainordinates <= 0)
        || (m_numberOfTimePoints <= 0)) {
        return;
    }
    
    const int32_t dataLength = static_cast<int32_t>(rowAverageDataInOut.size());
    if (dataLength != m_numberOfTimePoints) {
        CaretLogWarning("Data length incorrect.  Is "
                        + AString::number(dataLength)
                        + " but should be "
                        + AString::number(m_numberOfTimePoints));
        return;
    }
    if (dataLength <= 0) {
        return;
    }
    
    std::vector<float> connCorrData;
    {
        ConnectivityCorrelation* connCorrelation(getConnectivityCorrelation());
        if (connCorrelation == NULL) {
            std::fill(connCorrData.begin(),
                      connCorrData.end(),
                      0.0);
            return;
        }
        connCorrelation->getCorrelationForBrainordinateData(rowAverageDataInOut,
                                                            connCorrData);
    }
    
    if ( ! m_testConnectivityCorrelationFlag) {
        rowAverageDataInOut = connCorrData;
        return;
    }
    
    float mean = 0.0;
    float sumSquared = 0.0;
    computeDataMeanAndSumSquared(&rowAverageDataInOut[0],
                                 dataLength,
                                 mean,
                                 sumSquared);
    
    std::vector<float> processedRowAverageData(m_numberOfBrainordinates);
    
    /*
     * TSC: hyperthreading means some cores end up "faster" than others, so "static" scheduling is generally not as fast
     * there is almost no overhead to dynamic scheduling
     */
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int32_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {
        const float coefficient = correlation(rowAverageDataInOut,
                                              mean,
                                              sumSquared,
                                              iRow,
                                              dataLength);
        CaretAssertVectorIndex(processedRowAverageData, iRow);
        processedRowAverageData[iRow] = coefficient;
    }
    
    rowAverageDataInOut = processedRowAverageData;
    
    if (connCorrData.size() == rowAverageDataInOut.size()) {
        if (connCorrData.size() == m_numberOfBrainordinates) {
            const bool sameFlag(MathFunctions::compareArrays(&rowAverageDataInOut[0], &connCorrData[0], m_numberOfBrainordinates, 0.001));
            std::cout << "processRowAverageData() comparison " << AString::fromBool(sameFlag) << std::endl;
//            for (int64_t i = 0; i < m_numberOfBrainordinates; i++) {
//                std::cout << rowAverageDataInOut[i] << ", " << connCorrData[i] << std::endl;
//            }
        }
    }
}


/**
 * Compute the mean and sum-squared for each row so that they
 * are only calculated once.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::preComputeRowMeanAndSumSquared()
{
    CaretAssert(m_numberOfBrainordinates > 0);
    CaretAssert(m_numberOfTimePoints > 0);

    /*
     * TSC: hyperthreading means some cores end up "faster" than others, so "static" scheduling is generally not as fast
     * there is almost no overhead to dynamic scheduling
     */
#pragma omp CARET_PARFOR schedule(dynamic)
    for (int32_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {

        CaretAssertVectorIndex(m_rowData, iRow);
        
        if (m_cacheDataFlag) {
            CaretAssertVectorIndex(m_rowData[iRow].m_data, (m_numberOfTimePoints - 1));
            computeDataMeanAndSumSquared(&m_rowData[iRow].m_data[0],
                                         m_numberOfTimePoints,
                                         m_rowData[iRow].m_mean,
                                         m_rowData[iRow].m_sqrt_ssxx);
        }
        else {
            std::vector<float> data(m_numberOfTimePoints);
#pragma omp critical
            {//TSC: this can do disk access, which is not currently thread-safe
                m_parentDataSeriesCiftiFile->getRow(&data[0], iRow);
            }
            computeDataMeanAndSumSquared(&data[0],
                                         m_numberOfTimePoints,
                                         m_rowData[iRow].m_mean,
                                         m_rowData[iRow].m_sqrt_ssxx);
        }
    }
}

/**
 * Compute data's mean and sum-squared
 *
 * @param data
 *     Data on which mean and sum-squared are calculated
 * @param dataLength
 *     Number of items in data.
 * @param meanOut
 *     Output with mean of data.
 * @param sumSquaredOut
 *     Output with sum-squared.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::computeDataMeanAndSumSquared(const float* data,
                                                                      const int32_t dataLength,
                                                                      float& meanOut,
                                                                      float& sumSquaredOut) const
{
    meanOut = 0.0;
    sumSquaredOut = 0.0;
    if (dataLength <= 0) {
        return;
    }
    
    double sum = 0.0;
    double sumSquared = 0.0;
    
    for (int32_t i = 0; i < dataLength; i++) {
        const float d = data[i];
        sum        += d;
        sumSquared += (d * d);
    }
    
    meanOut = (sum / dataLength);
    const float ssxx = (sumSquared - (dataLength * meanOut * meanOut));
    //TSC: do not assert things that depend on input file content (a NaN in the data will trip it), you could print a warning instead
    //CaretAssert(ssxx >= 0.0);
    sumSquaredOut = std::sqrt(ssxx);
}


/**
 * Correlation from https://en.wikipedia.org/wiki/Pearson_product-moment_correlation_coefficient
 *
 * @param data
 *     Data for correlation
 * @param mean
 *     Mean of data
 * @param sumSquared
 *     Sum squared of data.
 * @param otherRowIndex
 *     Index of another row
 * @param numberOfPoints
 *     Number of points int the two arrays
 * @return
 *     The correlation coefficient computed on the two arrays.
 */
float
CiftiConnectivityMatrixDenseDynamicFile::correlation(const std::vector<float>& data,
                                                     const float mean,
                                                     const float sumSquared,
                                                     const int32_t otherRowIndex,
                                                     const int32_t numberOfPoints) const
{
    const double numFloat = numberOfPoints;
    double xySum = 0.0;
    
    CaretAssertVectorIndex(m_rowData, otherRowIndex);
    const RowData& otherData = m_rowData[otherRowIndex];
    
    if (m_cacheDataFlag) {
        xySum = dsdot(&data[0], &otherData.m_data[0], numberOfPoints);
    }
    else {
        std::vector<float> otherDataVector(m_numberOfTimePoints);
        m_parentDataSeriesCiftiFile->getRow(&otherDataVector[0], otherRowIndex);
        xySum = dsdot(&data[0], &otherDataVector[0], numberOfPoints);
    }
    
    const double ssxy = xySum - (numFloat * mean * otherData.m_mean);
    
    float correlationCoefficient = 0.0;
    if ((sumSquared > 0.0)
        && (otherData.m_sqrt_ssxx > 0.0)) {
        correlationCoefficient = (ssxy / (sumSquared * otherData.m_sqrt_ssxx));
    }
    return correlationCoefficient;
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
CiftiConnectivityMatrixDenseDynamicFile::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                                                 SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                                      const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return Pointer to connectivity correlation or NULL if not valid
 */
ConnectivityCorrelation*
CiftiConnectivityMatrixDenseDynamicFile::getConnectivityCorrelation() const
{
    if ( ! m_connectivityCorrelationFailedFlag) {
        if (m_connectivityCorrelation == NULL) {
            /*
             * Need data and timepoint count from parent file
             */
            CaretAssert(m_parentDataSeriesFile);
            CaretAssert(m_numberOfTimePoints >= 2);
            CaretAssert(m_numberOfBrainordinates >= 2);
            const int64_t numData(m_numberOfBrainordinates
                                  * m_numberOfTimePoints);
            m_dataSeriesMatrixData.resize(numData);
            
            CaretAssert(m_parentDataSeriesCiftiFile);
            for (int64_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {
                const int64_t offset(iRow * m_numberOfTimePoints);
                CaretAssertVectorIndex(m_dataSeriesMatrixData,
                                       (offset + (m_numberOfTimePoints - 1)));
                m_parentDataSeriesCiftiFile->getRow(&m_dataSeriesMatrixData[offset],
                                                    iRow);
            }
            const int64_t nextBrainordinateStride(m_numberOfTimePoints);
            const int64_t nextTimePointStride(1);
            AString errorMessage;
            ConnectivityCorrelation* cc = ConnectivityCorrelation::newInstance(&m_dataSeriesMatrixData[0],
                                                                               m_numberOfBrainordinates,
                                                                               nextBrainordinateStride,
                                                                               m_numberOfTimePoints,
                                                                               nextTimePointStride,
                                                                               errorMessage);
            if (cc != NULL) {
                m_connectivityCorrelation.reset(cc);
            }
            else {
                m_connectivityCorrelationFailedFlag = true;
                CaretLogSevere("Failed to create connectvity correlation for "
                               + m_parentDataSeriesFile->getFileNameNoPath());
            }
        }
    }
    
    return m_connectivityCorrelation.get();
}

/**
 * @return The correlation settings
 */
ConnectivityCorrelationSettings*
CiftiConnectivityMatrixDenseDynamicFile::getCorrelationSettings()
{
    return m_correlationSettings.get();
}

/**
 * @return The correlation settings (const method)
 */
const ConnectivityCorrelationSettings*
CiftiConnectivityMatrixDenseDynamicFile::getCorrelationSettings() const
{
    return m_correlationSettings.get();
}


