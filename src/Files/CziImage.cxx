
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
 *    Logical Rectangle for the full-resolution source image that defines the coordinates of the primitive
 * @param imageDataLogicalRect
 *    Logical rectangle defining region of source image that was read from the file that results
 * @param resolutionChangeMode
 *    Resolution change mode that created this image
 * @param resolutionChangeModeLevel
 *    Level from resolution change mode that created this image
 */
CziImage::CziImage(const CziImageFile* parentCziImageFile,
                   QImage* image,
                   const QRectF& fullResolutionLogicalRect,
                   const QRectF& imageDataLogicalRect,
                   const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                   const int32_t resolutionChangeModeLevel)
: CaretObject(),
m_parentCziImageFile(parentCziImageFile),
m_image(image),
m_fullResolutionLogicalRect(fullResolutionLogicalRect),
m_imageDataLogicalRect(imageDataLogicalRect),
m_resolutionChangeMode(resolutionChangeMode),
m_resolutionChangeModeLevel(resolutionChangeModeLevel)

{
    CaretAssert(image);

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_fullResolutionPixelsRect = QRectF(0, 0, m_fullResolutionLogicalRect.width(), m_fullResolutionLogicalRect.height());

    m_imagePixelsRect = QRectF(0, 0, m_image->width(), m_image->height());
    
    const PixelIndex logBotLeft(m_imageDataLogicalRect.x(),
                                m_imageDataLogicalRect.y() + m_imageDataLogicalRect.height(),
                                0.0f);
    const PixelIndex pixelBotLeft = transformPixelIndexToSpace(logBotLeft,
                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT,
                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT);
    
    /* Below only used in transform transformPixelIndexToSpace() */
    m_pixelsRegionOfInterestRect = QRectF(pixelBotLeft.getI(),
                                          pixelBotLeft.getJ(),
                                          m_imageDataLogicalRect.width(),
                                          m_imageDataLogicalRect.height());
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
        case CziPixelCoordSpaceEnum::IMAGE_DATA_PIXEL_BOTTOM_LEFT:
            /*
             * Convert to full resolution pixels bottom left
             */
            pixelIndex.setI(pixelIndex.getI() + m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() + m_pixelsRegionOfInterestRect.y());
            break;
        case CziPixelCoordSpaceEnum::IMAGE_DATA_PIXEL_TOP_LEFT:
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
        case CziPixelCoordSpaceEnum::IMAGE_DATA_PIXEL_BOTTOM_LEFT:
        {
            /*
             * Origin is from image origin bottom left
             */
            pixelIndex.setI(pixelIndex.getI() - m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() - m_pixelsRegionOfInterestRect.y());

            QRectF sourceRect(0, 0, m_pixelsRegionOfInterestRect.width(), m_pixelsRegionOfInterestRect.height());
            RectangleTransform transform(sourceRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT,
                                         m_imagePixelsRect,
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
        case CziPixelCoordSpaceEnum::IMAGE_DATA_PIXEL_TOP_LEFT:
        {
            /*
             * Origin is from image origin bottom left
             */
            pixelIndex.setI(pixelIndex.getI() - m_pixelsRegionOfInterestRect.x());
            pixelIndex.setJ(pixelIndex.getJ() - m_pixelsRegionOfInterestRect.y());

            QRectF sourceRect(0, 0, m_pixelsRegionOfInterestRect.width(), m_pixelsRegionOfInterestRect.height());
            RectangleTransform transform(sourceRect,
                                         RectangleTransform::Origin::BOTTOM_LEFT,
                                         m_imagePixelsRect,
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
    const float imageWidth(m_image->width());
    const float imageHeight(m_image->height());
    
    if ((imageWidth > 0.0)
        && (imageHeight > 0.0)) {
        const int64_t pixelI(pixelIndex.getI());
        const int64_t pixelJ(pixelIndex.getJ());
        
        if ((pixelI >= 0)
            && (pixelI < imageWidth)
            && (pixelJ >= 0)
            && (pixelJ < imageHeight)) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return True if the given pixel logical index is valid for this image
 * @param pixelLogicalIndex
 *    Pixel logical index
 */
bool
CziImage::isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const
{
    const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    return isPixelIndexValid(pixelIndex);
}

/**
 * Get the identification text for the pixel at the given pixel logical index
 * @param filename
 *    Name of CZI file
 * @param pixelLogicalIndex
 *     Pixel logical index
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
CziImage::getPixelIdentificationText(const AString& filename,
                                     const PixelLogicalIndex& pixelLogicalIndex,
                                     std::vector<AString>& columnOneTextOut,
                                     std::vector<AString>& columnTwoTextOut,
                                     std::vector<AString>& toolTipTextOut) const
{
    
    uint8_t rgba[4];
    const bool rgbaValidFlag = getPixelRGBA(pixelLogicalIndex,
                                            rgba);
    const AString rgbaText("RGBA ("
                           + (rgbaValidFlag
                              ? AString::fromNumbers(rgba, 4, ",")
                              : "Unknown")
                           + ")");
    
    const PixelIndex pixelIndex(m_parentCziImageFile->pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    const int64_t fullResPixelI(pixelIndex.getI());
    const int64_t fullResPixelJ(pixelIndex.getJ());
    const AString pixelText("Pixel IJ ("
                            + AString::number(fullResPixelI)
                            + ","
                            + AString::number(fullResPixelJ)
                            + ")");
    
    const AString logicalText("Logical IJ ("
                              + AString::number(pixelLogicalIndex.getI(), 'f', 3)
                              + ","
                              + AString::number(pixelLogicalIndex.getJ(), 'f', 3)
                              + ")");
    
    const PixelCoordinate pixelsSize(m_parentCziImageFile->getPixelSizeInMillimeters());
    const float pixelX(fullResPixelI * pixelsSize.getX());
    const float pixelY(fullResPixelJ * pixelsSize.getY());
    const AString mmText("Pixel XY ("
                         + AString::number(pixelX, 'f', 3)
                         + "mm,"
                         + AString::number(pixelY, 'f', 3)
                         + "mm)");
    
    
    
    columnOneTextOut.push_back("Filename");
    columnTwoTextOut.push_back(filename);
    
    columnOneTextOut.push_back(pixelText);
    columnTwoTextOut.push_back(logicalText);

    columnOneTextOut.push_back(rgbaText);
    columnTwoTextOut.push_back(mmText);

    toolTipTextOut.push_back(rgbaText);
    toolTipTextOut.push_back(pixelText);
    toolTipTextOut.push_back(logicalText);
    toolTipTextOut.push_back(mmText);
}

/**
 * @return A pixel index converted from a pixel logical index.
 * @param pixelLogicalIndex
 *    The logical pixel index.
 * Note: the "logical width/height" is often much greater than the pixel width/height.
 * Pixel index can be used to index into the image pixel data.
 */
PixelIndex
CziImage::pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const
{
    const int64_t logicalOffsetI(pixelLogicalIndex.getI() - m_imageDataLogicalRect.x());
    const int64_t logicalOffsetJ(pixelLogicalIndex.getJ() - m_imageDataLogicalRect.y());
    
    const float logicalWidth(m_imageDataLogicalRect.width());
    const float logicalHeight(m_imageDataLogicalRect.height());
    
    const float imageWidth(m_image->width());
    const float imageHeight(m_image->height());
    
    PixelIndex pixelIndex;
    
    if ((logicalWidth > 0.0)
        && (logicalHeight > 0.0)) {
        const int64_t pixelI((logicalOffsetI / logicalWidth)  * imageWidth);
        const int64_t pixelJ((logicalOffsetJ / logicalHeight) * imageHeight);
        
        pixelIndex.setI(pixelI);
        pixelIndex.setJ(pixelJ);
        pixelIndex.setK(0);
    }
    
    return pixelIndex;
}

/**
 * @return A pixel logical index converted from a pixel index.
 * @param pixelIndex
 *    The  pixel index.
 */
PixelLogicalIndex
CziImage::pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const
{
    const int64_t imageOffsetI(pixelIndex.getI());
    const int64_t imageOffsetJ(pixelIndex.getJ());
    
    const float logicalWidth(m_imageDataLogicalRect.width());
    const float logicalHeight(m_imageDataLogicalRect.height());
    
    const float imageWidth(m_image->width());
    const float imageHeight(m_image->height());
    
    PixelLogicalIndex pixelLogicalIndex;
    
    if ((imageWidth > 0.0)
        && (imageWidth > 0.0)) {
        const int64_t pixelLogicalI(((imageOffsetI / imageWidth) * logicalWidth) + m_imageDataLogicalRect.x());
        const int64_t pixelLogicalJ(((imageOffsetJ / imageHeight) * logicalHeight) + m_imageDataLogicalRect.y());
        
        pixelLogicalIndex.setI(pixelLogicalI);
        pixelLogicalIndex.setJ(pixelLogicalJ);
        pixelLogicalIndex.setK(0);
    }

    return pixelLogicalIndex;
}

/**
 * Get the pixel RGBA at the pixel logical index
 *
 * @param pixelLogicalIndex
 *     Pixel logical index
 * @param pixelRGBAOut
 *     RGBA at Pixel I, J
 * @return
 *     True if valid, else false.
 */
bool
CziImage::getPixelRGBA(const PixelLogicalIndex& pixelLogicalIndex,
                       uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = 0;
    pixelRGBAOut[1] = 0;
    pixelRGBAOut[2] = 0;
    pixelRGBAOut[3] = 0;
    
    /* COULD just read the pixel from the CZI file */
    
    const QImage* image = m_image.get();
    
    if (image != NULL) {
        const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
        if (isPixelIndexValid(pixelIndex)) {
            const int64_t pixelI(pixelIndex.getI());
            const int64_t pixelJ(pixelIndex.getJ());
            const QRgb rgb = image->pixel(pixelI,
                                          pixelJ);
            pixelRGBAOut[0] = static_cast<uint8_t>(qRed(rgb));
            pixelRGBAOut[1] = static_cast<uint8_t>(qGreen(rgb));
            pixelRGBAOut[2] = static_cast<uint8_t>(qBlue(rgb));
            pixelRGBAOut[3] = static_cast<uint8_t>(qAlpha(rgb));
            return true;
        }
        else {
            const AString msg("Pixel Logical Index: "
                              + pixelLogicalIndex.toString()
                              + " but valid rect is "
                              + CziUtilities::qRectToString(m_imageDataLogicalRect));
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
                                                                                       GraphicsPrimitive::TextureWrappingType::CLAMP_TO_BORDER,
                                                                                       GraphicsPrimitive::TextureMipMappingType::ENABLED,
                                                                                       GraphicsTextureMagnificationFilterEnum::LINEAR,
                                                                                       GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR,
                                                                                       textureBorderColorRGBA);
            
            /*
             * The Geometric Coordinates are those of the full-resolution.
             * The Texture Coordinates map to the subregion that contains
             * the image data.  This allows the same viewing transformations
             * for all CZI images since all primitives use the same
             * geometric coordinates.
             */
            const float minX = m_fullResolutionLogicalRect.x();
            const float maxX = m_fullResolutionLogicalRect.x() + m_fullResolutionLogicalRect.width();
            const float minY = m_fullResolutionLogicalRect.y();
            const float maxY = m_fullResolutionLogicalRect.y() + m_fullResolutionLogicalRect.height();

            const float imageLogicalWidth(m_imageDataLogicalRect.width());
            const float imageLogicalHeight(m_imageDataLogicalRect.height());
            
            const float textureMaxS2(((m_fullResolutionLogicalRect.right()
                                       - m_imageDataLogicalRect.right()) / imageLogicalWidth)
                                     + 1.0);
            const float textureMinS2(((m_fullResolutionLogicalRect.left()
                                       - m_imageDataLogicalRect.left()) / imageLogicalWidth)
                                     - 0.0);
            const float textureMinT2(((m_imageDataLogicalRect.top()
                                       - m_fullResolutionLogicalRect.top()) / imageLogicalHeight)
                                     + 1.0);
            const float textureMaxT2(((m_imageDataLogicalRect.bottom()
                                       - m_fullResolutionLogicalRect.bottom()) / imageLogicalHeight)
                                     - 0.0);
            
            /*
             * A Triangle Strip (consisting of two triangles) is used
             * for drawing the image.
             * The order of the vertices in the triangle strip is
             * Top Left, Bottom Left, Top Right, Bottom Right.
             */
            primitive->addVertex(minX, minY, textureMinS2, textureMinT2);  /* Top Left */
            primitive->addVertex(minX, maxY, textureMinS2, textureMaxT2);  /* Bottom Left */
            primitive->addVertex(maxX, minY, textureMaxS2, textureMinT2);  /* Top Right */
            primitive->addVertex(maxX, maxY, textureMaxS2, textureMaxT2);  /* Bottom Right */

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

