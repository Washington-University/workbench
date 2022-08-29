
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

#define __EVENT_MAP_YOKING_SELECT_MAP_DECLARE__
#include "EventMapYokingSelectMap.h"
#undef __EVENT_MAP_YOKING_SELECT_MAP_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventMapYokingSelectMap 
 * \brief Event sent when a yoked overlay or file selection changes.
 * \ingroup Files
 */

/**
 * Constructor for change in map yoking.
 *
 * @param mapYokingGroup
 *    Map yoking group that has a status change (selected map or enabled status)
 * @param caretMappableDataFile
 *    Caret mappable file that is causing this event.
 * @param mediaFile
 *    Media file causing this event
 * @param mapIndex
 *    Index of map selected.
 * @param mediaAllFramesStatus
 *    The media all frames status
 * @param selectionStatus
 *    Status of selection.
 */
EventMapYokingSelectMap::EventMapYokingSelectMap(const MapYokingGroupEnum::Enum mapYokingGroup,
                                                 const CaretMappableDataFile* caretMappableDataFile,
                                                 const AnnotationTextSubstitutionFile* annotationTextSubstitutionFile,
                                                 const HistologySlicesFile* histologySlicesFile,
                                                 const MediaFile* mediaFile,
                                                 const int32_t mapIndex,
                                                 const MapYokingGroupEnum::MediaAllFramesStatus mediaAllFramesStatus,
                                                 const bool mapOverlaySelectionStatus)
: Event(EventTypeEnum::EVENT_MAP_YOKING_SELECT_MAP),
m_mapYokingGroup(mapYokingGroup),
m_caretMappableDataFile(caretMappableDataFile),
m_annotationTextSubstitutionFile(annotationTextSubstitutionFile),
m_histologySlicesFile(histologySlicesFile),
m_mediaFile(mediaFile),
m_mapIndex(mapIndex)
{
    if (mapYokingGroup != MapYokingGroupEnum::MAP_YOKING_GROUP_OFF) {
        MapYokingGroupEnum::setSelectedMapIndex(mapYokingGroup, mapIndex);
        MapYokingGroupEnum::setMediaAllFramesStatus(mapYokingGroup, mediaAllFramesStatus);
        MapYokingGroupEnum::setEnabled(mapYokingGroup, mapOverlaySelectionStatus);
    }
}

/**
 * Destructor.
 */
EventMapYokingSelectMap::~EventMapYokingSelectMap()
{
}

/**
 * @return The map yoking group.
 */
MapYokingGroupEnum::Enum
EventMapYokingSelectMap::getMapYokingGroup() const
{
    return m_mapYokingGroup;
}

/**
 * @return Caret Mappable Data File for which event was issued.
 * Might be NULL.
 */
const CaretMappableDataFile*
EventMapYokingSelectMap::getCaretMappableDataFile() const
{
    return m_caretMappableDataFile;
}

/**
 * @return Annotation text substitution file for which event was issued.
 * Might be NULL.
 */
const AnnotationTextSubstitutionFile*
EventMapYokingSelectMap::getAnnotationTextSubstitutionFile() const
{
    return m_annotationTextSubstitutionFile;
}

/**
 * @return Histology slices  file for which event was issued.
 * Might be NULL.
 */
const HistologySlicesFile*
EventMapYokingSelectMap::getHistologySlicesFile() const
{
    return m_histologySlicesFile;
}

/**
 * @return Media  file for which event was issued.
 * Might be NULL.
 */
const MediaFile*
EventMapYokingSelectMap::getMediaFile() const
{
    return m_mediaFile;
}

/**
 * @return Map index selected.
 */
int32_t
EventMapYokingSelectMap::getMapIndex() const
{
    return m_mapIndex;
}

