#ifndef __HISTOLOGY_OVERLAY__H_
#define __HISTOLOGY_OVERLAY__H_

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
#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "MapYokingGroupEnum.h"
#include "PlainTextStringBuilder.h"
#include "SceneableInterface.h"

namespace caret {
    class CziImageFile;
    class HistologySlice;
    class HistologySlicesFile;
    class HistologySliceImage;
    class MediaFile;
    class SceneClassAssistant;
    
    class HistologyOverlay : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        HistologyOverlay(const int32_t tabIndex,
                         const int32_t overlayIndex);
        
        virtual ~HistologyOverlay();
        
        virtual void receiveEvent(Event* event);
        
        float getOpacity() const;
        
        void setOpacity(const float opacity);
        
        AString getName() const;
        
        void setOverlayNumber(const int32_t overlayIndex);
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        void copyData(const HistologyOverlay* overlay);
        
        void swapData(HistologyOverlay* overlay);
        
        class DrawingData {
        public:
            DrawingData() { }
            
            DrawingData(const int32_t tabIndex,
                        const int32_t overlayIndex,
                        HistologySlicesFile* selectedFile,
                        HistologySlice* histologySlice,
                        HistologySliceImage* histologySliceImage,
                        MediaFile* mediaFile,
                        const int32_t selectedSliceIndex,
                        const int32_t selectedSliceNumber,
                        const int32_t mediaFileIndexInSlice,
                        const bool supportsYokingFlag)
            :
            m_tabIndex(tabIndex),
            m_overlayIndex(overlayIndex),
            m_selectedFile(selectedFile),
            m_histologySlice(histologySlice),
            m_histologySliceImage(histologySliceImage),
            m_mediaFile(mediaFile),
            m_selectedSliceIndex(selectedSliceIndex),
            m_selectedSliceNumber(selectedSliceNumber),
            m_mediaFileIndexInSlice(mediaFileIndexInSlice),
            m_supportsYokingFlag(supportsYokingFlag) {  }
            
            int32_t m_tabIndex = -1;
            int32_t m_overlayIndex = -1;
            HistologySlicesFile* m_selectedFile = NULL;
            HistologySlice* m_histologySlice = NULL;
            HistologySliceImage* m_histologySliceImage = NULL;
            MediaFile* m_mediaFile = NULL;
            int32_t m_selectedSliceIndex = 0;
            int32_t m_selectedSliceNumber = 0;
            int32_t m_mediaFileIndexInSlice = 0;
            bool m_supportsYokingFlag = false;
        };
        
        class SelectionData {
        public:
            SelectionData() { }
            
            SelectionData(const int32_t tabIndex,
                          const int32_t overlayIndex,
                          std::vector<HistologySlicesFile*>& allFiles,
                          HistologySlicesFile* selectedFile,
                          const int32_t selectedSliceIndex,
                          const int32_t selectedSliceNumber,
                          const bool supportsYokingFlag)
            :
            m_tabIndex(tabIndex),
            m_overlayIndex(overlayIndex),
            m_allFiles(allFiles),
            m_selectedFile(selectedFile),
            m_selectedSliceIndex(selectedSliceIndex),
            m_selectedSliceNumber(selectedSliceNumber),
            m_supportsYokingFlag(supportsYokingFlag) {  }
            
            int32_t m_tabIndex = -1;
            int32_t m_overlayIndex = -1;
            std::vector<HistologySlicesFile*> m_allFiles;
            HistologySlicesFile* m_selectedFile = NULL;
            int32_t m_selectedSliceIndex = 0;
            int32_t m_selectedSliceNumber;
            bool m_supportsYokingFlag = false;
        };
        
        std::vector<DrawingData> getDrawingData(const int32_t selectedSliceIndex) const;
        
        SelectionData getSelectionData();
        
        SelectionData getSelectionData() const;
        
        void setSelectionData(HistologySlicesFile* selectedFile,
                              const int32_t selectedSliceIndex);
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        HistologyOverlay(const HistologyOverlay&);

        HistologyOverlay& operator=(const HistologyOverlay&);

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
        HistologySlicesFile* m_selectedFile;
        
        /** selected slice index */
        int32_t m_selectedSliceIndex;

        HistologySlicesFile* m_previousSelectedFile = NULL;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
        
//        friend class HistologyOverlaySettingsMenu;
    };
    
#ifdef __HISTOLOGY_OVERLAY_DECLARE__
#endif // __HISTOLOGY_OVERLAY_DECLARE__

} // namespace
#endif  //__HISTOLOGY_OVERLAY__H_
