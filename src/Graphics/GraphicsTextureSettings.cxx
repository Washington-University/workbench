
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
m_qImage(NULL)
{
    
}

/**
 * Constructor.
 */
GraphicsTextureSettings::GraphicsTextureSettings(const QImage* qImage,
                                                 const DimensionType   dimensionType,
                                                 const PixelFormatType pixelFormatType,
                                                 const PixelOrigin     pixelOrigin,
                                                 const WrappingType    wrappingType,
                                                 const MipMappingType  mipMappingType,
                                                 const GraphicsTextureMagnificationFilterEnum::Enum magnificationFilter,
                                                 const GraphicsTextureMinificationFilterEnum::Enum minificationFilter,
                                                 const std::array<float, 4>& borderColor)
: CaretObject(),
m_qImage(qImage),
m_dimensionType(dimensionType),
m_pixelFormatType(pixelFormatType),
m_pixelOrigin(pixelOrigin),
m_wrappingType(wrappingType),
m_mipMappingType(mipMappingType),
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
    m_qImage              = obj.m_qImage;
    m_dimensionType       = obj.m_dimensionType;
    m_pixelFormatType     = obj.m_pixelFormatType;
    m_pixelOrigin         = obj.m_pixelOrigin;
    m_wrappingType        = obj.m_wrappingType;
    m_mipMappingType      = obj.m_mipMappingType;
    m_magnificationFilter = obj.m_magnificationFilter;
    m_minificationFilter  = obj.m_minificationFilter;
    m_borderColor         = obj.m_borderColor;
}

/**
 * @return Pointer to the qImage instance
 */
const QImage*
GraphicsTextureSettings::getQImage() const
{
    return m_qImage;
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
 * @return The magnification filter
 */
GraphicsTextureMagnificationFilterEnum::Enum
GraphicsTextureSettings::getMagnificationFilter() const
{
    return m_magnificationFilter;
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

