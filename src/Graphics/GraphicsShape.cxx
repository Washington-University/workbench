
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
#include "CaretLogger.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitive.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsPrimitiveV3fN3f.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"

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
 * Delete all primitives.
 */
void
GraphicsShape::deleteAllPrimitives()
{
    for (auto iter : s_byteSpherePrimitives) {
        delete iter.second;
    }
    s_byteSpherePrimitives.clear();
    
    for (auto iter : s_byteCirclePrimitives) {
        delete iter.second;
    }
    s_byteCirclePrimitives.clear();

    for (auto iter : s_byteRingPrimitives) {
        delete iter.second;
    }
    s_byteRingPrimitives.clear();
    
    /* 
     * Need to delete here or it will cause a crash
     * as there is no EventManager when static data 
     * is deleted.
     */
    s_byteSquarePrimitive.reset();
}


/**
 * Draw an outline box.
 *
 * @param v1
 *    First vertex.
 * @param v2
 *    Second vertex.
 * @param v3
 *    Third vertex.
 * @param v4
 *    Fourth vertex.
 * @param rgba
 *    Color for drawing.
 * @param lineThicknessType
 *    Type of line thickness.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawBoxOutlineByteColor(const float v1[3],
                                    const float v2[3],
                                    const float v3[3],
                                    const float v4[3],
                                    const uint8_t rgba[4],
                                    const GraphicsPrimitive::SizeType lineThicknessType,
                                    const double lineThickness)
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN,
                                                                                       rgba));
    primitive->reserveForNumberOfVertices(4);
    primitive->addVertex(v1);
    primitive->addVertex(v2);
    primitive->addVertex(v3);
    primitive->addVertex(v4);
    
    primitive->setLineWidth(lineThicknessType,
                            lineThickness);
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw an outline box.
 *
 * @param v1
 *    First vertex.
 * @param v2
 *    Second vertex.
 * @param v3
 *    Third vertex.
 * @param v4
 *    Fourth vertex.
 * @param rgba
 *    Color for drawing.
 * @param lineThicknessType
 *    Type of line thickness.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawBoxOutlineFloatColor(const float v1[3],
                                        const float v2[3],
                                        const float v3[3],
                                        const float v4[3],
                                        const float rgba[4],
                                        const GraphicsPrimitive::SizeType lineThicknessType,
                                        const double lineThickness)
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN,
                                                                                       rgba));
    primitive->reserveForNumberOfVertices(4);
    primitive->addVertex(v1);
    primitive->addVertex(v2);
    primitive->addVertex(v3);
    primitive->addVertex(v4);
    
    primitive->setLineWidth(lineThicknessType,
                            lineThickness);
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw a filled box.  Vertices must be in a counter-clockwise order.
 *
 * @param v1
 *    First vertex.
 * @param v2
 *    Second vertex.
 * @param v3
 *    Third vertex.
 * @param v4
 *    Fourth vertex.
 * @param rgba
 *    Color for drawing.
 */
void
GraphicsShape::drawBoxFilledByteColor(const float v1[3],
                                   const float v2[3],
                                   const float v3[3],
                                   const float v4[3],
                                   const uint8_t rgba[4])
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES,
                                                                                       rgba));
    primitive->reserveForNumberOfVertices(6);
    primitive->addVertex(v1);
    primitive->addVertex(v2);
    primitive->addVertex(v3);
    primitive->addVertex(v3);
    primitive->addVertex(v4);
    primitive->addVertex(v1);
    
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw a filled box.  Vertices must be in a counter-clockwise order.
 *
 * @param v1
 *    First vertex.
 * @param v2
 *    Second vertex.
 * @param v3
 *    Third vertex.
 * @param v4
 *    Fourth vertex.
 * @param rgba
 *    Color for drawing.
 */
