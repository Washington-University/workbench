
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __GRAPHICS_TEXTURE_SETTINGS_DECLARE__
#include "GraphicsTextureSettings.h"
#undef __GRAPHICS_TEXTURE_SETTINGS_DECLARE__

#include <cstdint>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::GraphicsTextureSettings 
 * \brief Contains settins used for primitives with textures
 * \ingroup Graphics
 */

/**
 * Default constructor
 */
GraphicsTextureSettings::GraphicsTextureSettings()
: CaretObject(),
m_imageBytesPointer(NULL)
{
    
}

/**
 * Constructor.
 * @param imageBytesPointer
 *    Pointer to image data bytes
 * @param imageWidth
 *    Width of image in pixels
 * @param imageHeight
 *    Height of image in pixels
 * @param imageSlices
 *    Number of slices in image (1 if 2D)
 * @param dimensionType
 *    Texture dimension (2D or 3D)
 * @param pixelFormatType
 *    Pixel color components type
 * @param pixelOrigin
 *    Origin of first pixel in image
 * @param wrappingType
 *    Texture wrapping type
 * @param mipMappingType
 *    Mip mapping type
 * @param compressionType
 *    Type of compression
 * @param magnificationFilter
 *    Type of texture magnification
 * @param minificationFilter
 *    Type of texture minification
 * @param borderColor
 *    Color for texture border (used where there is no texture data)
 */
GraphicsTextureSettings::GraphicsTextureSettings(const uint8_t*        imageBytesPointer,
                                                 const int32_t         imageWidth,
                                                 const int32_t         imageHeight,
                                                 const int32_t         imageSlices,
                                                 const DimensionType   dimensionType,
                                                 const PixelFormatType pixelFormatType,
                                                 const PixelOrigin     pixelOrigin,
                                                 const WrappingType    wrappingType,
                                                 const MipMappingType  mipMappingType,
                                                 const CompressionType compressionType,
                                                 const GraphicsTextureMagnificationFilterEnum::Enum magnificationFilter,
                                                 const GraphicsTextureMinificationFilterEnum::Enum minificationFilter,
                                                 const std::array<float, 4>& borderColor)
: CaretObject(),
m_imageBytesPointer(const_cast<uint8_t*>(imageBytesPointer)),
m_imageWidth(imageWidth),
m_imageHeight(imageHeight),
m_imageSlices(imageSlices),
m_dimensionType(dimensionType),
m_pixelFormatType(pixelFormatType),
m_pixelOrigin(pixelOrigin),
m_wrappingType(wrappingType),
m_mipMappingType(mipMappingType),
m_compressionType(compressionType),
m_magnificationFilter(magnificationFilter),
m_minificationFilter(minificationFilter),
m_borderColor(borderColor)
{
    
}

/**
 * Destructor.
 */
GraphicsTextureSettings::~GraphicsTextureSettings()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
GraphicsTextureSettings::GraphicsTextureSettings(const GraphicsTextureSettings& obj)
: CaretObject(obj)
{
    this->copyHelperGraphicsTextureSettings(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
GraphicsTextureSettings&
GraphicsTextureSettings::operator=(const GraphicsTextureSettings& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperGraphicsTextureSettings(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
GraphicsTextureSettings::copyHelperGraphicsTextureSettings(const GraphicsTextureSettings& obj)
{
    m_imageBytesPointer   = obj.m_imageBytesPointer;
    m_imageWidth          = obj.m_imageWidth;
    m_imageHeight         = obj.m_imageHeight;
    m_imageSlices         = obj.m_imageSlices;
    m_dimensionType       = obj.m_dimensionType;
    m_pixelFormatType     = obj.m_pixelFormatType;
    m_pixelOrigin         = obj.m_pixelOrigin;
    m_wrappingType        = obj.m_wrappingType;
    m_mipMappingType      = obj.m_mipMappingType;
    m_unpackAlignment     = obj.m_unpackAlignment;
    m_compressionType     = obj.m_compressionType;
    m_magnificationFilter = obj.m_magnificationFilter;
    m_minificationFilter  = obj.m_minificationFilter;
    m_borderColor         = obj.m_borderColor;
}

/**
 * @return Pointer to the qImage bytes
 */
const uint8_t*
GraphicsTextureSettings::getImageBytesPointer() const
{
    return m_imageBytesPointer;
}

/**
 * @return The widget of the image
 */
int32_t
GraphicsTextureSettings::getImageWidth() const
{
    return m_imageWidth;
}

/**
 * @return The height of the image
 */
int32_t
GraphicsTextureSettings::getImageHeight() const
{
    return m_imageHeight;
}

/**
 * @return The number of slices in the image (2D is 1)
 */
int32_t
GraphicsTextureSettings::getImageSlices() const
{
    return m_imageSlices;
}

/**
 * @return The dimension type
 */
GraphicsTextureSettings::DimensionType
GraphicsTextureSettings::getDimensionType() const
{
    return m_dimensionType;
}

/**
 * @return The pixel format
 */
GraphicsTextureSettings::PixelFormatType
GraphicsTextureSettings::getPixelFormatType() const
{
    return m_pixelFormatType;
}

/**
 * @return The pixel origin
 */
GraphicsTextureSettings::PixelOrigin
GraphicsTextureSettings::getPixelOrigin() const
{
    return m_pixelOrigin;
}

/**
 * @return The wrapping type
 */
GraphicsTextureSettings::WrappingType
GraphicsTextureSettings::getWrappingType() const
{
    return m_wrappingType;
}

/**
 * @return The mip mapping type
 */
GraphicsTextureSettings::MipMappingType
GraphicsTextureSettings::getMipMappingType() const
{
    return m_mipMappingType;
}

/**
 * @return The type of compression
 */
GraphicsTextureSettings::CompressionType
GraphicsTextureSettings::getCompressionType() const
{
    return m_compressionType;
}

/**
 * @return The magnification filter
 */
GraphicsTextureMagnificationFilterEnum::Enum
GraphicsTextureSettings::getMagnificationFilter() const
{
    return m_magnificationFilter;
}

/**
 * @return The unpack alignment.  See OpenGL documentation.  If wrong
 * image will be distorted (skewed).  May be needed if length of row in bytes
 * is not divisible by 4.
 */
int32_t
GraphicsTextureSettings::getUnpackAlignment() const
{
    return m_unpackAlignment;
}

void
GraphicsTextureSettings::setUnpackAlignmnet(const int32_t unpackAlignment)
{
    m_unpackAlignment = unpackAlignment;
}

/**
 * Set the magification filter
 * @param magificationFilter
 *     New value
 */
void
GraphicsTextureSettings::setMagnificationFilter(const GraphicsTextureMagnificationFilterEnum::Enum magnificationFilter)
{
    m_magnificationFilter = magnificationFilter;
}

/**
 * @return The minification filter
 */
GraphicsTextureMinificationFilterEnum::Enum
GraphicsTextureSettings::getMinificationFilter() const
{
    return m_minificationFilter;
}

/**
 * Set the minification filter
 * @param minificationFilter
 *     New value
 */
void
GraphicsTextureSettings::setMinificationFilter(const GraphicsTextureMinificationFilterEnum::Enum minificationFilter)
{
    m_minificationFilter = minificationFilter;
}

/**
 * @return The border color
 */
std::array<float, 4>
GraphicsTextureSettings::getBorderColor() const
{
    return m_borderColor;
}
/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GraphicsTextureSettings::toString() const
{
    return "GraphicsTextureSettings";
}

