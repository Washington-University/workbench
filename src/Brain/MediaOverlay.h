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

#include <array>
#include <set>

#include "CaretObject.h"
#include "CziImageResolutionChangeModeEnum.h"
#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "PlainTextStringBuilder.h"
#include "SceneableInterface.h"

namespace caret {
    class CziImageFile;
    class MediaFile;
    class SceneClassAssistant;
    
    class MediaOverlay : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        MediaOverlay(const int32_t tabIndex,
                     const int32_t overlayIndex);
        
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
        
        class SelectionData {
        public:
            SelectionData() { }
            
            SelectionData(const int32_t tabIndex,
                          const int32_t overlayIndex,
                          std::vector<MediaFile*>& mediaFiles,
                          MediaFile* selectedMediaFile,
                          CziImageFile* selectedCziImageFile,
                          const int32_t selectedFrameIndex,
                          const AString selectedFrameName,
                          const bool fileSupportsAllFramesFlag,
                          const bool allFramesSelectedFlag,
                          const bool supportsYokingFlag,
                          const bool allChannelsSelectedFlag,
                          const int32_t selectedChannelIndex,
                          const CziImageResolutionChangeModeEnum::Enum cziResolutionChangeMode,
                          const int32_t cziManualPyramidLayerIndex,
                          const int32_t cziManualPyramidLayerMinimumValue,
                          const int32_t cziManualPyramidLayerMaximumValue)
            :
            m_tabIndex(tabIndex),
            m_overlayIndex(overlayIndex),
            m_mediaFiles(mediaFiles),
            m_selectedMediaFile(selectedMediaFile),
            m_constSelectedMediaFile(selectedMediaFile),
            m_selectedCziImageFile(selectedCziImageFile),
            m_selectedFrameIndex(selectedFrameIndex),
            m_selectedFrameName(selectedFrameName),
            m_fileSupportsAllFramesFlag(fileSupportsAllFramesFlag),
            m_allFramesSelectedFlag(allFramesSelectedFlag),
            m_supportsYokingFlag(supportsYokingFlag),
            m_allChannelsSelectedFlag(allChannelsSelectedFlag),
            m_selectedChannelIndex(selectedChannelIndex),
            m_cziResolutionChangeMode(cziResolutionChangeMode),
            m_cziManualPyramidLayerIndex(cziManualPyramidLayerIndex),
            m_cziManualPyramidLayerMinimumValue(cziManualPyramidLayerMinimumValue),
            m_cziManualPyramidLayerMaximumValue(cziManualPyramidLayerMaximumValue)
            {
                if ( ! m_fileSupportsAllFramesFlag) {
                    m_allFramesSelectedFlag = false;
                }
            }
            
            int32_t m_tabIndex = -1;
            int32_t m_overlayIndex = -1;
            std::vector<MediaFile*> m_mediaFiles;
            MediaFile* m_selectedMediaFile = NULL;
            const MediaFile* m_constSelectedMediaFile = NULL;
            CziImageFile* m_selectedCziImageFile = NULL;
            int32_t m_selectedFrameIndex = 0;
            AString m_selectedFrameName;
            bool m_fileSupportsAllFramesFlag = false;
            bool m_allFramesSelectedFlag = false;
            bool m_supportsYokingFlag = false;
            bool m_allChannelsSelectedFlag = false;
            int32_t m_selectedChannelIndex = -1;
            
            CziImageResolutionChangeModeEnum::Enum m_cziResolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;
            int32_t m_cziManualPyramidLayerIndex = 0;
            int32_t m_cziManualPyramidLayerMinimumValue = 0;
            int32_t m_cziManualPyramidLayerMaximumValue = 0;
        };
        
        SelectionData getSelectionData();
        
        SelectionData getSelectionData() const;
        
        void setSelectionData(MediaFile* selectedFile,
                              const int32_t selectedFrameIndex);
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        void setCziPyramidLayerIndex(const int32_t pyramidLayerIndex);
        
        void setCziAllScenesSelected(const bool selectAll);

        int32_t getSelectedChannelIndex() const;
        
        bool isAllChannelsSelected() const;
        
        void setSelectedChannelIndex(const int32_t channelIndex);
        
        void setAllChannelsSelected(const bool status);
        
        void setCziResolutionChangeMode(const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        MediaOverlay(const MediaOverlay&);

        MediaOverlay& operator=(const MediaOverlay&);

        /** Name of overlay (DO NOT COPY)*/
        AString m_name;
        
        /** Tab index containing this overlay (DO NOT COPY) **/
        const int32_t m_tabIndex;
        
        /** Index of this overlay (DO NOT COPY)*/
        const int32_t m_overlayIndex;
        
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

        bool m_allFramesSelectedFlag = true;
        
        int32_t m_selectedChannelIndex = 1;
        
        bool m_allChannelsSelectedFlag = true;
        
        CziImageResolutionChangeModeEnum::Enum m_cziResolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;

        int32_t m_cziManualPyramidLayerIndex = 1;
        
        MediaFile* m_previousSelectedFile = NULL;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
        
        friend class MediaOverlaySettingsMenu;
    };
    
#ifdef __MEDIA_OVERLAY_DECLARE__
#endif // __MEDIA_OVERLAY_DECLARE__

} // namespace
#endif  //__MEDIA_OVERLAY__H_
