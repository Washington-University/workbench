#ifndef __MEDIA_OVERLAY__H_
#define __MEDIA_OVERLAY__H_

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

#include <set>

#include "CaretObject.h"
#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "PlainTextStringBuilder.h"
#include "SceneableInterface.h"

namespace caret {
    class MediaFile;
    class SceneClassAssistant;
    
    class MediaOverlay : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        MediaOverlay();
        
        virtual ~MediaOverlay();
        
        virtual void receiveEvent(Event* event);
        
        float getOpacity() const;
        
        void setOpacity(const float opacity);
        
        AString getName() const;
        
        void setOverlayNumber(const int32_t overlayIndex);
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        void copyData(const MediaOverlay* overlay);
        
        void swapData(MediaOverlay* overlay);
        
        void getSelectionData(std::vector<MediaFile*>& filesOut,
                              MediaFile* &selectedFileOut,
                              int32_t& selectedFrameIndexOut);
        
        void getSelectionData(MediaFile* &selectedFileOut,
                              int32_t& selectedFrameIndexOut);
        
        void setSelectionData(MediaFile* selectedFile,
                              const int32_t selectedFrameIndex);
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        MediaOverlay(const MediaOverlay&);

        MediaOverlay& operator=(const MediaOverlay&);

        /** Name of overlay (DO NOT COPY)*/
        AString m_name;
        
        /** Index of this overlay (DO NOT COPY)*/
        int32_t m_overlayIndex;
        
        /** opacity for overlay */
        float m_opacity;
        
        /** enabled status */
        mutable bool m_enabled;
        
        /** map yoking group */
        MapYokingGroupEnum::Enum m_mapYokingGroup;
        
        /** selected  file */
        MediaFile* m_selectedFile;
        
        /** selected frame index */
        int32_t m_selectedFrameIndex;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __MEDIA_OVERLAY_DECLARE__
#endif // __MEDIA_OVERLAY_DECLARE__

} // namespace
#endif  //__MEDIA_OVERLAY__H_
