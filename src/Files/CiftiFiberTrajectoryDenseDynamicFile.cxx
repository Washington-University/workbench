
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

#define __CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_DECLARE__
#include "CiftiFiberTrajectoryDenseDynamicFile.h"
#undef __CIFTI_BRAINORDINATE_FIBER_TRAJECTORY_DYNAMIC_FILE_DECLARE__

#include "CaretLogger.h"
#include "ChartDataCartesian.h"
#include "CiftiFiberTrajectoryFile.h"
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "SceneClass.h"
#include "SceneClassArray.h"

using namespace caret;


    
/**
 * \class caret::CiftiFiberTrajectoryDenseDynamicFile 
 * \brief Connectivity from Fiber Trajectory
 * \ingroup Files
 *
 */

/**
 * @return Index of fiber counts map
 */
int32_t
CiftiFiberTrajectoryDenseDynamicFile::getFiberCountsMapIndex()
{
    return s_fiberCountsMapIndex;
}

/**
 * @rerturn Index of fiber distance map
 */
int32_t
CiftiFiberTrajectoryDenseDynamicFile::getFiberDistancesMapIndex()
{
    return s_fiberDistancesMapIndex;
}

/**
 * Constructor.
 * @param parentFiberTrajectoryFile
 *    The parent fiber trajectory file
 */
CiftiFiberTrajectoryDenseDynamicFile::CiftiFiberTrajectoryDenseDynamicFile(CiftiFiberTrajectoryFile* parentFiberTrajectoryFile)
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_DYNAMIC)
{
    m_parentFiberTrajectoryFile = parentFiberTrajectoryFile;
    CaretAssert(m_parentFiberTrajectoryFile);
    
    m_sceneClassAssistant.reset(new SceneClassAssistant());
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
    }
    
    AString errorMessage;
    
    try {
        const CaretSparseFile* sparseFile = m_parentFiberTrajectoryFile->m_sparseFile;
        if (sparseFile == NULL) {
            throw DataFileException("Sparse file is NULL in parent file "
                                           + m_parentFiberTrajectoryFile->getFileName());
        }
        
        const CiftiXML& trajFileXML(sparseFile->getCiftiXML());

        CiftiFile* ciftiFile = new CiftiFile();
        
        /*
         * Copy XML from traj File
         */
        CiftiXML myCiftiXML = trajFileXML;
                
        /*
         * Change column map to scalars
         */
        CiftiScalarsMap scalarsMap;
        scalarsMap.setLength(2);
        scalarsMap.setMapName(s_fiberCountsMapIndex,
                              "Fiber Counts");
        scalarsMap.setMapName(s_fiberDistancesMapIndex,
                              "Fiber Distances");
        myCiftiXML.setMap(CiftiXML::ALONG_COLUMN,
                          scalarsMap);
        ciftiFile->setCiftiXML(myCiftiXML);
        
        /*
         * Add data to the file
         */
        CiftiBrainModelsMap brainModelsMap(myCiftiXML.getBrainModelsMap(CiftiXML::ALONG_ROW));
        const float zero(0.0);
        std::vector<float> rowData(brainModelsMap.getLength(),
                                   zero);
        ciftiFile->setRow(&rowData[0],
                             s_fiberCountsMapIndex);
        const float one(1.0);
        std::fill(rowData.begin(), rowData.end(), one);
        ciftiFile->setRow(&rowData[0],
                          s_fiberDistancesMapIndex);

        m_ciftiFile.grabNew(ciftiFile);
        
        /*
         * Create a filename for this file
         */
        FileInformation initialFileNameInfo(m_parentFiberTrajectoryFile->getFileName());
        const AString myFileName = initialFileNameInfo.getAsLocalAbsoluteFilePath(m_parentFiberTrajectoryFile->getFilePath(),
                                                                                      getDataFileType());
        
        setFileName(myFileName);
        
        initializeAfterReading(myFileName);
        
        updateAfterFileDataChanges();
        setModified();
        
        m_validFileFlag = true;
    } catch (const DataFileException& e) {
        CaretLogSevere("Failed to create dynamic child file for "
                       + m_parentFiberTrajectoryFile->getFileName()
                       + "\nError: "
                       + e.whatString());
        m_validFileFlag = false;
    }
    
    m_enabledAsLayerFlag = true;
    m_dataLoadingEnabled = true;

    m_sceneClassAssistant->add("m_dataLoadingEnabled",
                               &m_dataLoadingEnabled);
    m_sceneClassAssistant->add("m_enabledAsLayerFlag",
                               &m_enabledAsLayerFlag);
    m_sceneClassAssistant->addArray("m_chartingEnabledForTab",
                                    m_chartingEnabledForTab,
                                    BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                    m_chartingEnabledForTab[0]);
}

