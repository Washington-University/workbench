#ifndef __EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_H__
#define __EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_H__

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

#include <set>

#include "Event.h"



namespace caret {
    class CaretMappableDataFile;

    class EventCaretMappableDataFileMapsViewedInOverlays : public Event {
        
    public:
        EventCaretMappableDataFileMapsViewedInOverlays(const CaretMappableDataFile* caretMappableDataFile);
        
        virtual ~EventCaretMappableDataFileMapsViewedInOverlays();
        
        const CaretMappableDataFile* getCaretMappableDataFile() const;
        
        void addMapIndex(const int32_t mapIndex);

        std::set<int32_t> getSelectedMapIndices() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventCaretMappableDataFileMapsViewedInOverlays(const EventCaretMappableDataFileMapsViewedInOverlays&);

        EventCaretMappableDataFileMapsViewedInOverlays& operator=(const EventCaretMappableDataFileMapsViewedInOverlays&);
        
        // ADD_NEW_MEMBERS_HERE

        const CaretMappableDataFile* m_caretMappableDataFile;
        
        std::set<int32_t> m_mapIndices;
        
    };
    
#ifdef __EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_DECLARE__

} // namespace
#endif  //__EVENT_CARET_MAPPABLE_DATA_FILE_MAPS_VIEWED_IN_OVERLAYS_H__
