
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

#define __GRAPHICS_SHAPE_OPTIMIZED_DRAWING_DECLARE__
#include "GraphicsShapeOptimizedDrawing.h"
#undef __GRAPHICS_SHAPE_OPTIMIZED_DRAWING_DECLARE__

#include "CaretAssert.h"
#include "CaretOpenGLInclude.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fN3f.h"
#include "Vector3D.h"

using namespace caret;

/**
 * \class caret::GraphicsShapeOptimizedDrawing 
 * \brief Optimized drawing of shapes
 * \ingroup Brain
 */

/**
 * Draw a octahedron with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the sphere
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawOctahedron(const float diameter,
                                              const RgbaUnsignedByte& rgba)
{
    getInstance()->drawOctahedronImplementation(diameter,
                                                rgba);
}

/**
 * Draw a octahedron with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the sphere
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawOctahedron(const Vector3D& xyz,
                                              const float diameter,
                                              const RgbaUnsignedByte& rgba)
{
    getInstance()->drawOctahedronImplementation(xyz,
                                                diameter,
                                                rgba);
}

/**
 * Draw a sphere with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the sphere
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawSphere(const float diameter,
                                          const RgbaUnsignedByte& rgba)
{
    getInstance()->drawSphereImplementation(diameter,
                                            rgba);
}

/**
 * Draw a sphere with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the sphere
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawSphere(const Vector3D& xyz,
                                          const float diameter,
                                          const RgbaUnsignedByte& rgba)
{
    getInstance()->drawSphereImplementation(xyz,
                                            diameter,
                                            rgba);
}

/**
 * @return The singleton instance of this class
 */
GraphicsShapeOptimizedDrawing*
GraphicsShapeOptimizedDrawing::getInstance()
{
    if (s_singletonInstance == NULL) {
        s_singletonInstance = new GraphicsShapeOptimizedDrawing();
    }
    
    CaretAssert(s_singletonInstance);
    return s_singletonInstance;
}


/**
 * Constructor.
 */
GraphicsShapeOptimizedDrawing::GraphicsShapeOptimizedDrawing()
: CaretObject()
{
    const int32_t numberOfSlices(2);
    const int32_t numberOfStacks(2);
    const bool joinWithDegeneratesFlag(true);
    m_sphereTriangleStripInfo.reset(createSphereTriangleStrips(numberOfSlices,
                                                         numberOfStacks,
                                                         joinWithDegeneratesFlag));
    
    m_octahedronTriangleStripInfo.reset(createOctahedronTriangleStrips(joinWithDegeneratesFlag));
}

/**
 * Destructor.
 */
GraphicsShapeOptimizedDrawing::~GraphicsShapeOptimizedDrawing()
{
}

/**
 * Draw a octahedron with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the octahedron
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawOctahedronImplementation(const float diameter,
                                                            const RgbaUnsignedByte& rgba)
{
    glPushMatrix();
    glScalef(diameter, diameter, diameter);
    GraphicsPrimitiveV3fN3f* primitive(getOctahedronPrimitiveForRGBA(rgba));
    GraphicsEngineDataOpenGL::draw(primitive);
    glPopMatrix();
}

/**
 * Draw a octahedron with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the octahedron
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawOctahedronImplementation(const Vector3D& xyz,
                                                        const float diameter,
                                                        const RgbaUnsignedByte& rgba)
{
    glPushMatrix();
    glTranslatef(xyz[0], xyz[1], xyz[2]);
    glScalef(diameter, diameter, diameter);
    GraphicsPrimitiveV3fN3f* primitive(getOctahedronPrimitiveForRGBA(rgba));
    GraphicsEngineDataOpenGL::draw(primitive);
    glPopMatrix();
}

/**
 * Draw a sphere with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the sphere
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawSphereImplementation(const float diameter,
                                                            const RgbaUnsignedByte& rgba)
{
    glPushMatrix();
    glScalef(diameter, diameter, diameter);
    GraphicsPrimitiveV3fN3f* primitive(getSpherePrimitiveForRGBA(rgba));
    GraphicsEngineDataOpenGL::draw(primitive);
    glPopMatrix();
}

/**
 * Draw a sphere with the given diameter and rgba color at the current location
 * @param diameter
 *    Diameter of the sphere
 * @param rgba
 *    Rgba Color components
 */
