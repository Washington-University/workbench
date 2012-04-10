#ifndef __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR__H_
#define __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR__H_

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

#include <map>

#include "CaretObject.h"
#include "StructureEnum.h"

namespace caret {
    
    class Model;
    class ModelSurface;
    
    class ModelSurfaceSelector : public CaretObject {
        
    public:
        ModelSurfaceSelector();
        
        virtual ~ModelSurfaceSelector();
        
        ModelSurface* getSelectedSurfaceController();
        
        StructureEnum::Enum getSelectedStructure();
        
        void setSelectedSurfaceController(ModelSurface* surfaceController);
        
        void setSelectedStructure(const StructureEnum::Enum selectedStructure);
        
        void getSelectableStructures(
                        std::vector<StructureEnum::Enum>& selectableSructuresOut) const;
                                     
        void getSelectableSurfaceControllers(
                        std::vector<ModelSurface*>& selectableSurfaceControllersOut) const;
        
        void updateSelector(const std::vector<Model*> modelDisplayControllers);
        
        AString toString() const;
    private:
        ModelSurfaceSelector(const ModelSurfaceSelector&);

        ModelSurfaceSelector& operator=(const ModelSurfaceSelector&);
        
        void updateSelector();
        
    private:
        std::vector<StructureEnum::Enum> availableStructures;
        
        StructureEnum::Enum selectedStructure;
        
        StructureEnum::Enum defaultStructure;
        
        std::vector<ModelSurface*> allSurfaceControllers;
                            
        std::vector<ModelSurface*> availableSurfaceControllers;
        
        ModelSurface* selectedSurfaceController;
        
        std::map<StructureEnum::Enum, ModelSurface*> previousSelectedSurfaceController;
    };
    
#ifdef __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__

} // namespace
#endif  //__MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR__H_
