#ifndef __MODEL_SURFACE_H__
#define __MODEL_SURFACE_H__

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

    class Surface;
    
    /// Controls the display of a surface.
    class ModelDisplayControllerSurface : public ModelDisplayController, public EventListenerInterface  {
        
    public:
        ModelDisplayControllerSurface(Brain* brain,
                                      Surface* surface);
        
        virtual ~ModelDisplayControllerSurface();
        
        virtual void resetView(const int32_t windowTabNumber);
        
        void lateralView(const int32_t windowTabNumber);
        
        void medialView(const int32_t windowTabNumber);
        
        virtual void receiveEvent(Event* event);
        
    private:
        ModelDisplayControllerSurface(const ModelDisplayControllerSurface&);
        
        ModelDisplayControllerSurface& operator=(const ModelDisplayControllerSurface&);
        
    private:
        void initializeMembersModelDisplayControllerSurface();
        
    public:
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        virtual AString getNameForBrowserTab() const;
        
        void setDefaultScalingToFitWindow();
        
    private:
        /**Surface that uses this model */
        Surface* surface;
        
    };

} // namespace

#endif // __MODEL_SURFACE_H__
