#ifndef __SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_H__
#define __SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_H__

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


#include "SurfaceMontageConfigurationAbstract.h"

namespace caret {
    class SceneClassAssistant;
    class SurfaceSelectionModel;
    
    class SurfaceMontageConfigurationHippocampus : public SurfaceMontageConfigurationAbstract {
        
    public:
        SurfaceMontageConfigurationHippocampus(const int32_t tabIndex);
        
        virtual ~SurfaceMontageConfigurationHippocampus();
        
        SurfaceSelectionModel* getLeftFirstSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getLeftFirstSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getLeftSecondSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getLeftSecondSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getRightFirstSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getRightFirstSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getRightSecondSurfaceSelectionModel();

        const SurfaceSelectionModel* getRightSecondSurfaceSelectionModel() const;
        
        bool isLeftEnabled() const;
        
        void setLeftEnabled(const bool enabled);
        
        bool isRightEnabled() const;
        
        void setRightEnabled(const bool enabled);
        
        bool isLateralEnabled() const;
        
        void setLateralEnabled(const bool enabled);
        
        bool isMedialEnabled() const;
        
        void setMedialEnabled(const bool enabled);
        
        bool isFirstSurfaceEnabled() const;
        
        void setFirstSurfaceEnabled(const bool enabled);
        
        bool isSecondSurfaceEnabled() const;
        
        void setSecondSurfaceEnabled(const bool enabled);
        
        virtual void initializeSelectedSurfaces();
        
        virtual bool isValid();
        
        virtual void updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports);
        
        virtual void copyConfiguration(SurfaceMontageConfigurationAbstract* configuration);
        
    private:
        SurfaceMontageConfigurationHippocampus(const SurfaceMontageConfigurationHippocampus&);

        SurfaceMontageConfigurationHippocampus& operator=(const SurfaceMontageConfigurationHippocampus&);
        
    public:
        virtual AString toString() const;
        
        virtual void getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const;
        
        virtual void getDisplayedSurfaces(std::vector<Surface*>& surfacesOut) const;
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveMembersToScene(const SceneAttributes* sceneAttributes,
                                        SceneClass* sceneClass);
        
        virtual void restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                             const SceneClass* sceneClass);
        
    private:
        SceneClassAssistant* m_sceneAssistant;

        SurfaceSelectionModel* m_leftFirstSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_leftSecondSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_rightFirstSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_rightSecondSurfaceSelectionModel;
        
        bool m_leftEnabled;
        
        bool m_rightEnabled;
        
        bool m_firstSurfaceEnabled;
        
        bool m_secondSurfaceEnabled;
        
        bool m_lateralEnabled;
        
        bool m_medialEnabled;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ModelSurfaceMontage;
        
    };
    
#ifdef __SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_H__