/**
 * Destructor.
 */
CiftiFiberTrajectoryDenseDynamicFile::~CiftiFiberTrajectoryDenseDynamicFile()
{
    
}

/**
 * @return True if this file is valid, else false.
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isValidFile()
{
    return m_validFileFlag;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isLineSeriesChartingEnabled(const int32_t tabIndex) const
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
CiftiFiberTrajectoryDenseDynamicFile::isLineSeriesChartingSupported() const
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
CiftiFiberTrajectoryDenseDynamicFile::setLineSeriesChartingEnabled(const int32_t tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiFiberTrajectoryDenseDynamicFile::getSupportedLineSeriesChartDataTypes(std::vector<ChartOneDataTypeEnum::Enum>& chartDataTypesOut) const
{
    helpGetSupportedLineSeriesChartDataTypes(chartDataTypesOut);
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
CiftiFiberTrajectoryDenseDynamicFile::loadLineSeriesChartDataForSurfaceNode(const StructureEnum::Enum structure,
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
CiftiFiberTrajectoryDenseDynamicFile::loadAverageLineSeriesChartDataForSurfaceNodes(const StructureEnum::Enum structure,
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
CiftiFiberTrajectoryDenseDynamicFile::loadLineSeriesChartDataForVoxelAtCoordinate(const float xyz[3])
{
    ChartDataCartesian* chartData = helpLoadChartDataForVoxelAtCoordinate(xyz);
    return chartData;
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
CiftiFiberTrajectoryDenseDynamicFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    CiftiMappableDataFile::saveFileDataToScene(sceneAttributes,
                                               sceneClass);
    
    m_sceneClassAssistant->saveMembers(sceneAttributes,
                                       sceneClass);
    CaretAssert(m_ciftiFile);
    const int64_t columnCount(m_ciftiFile->getNumberOfColumns());
    if (columnCount > 0) {
        std::vector<float> dynamicConnectivityFiberCountsData(columnCount, 0.0);
        std::vector<float> dynamicConnectivityFiberDistancesData(columnCount, 0.0);
        m_ciftiFile->getRow(&dynamicConnectivityFiberCountsData[0],
                            s_fiberCountsMapIndex);
        m_ciftiFile->getRow(&dynamicConnectivityFiberDistancesData[0],
                            s_fiberDistancesMapIndex);
        sceneClass->addFloatVector("dynamicConnectivityFiberCountsData",
                                   dynamicConnectivityFiberCountsData);
        sceneClass->addFloatVector("dynamicConnectivityFiberDistancesData",
                                   dynamicConnectivityFiberDistancesData);
    }
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
CiftiFiberTrajectoryDenseDynamicFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    CiftiMappableDataFile::restoreFileDataFromScene(sceneAttributes,
                                                    sceneClass);
    m_sceneClassAssistant->restoreMembers(sceneAttributes,
                                          sceneClass);
    
    CaretAssert(m_ciftiFile);
    const int64_t columnCount(m_ciftiFile->getNumberOfColumns());
    if (columnCount > 0) {
        std::vector<float> dynamicConnectivityFiberCountsData(sceneClass->getFloatVector("dynamicConnectivityFiberCountsData"));
        if (static_cast<int64_t>(dynamicConnectivityFiberCountsData.size()) != columnCount) {
            dynamicConnectivityFiberCountsData.resize(columnCount);
            std::fill(dynamicConnectivityFiberCountsData.begin(),
                      dynamicConnectivityFiberCountsData.end(),
                      0.0);
        }
        m_ciftiFile->setRow(&dynamicConnectivityFiberCountsData[0],
                            s_fiberCountsMapIndex);
        
        std::vector<float> dynamicConnectivityFiberDistancesData(sceneClass->getFloatVector("dynamicConnectivityFiberDistancesData"));
        if (static_cast<int64_t>(dynamicConnectivityFiberDistancesData.size()) != columnCount) {
            dynamicConnectivityFiberDistancesData.resize(columnCount);
            std::fill(dynamicConnectivityFiberDistancesData.begin(),
                      dynamicConnectivityFiberDistancesData.end(),
                      0.0);
        }
        m_ciftiFile->setRow(&dynamicConnectivityFiberDistancesData[0],
                            s_fiberDistancesMapIndex);
    }
}

/**
 * True if dynamically loaded data is valid
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isDataValid() const
{
    return false;
}

/**
 * @return True if this file supports enabled as a connectivity layer
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isEnabledAsLayerSupported() const
{
    return true;
}

/**
 * @return True if this file is enabled as a connectivity layer
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isEnabledAsLayer() const
{
    return m_enabledAsLayerFlag;
}

/**
 * Setf this file is enabled as a connectivity layer
 * @param enabled
 *    New status
 */
