#ifndef __EVENT_OVERLAY_GET_YOKED_H__
#define __EVENT_OVERLAY_GET_YOKED_H__

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


#include "Event.h"
#include "OverlayYokingGroupEnum.h"

namespace caret {
    class BrowserTabContent;
    class CaretMappableDataFile;
    class Overlay;
    
    class EventOverlayYokingGroupGet : public Event {
        
    public:
        EventOverlayYokingGroupGet(const OverlayYokingGroupEnum::Enum yokingGroup);
        
        virtual ~EventOverlayYokingGroupGet();
        
        OverlayYokingGroupEnum::Enum getYokingGroup() const;
        
        void addYokedOverlay(const AString& modelName,
                             const int32_t tabIndex,
                             Overlay* overlay);
        
        int32_t getNumberOfYokedOverlays() const;
        
        Overlay* getYokedOverlay(const int32_t indx);
        
//        void getYokedOverlay(const int32_t indx,
//                             AString& modelNameOut,
//                             int32_t tabIndexOut,
//                             Overlay*& overlayOut,
//                             CaretMappableDataFile*& overlayFileOut,
//                             AString& overlayFileNameOut,
//                             int32_t& overlayFileNumberOfMapsOut) const;
        
//        int32_t getMaximumMapIndex() const;
//
//        void setMapIndex(const int32_t mapIndex);
//        
//        void synchronizeSelectedMaps(Overlay* overlay);
        
        bool validateCompatibility(CaretMappableDataFile* overlayFile,
                                   AString& messageOut) const;
        
    private:
        EventOverlayYokingGroupGet(const EventOverlayYokingGroupGet&);

        EventOverlayYokingGroupGet& operator=(const EventOverlayYokingGroupGet&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        struct YokedOverlayInfo {
            AString m_modelName;
           
            int32_t m_tabIndex;
            
            Overlay* m_overlay;
            
            CaretMappableDataFile* m_overlayFile;
            
            AString m_overlayFileName;
            
            int32_t m_overlayFileNumberOfMaps;
        };
        
        std::vector<YokedOverlayInfo> m_yokedOverlays;
        
        const OverlayYokingGroupEnum::Enum m_yokingGroup;

        std::vector<int32_t> m_validTabIndices;
        
        bool m_addingFirstYokedOverlayFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_OVERLAY_GET_YOKED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_OVERLAY_GET_YOKED_DECLARE__

} // namespace
#endif  //__EVENT_OVERLAY_GET_YOKED_H__
