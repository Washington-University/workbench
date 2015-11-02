#ifndef __OVERLAY__H_
#define __OVERLAY__H_

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
#include "WholeBrainVoxelDrawingMode.h"
#include "StructureEnum.h"

namespace caret {
    class AnnotationColorBar;
    class CaretMappableDataFile;
    class SceneClassAssistant;
    
    class Overlay : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        enum IncludeVolumeFiles {
            INCLUDE_VOLUME_FILES_YES,
            INCLUDE_VOLUME_FILES_NO
        };
        
        Overlay(const std::vector<StructureEnum::Enum>& includeSurfaceStructures,
                const Overlay::IncludeVolumeFiles includeVolumeFiles);
        
        virtual ~Overlay();
        
        virtual void receiveEvent(Event* event);
        
        float getOpacity() const;
        
        void setOpacity(const float opacity);
        
        AString getName() const;
        
        void setOverlayNumber(const int32_t overlayIndex);
        
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        bool isEnabled() const;
        
        void setEnabled(const bool enabled);
        
        WholeBrainVoxelDrawingMode::Enum getWholeBrainVoxelDrawingMode() const;
        
        void setWholeBrainVoxelDrawingMode(const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode);
        
        void copyData(const Overlay* overlay);
        
        void swapData(Overlay* overlay);
        
        void getSelectionData(std::vector<CaretMappableDataFile*>& mapFilesOut,
                              CaretMappableDataFile* &selectedMapFileOut,
                              int32_t& selectedMapIndexOut);
        
        void getSelectionData(CaretMappableDataFile* &selectedMapFileOut,
                              int32_t& selectedMapIndexOut);
        
        void setSelectionData(CaretMappableDataFile* selectedMapFile,
                              const int32_t selectedMapIndex);
        
        MapYokingGroupEnum::Enum getMapYokingGroup() const;
        
        void setMapYokingGroup(const MapYokingGroupEnum::Enum mapYokingGroup);
        
        AnnotationColorBar* getColorBar();
        
        const AnnotationColorBar* getColorBar() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        Overlay(const Overlay&);

        Overlay& operator=(const Overlay&);

        /** Surface structures of data files displayed in this overlay */
        const std::vector<StructureEnum::Enum> m_includeSurfaceStructures;
        
        /** Include volume files in this overlay */
        const IncludeVolumeFiles m_includeVolumeFiles;
        
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
        
        /** available mappable files */
        //std::vector<CaretMappableDataFile*> m_mapFiles;
        
        /** selected mappable file */
        CaretMappableDataFile* m_selectedMapFile;
        
        /** selected map index */
        int32_t m_selectedMapIndex;
        
        /** selected data file map unique id */
        //AString m_selectedMapUniqueID;
        
        /** Voxel drawing mode in Whole Brain View */
        WholeBrainVoxelDrawingMode::Enum m_wholeBrainVoxelDrawingMode;
        
        /** The color bar displayed in the graphics window */
        AnnotationColorBar* m_colorBar;
        
        /** helps with scene save/restore */
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __OVERLAY_DECLARE__
#endif // __OVERLAY_DECLARE__

} // namespace
#endif  //__OVERLAY__H_