void
GraphicsShape::drawBoxFilledFloatColor(const float v1[3],
                                       const float v2[3],
                                       const float v3[3],
                                       const float v4[3],
                                       const float rgba[4])
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES,
                                                                                       rgba));
    primitive->reserveForNumberOfVertices(6);
    primitive->addVertex(v1);
    primitive->addVertex(v2);
    primitive->addVertex(v3);
    primitive->addVertex(v3);
    primitive->addVertex(v4);
    primitive->addVertex(v1);
    
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}


/**
 * Draw an outline ellipse.
 *
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 * @param rgba
 *    Color for drawing.
 * @param lineThicknessType
 *    Type of line thickness.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawEllipseOutlineByteColor(const double majorAxis,
                                           const double minorAxis,
                                           const uint8_t rgba[4],
                                           const GraphicsPrimitive::SizeType lineThicknessType,
                                           const double lineThickness)
{
    std::vector<float> ellipseXYZ;
    createEllipseVertices(majorAxis, minorAxis, ellipseXYZ);
    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN,
                                                                                       rgba));
    const int32_t numVertices = static_cast<int32_t>(ellipseXYZ.size() / 3);
    primitive->reserveForNumberOfVertices(numVertices);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&ellipseXYZ[i * 3]);
    }
    
    primitive->setLineWidth(lineThicknessType,
                            lineThickness);
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw a filled ellipse.
 *
 * @param majorAxis
 *    Diameter of the major axis.
 * @param minorAxis
 *    Diameter of the minor axis.
 * @param rgba
 *    Color for drawing.
 */
void
GraphicsShape::drawEllipseFilledByteColor(const double majorAxis,
                                          const double minorAxis,
                                          const uint8_t rgba[4])
{
    std::vector<float> ellipseXYZ;
    createEllipseVertices(majorAxis, minorAxis, ellipseXYZ);
    const int32_t numVertices = static_cast<int32_t>(ellipseXYZ.size() / 3);

    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN,
                                                                                       rgba));
    primitive->reserveForNumberOfVertices(numVertices + 1);
    const float center[3] = { 0.0f, 0.0f, 0.0f };
    primitive->addVertex(center);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&ellipseXYZ[i * 3]);
    }
    primitive->addVertex(&ellipseXYZ[0]);
    
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw lines: Each pair of vertices is a separate line segment
 * similar to OpenGL's GL_LINES
 *
 * @param xyz
 *    Coordinates of the line end points.
 * @param rgba
 *    Color for drawing.
 * @param lineThicknessType
 *    Type of line thickness.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawLinesByteColor(const std::vector<float>& xyz,
                                  const uint8_t rgba[4],
                                  const GraphicsPrimitive::SizeType lineThicknessType,
                                  const double lineThickness)
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES,
                                                                                       rgba));
    const int32_t numVertices = static_cast<int32_t>(xyz.size() / 3);
    primitive->reserveForNumberOfVertices(numVertices);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&xyz[i * 3]);
    }
    
    primitive->setLineWidth(lineThicknessType,
                            lineThickness);
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw line strip similar to OpenGL's GL_LINE_STRIP
 * and use Bevel Joins at connected vertices
 *
 * @param xyz
 *    Coordinates of the line end points.
 * @param rgba
 *    Color for drawing.
 * @param lineThicknessType
 *    Type of line thickness.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawLineStripBevelJoinByteColor(const std::vector<float>& xyz,
                                               const uint8_t rgba[4],
                                               const GraphicsPrimitive::SizeType lineThicknessType,
                                               const double lineThickness)
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN,
                                                                                       rgba));
    const int32_t numVertices = static_cast<int32_t>(xyz.size() / 3);
    primitive->reserveForNumberOfVertices(numVertices);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&xyz[i * 3]);
    }
    
    primitive->setLineWidth(lineThicknessType,
                            lineThickness);
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw line strip similar to OpenGL's GL_LINE_STRIP
 * and use Bevel Joins at connected vertices
 *
 * @param xyz
 *    Coordinates of the line end points.
 * @param rgba
 *    Color for drawing.
 * @param lineThicknessType
 *    Type of line thickness.
 * @param lineThickness
 *    Thickness of the line.
 */
