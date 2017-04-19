
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

#define __GRAPHICS_PRIMITIVE_V3F_DECLARE__
#include "GraphicsPrimitiveV3f.h"
#undef __GRAPHICS_PRIMITIVE_V3F_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsPrimitiveV3f 
 * \brief Primitive containing XYZ with one color (float, unsigned byte) applied to all vertices.
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
GraphicsPrimitiveV3f::GraphicsPrimitiveV3f(const PrimitiveType primitiveType,
                                           const float rgba[4])
: GraphicsPrimitive(VertexType::FLOAT_XYZ,
                    NormalVectorType::NONE,
                    ColorType::FLOAT_RGBA,
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
GraphicsPrimitiveV3f::GraphicsPrimitiveV3f(const PrimitiveType primitiveType,
                                           const uint8_t rgba[4])
: GraphicsPrimitive(VertexType::FLOAT_XYZ,
                    NormalVectorType::NONE,
                    ColorType::UNSIGNED_BYTE_RGBA,
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
GraphicsPrimitiveV3f::~GraphicsPrimitiveV3f()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsPrimitiveV3f::GraphicsPrimitiveV3f(const GraphicsPrimitiveV3f& obj)
: GraphicsPrimitive(obj)
{
    this->copyHelperGraphicsPrimitiveV3f(obj);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsPrimitiveV3f::copyHelperGraphicsPrimitiveV3f(const GraphicsPrimitiveV3f& obj)
{
    
}

/**
 * Add a vertex.
 * 
 * @param xyz
 *     Coordinate of vertex.
 */
void
GraphicsPrimitiveV3f::addVertex(const float xyz[3])
{
    m_xyz.insert(m_xyz.end(),
                 xyz, xyz + 3);
    
    switch (m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
            m_floatRGBA.insert(m_floatRGBA.end(),
                               m_floatSolidRGBA, m_floatSolidRGBA + 4);
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
            m_unsignedByteRGBA.insert(m_unsignedByteRGBA.end(),
                                      m_unsignedByteSolidRGBA, m_unsignedByteSolidRGBA + 4);
            break;
    }
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
 */
void
GraphicsPrimitiveV3f::addVertex(const float x,
                                   const float y,
                                   const float z)
{
    m_xyz.push_back(x);
    m_xyz.push_back(y);
    m_xyz.push_back(z);
    switch (m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
            m_floatRGBA.insert(m_floatRGBA.end(),
                               m_floatSolidRGBA, m_floatSolidRGBA + 4);
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
            m_unsignedByteRGBA.insert(m_unsignedByteRGBA.end(),
                                      m_unsignedByteSolidRGBA, m_unsignedByteSolidRGBA + 4);
            break;
    }
}

/**
 * Add a 2D vertex.  Z will be zero.
 *
 * @param x
 *     X-coordinate of vertex.
 * @param y
 *     Y-coordinate of vertex.
 * @param rgba
 *     RGBA color components ranging 0.0 to 1.0.
 */
void
GraphicsPrimitiveV3f::addVertex(const float x,
                                   const float y)
{
    m_xyz.push_back(x);
    m_xyz.push_back(y);
    m_xyz.push_back(0.0);
    switch (m_colorType) {
        case GraphicsPrimitive::ColorType::FLOAT_RGBA:
            m_floatRGBA.insert(m_floatRGBA.end(),
                               m_floatSolidRGBA, m_floatSolidRGBA + 4);
            break;
        case GraphicsPrimitive::ColorType::UNSIGNED_BYTE_RGBA:
            m_unsignedByteRGBA.insert(m_unsignedByteRGBA.end(),
                                      m_unsignedByteSolidRGBA, m_unsignedByteSolidRGBA + 4);
            break;
    }
}

/**
 * Add XYZ vertices from an array.
 *
 * @param xyzArray
 *    Array containing XYZ vertex data.
 * @param numberOfVertices
 *    Number of vertices (xyz triplets) to add
 */
void
GraphicsPrimitiveV3f::addVertices(const float xyzArray[],
                                  const int32_t numberOfVertices)
{
    for (int32_t i = 0; i < numberOfVertices; i++) {
        addVertex(&xyzArray[i*3]);
    }
}


/**
 * Clone this primitive.
 */
GraphicsPrimitive*
GraphicsPrimitiveV3f::clone() const
{
    GraphicsPrimitiveV3f* obj = new GraphicsPrimitiveV3f(*this);
    return obj;
}

