
#ifndef __BRAIN_STRUCTURE_H__
#define __BRAIN_STRUCTURE_H__

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

#include <vector>

#include <stdint.h>

#include "CaretObject.h"

namespace caret {
    
    class Brain;
    class Surface;
    
    /**
     * Maintains view of some type of object.
     */
    class BrainStructure : public CaretObject {
        
    public:
        BrainStructure(Brain* brain);
        
        ~BrainStructure();
        
    private:
        BrainStructure(const BrainStructure& s);
        
        BrainStructure& operator=(const BrainStructure& s);

    public:
        bool addSurface(Surface* s);
        
        int getNumberOfSurfaces() const;
        
        Surface* getSurface(const int32_t indx);
        
        Brain* getBrain();
        
        int32_t getNumberOfNodes() const;
        
        const float* getNodeColor(const int32_t nodeIndex) const;
        
    private:
        Brain* brain;
        
        std::vector<Surface*> surfaces;
        
        std::vector<float> nodeColoring;
    };
    
} // namespace

#endif // __BRAIN_STRUCTURE_H__