void
GraphicsShape::drawLineStripMiterJoinByteColor(const std::vector<float>& xyz,
                                               const uint8_t rgba[4],
                                               const GraphicsPrimitive::SizeType lineThicknessType,
                                               const double lineThickness)
{
    std::unique_ptr<GraphicsPrimitiveV3f> primitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_MITER_JOIN,
                                                                                       rgba));
    const int32_t numVertices = static_cast<int32_t>(xyz.size() / 3);
    primitive->reserveForNumberOfVertices(numVertices);
    for (int32_t i = 0; i < numVertices; i++) {
        primitive->addVertex(&xyz[i * 3]);
    }
    
    primitive->setLineWidth(lineThicknessType,
                            lineThickness);
    primitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES);
    
    GraphicsEngineDataOpenGL::draw(primitive.get());
}

/**
 * Draw a sphere at the given XYZ coordinate
 *
 * @param xyz
 *     XYZ-coordinate of sphere
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the sphere.
 */
void
GraphicsShape::drawSphereByteColor(const float xyz[3],
                                   const uint8_t rgba[4],
                                   const float diameter)
{
    const int32_t numLatLonDivisions = 10;
    
    GraphicsPrimitive* spherePrimitive = NULL;
    for (const auto iter : s_byteSpherePrimitives) {
        const auto& key = iter.first;
        if ((key  == numLatLonDivisions)) {
            spherePrimitive = iter.second;
            CaretAssert(spherePrimitive);
            break;
        }
    }
    
    if (spherePrimitive == NULL) {
        const bool useStripsFlag = true;
        if (useStripsFlag) {
            spherePrimitive = createSpherePrimitiveTriangleStrips(numLatLonDivisions);
        }
        else {
            spherePrimitive = createSpherePrimitiveTriangles(numLatLonDivisions);
        }
        /* colors may change but not coordinates/normals */
        spherePrimitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_MANY_TIMES);
        spherePrimitive->setUsageTypeColors(GraphicsPrimitive::UsageType::MODIFIED_MANY_DRAWN_MANY_TIMES);
        s_byteSpherePrimitives.insert(std::make_pair(numLatLonDivisions,
                                                     spherePrimitive));
    }

    CaretAssert(spherePrimitive);
    
    spherePrimitive->replaceAllVertexSolidByteRGBA(rgba);
    
    glPushMatrix();
    glTranslatef(xyz[0], xyz[1], xyz[2]);
    glScalef(diameter, diameter, diameter);
    GraphicsEngineDataOpenGL::draw(spherePrimitive);
    glPopMatrix();
}

/**
 * Draw a filled circle at the given XYZ coordinate
 *
 * @param xyz
 *     XYZ-coordinate of circle
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the circle.
 */
void
GraphicsShape::drawCircleFilled(const float xyz[3],
                             const uint8_t rgba[4],
                             const float diameter)
{
    const int32_t numberOfDivisions = 20;
    
    GraphicsPrimitive* circlePrimitive = NULL;
    for (const auto keyPrim : s_byteCirclePrimitives) {
        if (keyPrim.first == numberOfDivisions) {
            circlePrimitive = keyPrim.second;
            break;
        }
    }
    
    if (circlePrimitive == NULL) {
        const float radius = 0.5f;
        circlePrimitive = createCirclePrimitive(numberOfDivisions,
                                                radius);
        circlePrimitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_MANY_TIMES);
        circlePrimitive->setUsageTypeColors(GraphicsPrimitive::UsageType::MODIFIED_MANY_DRAWN_MANY_TIMES);
        s_byteCirclePrimitives.insert(std::make_pair(numberOfDivisions,
                                                     circlePrimitive));
    }
    
    CaretAssert(circlePrimitive);
    
    circlePrimitive->replaceAllVertexSolidByteRGBA(rgba);
    
    glPushMatrix();
    if (xyz != NULL) {
        glTranslatef(xyz[0], xyz[1], xyz[2]);
    }
    glScalef(diameter, diameter, 1.0f);
    GraphicsEngineDataOpenGL::draw(circlePrimitive);
    glPopMatrix();
}

