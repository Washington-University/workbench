#ifndef __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_H__
#define __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_H__

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


#include "CaretObject.h"

#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "SurfaceMontageConfigurationTypeEnum.h"
#include "SurfaceMontageLayoutOrientationEnum.h"
#include "SurfaceMontageViewport.h"

namespace caret {
    class OverlaySet;
    class PlainTextStringBuilder;
    class SceneClassAssistant;
    class SurfaceMontageViewport;

    class SurfaceMontageConfigurationAbstract : public CaretObject, public SceneableInterface {
        
    public:
        enum SupportLayoutOrientation {
            SUPPORTS_LAYOUT_ORIENTATION_YES,
            SUPPORTS_LAYOUT_ORIENTATION_NO,
        };
        
        SurfaceMontageConfigurationAbstract(const SurfaceMontageConfigurationTypeEnum::Enum configuration,
                                            const SupportLayoutOrientation supportsLayoutOrientation);
        
        virtual ~SurfaceMontageConfigurationAbstract();
        
        SurfaceMontageConfigurationTypeEnum::Enum getConfigurationType() const;
        
        virtual void initializeSelectedSurfaces() = 0;
        
        virtual bool isValid() = 0;
        
        OverlaySet* getOverlaySet();
        
        const OverlaySet* getOverlaySet() const;
        
        bool hasLayoutOrientation() const;
        
        SurfaceMontageLayoutOrientationEnum::Enum getLayoutOrientation() const;
        
        void setLayoutOrientation(const SurfaceMontageLayoutOrientationEnum::Enum layoutOrientation);
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
        void getSurfaceMontageViewportsForDrawing(std::vector<SurfaceMontageViewport*>& surfaceMontageViewports);
        
        void getSurfaceMontageViewportsForTransformation(std::vector<const SurfaceMontageViewport*>& surfaceMontageViewports) const;
        
        virtual void copyConfiguration(SurfaceMontageConfigurationAbstract* configuration);
        
    private:
        SurfaceMontageConfigurationAbstract(const SurfaceMontageConfigurationAbstract&);

        SurfaceMontageConfigurationAbstract& operator=(const SurfaceMontageConfigurationAbstract&);
        
    protected:
        
        /**
         * Update the montage viewports using the current selected surfaces and settings.
         *
         * @param surfaceMontageViewports
         *     Will be loaded with the montage viewports.
         */
        virtual void updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports) = 0;
        
        void setupOverlaySet(const AString& overlaySetName,
                             const int32_t tabIndex,
                             const std::vector<StructureEnum::Enum>& includeSurfaceStructures);
        
        virtual void saveMembersToScene(const SceneAttributes* sceneAttributes,
                                                         SceneClass* sceneClass) = 0;
        
        virtual void restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                                              const SceneClass* sceneClass) = 0;

    public:

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const = 0;
        
        virtual void getDisplayedSurfaces(std::vector<Surface*>& surfacesOut) const = 0;
        
    private:
        SceneClassAssistant* m_sceneAssistant;

        const SurfaceMontageConfigurationTypeEnum::Enum m_configurationType;
        
        const SupportLayoutOrientation m_supportsLayoutOrientation;
        
        SurfaceMontageLayoutOrientationEnum::Enum m_layoutOrientation;
        
        OverlaySet* m_overlaySet;
        
        std::vector<SurfaceMontageViewport> m_surfaceMontageViewports;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class ModelSurfaceMontage;

    };
    
#ifdef __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_H__