void
GraphicsShapeOptimizedDrawing::drawSphereImplementation(const Vector3D& xyz,
                                                            const float diameter,
                                                            const RgbaUnsignedByte& rgba)
{
    glPushMatrix();
    glTranslatef(xyz[0], xyz[1], xyz[2]);
    glScalef(diameter, diameter, diameter);
    GraphicsPrimitiveV3fN3f* primitive(getSpherePrimitiveForRGBA(rgba));
    GraphicsEngineDataOpenGL::draw(primitive);
    glPopMatrix();
}

/**
 * Get (and if necessary generate) a octahedron with diameter 1 with the given rgba coloring
 * @param rgba
 *   RGBA for octahedron
 */
GraphicsPrimitiveV3fN3f*
GraphicsShapeOptimizedDrawing::getOctahedronPrimitiveForRGBA(const RgbaUnsignedByte& rgba)
{
    GraphicsPrimitiveV3fN3f* primitive(NULL);
    
    auto iter(m_octahedronPrimitives.find(rgba.getUnsignedIntRGBA()));
    if (iter != m_octahedronPrimitives.end()) {
        primitive = iter->second;
    }
    else {
        primitive = new GraphicsPrimitiveV3fN3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                rgba.getRgbaPointer());
        CaretAssert(static_cast<int32_t>(m_octahedronTriangleStripInfo->coordinates.size())
                    == (m_octahedronTriangleStripInfo->totalVertices * 3));
        primitive->addVertices(m_octahedronTriangleStripInfo->coordinates.data(),
                               m_octahedronTriangleStripInfo->normals.data(),
                               m_octahedronTriangleStripInfo->totalVertices);
        m_octahedronPrimitives.insert(std::make_pair(rgba.getUnsignedIntRGBA(), primitive));
        
        std::cout << "Create octahedron with " << m_octahedronTriangleStripInfo->totalVertices
        << " vertices for: " << rgba.toString() << std::endl;
    }
    
    CaretAssert(primitive);
    return primitive;
}

/**
 * Get (and if necessary generate) a sphere with diameter 1 with the given rgba coloring
 * @param rgba
 *   RGBA for sphere
 */
GraphicsPrimitiveV3fN3f*
GraphicsShapeOptimizedDrawing::getSpherePrimitiveForRGBA(const RgbaUnsignedByte& rgba)
{
    GraphicsPrimitiveV3fN3f* primitive(NULL);
    
    auto iter(m_spherePrimitives.find(rgba.getUnsignedIntRGBA()));
    if (iter != m_spherePrimitives.end()) {
        primitive = iter->second;
    }
    else {
        primitive = new GraphicsPrimitiveV3fN3f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                rgba.getRgbaPointer());
        CaretAssert(static_cast<int32_t>(m_sphereTriangleStripInfo->coordinates.size())
                    == (m_sphereTriangleStripInfo->totalVertices * 3));
        primitive->addVertices(m_sphereTriangleStripInfo->coordinates.data(),
                               m_sphereTriangleStripInfo->normals.data(),
                               m_sphereTriangleStripInfo->totalVertices);
        m_spherePrimitives.insert(std::make_pair(rgba.getUnsignedIntRGBA(), primitive));
        
        std::cout << "Create sphere with " << m_sphereTriangleStripInfo->totalVertices
        << " vertices for: " << rgba.toString() << std::endl;
    }
    
    CaretAssert(primitive);
    return primitive;
}

/**
 * Create triangle strip vertices for a sphere of diameter 1 centered at the origin.
 * Note: Generated by Claude.
 *
 * @param numberOfSlices  Divisions around the Z axis (longitude), >= 3.
 * @param numberOfStacks  Divisions from +Z pole to -Z pole (latitude), >= 2.
 * @param joinWithDegenerates
 *    If true, all bands are connected into ONE strip using degenerate
 *    triangles (a single glDrawArrays call).  If false, the data holds
 *    separate strips of equal length, one per latitude band.
 */
