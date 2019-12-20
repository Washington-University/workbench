
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_DECLARE__
#include "EventCaretMappableDataFilesAndMapsInDisplayedOverlays.h"
#undef __EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventCaretMappableDataFilesAndMapsInDisplayedOverlays 
 * \brief Get caret mappable files and their map indices in displayed overlays
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::EventCaretMappableDataFilesAndMapsInDisplayedOverlays()
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_AND_MAPS_IN_DISPLAYED_OVERLAYS)
{
    
}

/**
 * Destructor.
 */
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::~EventCaretMappableDataFilesAndMapsInDisplayedOverlays()
{
}

/**
 * Add file and map displayed in an chart one overlay.
 *
 * @param mapFile
 *     File to add.
 * @param mapIndex
 *     Index of selected map.
 * @param tabIndex
 * Index of the tab
 */
void
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::addChartOneFileAndMap(CaretMappableDataFile* mapFile,
                                                                             const int32_t mapIndex,
                                                                             const int32_t tabIndex)
{
    if ( ! satisfiesConstraints(tabIndex)) {
        return;
    }
    
    {
    auto iter = m_mapFilesAndIndices.find(mapFile);
    if (iter != m_mapFilesAndIndices.end()) {
        iter->second.insert(mapIndex);
    }
    else {
        std::set<int32_t> indicesSet;
        indicesSet.insert(mapIndex);
        m_mapFilesAndIndices.insert(std::make_pair(mapFile,
                                                   indicesSet));
    }
    }
    
    auto iter = m_chartOneMapFilesAndIndices.find(mapFile);
    if (iter != m_chartOneMapFilesAndIndices.end()) {
        iter->second.insert(mapIndex);
    }
    else {
        std::set<int32_t> indicesSet;
        indicesSet.insert(mapIndex);
        m_chartOneMapFilesAndIndices.insert(std::make_pair(mapFile,
                                                                indicesSet));
    }
}

/**
 * Add file and map displayed in an chart two overlay.
 *
 * @param mapFile
 *     File to add.
 * @param mapIndex
 *     Index of selected map.
 * @param tabIndex
 * Index of the tab
 */
void
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::addChartTwoFileAndMap(CaretMappableDataFile* mapFile,
                                                                             const int32_t mapIndex,
                                                                             const int32_t tabIndex)
{
    if ( ! satisfiesConstraints(tabIndex)) {
        return;
    }
    
    {
        auto iter = m_mapFilesAndIndices.find(mapFile);
        if (iter != m_mapFilesAndIndices.end()) {
            iter->second.insert(mapIndex);
        }
        else {
            std::set<int32_t> indicesSet;
            indicesSet.insert(mapIndex);
            m_mapFilesAndIndices.insert(std::make_pair(mapFile,
                                                       indicesSet));
        }
    }
    
    auto iter = m_chartTwoMapFilesAndIndices.find(mapFile);
    if (iter != m_chartTwoMapFilesAndIndices.end()) {
        iter->second.insert(mapIndex);
    }
    else {
        std::set<int32_t> indicesSet;
        indicesSet.insert(mapIndex);
        m_chartTwoMapFilesAndIndices.insert(std::make_pair(mapFile,
                                                           indicesSet));
    }
}

/**
 * Add file and map displayed in an brainordinate (surface/volume) overlay.
 *
 * @param mapFile
 *     File to add.
 * @param mapIndex
 *     Index of selected map.
 * @param tabIndex
 * Index of the tab
 */
void
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::addBrainordinateFileAndMap(CaretMappableDataFile* mapFile,
                                                                                  const int32_t mapIndex,
                                                                                  const int32_t tabIndex)
{
    if ( ! satisfiesConstraints(tabIndex)) {
        return;
    }
    
    {
    auto iter = m_mapFilesAndIndices.find(mapFile);
    if (iter != m_mapFilesAndIndices.end()) {
        iter->second.insert(mapIndex);
    }
    else {
        std::set<int32_t> indicesSet;
        indicesSet.insert(mapIndex);
        m_mapFilesAndIndices.insert(std::make_pair(mapFile,
                                                   indicesSet));
    }
    }
    
    auto iter = m_surfaceVolumeMapFilesAndIndices.find(mapFile);
    if (iter != m_surfaceVolumeMapFilesAndIndices.end()) {
        iter->second.insert(mapIndex);
    }
    else {
        std::set<int32_t> indicesSet;
        indicesSet.insert(mapIndex);
        m_surfaceVolumeMapFilesAndIndices.insert(std::make_pair(mapFile,
                                                   indicesSet));
    }
}

/**
 * @return Files and maps selected in overlays for both brainordinates
 * (surfaces and volumes) and charts.
 */
std::vector<EventCaretMappableDataFilesAndMapsInDisplayedOverlays::FileInfo>
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::getFilesAndMaps() const
{
    std::vector<FileInfo> infoOut;
    
    for (auto iter : m_surfaceVolumeMapFilesAndIndices) {
        infoOut.push_back(FileInfo(OverlayType::BRAINORDINATE,
                                   iter.first,
                                   iter.second));
    }
    for (auto iter : m_chartOneMapFilesAndIndices) {
        infoOut.push_back(FileInfo(OverlayType::CHART_ONE,
                                   iter.first,
                                   iter.second));
    }
    for (auto iter : m_chartTwoMapFilesAndIndices) {
        infoOut.push_back(FileInfo(OverlayType::CHART_TWO,
                                   iter.first,
                                   iter.second));
    }

    return infoOut;
}


/**
 * @return Map containing with each pair a map file and map indices selected for the file.
 */
std::map<CaretMappableDataFile*, std::set<int32_t>>
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::getMapFilesAndIndices() const
{
    return m_mapFilesAndIndices;
}

/**
 * Test to see if the given window index and tab index satisfy optional constraints
 * @param tabIndex
 * Index of the tab
 */
bool
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::satisfiesConstraints(const int32_t tabIndex)
{
    if (m_windowIndex >= 0) {
//        if (windowIndex != m_windowIndex) {
//            return false;
//        }
    }
    
    if ( ! m_tabIndices.empty()) {
        if (m_tabIndices.find(tabIndex) == m_tabIndices.end()) {
            return false;
        }
    }
    
    return true;
}

/**
 * Set a constraint to only get overlay for the given window
 * @param windowIndex
 * Index of the window
 */
void
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::setWindowIndexConstraint(const int32_t windowIndex)
{
    m_windowIndex = windowIndex;
}

/**
 * Set a constraint to only get overlay for the given tabs
 * @param tabIndices
 * Indices of the tabs
 */
void
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::setTabIndicesConstraint(const std::set<int32_t>& tabIndices)
{
    m_tabIndices = tabIndices;
}

/**
 * Constructor.
 * 
 * @param overlayType
 *     Type of overlay
 * @param mapFile
 *     Map file in the overlay(s)
 * @param mapIndices
 *     Indices of maps selected in overlays
 */
EventCaretMappableDataFilesAndMapsInDisplayedOverlays::FileInfo::FileInfo(OverlayType overlayType,
                                                                          CaretMappableDataFile* mapFile,
                                                                          const std::set<int32_t>& mapIndices)
: m_overlayType(overlayType),
m_mapFile(mapFile),
m_mapIndices(mapIndices)
{
}

