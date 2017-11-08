#ifndef __GRAPHICS_SHAPE_H__
#define __GRAPHICS_SHAPE_H__

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

#include "CaretObject.h"
#include "GraphicsPrimitive.h"



namespace caret {

    class GraphicsShape : public CaretObject {
        
    public:
        virtual ~GraphicsShape();
        
        static void drawBoxOutlineByteColor(void* openglContextPointer,
                                            const float v1[3],
                                            const float v2[3],
                                            const float v3[3],
                                            const float v4[3],
                                            const uint8_t rgba[4],
                                            const GraphicsPrimitive::SizeType lineThicknessType,
                                            const double lineThickness);
        
        static void drawBoxFilledByteColor(void* openglContextPointer,
                                           const float v1[3],
                                           const float v2[3],
                                           const float v3[3],
                                           const float v4[3],
                                           const uint8_t rgba[4]);
        
        static void drawEllipseOutlineByteColor(void* openglContextPointer,
                                                const double majorAxis,
                                                const double minorAxis,
                                                const uint8_t rgba[4],
                                                const GraphicsPrimitive::SizeType lineThicknessType,
                                                const double lineThickness);
        
        static void drawEllipseFilledByteColor(void* openglContextPointer,
                                               const double majorAxis,
                                               const double minorAxis,
                                               const uint8_t rgba[4]);
        
        static void drawLinesByteColor(void* openglContextPointer,
                                       const std::vector<float>& xyz,
                                       const uint8_t rgba[4],
                                       const GraphicsPrimitive::SizeType lineThicknessType,
                                       const double lineThickness);
        
        static void drawLineStripByteColor(void* openglContextPointer,
                                           const std::vector<float>& xyz,
                                           const uint8_t rgba[4],
                                           const GraphicsPrimitive::SizeType lineThicknessType,
                                           const double lineThickness);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        GraphicsShape();
        
        GraphicsShape(const GraphicsShape&);

        GraphicsShape& operator=(const GraphicsShape&);
        
        static void createEllipseVertices(const double majorAxis,
                                          const double minorAxis,
                                          std::vector<float>& ellipseXYZOut);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_SHAPE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_SHAPE_DECLARE__

} // namespace
#endif  //__GRAPHICS_SHAPE_H__
