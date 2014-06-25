#ifndef __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_H__
#define __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_H__

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
    
    class SurfaceMontageConfigurationFlatMaps : public SurfaceMontageConfigurationAbstract {
        
    public:
        SurfaceMontageConfigurationFlatMaps(const int32_t tabIndex);
        
        virtual ~SurfaceMontageConfigurationFlatMaps();
        
        virtual void initializeSelectedSurfaces();
        
        virtual bool isValid();
        
        SurfaceSelectionModel* getLeftSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getLeftSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getRightSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getRightSurfaceSelectionModel() const;
        
        SurfaceSelectionModel* getCerebellumSurfaceSelectionModel();
        
        const SurfaceSelectionModel* getCerebellumSurfaceSelectionModel() const;
        
        bool isLeftEnabled() const;
        
        void setLeftEnabled(const bool enabled);
        
        bool isRightEnabled() const;
        
        void setRightEnabled(const bool enabled);
        
        bool isCerebellumEnabled() const;
        
        void setCerebellumEnabled(const bool enabled);
        
        virtual void updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports);
        
        virtual void copyConfiguration(SurfaceMontageConfigurationAbstract* configuration);
        
    private:
        SurfaceMontageConfigurationFlatMaps(const SurfaceMontageConfigurationFlatMaps&);

        SurfaceMontageConfigurationFlatMaps& operator=(const SurfaceMontageConfigurationFlatMaps&);
        
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

        SurfaceSelectionModel* m_leftSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_rightSurfaceSelectionModel;
        
        SurfaceSelectionModel* m_cerebellumSurfaceSelectionModel;
        
        bool m_leftEnabled;
        
        bool m_rightEnabled;
        
        bool m_cerebellumEnabled;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ModelSurfaceMontage;
        
    };
    
#ifdef __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_DECLARE__

} // namespace
#endif  //__SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_H__