/**
 * Draw a square at the given XYZ coordinate
 *
 * @param xyz
 *     XYZ-coordinate of square
 * @param rgba
 *    Color for drawing.
 * @param diameter
 *    Diameter of the square.
 */

void
GraphicsShape::drawSquare(const float xyz[3],
                          const uint8_t rgba[4],
                          const float diameter)
{
    if ( ! s_byteSquarePrimitive) {
        s_byteSquarePrimitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES,
                                                                       rgba));
        /*
         * The square is made up of four triangles
         * with two triangles for the 'front' and
         * two triangles for the 'back' so that it is
         * never culled.
         */
        
        const float radius = 0.5f;
        
        /* counter clockwise triangle */
        s_byteSquarePrimitive->addVertex(-radius, -radius);
        s_byteSquarePrimitive->addVertex( radius, -radius);
        s_byteSquarePrimitive->addVertex( radius,  radius);
        
        /* counter clockwise triangle */
        s_byteSquarePrimitive->addVertex(-radius, -radius);
        s_byteSquarePrimitive->addVertex( radius,  radius);
        s_byteSquarePrimitive->addVertex(-radius,  radius);
        
        /* clockwise triangle */
        s_byteSquarePrimitive->addVertex(-radius, -radius);
        s_byteSquarePrimitive->addVertex(-radius,  radius);
        s_byteSquarePrimitive->addVertex( radius,  radius);
        
        /* clockwise triangle */
        s_byteSquarePrimitive->addVertex(-radius, -radius);
        s_byteSquarePrimitive->addVertex( radius,  radius);
        s_byteSquarePrimitive->addVertex( radius, -radius);
    }
    
    CaretAssert(s_byteSquarePrimitive.get());
    
    s_byteSquarePrimitive->replaceAllVertexSolidByteRGBA(rgba);
    
    glPushMatrix();
    if (xyz != NULL) {
        glTranslatef(xyz[0], xyz[1], xyz[2]);
        updateModelMatrixToFaceViewer();
        glScalef(diameter, diameter, 1.0f);
    }
    GraphicsEngineDataOpenGL::draw(s_byteSquarePrimitive.get());
    glPopMatrix();
}

/**
 * Draw a ring at the given XYZ coordinate
 *
 * @param xyz
 *     XYZ-coordinate of ring
 * @param rgba
 *    Color for drawing.
 * @param innerRadius
 *    Inner adius of the ring.
 * @param outerRadius
 *    Outer adius of the ring.
 */
void
GraphicsShape::drawRing(const float xyz[3],
                        const uint8_t rgba[4],
                        const double innerRadius,
                        const double outerRadius)
{
    const int32_t numberOfDivisions = 20;
    
    GraphicsPrimitive* ringPrimitive = NULL;
    for (const auto keyPrim : s_byteRingPrimitives) {
        if (keyPrim.first.matches(numberOfDivisions,
                                  innerRadius,
                                  outerRadius)) {
            ringPrimitive = keyPrim.second;
            break;
        }
    }
    
    if (ringPrimitive == NULL) {
        RingKey ringKey(numberOfDivisions, innerRadius, outerRadius);
        ringPrimitive = createRingPrimitive(ringKey);
        ringPrimitive->setUsageTypeAll(GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_MANY_TIMES);
        ringPrimitive->setUsageTypeColors(GraphicsPrimitive::UsageType::MODIFIED_MANY_DRAWN_MANY_TIMES);
        s_byteRingPrimitives.insert(std::make_pair(RingKey(numberOfDivisions, innerRadius, outerRadius),
                                                   ringPrimitive));
    }
    
    CaretAssert(ringPrimitive);
    
    ringPrimitive->replaceAllVertexSolidByteRGBA(rgba);
    
    glPushMatrix();
    if (xyz != NULL) {
        glTranslatef(xyz[0], xyz[1], xyz[2]);
    }
    GraphicsEngineDataOpenGL::draw(ringPrimitive);
    glPopMatrix();
}

