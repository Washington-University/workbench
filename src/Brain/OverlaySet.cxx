
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

#include <algorithm>
#include <deque>

#include <QRegExp>

#define __OVERLAY_SET_DECLARE__
#include "OverlaySet.h"
#undef __OVERLAY_SET_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventMapYokingSelectMap.h"
#include "EventMapYokingValidation.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelSurfaceMontage.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "Overlay.h"
#include "PlainTextStringBuilder.h"
#include "Scene.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class OverlaySet
 * \brief Contains a set of overlay assignments
 *
 * The maximum number of overlays is fixed.  The number
 * of overlays presented to the user varies and is
 * controlled using the ToolBox in a Browser Window.
 * 
 * The primary overlay is always the overlay at index zero.
 * The underlay is the overlay at (numberOfDisplayedOverlays - 1).
 * When models are colored, the overlays are assigned 
 * starting with the underlay and concluding with the primary
 * overlay.
 */

/**
 * \class OverlaySet
 * \brief Contains a set of overlay assignments
 *
 * The maximum number of overlays is fixed.  The number
 * of overlays presented to the user varies and is
 * controlled using the ToolBox in a Browser Window.
 * 
 * The primary overlay is always the overlay at index zero.
 * The underlay is the overlay at (numberOfDisplayedOverlays - 1).
 * When models are colored, the overlays are assigned 
 * starting with the underlay and concluding with the primary
 * overlay.
 */

/**
 * Constructor for the given surface structures, surface types, and volumes.
 *
 * @param name
 *     Name for this overlay set
 * @param tabIndex
 *     Index of tab for this overlay set.
 * @param includeSurfaceStructures
 *     Surface structures for data files displayed in this overlay set.
 * @param includeVolumeFiles
 *     Surface structures for data files displayed in this overlay set.
 */
OverlaySet::OverlaySet(const AString& name,
                       const int32_t tabIndex,
                       const std::vector<StructureEnum::Enum>& includeSurfaceStructures,
                       const Overlay::IncludeVolumeFiles includeVolumeFiles)
: CaretObject(),
m_name(name),
m_tabIndex(tabIndex),
m_includeSurfaceStructures(includeSurfaceStructures),
m_includeVolumeFiles(includeVolumeFiles)
{
    m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_numberOfDisplayedOverlays",
                          &m_numberOfDisplayedOverlays);
    
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i] = new Overlay(includeSurfaceStructures,
                                    includeVolumeFiles);
    }
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_VALIDATION);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP);
}

/**
 * Destructor.
 */
OverlaySet::~OverlaySet()
{
    EventManager::get()->removeAllEventsFromListener(this);
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        delete m_overlays[i];
    }
    delete m_sceneAssistant;
}

/**
 * Copy the given overlay set to this overlay set.
 * @param overlaySet
 *    Overlay set that is copied.
 */
void 
OverlaySet::copyOverlaySet(const OverlaySet* overlaySet)
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i]->copyData(overlaySet->getOverlay(i));
    }
    m_numberOfDisplayedOverlays = overlaySet->m_numberOfDisplayedOverlays;
}

/**
 * @return Returns the top-most overlay regardless of its enabled status.
 */
Overlay* 
OverlaySet::getPrimaryOverlay()
{
    return m_overlays[0];
}

/**
 * @return Returns the underlay which is the lowest
 * displayed overlay.
 */
Overlay* 
OverlaySet::getUnderlay()
{
    return m_overlays[getNumberOfDisplayedOverlays() - 1];
}

/*
 * Get the bottom-most overlay that is a volume file for the given
 * browser tab.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the bottom-most overlay that is set a a volume file.
 * Will return NULL if no, enabled overlays are set to a volume file.
 */
Overlay*
OverlaySet::getUnderlayContainingVolume()
{
    Overlay* underlayOut(NULL);
    
    const int32_t numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = (numOverlays - 1); i >= 0; i--) {
        if (m_overlays[i]->isEnabled()) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex;
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
            m_overlays[i]->getSelectionData(mapFile,
                                            mapIndex);
            
            if (mapFile != NULL) {
                if (mapFile->isVolumeMappable()) {
                    const VolumeMappableInterface* vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
                    if (vf != NULL) {
                        underlayOut = m_overlays[i];
                        break;
                    }
                }
            }
        }
    }
    
    if (underlayOut == NULL) {
        /*
         * If we are here, either there are no volume files or
         * no overlays are enabled containing a volume file.
         * So, find the lowest layer than contains a volume file,
         * even if the layer is disabled.
         */
        for (int32_t i = 0; i < numOverlays; i++) {
            CaretMappableDataFile* mapFile = NULL;
            int32_t mapIndex;
            CaretAssertArrayIndex(m_overlays, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, i);
            m_overlays[i]->getSelectionData(mapFile,
                                            mapIndex);
            
            if (mapFile != NULL) {
                if (mapFile->isVolumeMappable()) {
                    const VolumeMappableInterface* vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
                    if (vf != NULL) {
                        underlayOut = m_overlays[i];
                        break;
                    }
                }
            }
        }
    }
    
    return underlayOut;
}

