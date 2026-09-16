
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

#define __DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_DECLARE__
#include "DynamicConnectivityFileLoadingManager.h"
#undef __DYNAMIC_CONNECTIVITY_FILE_LOADING_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "DynamicConnectivityFileInterface.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "EventBrowserTabGetAllViewed.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventSurfaceNodesGetNearXYZ.h"
#include "HtmlTableBuilder.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "ScenePrimitiveArray.h"
#include "Surface.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::DynamicConnectivityFileLoadingManager
 * \brief Manages loading data from cifti connectivity files
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param brain
 *    Brain that uses this instance.
 */
DynamicConnectivityFileLoadingManager::DynamicConnectivityFileLoadingManager()
: CaretObject()
{
}

/**
 * Destructor.
 */
DynamicConnectivityFileLoadingManager::~DynamicConnectivityFileLoadingManager()
{
}

/**
 * @return All connectivity files using the given yoking group
 * @param brain
 *    Brain from which to get files
 * @param yokingGroup
 *    Desired yoking group
 */
std::vector<ConnectivityFileInterface*>
DynamicConnectivityFileLoadingManager::getConnectivityFilesWithYokingGroup(Brain* brain,
                                                                           const GeneralYokingGroupEnum::Enum yokingGroup) const
{
    std::vector<ConnectivityFileInterface*> filesOut;
    
    std::vector<ConnectivityFileInterface*> allConnFiles(brain->getAllConnectivityFiles());
    for (ConnectivityFileInterface* cf : allConnFiles) {
        DynamicConnectivityFileInterface* dynConnFile(dynamic_cast<DynamicConnectivityFileInterface*>(cf));
        if (dynConnFile != NULL) {
            /*
             * Only use dynamic connectivity file if it matches the yoking group
             */
            if (yokingGroup == dynConnFile->getDynamicConnectivityYokingGroup()) {
                filesOut.push_back(cf);
            }
        }
        else {
            /*
             * for NON-dynamic (just connectivity) only use when requested
             * yoking group is off.
             */
            if (yokingGroup == GeneralYokingGroupEnum::OFF) {
                filesOut.push_back(cf);
            }
        }
    }
    
    return filesOut;
}


/**
 * Load row from the given parcel file.
 *
 * @param brain
 *    Brain for which data is loaded.
 * @param parcelFile
 *    The parcel file.
 * @param rowIndex
 *    Index of the row.
 * @param columnIndex
 *    Index of the column.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @return
 *    true if success, else false.
 */
