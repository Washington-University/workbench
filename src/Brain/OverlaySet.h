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
        void findUnderlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                               const bool includeVolumeFiles,
                               std::vector<CaretMappableDataFile*>& filesOut,
                               std::vector<int32_t>& mapIndicesOut);
        
        void findMiddleLayerFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                                  const bool includeVolumeFiles,
                                  std::vector<CaretMappableDataFile*>& filesOut,
                                  std::vector<int32_t>& mapIndicesOut);
        
        void findOverlayFiles(const std::vector<StructureEnum::Enum>& matchToStructures,
                              const bool includeVolumeFiles,
                              std::vector<CaretMappableDataFile*>& filesOut,
                              std::vector<int32_t>& mapIndicesOut);
        
        
        OverlaySet(const OverlaySet&);
        
        OverlaySet& operator=(const OverlaySet&);
        
        Overlay* m_overlays[BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS];
        
        bool findFilesWithMapNamed(std::vector<CaretMappableDataFile*>& matchedFilesOut,
                                   std::vector<int32_t>& matchedFileIndicesOut,
                                   const std::vector<StructureEnum::Enum>& matchToStructures,
                                   const DataFileTypeEnum::Enum dataFileType,
                                   const bool matchToVolumeData,
                                   const AString& matchToNamesRegularExpressionText,
                                   const bool matchToNamesRegularExpressionResult,
                                   const bool matchOneFilePerStructure);
        
        std::vector<VolumeFile*> getVolumeFiles() const;
        
        AString m_name;
        
        int32_t m_tabIndex;
        
        /** Surface structures of data files displayed in this overlay */
        const std::vector<StructureEnum::Enum> m_includeSurfaceStructures;
        
        /** Include volume files in this overlay */
        const Overlay::IncludeVolumeFiles m_includeVolumeFiles;
        
        int32_t m_numberOfDisplayedOverlays;
        
        SceneClassAssistant* m_sceneAssistant;

        /** regular expression for matching myeline names - NOT saved to scenes */
        static const AString s_myelinMatchRegularExpressionText;
        
        /** regular expression for matching shape names - NOT saved to scenes */
        static const AString s_shapeMatchRegularExpressionText;
        
        /** regular expression for matching shape and myelin names - NOT saved to scenes */
        static const AString s_shapeMyelinMatchRegularExpressionText;
    };
    
#ifdef __OVERLAY_SET_DECLARE__
    AString const OverlaySet::s_myelinMatchRegularExpressionText = "(myelin)";
    AString const OverlaySet::s_shapeMatchRegularExpressionText = "(sulc|shape|curv|depth|thick)";
    AString const OverlaySet::s_shapeMyelinMatchRegularExpressionText = "(myelin|sulc|shape|curv|depth|thick)";
#endif // __OVERLAY_SET_DECLARE__

} // namespace
#endif  //__OVERLAY_SET__H_
