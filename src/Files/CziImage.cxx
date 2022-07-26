
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

static bool debugFlag(false);

/**
 * Constructor
 * @param parentCziImageFile
 *    The parent CZI Image File
 * @param imageName
 *    Name for image that may be used when debugging
 * @param qimage
 *    The QImage instance
 * @param fullResolutionLogicalRect
 *    Logical Rectangle for the full-resolution source image that defines the coordinates of the primitive
 * @param imageDataLogicalRect
 *    Logical rectangle defining region of source image that was read from the file that results
 */
CziImage::CziImage(const CziImageFile* parentCziImageFile,
                   const AString& imageName,
                   QImage* qimage,
                   const QRectF& fullResolutionLogicalRect,
                   const QRectF& imageDataLogicalRect)
: CaretObject(),
m_parentCziImageFile(parentCziImageFile),
m_imageName(imageName),
m_imageStorageFormat(ImageStorageFormat::Q_IMAGE),
m_qimageData(qimage),
m_cziImageData(NULL),
m_imageWidth((qimage != NULL) ? qimage->width() : 0),
m_imageHeight((qimage != NULL) ? qimage->height() : 0),
m_fullResolutionLogicalRect(fullResolutionLogicalRect),
m_imageDataLogicalRect(imageDataLogicalRect)
{
    CaretAssert(qimage);

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_fullResolutionPixelsRect = QRectF(0, 0, m_fullResolutionLogicalRect.width(), m_fullResolutionLogicalRect.height());

    m_imagePixelsRect = QRectF(0, 0, m_imageWidth, m_imageHeight);
}

/**
 * Constructor
 * @param parentCziImageFile
 *    The parent CZI Image File
 * @param imageName
 *    Name for image that may be used when debugging
 * @param image
 *    The QImage instance
 * @param fullResolutionLogicalRect
 *    Logical Rectangle for the full-resolution source image that defines the coordinates of the primitive
 * @param imageDataLogicalRect
 *    Logical rectangle defining region of source image that was read from the file that results
 */
CziImage::CziImage(const CziImageFile* parentCziImageFile,
                   const AString& imageName,
                   std::shared_ptr<libCZI::IBitmapData>& cziImageData,
                   const QRectF& fullResolutionLogicalRect,
                   const QRectF& imageDataLogicalRect)
: CaretObject(),
m_parentCziImageFile(parentCziImageFile),
m_imageName(imageName),
m_imageStorageFormat(ImageStorageFormat::CZI_IMAGE),
m_qimageData((QImage*)NULL),
m_cziImageData(cziImageData),
m_imageWidth((cziImageData != NULL) ? cziImageData->GetWidth() : 0),
m_imageHeight((cziImageData != NULL) ? cziImageData->GetHeight() : 0),
m_fullResolutionLogicalRect(fullResolutionLogicalRect),
m_imageDataLogicalRect(imageDataLogicalRect)
{
    CaretAssert(cziImageData.get());

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_fullResolutionPixelsRect = QRectF(0, 0, m_fullResolutionLogicalRect.width(), m_fullResolutionLogicalRect.height());
    
    m_imagePixelsRect = QRectF(0, 0, m_imageWidth, m_imageHeight);
}

/**
 * Destructor.
 */