bool
DynamicConnectivityFileLoadingManager::loadRowOrColumnFromParcelFile(Brain* brain,
                                                                      CiftiConnectivityMatrixParcelFile* parcelFile,
                                                                      const int32_t rowIndex,
                                                                      const int32_t columnIndex,
                                                                      std::vector<AString>& rowColumnInformationOut,
                                                                      HtmlTableBuilder& htmlTableBuilder)
{
    CaretAssert(parcelFile);
    
    int32_t rowColumnIndexToLoad = -1;
    switch (parcelFile->getMatrixLoadingDimension()) {
        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
            rowColumnIndexToLoad = columnIndex;
            break;
        case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
            rowColumnIndexToLoad = rowIndex;
            break;
    }
    
    /*
     * If yoked, find other files yoked to the same group
     */
    const YokingGroupEnum::Enum selectedYokingGroup = parcelFile->getYokingGroup();
    std::vector<CiftiConnectivityMatrixParcelFile*> parcelFilesToLoadFrom;
    if (selectedYokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
        for (int32_t i = 0; i < brain->getNumberOfConnectivityMatrixParcelFiles(); i++) {
            CiftiConnectivityMatrixParcelFile* pf = brain->getConnectivityMatrixParcelFile(i);
            if (pf->getYokingGroup() == selectedYokingGroup) {
                parcelFilesToLoadFrom.push_back(pf);
            }
        }
    }
    else {
        parcelFilesToLoadFrom.push_back(parcelFile);
    }
    
    const int32_t mapIndex = 0;
    
    /*
     * Load row/color for the "parcelFile" and any other files with
     * which it is yoked.
     */
    for (std::vector<CiftiConnectivityMatrixParcelFile*>::iterator iter = parcelFilesToLoadFrom.begin();
         iter != parcelFilesToLoadFrom.end();
         iter++) {
        CiftiConnectivityMatrixParcelFile* pf = *iter;
        switch (pf->getMatrixLoadingDimension()) {
            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                pf->loadDataForColumnIndex(rowColumnIndexToLoad);
                pf->updateScalarColoringForMap(mapIndex);
                rowColumnInformationOut.push_back(pf->getFileNameNoPath()
                                                  + " column index="
                                                  + AString::number(rowColumnIndexToLoad + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                htmlTableBuilder.addRow(("Column Index: " + AString::number(rowColumnIndexToLoad + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                         pf->getFileNameNoPath());
                break;
            case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                pf->loadDataForRowIndex(rowColumnIndexToLoad);
                pf->updateScalarColoringForMap(mapIndex);
                rowColumnInformationOut.push_back(pf->getFileNameNoPath()
                                                  + " row index="
                                                  + AString::number(rowColumnIndexToLoad + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                htmlTableBuilder.addRow(("Row Index: " + AString::number(rowColumnIndexToLoad + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                         pf->getFileNameNoPath());
                break;
        }
    }
    
    return true;
}

bool
DynamicConnectivityFileLoadingManager::loadRowOrColumnFromConnectivityMatrixFile(
                                                                                  DynamicConnectivityFileInterface* ciftiConnMatrixFile,
                                                                                  const int32_t rowIndex,
                                                                                  const int32_t columnIndex,
                                                                                  std::vector<AString>& rowColumnInformationOut,
                                                                                  HtmlTableBuilder& htmlTableBuilder)
{
    const int32_t mapIndex = 0;
    
    CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(ciftiConnMatrixFile));
    CaretAssert(mapFile);
    if (rowIndex >= 0) {
        ciftiConnMatrixFile->loadDataForRowIndex(rowIndex);
        mapFile->updateScalarColoringForMap(mapIndex);
        rowColumnInformationOut.push_back(mapFile->getFileNameNoPath()
                                          + " row index="
                                          + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
        htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                mapFile->getFileNameNoPath());
    }
    else if (columnIndex >= 0) {
        ciftiConnMatrixFile->loadDataForColumnIndex(columnIndex);
        mapFile->updateScalarColoringForMap(mapIndex);
        rowColumnInformationOut.push_back(mapFile->getFileNameNoPath()
                                          + " column index="
                                          + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
        htmlTableBuilder.addRow(("Column Index: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                mapFile->getFileNameNoPath());
    }
    
    return true;
}


/**
 * Load data for the given surface node index.
 * @param brain
 *    Brain for which data is loaded.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 * @param loadDataOnlyForThisFileIfNotNULL
 *    If this is not NULL, only load data for this file.  This is used when data is not found for a voxel
 *    and there is attempt to load data for the surface node nearest the voxel.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
DynamicConnectivityFileLoadingManager::loadDataForSurfaceNode(Brain* brain,
                                                               const SurfaceFile* surfaceFile,
                                                               const int32_t nodeIndex,
                                                               const CaretMappableDataFile* loadDataOnlyForThisFileIfNotNULL,
                                                               std::vector<AString>& rowColumnInformationOut,
                                                               HtmlTableBuilder& htmlTableBuilder)
{
    CaretAssert(surfaceFile);
    const AString structureAndVertexInfo(+ " "
                                         + StructureEnum::toGuiName(surfaceFile->getStructure())
                                         + " Vertex: "
                                         + AString::number(nodeIndex));
    bool dataWasLoadedForAnyFileFlag(false);
    
    std::vector<GeneralYokingGroupEnum::Enum> allYokingGroups;
    GeneralYokingGroupEnum::getAllEnums(allYokingGroups);
    for (GeneralYokingGroupEnum::Enum yokingGroup : allYokingGroups) {
        bool dataLoadedForThisYokingGroupFlag(false);
        std::vector<ConnectivityFileInterface*> allYokedConnFiles(getConnectivityFilesWithYokingGroup(brain,
                                                                                                      yokingGroup));
        for (ConnectivityFileInterface* connFile : allYokedConnFiles) {
            CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(connFile));
            CaretAssert(mapFile);
            if (loadDataOnlyForThisFileIfNotNULL != NULL) {
                if (mapFile != loadDataOnlyForThisFileIfNotNULL) {
                    continue;
                }
            }
            if ( ! mapFile->isEmpty()) {
                const int32_t mapIndex = 0;
                int64_t rowIndex = -1;
                int64_t columnIndex = -1;
                std::vector<float> brainordinateRawDataSeriesOut;
                if (connFile->loadMapDataForSurfaceNode(surfaceFile->getNumberOfNodes(),
                                                        surfaceFile->getStructure(),
                                                        nodeIndex,
                                                        rowIndex,
                                                        columnIndex,
                                                        brainordinateRawDataSeriesOut)) {
                    mapFile->updateScalarColoringForMap(mapIndex);
                    dataLoadedForThisYokingGroupFlag = true;
                    dataWasLoadedForAnyFileFlag = true;
                }
                
                if (dataLoadedForThisYokingGroupFlag) {
                    if (rowIndex >= 0) {
                        /*
                         * Get row/column info for node
                         */
                        rowColumnInformationOut.push_back(mapFile->getFileNameNoPath()
                                                          + " vertex index="
                                                          + AString::number(nodeIndex)
                                                          + ", row index="
                                                          + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                        htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                                (mapFile->getFileNameNoPath() + structureAndVertexInfo));
                    }
                    else if (columnIndex >= 0) {
                        /*
                         * Get row/column info for node
                         */
                        rowColumnInformationOut.push_back(mapFile->getFileNameNoPath()
                                                          + " vertex index="
                                                          + AString::number(nodeIndex)
                                                          + ", column index="
                                                          + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                        htmlTableBuilder.addRow(("Column Index: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                                (mapFile->getFileNameNoPath() + structureAndVertexInfo));
                    }

                    if (yokingGroup != GeneralYokingGroupEnum::OFF) {
                        /*
                         * Only dynamic connectivity files will have a "non-OFF" yoking group
                         */
                        correlateWithOtherFiles(allYokedConnFiles,
                                                mapFile,
                                                brainordinateRawDataSeriesOut);
                        
                        /*
                         * Get out of dyn file loop
                         */
                        break;
                    }
                }
            }
        }
    }
    
    if (dataWasLoadedForAnyFileFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return dataWasLoadedForAnyFileFlag;
}

/**
 * Correlate the given correlation data with data in other data files
 * @param allYokedDynFiles
 *    All dynamic connectivity files yoked to the 'mapFileThatLoadedData'
 * param mapFIleThatLoadedData
 *    File that is  correltrated witrh other files
 * @param brainordinateSeriesData
 *    The numerical data used for correlation
 */
void
DynamicConnectivityFileLoadingManager::correlateWithOtherFiles(std::vector<ConnectivityFileInterface*> allYokedConnFiles,
                                                               const CaretMappableDataFile* mapFileThatLoadedData,
                                                               std::vector<float>& brainordinateSeriesData)
{
    std::vector<DynamicConnectivityFileInterface*> allYokedDynFiles;
    for (ConnectivityFileInterface* cf : allYokedConnFiles) {
        DynamicConnectivityFileInterface* dcf(dynamic_cast<DynamicConnectivityFileInterface*>(cf));
        if (dcf != NULL) {
            allYokedDynFiles.push_back(dcf);
        }
    }
    if ( ! allYokedDynFiles.empty()) {
        correlateWithOtherFiles(allYokedDynFiles,
                                mapFileThatLoadedData,
                                brainordinateSeriesData);
    }
}

/**
 * Correlate the given correlation data with data in other data files
 * @param allYokedDynFiles
 *    All dynamic connectivity files yoked to the 'mapFileThatLoadedData'
 * param mapFIleThatLoadedData
 *    File that is  correltrated witrh other files
 * @param brainordinateSeriesData
 *    The numerical data used for correlation
 */
void
DynamicConnectivityFileLoadingManager::correlateWithOtherFiles(std::vector<DynamicConnectivityFileInterface*> allYokedDynFiles,
                                                               const CaretMappableDataFile* mapFileThatLoadedData,
                                                               std::vector<float>& brainordinateSeriesData)
{
    if (brainordinateSeriesData.empty()) {
        return;
    }
    
    const int64_t dataStride(1);
    
    const DynamicConnectivityFileInterface* dcfi(dynamic_cast<const DynamicConnectivityFileInterface*>(mapFileThatLoadedData));
    CaretAssert(dcfi);
    const bool noDemeanFlag(dcfi->getCorrelationSettings()->isCorrelationNoDemeanEnabled());
    ConnectivityCorrelationTwo::DataSet dataSet(ConnectivityCorrelationTwo::createDataSet(brainordinateSeriesData.data(),
                                                                                          brainordinateSeriesData.size(),
                                                                                          dataStride,
                                                                                          noDemeanFlag));
    for (DynamicConnectivityFileInterface* yokedDynFile : allYokedDynFiles) {
        if (yokedDynFile != dcfi) {
            yokedDynFile->loadDataForCorrelationWithDataSet(dataSet, "Correlation");
        }
    }

}
/**
 * Load data for each of the given surface node indices and average the data.
 * @param brain
 *    Brain for which data is loaded.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndices
 *    Indices of the surface nodes.
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
DynamicConnectivityFileLoadingManager::loadAverageDataForSurfaceNodes(Brain* brain,
                                                                       const SurfaceFile* surfaceFile,
                                                                       const std::vector<int32_t>& nodeIndices)
{
    bool dataWasLoadedForAnyFileFlag(false);
    std::vector<GeneralYokingGroupEnum::Enum> allYokingGroups;
    GeneralYokingGroupEnum::getAllEnums(allYokingGroups);
    for (GeneralYokingGroupEnum::Enum yokingGroup : allYokingGroups) {
        bool dataLoadedForThisYokingGroupFlag(false);
        std::vector<ConnectivityFileInterface*> allYokedConnFiles(getConnectivityFilesWithYokingGroup(brain,
                                                                                                       yokingGroup));
        for (ConnectivityFileInterface* connFile : allYokedConnFiles) {
            CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(connFile));
            CaretAssert(mapFile);
            if ( ! mapFile->isEmpty()) {
                const int32_t mapIndex = 0;
                std::vector<float> correlationData;
                if (connFile->loadMapAverageDataForSurfaceNodes(surfaceFile->getNumberOfNodes(),
                                                               surfaceFile->getStructure(),
                                                               nodeIndices,
                                                                correlationData)) {
                    dataLoadedForThisYokingGroupFlag = true;
                    dataWasLoadedForAnyFileFlag = true;
                }
                    
                mapFile->updateScalarColoringForMap(mapIndex);
                    
                if (dataLoadedForThisYokingGroupFlag) {
                    /*
                     * Not sure how to correlate with average
                     */
                    const bool correlateWithOthersFlag(false);
                    if (correlateWithOthersFlag) {
                        if (yokingGroup != GeneralYokingGroupEnum::OFF) {
                            /*
                             * Correlate with other files using same yoking group
                             */
                            correlateWithOtherFiles(allYokedConnFiles,
                                                    mapFile,
                                                    correlationData);
                            /*
                             * Get out of allYokedDynFiles loop and move on to next yoking group
                             */
                            break;
                        }
                    }
                }
            }
        }
    }

    if (dataWasLoadedForAnyFileFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return dataWasLoadedForAnyFileFlag;
}

/**
 * Load data for the voxel near the given coordinate.
 * @param brain
 *    Brain for which data is loaded.
 * @param xyz
 *     Coordinate of voxel.
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
DynamicConnectivityFileLoadingManager::loadDataForVoxelAtCoordinate(Brain* brain,
                                                                     const float xyz[3],
                                                                     std::vector<AString>& rowColumnInformationOut,
                                                                     HtmlTableBuilder& htmlTableBuilder)
{
    const AString voxelInfo(" Voxel: "
                            + AString::fromNumbers(xyz, 3, ","));
    
    bool dataWasLoadedForAnyFileFlag(false);
    
    std::vector<GeneralYokingGroupEnum::Enum> allYokingGroups;
    GeneralYokingGroupEnum::getAllEnums(allYokingGroups);
    for (GeneralYokingGroupEnum::Enum yokingGroup : allYokingGroups) {
        bool dataLoadedForThisYokingGroupFlag(false);
        std::vector<ConnectivityFileInterface*> allYokedConnFiles(getConnectivityFilesWithYokingGroup(brain,
                                                                                                       yokingGroup));
        for (ConnectivityFileInterface* connFile : allYokedConnFiles) {
            CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(connFile));
            CaretAssert(mapFile);
            if (mapFile->isEmpty() == false) {
                const int32_t mapIndex = 0;
                int64_t rowIndex;
                int64_t columnIndex;
                std::vector<float> correlationData;
                if (connFile->loadMapDataForVoxelAtCoordinate(mapIndex,
                                                              xyz,
                                                              rowIndex,
                                                              columnIndex,
                                                              correlationData)) {
                    mapFile->updateScalarColoringForMap(mapIndex);
                    dataWasLoadedForAnyFileFlag = true;
                    dataLoadedForThisYokingGroupFlag = true;
                    
                    if (rowIndex >= 0) {
                        /*
                         * Get row/column info for node
                         */
                        rowColumnInformationOut.push_back(mapFile->getFileNameNoPath()
                                                          + " Voxel XYZ="
                                                          + AString::fromNumbers(xyz, 3, ",")
                                                          + ", row index="
                                                          + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                        htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                                (mapFile->getFileNameNoPath() + voxelInfo));
                    }
                    else if (columnIndex >= 0) {
                        /*
                         * Get row/column info for node
                         */
                        rowColumnInformationOut.push_back(mapFile->getFileNameNoPath()
                                                          + " Voxel XYZ="
                                                          + AString::fromNumbers(xyz, 3, ",")
                                                          + ", column index="
                                                          + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                        htmlTableBuilder.addRow(("Column Index: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                                (mapFile->getFileNameNoPath() + voxelInfo));
                    }
                    else {
                        if (yokingGroup == GeneralYokingGroupEnum::OFF) {
                            /*
                             * Look for nearby surface nodes
                             * 'maxDist' is maximum distance a coordinate may be
                             * from the query (XYZ).
                             */
                            const float maxDist(2.0);
                            EventSurfaceNodesGetNearXYZ nearbyNodesEvent(xyz,
                                                                         maxDist);
                            EventManager::get()->sendEvent(nearbyNodesEvent.getPointer());
                            const int32_t numNearbyNodes(nearbyNodesEvent.getNumberOfNearbyNodes());
                            for (int32_t i = 0; i < numNearbyNodes; i++) {
                                const EventSurfaceNodesGetNearXYZ::NodeInfo nodeInfo(nearbyNodesEvent.getNearbyNode(i));
                                
                                const bool dataValidFlag = loadDataForSurfaceNode(brain,
                                                                                  nodeInfo.getSurfaceFile(),
                                                                                  nodeInfo.getNodeIndex(),
                                                                                  mapFile, /* only try loading data for this file */
                                                                                  rowColumnInformationOut,
                                                                                  htmlTableBuilder);
                                if (dataValidFlag) {
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (dataLoadedForThisYokingGroupFlag) {
                        if (yokingGroup != GeneralYokingGroupEnum::OFF) {
                            /*
                             * Correlate with other files using same yoking group
                             */
                            correlateWithOtherFiles(allYokedConnFiles,
                                                    mapFile,
                                                    correlationData);
                            /*
                             * Get out of allYokedDynFiles loop and move on to next yoking group
                             */
                            break;
                        }
                    }
                }
            }
        }
    }
    
    if (dataWasLoadedForAnyFileFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return dataWasLoadedForAnyFileFlag;
}

/**
 * Load average data for the given voxel indices.
 *
 * @param brain
 *    Brain for which data is loaded.
 * @param volumeDimensionIJK
 *    Dimensions of the volume.
 * @param voxelIndices
 *    Indices for averaging of data.
 * @return
 *    true if any data was loaded, else false.
 * @throw DataFileException
 *    If an error occurs.
 */
bool
DynamicConnectivityFileLoadingManager::loadAverageDataForVoxelIndices(Brain* brain,
                                                                       const int64_t volumeDimensionIJK[3],
                                                                       const std::vector<VoxelIJK>& voxelIndices)
{
    bool dataWasLoadedForAnyFileFlag(false);
    std::vector<GeneralYokingGroupEnum::Enum> allYokingGroups;
    GeneralYokingGroupEnum::getAllEnums(allYokingGroups);
    for (GeneralYokingGroupEnum::Enum yokingGroup : allYokingGroups) {
        bool dataLoadedForThisYokingGroupFlag(false);
        std::vector<ConnectivityFileInterface*> allYokedConnFiles(getConnectivityFilesWithYokingGroup(brain,
                                                                                                       yokingGroup));
        for (ConnectivityFileInterface* connFile : allYokedConnFiles) {
            CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(connFile));
            CaretAssert(mapFile);
            if (mapFile->isEmpty() == false) {
                const int32_t mapIndex = 0;
                std::vector<float> correlationData;
                if (connFile->loadMapAverageDataForVoxelIndices(mapIndex,
                                                                volumeDimensionIJK,
                                                                voxelIndices,
                                                                correlationData)) {
                    dataLoadedForThisYokingGroupFlag = true;
                    dataWasLoadedForAnyFileFlag = true;
                }
                
                mapFile->updateScalarColoringForMap(mapIndex);
                
                if (dataLoadedForThisYokingGroupFlag) {
                    /*
                     * Not sure how to correlate with average
                     */
                    const bool correlateWithOthersFlag(false);
                    if (correlateWithOthersFlag) {
                        if (yokingGroup != GeneralYokingGroupEnum::OFF) {
                            /*
                             * Correlate with other files using same yoking group
                             */
                            correlateWithOtherFiles(allYokedConnFiles,
                                                    mapFile,
                                                    correlationData);
                            /*
                             * Get out of allYokedDynFiles loop and move on to next yoking group
                             */
                            break;
                        }
                    }
                }
            }
        }
    }

    if (dataWasLoadedForAnyFileFlag) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return dataWasLoadedForAnyFileFlag;
}

/**
 * @param brain
 *    Brain for containing network files.
 *
 * @return True if there are enabled connectivity
 * files that retrieve data from the network.
 */
bool
DynamicConnectivityFileLoadingManager::hasNetworkFiles(Brain* brain) const
{
    const std::vector<ConnectivityFileInterface*> allConnFiles(brain->getAllConnectivityFiles());
    
    for (ConnectivityFileInterface* cf : allConnFiles) {
        CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(cf));
        CaretAssert(mapFile);
        if (mapFile->isEmpty() == false) {
            if (DataFile::isFileOnNetwork(mapFile->getFileName())) {
                if (cf->isMapDataLoadingEnabled()) {
                    return true;
                }
            }
        }
    }
    
    return false;
}


