#ifndef __SPHERE_OPEN_G_L__H_
#define __SPHERE_OPEN_G_L__H_

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

    class SphereOpenGL : public CaretObject {
        
    public:
        SphereOpenGL(const float radius);
        
        virtual ~SphereOpenGL();

        void drawWithQuadStrips();

    private:
        SphereOpenGL(const SphereOpenGL&);

        SphereOpenGL& operator=(const SphereOpenGL&);
        
    public:
        virtual AString toString() const;
        
    private:
        std::vector<float> coordinates;
        std::vector<float> normals;
        std::vector<int> vertices;  
        
        std::vector<int> quadStripVerticesStartIndex;
        std::vector<int> quadStripVerticesEndIndex;
    };
    
#ifdef __SPHERE_OPEN_G_L_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPHERE_OPEN_G_L_DECLARE__

} // namespace
#endif  //__SPHERE_OPEN_G_L__H_