CziImage::~CziImage()
{
    //std::cout << "Deleting CZI Image: " << m_imageName << std::endl;
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
 * @return True if all possible image data is loaded for the scene containing this image
 */
bool
CziImage::isEntireImageLoaded() const
{
    const float tol(2.0);
    if (CziUtilities::equalWithTolerance(m_fullResolutionLogicalRect,
                                         m_imageDataLogicalRect,
                                         tol)) {
        
    }
    return false;
}

/**
 * @return "logical rectangle" defines the region of this image in the source file.
 */
QRectF
CziImage::getImageDataLogicalRect() const
{
    return m_imageDataLogicalRect;
}

/**
 * @return full resolution rectangle defines the full bounds of the original image in the file
 */
QRectF
CziImage::getFullResolutionLogicalRect() const
{
    return m_fullResolutionLogicalRect;
}


/**
 * @return Width of image
 */
int32_t
CziImage::getWidth() const
{
    return m_imageWidth;
}

/*
 * @return Height of image
 */
int32_t
CziImage::getHeight() const
{
    return m_imageHeight;
}

/**
 * @return True if the given pixel index is valid for the image in CziPixelCoordSpaceEnum::PIXEL_BOTTOM_LEFT
 * @param pixelIndex
 *     Image of pixel
 */
bool
CziImage::isPixelIndexValid(const PixelIndex& pixelIndex) const
{
    const float imageWidth(m_imageWidth);
    const float imageHeight(m_imageHeight);
    
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
    
    const float imageWidth(m_imageWidth);
    const float imageHeight(m_imageHeight);
    
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
    
    const float imageWidth(m_imageWidth);
    const float imageHeight(m_imageHeight);
    
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
 * Get the pixel RGBA at the pixel logical index from the image data
 *
 * @param pixelLogicalIndex
 *     Pixel logical index
 * @param pixelRGBAOut
 *     RGBA at Pixel I, J
 * @return
 *     True if valid, else false.
 */
bool
CziImage::getImageDataPixelRGBA(const PixelLogicalIndex& pixelLogicalIndex,
                                uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = 0;
    pixelRGBAOut[1] = 0;
    pixelRGBAOut[2] = 0;
    pixelRGBAOut[3] = 0;
        
    const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    if (isPixelIndexValid(pixelIndex)) {
        const int64_t pixelI(pixelIndex.getI());
        const int64_t pixelJ(pixelIndex.getJ());
        
        switch (m_imageStorageFormat) {
            case ImageStorageFormat::INVALID:
                CaretAssert(0);
                break;
            case ImageStorageFormat::CZI_IMAGE:
            {
                const int32_t bytesPerPixel(3);
                
                const libCZI::BitmapLockInfo bitmapInfo(m_cziImageData->Lock());
                const int32_t rowStride(bitmapInfo.stride);
                const int32_t rowOffset(rowStride * pixelJ);
                const int32_t pixelOffset((pixelI * bytesPerPixel)
                                          + rowOffset);
                CaretAssert(pixelOffset < static_cast<int32_t>(bitmapInfo.size));

                /* Data is BGR ! */
                const uint8_t* dataPtr(static_cast<uint8_t*>(bitmapInfo.ptrDataRoi));
                pixelRGBAOut[0] = static_cast<uint8_t>(dataPtr[pixelOffset + 2]);
                pixelRGBAOut[1] = static_cast<uint8_t>(dataPtr[pixelOffset + 1]);
                pixelRGBAOut[2] = static_cast<uint8_t>(dataPtr[pixelOffset + 0]);
                pixelRGBAOut[3] = static_cast<uint8_t>(255);
                
                m_cziImageData->Unlock();
                return true;
            }
                break;
            case ImageStorageFormat::Q_IMAGE:
                if (m_qimageData) {
                    QRgb rgb = m_qimageData->pixel(pixelI,
                                              pixelJ);
                    pixelRGBAOut[0] = static_cast<uint8_t>(qRed(rgb));
                    pixelRGBAOut[1] = static_cast<uint8_t>(qGreen(rgb));
                    pixelRGBAOut[2] = static_cast<uint8_t>(qBlue(rgb));
                    pixelRGBAOut[3] = static_cast<uint8_t>(qAlpha(rgb));
                    return true;
                }
                break;
        }
    }
    else {
        const AString msg("Pixel Logical Index: "
                          + pixelLogicalIndex.toString()
                          + " but valid rect is "
                          + CziUtilities::qRectToString(m_imageDataLogicalRect));
        CaretLogSevere(msg);
    }

    return false;
}

/**
 * @return The graphics primitive for drawing the image as a texture in media drawing model.
 */
GraphicsPrimitiveV3fT2f*
CziImage::getGraphicsPrimitiveForMediaDrawing() const
{
    switch (m_imageStorageFormat) {
        case ImageStorageFormat::INVALID:
            CaretAssertMessage(0, "Image storage format is invalid");
            return NULL;
            break;
        case ImageStorageFormat::CZI_IMAGE:
            break;
        case ImageStorageFormat::Q_IMAGE:
            if (m_qimageData == NULL) {
                CaretAssertMessage(0, "QImage is invalid");
                return NULL;
            }
            break;
    }
    
    if ((m_imageWidth <= 0)
        || (m_imageHeight) <= 0) {
        return NULL;
    }
    
    if (m_graphicsPrimitiveForMediaDrawing == NULL) {
        int32_t width(0);
        int32_t height(0);
        
        /*
         * If image is too big for OpenGL texture limits, scale image to acceptable size
         */
        const int32_t maxTextureWidthHeight = GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension();
        if (maxTextureWidthHeight > 0) {
            const int32_t excessWidth(m_imageWidth - maxTextureWidthHeight);
            const int32_t excessHeight(m_imageHeight - maxTextureWidthHeight);
            if ((excessWidth > 0)
                || (excessHeight > 0)) {
                if (excessWidth > excessHeight) {
                    CaretLogWarning(m_parentCziImageFile->getFileName()
                                    + " is too big for texture.  Maximum width/height is: "
                                    + AString::number(maxTextureWidthHeight)
                                    + " Image Width: "
                                    + AString::number(m_imageWidth)
                                    + " Image Height: "
                                    + AString::number(m_imageHeight));
                }
            }
        }
        
        uint8_t* ptrBytesRGBA(NULL);
        bool validRGBA(false);
        
        libCZI::BitmapLockInfo cziBitmapLockInfo;
        bool cziLockFlag(false);
        GraphicsTextureSettings::PixelFormatType pixelFormatType(GraphicsTextureSettings::PixelFormatType::NONE);
        
        GraphicsTextureSettings::PixelOrigin pixelOrigin(GraphicsTextureSettings::PixelOrigin::BOTTOM_LEFT);
        
        /*
         * Image formats may pad each row of data so that the row is an even number (or multiple of 4/8)
         * length with padding at the end of the row.
         */
        int32_t rowStride(-1);
        
        switch (m_imageStorageFormat) {
            case ImageStorageFormat::INVALID:
                CaretAssertMessage(0, "Image storage format is invalid");
                return NULL;
                break;
            case ImageStorageFormat::CZI_IMAGE:
                if (m_cziImageData->GetPixelType() == libCZI::PixelType::Bgr24) {
                    cziBitmapLockInfo = m_cziImageData->Lock();
                    pixelFormatType = GraphicsTextureSettings::PixelFormatType::BGR;
                    pixelOrigin     = GraphicsTextureSettings::PixelOrigin::TOP_LEFT;
                    cziLockFlag = true;
                    
                    if (cziBitmapLockInfo.size > 0) {
                        validRGBA = true;
                        ptrBytesRGBA = static_cast<uint8_t*>(cziBitmapLockInfo.ptrData);
                        width        = m_cziImageData->GetWidth();
                        height       = m_cziImageData->GetHeight();
                        rowStride    = cziBitmapLockInfo.stride;
                    }
                }
                break;
            case ImageStorageFormat::Q_IMAGE:
                if (m_qimageData->format() != QImage::Format_ARGB32) {
                    m_qimageData->convertTo(QImage::Format_ARGB32);
                }
                validRGBA = (m_qimageData->format() == QImage::Format_ARGB32);
                if (validRGBA) {
                    pixelFormatType = GraphicsTextureSettings::PixelFormatType::RGBA;
                    pixelOrigin     = GraphicsTextureSettings::PixelOrigin::BOTTOM_LEFT;
                    rowStride       = width * 4; /* RGBA */
                }
                break;
        }

        
        if (validRGBA) {
            CaretAssert(ptrBytesRGBA);
            CaretAssert(width > 0);
            CaretAssert(height > 0);
            const std::array<float, 4> textureBorderColorRGBA { 0.0, 0.0, 0.0, 0.0 };
            
            GraphicsTextureSettings textureSettings;
            
            switch (m_imageStorageFormat) {
                case ImageStorageFormat::INVALID:
                    CaretAssert(0);
                    break;
                case ImageStorageFormat::CZI_IMAGE:
                    textureSettings = GraphicsTextureSettings(ptrBytesRGBA,
                                                              width,
                                                              height,
                                                              1, /* slices */
                                                              GraphicsTextureSettings::DimensionType::FLOAT_STR_2D,
                                                              pixelFormatType,
                                                              pixelOrigin,
                                                              GraphicsTextureSettings::WrappingType::CLAMP_TO_BORDER,
                                                              GraphicsTextureSettings::MipMappingType::ENABLED,
                                                              GraphicsTextureSettings::CompressionType::DISABLED,
                                                              GraphicsTextureMagnificationFilterEnum::LINEAR,
                                                              GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR,
                                                              textureBorderColorRGBA);
                    break;
                case ImageStorageFormat::Q_IMAGE:
                    textureSettings = GraphicsTextureSettings(m_qimageData->constBits(),
                                                              width,
                                                              height,
                                                              1, /* slices */
                                                              GraphicsTextureSettings::DimensionType::FLOAT_STR_2D,
                                                              pixelFormatType,
                                                              pixelOrigin,
                                                              GraphicsTextureSettings::WrappingType::CLAMP_TO_BORDER,
                                                              GraphicsTextureSettings::MipMappingType::ENABLED,
                                                              GraphicsTextureSettings::CompressionType::DISABLED,
                                                              GraphicsTextureMagnificationFilterEnum::LINEAR,
                                                              GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR,
                                                              textureBorderColorRGBA);
                    break;
                    
            }
            GraphicsPrimitiveV3fT2f* primitive = GraphicsPrimitive::newPrimitiveV3fT2f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                                       textureSettings);
            
            /*
             * The Geometric Coordinates are always the full resolution logical
             * coordinates, regardless of the image data that is loaded.  So,
             * the panning and zooming does not need to change as new image data
             * is loaded for different regions.
             */
            const float minX = m_fullResolutionLogicalRect.x();
            const float maxX = m_fullResolutionLogicalRect.x() + m_fullResolutionLogicalRect.width();
            const float minY = m_fullResolutionLogicalRect.y();
            const float maxY = m_fullResolutionLogicalRect.y() + m_fullResolutionLogicalRect.height();
            
            const float imageLogicalWidth(m_imageDataLogicalRect.width());
            const float imageLogicalHeight(m_imageDataLogicalRect.height());
            
            /*
             * The image data may be only a sub-region of the full resolution area.  But, the
             * texture coordinates need to be set for the full resolution region as that is what
             * is used for the geometric coordinates.  So, in these instances, the texture coordinates
             * may be less than zero and/or greater than one.  The texture border color will be used
             * outside of the image data.
             *
             *  -------------------
             *  |                 |   Dots are image logical region
             *  |     .......     |   Lines are full resolution region
             *  |     .     .     |
             *  |     .......     |
             *  -------------------
             */
            const float textureMinS((m_fullResolutionLogicalRect.left() - m_imageDataLogicalRect.left())
                                     / imageLogicalWidth);
            const float textureMaxS((m_fullResolutionLogicalRect.right() - m_imageDataLogicalRect.left())
                                    / imageLogicalWidth);
            const float textureMinT((m_fullResolutionLogicalRect.top() - m_imageDataLogicalRect.top())
                                    / imageLogicalHeight);
            const float textureMaxT((m_fullResolutionLogicalRect.bottom() - m_imageDataLogicalRect.top())
                                    / imageLogicalHeight);

            if (debugFlag) {
                std::cout << "X: " << minX << ", " << maxX << "    Y: " << minY << ", " << maxY << std::endl;
                std::cout << "  Full Rect: " << CziUtilities::qRectToString(m_fullResolutionLogicalRect) << std::endl;
                std::cout << "  Img Rect:  " << CziUtilities::qRectToString(m_imageDataLogicalRect) << std::endl;
                std::cout << "  Txt S:     " << textureMinS << ", " << textureMaxS << std::endl;
                std::cout << "  Txt T:     " << textureMinT << ", " << textureMaxT << std::endl;
            }
            
            /*
             * A Triangle Strip (consisting of two triangles) is used
             * for drawing the image.
             * The order of the vertices in the triangle strip is
             * Top Left, Bottom Left, Top Right, Bottom Right.
             */
            primitive->addVertex(minX, minY, textureMinS, textureMinT);  /* Top Left */
            primitive->addVertex(minX, maxY, textureMinS, textureMaxT);  /* Bottom Left */
            primitive->addVertex(maxX, minY, textureMaxS, textureMinT);  /* Top Right */
            primitive->addVertex(maxX, maxY, textureMaxS, textureMaxT);  /* Bottom Right */

            m_graphicsPrimitiveForMediaDrawing.reset(primitive);

            if (debugFlag) {
                std::cout << "Loaded primitive: ";
                primitive->print();
                std::cout << std::endl << std::endl;
            }
        }
        
        if (cziLockFlag) {
            m_cziImageData->Unlock();
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

