
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __CZI_IMAGE_DECLARE__
#include "CziImage.h"
#undef __CZI_IMAGE_DECLARE__

#include <QImage>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImageFile.h"
#include "CziUtilities.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "RectangleTransform.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::CziImage 
 * \brief Image read from CZI file
 * \ingroup Files
 */

/**
 * Constructor
 * @param parentCziImageFile
 *    The parent CZI Image File
 * @param image
 *    The QImage instance
 * @param fullResolutionLogicalRect
 *    Logical Rectangle for the full-resolution source image
 * @param logicalRect
 *    Logical rectangle defining region of source image that was read from the file
 */
CziImage::CziImage(const CziImageFile* parentCziImageFile,
                   QImage* image,
                   const QRectF& fullResolutionLogicalRect,
                   const QRectF& logicalRect)
: CaretObject(),
m_parentCziImageFile(parentCziImageFile),
m_image(image),
m_fullResolutionLogicalRect(fullResolutionLogicalRect),
m_logicalRect(logicalRect)
{
    CaretAssert(image);

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_fullResolutionPixelsRect = QRectF(0, 0, m_fullResolutionLogicalRect.width(), m_fullResolutionLogicalRect.height());

    m_pixelsRect = QRectF(0, 0, m_image->width(), m_image->height());
    
    const PixelIndex logBotLeft(m_logicalRect.x(),
                                m_logicalRect.y() + m_logicalRect.height(),
                                0.0f);
    const PixelIndex pixelBotLeft = transformPixelIndexToSpace(logBotLeft,
                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT,
                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT);
    m_pixelsRegionOfInterestRect = QRectF(pixelBotLeft.getI(),
                                          pixelBotLeft.getJ(),
                                          m_logicalRect.width(),
                                          m_logicalRect.height());
}

/**
 * Destructor.
 */
CziImage::~CziImage()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CziImage::toString() const
{
    return "CziImage";
}

/**
 * @return Width of image
 */
int32_t
CziImage::getWidth() const
{
    if (m_image != NULL) {
        return m_image->width();
    }
    return 0;
}

/*
 * @return Height of image
 */
int32_t
CziImage::getHeight() const
{
    if (m_image != NULL) {
        return m_image->height();
    }
    return 0;
}

/**
 * Transform a pixel index to a different pixel space
 * @param pixelIndexIn
 *    The pixel index
 * @param fromPixelCoordSpace
 *    Current space of the input pixel
 * @param toPixelCoordSpace
 *    Space to transform to
 * @return Pixel index in new space
 */