void
CiftiFiberTrajectoryDenseDynamicFile::setEnabledAsLayer(const bool enabled)
{
    m_enabledAsLayerFlag = enabled;
}


/**
 * @return The parent fiber trajectory file
 */
CiftiFiberTrajectoryFile*
CiftiFiberTrajectoryDenseDynamicFile::getParentFiberTrajectoryFile()
{
    return m_parentFiberTrajectoryFile;
}

/**
 * @return The parent fiber trajectory file (const method)
 */
const CiftiFiberTrajectoryFile*
CiftiFiberTrajectoryDenseDynamicFile::getParentFiberTrajectoryFile() const
{
    return m_parentFiberTrajectoryFile;
}

/**
 * Set the dynamic connectivity data (called by CiftiFiberTrajectoryFile)
 * @param dynamicConnectivityFiberCountsData
 *    The fiber counts
 * @param dynamicConnectivityFiberDistancesData
 *    The fiber distances
 * @param dataLoadingName
 *    Name of data loaded
 */
void
CiftiFiberTrajectoryDenseDynamicFile::setDynamicConnectivityData(const std::vector<float>& dynamicConnectivityFiberCountsData,
                                                                 const std::vector<float>& dynamicConnectivityFiberDistancesData,
                                                                 const AString& dataLoadingName)
{
    CaretAssert(m_ciftiFile);
    CaretAssert(m_ciftiFile->getNumberOfColumns() == static_cast<int64_t>(dynamicConnectivityFiberCountsData.size()));
    CaretAssert(m_ciftiFile->getNumberOfColumns() == static_cast<int64_t>(dynamicConnectivityFiberDistancesData.size()));
    
    m_ciftiFile->setRow(&dynamicConnectivityFiberCountsData[0],
                        s_fiberCountsMapIndex);
    m_ciftiFile->setRow(&dynamicConnectivityFiberDistancesData[0],
                        s_fiberDistancesMapIndex);
    
    m_dataLoadingName = dataLoadingName;
    
    const AString loadingName(m_dataLoadingName.replace("_", " "));
    setMapName(s_fiberCountsMapIndex,
               "Fiber Counts " + loadingName);
    setMapName(s_fiberDistancesMapIndex,
               "Fiber Distances " + loadingName);
    invalidateColoringInAllMaps();
}

/**
 * @return Name from data loading
 */
AString
CiftiFiberTrajectoryDenseDynamicFile::getDataLoadingName() const
{
    return m_dataLoadingName;
}


/**
 * Load data for the given surface node.
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param nodeIndex
 *    Index of the surface node.
 * @return
 *    Index of row that was loaded or -1 if no data was found for node.
 */
int64_t
CiftiFiberTrajectoryDenseDynamicFile::loadDataForSurfaceNode(const StructureEnum::Enum /*structure*/,
                                                             const int32_t /*surfaceNumberOfNodes*/,
                                                             const int32_t /*nodeIndex*/)
{
    /* Data is loaded by parent fiber trajectory file */
    return -1;
}

