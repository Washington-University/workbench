
#ifndef __SURFACE_H__
#define __SURFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include <vector>

#include "SurfaceFile.h"

namespace caret {
    
    class BoundingBox;
    class BrainStructure;
    
    /**
     * Maintains view of some type of object.
     */
    class Surface : public SurfaceFile {
        
    public:
        Surface();
        
        ~Surface();

        Surface(const Surface& s);
        
        Surface& operator=(const Surface& s);
        
        AString getNameForGUI(bool includeStructureFlag) const;
        
        void getBounds(BoundingBox& boundingBoxOut) const;
        
        const BrainStructure* getBrainStructure() const;
        
        BrainStructure* getBrainStructure();
        
        void setBrainStructure(BrainStructure* brainStructure);
        
    private:
        void initializeMemberSurface();
        
        void copyHelperSurface(const Surface& s);

        BrainStructure* brainStructure;
    };

} // namespace

#endif // __SURFACE_H__
