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
#include "SurfaceTypeEnum.h"

namespace caret {

    class BrainStructure;
    class Surface;
    
    class SurfaceSelection : public CaretObject {
        
    public:
        SurfaceSelection();
        
        SurfaceSelection(const StructureEnum::Enum structure);
        
        SurfaceSelection(BrainStructure* brainStructure);
        
        SurfaceSelection(const SurfaceTypeEnum::Enum surfaceType);
        
        virtual ~SurfaceSelection();
        
        Surface* getSurface();
        
        const Surface* getSurface() const;
        
        void setSurface(Surface* surface);
        
        std::vector<Surface*> getAvailableSurfaces() const;
        
    private:
        enum Mode {
            MODE_BRAIN_STRUCTURE,
            MODE_STRUCTURE,
            MODE_SURFACE_TYPE
        };
        
        SurfaceSelection(const SurfaceSelection&);

        SurfaceSelection& operator=(const SurfaceSelection&);
        
        void updateSelection() const;
        
        mutable Surface* selectedSurface;
        
        BrainStructure* brainStructure;
        
        Mode mode;
        
        /** If empty, allow any structure, otherwise restrict to these structures */
        std::vector<StructureEnum::Enum> allowableStructures;
        
        /** If empty, allow any surface type, otherwise restrict to these types */
        std::vector<SurfaceTypeEnum::Enum> allowableSurfaceTypes;
    };
    
#ifdef __SURFACE_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SURFACE_SELECTION_DECLARE__

} // namespace
#endif  //__SURFACE_SELECTION__H_
