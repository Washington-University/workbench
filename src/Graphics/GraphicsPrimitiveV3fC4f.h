#ifndef __GRAPHICS_PRIMITIVE_V3F_C4F_H__
#define __GRAPHICS_PRIMITIVE_V3F_C4F_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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



#include <memory>

#include "GraphicsPrimitive.h"



namespace caret {

    class GraphicsPrimitiveV3fC4f : public GraphicsPrimitive {
        
    public:
        GraphicsPrimitiveV3fC4f(const PrimitiveType primitiveType);
        
        virtual ~GraphicsPrimitiveV3fC4f();
        
        GraphicsPrimitiveV3fC4f(const GraphicsPrimitiveV3fC4f& obj);

        void addVertex(const float xyz[3],
                       const float rgba[4]);

        void addVertex(const float x,
                       const float y,
                       const float z,
                       const float rgba[4]);
        
        virtual GraphicsPrimitive* clone() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        GraphicsPrimitiveV3fC4f& operator=(const GraphicsPrimitiveV3fC4f& obj);
        
        void copyHelperGraphicsPrimitiveV3fC4f(const GraphicsPrimitiveV3fC4f& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_V3F_C4F_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_V3F_C4F_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_V3F_C4F_H__
