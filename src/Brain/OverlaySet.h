#ifndef __OVERLAY_SET__H_
#define __OVERLAY_SET__H_

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

#include "BrainConstants.h"
#include "CaretObject.h"
#include "EventListenerInterface.h"
#include "Overlay.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {

    class BrowserTabContent;
    class LabelFile;
    class SceneClassAssistant;
    class Surface;
    class PlaneTextStringBuilder;
    class VolumeFile;
    class VolumeMappableInterface;
    
    class OverlaySet : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
    public:
        OverlaySet(const AString& name,
                   const int32_t tabIndex,
                   const std::vector<StructureEnum::Enum>& includeSurfaceStructures,
                   const Overlay::IncludeVolumeFiles includeVolumeFiles);
        
        virtual ~OverlaySet();
        
        virtual void receiveEvent(Event* event);
        
        void copyOverlaySet(const OverlaySet* overlaySet);
        
        Overlay* getPrimaryOverlay();
        
        Overlay* getUnderlay();
        
        VolumeMappableInterface* getUnderlayVolume();
        
        const VolumeMappableInterface* getUnderlayVolume() const;
        
        Overlay* getUnderlayContainingVolume();
        
        Overlay* getOverlay(const int32_t overlayNumber);
        
        const Overlay* getOverlay(const int32_t overlayNumber) const;
        
        void addDisplayedOverlay();
        
        void setNumberOfDisplayedOverlays(const int32_t numberOfDisplayedOverlays);
        
        int32_t getNumberOfDisplayedOverlays() const;
        
        void insertOverlayAbove(const int32_t overlayIndex);
        
        void insertOverlayBelow(const int32_t overlayIndex);
        
        void removeDisplayedOverlay(const int32_t overlayIndex);
        
        void moveDisplayedOverlayUp(const int32_t overlayIndex);
        
        void moveDisplayedOverlayDown(const int32_t overlayIndex);
        
        VolumeMappableInterface* setUnderlayToVolume();
        
        void initializeOverlays();
        
        void getSelectedMapIndicesForFile(const CaretMappableDataFile* caretMappableDataFile,
                                    const bool isLimitToEnabledOverlays,
                                    std::vector<int32_t>& selectedMapIndicesOut) const;
        
        void getLabelFilesForSurface(const Surface* surface,
                                     std::vector<LabelFile*>& labelFilesOut,
                                     std::vector<int32_t>& labelMapIndicesOut);
        
        void getLabelMappedFilesAndMapIndices(std::vector<CaretMappableDataFile*>& mapFilesOut,
                                              std::vector<int32_t>& mapIndicesOut) const;
        
        bool hasObliqueOnlyVolumeSelected() const;
        
        void resetOverlayYokingToOff();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    public:
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
    private:
        OverlaySet(const OverlaySet&);
        
        OverlaySet& operator=(const OverlaySet&);
        
        Overlay* m_overlays[BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        AString m_name;
        
        int32_t m_tabIndex;
        
        /** Surface structures of data files displayed in this overlay */
        const std::vector<StructureEnum::Enum> m_includeSurfaceStructures;
        
        /** Include volume files in this overlay */
        const Overlay::IncludeVolumeFiles m_includeVolumeFiles;
        
        int32_t m_numberOfDisplayedOverlays;
        
        SceneClassAssistant* m_sceneAssistant;
    };
    
#ifdef __OVERLAY_SET_DECLARE__
#endif // __OVERLAY_SET_DECLARE__

} // namespace
#endif  //__OVERLAY_SET__H_
