
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
#include "GraphicsPrimitiveV3fT3f.h"
#undef __GRAPHICS_PRIMITIVE_V3F_T3F_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsPrimitiveV3fT3f 
 * \brief Primitive containing XYZ with and texture coordinates applied to all vertices for 3D images (volumes)
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
 * @param imageSlices
 *     Slices of the image.
 * @param texturePixelFormatType
 *     Format of pixels in texture image
 * @param texturePixelOrigin
 *   Location of first pixel in texture image
 * @param textureWrappingType
 *     Type of texture wrapping
 * @param textureMipMappingType
 *     Type of texture mip mapping
 * @param textureMagnificationFilter
 *    Texture filtering for when screen pixel is smaller than texture  texel
 * @param textureMinificationFilter
 *    Texture filtering for when screen pixel is larger than texture texel
 * @param textureBorderColorRGBA
 *    Color used when clamp to border is used for warpping
 */
GraphicsPrimitiveV3fT3f::GraphicsPrimitiveV3fT3f(const PrimitiveType primitiveType,
                                                 const uint8_t* imageBytesRGBA,
                                                 const int32_t imageWidth,
                                                 const int32_t imageHeight,
                                                 const int32_t imageSlices,
                                                 const GraphicsTextureSettings::PixelFormatType texturePixelFormatType,
                                                 const GraphicsTextureSettings::PixelOrigin texturePixelOrigin,
                                                 const GraphicsTextureSettings::WrappingType textureWrappingType,
                                                 const GraphicsTextureSettings::MipMappingType textureMipMappingType,
                                                 const GraphicsTextureMagnificationFilterEnum::Enum textureMagnificationFilter,
                                                 const GraphicsTextureMinificationFilterEnum::Enum textureMinificationFilter,
                                                 const std::array<float, 4>& textureBorderColorRGBA)
: GraphicsPrimitive(VertexDataType::FLOAT_XYZ,
                    NormalVectorDataType::NONE,
                    ColorDataType::NONE,
                    VertexColorType::NONE,
                    GraphicsTextureSettings::DimensionType::FLOAT_STR_3D,
                    texturePixelFormatType,
                    texturePixelOrigin,
                    textureWrappingType,
                    textureMipMappingType,
                    textureMagnificationFilter,
                    textureMinificationFilter,
                    primitiveType)
{
    const int32_t rowStride(-1); /* negative is tightly packed */
    setTextureImage(imageBytesRGBA,
                    imageWidth,
                    imageHeight,
                    imageSlices,
                    rowStride);
    setTextureBorderColorRGBA(textureBorderColorRGBA);
}

/**
 * Destructor.
 */
GraphicsPrimitiveV3fT3f::~GraphicsPrimitiveV3fT3f()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsPrimitiveV3fT3f::GraphicsPrimitiveV3fT3f(const GraphicsPrimitiveV3fT3f& obj)
: GraphicsPrimitive(obj)
{
    this->copyHelperGraphicsPrimitiveV3fT3f(obj);
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsPrimitiveV3fT3f::copyHelperGraphicsPrimitiveV3fT3f(const GraphicsPrimitiveV3fT3f& /*obj*/)
{
    
}

/**
 * Add a vertex.
 * 
 * @param xyz
 *     Coordinate of vertex.
 * @parma str
 *     Texture coordinates
 */
void
GraphicsPrimitiveV3fT3f::addVertex(const float xyz[3],
                                   const float str[2])
{
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
 * @param r
 *     R-coordinate of texture
 */
void
GraphicsPrimitiveV3fT3f::addVertex(const float x,
                                   const float y,
                                   const float z,
                                   const float s,
                                   const float t,
                                   const float r)
{
    const float xyz[] { x, y, z };
    const float str[] { s, t, r };
    addVertex(xyz,
              str);
}

/**
 * Clone this primitive.
 */
GraphicsPrimitive*
GraphicsPrimitiveV3fT3f::clone() const
{
    GraphicsPrimitiveV3fT3f* obj = new GraphicsPrimitiveV3fT3f(*this);
    return obj;
}

