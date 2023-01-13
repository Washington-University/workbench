
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

#include <QImage>
#include <QImageWriter>

#include "CaretAssert.h"
#include "FileInformation.h"

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
 * @param textureSettings
 *    Settings for texture mapping
 */
GraphicsPrimitiveV3fT2f::GraphicsPrimitiveV3fT2f(const PrimitiveType primitiveType,
                                                 const GraphicsTextureSettings& textureSettings)
: GraphicsPrimitive(VertexDataType::FLOAT_XYZ,
                    NormalVectorDataType::NONE,
                    ColorDataType::NONE,
                    VertexColorType::NONE,
                    textureSettings,
                    primitiveType)
{
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
 * @param xyz
 *     Coordinate of vertex.
 * @param s
 *     S-coordinate of texture
 * @param t
 *     T-coordinate of texture
 */
void
GraphicsPrimitiveV3fT2f::addVertex(const float xyz[3],
                                   const float s,
                                   const float t)
{
    const float str[] { s, t, 0.0f } ;
    addVertex(xyz,
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

/**
 * Export the texture to an image file
 */
bool
GraphicsPrimitiveV3fT2f::exportTextureToImageFile(const AString& imageFileName,
                                                  AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    const GraphicsTextureSettings& textureSettings(getTextureSettings());
    
    QImage::Format qtFormat(QImage::Format_Invalid);
    switch (textureSettings.getPixelFormatType()) {
        case GraphicsTextureSettings::PixelFormatType::NONE:
            break;
        case GraphicsTextureSettings::PixelFormatType::BGR:
            break;
        case GraphicsTextureSettings::PixelFormatType::BGRA:
            break;
        case GraphicsTextureSettings::PixelFormatType::BGRX:
            break;
        case GraphicsTextureSettings::PixelFormatType::RGB:
            break;
        case GraphicsTextureSettings::PixelFormatType::RGBA:
            qtFormat = QImage::Format_RGBA8888;
            break;
    }
    
    if (qtFormat == QImage::Format_Invalid) {
        errorMessageOut = "Invalid image format for writing to Qt.";
    }
    
    const int32_t imageWidth(textureSettings.getImageWidth());
    const int32_t imageHeight(textureSettings.getImageHeight());
    
    switch (textureSettings.getPixelOrigin()) {
        case GraphicsTextureSettings::PixelOrigin::NONE:
            break;
        case GraphicsTextureSettings::PixelOrigin::BOTTOM_LEFT:
            break;
        case GraphicsTextureSettings::PixelOrigin::TOP_LEFT:
            break;
    }
    
    const uint8_t* dataPtr(textureSettings.getImageBytesPointer());
    CaretAssert(dataPtr);
    
    QImage image(dataPtr,
                 imageWidth,
                 imageHeight,
                 qtFormat);
    FileInformation fileInfo(imageFileName);
    const QString writeToFileName(fileInfo.getAbsoluteFilePath());
    QImageWriter writer(writeToFileName);
    if ( ! writer.write(image)) {
        errorMessageOut = ("Error writing mask image file: "
                           + writeToFileName
                           + "\n"
                           + writer.errorString());
        return false;
    }
    
    return true;
}

