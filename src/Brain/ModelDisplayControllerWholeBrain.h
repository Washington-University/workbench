#ifndef __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__
#define __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include "BrainConstants.h"
#include "ModelDisplayController.h"
#include "SurfaceTypeEnum.h"


namespace caret {

    class Brain;
    
    /// Controls the display of a whole brain.
    class ModelDisplayControllerWholeBrain : public ModelDisplayController {
        
    public:
        ModelDisplayControllerWholeBrain(Brain* brain);
        
        virtual ~ModelDisplayControllerWholeBrain();
        
        Brain* getBrain();
        
        void getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum> surfaceTypesOut);
        
        SurfaceTypeEnum::Enum getSelectedSurfaceType(const int32_t windowTabNumber);
        
        void setSelectedSurfaceType(const int32_t windowTabNumber,
                                    const SurfaceTypeEnum::Enum surfaceType);
        
    private:
        ModelDisplayControllerWholeBrain(const ModelDisplayControllerWholeBrain&);
        
        ModelDisplayControllerWholeBrain& operator=(const ModelDisplayControllerWholeBrain&);
        
    private:
        void initializeMembersModelDisplayControllerWholeBrain();
        
        void updateController();
        
    public:
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
    private:
        /** Brain to which this controller belongs */
        Brain* brain;
        
        /** Type of surface for display */
        mutable SurfaceTypeEnum::Enum selectedSurfaceType[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Available surface types */
        std::vector<SurfaceTypeEnum::Enum> availableSurfaceTypes;
    };

} // namespace

#endif // __MODEL_DISPLAY_CONTROLLER_WHOLE_BRAIN_H__
