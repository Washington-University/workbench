
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

#define __GRAPHICS_PRIMITIVE_V3F_T3F_DECLARE__
#include "GraphicsPrimitiveV3fT3F.h"
#undef __GRAPHICS_PRIMITIVE_V3F_T3F_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsPrimitiveV3fT3F 
 * \brief Primitive containing XYZ with and texture coordinates applied to all vertices.
 * \ingroup Graphics
 */

/**
 * Constructor for solid color float RGBA.
 * 
 * @param primitiveType
 *     Type of primitive drawn (triangles, lines, etc.)
 * @param imageBytesRGBA
 *     Bytes containing the image data.
 * @param imageWidth
 *     Width of the actual image.
 * @param imageHeight
 *     Height of the image.
 */
GraphicsPrimitiveV3fT3F::GraphicsPrimitiveV3fT3F(const PrimitiveType primitiveType,
                                                 const uint8_t* imageBytesRGBA,
                                                 const int32_t imageWidth,
                                                 const int32_t imageHeight)
: GraphicsPrimitive(VertexType::FLOAT_XYZ,
                    NormalVectorType::NONE,
                    ColorType::NONE,
                    TextureType::FLOAT_STR,
                    primitiveType)
{
    setTextureImage(imageBytesRGBA,
                    imageWidth,
                    imageHeight);
}

/**
 * Destructor.
 */
GraphicsPrimitiveV3fT3F::~GraphicsPrimitiveV3fT3F()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsPrimitiveV3fT3F::GraphicsPrimitiveV3fT3F(const GraphicsPrimitiveV3fT3F& obj)
: GraphicsPrimitive(obj)
{
    this->copyHelperGraphicsPrimitiveV3fT3F(obj);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsPrimitiveV3fT3F::copyHelperGraphicsPrimitiveV3fT3F(const GraphicsPrimitiveV3fT3F& /*obj*/)
{
    
}

/**
 * Add a vertex.
 * 
 * @param xyz
 *     Coordinate of vertex.
 * @parma st
 *     Texture coordinates
 */
void
GraphicsPrimitiveV3fT3F::addVertex(const float xyz[3],
                                   const float st[2])
{
    m_xyz.insert(m_xyz.end(),
                 xyz, xyz + 3);
    m_floatTextureSTR.push_back(st[0]);
    m_floatTextureSTR.push_back(st[1]);
    m_floatTextureSTR.push_back(0.0f);
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
 * @param s
 *     S-coordinate of texture
 * @param t
 *     T-coordinate of texture
 */
void
GraphicsPrimitiveV3fT3F::addVertex(const float x,
                                   const float y,
                                   const float z,
                                   const float s,
                                   const float t)
{
    m_xyz.push_back(x);
    m_xyz.push_back(y);
    m_xyz.push_back(z);
    m_floatTextureSTR.push_back(s);
    m_floatTextureSTR.push_back(t);
    m_floatTextureSTR.push_back(0.0f);
}

/**
 * Add a 2D vertex.  Z will be zero.
 *
 * @param x
 *     X-coordinate of vertex.
 * @param y
 *     Y-coordinate of vertex.
 * @param s
 *     S-coordinate of texture
 * @param t
 *     T-coordinate of texture
 */
void
GraphicsPrimitiveV3fT3F::addVertex(const float x,
                                   const float y,
                                   const float s,
                                   const float t)
{
    m_xyz.push_back(x);
    m_xyz.push_back(y);
    m_xyz.push_back(0.0f);
    m_floatTextureSTR.push_back(s);
    m_floatTextureSTR.push_back(t);
    m_floatTextureSTR.push_back(0.0f);
}

/**
 * Clone this primitive.
 */
GraphicsPrimitive*
GraphicsPrimitiveV3fT3F::clone() const
{
    GraphicsPrimitiveV3fT3F* obj = new GraphicsPrimitiveV3fT3F(*this);
    return obj;
}

