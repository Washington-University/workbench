
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

#define __EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_DECLARE__
#include "EventCaretMappableDataFileMapsViewedInOverlays.h"
#undef __EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventCaretMappableDataFileMapsViewedInOverlays 
 * \brief Get maps viewed as an overlay for the given data file type
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventCaretMappableDataFileMapsViewedInOverlays::EventCaretMappableDataFileMapsViewedInOverlays(const CaretMappableDataFile* caretMappableDataFile)
: Event(EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS),
m_caretMappableDataFile(caretMappableDataFile)
{
    CaretAssert(caretMappableDataFile);
}

/**
 * Destructor.
 */
EventCaretMappableDataFileMapsViewedInOverlays::~EventCaretMappableDataFileMapsViewedInOverlays()
{
}

/**
 * @return The type of data file for which maps viewed as overlay are desired.
 */
const CaretMappableDataFile*
EventCaretMappableDataFileMapsViewedInOverlays::getCaretMappableDataFile() const
{
    return m_caretMappableDataFile;
}

/**
 * Add map index for caret mappable data file.
 *
 * @param mapIndex
 *    Selected map index for data file.
 */
void
EventCaretMappableDataFileMapsViewedInOverlays::addMapIndex(const int32_t mapIndex)
{
    m_mapIndices.insert(mapIndex);
}

/**
 * @return Displayed as overlay map indices for file.
 */
std::set<int32_t>
EventCaretMappableDataFileMapsViewedInOverlays::getSelectedMapIndices() const
{
    return m_mapIndices;
}
