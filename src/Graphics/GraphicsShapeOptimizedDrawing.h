#ifndef __GRAPHICS_SHAPE_OPTIMIZED_DRAWING_H__
#define __GRAPHICS_SHAPE_OPTIMIZED_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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


#include <map>
#include <memory>

#include "CaretObject.h"
#include "RgbaUnsignedByte.h"

namespace caret {

    class GraphicsPrimitiveV3fN3f;
    class RgbaUnsignedByte;
    class Vector3D;
    
    class GraphicsShapeOptimizedDrawing : public CaretObject {
        
    public:
        static void drawOctahedron(const float diameter,
                                   const RgbaUnsignedByte& rgba);
        
        static void drawOctahedron(const Vector3D& xyz,
                                   const float diameter,
                                   const RgbaUnsignedByte& rgba);

        static void drawSphere(const float diameter,
                               const RgbaUnsignedByte& rgba);
        
        static void drawSphere(const Vector3D& xyz,
                               const float diameter,
                               const RgbaUnsignedByte& rgba);
        
        virtual ~GraphicsShapeOptimizedDrawing();
        
        GraphicsShapeOptimizedDrawing(const GraphicsShapeOptimizedDrawing&) = delete;

        GraphicsShapeOptimizedDrawing& operator=(const GraphicsShapeOptimizedDrawing&) = delete;
        
        // ADD_NEW_METHODS_HERE

    private:
        struct SphereTriangleStrips {
            std::vector<float> coordinates;  // x,y,z per vertex
            std::vector<float> normals;      // x,y,z per vertex
            int numberOfStrips = 0;
            int verticesPerStrip = 0;        // only valid when not joined
            int totalVertices = 0;
        };

        struct OctahedronStrips {
            std::vector<float> coordinates;   // x,y,z per vertex
            std::vector<float> normals;       // x,y,z per vertex (smooth, radial)
            std::vector<int>   stripFirst;    // first vertex of each strip
            std::vector<int>   stripCount;    // number of vertices in each strip
            int totalVertices = 0;
        };

        GraphicsShapeOptimizedDrawing();
        
        GraphicsPrimitiveV3fN3f* getOctahedronPrimitiveForRGBA(const RgbaUnsignedByte& rgba);
        
        GraphicsPrimitiveV3fN3f* getSpherePrimitiveForRGBA(const RgbaUnsignedByte& rgba);
        
        OctahedronStrips* createOctahedronTriangleStrips(const bool joinWithDegenerates);
        
        SphereTriangleStrips* createSphereTriangleStrips(const int numberOfSlices,
                                                         const int numberOfStacks,
                                                         const bool joinWithDegenerates);
        
        void drawOctahedronImplementation(const float diameter,
                                          const RgbaUnsignedByte& rgba);
        
        void drawOctahedronImplementation(const Vector3D& xyz,
                                          const float diameter,
                                          const RgbaUnsignedByte& rgba);

        void drawSphereImplementation(const float diameter,
                                      const RgbaUnsignedByte& rgba);
        
        void drawSphereImplementation(const Vector3D& xyz,
                                      const float diameter,
                                      const RgbaUnsignedByte& rgba);
        
        static GraphicsShapeOptimizedDrawing* getInstance();
        
        /**
         * Key is a 32-bit unsigned integer containing the RGBA byte coloring.
         * Comparison will likely be faster than comparing 4 bytes.
         */
        std::map<uint32_t, GraphicsPrimitiveV3fN3f*> m_octahedronPrimitives;
        
        /**
         * Key is a 32-bit unsigned integer containing the RGBA byte coloring.
         * Comparison will likely be faster than comparing 4 bytes.
         */
        std::map<uint32_t, GraphicsPrimitiveV3fN3f*> m_spherePrimitives;
        
        std::unique_ptr<SphereTriangleStrips> m_sphereTriangleStripInfo;
        
        std::unique_ptr<OctahedronStrips> m_octahedronTriangleStripInfo;
        
        static GraphicsShapeOptimizedDrawing* s_singletonInstance;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_SHAPE_OPTIMIZED_DRAWING_DECLARE__
    GraphicsShapeOptimizedDrawing* GraphicsShapeOptimizedDrawing::s_singletonInstance = NULL;
#endif // __GRAPHICS_SHAPE_OPTIMIZED_DRAWING_DECLARE__

} // namespace
#endif  //__GRAPHICS_SHAPE_OPTIMIZED_DRAWING_H__