/*
 * Get the bottom-most overlay that is a volume file for the given
 * browser tab and return its volume file.
 * @param browserTabContent
 *    Content of browser tab.
 * @return Returns the bottom-most overlay that is set a a volume file.
 * Will return NULL if no, enabled overlays are set to a volume file.
 */
VolumeMappableInterface*
OverlaySet::getUnderlayVolume()
{
    VolumeMappableInterface* vf = NULL;
 
    Overlay* underlay = getUnderlayContainingVolume();
    if (underlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex;
        underlay->getSelectionData(mapFile, mapIndex);
        if (mapFile != NULL) {
            vf = dynamic_cast<VolumeMappableInterface*>(mapFile);
        }
    }
    
    return vf;
}

/**
 * If NO overlay (any overlay) is set to a volume, set the underlay to the first
 * volume that it finds.
 * @return Returns the volume file that was selected or NULL if no
 *    volume file was found.
 */
VolumeMappableInterface* 
OverlaySet::setUnderlayToVolume()
{
    VolumeMappableInterface * vf = getUnderlayVolume();
    
    if (vf == NULL) {
        const int32_t overlayIndex = getNumberOfDisplayedOverlays() - 1;
        if (overlayIndex >= 0) {
            std::vector<CaretMappableDataFile*> mapFiles;
            CaretMappableDataFile* mapFile;
            //AString mapUniqueID;
            int32_t mapIndex;
            m_overlays[overlayIndex]->getSelectionData(mapFiles, 
                                                          mapFile, 
                                                          //mapUniqueID,
                                                          mapIndex);
            
            const int32_t numMapFiles = static_cast<int32_t>(mapFiles.size());
            for (int32_t i = 0; i < numMapFiles; i++) {
                if (mapFiles[i]->isVolumeMappable()) {
                    vf = dynamic_cast<VolumeMappableInterface*>(mapFiles[i]);
                    if (vf != NULL) {
                        CaretMappableDataFile* cmdf = dynamic_cast<CaretMappableDataFile*>(vf);
                        CaretAssert(cmdf);
                        m_overlays[overlayIndex]->setSelectionData(cmdf, 0);
                        break;
                    }
                }
            }
        }
    }
    
    return vf;
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
const Overlay* 
OverlaySet::getOverlay(const int32_t overlayNumber) const
{
    CaretAssertArrayIndex(m_overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return m_overlays[overlayNumber];    
}

/**
 * Get the overlay at the specified index.
 * @param overlayNumber
 *   Index of the overlay.
 * @return Overlay at the given index.
 */
Overlay* 
OverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(m_overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, 
                          overlayNumber);
    return m_overlays[overlayNumber];    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
OverlaySet::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the window's content.
 *
 * @param descriptionOut
 *    Description of the window's content.
 */
void
OverlaySet::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Overlay Set");
    
    const int numOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numOverlays; i++) {
        if (getOverlay(i)->isEnabled()) {
            descriptionOut.pushIndentation();
            
            descriptionOut.addLine("Overlay "
                    + AString::number(i + 1)
                    + ": ");
            
            descriptionOut.pushIndentation();
            getOverlay(i)->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
            
            descriptionOut.popIndentation();
        }
    }
}


/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void 
OverlaySet::addDisplayedOverlay()
{
    m_numberOfDisplayedOverlays++;
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
}

/**
 * @return Returns the number of displayed overlays. 
 */
int32_t 
OverlaySet::getNumberOfDisplayedOverlays() const
{
    return m_numberOfDisplayedOverlays;
}

/**
 * Sets the number of displayed overlays.
 * @param numberOfDisplayedOverlays
 *   Number of overlays for display.
 */
