#ifndef __MODEL_SURFACE_SELECTOR__H_
#define __MODEL_SURFACE_SELECTOR__H_

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

#include <map>

#include "CaretObject.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {
    
    class Model;
    class ModelSurface;
    
    class ModelSurfaceSelector : public CaretObject, public SceneableInterface {
        
    public:
        ModelSurfaceSelector();
        
        virtual ~ModelSurfaceSelector();
        
        ModelSurface* getSelectedSurfaceModel();
        
        StructureEnum::Enum getSelectedStructure();
        
        void setSelectedSurfaceModel(ModelSurface* surfaceModel);
        
        void setSelectedStructure(const StructureEnum::Enum selectedStructure);
        
        void getSelectableStructures(
                        std::vector<StructureEnum::Enum>& selectableSructuresOut) const;
                                     
        void getSelectableSurfaceModels(
                        std::vector<ModelSurface*>& selectableSurfaceModelsOut) const;
        
        void updateSelector(const std::vector<Model*> modelDisplayModels);
        
        AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private:
        ModelSurfaceSelector(const ModelSurfaceSelector&);

        ModelSurfaceSelector& operator=(const ModelSurfaceSelector&);
        
        void updateSelector();
        
    private:
        std::vector<StructureEnum::Enum> m_availableStructures;
        
        StructureEnum::Enum m_selectedStructure;
        
        StructureEnum::Enum m_defaultStructure;
        
        std::vector<ModelSurface*> m_allSurfaceModels;
                            
        std::vector<ModelSurface*> m_availableSurfaceModels;
        
        ModelSurface* m_selectedSurfaceModel;
        
        std::map<StructureEnum::Enum, ModelSurface*> m_previousSelectedSurfaceModel;
    };
    
#ifdef __MODEL_SURFACE_SELECTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MODEL_SURFACE_SELECTOR_DECLARE__

} // namespace
#endif  //__MODEL_SURFACE_SELECTOR__H_
