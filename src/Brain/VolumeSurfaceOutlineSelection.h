#ifndef __VOLUME_SURFACE_OUTLINE_SELECTION__H_
#define __VOLUME_SURFACE_OUTLINE_SELECTION__H_

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

namespace caret {

    class Surface;
    class SurfaceSelectionModel;
    class VolumeSurfaceOutlineColorOrTabModel;
    
    class VolumeSurfaceOutlineSelection : public CaretObject {
        
    public:
        VolumeSurfaceOutlineSelection();
        
        virtual ~VolumeSurfaceOutlineSelection();
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);
        
        float getThickness() const;
        
        void setThickness(const float thickness);
        
        SurfaceSelectionModel* getSurfaceSelectionModel();
        
        const Surface* getSurface() const;
        
        Surface* getSurface();
        
        VolumeSurfaceOutlineColorOrTabModel* getColorOrTabModel();
        
        const VolumeSurfaceOutlineColorOrTabModel* getColorOrTabModel() const;
        
    private:
        VolumeSurfaceOutlineSelection(const VolumeSurfaceOutlineSelection&);

        VolumeSurfaceOutlineSelection& operator=(const VolumeSurfaceOutlineSelection&);
        
    public:
        virtual AString toString() const;
        
    private:
        bool displayed;
        float thickness;
        SurfaceSelectionModel* surfaceSelectionModel;
        VolumeSurfaceOutlineColorOrTabModel* colorOrTabModel;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_SELECTION_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_SELECTION__H_
