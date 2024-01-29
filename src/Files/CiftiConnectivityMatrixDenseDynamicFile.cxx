
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
#include "ConnectivityCorrelationTwo.h"
#include "ConnectivityCorrelationSettings.h"
#include "FileInformation.h"
#include "SceneClassAssistant.h"

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
m_enabledAsLayer(true)
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
    CaretAssert(ciftiFile);
    m_validDataFlag = false;
    
    m_parentDataSeriesCiftiFile = const_cast<CiftiFile*>(ciftiFile);
    CaretAssert(m_parentDataSeriesCiftiFile);
    if (m_parentDataSeriesCiftiFile == NULL) {
        CaretLogSevere(ciftiFile->getFileName()
                       + " is not a data series file !!!");
        return;
    }
    
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
    
    if ((m_numberOfBrainordinates > 0)
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
CiftiConnectivityMatrixDenseDynamicFile::getProcessedDataForRow(std::vector<float>& dataOut,
                                                                const int64_t& index) const
{
    if ((m_numberOfBrainordinates <= 0)
        || (m_numberOfTimePoints <= 0)) {
        return;
    }
    
    CaretAssert(static_cast<int64_t>(dataOut.size()) == m_numberOfBrainordinates);

    ConnectivityCorrelationTwo* connCorrelationTwo(getConnectivityCorrelationTwo());
    if (connCorrelationTwo == NULL) {
        std::fill(dataOut.begin(),
                  dataOut.end(),
                  0.0);
        return;
    }

    connCorrelationTwo->computeForDataSetIndex(index,
                                               dataOut);
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
ConnectivityCorrelationTwo*
CiftiConnectivityMatrixDenseDynamicFile::getConnectivityCorrelationTwo() const
{
    if ( ! m_connectivityCorrelationFailedFlag) {
        /**
         * Need to recreate correlation algorithm if settins have changed
         */
        if (m_connectivityCorrelationTwo != NULL) {
            if (*m_correlationSettings != *m_connectivityCorrelationTwo->getSettings()) {
                m_connectivityCorrelationTwo.reset();
                CaretLogFine("Recreating correlation algorithm for "
                             + getFileName());
            }
        }
        if (m_connectivityCorrelationTwo == NULL) {
            /*
             * Need data and timepoint count from parent file
             */
            CaretAssert(m_parentDataSeriesFile);
            CaretAssert(m_numberOfTimePoints >= 2);
            CaretAssert(m_numberOfBrainordinates >= 2);
            const int64_t numData(m_numberOfBrainordinates
                                  * m_numberOfTimePoints);
            m_dataSeriesMatrixData.resize(numData);
            
            std::vector<const float*> rowDataPointers;
            CaretAssert(m_parentDataSeriesCiftiFile);
            for (int64_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {
                const int64_t offset(iRow * m_numberOfTimePoints);
                CaretAssertVectorIndex(m_dataSeriesMatrixData,
                                       (offset + (m_numberOfTimePoints - 1)));
                m_parentDataSeriesCiftiFile->getRow(&m_dataSeriesMatrixData[offset],
                                                    iRow);
                rowDataPointers.push_back(&m_dataSeriesMatrixData[offset]);
            }
            
            const int64_t nextTimePointStride(1);
            AString errorMessage;
            ConnectivityCorrelationTwo* cc = ConnectivityCorrelationTwo::newInstance(getFileName(),
                                                                                     *m_correlationSettings,
                                                                                     rowDataPointers,
                                                                                     m_numberOfTimePoints,
                                                                                     nextTimePointStride,
                                                                                     errorMessage);
            if (cc != NULL) {
                m_connectivityCorrelationTwo.reset(cc);
            }
            else {
                m_connectivityCorrelationFailedFlag = true;
                CaretLogSevere("Failed to create connectvity correlation for "
                               + m_parentDataSeriesFile->getFileNameNoPath());
            }
        }
    }
    
    return m_connectivityCorrelationTwo.get();
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


