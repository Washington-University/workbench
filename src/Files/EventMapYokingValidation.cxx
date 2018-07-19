
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __EVENT_MAP_YOKING_VALIDATION_DECLARE__
#include "EventMapYokingValidation.h"
#undef __EVENT_MAP_YOKING_VALIDATION_DECLARE__

#include "AnnotationTextSubstitutionFile.h"
#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EventBrowserTabIndicesGetAll.h"
#include "EventManager.h"
#include "EventTypeEnum.h"

using namespace caret;



/**
 * \class caret::EventMapYokingValidation
 * \brief Get map files yoked to a mapping group for validation.
 * \ingroup Files
 *
 * When the user selects map yoking for a file, verify that the new
 * file contains the same number of maps as any other files using
 * the same yoking group.  If not, warn the user.
 */

/**
 * Constructor for validation files with map yoked to the given yoking group.
 *
 * @param mapYokingGroup
 *    The map yoking group that will be validated for compatibility.
 */
EventMapYokingValidation::EventMapYokingValidation(const MapYokingGroupEnum::Enum mapYokingGroup)
: Event(EventTypeEnum::EVENT_MAP_YOKING_VALIDATION),
m_mapYokingGroup(mapYokingGroup)
{
    EventBrowserTabIndicesGetAll allTabsEvent;
    EventManager::get()->sendEvent(allTabsEvent.getPointer());
    
    m_validTabIndices = allTabsEvent.getAllBrowserTabIndices();
    
}

/**
 * Destructor.
 */
EventMapYokingValidation::~EventMapYokingValidation()
{
}

/**
 * Add a map file, if it is yoked to the same yoking group, so that it
 * may be used in the compatibility test.
 *
 * @param caretMapFile
 *     The map file.
 * @param mapYokingGroup
 *     Yoking group status of the file
 * @param tabIndex
 *     Index of tab in which the file is displayed.
 */
void
EventMapYokingValidation::addMapYokedFile(const CaretMappableDataFile* caretMapFile,
                                          const MapYokingGroupEnum::Enum mapYokingGroup,
                                          const int32_t tabIndex)
{
    CaretAssert(caretMapFile);
    
    if (mapYokingGroup == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
    
    if (mapYokingGroup != m_mapYokingGroup) {
        return;
    }
    
    if (std::find(m_validTabIndices.begin(),
                  m_validTabIndices.end(),
                  tabIndex) == m_validTabIndices.end()) {
        return;
    }
 
    m_yokedFileInfo.insert(YokedFileInfo(NULL,
                                         caretMapFile,
                                         tabIndex));
}

/**
 * Add a annotation text substitution file, if it is yoked to the same yoking group, 
 * so that it may be used in the compatibility test.
 *
 * @param annTextSubFile
 *     The annotation text substitution file.
 * @param mapYokingGroup
 *     Yoking group status of the file
 */
void
EventMapYokingValidation::addAnnotationTextSubstitutionFile(const AnnotationTextSubstitutionFile* annTextSubFile,
                                                            const MapYokingGroupEnum::Enum mapYokingGroup)
{
    CaretAssert(annTextSubFile);
    
    if (mapYokingGroup == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        return;
    }
    
    if (mapYokingGroup != m_mapYokingGroup) {
        return;
    }
    
    m_yokedFileInfo.insert(YokedFileInfo(annTextSubFile,
                                         NULL,
                                         -1));
}


/**
 * Add a map file, if it is yoked to the same yoking group, so that it
 * may be used in the compatibility test.
 *
 * @param caretMapFile
 *     The map file.
 * @param mapYokingGroupsForAllTabs
 *     Yoking group status of the file for all tabs.  Number of elements
 *     MUST BE BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS.
 */
void
EventMapYokingValidation::addMapYokedFileAllTabs(const CaretMappableDataFile* caretMapFile,
                            const MapYokingGroupEnum::Enum* mapYokingGroupsForAllTabs)
{
    CaretAssert(caretMapFile);
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        if (mapYokingGroupsForAllTabs[iTab] == MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
            continue;
        }
        
        if (mapYokingGroupsForAllTabs[iTab] != m_mapYokingGroup) {
            continue;
        }
        
        if (std::find(m_validTabIndices.begin(),
                      m_validTabIndices.end(),
                      iTab) == m_validTabIndices.end()) {
            continue;
        }
        
        m_yokedFileInfo.insert(YokedFileInfo(NULL,
                                             caretMapFile,
                                             iTab));
    }
}

