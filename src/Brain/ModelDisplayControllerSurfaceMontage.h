#ifndef __MODEL_SURFACE_MONTAGE_H__
#define __MODEL_SURFACE_MONTAGE_H__

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


#include "EventListenerInterface.h"
#include "ModelDisplayController.h"

namespace caret {

    class SurfaceSelection;
    
    /// Controls the display of a surface montage
    class ModelDisplayControllerSurfaceMontage : public ModelDisplayController, public EventListenerInterface  {
        
    public:
        ModelDisplayControllerSurfaceMontage(Brain* brain);
        
        virtual ~ModelDisplayControllerSurfaceMontage();
        
        virtual void resetView(const int32_t windowTabNumber);
        
        virtual void receiveEvent(Event* event);
        
        OverlaySet* getOverlaySet(const int tabIndex);
        
        const OverlaySet* getOverlaySet(const int tabIndex) const;
        
        void initializeOverlays();
        
        SurfaceSelection* getLeftSurfaceSelection();
        
        SurfaceSelection* getLeftSecondSurfaceSelection();
        
        SurfaceSelection* getRightSurfaceSelection();
        
        SurfaceSelection* getRightSecondSurfaceSelection();
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        void setDefaultScalingToFitWindow();
        
        bool isDualConfigurationEnabled() const;
        
        void setDualConfigurationEnabled(const bool enabled);
        
    private:
        ModelDisplayControllerSurfaceMontage(const ModelDisplayControllerSurfaceMontage&);
        
        ModelDisplayControllerSurfaceMontage& operator=(const ModelDisplayControllerSurfaceMontage&);
        
        void initializeMembersModelDisplayControllerSurfaceMontage();

        SurfaceSelection* leftSurfaceSelection;
        
        SurfaceSelection* leftSecondSurfaceSelection;
        
        SurfaceSelection* rightSurfaceSelection;
        
        SurfaceSelection* rightSecondSurfaceSelection;
        
        bool dualConfigurationEnabled;
    };

} // namespace

#endif // __MODEL_SURFACE_MONTAGE_H__