PixelIndex
CziImage::transformPixelIndexToSpace(const PixelIndex& pixelIndexIn,
                                     const CziPixelCoordSpaceEnum::Enum fromPixelCoordSpace,
                                     const CziPixelCoordSpaceEnum::Enum toPixelCoordSpace) const
{
    PixelIndex pixelIndex(pixelIndexIn);
    if (fromPixelCoordSpace == toPixelCoordSpace) {
        return pixelIndex;
    }
    
    
    /*
     * First, convert index into full resolution image space with
     * origin at the bottom
     */
    switch (fromPixelCoordSpace) {
        case CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT:
        {
            RectangleTransform transform(m_fullResolutionLogicalRect,
                                         RectangleTransform::Origin::TOP_LEFT,
                                         m_fullResolutionPixelsRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT);
            if ( ! transform.isValid()) {
                CaretLogSevere("Creating rectangle transform FROM failed: "
                               + transform.getErrorMessage());
                return pixelIndex;
            }
            float x(0.0), y(0.0);
            transform.transformSourceToTarget(pixelIndex.getI(), pixelIndex.getJ(),
                                              x, y);
            pixelIndex.setI(static_cast<int64_t>(x));
            pixelIndex.setJ(static_cast<int64_t>(y));
        }
            break;
        case CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT:
            /*
             * Pixel is in full resolution image with origin bottom left
             */
            break;
        case CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT:
            /*
             * Convert to bottom
             */
            pixelIndex.setJ(m_fullResolutionPixelsRect.height() - pixelIndex.getJ());
            break;
        case CziPixelCoordSpaceEnum::PIXEL_BOTTOM_LEFT:
            /*
             * Convert to full resolution pixels bottom left
             */
            pixelIndex.setI(pixelIndex.getI() + m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() + m_pixelsRegionOfInterestRect.y());
            break;
        case CziPixelCoordSpaceEnum::PIXEL_TOP_LEFT:
            /*
             * Convert to full resolution pixels bottom left
             */
            pixelIndex.setI(pixelIndex.getI() + m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() + m_pixelsRegionOfInterestRect.y());
            /*
             * Convert to distance from top
             */
            pixelIndex.setJ(m_pixelsRegionOfInterestRect.height() - pixelIndex.getJ());
            break;
    }
    
    switch (toPixelCoordSpace) {
        case CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT:
        {
            RectangleTransform transform(m_fullResolutionPixelsRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT,
                                         m_fullResolutionLogicalRect,
                                         RectangleTransform::Origin::TOP_LEFT);
            if ( ! transform.isValid()) {
                CaretLogSevere("Creating rectangle transform TO (1) failed: "
                               + transform.getErrorMessage());
                return pixelIndex;
            }
            float x(0.0), y(0.0);
            transform.transformSourceToTarget(pixelIndex.getI(), pixelIndex.getJ(),
                                              x, y);
            pixelIndex.setI(static_cast<int64_t>(x));
            pixelIndex.setJ(static_cast<int64_t>(y));
        }
            break;
        case CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT:
            /*
             * Pixel was transformed to this space so no additional transform needed
             */
            break;
        case CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT:
            /*
             * Offset from top
             */
            pixelIndex.setJ(m_fullResolutionPixelsRect.height() - pixelIndex.getJ());
            break;
        case CziPixelCoordSpaceEnum::PIXEL_BOTTOM_LEFT:
        {
            /*
             * Origin is from image origin bottom left
             */
            pixelIndex.setI(pixelIndex.getI() - m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() - m_pixelsRegionOfInterestRect.y());

            QRectF sourceRect(0, 0, m_pixelsRegionOfInterestRect.width(), m_pixelsRegionOfInterestRect.height());
            RectangleTransform transform(sourceRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT,
                                         m_pixelsRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT);
            if ( ! transform.isValid()) {
                CaretLogSevere("Creating rectangle transform TO (2) failed: "
                               + transform.getErrorMessage());
                return pixelIndex;
            }
            float x(0.0), y(0.0);
            transform.transformSourceToTarget(pixelIndex.getI(), pixelIndex.getJ(),
                                              x, y);
            pixelIndex.setI(static_cast<int64_t>(x));
            pixelIndex.setJ(static_cast<int64_t>(y));
        }
            break;
        case CziPixelCoordSpaceEnum::PIXEL_TOP_LEFT:
        {
            /*
             * Origin is from image origin bottom left
             */
            pixelIndex.setI(pixelIndex.getI() - m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() - m_pixelsRegionOfInterestRect.y());

            QRectF sourceRect(0, 0, m_pixelsRegionOfInterestRect.width(), m_pixelsRegionOfInterestRect.height());
            RectangleTransform transform(sourceRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT,
                                         m_pixelsRect,
                                         RectangleTransform::Origin::TOP_LEFT);
            if ( ! transform.isValid()) {
                CaretLogSevere("Creating rectangle transform TO (2) failed: "
                               + transform.getErrorMessage());
                return pixelIndex;
            }
            float x(0.0), y(0.0);
            transform.transformSourceToTarget(pixelIndex.getI(), pixelIndex.getJ(),
                                              x, y);
            pixelIndex.setI(static_cast<int64_t>(x));
            pixelIndex.setJ(static_cast<int64_t>(y));
        }
            break;
    }

    return pixelIndex;
}

/**
 * @return True if the given pixel index is valid for the image in CziPixelCoordSpaceEnum::PIXEL_BOTTOM_LEFT
 * @param pixelIndex
 *     Image of pixel
 */
