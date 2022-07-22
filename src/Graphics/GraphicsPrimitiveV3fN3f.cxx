
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

#define __GRAPHICS_PRIMITIVE_V3F_N3F_DECLARE__
#include "GraphicsPrimitiveV3fN3f.h"
#undef __GRAPHICS_PRIMITIVE_V3F_N3F_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GraphicsEngineDataOpenGL.h"

using namespace caret;


    
/**
 * \class caret::GraphicsPrimitiveV3fN3f 
 * \brief Primitive containing vertices, normals, and with one color (float, unsigned byte) applied to all vertices.
 * \ingroup Graphics
 *
 * When we move to using the programmable pipeline OpenGL, the color components
 * can be passed to the shader to avoid adding the color to each vertex.
 */

/**
 * Constructor for solid color float RGBA.
 * 
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 * @param rgba
 *     RGBA color components ranging 0.0 to 1.0.
 */
GraphicsPrimitiveV3fN3f::GraphicsPrimitiveV3fN3f(const PrimitiveType primitiveType,
                                           const float rgba[4])
: GraphicsPrimitive(VertexDataType::FLOAT_XYZ,
                    NormalVectorDataType::FLOAT_XYZ,
                    ColorDataType::FLOAT_RGBA,
                    VertexColorType::SOLID_RGBA,
                    GraphicsTextureSettings(),
                    primitiveType)
{
    m_floatSolidRGBA[0] = rgba[0];
    m_floatSolidRGBA[1] = rgba[1];
    m_floatSolidRGBA[2] = rgba[2];
    m_floatSolidRGBA[3] = rgba[3];
}

/**
 * Constructor for solid color unsigned byte RGBA.
 *
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 * @param rgba
 *     RGBA color components ranging 0.0 to 1.0.
 */
GraphicsPrimitiveV3fN3f::GraphicsPrimitiveV3fN3f(const PrimitiveType primitiveType,
                                           const uint8_t rgba[4])
: GraphicsPrimitive(VertexDataType::FLOAT_XYZ,
                    NormalVectorDataType::FLOAT_XYZ,
                    ColorDataType::UNSIGNED_BYTE_RGBA,
                    VertexColorType::SOLID_RGBA,
                    GraphicsTextureSettings(),
                    primitiveType)
{
    m_unsignedByteSolidRGBA[0] = rgba[0];
    m_unsignedByteSolidRGBA[1] = rgba[1];
    m_unsignedByteSolidRGBA[2] = rgba[2];
    m_unsignedByteSolidRGBA[3] = rgba[3];
}

/**
 * Destructor.
 */
GraphicsPrimitiveV3fN3f::~GraphicsPrimitiveV3fN3f()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsPrimitiveV3fN3f::GraphicsPrimitiveV3fN3f(const GraphicsPrimitiveV3fN3f& obj)
: GraphicsPrimitive(obj)
{
    this->copyHelperGraphicsPrimitiveV3fN3f(obj);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsPrimitiveV3fN3f::copyHelperGraphicsPrimitiveV3fN3f(const GraphicsPrimitiveV3fN3f& obj)
{
    for (int32_t i = 0; i < 4; i++) {
        m_floatSolidRGBA[i]        = obj.m_floatSolidRGBA[i];
        m_unsignedByteSolidRGBA[i] = obj.m_unsignedByteSolidRGBA[i];
    }
}

/**
 * Add a vertex.
 * 
 * @param xyz
 *     Coordinate of vertex.
 * @param normalXYZ
 *     Normal vector
 */
void
GraphicsPrimitiveV3fN3f::addVertex(const float xyz[3],
                                   const float normalXYZ[3])
{
    addVertexProtected(xyz,
                       normalXYZ,
                       m_floatSolidRGBA,
                       m_unsignedByteSolidRGBA,
                       NULL);
}

/**
 * Add a vertex.
 *
 * @param xyz
 *     Coordinate of vertex.
 * @param normalXYZ
 *     Normal vector
 */
void
GraphicsPrimitiveV3fN3f::addVertex(const double xyz[3],
                                   const double normalXYZ[3])
{
    const float floatXYZ[] {
        static_cast<float>(xyz[0]),
        static_cast<float>(xyz[1]),
        static_cast<float>(xyz[2])
    };
    const float floatNormalXYZ[3] {
        static_cast<float>(normalXYZ[0]),
        static_cast<float>(normalXYZ[1]),
        static_cast<float>(normalXYZ[2])
    };
    addVertexProtected(floatXYZ,
                       floatNormalXYZ,
                       m_floatSolidRGBA,
                       m_unsignedByteSolidRGBA,
                       NULL);
}

/**
 * Add a vertex.
 *
 * @param x
 *     X-coordinate of vertex.
 * @param y
 *     Y-coordinate of vertex.
 * @param z
 *     Z-coordinate of vertex.
 * @param normalX
 *     X component of normal vector
 * @param normalY
 *     Y component of normal vector
 * @param normalZ
 *     Z component of normal vector
 */
void
GraphicsPrimitiveV3fN3f::addVertex(const float x,
                                   const float y,
                                   const float z,
                                   const float normalX,
                                   const float normalY,
                                   const float normalZ)
{
    const float xyz[] { x, y, z };
    const float normalXYZ[] { normalX, normalY, normalZ };
    addVertexProtected(xyz,
                       normalXYZ,
                       m_floatSolidRGBA,
                       m_unsignedByteSolidRGBA,
                       NULL);
}

/**
 * Add a 2D vertex.  Z will be zero.
 *
 * @param x
 *     X-coordinate of vertex.
 * @param y
 *     Y-coordinate of vertex.
 * @param normalX
 *     X component of normal vector
 * @param normalY
 *     Y component of normal vector
 */
void
GraphicsPrimitiveV3fN3f::addVertex(const float x,
                                   const float y,
                                   const float normalX,
                                   const float normalY)
{
    const float xyz[] { x, y, 0.0f };
    const float normalXYZ[] { normalX, normalY, 1.0f };
    addVertexProtected(xyz,
                       normalXYZ,
                       m_floatSolidRGBA,
                       m_unsignedByteSolidRGBA,
                       NULL);
}

/**
 * Add XYZ vertices from an array.
 *
 * @param xyzArray
 *    Array containing XYZ vertex data.
 * @param normalXyzArray
 *    Array containing XYZ normal vector data.
 * @param numberOfVertices
 *    Number of vertices (xyz triplets) to add
 */
void
GraphicsPrimitiveV3fN3f::addVertices(const float xyzArray[],
                                     const float normalXyzArray[],
                                     const int32_t numberOfVertices)
{
    for (int32_t i = 0; i < numberOfVertices; i++) {
        addVertex(&xyzArray[i*3],
                  &normalXyzArray[i*3]);
    }
}


/**
 * Clone this primitive.
 */
GraphicsPrimitive*
GraphicsPrimitiveV3fN3f::clone() const
{
    GraphicsPrimitiveV3fN3f* obj = new GraphicsPrimitiveV3fN3f(*this);
    return obj;
}

