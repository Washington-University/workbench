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


#include "CaretObject.h"
#include "StructureEnum.h"

namespace caret {
    
    class ModelDisplayController;
    class ModelDisplayControllerSurface;
    
    class ModelDisplayControllerSurfaceSelector : public CaretObject {
        
    public:
        ModelDisplayControllerSurfaceSelector();
        
        virtual ~ModelDisplayControllerSurfaceSelector();
        
        ModelDisplayControllerSurface* getSelectedSurfaceController();
        
        StructureEnum::Enum getSelectedStructure();
        
        void setSelectedSurfaceController(ModelDisplayControllerSurface* surfaceController);
        
        void setSelectedStructure(const StructureEnum::Enum selectedStructure);
        
        void getSelectableStructures(
                        std::vector<StructureEnum::Enum>& selectableSructuresOut) const;
                                     
        void getSelectableSurfaceControllers(
                        std::vector<ModelDisplayControllerSurface*>& selectableSurfaceControllersOut) const;
        
        void updateSelector(const std::vector<ModelDisplayController*> modelDisplayControllers);
        
        AString toString() const;
    private:
        ModelDisplayControllerSurfaceSelector(const ModelDisplayControllerSurfaceSelector&);

        ModelDisplayControllerSurfaceSelector& operator=(const ModelDisplayControllerSurfaceSelector&);
        
        void updateSelector();
        
    private:
        std::vector<StructureEnum::Enum> availableStructures;
        
        StructureEnum::Enum selectedStructure;
        
        StructureEnum::Enum defaultStructure;
        
        std::vector<ModelDisplayControllerSurface*> allSurfaceControllers;
                            
        std::vector<ModelDisplayControllerSurface*> availableSurfaceControllers;
        
        ModelDisplayControllerSurface* selectedSurfaceController;
        
    };
    
#ifdef __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__

} // namespace
#endif  //__MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR__H_
