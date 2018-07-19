#ifndef __EVENT_MAP_YOKING_SELECT_MAP_H__
#define __EVENT_MAP_YOKING_SELECT_MAP_H__

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


#include "Event.h"
#include "MapYokingGroupEnum.h"

namespace caret {

    class AnnotationTextSubstitutionFile;
    class CaretMappableDataFile;
    
    class EventMapYokingSelectMap : public Event {
        
    public:
        EventMapYokingSelectMap(const MapYokingGroupEnum::Enum mapYokingGroup,
                                const CaretMappableDataFile* caretMappableDataFile,
                                const AnnotationTextSubstitutionFile* annotationTextSubstitutionFile,
                                const int32_t mapIndex,
                                const bool selectionStatus);
        
        virtual ~EventMapYokingSelectMap();
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        const CaretMappableDataFile* getCaretMappableDataFile() const;
        
        const AnnotationTextSubstitutionFile* getAnnotationTextSubstitutionFile() const;
        
        int32_t getMapIndex() const;
        
        bool getSelectionStatus() const;

        // ADD_NEW_METHODS_HERE

    private:
        EventMapYokingSelectMap(const EventMapYokingSelectMap&);

        EventMapYokingSelectMap& operator=(const EventMapYokingSelectMap&);
        
        const MapYokingGroupEnum::Enum m_mapYokingGroup;
        
        const CaretMappableDataFile* m_caretMappableDataFile;
        
        const AnnotationTextSubstitutionFile* m_annotationTextSubstitutionFile;
        
        const int32_t m_mapIndex;
        
        const bool m_selectionStatus;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_MAP_YOKING_SELECT_MAP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_MAP_YOKING_SELECT_MAP_DECLARE__

} // namespace
#endif  //__EVENT_MAP_YOKING_SELECT_MAP_H__