void 
OverlaySet::setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays)
{
    const int32_t oldNumberOfDisplayedOverlays = m_numberOfDisplayedOverlays;
    m_numberOfDisplayedOverlays = numberOfDisplayedOverlays;
    if (m_numberOfDisplayedOverlays < BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS;
    }
    if (m_numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    }
    
    /*
     * If one overlay added (probably through GUI),
     * shift all overlays down one position so that 
     * new overlay appears at the top
     */ 
    const int32_t numberOfOverlaysAdded = m_numberOfDisplayedOverlays - oldNumberOfDisplayedOverlays;
    if (numberOfOverlaysAdded == 1) {
        for (int32_t i = (m_numberOfDisplayedOverlays - 1); i >= 0; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}

/**
 * Insert an overlay below this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added below
 */
void 
OverlaySet::insertOverlayAbove(const int32_t overlayIndex)
{
    if (m_numberOfDisplayedOverlays < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays++;
        
        for (int32_t i = (m_numberOfDisplayedOverlays - 2); i >= overlayIndex; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}

/**
 * Insert an overlay above this overlay
 * @param overlayIndex
 *     Index of overlay for which an overlay is added above
 */
void 
OverlaySet::insertOverlayBelow(const int32_t overlayIndex)
{
    if (m_numberOfDisplayedOverlays < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays++;
        
        for (int32_t i = (m_numberOfDisplayedOverlays - 2); i > overlayIndex; i--) {
            moveDisplayedOverlayDown(i);
        }
    }
}


/**
 * Remove a displayed overlay.  This method will have
 * no effect if the minimum number of overlays are
 * displayed
 *
 * @param overlayIndex 
 *    Index of overlay for removal from display.
 */
void 
OverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_overlays,
                          BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS,
                          overlayIndex);
    m_overlays[overlayIndex]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    
    if (m_numberOfDisplayedOverlays > BrainConstants::MINIMUM_NUMBER_OF_OVERLAYS) {
        m_numberOfDisplayedOverlays--;
        for (int32_t i = overlayIndex; i < m_numberOfDisplayedOverlays; i++) {
            m_overlays[i]->copyData(m_overlays[i+1]);
        }
    }
}

/**
 * Move the overlay at the given index up one level
 * (swap it with overlayIndex - 1).  This method will
 * have no effect if the overlay is the top-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved up.
 */
void 
OverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
{
    if (overlayIndex > 0) {
        m_overlays[overlayIndex]->swapData(m_overlays[overlayIndex - 1]);
    }
}

/**
 * Move the overlay at the given index down one level
 * (swap it with overlayIndex + 1).  This method will
 * have no effect if the overlay is the bottom-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved down.
 */
void 
OverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
{
    const int32_t nextOverlayIndex = overlayIndex + 1;
    if (nextOverlayIndex < m_numberOfDisplayedOverlays) {
        m_overlays[overlayIndex]->swapData(m_overlays[nextOverlayIndex]);
    }
}

/**
 * Match the desired names to maps (or file if no maps match) and optionally
 * structure to files/maps and return the matches.
 *
 * Note: If there are NO match names, all files are matched using the first
 * map in each file.
 *
 * @param matchedFilesOut
 *     Output to which matched files are APPENDED.
 * @param matchedFileIndicesOut
 *     Output to which matched file indices are APPENDED.
 * @param matchToStructures
 *     If not empty, only include files that map to these structures.  If
 *     matchToVolumeData is true, this parameter is ignored.  If this is (empty
 *     OR All) AND matchToVolumeData is false, all structures match.
 * @param dataFileType
 *     Data file type of desired files.
 * @param matchToVolumeData
 *     Include only files that map to volume data.  If true, matchToStructures
 *     is ignored.
 * @param matchToNamesRegularExpressionText
 *     Text for regular expression used for name matching.
 * @param matchToNamesRegularExpressionResult
 *     Status of regular expression matching for inclusion of file.
 * @param matchOneFilePerStructure
 *     If true, limit matched files so there is no more than one file
 *     for each structure.
 * @param
 *     True if matching files were found, else false.
 */
bool
OverlaySet::findFilesWithMapNamed(std::vector<CaretMappableDataFile*>& matchedFilesOut,
                                  std::vector<int32_t>& matchedFileIndicesOut,
                                  const std::vector<StructureEnum::Enum>& matchToStructures,
                                  const DataFileTypeEnum::Enum dataFileType,
                                  const bool matchToVolumeData,
                                  const AString& matchToNamesRegularExpressionText,
                                  const bool matchToNamesRegularExpressionResult,
                                  const bool matchOneFilePerStructure)
{
    std::vector<CaretMappableDataFile*> matchedFiles;
    std::vector<int32_t> matchedFileIndices;
    
    /*
     * Aggregate matching names and make them lower case
     */
    QRegExp regularExpression;
    if (matchToNamesRegularExpressionText.isEmpty() == false) {
        regularExpression = QRegExp(matchToNamesRegularExpressionText);
    }
    
    /*
     * Get files matching data type
     */
    EventCaretMappableDataFilesGet mapFileGetEvent(dataFileType);
    EventManager::get()->sendEvent(mapFileGetEvent.getPointer());
    std::vector<CaretMappableDataFile*> matchToMapFiles;
    mapFileGetEvent.getAllFiles(matchToMapFiles);
    const int32_t numberOfMatchFiles = static_cast<int32_t>(matchToMapFiles.size());
    if (numberOfMatchFiles <= 0) {
        return false;
    }
    
    /*
     * Determine which files should be tested by examing 
     * structure or if volume
     */
    std::vector<CaretMappableDataFile*> testMapFiles;
    for (int32_t iFile = 0; iFile < numberOfMatchFiles; iFile++) {
        bool fileMatchFlag = false;
        CaretMappableDataFile* mapFile = matchToMapFiles[iFile];
        
        /*
         * Volume mappable files only?
         */
        if (matchToVolumeData) {
            if (mapFile->isVolumeMappable()) {
                fileMatchFlag = true;
            }
        }
        
        /*
         * Test structures?
         */
        if (matchToStructures.empty() == false) {
            const StructureEnum::Enum mapFileStructure = mapFile->getStructure();
            /*
             * File maps to ALL structures?
             */
            if (mapFileStructure == StructureEnum::ALL) {
                fileMatchFlag = true;
            }
            else {
                /*
                 * Specific structutures
                 */
                if (std::find(matchToStructures.begin(),
                              matchToStructures.end(),
                              mapFileStructure) != matchToStructures.end()) {
                    fileMatchFlag = true;
                }
            }
        }
        
        if (fileMatchFlag) {
            if (mapFile->getNumberOfMaps() > 0) {
                testMapFiles.push_back(mapFile);
            }
        }
    }
    
    /*
     * No files to test?
     */
    const int32_t numTestFiles = static_cast<int32_t>(testMapFiles.size());
    if (numTestFiles <= 0) {
        return false;
    }
    
    
    std::set<StructureEnum::Enum> matchedStructures;
    
    /*
     * If there are names to match
     */
    if (matchToNamesRegularExpressionText.isEmpty() == false) {
        /*
         * First preference is matching MAP name
         */
            for (int32_t iFile = 0; iFile < numTestFiles; iFile++) {
                CaretMappableDataFile* mapFile = testMapFiles[iFile];
                const StructureEnum::Enum mapFileStructure = mapFile->getStructure();
                
                if (matchOneFilePerStructure) {
                    if (matchedStructures.find(mapFileStructure) != matchedStructures.end()) {
                        continue;
                    }
                }
                
                /*
                 * If NOT matching, exclude files whose name matches
                 */
                if (matchToNamesRegularExpressionResult == false) {
                    const AString fileName = mapFile->getFileNameNoPath().toLower();
                    
                    const bool fileNameMatch = (regularExpression.indexIn(fileName) >= 0);
                    if (fileNameMatch) {
                        continue;
                    }
                }
                
                const int32_t numMaps = mapFile->getNumberOfMaps();
                for (int32_t iMap = 0; iMap < numMaps; iMap++) {
                    const AString mapName = mapFile->getMapName(iMap).toLower();
                    const bool match = (regularExpression.indexIn(mapName) >= 0);
                    if (match == matchToNamesRegularExpressionResult) {
                        matchedFiles.push_back(mapFile);
                        matchedFileIndices.push_back(iMap);
                        
                        matchedStructures.insert(mapFileStructure);
                        break;
                    }
                }
            }
        
        /*
         * Find matching FILE name if NO map matches
         */
        if (matchedFiles.empty()) {
            for (int32_t iFile = 0; iFile < numTestFiles; iFile++) {
                CaretMappableDataFile* mapFile = testMapFiles[iFile];
                const StructureEnum::Enum mapFileStructure = mapFile->getStructure();
                
                if (matchOneFilePerStructure) {
                    if (matchedStructures.find(mapFileStructure) != matchedStructures.end()) {
                        continue;
                    }
                }
                
                const AString fileName = mapFile->getFileNameNoPath().toLower();
                const bool match = (regularExpression.indexIn(fileName) >= 0);
                if (match == matchToNamesRegularExpressionResult) {
                    matchedFiles.push_back(mapFile);
                    matchedFileIndices.push_back(0);
                    
                    matchedStructures.insert(mapFileStructure);
                }
            }
        }
    }
    else {
        /*
         * No names to match so just match to first map in each file
         */
        for (int32_t iFile = 0; iFile < numTestFiles; iFile++) {
            CaretMappableDataFile* mapFile = testMapFiles[iFile];
            const StructureEnum::Enum mapFileStructure = mapFile->getStructure();
            
            if (matchOneFilePerStructure) {
                if (matchedStructures.find(mapFileStructure) != matchedStructures.end()) {
                    continue;
                }
            }
            
            matchedFiles.push_back(mapFile);
            matchedFileIndices.push_back(0);
            
            matchedStructures.insert(mapFileStructure);
        }
    }
    
    CaretAssert(matchedFiles.size() == matchedFileIndices.size());
    
    const bool filesFound = (matchedFiles.empty() == false);
    
    /*
     * APPEND to output, do not replace
     */
    matchedFilesOut.insert(matchedFilesOut.end(),
                           matchedFiles.begin(),
                           matchedFiles.end());
    matchedFileIndicesOut.insert(matchedFileIndicesOut.end(),
                                 matchedFileIndices.begin(),
                                 matchedFileIndices.end());
    
    return filesFound;
}

/**
 * Find underlay files.
 *
 * @param matchToStructures
 *    Structures to include.
 * @param includeVolumeFiles 
 *    Include volume files.
 * @param filesOut
 *    Output containing files that were selected.
 * @param mapIndicesOut
 *    Output containing maps indices in files that were selected.
 */
void
OverlaySet::findUnderlayFiles( const std::vector<StructureEnum::Enum>& matchToStructures,
                              const bool includeVolumeFiles,
                              std::vector<CaretMappableDataFile*>& filesOut,
                              std::vector<int32_t>& mapIndicesOut)
{
    /*
     * First, try to find CIFTI shape files
     */
    if (findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                              false,
                              s_shapeMatchRegularExpressionText,
                              true,
                              true) == false) {
        
        /*
         * Second, try to find METRIC shape files
         */
        findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::METRIC,
                              false,
                              s_shapeMatchRegularExpressionText,
                              true,
                              true);
    }
    
    if (includeVolumeFiles) {
        std::vector<VolumeFile*> volumeFiles = getVolumeFiles();
        
        const int32_t numVolumes = static_cast<int32_t>(volumeFiles.size());
        if (numVolumes > 0) {
            bool foundAnatomyVolume = false;

            for (int32_t i = 0; i < numVolumes; i++) {
                VolumeFile* vf = volumeFiles[i];
                if (vf->getType() == SubvolumeAttributes::ANATOMY) {
                    if (vf->getNumberOfMaps() > 0) {
                        filesOut.push_back(vf);
                        mapIndicesOut.push_back(0);
                        foundAnatomyVolume = true;
                        break;
                    }
                }
            }
            
            if (foundAnatomyVolume == false) {
                for (int32_t i = 0; i < numVolumes; i++) {
                    VolumeFile* vf = volumeFiles[i];
                    bool testIt = true;
                    if (vf->getType() == SubvolumeAttributes::LABEL) {
                        testIt = false;
                    }
                    if (testIt) {
                        if (vf->getNumberOfMaps() > 0) {
                            PaletteColorMapping* pcm = vf->getMapPaletteColorMapping(0);
                            if (pcm != NULL) {
                                const AString paletteName = pcm->getSelectedPaletteName();
                                if (paletteName.contains("gray")
                                    || paletteName.contains("grey")) {
                                    filesOut.push_back(vf);
                                    mapIndicesOut.push_back(0);
                                    foundAnatomyVolume = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    CaretAssert(filesOut.size() == mapIndicesOut.size());
}

/**
 * @return All volume files.
 */
std::vector<VolumeFile*>
OverlaySet::getVolumeFiles() const
{
    std::vector<VolumeFile*> volumeFiles;
    
    EventCaretMappableDataFilesGet mapFileGetEvent(DataFileTypeEnum::VOLUME);
    EventManager::get()->sendEvent(mapFileGetEvent.getPointer());
    std::vector<CaretMappableDataFile*> matchToMapFiles;
    mapFileGetEvent.getAllFiles(matchToMapFiles);
    
    for (std::vector<CaretMappableDataFile*>::iterator iter = matchToMapFiles.begin();
         iter != matchToMapFiles.end();
         iter++) {
        VolumeFile* vf = dynamic_cast<VolumeFile*>(*iter);
        CaretAssert(vf);
        volumeFiles.push_back(vf);
    }
    
    return volumeFiles;
}


/**
 * Find middle layer files.
 *
 * @param matchToStructures
 *    Structures to include.
 * @param includeVolumeFiles
 *    Include volume files.
 * @param filesOut
 *    Output containing files that were selected.
 * @param mapIndicesOut
 *    Output containing maps indices in files that were selected.
 */
void
OverlaySet::findMiddleLayerFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                 const bool includeVolumeFiles,
                                 std::vector<CaretMappableDataFile*>& filesOut,
                                 std::vector<int32_t>& mapIndicesOut)
{
    std::vector<AString> matchToNames;
    
    /*
     * First, try to find CIFTI scalar files with myelin
     */
    if (findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                              includeVolumeFiles,
                              s_myelinMatchRegularExpressionText,
                              true,
                              false) == false) {
        
        /*
         * Second, try to find METRIC files with neither shape nor myelin
         */
        findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::METRIC,
                              includeVolumeFiles,
                              s_myelinMatchRegularExpressionText,
                              true,
                              false);
    }
    
    /*
     * Second, try to find CIFTI scalar files with neither shape nor myelin
     */
    if (findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR,
                              includeVolumeFiles,
                              s_shapeMyelinMatchRegularExpressionText,
                              false,
                              false) == false) {
        
        /*
         * Second, try to find METRIC files with neither shape nor myelin
         */
        findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::METRIC,
                              includeVolumeFiles,
                              s_shapeMyelinMatchRegularExpressionText,
                              false,
                              false);
    }
    
    if (includeVolumeFiles) {
        std::vector<VolumeFile*> volumeFiles = getVolumeFiles();
        const int32_t numVolumes = static_cast<int32_t>(volumeFiles.size());
        for (int32_t i = 0; i < numVolumes; i++) {
            VolumeFile* vf = volumeFiles[i];
            if ((vf->getType() == SubvolumeAttributes::FUNCTIONAL)) {
                if (vf->getNumberOfMaps() > 0) {
                    filesOut.push_back(vf);
                    mapIndicesOut.push_back(0);
                    break;
                }
            }
        }
    }
    
    CaretAssert(filesOut.size() == mapIndicesOut.size());
}

/**
 * Find overlay files.
 *
 * @param matchToStructures
 *    Structures to include.
 * @param includeVolumeFiles
 *    Include volume files.
 * @param filesOut
 *    Output containing files that were selected.
 * @param mapIndicesOut
 *    Output containing maps indices in files that were selected.
 */
void
OverlaySet::findOverlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                             const bool includeVolumeFiles,
                             std::vector<CaretMappableDataFile*>& filesOut,
                             std::vector<int32_t>& mapIndicesOut)
{
    /*
     * First, try to find CIFTI LABEL files
     */
    if (findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
                              includeVolumeFiles,
                              "",
                              true,
                              true) == false) {
        
        /*
         * Second, try to find LABEL files
         */
        findFilesWithMapNamed(filesOut,
                              mapIndicesOut,
                              matchToStructures,
                              DataFileTypeEnum::LABEL,
                              includeVolumeFiles,
                              "",
                              true,
                              true);
    }
    
    if (includeVolumeFiles) {
        std::vector<VolumeFile*> volumeFiles = getVolumeFiles();
        const int32_t numVolumes = static_cast<int32_t>(volumeFiles.size());
        for (int32_t i = 0; i < numVolumes; i++) {
            VolumeFile* vf = volumeFiles[i];
            if (vf->getType() == SubvolumeAttributes::LABEL) {
                if (vf->getNumberOfMaps() > 0) {
                    filesOut.push_back(vf);
                    mapIndicesOut.push_back(0);
                    break;
                }
            }
        }
    }
    CaretAssert(filesOut.size() == mapIndicesOut.size());
}


/**
 * Initialize the overlays.
 */
void
OverlaySet::initializeOverlays()
{
    bool isMatchToVolumeUnderlay = false;
    bool isMatchToVolumeOverlays = false;
    
    switch (m_includeVolumeFiles) {
        case Overlay::INCLUDE_VOLUME_FILES_NO:
            break;
        case Overlay::INCLUDE_VOLUME_FILES_YES:
            /*
             * If no surface structures, then it must be volume slice view
             * so allow volumes to be in the overlays.
             */
            if (m_includeSurfaceStructures.empty()) {
                isMatchToVolumeOverlays = true;
            }
            isMatchToVolumeUnderlay = true;
            break;
    }
    
    /*
     * Underlays consist of anatomical type data
     */
    std::vector<CaretMappableDataFile*> underlayMapFiles;
    std::vector<int32_t> underlayMapIndices;
    findUnderlayFiles(m_includeSurfaceStructures,
                      isMatchToVolumeUnderlay,
                      underlayMapFiles,
                      underlayMapIndices);
    
    /*
     * Middle layers are Cifti labels or Gifti Labels
     * that do not contain shape data
     */
    std::vector<CaretMappableDataFile*> middleLayerMapFiles;
    std::vector<int32_t> middleLayerMapIndices;
    findMiddleLayerFiles(m_includeSurfaceStructures,
                         isMatchToVolumeOverlays,
                         middleLayerMapFiles,
                         middleLayerMapIndices);
    
    /*
     * Overlays consist of Cifti scalars or Gifti Metric
     */
    std::vector<CaretMappableDataFile*> overlayMapFiles;
    std::vector<int32_t> overlayMapIndices;
    findOverlayFiles(m_includeSurfaceStructures,
                         isMatchToVolumeOverlays,
                         overlayMapFiles,
                         overlayMapIndices);
    
    const int32_t numberOfUnderlayFiles = static_cast<int32_t>(underlayMapFiles.size());
    
    /*
     * Number of overlay that are displayed.
     */
    const int32_t numberOfDisplayedOverlays = getNumberOfDisplayedOverlays();
    
    
    /*
     * Track overlay that were initialized
     */
    std::vector<bool> overlayInitializedFlag(numberOfDisplayedOverlays,
                                             false);
    
    /*
     * Put in the shape files at the bottom
     * Note that highest overlay index is bottom
     */
    int32_t overlayIndexForUnderlay = (numberOfDisplayedOverlays - 1);
    for (int32_t underlayFileIndex = 0; underlayFileIndex < numberOfUnderlayFiles; underlayFileIndex++) {
        if (overlayIndexForUnderlay >= 0) {
            Overlay* overlay = getOverlay(overlayIndexForUnderlay);
            overlay->setSelectionData(underlayMapFiles[underlayFileIndex],
                                      underlayMapIndices[underlayFileIndex]);
            overlayInitializedFlag[overlayIndexForUnderlay] = true;
            overlayIndexForUnderlay--;
        }
        else {
            break;
        }
    }
    
    /*
     * Combine overlay and middle layer files
     */
    std::vector<CaretMappableDataFile*> upperLayerFiles;
    std::vector<int32_t> upperLayerIndices;
    upperLayerFiles.insert(upperLayerFiles.end(),
                           overlayMapFiles.begin(),
                           overlayMapFiles.end());
    upperLayerIndices.insert(upperLayerIndices.end(),
                             overlayMapIndices.begin(),
                             overlayMapIndices.end());
    upperLayerFiles.insert(upperLayerFiles.end(),
                           middleLayerMapFiles.begin(),
                           middleLayerMapFiles.end());
    upperLayerIndices.insert(upperLayerIndices.end(),
                             middleLayerMapIndices.begin(),
                             middleLayerMapIndices.end());
    CaretAssert(upperLayerFiles.size() == upperLayerIndices.size());
    
    const int32_t numberOfUpperFiles = static_cast<int32_t>(upperLayerFiles.size());
    
    /*
     * Put in overlay and middle layer files
     */
    for (int32_t upperFileIndex = 0; upperFileIndex < numberOfUpperFiles; upperFileIndex++) {
        /*
         * Find available overlay
         */
        int32_t upperLayerOverlayIndex = -1;
        for (int32_t overlayIndex = 0; overlayIndex < numberOfDisplayedOverlays; overlayIndex++) {
            if (overlayInitializedFlag[overlayIndex] == false) {
                upperLayerOverlayIndex = overlayIndex;
                break;
            }
        }
        
        if (upperLayerOverlayIndex >= 0) {
            Overlay* upperLayerOverlay = getOverlay(upperLayerOverlayIndex);
            upperLayerOverlay->setSelectionData(upperLayerFiles[upperFileIndex],
                                                upperLayerIndices[upperFileIndex]);
            overlayInitializedFlag[upperLayerOverlayIndex] = true;
        }
        else {
            break;
        }
    }
    
    /*
     * Disable overlays that were not initialized
     */
    for (int32_t i = 0; i < numberOfDisplayedOverlays; i++) {
        CaretAssertVectorIndex(overlayInitializedFlag, i);
        getOverlay(i)->setEnabled(overlayInitializedFlag[i]);
    }
}


/**
 * Get any label files that are selected and applicable for the given surface.
 * @param surface
 *    Surface for which label files are desired.
 * @param labelFilesOut
 *    Label files that are applicable to the given surface.
 * @param labelMapIndicesOut
 *    Selected map indices in the output label files.
 */
void 
OverlaySet::getLabelFilesForSurface(const Surface* surface,
                                    std::vector<LabelFile*>& labelFilesOut,
                                    std::vector<int32_t>& labelMapIndicesOut)
{
    CaretAssert(surface);
    
    labelFilesOut.clear();
    labelMapIndicesOut.clear();
    
    const int32_t numberOfOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = getOverlay(i);
        if (overlay->isEnabled()) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile, mapIndex);
            if (mapFile != NULL) {
                if (mapFile->getDataFileType() == DataFileTypeEnum::LABEL) {
                    if (mapFile->getStructure() == surface->getStructure()) {
                        LabelFile* labelFile = dynamic_cast<LabelFile*>(mapFile);
                        labelFilesOut.push_back(labelFile);
                        labelMapIndicesOut.push_back(mapIndex);
                    }
                }
            }
        }
    }
}