/**
 * Create the primitive for a circle.
 *
 * @param numberOfDivisions
 *    Number of divisions
 * @param radius
 *    Radius of the circle.
 */
GraphicsPrimitive*
GraphicsShape::createCirclePrimitive(const int32_t numberOfDivisions,
                                     const double radius)
{
    /*
     * Setup step size based upon number of points around circle
     */
    //    const int32_t numberOfPoints = 8;
    const float step = (2.0 * M_PI) / numberOfDivisions;
    
    GraphicsPrimitiveV3f* primitive = GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN,
                                                                         (uint8_t[]) { 255, 255, 255, 255 });
    primitive->reserveForNumberOfVertices(numberOfDivisions + 2);
    /*
     * Generate points around ring
     */
    const float z = 0.0;
    primitive->addVertex(0.0f, 0.0f, z);
    for (int32_t i = 0; i <= numberOfDivisions; i++) {
        const float t = step * i;
        const float x = radius * std::cos(t);
        const float y = radius * std::sin(t);
        
        primitive->addVertex(x, y, z);
    }
    
    return primitive;
    
}


/**
 * Create the vertices for a ring.
 *
 * @param numberOfDivisions
 *    Number of divisions
 * @param innerRadius
 *    Inner radius of the ring.
 * @param outerRadius
 *    Outer radius of the ring.
 * @return
 *    Primitive containing vertices of ring.
 */
GraphicsPrimitive*
GraphicsShape::createRingPrimitive(const RingKey& ringKey)
{
    /*
     * Setup step size based upon number of points around circle
     */
    const float step = (2.0 * M_PI) / ringKey.m_numberOfDivisions;
    
    GraphicsPrimitiveV3f* primitive = GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                         (uint8_t[]) { 255, 255, 255, 255 });
    primitive->reserveForNumberOfVertices((ringKey.m_numberOfDivisions + 1) * 2);
    /*
     * Generate points around ring
     */
    const float z = 0.0;
    for (int32_t i = 0; i <= ringKey.m_numberOfDivisions; i++) {
        const float t = step * i;
        
        const float xin  = ringKey.m_innerRadius * std::cos(t);
        const float yin  = ringKey.m_innerRadius * std::sin(t);
        const float xout = ringKey.m_outerRadius * std::cos(t);
        const float yout = ringKey.m_outerRadius * std::sin(t);
        
        primitive->addVertex(xin, yin, z);
        primitive->addVertex(xout, yout, z);
    }
    
    return primitive;
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
 * Create an XYZ coordinate on a sphere.
 *
 * @param radius
 *     Radius of the sphere
 * @param latDegrees
 *     Latitude on sphere in degrees
 * @param lonDegrees
 *     Longitude on sphere in degrees
 * @param latIndex
 *     Index of the latitude sections
 * @param numLat
 *     Number of latitude sections
 * @param xyzOut
 *     Output with XYZ coordinate
 * @param normalXyzOut
 *     Output with Normal vector
 */
