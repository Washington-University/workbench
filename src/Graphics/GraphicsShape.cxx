
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

#define __GRAPHICS_SHAPE_DECLARE__
#include "GraphicsShape.h"
#undef __GRAPHICS_SHAPE_DECLARE__

#include <cmath>

#include "CaretAssert.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsOpenGLLineDrawing.h"
#include "GraphicsPrimitiveV3f.h"

using namespace caret;


    
/**
 * \class caret::GraphicsShape 
 * \brief Methods for drawing shapes.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsShape::GraphicsShape()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
GraphicsShape::~GraphicsShape()
{
}

/**
 * Draw an outline ellipse.
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 * @param rgba
 *    Color for drawing.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawEllipseOutlineByteColor(void* openglContextPointer,
                                           const double majorAxis,
                                           const double minorAxis,
                                           const uint8_t rgba[4],
                                           const double lineThickness)
{
    std::vector<float> ellipseXYZ;
    createEllipseVertices(majorAxis, minorAxis, ellipseXYZ);
    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::LINE_LOOP,
                                                                                       rgba));
    const int32_t numVertices = static_cast<int32_t>(ellipseXYZ.size() / 3);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&ellipseXYZ[i * 3]);
    }
    
    primitive->setLineWidth(GraphicsPrimitive::SizeType::PIXELS,
                            lineThickness);
    primitive->setUsageType(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsOpenGLLineDrawing::draw(openglContextPointer,
                                    primitive.get());
}

/**
 * Draw a filled ellipse.
 *
 * @param openglContextPointer
 *     Pointer to OpenGL context.
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 * @param rgba
 *    Color for drawing.
 */
void
GraphicsShape::drawEllipseFilledByteColor(void* openglContextPointer,
                                          const double majorAxis,
                                          const double minorAxis,
                                          const uint8_t rgba[4])
{
    std::vector<float> ellipseXYZ;
    createEllipseVertices(majorAxis, minorAxis, ellipseXYZ);

    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::TRIANGLE_FAN,
                                                                                       rgba));
    const float center[3] = { 0.0f, 0.0f, 0.0f };
    primitive->addVertex(center);
    const int32_t numVertices = static_cast<int32_t>(ellipseXYZ.size() / 3);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&ellipseXYZ[i * 3]);
    }
    primitive->addVertex(&ellipseXYZ[0]);
    
    primitive->setUsageType(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(openglContextPointer,
                                   primitive.get());
}

/**
 * Create the vertices for the ellipse
 *
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 * @param elliposeXYZOut
 *    Contains vertices upon exit.
 */
void
GraphicsShape::createEllipseVertices(const double majorAxis,
                                     const double minorAxis,
                                     std::vector<float>& ellipseXYZOut)
{
    const float a = majorAxis / 2.0f;
    const float b = minorAxis / 2.0f;
    
    const float z = 0.0f;
    
    const int32_t numSlices = 10;
    
    const float totalPoints = numSlices * 4;
    ellipseXYZOut.resize(totalPoints * 3);
    
    /*
     * Computation of the points on the ellipse is performed
     * using the parametric equation (a * cos(t), b*sin(t)).
     * Using this parametric equation provides an equal length
     * between points on the ellipse.
     * The symmetry of the four quadrants is used to that
     * we only need to compute points in the first quadrant.
     */
    const float angleStep = ((M_PI / 2.0f) / numSlices);
    
    for (int32_t i = 0; i <= numSlices; i++) {
        const float x = a * std::cos(i * angleStep);
        const float y = b * std::sin(i * angleStep);
        
        const int32_t quadOneIndex = i * 3;
        CaretAssertVectorIndex(ellipseXYZOut, quadOneIndex + 2);
        ellipseXYZOut[quadOneIndex]     = x;
        ellipseXYZOut[quadOneIndex + 1] = y;
        ellipseXYZOut[quadOneIndex + 2] = z;
        
        if (i < numSlices) {
            const int32_t quadTwoIndex = ((numSlices * 2) - i) * 3;
            CaretAssertVectorIndex(ellipseXYZOut, quadTwoIndex + 2);
            ellipseXYZOut[quadTwoIndex]     = -x;
            ellipseXYZOut[quadTwoIndex + 1] = y;
            ellipseXYZOut[quadTwoIndex + 2] = z;
        }
        
        if (i > 0) {
            const int32_t quadThreeIndex = ((numSlices * 2) + i) * 3;
            CaretAssertVectorIndex(ellipseXYZOut, quadThreeIndex + 2);
            ellipseXYZOut[quadThreeIndex]     = -x;
            ellipseXYZOut[quadThreeIndex + 1] = -y;
            ellipseXYZOut[quadThreeIndex + 2] = z;
        }
        
        if ((i > 0) && (i < numSlices)) {
            const int32_t quadFourIndex = ((numSlices * 4) - i) * 3;
            CaretAssertVectorIndex(ellipseXYZOut, quadFourIndex + 2);
            ellipseXYZOut[quadFourIndex]     = x;
            ellipseXYZOut[quadFourIndex + 1] = -y;
            ellipseXYZOut[quadFourIndex + 2] = z;
        }
    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
GraphicsShape::toString() const
{
    return "GraphicsShape";
}

