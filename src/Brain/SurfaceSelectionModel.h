#ifndef __SURFACE_SELECTION_MODEL_H__
#define __SURFACE_SELECTION_MODEL_H__

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
#include "SurfaceTypeEnum.h"

namespace caret {

    class Surface;
    
    class SurfaceSelectionModel : public CaretObject, public SceneableInterface {
        
    public:
        SurfaceSelectionModel(const StructureEnum::Enum structure,
                              const std::vector<SurfaceTypeEnum::Enum>& surfaceTypes);
        
        SurfaceSelectionModel(const std::vector<SurfaceTypeEnum::Enum>& surfaceTypes);
        
        virtual ~SurfaceSelectionModel();
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        void setSurface(Surface* surface);
        
        void setSurfaceToType(const SurfaceTypeEnum::Enum surfaceType,
                              const SurfaceTypeEnum::Enum surfaceType2 = SurfaceTypeEnum::UNKNOWN,
                              const SurfaceTypeEnum::Enum surfaceType3 = SurfaceTypeEnum::UNKNOWN,
                              const SurfaceTypeEnum::Enum surfaceType4 = SurfaceTypeEnum::UNKNOWN,
                              const SurfaceTypeEnum::Enum surfaceType5 = SurfaceTypeEnum::UNKNOWN);
        
        std::vector<Surface*> getAvailableSurfaces() const;
        
        void updateModel() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        SurfaceSelectionModel(const SurfaceSelectionModel&);

        SurfaceSelectionModel& operator=(const SurfaceSelectionModel&);
        
        mutable Surface* m_selectedSurface;
        
        /** If empty, allow any structure, otherwise restrict to these structures */
        std::vector<StructureEnum::Enum> m_allowableStructures;
        
        /** If empty, allow any surface type, otherwise restrict to these types */
        std::vector<SurfaceTypeEnum::Enum> m_allowableSurfaceTypes;
        
    };
    
#ifdef __SURFACE_SELECTION_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_SELECTION_MODEL_DECLARE__

} // namespace
#endif  //__SURFACE_SELECTION_MODEL_H__