void
GraphicsShape::createSphereXYZ(const float radius,
                               const float latDegrees,
                               const float lonDegrees,
                               const int32_t latIndex,
                               const int32_t numLat,
                               float xyzOut[3],
                               float normalXyzOut[3])
{
    constexpr float degToRad = M_PI / 180.0f;
    
    const float lonRad = lonDegrees * degToRad;
    const float latRad = latDegrees * degToRad;
    const float z = radius * std::sin(latRad);
    

    xyzOut[0] = radius * std::cos(latRad) * std::cos(lonRad);
    xyzOut[1] = radius * std::cos(latRad) * std::sin(lonRad);
    xyzOut[2] = z;
    
    if (latIndex == 0) {
        xyzOut[0] =  0.0f;
        xyzOut[1] =  0.0f;
        xyzOut[2] = -radius;
    }
    else if (latIndex == (numLat - 1)) {
        xyzOut[0] = 0.0f;
        xyzOut[1] = 0.0f;
        xyzOut[2] = radius;
    }
    
    normalXyzOut[0] = xyzOut[0];
    normalXyzOut[1] = xyzOut[1];
    normalXyzOut[2] = xyzOut[2];
    MathFunctions::normalizeVector(normalXyzOut);
    
}

/**
 * Create a sphere with radius of 0.5 and with the given number
 * of latitude and longitude divisions.
 *
 * @param numberOfLatLon
 *     Number of latitude/longitude divisions
 * @return
 *     Graphics primitive containing the sphere.
 */
GraphicsPrimitiveV3fN3f*
GraphicsShape::createSpherePrimitiveTriangles(const int32_t numberOfLatLon)
{
    if (numberOfLatLon < 3) {
        CaretAssertMessage(0, "For sphere, must have at least 3 lat/lon divisions");
        CaretLogSevere("For sphere, must have at least 3 lat/lon divisions");
        return NULL;
    }
    
    bool debugFlag = false;
    if (debugFlag) {
        std::cout << std::endl;
        std::cout << "TRIANGLE START" << std::endl;
    }
    
    const float radius = 0.5f;
    
    const int numLat = numberOfLatLon;
    const int numLon = numberOfLatLon;
    
    uint8_t rgba[4] = { 255, 255, 255, 255 };
    GraphicsPrimitiveV3fN3f* primitive = GraphicsPrimitive::newPrimitiveV3fN3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES,
                                                                               rgba);
    
    const float dLat = 180.0 / numLat;
    for (int iLat = 0; iLat < numLat; iLat++) {
        const float latDeg = -90.0f + (iLat * dLat);
        
        const float latDeg2 = -90.0f + ((iLat + 1) * dLat);
        
        const float dLon = 360.0f / numLon;
        for (int iLon = 0; iLon < numLon; iLon++) {
            const float lonDeg = iLon * dLon;
            
            float xyz1[3];
            float normal1[3];
            createSphereXYZ(radius, latDeg, lonDeg, iLat, numLat, xyz1, normal1);
            
            float xyz2[3];
            float normal2[3];
            createSphereXYZ(radius, latDeg2, lonDeg, iLat + 1, numLat, xyz2, normal2);
            

            float xyz3[3];
            float normal3[3];
            createSphereXYZ(radius, latDeg, (lonDeg + dLon), iLat, numLat, xyz3, normal3);
            
            float xyz4[3];
            float normal4[3];
            createSphereXYZ(radius, latDeg2, (lonDeg + dLon), iLat + 1, numLat, xyz4, normal4);
            
            primitive->addVertex(xyz1, normal1);
            primitive->addVertex(xyz3, normal3);
            primitive->addVertex(xyz2, normal2);
            
            primitive->addVertex(xyz2, normal2);
            primitive->addVertex(xyz3, normal3);
            primitive->addVertex(xyz4, normal4);
            
            if (debugFlag) {
                const int32_t indx1 = (primitive->getNumberOfVertices() - 2);
                const int32_t indx2 = (primitive->getNumberOfVertices() - 1);
                std::cout << "   " << iLat << ", " << iLon << std::endl;
                std::cout << "       " << indx1 << ":" << latDeg << ", " << lonDeg << ", " << AString::fromNumbers(xyz1, 3, ",") << std::endl;
                std::cout << "       " << indx2 << ":" << latDeg2 << ", " << lonDeg << ", " << AString::fromNumbers(xyz2, 3, ",") << std::endl;
                std::cout << "       "  << AString::fromNumbers(xyz3, 3, ",") << std::endl;
                std::cout << "       "  << AString::fromNumbers(xyz4, 3, ",") << std::endl;
            }
        }
        
        if (debugFlag) std::cout << std::endl;
    }
    
    return primitive;
}

