#ifndef __EVENT_MAP_YOKING_VALIDATION_H__
#define __EVENT_MAP_YOKING_VALIDATION_H__

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
#include "MapYokingGroupEnum.h"


namespace caret {

    class AnnotationTextSubstitutionFile;
    class CaretMappableDataFile;
    class MediaFile;
    
    class EventMapYokingValidation : public Event {
        
    public:
        EventMapYokingValidation(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        virtual ~EventMapYokingValidation();
        
        void addMapYokedFile(const CaretMappableDataFile* caretMapFile,
                             const MapYokingGroupEnum::Enum mapYokingGroup,
                             const int32_t tabIndex);
        
        void addMediaYokedFile(const MediaFile* mediaFile,
                               const MapYokingGroupEnum::Enum mapYokingGroup,
                               const int32_t tabIndex);
        
        void addAnnotationTextSubstitutionFile(const AnnotationTextSubstitutionFile* annTextSubFile,
                                               const MapYokingGroupEnum::Enum mapYokingGroup);
        
        void addMapYokedFileAllTabs(const CaretMappableDataFile* caretMapFile,
                                    const MapYokingGroupEnum::Enum* mapYokingGroupsForAllTabs);
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        bool validateCompatibility(const AnnotationTextSubstitutionFile* annTextSubFile,
                                   const CaretMappableDataFile* caretMapFile,
                                   const MediaFile* mediaFile,
                                   int32_t& numberOfYokedFilesOut,
                                   AString& messageOut) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        class YokedFileInfo {
        public:
            YokedFileInfo(const AnnotationTextSubstitutionFile* annTextSubFile,
                          const CaretMappableDataFile* caretMapFile,
                          const MediaFile* mediaFile,
                          const int32_t tabIndex);

            const AnnotationTextSubstitutionFile* m_annTextSubFile;
            
            const CaretMappableDataFile* m_mapFile;
            
            const MediaFile* m_mediaFile;
            
            const int32_t m_tabIndex;
            
            int32_t m_numberOfMaps;
            
            AString m_infoText;
            
            bool operator<(const YokedFileInfo& rhs) const {
                return (m_tabIndex < rhs.m_tabIndex);
            }
        };
        
        EventMapYokingValidation(const EventMapYokingValidation&);

        EventMapYokingValidation& operator=(const EventMapYokingValidation&);
        
        std::set<YokedFileInfo> m_yokedFileInfo;
        
        std::vector<int32_t> m_validTabIndices;
        
        const MapYokingGroupEnum::Enum m_mapYokingGroup;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_MAP_YOKING_VALIDATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_MAP_YOKING_VALIDATION_DECLARE__

} // namespace
#endif  //__EVENT_MAP_YOKING_VALIDATION_H__