GraphicsShapeOptimizedDrawing::SphereTriangleStrips*
GraphicsShapeOptimizedDrawing::createSphereTriangleStrips(const int numberOfSlices,
                                                    const int numberOfStacks,
                                                    const bool joinWithDegenerates)
{
    const double radius = 0.5;
    const double pi = 3.14159265358979323846;
    
    SphereTriangleStrips* s(new SphereTriangleStrips());
    const int slices = (numberOfSlices < 3) ? 3 : numberOfSlices;
    const int stacks = (numberOfStacks < 2) ? 2 : numberOfStacks;
    
    auto addVertex = [&](const int stack, const int slice) {
        const double phi   = pi * static_cast<double>(stack) / stacks;           // 0 at +Z pole
        const double theta = 2.0 * pi * static_cast<double>(slice % slices) / slices;
        const double nx = std::sin(phi) * std::cos(theta);
        const double ny = std::sin(phi) * std::sin(theta);
        const double nz = std::cos(phi);
        s->normals.push_back(static_cast<float>(nx));
        s->normals.push_back(static_cast<float>(ny));
        s->normals.push_back(static_cast<float>(nz));
        s->coordinates.push_back(static_cast<float>(nx * radius));
        s->coordinates.push_back(static_cast<float>(ny * radius));
        s->coordinates.push_back(static_cast<float>(nz * radius));
    };
    
    for (int i = 0; i < stacks; i++) {
        if (joinWithDegenerates && (i > 0)) {
            // repeat last vertex of previous band and first vertex of this band
            addVertex(i, slices);   // last vertex of previous band (stack i, slice 'slices')
            addVertex(i, 0);        // first vertex of this band
        }
        for (int j = 0; j <= slices; j++) {  // <= closes the seam
            addVertex(i,     j);    // upper ring
            addVertex(i + 1, j);    // lower ring
        }
    }
    
    CaretAssert(s->coordinates.size() == s->normals.size());
    
    s->numberOfStrips   = joinWithDegenerates ? 1 : stacks;
    s->verticesPerStrip = 2 * (slices + 1);
    s->totalVertices    = static_cast<int>(s->coordinates.size() * 3);
    
    return s;
}

/**
 * Create triangle strip vertices for an octahedron of diameter 1
 * centered at the origin.  Triangles are counter-clockwise viewed from outside.
 * Note: Generated by Claude.
 *
 * @param joinWithDegenerates
 *    If true, everything is one strip (19 vertices) joined with degenerate
 *    triangles.  If false, three strips (8 + 3 + 3 vertices).
 */
GraphicsShapeOptimizedDrawing::OctahedronStrips*
GraphicsShapeOptimizedDrawing::createOctahedronTriangleStrips(const bool joinWithDegenerates)
{
    const float radius = 0.5f;
    enum { PX, NX, PY, NY, PZ, NZ };
    const float unit[6][3] = {
        { 1.0f,  0.0f,  0.0f },  // PX
        {-1.0f,  0.0f,  0.0f },  // NX
        { 0.0f,  1.0f,  0.0f },  // PY
        { 0.0f, -1.0f,  0.0f },  // NY
        { 0.0f,  0.0f,  1.0f },  // PZ
        { 0.0f,  0.0f, -1.0f }   // NZ
    };
    
    const std::vector<std::vector<int> > strips = {
        { PX, NZ, PY, NX, PZ, NY, PX, NZ },  // six "side" faces
        { PX, PY, PZ },                      // top cap
        { NX, NZ, NY }                       // bottom cap
    };
    
    OctahedronStrips* o(new OctahedronStrips());
    
    auto vertexCount = [&]() { return static_cast<int>(o->coordinates.size() / 3); };
    auto add = [&](const int id) {
        for (int i = 0; i < 3; i++) {
            o->normals.push_back(unit[id][i]);
            o->coordinates.push_back(unit[id][i] * radius);
        }
    };
    
    int lastId = -1;
    int joinedFirst = 0;
    for (const std::vector<int>& strip : strips) {
        if (joinWithDegenerates) {
            if (lastId >= 0) {
                add(lastId);      // repeat last vertex of previous strip
                add(strip[0]);    // repeat first vertex of this strip
                if ((vertexCount() % 2) != 0) {
                    add(strip[0]); // keep triangle winding correct
                }
            }
        }
        else {
            o->stripFirst.push_back(vertexCount());
        }
        for (const int id : strip) {
            add(id);
        }
        if (! joinWithDegenerates) {
            o->stripCount.push_back(static_cast<int>(strip.size()));
        }
        lastId = strip.back();
    }
    
    if (joinWithDegenerates) {
        o->stripFirst.push_back(joinedFirst);
        o->stripCount.push_back(vertexCount());
    }
    
    o->totalVertices = static_cast<int32_t>(o->coordinates.size() / 3);
    
    return o;
}
