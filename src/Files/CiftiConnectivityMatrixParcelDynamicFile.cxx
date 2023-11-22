
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
#include "CiftiConnectivityMatrixParcelDynamicFile.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiFile.h"
#include "ConnectivityCorrelation.h"
#include "ConnectivityCorrelationSettings.h"
#include "FileInformation.h"
#include "MathFunctions.h"
#include "SceneClassAssistant.h"
#include "dot_wrapper.h"

using namespace caret;

/**
 * \class caret::CiftiConnectivityMatrixParcelDynamicFile
 * \brief Connectivity Dynamic Dense x Dense File version of parcel-series
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
 * @param parentParcelSeriesFile
 *     Parent parcel series file.
 */
CiftiConnectivityMatrixParcelDynamicFile::CiftiConnectivityMatrixParcelDynamicFile(CiftiParcelSeriesFile* parentParcelSeriesFile)
: CiftiMappableConnectivityMatrixDataFile(DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC),
m_parentParcelSeriesFile(parentParcelSeriesFile),
m_parentParcelSeriesCiftiFile(NULL),
m_numberOfParcels(-1),
m_numberOfTimePoints(-1),
m_validDataFlag(false),
m_enabledAsLayer(true)
{
    CaretAssert(m_parentParcelSeriesFile);

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
CiftiConnectivityMatrixParcelDynamicFile::~CiftiConnectivityMatrixParcelDynamicFile()
{
}

/**
 * @return The parent  parcel series file (const method)
 */
const CiftiParcelSeriesFile*
CiftiConnectivityMatrixParcelDynamicFile::getParentParcelSeriesFile() const
{
    return m_parentParcelSeriesFile;
}

/**
 * @return The parent parcel series file.
 */
CiftiParcelSeriesFile*
CiftiConnectivityMatrixParcelDynamicFile::getParentParcelSeriesFile()
{
    return m_parentParcelSeriesFile;
}

/**
 * @return True if enabled as a layer.
 */
bool
CiftiConnectivityMatrixParcelDynamicFile::isEnabledAsLayer() const
{
    return m_enabledAsLayer;
}

/**
 * Set enabled as a layer.
 *
 * @param True if enabled as a layer.
 */
void
CiftiConnectivityMatrixParcelDynamicFile::setEnabledAsLayer(const bool enabled)
{
    m_enabledAsLayer = enabled;
}

/**
 * @return True if this file type supports writing, else false.
 *
 * Dynamic files do NOT support writing.
 */
bool
CiftiConnectivityMatrixParcelDynamicFile::supportsWriting() const
{
    return false;
}

/**
 * @return Is the data within the file valid?
 */
bool
CiftiConnectivityMatrixParcelDynamicFile::isDataValid() const
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
CiftiConnectivityMatrixParcelDynamicFile::updateAfterReading(const CiftiFile* ciftiFile)
{
    m_validDataFlag = false;
    
    m_parentParcelSeriesCiftiFile = const_cast<CiftiFile*>(ciftiFile);
    
    AString path, nameNoExt, ext;
    FileInformation fileInfo(m_parentParcelSeriesCiftiFile->getFileName());
    fileInfo.getFileComponents(path, nameNoExt, ext);
    setFileName(FileInformation::assembleFileComponents(path,
                                                        nameNoExt,
                                                        DataFileTypeEnum::toFileExtension(DataFileTypeEnum::CONNECTIVITY_PARCEL_DYNAMIC)));
    
    /*
     * Need dimensions of data
     * Note that CIFTI XML in this file is identifical to CIFTI XML in parent data-series file
     */
    const CiftiXML& ciftiXML = getCiftiFile()->getCiftiXML();
    
    CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::PARCELS);
    CaretAssert(ciftiXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES);
    m_numberOfParcels    = ciftiXML.getParcelsMap(CiftiXML::ALONG_COLUMN).getLength();
    m_numberOfTimePoints = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW).getLength();
    
    if ((m_numberOfParcels > 0)
        && (m_numberOfTimePoints > 0)) {
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
CiftiConnectivityMatrixParcelDynamicFile::getDataForColumn(float* /*dataOut*/,
                                                          const int64_t& /*index*/) const
{
    const AString msg("Should never be called for Parcel Dynamic File");
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
CiftiConnectivityMatrixParcelDynamicFile::getDataForRow(float* dataOut,
                                                       const int64_t& index) const
{
    m_parentParcelSeriesCiftiFile->getRow(dataOut,
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
CiftiConnectivityMatrixParcelDynamicFile::getProcessedDataForColumn(float* /*dataOut*/,
                                                                   const int64_t& /*index*/) const
{
    const AString msg("Should never be called for Parcel Dynamic File");
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
CiftiConnectivityMatrixParcelDynamicFile::getProcessedDataForRow(float* dataOut,
                                                                const int64_t& index) const
{
    if ((m_numberOfParcels <= 0)
        || (m_numberOfTimePoints <= 0)) {
        return;
    }
    
    std::vector<float> dataVector(m_numberOfParcels, 0.0);
    ConnectivityCorrelation* connCorrelation(getConnectivityCorrelation());
    if (connCorrelation != NULL) {
        connCorrelation->getCorrelationForBrainordinate(index,
                                                        dataVector);
        CaretAssert(static_cast<int32_t>(dataVector.size()) == m_numberOfParcels);
    }
    
    for (int32_t i = 0; i < m_numberOfParcels; i++) {
        CaretAssertVectorIndex(dataVector, i);
        dataOut[i] = dataVector[i];
    }
}

/**
 * Some file types may perform additional processing of row average data and
 * can override this method.
 *
 * @param rowAverageDataInOut
 *     The row average data.
 */
void
CiftiConnectivityMatrixParcelDynamicFile::processRowAverageData(std::vector<float>& rowAverageDataInOut)
{
    if ((m_numberOfParcels <= 0)
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
    
    std::vector<float> connData(m_numberOfParcels, 0.0);
    ConnectivityCorrelation* connCorrelation(getConnectivityCorrelation());
    if (connCorrelation != NULL) {
        connCorrelation->getCorrelationForBrainordinateData(rowAverageDataInOut,
                                                            connData);
        CaretAssert(static_cast<int32_t>(connData.size()) == m_numberOfParcels);
    }

    rowAverageDataInOut = connData;
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
CiftiConnectivityMatrixParcelDynamicFile::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
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
CiftiConnectivityMatrixParcelDynamicFile::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                                      const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return Pointer to connectivity correlation or NULL if not valid
 */
ConnectivityCorrelation*
CiftiConnectivityMatrixParcelDynamicFile::getConnectivityCorrelation() const
{
    if ( ! m_connectivityCorrelationFailedFlag) {
        /**
         * Need to recreate correlation algorithm if settins have changed
         */
        if (m_connectivityCorrelation != NULL) {
            if (*m_correlationSettings != *m_connectivityCorrelation->getSettings()) {
                m_connectivityCorrelation.reset();
                CaretLogFine("Recreating correlation algorithm for "
                             + getFileName());
            }
        }
        if (m_connectivityCorrelation == NULL) {
            /*
             * Need data and timepoint count from parent file
             */
            CaretAssert(m_parentParcelSeriesFile);
            CaretAssert(m_numberOfTimePoints >= 2);
            CaretAssert(m_numberOfParcels >= 2);
            const int64_t numData(m_numberOfParcels
                                  * m_numberOfTimePoints);
            m_parcelSeriesMatrixData.resize(numData);
            
            std::vector<const float*> parcelDataPointers;
            CaretAssert(m_parentParcelSeriesCiftiFile);
            for (int64_t iRow = 0; iRow < m_numberOfParcels; iRow++) {
                const int64_t offset(iRow * m_numberOfTimePoints);
                CaretAssertVectorIndex(m_parcelSeriesMatrixData,
                                       (offset + (m_numberOfTimePoints - 1)));
                m_parentParcelSeriesCiftiFile->getRow(&m_parcelSeriesMatrixData[offset],
                                                    iRow);
                parcelDataPointers.push_back(&m_parcelSeriesMatrixData[offset]);
            }
            
            const int64_t nextParcelStride(m_numberOfTimePoints);
            const int64_t nextTimePointStride(1);
            AString errorMessage;
            ConnectivityCorrelation* cc = ConnectivityCorrelation::newInstanceParcels(*m_correlationSettings,
                                                                                      parcelDataPointers,
                                                                                      m_numberOfTimePoints,
                                                                                      errorMessage);
            if (cc != NULL) {
                m_connectivityCorrelation.reset(cc);
            }
            else {
                m_connectivityCorrelationFailedFlag = true;
                CaretLogSevere("Failed to create connectvity correlation for "
                               + m_parentParcelSeriesFile->getFileNameNoPath());
            }
        }
    }
    
    return m_connectivityCorrelation.get();
}

/**
 * @return The correlation settings
 */
ConnectivityCorrelationSettings*
CiftiConnectivityMatrixParcelDynamicFile::getCorrelationSettings()
{
    return m_correlationSettings.get();
}

/**
 * @return The correlation settings (const method)
 */
const ConnectivityCorrelationSettings*
CiftiConnectivityMatrixParcelDynamicFile::getCorrelationSettings() const
{
    return m_correlationSettings.get();
}