bool
CziImage::isPixelIndexValid(const PixelIndex& pixelIndex) const
{
    const QPoint point(pixelIndex.getI(),
                       pixelIndex.getJ());
    if (m_pixelsRegionOfInterestRect.contains(point)) {
        return true;
    }
    return false;
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param filename
 *    Name of CZI file
 * @param pixelIndexOriginAtTop
 *     Image of pixel in FULL RES image with origin top left
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
CziImage::getPixelIdentificationText(const AString& filename,
                                     const PixelIndex& pixelIndexOriginAtTop,
                                     std::vector<AString>& columnOneTextOut,
                                     std::vector<AString>& columnTwoTextOut,
                                     std::vector<AString>& toolTipTextOut) const
{
    columnOneTextOut.push_back("Filename");
    columnTwoTextOut.push_back(filename);
    
    uint8_t rgba[4];
    const bool rgbaValidFlag = getImagePixelRGBA(pixelIndexOriginAtTop,
                                                 rgba);
    const AString rgbaText("RGBA ("
                           + (rgbaValidFlag
                              ? AString::fromNumbers(rgba, 4, ",")
                              : "Unknown")
                           + ")");
    columnOneTextOut.push_back(rgbaText);
    toolTipTextOut.push_back(rgbaText);
    
    const AString pixelText("Pixel IJ ("
                            + AString::number(pixelIndexOriginAtTop.getI())
                            + ","
                            + AString::number(pixelIndexOriginAtTop.getJ())
                            + ")");
    columnTwoTextOut.push_back(pixelText);
    toolTipTextOut.push_back(pixelText);
    
    const PixelCoordinate pixelsSize(m_parentCziImageFile->getPixelSizeInMillimeters());
    const float pixelX(pixelIndexOriginAtTop.getI() * pixelsSize.getX());
    const float pixelY(pixelIndexOriginAtTop.getJ() * pixelsSize.getY());
    columnOneTextOut.push_back("");
    const AString mmText("Pixel XY ("
                         + AString::number(pixelX, 'f', 3)
                         + "mm,"
                         + AString::number(pixelY, 'f', 3)
                         + "mm)");
    columnTwoTextOut.push_back(mmText);
    toolTipTextOut.push_back(mmText);
}

/**
 * Get the pixel RGBA at the given pixel I and J.
 *
 * @param pixelIndexOriginAtTop
 *     Image of pixel in FULL RES image with origin top left
 * @param pixelRGBAOut
 *     RGBA at Pixel I, J
 * @return
 *     True if valid, else false.
 */
bool
CziImage::getImagePixelRGBA(const PixelIndex& pixelIndexOriginAtTop,
                            uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = 0;
    pixelRGBAOut[1] = 0;
    pixelRGBAOut[2] = 0;
    pixelRGBAOut[3] = 0;
    
    /* COULD just read the pixel from the CZI file */
    
    const QImage* image = m_image.get();
    
    if (image != NULL) {
        /*
         * Transform the full-resolution pixel index to space for this image
         */
        const PixelIndex pixelIndex = transformPixelIndexToSpace(pixelIndexOriginAtTop,
                                                                 CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT,
                                                                 CziPixelCoordSpaceEnum::PIXEL_TOP_LEFT);
        const int32_t w = image->width();
        const int32_t h = image->height();
        
        const int64_t pixelI(pixelIndex.getI());
        const int64_t pixelJ(pixelIndex.getJ());
        
        if ((pixelI >= 0)
            && (pixelI < w)
            && (pixelJ >= 0)
            && (pixelJ < h)) {
            const QRgb rgb = image->pixel(pixelI,
                                          pixelJ);
            pixelRGBAOut[0] = static_cast<uint8_t>(qRed(rgb));
            pixelRGBAOut[1] = static_cast<uint8_t>(qGreen(rgb));
            pixelRGBAOut[2] = static_cast<uint8_t>(qBlue(rgb));
            pixelRGBAOut[3] = static_cast<uint8_t>(qAlpha(rgb));
            return true;
        }
        else {
            const AString msg("Pixel Index: "
                              + pixelIndex.toString()
                              + " but valid max I="
                              + AString::number(w - 1)
                              + ", max J="
                              + AString::number(h - 1));
            CaretLogSevere(msg);
        }
    }

    return false;
}

/**
 * @return The graphics primitive for drawing the image as a texture in media drawing model.
 */
GraphicsPrimitiveV3fT2f*
CziImage::getGraphicsPrimitiveForMediaDrawing() const
{
    if (m_image == NULL) {
        return NULL;
    }
    
    if (m_graphicsPrimitiveForMediaDrawing == NULL) {
        std::vector<uint8_t> bytesRGBA;
        int32_t width(0);
        int32_t height(0);
        
        /*
         * If image is too big for OpenGL texture limits, scale image to acceptable size
         */
        const int32_t maxTextureWidthHeight = GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension();
        if (maxTextureWidthHeight > 0) {
            const int32_t excessWidth(m_image->width() - maxTextureWidthHeight);
            const int32_t excessHeight(m_image->height() - maxTextureWidthHeight);
            if ((excessWidth > 0)
                || (excessHeight > 0)) {
                if (excessWidth > excessHeight) {
                    CaretLogWarning(m_parentCziImageFile->getFileName()
                                    + " is too big for texture.  Maximum width/height is: "
                                    + AString::number(maxTextureWidthHeight)
                                    + " Image Width: "
                                    + AString::number(m_image->width())
                                    + " Image Height: "
                                    + AString::number(m_image->height()));
                }
            }
        }
        
        /*
         * Some images may use a color table so convert images
         * if there are not in preferred format prior to
         * getting colors of pixels
         */
        bool validRGBA(false);
        if (m_image->format() != QImage::Format_ARGB32) {
            QImage image = m_image->convertToFormat(QImage::Format_ARGB32);
            if (! image.isNull()) {
                ImageFile convImageFile;
                convImageFile.setFromQImage(image);
                validRGBA = convImageFile.getImageBytesRGBA(ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                                            bytesRGBA,
                                                            width,
                                                            height);
            }
        }
        else {
            validRGBA = ImageFile::getImageBytesRGBA(m_image.get(),
                                                     ImageFile::IMAGE_DATA_ORIGIN_AT_BOTTOM,
                                                     bytesRGBA,
                                                     width,
                                                     height);
        }
        
        if (validRGBA) {
            const std::array<float, 4> textureBorderColorRGBA { 0.0, 0.0, 0.0, 0.0 };
            GraphicsPrimitiveV3fT2f* primitive = GraphicsPrimitive::newPrimitiveV3fT2f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                                       &bytesRGBA[0],
                                                                                       width,
                                                                                       height,
                                                                                       GraphicsPrimitive::TextureWrappingType::CLAMP,
                                                                                       GraphicsPrimitive::TextureMipMappingType::ENABLED,
                                                                                       GraphicsTextureMagnificationFilterEnum::LINEAR,
                                                                                       GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR,
                                                                                       textureBorderColorRGBA);
            
            
            PixelIndex logicalBottomLeft(static_cast<float>(m_logicalRect.x()),
                                         static_cast<float>(m_logicalRect.y() + m_logicalRect.height()),
                                         0.0f);
            const PixelIndex pixelBottomLeft = transformPixelIndexToSpace(logicalBottomLeft,
                                                                          CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT,
                                                                          CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT);
            PixelIndex logicalTopRight(static_cast<float>(m_logicalRect.x() + m_logicalRect.width()),
                                       static_cast<float>(m_logicalRect.y()),
                                       0.0f);
            const PixelIndex pixelTopRight = transformPixelIndexToSpace(logicalTopRight,
                                                                        CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT,
                                                                        CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT);

            /*
             * Coordinates at EDGE of the pixels
             * in CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT
             */
            const float minX = pixelBottomLeft.getI();
            const float maxX = pixelTopRight.getI();
            const float minY = pixelBottomLeft.getJ();
            const float maxY = pixelTopRight.getJ();
            
            /*
             * A Triangle Strip (consisting of two triangles) is used
             * for drawing the image.
             * The order of the vertices in the triangle strip is
             * Top Left, Bottom Left, Top Right, Bottom Right.
             */
            const float minTextureST(0.0);
            const float maxTextureST(1.0);
            primitive->addVertex(minX, maxY, minTextureST, maxTextureST);  /* Top Left */
            primitive->addVertex(minX, minY, minTextureST, minTextureST);  /* Bottom Left */
            primitive->addVertex(maxX, maxY, maxTextureST, maxTextureST);  /* Top Right */
            primitive->addVertex(maxX, minY, maxTextureST, minTextureST);  /* Bottom Right */
            
            m_graphicsPrimitiveForMediaDrawing.reset(primitive);
        }
    }
    
    return m_graphicsPrimitiveForMediaDrawing.get();
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
CziImage::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "CziImage",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
CziImage::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

