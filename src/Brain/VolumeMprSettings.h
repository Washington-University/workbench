#ifndef __VOLUME_MPR_SETTINGS_H__
#define __VOLUME_MPR_SETTINGS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"
#include "VolumeMprOrientationModeEnum.h"
#include "VolumeMprViewModeEnum.h"
#include "SceneableInterface.h"


namespace caret {
    class SceneClassAssistant;

    class VolumeMprSettings : public CaretObject, public SceneableInterface {
        
    public:
        VolumeMprSettings();
        
        virtual ~VolumeMprSettings();
        
        VolumeMprSettings(const VolumeMprSettings& obj);

        VolumeMprSettings& operator=(const VolumeMprSettings& obj);
        
        VolumeMprOrientationModeEnum::Enum getOrientationMode() const;
        
        void setOrientationMode(const VolumeMprOrientationModeEnum::Enum orientation);
        
        VolumeMprViewModeEnum::Enum getViewMode() const;
        
        void setViewMode(const VolumeMprViewModeEnum::Enum viewType);
        
        float getSliceThickness() const;
        
        void setSliceThickness(const float sliceThickness);

        bool isAxialSliceThicknessEnabled() const;
        
        void setAxialSliceThicknessEnabled(const bool enabled);

        bool isCoronalSliceThicknessEnabled() const;
        
        void setCoronalSliceThicknessEnabled(const bool enabled);
        
        bool isParasagittalSliceThicknessEnabled() const;
        
        void setParasagittalSliceThicknessEnabled(const bool enabled);
        
        void reset();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperVolumeMprSettings(const VolumeMprSettings& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        VolumeMprViewModeEnum::Enum m_viewMode = VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION;
        
        VolumeMprOrientationModeEnum::Enum m_orientationMode = VolumeMprOrientationModeEnum::NEUROLOGICAL;
        
        float m_sliceThickness = 20.0f;
        
        bool m_axialSliceThicknessEnabled = false;
        
        bool m_coronalSliceThicknessEnabled = false;
        
        bool m_parasagittalSliceThicknessEnabled = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_MPR_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MPR_SETTINGS_DECLARE__

} // namespace
#endif  //__VOLUME_MPR_SETTINGS_H__