/**
 * @return The map yoking group.
 */
MapYokingGroupEnum::Enum
EventMapYokingValidation::getMapYokingGroup() const
{
    return m_mapYokingGroup;
}

/**
 * Validate the file for compatibility.
 *
 * @param annTextSubFile
 *     The annotation text substitution file.
 * @param caretMapFile
 *     The map file.
 * @param numberOfYokedFilesOut
 *     Number of files, excluding the given file, currently yoked 
 *     to this yoking group.
 * @param messageOut
 *     Message containing information about any incompatibility.
 * @return
 *     True if new file is compatible with any existing yoked files, else false.
 */
bool
EventMapYokingValidation::validateCompatibility(const AnnotationTextSubstitutionFile* annTextSubFile,
                                                const CaretMappableDataFile* caretMapFile,
                                                int32_t& numberOfYokedFilesOut,
                                                AString& messageOut) const
{
    numberOfYokedFilesOut = 0;
    messageOut = "";
 
    AString filename;
    int32_t numberOfMaps = 0;
    AString message;
    if (annTextSubFile != NULL) {
        filename     = annTextSubFile->getFileNameNoPath();
        numberOfMaps = annTextSubFile->getNumberOfValues();
        message = (AString::number(numberOfMaps)
                   + " substitution values in ");
    }
    else if (caretMapFile != NULL) {
        filename     = caretMapFile->getFileNameNoPath();
        numberOfMaps = caretMapFile->getNumberOfMaps();
        message = (AString::number(numberOfMaps)
                   + " maps in ");
    }
    else {
        CaretAssert(0);
    }
    
    for (std::set<YokedFileInfo>::const_iterator iter = m_yokedFileInfo.begin();
         iter != m_yokedFileInfo.end();
         iter++) {
        const YokedFileInfo& yfi = *iter;
        numberOfYokedFilesOut++;
        
        if (yfi.m_numberOfMaps != numberOfMaps) {
            messageOut.appendWithNewLine("   " + yfi.m_infoText);
        }
    }
    
    if (messageOut.isEmpty()) {
        return true;
    }
    
    const AString fileInfo("Incompatibilities for yoking:\n"
                           + message
                           + filename
                           + "\n\n");
    messageOut.insert(0, fileInfo);
                           
    return false;
}

/**
 * Constructor for yoked file information.
 *
 * @param annTextSubFile
 *     The annotation text substitution file.
 * @param caretMapFile
 *     The map file.
 * @param tabIndex
 *     Index of tab in which the file is displayed.
 */
EventMapYokingValidation::YokedFileInfo::YokedFileInfo(const AnnotationTextSubstitutionFile* annTextSubFile,
                                                       const CaretMappableDataFile* caretMapFile,
                                                       const int32_t tabIndex)
: m_annTextSubFile(annTextSubFile),
m_mapFile(caretMapFile),
m_tabIndex(tabIndex)
{
    if (m_annTextSubFile != NULL) {
        m_numberOfMaps = annTextSubFile->getNumberOfValues();
        
        m_infoText = (AString::number(m_numberOfMaps)
                      + " substitution values in "
                      + m_annTextSubFile->getFileNameNoPath());
    }
    else if (m_mapFile != NULL) {
        m_numberOfMaps = caretMapFile->getNumberOfMaps();
        
        m_infoText = (AString::number(m_numberOfMaps)
                      + " maps in tab "
                      + AString::number(tabIndex + 1)
                      + " file: "
                      + caretMapFile->getFileNameNoPath());
    }
    else {
        CaretAssert(0);
    }
}
