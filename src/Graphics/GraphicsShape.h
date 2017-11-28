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


#include <map>
#include <memory>

#include "CaretObject.h"
#include "GraphicsPrimitive.h"



namespace caret {

    class GraphicsPrimitiveV3f;
    class GraphicsPrimitiveV3fN3f;
    
    class GraphicsShape : public CaretObject {
        
    public:
        virtual ~GraphicsShape();
        
        static void drawBoxOutlineByteColor(const float v1[3],
                                            const float v2[3],
                                            const float v3[3],
                                            const float v4[3],
                                            const uint8_t rgba[4],
                                            const GraphicsPrimitive::SizeType lineThicknessType,
                                            const double lineThickness);
        
        static void drawBoxOutlineFloatColor(const float v1[3],
                                             const float v2[3],
                                             const float v3[3],
                                             const float v4[3],
                                             const float rgba[4],
                                             const GraphicsPrimitive::SizeType lineThicknessType,
                                             const double lineThickness);
        
        static void drawBoxFilledByteColor(const float v1[3],
                                           const float v2[3],
                                           const float v3[3],
                                           const float v4[3],
                                           const uint8_t rgba[4]);
        
        static void drawBoxFilledFloatColor(const float v1[3],
                                            const float v2[3],
                                            const float v3[3],
                                            const float v4[3],
                                            const float rgba[4]);
        
        static void drawEllipseOutlineByteColor(const double majorAxis,
                                                const double minorAxis,
                                                const uint8_t rgba[4],
                                                const GraphicsPrimitive::SizeType lineThicknessType,
                                                const double lineThickness);
        
        static void drawEllipseFilledByteColor(const double majorAxis,
                                               const double minorAxis,
                                               const uint8_t rgba[4]);
        
        static void drawLinesByteColor(const std::vector<float>& xyz,
                                       const uint8_t rgba[4],
                                       const GraphicsPrimitive::SizeType lineThicknessType,
                                       const double lineThickness);
        
        static void drawLineStripBevelJoinByteColor(const std::vector<float>& xyz,
                                                    const uint8_t rgba[4],
                                                    const GraphicsPrimitive::SizeType lineThicknessType,
                                                    const double lineThickness);
        
        static void drawLineStripMiterJoinByteColor(const std::vector<float>& xyz,
                                                    const uint8_t rgba[4],
                                                    const GraphicsPrimitive::SizeType lineThicknessType,
                                                    const double lineThickness);
        
        static void drawSphereByteColor(const float xyz[3],
                                        const uint8_t rgba[4],
                                        const float diameter);
        
        static void drawCircleFilled(const float xyz[3],
                                     const uint8_t rgba[4],
                                     const float diameter);
        
        static void drawSquare(const float xyz[3],
                               const uint8_t rgba[4],
                               const float diameter);
        
        static void drawRing(const float xyz[3],
                             const uint8_t rgba[4],
                             const double innerRadius,
                             const double outerRadius);
        
        static void deleteAllPrimitives();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class RingKey {
        public:
            RingKey(const int32_t numberOfDivisions,
                    const float innerRadius,
                    const float outerRadius)
            : m_numberOfDivisions(numberOfDivisions),
            m_innerRadius(innerRadius),
            m_outerRadius(outerRadius) { }
            
            bool matches(const int32_t numberOfDivisions,
                         const float innerRadius,
                         const float outerRadius) const {
                if ((m_numberOfDivisions == numberOfDivisions)
                    && (m_innerRadius == innerRadius)
                    && (m_outerRadius == outerRadius)) {
                    return true;
                }
                return false;
            }
            
            bool operator<(const RingKey& rhs) const {
                if (m_numberOfDivisions < rhs.m_numberOfDivisions) {
                    return true;
                }
                if (m_innerRadius < rhs.m_innerRadius) {
                    return true;
                }
                if (m_outerRadius < rhs.m_outerRadius) {
                    return true;
                }
                return false;
            }
            
            int32_t m_numberOfDivisions;
            float m_innerRadius;
            float m_outerRadius;
        };
        
        GraphicsShape();
        
        GraphicsShape(const GraphicsShape&);

        GraphicsShape& operator=(const GraphicsShape&);
        
        static void updateModelMatrixToFaceViewer();
        
        static GraphicsPrimitive* createCirclePrimitive(const int32_t numberOfDivisions,
                                                        const double radius);
        
        static GraphicsPrimitive* createRingPrimitive(const RingKey& ringKey);
        
        static void createEllipseVertices(const double majorAxis,
                                          const double minorAxis,
                                          std::vector<float>& ellipseXYZOut);
        
        
        static GraphicsPrimitiveV3fN3f* createSpherePrimitiveTriangles(const int32_t numberOfLatLon);
        
        static GraphicsPrimitiveV3fN3f* createSpherePrimitiveTriangleStrips(const int32_t numberOfLatLon);
        
        static void createSphereXYZ(const float radius,
                                    const float latDegrees,
                                    const float lonDegrees,
                                    const int32_t latIndex,
                                    const int32_t numLat,
                                    float xyzOut[3],
                                    float normalXyzOut[3]);
        
        static std::unique_ptr<GraphicsPrimitiveV3f> s_byteSquarePrimitive;
        
        static std::map<int32_t, GraphicsPrimitive*> s_byteSpherePrimitives;
        
        static std::map<int32_t, GraphicsPrimitive*> s_byteCirclePrimitives;
        
        
        static std::map<RingKey, GraphicsPrimitive*> s_byteRingPrimitives;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_SHAPE_DECLARE__
    std::map<int32_t, GraphicsPrimitive*> GraphicsShape::s_byteSpherePrimitives;
    std::map<int32_t, GraphicsPrimitive*> GraphicsShape::s_byteCirclePrimitives;
    std::map<GraphicsShape::RingKey, GraphicsPrimitive*> GraphicsShape::s_byteRingPrimitives;
    std::unique_ptr<GraphicsPrimitiveV3f> GraphicsShape::s_byteSquarePrimitive;
#endif // __GRAPHICS_SHAPE_DECLARE__

} // namespace
#endif  //__GRAPHICS_SHAPE_H__
