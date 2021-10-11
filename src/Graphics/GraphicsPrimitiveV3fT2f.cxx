
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

#define __GRAPHICS_PRIMITIVE_V3F_T2F_DECLARE__
#include "GraphicsPrimitiveV3fT2f.h"
#undef __GRAPHICS_PRIMITIVE_V3F_T2F_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsPrimitiveV3fT2f
 * \brief Primitive containing XYZ with and texture coordinates applied to all vertices used for 2D images.
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
 * @param textureWrappingType
 *     Type of texture wrapping
 * @param textureFilteringType
 *     Type of texture filtering
 * @param textureMagnificationFilter
 *    Texture filtering for when screen pixel is smaller than texture  texel
 * @param textureMinificationFilter
 *    Texture filtering for when screen pixel is larger than texture texel
 */
GraphicsPrimitiveV3fT2f::GraphicsPrimitiveV3fT2f(const PrimitiveType primitiveType,
                                                 const uint8_t* imageBytesRGBA,
                                                 const int32_t imageWidth,
                                                 const int32_t imageHeight,
                                                 const TextureWrappingType textureWrappingType,
                                                 const TextureFilteringType textureFilteringType,
                                                 const GraphicsTextureMagnificationFilterEnum::Enum textureMagnificationFilter,
                                                 const GraphicsTextureMinificationFilterEnum::Enum textureMinificationFilter)
: GraphicsPrimitive(VertexDataType::FLOAT_XYZ,
                    NormalVectorDataType::NONE,
                    ColorDataType::NONE,
                    VertexColorType::NONE,
                    TextureDataType::FLOAT_STR_2D,
                    textureWrappingType,
                    textureFilteringType,
                    textureMagnificationFilter,
                    textureMinificationFilter,
                    primitiveType)
{
    const int32_t imageSlices(1);
    setTextureImage(imageBytesRGBA,
                    imageWidth,
                    imageHeight,
                    imageSlices);
}

/**
 * Destructor.
 */
GraphicsPrimitiveV3fT2f::~GraphicsPrimitiveV3fT2f()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsPrimitiveV3fT2f::GraphicsPrimitiveV3fT2f(const GraphicsPrimitiveV3fT2f& obj)
: GraphicsPrimitive(obj)
{
    this->copyHelperGraphicsPrimitiveV3fT2f(obj);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsPrimitiveV3fT2f::copyHelperGraphicsPrimitiveV3fT2f(const GraphicsPrimitiveV3fT2f& /*obj*/)
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
GraphicsPrimitiveV3fT2f::addVertex(const float xyz[3],
                                   const float st[2])
{
    const float str[] { st[0], st[1], 0.0f } ;
    addVertexProtected(xyz,
                       NULL,
                       NULL,
                       NULL,
                       str);
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
GraphicsPrimitiveV3fT2f::addVertex(const float x,
                                   const float y,
                                   const float z,
                                   const float s,
                                   const float t)
{
    const float xyz[] { x, y, z };
    const float str[] { s, t, 0.0f };
    addVertex(xyz,
              str);
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
GraphicsPrimitiveV3fT2f::addVertex(const float x,
                                   const float y,
                                   const float s,
                                   const float t)
{
    addVertex(x, y, 0.0f, s, t);
}

/**
 * Clone this primitive.
 */
GraphicsPrimitive*
GraphicsPrimitiveV3fT2f::clone() const
{
    GraphicsPrimitiveV3fT2f* obj = new GraphicsPrimitiveV3fT2f(*this);
    return obj;
}