/**
 * Load average data for the given surface nodes.
 *
 * @param structure
 *    Structure in which surface node is located.
 * @param surfaceNumberOfNodes
 *    Number of nodes in surface.
 * @param nodeIndices
 *    Indices of the surface nodes.
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::loadDataAverageForSurfaceNodes(const StructureEnum::Enum /*structure*/,
                                                                     const int32_t /*surfaceNumberOfNodes*/,
                                                                     const std::vector<int32_t>& /*nodeIndices*/)
{
    /* Data is loaded by parent fiber trajectory file */
    return false;
}

/**
 * Load data for a voxel at the given coordinate.
 *
 * @param xyz
 *    Coordinate of voxel.
 * @return
 *    Index of row that was loaded or -1 if no data was found for coordinate.
 */
int64_t
CiftiFiberTrajectoryDenseDynamicFile::loadDataForVoxelAtCoordinate(const float* /* xyz[3]*/)
{
    /* Data is loaded by parent fiber trajectory file */
    return -1;
}

/**
 * Load connectivity data for the voxel indices and then average the data.
 *
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices of voxels.
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::loadAverageDataForVoxelIndices(const int64_t* /*volumeDimensionIJK[3]*/,
                                                                     const std::vector<VoxelIJK>& /*voxelIndices*/)
{
    /* Data is loaded by parent fiber trajectory file */
    return false;
}

/**
 * Load the given row index from the file even if the file is disabled for data loading
 *
 * @param rowIndex
 *    Index of row that is loaded.
 * @return
 *    True if data loaded, else false
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::loadDataForRowIndex(const int64_t /*rowIndex*/)
{
    /* Data is loaded by parent fiber trajectory file */
    return false;
}

/**
 * @return Is data loading supported?
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isDataLoadingSupported() const
{
    /*
     * If loading is disabled by parent fiber trajectory file,
     * this file will not get updated.
     */
    return false;
}

/**
 * @return Is data loading enabled?
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isDataLoadingEnabled() const
{
    return m_dataLoadingEnabled;
}

/**
 * Set data loading enabled.
 *
 * @param loadingEnabled
 *    New status of data loading.
 */
void
CiftiFiberTrajectoryDenseDynamicFile::setDataLoadingEnabled(const bool loadingEnabled)
{
    m_dataLoadingEnabled = loadingEnabled;
}

/**
 * @return True if matching fiber orientation is supported, else false.
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isMatchingFiberOrientationFileSupported() const
{
    return false;
}

/**
 * @return The selected matching fiber orientation file. May be NULL.
 */
const CiftiFiberOrientationFile*
CiftiFiberTrajectoryDenseDynamicFile::getMatchingFiberOrientationFile() const
{
    /* Not supported */
    return NULL;
}

/**
 * @return The selected matching fiber orientation file. May be NULL.
 */
CiftiFiberOrientationFile*
CiftiFiberTrajectoryDenseDynamicFile::getMatchingFiberOrientationFile()
{
    /* Not supported */
    return NULL;
}

/**
 * Is the given fiber orientation file compatible with this fiber trajectory file
 *
 * @param fiberOrientationFile
 *    File tested for compatibilty
 * @return
 *    True if file is compatible, else false.
 */
bool
CiftiFiberTrajectoryDenseDynamicFile::isFiberOrientationFileCombatible(const CiftiFiberOrientationFile* /*fiberOrientationFile*/) const
{
    /* Not supported */
    return false;
}


/**
 * Set the selected matching fiber orientation file.  No test of compatibility
 * is made.  If this is a "single row" trajectory file, the data for the single
 * row is loaded.
 *
 * @param matchingFiberOrientationFile
 *    New selection for matching fiber orientation file.
 */
void
CiftiFiberTrajectoryDenseDynamicFile::setMatchingFiberOrientationFile(CiftiFiberOrientationFile* /*matchingFiberOrientationFile*/)
{
    /* Not supported */
}

/**
 * Update the matching fiber orientation file from the first compatible file in the list.
 * If none are found, the matching file will become NULL.  If the current matching file
 * is valid, no action is taken.
 *
 * @param matchingFiberOrientationFiles
 *    The fiber orientation files.
 */
void
CiftiFiberTrajectoryDenseDynamicFile::updateMatchingFiberOrientationFileFromList(std::vector<CiftiFiberOrientationFile*> /*matchingFiberOrientationFiles*/)
{
    /* Not supported */
}
