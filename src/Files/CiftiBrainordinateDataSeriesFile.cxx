
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

#define __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_DECLARE__
#include "CiftiBrainordinateDataSeriesFile.h"
#undef __CIFTI_BRAINORDINATE_DATA_SERIES_FILE_DECLARE__

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#include "CiftiFile.h"
#include "DataFileException.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::CiftiBrainordinateDataSeriesFile 
 * \brief CIFTI Brainordinate by Data-Series File.
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiBrainordinateDataSeriesFile::CiftiBrainordinateDataSeriesFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    
    m_matrixDenseDynamicFile = new CiftiConnectivityMatrixDenseDynamicFile(this);
}

/**
 * Destructor.
 */
CiftiBrainordinateDataSeriesFile::~CiftiBrainordinateDataSeriesFile()
{
    delete m_matrixDenseDynamicFile;
    
}

/**
 * Clear the contents of the file.
 */
void
CiftiBrainordinateDataSeriesFile::clear()
{
    CiftiMappableDataFile::clear();
    m_matrixDenseDynamicFile->clear();
}

/**
 * Read the file.
 *
 * @param ciftiMapFileName
 *    Name of the file to read.
 * @throw
 *    DataFileException if there is an error reading the file.
 */
void
CiftiBrainordinateDataSeriesFile::readFile(const AString& ciftiMapFileName)
{
    CiftiMappableDataFile::readFile(ciftiMapFileName);
    
    try {
        m_matrixDenseDynamicFile->readFile(ciftiMapFileName);
        m_matrixDenseDynamicFile->updateAfterReading(getCiftiFile());
        
        /*
         * Palette for dynamic file is in my CIFTI file metadata
         */
        GiftiMetaData* fileMetaData = m_ciftiFile->getCiftiXML().getFileMetaData();
        const AString encodedPaletteColorMappingString = fileMetaData->get(s_paletteColorMappingNameInMetaData);
        if ( ! encodedPaletteColorMappingString.isEmpty()) {
            if (m_matrixDenseDynamicFile-getNumberOfMaps() > 0) {
                PaletteColorMapping* pcm = m_matrixDenseDynamicFile->getMapPaletteColorMapping(0);
                CaretAssert(pcm);
                pcm->decodeFromStringXML(encodedPaletteColorMappingString);
            }
        }
        
        m_matrixDenseDynamicFile->clearModified();
        
//        /*
//         * Remove palette color mapping from metadata so not seen by user
//         */
//        fileMetaData->remove(s_paletteColorMappingNameInMetaData);
//        clearModified();
    }
    catch (const DataFileException& dfe) {
        throw DataFileException("While reading/updating "
                                + ciftiMapFileName
                                + " as a dense dynamic file: "
                                + dfe.whatString());
    }
}

/**
 * Write the file.
 *
 * @param ciftiMapFileName
 *    Name of the file to write.
 * @throw
 *    DataFileException if there is an error writing the file.
 */
void
CiftiBrainordinateDataSeriesFile::writeFile(const AString& ciftiMapFileName)
{
    /*
     * Put the child dynamic data-series file's palette in the file's metadata.
     */
    GiftiMetaData* fileMetaData = m_ciftiFile->getCiftiXML().getFileMetaData();
    CaretAssert(fileMetaData);
    if (m_matrixDenseDynamicFile-getNumberOfMaps() > 0) {
        fileMetaData->set(s_paletteColorMappingNameInMetaData,
                          m_matrixDenseDynamicFile->getMapPaletteColorMapping(0)->encodeInXML());
    }
    else {
        fileMetaData->remove(s_paletteColorMappingNameInMetaData);
    }
    
    CiftiMappableDataFile::writeFile(ciftiMapFileName);

//    /*
//     * Remove palette color mapping from metadata so not seen by user
//     */
//    fileMetaData->remove(s_paletteColorMappingNameInMetaData);
    
    clearModified();
    m_matrixDenseDynamicFile->clearModified();
}
/**
 * @return Is charting enabled for this file?
 */
