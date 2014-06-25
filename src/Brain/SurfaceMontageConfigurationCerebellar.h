#ifndef __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_H__
#define __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_H__

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
    
    class SurfaceMontageConfigurationCerebellar : public SurfaceMontageConfigurationAbstract {
        
    public:
        SurfaceMontageConfigurationCerebellar(const int32_t tabIndex);
        
        virtual ~SurfaceMontageConfigurationCerebellar();
        
        virtual void initializeSelectedSurfaces();
        
        SurfaceSelectionModel* getFirstSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getFirstSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getSecondSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getSecondSurfaceSelectionModel() const;
        
        bool isFirstSurfaceEnabled() const;
        
        void setFirstSurfaceEnabled(const bool enabled);
        
        bool isSecondSurfaceEnabled() const;
        
        void setSecondSurfaceEnabled(const bool enabled);
        
        bool isDorsalEnabled() const;
        
        void setDorsalEnabled(const bool enabled);
        
        bool isVentralEnabled() const;
        
        void setVentralEnabled(const bool enabled);
        
        bool isAnteriorEnabled() const;
        
        void setAnteriorEnabled(const bool enabled);
        
        bool isPosteriorEnabled() const;
        
        void setPosteriorEnabled(const bool enabled);
        
        virtual void updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports);
        
        virtual bool isValid();
        
        virtual void copyConfiguration(SurfaceMontageConfigurationAbstract* configuration);
        
    private:
        SurfaceMontageConfigurationCerebellar(const SurfaceMontageConfigurationCerebellar&);

        SurfaceMontageConfigurationCerebellar& operator=(const SurfaceMontageConfigurationCerebellar&);
        
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

        SurfaceSelectionModel* m_firstSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_secondSurfaceSelectionModel;
        
        bool m_firstSurfaceEnabled;
        
        bool m_secondSurfaceEnabled;
        
        bool m_dorsalEnabled;
        
        bool m_ventralEnabled;
        
        bool m_anteriorEnabled;
        
        bool m_posteriorEnabled;
        
    // ADD_NEW_MEMBERS_HERE

        friend class ModelSurfaceMontage;
        
    };
    
#ifdef __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_H__
