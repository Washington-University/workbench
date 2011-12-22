#ifndef __SURFACE_SELECTION__H_
#define __SURFACE_SELECTION__H_

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

    class Surface;
    
    class SurfaceSelection : public CaretObject {
        
    public:
        SurfaceSelection();
        
        SurfaceSelection(const StructureEnum::Enum structure);
        
        virtual ~SurfaceSelection();
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        std::vector<Surface*> getAvailableSurfaces() const;
        
    private:
        SurfaceSelection(const SurfaceSelection&);

        SurfaceSelection& operator=(const SurfaceSelection&);
        
        void updateSelection() const;
        
        mutable StructureEnum::Enum selectedStructure;
        
        mutable Surface* selectedSurface;
        
        /** If empty, allow any structure, otherwise restrict to these structures */
        std::vector<StructureEnum::Enum> allowableStructures;
    };
    
#ifdef __SURFACE_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_SELECTION_DECLARE__

} // namespace
#endif  //__SURFACE_SELECTION__H_
