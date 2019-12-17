
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

#define __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_DECLARE__
#include "CiftiConnectivityMatrixDataFileManager.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_DATA_FILE_MANAGER_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "EventBrowserTabGetAllViewed.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "HtmlTableBuilder.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "ScenePrimitiveArray.h"
#include "Surface.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * \class caret::CiftiConnectivityMatrixDataFileManager
 * \brief Manages loading data from cifti connectivity files
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param brain
 *    Brain that uses this instance.
 */
CiftiConnectivityMatrixDataFileManager::CiftiConnectivityMatrixDataFileManager()
: CaretObject()
{
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixDataFileManager::~CiftiConnectivityMatrixDataFileManager()
{
}

/**
 * Get the connectivity files that are displayed.
 *
 * @param brain
 *    Brain for which data is loaded.
 * @param ciftiMatrixFilesOut
 *    Output with displayed files.
 */
void
CiftiConnectivityMatrixDataFileManager::getDisplayedConnectivityMatrixFiles(Brain* brain,
                                   std::vector<CiftiMappableConnectivityMatrixDataFile*>& ciftiMatrixFilesOut) const
{
    ciftiMatrixFilesOut.clear();
    
    /*
     * WB-633 New requirements for updating of connectivity data
     * Implementation on hold and explained in Jira.
     *
     * 7/15/2016 - USE ALL FILES, DO NOT TEST FOR DISPLAYED VS NOT-DISPLAYED FILES
     */
    const bool allFilesFlag = true;
    if (allFilesFlag) {
        /*
         * All cifti matrix files, even if not displayed?
         */
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFilesOut);
        return;
    }
    
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);

    
    /*
     * Get all browser tabs and only save transformations for tabs
     * that are valid.
     */
    EventBrowserTabGetAllViewed getViewedTabs;
    EventManager::get()->sendEvent(getViewedTabs.getPointer());
    std::vector<int32_t> tabIndices = getViewedTabs.getViewdedBrowserTabIndices();
    
    std::vector<int32_t> windowIndices; // empty is all windows
    EventGetDisplayedDataFiles displayedFilesEvent(windowIndices,
                                                   tabIndices);
    EventManager::get()->sendEvent(displayedFilesEvent.getPointer());
    std::set<const CaretDataFile*> displayedDataFiles = displayedFilesEvent.getDisplayedDataFiles();
    
    /*
     * Find CIFTI matrix files that are displayed in an overlay
     */
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        if (std::find(displayedDataFiles.begin(),
                      displayedDataFiles.end(),
                      *iter) != displayedDataFiles.end()) {
            ciftiMatrixFilesOut.push_back(*iter);
        }
    }
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
CiftiConnectivityMatrixDataFileManager::loadRowOrColumnFromParcelFile(Brain* brain,
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
CiftiConnectivityMatrixDataFileManager::loadRowOrColumnFromConnectivityMatrixFile(
                                               CiftiMappableConnectivityMatrixDataFile* ciftiConnMatrixFile,
                                               const int32_t rowIndex,
                                               const int32_t columnIndex,
                                                                                  std::vector<AString>& rowColumnInformationOut,
                                                                                  HtmlTableBuilder& htmlTableBuilder)
{
    const int32_t mapIndex = 0;
    
    if (rowIndex >= 0) {
        ciftiConnMatrixFile->loadDataForRowIndex(rowIndex);
        ciftiConnMatrixFile->updateScalarColoringForMap(mapIndex);
        rowColumnInformationOut.push_back(ciftiConnMatrixFile->getFileNameNoPath()
                                          + " row index="
                                          + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
        htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                 ciftiConnMatrixFile->getFileNameNoPath());
    }
    else if (columnIndex >= 0) {
        ciftiConnMatrixFile->loadDataForColumnIndex(columnIndex);
        ciftiConnMatrixFile->updateScalarColoringForMap(mapIndex);
        rowColumnInformationOut.push_back(ciftiConnMatrixFile->getFileNameNoPath()
                                          + " column index="
                                          + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
        htmlTableBuilder.addRow(("Column Index: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                 ciftiConnMatrixFile->getFileNameNoPath());
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
 * @param rowColumnInformationOut
 *    Appends one string for each row/column loaded
 * @return
 *    true if any connectivity loaders are active, else false.
 */
bool
CiftiConnectivityMatrixDataFileManager::loadDataForSurfaceNode(Brain* brain,
                                                               const SurfaceFile* surfaceFile,
                                                               const int32_t nodeIndex,
                                                               std::vector<AString>& rowColumnInformationOut,
                                                               HtmlTableBuilder& htmlTableBuilder)
{
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    getDisplayedConnectivityMatrixFiles(brain,
                                        ciftiMatrixFiles);
    
    
    bool haveData = false;
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            int64_t rowIndex = -1;
            int64_t columnIndex = -1;
            cmf->loadMapDataForSurfaceNode(mapIndex,
                                           surfaceFile->getNumberOfNodes(),
                                           surfaceFile->getStructure(),
                                           nodeIndex,
                                           rowIndex,
                                           columnIndex);
            cmf->updateScalarColoringForMap(mapIndex);
            haveData = true;
            
            if (rowIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(cmf->getFileNameNoPath()
                                                  + " vertex index="
                                                  + AString::number(nodeIndex)
                                                  + ", row index="
                                                  + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                         cmf->getFileNameNoPath());
            }
            else if (columnIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(cmf->getFileNameNoPath()
                                                  + " vertex index="
                                                  + AString::number(nodeIndex)
                                                  + ", column index="
                                                  + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                htmlTableBuilder.addRow(("Column Index: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                         cmf->getFileNameNoPath());
            }
        }
    }
    
    if (haveData) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
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
CiftiConnectivityMatrixDataFileManager::loadAverageDataForSurfaceNodes(Brain* brain,
                                                                       const SurfaceFile* surfaceFile,
                                                                       const std::vector<int32_t>& nodeIndices)
{
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    getDisplayedConnectivityMatrixFiles(brain,
                                        ciftiMatrixFiles);
    
    bool haveData = false;
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            cmf->loadMapAverageDataForSurfaceNodes(mapIndex,
                                                   surfaceFile->getNumberOfNodes(),
                                                   surfaceFile->getStructure(),
                                                   nodeIndices);
            cmf->updateScalarColoringForMap(mapIndex);
            haveData = true;
        }
    }
    
    if (haveData) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
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
CiftiConnectivityMatrixDataFileManager::loadDataForVoxelAtCoordinate(Brain* brain,
                                                                     const float xyz[3],
                                                                     std::vector<AString>& rowColumnInformationOut,
                                                                     HtmlTableBuilder& htmlTableBuilder)
{
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    getDisplayedConnectivityMatrixFiles(brain,
                                        ciftiMatrixFiles);
    
    bool haveData = false;
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            int64_t rowIndex;
            int64_t columnIndex;
            cmf->loadMapDataForVoxelAtCoordinate(mapIndex,
                                                 xyz,
                                                 rowIndex,
                                                 columnIndex);
            cmf->updateScalarColoringForMap(mapIndex);
            haveData = true;
            
            if (rowIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(cmf->getFileNameNoPath()
                                                  + " Voxel XYZ="
                                                  + AString::fromNumbers(xyz, 3, ",")
                                                  + ", row index="
                                                  + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                htmlTableBuilder.addRow(("Row Index: " + AString::number(rowIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                         cmf->getFileNameNoPath());
            }
            else if (columnIndex >= 0) {
                /*
                 * Get row/column info for node
                 */
                rowColumnInformationOut.push_back(cmf->getFileNameNoPath()
                                                  + " Voxel XYZ="
                                                  + AString::fromNumbers(xyz, 3, ",")
                                                  + ", column index="
                                                  + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI()));
                htmlTableBuilder.addRow(("Column Index: " + AString::number(columnIndex + CiftiMappableDataFile::getCiftiFileRowColumnIndexBaseForGUI())),
                                         cmf->getFileNameNoPath());
            }
        }
    }
    
    if (haveData) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
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
CiftiConnectivityMatrixDataFileManager::loadAverageDataForVoxelIndices(Brain* brain,
                                                                       const int64_t volumeDimensionIJK[3],
                                                                       const std::vector<VoxelIJK>& voxelIndices)
{
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    getDisplayedConnectivityMatrixFiles(brain,
                                        ciftiMatrixFiles);
    
    bool haveData = false;
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmf = *iter;
        if (cmf->isEmpty() == false) {
            const int32_t mapIndex = 0;
            if (cmf->loadMapAverageDataForVoxelIndices(mapIndex,
                                                       volumeDimensionIJK,
                                                       voxelIndices)) {
            }
            haveData = true;
            
            cmf->updateScalarColoringForMap(mapIndex);
        }
    }
    
    if (haveData) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    
    return haveData;
}

/**
 * @param brain
 *    Brain for containing network files.
 *
 * @return True if there are enabled connectivity
 * files that retrieve data from the network.
 */
bool
CiftiConnectivityMatrixDataFileManager::hasNetworkFiles(Brain* brain) const
{
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    getDisplayedConnectivityMatrixFiles(brain,
                                        ciftiMatrixFiles);
    
    
    for (std::vector<CiftiMappableConnectivityMatrixDataFile*>::iterator iter = ciftiMatrixFiles.begin();
         iter != ciftiMatrixFiles.end();
         iter++) {
        CiftiMappableConnectivityMatrixDataFile* cmdf = *iter;
        
        if (cmdf->isEmpty() == false) {
            if (DataFile::isFileOnNetwork(cmdf->getFileName())) {
                const int32_t numMaps = cmdf->getNumberOfMaps();
                for (int32_t i = 0; i < numMaps; i++) {
                    if (cmdf->isMapDataLoadingEnabled(i)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}