/**
 * For the given caret mappable data file, find overlays in which the
 * file is selected and return the indices of the selected maps.
 *
 * @param caretMappableDataFile
 *    The caret mappable data file.
 * @param isLimitToEnabledOverlays
 *    If true, only include map indices for overlay that are enabled.  
 *    Otherwise, include map indices for all overlays.
 * @param selectedMapIndicesOut
 *    Output containing map indices for the given caret mappable data files
 *    that are selected as overlays in this overlay set.
 */
void
OverlaySet::getSelectedMapIndicesForFile(const CaretMappableDataFile* caretMappableDataFile,
                                         const bool isLimitToEnabledOverlays,
                                         std::vector<int32_t>& selectedMapIndicesOut) const
{
    selectedMapIndicesOut.clear();
    
    /*
     * Put indices in a set to avoid duplicates and keep them sorted.
     */
    std::set<int32_t> mapIndicesSet;
    
    const int32_t numberOfOverlays = getNumberOfDisplayedOverlays();
    for (int32_t i = 0; i < numberOfOverlays; i++) {
        Overlay* overlay = const_cast<Overlay*>(getOverlay(i));
        bool checkIt = true;
        if (isLimitToEnabledOverlays) {
            if (overlay->isEnabled() == false) {
                checkIt = false;
            }
        }
        
        if (checkIt) {
            CaretMappableDataFile* mapFile;
            int32_t mapIndex;
            overlay->getSelectionData(mapFile, mapIndex);
            if (mapFile == caretMappableDataFile) {
                mapIndicesSet.insert(mapIndex);
            }
        }
    }
    
    selectedMapIndicesOut.insert(selectedMapIndicesOut.end(),
                                 mapIndicesSet.begin(),
                                 mapIndicesSet.end());
}