bool
CiftiBrainordinateDataSeriesFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiBrainordinateDataSeriesFile::isLineSeriesChartingSupported() const
{
    if (getNumberOfMaps() > 1) {
        return true;
    }
    
    return false;
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiBrainordinateDataSeriesFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                                     const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Load charting data for the surface with the given structure and node index.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndex
 *     Index of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will return true.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateDataSeriesFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
                                                               const int32_t nodeIndex)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNode(structure,
                                                           nodeIndex);
    return chartData;
}

/**
 * Load average charting data for the surface with the given structure and node indices.
 *
 * @param structure
 *     The surface's structure.
 * @param nodeIndices
 *     Indices of the node.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateDataSeriesFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
                                                                      const std::vector<int32_t>& nodeIndices)
{
    ChartDataCartesian* chartData = helpLoadChartDataForSurfaceNodeAverage(structure,
                                                                  nodeIndices);
    return chartData;
}

/**
 * Load charting data for the voxel enclosing the given coordinate.
 *
 * @param xyz
 *     Coordinate of voxel.
 * @return
 *     Pointer to the chart data.  If the data FAILED to load,
 *     the returned pointer will be NULL.  Caller takes ownership
 *     of the pointer and must delete it when no longer needed.
 */
ChartDataCartesian*
CiftiBrainordinateDataSeriesFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3])
{
    ChartDataCartesian* chartData = helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiBrainordinateDataSeriesFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
}

/**
 * Save file data from the scene.  For subclasses that need to
 * save to a scene, this method should be overriden.  sceneClass
 * will be valid and any scene data should be added to it.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.
 */
void
CiftiBrainordinateDataSeriesFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                 SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    sceneClass->addBooleanArray("m_chartingEnabledForTab",
                                m_chartingEnabledForTab,
                                BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    sceneClass->addClass(m_matrixDenseDynamicFile->saveToScene(sceneAttributes,
                                                               "m_matrixDenseDynamicFile"));
}

/**
 * Restore file data from the scene.  For subclasses that need to
 * restore from a scene, this method should be overridden. The scene class
 * will be valid and any scene data may be obtained from it.
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
CiftiBrainordinateDataSeriesFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    
    const ScenePrimitiveArray* tabArray = sceneClass->getPrimitiveArray("m_chartingEnabledForTab");
    if (tabArray != NULL) {
        sceneClass->getBooleanArrayValue("m_chartingEnabledForTab",
                                         m_chartingEnabledForTab,
                                         BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    }
    else {
        /*
         * Obsolete value when charting was not 'per tab'
         */
        const bool chartingEnabled = sceneClass->getBooleanValue("m_chartingEnabled",
                                                                   false);
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_chartingEnabledForTab[i] = chartingEnabled;
        }
    }

    const SceneClass* dynamicFileSceneClass = sceneClass->getClass("m_matrixDenseDynamicFile");
    if (dynamicFileSceneClass != NULL) {
        m_matrixDenseDynamicFile->restoreFromScene(sceneAttributes,
                                                   dynamicFileSceneClass);
    }
}

/**
 * @return My matrix dense dynamic file representation.
 */
CiftiConnectivityMatrixDenseDynamicFile*
CiftiBrainordinateDataSeriesFile::getConnectivityMatrixDenseDynamicFile()
{
    return m_matrixDenseDynamicFile;
}

/**
 * @return My matrix dense dynamic file representation (const method).
 */
const CiftiConnectivityMatrixDenseDynamicFile*
CiftiBrainordinateDataSeriesFile::getConnectivityMatrixDenseDynamicFile() const
{
    return m_matrixDenseDynamicFile;
}

/**
 * @return True if any of the maps in this file contain a
 * color mapping that possesses a modified status.
 */
bool
CiftiBrainordinateDataSeriesFile::isModifiedPaletteColorMapping() const
{
    /*
     * This method is override because we need to know if the
     * encapsulated dynamic dense file has a modified palette.
     * When restoring a scene, a file with any type of modification
     * must be reloaded to remove any modifications.  Note that
     * when a scene is restored, files that are not modified and
     * are in the new scene are NOT reloaded to save time.
     */
    if (CiftiMappableDataFile::isModifiedPaletteColorMapping()) {
        return true;
    }
    
    if (m_matrixDenseDynamicFile->isModifiedPaletteColorMapping()) {
        return true;
    }
    
    return false;
}



