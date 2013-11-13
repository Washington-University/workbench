#ifndef __EVENT_OVERLAY_GET_YOKED_H__
#define __EVENT_OVERLAY_GET_YOKED_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_OVERLAY_GET_YOKED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_OVERLAY_GET_YOKED_DECLARE__

} // namespace
#endif  //__EVENT_OVERLAY_GET_YOKED_H__