/**
 * Reset the yoking status of all overlays to off.
 */
void
OverlaySet::resetOverlayYokingToOff()
{
    for (int i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; i++) {
        m_overlays[i]->setMapYokingGroup(MapYokingGroupEnum::MAP_YOKING_GROUP_OFF);
    }
}


/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
OverlaySet::saveToScene(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "OverlaySet",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
//    const int32_t numOverlaysToSave = BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS;
    const int32_t numOverlaysToSave = getNumberOfDisplayedOverlays();
    
    std::vector<SceneClass*> overlayClassVector;
    for (int i = 0; i < numOverlaysToSave; i++) {
        overlayClassVector.push_back(m_overlays[i]->saveToScene(sceneAttributes, "m_overlays"));
    }
    
    SceneClassArray* overlayClassArray = new SceneClassArray("m_overlays",
                                                             overlayClassVector);
    sceneClass->addChild(overlayClassArray);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void 
OverlaySet::restoreFromScene(const SceneAttributes* sceneAttributes,
                             const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    const SceneClassArray* overlayClassArray = sceneClass->getClassArray("m_overlays");
    if (overlayClassArray != NULL) {
        const int32_t numOverlays = std::min(overlayClassArray->getNumberOfArrayElements(),
                                             (int32_t)BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS);
        for (int32_t i = 0; i < numOverlays; i++) {
            m_overlays[i]->restoreFromScene(sceneAttributes, 
                                            overlayClassArray->getClassAtIndex(i));
        }
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
OverlaySet::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_VALIDATION) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingValidation* mapYokeEvent = dynamic_cast<EventMapYokingValidation*>(event);
        CaretAssert(mapYokeEvent);

        const MapYokingGroupEnum::Enum requestedYokingGroup = mapYokeEvent->getMapYokingGroup();
        if (requestedYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                Overlay* overlay = getOverlay(j);
                
                CaretMappableDataFile* mapFile = NULL;
                int32_t mapIndex = -1;
                overlay->getSelectionData(mapFile,
                                          mapIndex);
                if (mapFile != NULL) {
                    mapYokeEvent->addMapYokedFile(mapFile, overlay->getMapYokingGroup(), m_tabIndex);
                }
            }
        }
        
        mapYokeEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP) {
        /*
         * The events intended for overlays are received here so that
         * only DISPLAYED overlays are updated.
         */
        EventMapYokingSelectMap* selectMapEvent = dynamic_cast<EventMapYokingSelectMap*>(event);
        CaretAssert(selectMapEvent);
        const MapYokingGroupEnum::Enum eventYokingGroup = selectMapEvent->getMapYokingGroup();
        if (eventYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            const int32_t yokingGroupMapIndex = MapYokingGroupEnum::getSelectedMapIndex(eventYokingGroup);
            const bool yokingGroupSelectedStatus = MapYokingGroupEnum::isEnabled(eventYokingGroup);
            const CaretMappableDataFile* eventMapFile = selectMapEvent->getCaretMappableDataFile();
            
            /*
             * Find all overlays with the requested yoking
             */
            const int32_t overlayCount = getNumberOfDisplayedOverlays();
            for (int32_t j = 0; j < overlayCount; j++) {
                Overlay* overlay = getOverlay(j);
                
                if (overlay->getMapYokingGroup() == selectMapEvent->getMapYokingGroup()) {
                    CaretMappableDataFile* mapFile = NULL;
                    int32_t mapIndex = -1;
                    overlay->getSelectionData(mapFile,
                                              mapIndex);
                    
                    if (mapFile != NULL) {
                        if (yokingGroupMapIndex < mapFile->getNumberOfMaps()) {
                            overlay->setSelectionData(mapFile,
                                                      yokingGroupMapIndex);
                        }
                        
                        if (mapFile == eventMapFile) {
                            /* only alter status if event was sent by mappable file */
                            if (selectMapEvent->getCaretMappableDataFile() != NULL) {
                                overlay->setEnabled(yokingGroupSelectedStatus);
                            }
                        }
                    }
                }
            }
            
            selectMapEvent->setEventProcessed();
        }
        
        
//        const MapYokingGroupEnum::Enum mapYokingGroup = selectMapEvent->get
    }
}

