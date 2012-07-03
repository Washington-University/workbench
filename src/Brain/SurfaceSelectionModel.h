#ifndef __SURFACE_SELECTION_MODEL_H__
#define __SURFACE_SELECTION_MODEL_H__

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "CaretObject.h"

#include "SceneableInterface.h"
#include "StructureEnum.h"
#include "SurfaceTypeEnum.h"

namespace caret {

    class BrainStructure;
    class Surface;
    
    class SurfaceSelectionModel : public CaretObject, public SceneableInterface {
        
    public:
        SurfaceSelectionModel();
        
        SurfaceSelectionModel(const StructureEnum::Enum structure);
        
        SurfaceSelectionModel(BrainStructure* brainStructure);
        
        SurfaceSelectionModel(const SurfaceTypeEnum::Enum surfaceType);
        
        virtual ~SurfaceSelectionModel();
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        void setSurface(Surface* surface);
        
        std::vector<Surface*> getAvailableSurfaces() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        enum Mode {
            MODE_BRAIN_STRUCTURE,
            MODE_STRUCTURE,
            MODE_SURFACE_TYPE
        };
        
        SurfaceSelectionModel(const SurfaceSelectionModel&);

        SurfaceSelectionModel& operator=(const SurfaceSelectionModel&);
        
        void updateSelection() const;
        
        mutable Surface* m_selectedSurface;
        
        BrainStructure* m_brainStructure;
        
        Mode m_mode;
        
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