/**
 * Create a sphere with radius of 0.5 and with the given number
 * of latitude and longitude divisions.
 *
 * @param numberOfLatLon
 *     Number of latitude/longitude divisions
 * @return
 *     Graphics primitive containing the sphere.
 */
GraphicsPrimitiveV3fN3f*
GraphicsShape::createSpherePrimitiveTriangleStrips(const int32_t numberOfLatLon)
{
    if (numberOfLatLon < 3) {
        CaretAssertMessage(0, "For sphere, must have at least 3 lat/lon divisions");
        CaretLogSevere("For sphere, must have at least 3 lat/lon divisions");
        return NULL;
    }
    
    bool debugFlag = false;
    if (debugFlag) {
        std::cout << std::endl;
        std::cout << "TRIANGLE START" << std::endl;
    }
    
    const float radius = 0.5f;
    
    const int numLat = numberOfLatLon;
    const int numLon = numberOfLatLon;
    
    uint8_t rgba[4] = { 255, 255, 255, 255 };
    GraphicsPrimitiveV3fN3f* primitive = GraphicsPrimitive::newPrimitiveV3fN3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                               rgba);
    
    const float dLat = 180.0 / numLat;
    for (int iLat = 0; iLat < numLat; iLat++) {
        const float latDeg = -90.0f + (iLat * dLat);
        
        const float latDeg2 = -90.0f + ((iLat + 1) * dLat);
        
        const float dLon = 360.0f / numLon;
        for (int iLon = 0; iLon <= numLon; iLon++) {
            const float lonDeg = iLon * dLon;
            
            float xyz1[3];
            float normal1[3];
            createSphereXYZ(radius, latDeg, lonDeg, iLat, numLat, xyz1, normal1);
            
            float xyz2[3];
            float normal2[3];
            createSphereXYZ(radius, latDeg2, lonDeg, iLat + 1, numLat, xyz2, normal2);
            
            primitive->addVertex(xyz2, normal2);
            primitive->addVertex(xyz1, normal1);
            
            if (debugFlag) {
                const int32_t indx1 = (primitive->getNumberOfVertices() - 2);
                const int32_t indx2 = (primitive->getNumberOfVertices() - 1);
                std::cout << "   " << iLat << ", " << iLon << std::endl;
                std::cout << "       " << indx1 << ":" << latDeg << ", " << lonDeg << ", " << AString::fromNumbers(xyz1, 3, ",") << std::endl;
                std::cout << "       " << indx2 << ":" << latDeg2 << ", " << lonDeg << ", " << AString::fromNumbers(xyz2, 3, ",") << std::endl;
            }
        }
        
        if (iLat < (numLat - 1)) {
            primitive->addPrimitiveRestart();
        }
        if (debugFlag) std::cout << std::endl;
    }
    
    return primitive;
}

/**
 * Update the model matrix to face the viewer by applying a 
 * technique known as "billboarding".  Callers need to
 * push/pop the modelview matrix before/after calling 
 * this method.
 *
 * The result is that the X-Y plane faces the user.
 */
void
GraphicsShape::updateModelMatrixToFaceViewer()
{
    /*
     * Get the translation and rotation from the
     * current modelview matrix.
     */
    double modelMatrixArray[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrixArray);
    Matrix4x4 matrix;
    matrix.setMatrixFromOpenGL(modelMatrixArray);
    float txyz[3];
    matrix.getTranslation(txyz);
    double sx, sy, sz;
    matrix.getScale(sx, sy, sz);
    
    /*
     * Replace modelview matrix with
     * just translation and scaling.
     */
    glLoadIdentity();
    glTranslatef(txyz[0], txyz[1], txyz[2]);
    glScalef(sx, sy, sz);
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

