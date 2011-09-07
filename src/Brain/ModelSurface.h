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


#include "ModelBase.h"



namespace caret {

    class Brain;
    class Surface;
    
    /// Controls views of a surface.
    class ModelSurface : public ModelBase {
        
    public:
        ModelSurface(Brain* brain,
                      Surface* surface);
        
        virtual ~ModelSurface();
        
    private:
        ModelSurface(const ModelSurface& o);
        
        ModelSurface& operator=(const ModelSurface& o);
        
    private:
        void copyHelper(const ModelSurface& o);
        
        void initializeMembersModelSurface();
        
    public:
        Surface* getSurface();
        
        AString getNameForGUI(const bool includeStructureFlag) const;
        
        void setDefaultScalingToFitWindow();
        
    private:
        /**Surface that uses this model */
        Surface* surface;
        
    };

} // namespace

#endif // __MODEL_SURFACE_H__
