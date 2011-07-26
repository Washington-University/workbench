#ifndef __MODEL_CONTROLLER_SURFACE_H__
#define __MODEL_CONTROLLER_SURFACE_H__

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


#include "ModelController.h"



namespace caret {

    class Brain;
    class Surface;
    /**
     * Controls views of a surface.
     */
    class ModelControllerSurface : public ModelController {
        
    public:
        ModelControllerSurface(Brain* brain,
                      Surface* surface);
        
        virtual ~ModelControllerSurface();
        
    private:
        ModelControllerSurface(const ModelControllerSurface& o);
        
        ModelControllerSurface& operator=(const ModelControllerSurface& o);
        
    private:
        void copyHelper(const ModelControllerSurface& o);
        
        void initializeMembersModelControllerSurface();
        
    public:
        Surface* getSurface();
        
        std::string getNameForGUI(const bool includeStructureFlag) const;
        
        void setDefaultScalingToFitWindow();
        
    private:
        /**Surface in this controller */
        Surface* surface;
        
    };

} // namespace

#endif // __MODEL_CONTROLLER_SURFACE_H__
