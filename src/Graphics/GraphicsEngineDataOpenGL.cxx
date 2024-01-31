
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

#define __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__
#include "GraphicsEngineDataOpenGL.h"
#undef __GRAPHICS_ENGINE_DATA_OPEN_G_L_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DeveloperFlagsEnum.h"
#include "EventGraphicsOpenGLCreateBufferObject.h"
#include "EventGraphicsOpenGLCreateTextureName.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GraphicsOpenGLBufferObject.h"
#include "GraphicsOpenGLError.h"
#include "GraphicsOpenGLPolylineTriangles.h"
#include "GraphicsOpenGLTextureName.h"
#include "GraphicsPrimitive.h"
#include "GraphicsPrimitiveSelectionHelper.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsShape.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "Matrix4x4.h"

using namespace caret;


    
/**
 * \class caret::GraphicsEngineDataOpenGL 
 * \brief Contains data the OpenGL Graphics Engine may associate with primtive data.
 * \ingroup Graphics
 */

/**
 * Constructor.
 */
GraphicsEngineDataOpenGL::GraphicsEngineDataOpenGL()
: GraphicsEngineData()
{
    
}

/**
 * Destructor.
 */
GraphicsEngineDataOpenGL::~GraphicsEngineDataOpenGL()
{
    deleteBuffers();
}

/**
 * Delete the OpenGL buffer objects.
 */
void
GraphicsEngineDataOpenGL::deleteBuffers()
{
    if (m_textureImageDataName != NULL) {
        delete m_textureImageDataName;
        m_textureImageDataName = NULL;
    }
}

/**
 * Invalidate the coordinates after they have
 * changed in the graphics primitive.
 */
void
GraphicsEngineDataOpenGL::invalidateCoordinates()
{
    m_reloadCoordinatesFlag = true;
}

/**
 * Invalidate the colors after they have
 * changed in the graphics primitive.
 */
void
GraphicsEngineDataOpenGL::invalidateColors()
{
    m_reloadColorsFlag = true;
}

/**
 * Invalidate the texture coordinates after they have
 * changed in the graphics primitive.
 */
void
GraphicsEngineDataOpenGL::invalidateTextureCoordinates()
{
    m_reloadTextureCoordinatesFlag = true;
}


/**
 * Get the OpenGL Buffer Usage Hint from the primitive.
 *
 * @param primitiveUsageType
 *     The graphics primitive usage type
 * @return
 *     The OpenGL Buffer Usage Hint.
 */
GLenum
GraphicsEngineDataOpenGL::getOpenGLBufferUsageHint(const GraphicsPrimitive::UsageType primitiveUsageType) const
{
    GLenum usageHint = GL_STREAM_DRAW;
    switch (primitiveUsageType) {
        case GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_FEW_TIMES:
            usageHint = GL_STREAM_DRAW;
            break;
        case GraphicsPrimitive::UsageType::MODIFIED_ONCE_DRAWN_MANY_TIMES:
            usageHint = GL_STATIC_DRAW;
            break;
        case GraphicsPrimitive::UsageType::MODIFIED_MANY_DRAWN_MANY_TIMES:
            usageHint = GL_DYNAMIC_DRAW;
            break;
    }
    
    return usageHint;
}

/**
 * Load the coordinate buffer.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadCoordinateBuffer(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    GLenum usageHint = getOpenGLBufferUsageHint(primitive->getUsageTypeCoordinates());
    
    
    GLsizei coordinateCount = 0;
    switch (primitive->m_vertexDataType) {
        case GraphicsPrimitive::VertexDataType::FLOAT_XYZ:
            m_coordinateDataType = GL_FLOAT;
            m_coordinatesPerVertex = 3; /* X, Y, Z */
            
            coordinateCount = primitive->m_xyz.size();
            const GLuint xyzSizeBytes = coordinateCount * sizeof(float);
            CaretAssert(xyzSizeBytes > 0);
            const GLvoid* xyzDataPointer = (const GLvoid*)&primitive->m_xyz[0];
            
            /*
             * Coordinate buffer may have been created.
             * Coordinates may change but the number of coordinates does not change.
             */
            if (m_coordinateBufferObject == NULL) {
                EventGraphicsOpenGLCreateBufferObject createEvent;
                EventManager::get()->sendEvent(createEvent.getPointer());
                m_coordinateBufferObject.reset(createEvent.getOpenGLBufferObject());
                CaretAssert(m_coordinateBufferObject);
            }
            
            CaretAssert(m_coordinateBufferObject->getBufferObjectName());
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_coordinateBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         xyzSizeBytes,
                         xyzDataPointer,
                         usageHint);
            break;
    }
    
    if (m_coordinatesPerVertex > 0) {
        m_arrayIndicesCount = coordinateCount / m_coordinatesPerVertex;
    }
    else {
        m_arrayIndicesCount = 0;
    }
    
    m_reloadCoordinatesFlag = false;
}

/**
 * Load the normal vector buffer.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadNormalVectorBuffer(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    GLenum usageHint = getOpenGLBufferUsageHint(primitive->getUsageTypeNormals());
    
    
    switch (primitive->m_normalVectorDataType) {
        case GraphicsPrimitive::NormalVectorDataType::NONE:
            break;
        case GraphicsPrimitive::NormalVectorDataType::FLOAT_XYZ:
        {
            m_normalVectorDataType = GL_FLOAT;
            const GLuint normalSizeBytes = primitive->m_floatNormalVectorXYZ.size() * sizeof(float);
            CaretAssert(normalSizeBytes > 0);
            const GLvoid* normalDataPointer = (const GLvoid*)&primitive->m_floatNormalVectorXYZ[0];
            
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_normalVectorBufferObject.reset(createEvent.getOpenGLBufferObject());
            CaretAssert(m_normalVectorBufferObject->getBufferObjectName());
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_normalVectorBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         normalSizeBytes,
                         normalDataPointer,
                         usageHint);
        }
            break;
    }
}

/**
 * Load the color buffer.
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadColorBuffer(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    GLenum usageHint = getOpenGLBufferUsageHint(primitive->getUsageTypeColors());
    
    switch (primitive->m_colorDataType) {
        case GraphicsPrimitive::ColorDataType::NONE:
            break;
        case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
        {
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_colorBufferObject.reset(createEvent.getOpenGLBufferObject());
            CaretAssert(m_colorBufferObject->getBufferObjectName());
            
            m_componentsPerColor = 4;
            m_colorDataType = GL_FLOAT;
            
            const GLuint colorSizeBytes = primitive->m_floatRGBA.size() * sizeof(float);
            CaretAssert(colorSizeBytes > 0);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_floatRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_colorBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         usageHint);
        }
            break;
        case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
        {
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_colorBufferObject.reset(createEvent.getOpenGLBufferObject());
            CaretAssert(m_colorBufferObject->getBufferObjectName());
            
            m_componentsPerColor = 4;
            m_colorDataType = GL_UNSIGNED_BYTE;
            
            const GLuint colorSizeBytes = primitive->m_unsignedByteRGBA.size() * sizeof(uint8_t);
            CaretAssert(colorSizeBytes > 0);
            const GLvoid* colorDataPointer = (const GLvoid*)&primitive->m_unsignedByteRGBA[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_colorBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         colorSizeBytes,
                         colorDataPointer,
                         usageHint);
        }
            break;
    }
    
    m_reloadColorsFlag = false;
}

/**
 * Load the texture coordinate buffer.
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadTextureCoordinateBuffer(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
 
    GLenum usageHint = getOpenGLBufferUsageHint(primitive->getUsageTypeTextureCoordinates());
    
    const GraphicsTextureSettings& textureSettings(primitive->getTextureSettings());
    
    switch (textureSettings.getDimensionType()) {
        case GraphicsTextureSettings::DimensionType::FLOAT_STR_2D:
        case GraphicsTextureSettings::DimensionType::FLOAT_STR_3D:
        {
            /*
             * Note both 2D and 3D use 3 coordinates.
             * For 2D, the "R" component is always 0.
             */
            EventGraphicsOpenGLCreateBufferObject createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_textureCoordinatesBufferObject.reset(createEvent.getOpenGLBufferObject());
            CaretAssert(m_textureCoordinatesBufferObject->getBufferObjectName());
            
            m_textureCoordinatesDataType = GL_FLOAT;
            const GLuint textureCoordSizeBytes = primitive->m_floatTextureSTR.size() * sizeof(float);
            CaretAssert(textureCoordSizeBytes > 0);
            const GLvoid* textureCoordDataPointer = (const GLvoid*)&primitive->m_floatTextureSTR[0];
            
            glBindBuffer(GL_ARRAY_BUFFER,
                         m_textureCoordinatesBufferObject->getBufferObjectName());
            glBufferData(GL_ARRAY_BUFFER,
                         textureCoordSizeBytes,
                         textureCoordDataPointer,
                         usageHint);
            
        }
            break;
        case GraphicsTextureSettings::DimensionType::NONE:
            break;
    }
    
    m_reloadTextureCoordinatesFlag = false;
}


/**
 * Load the buffers with data from the grpahics primitive.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadAllBuffers(GraphicsPrimitive* primitive)
{
    loadCoordinateBuffer(primitive);
    loadNormalVectorBuffer(primitive);
    loadColorBuffer(primitive);
    loadTextureCoordinateBuffer(primitive);    
}

/**
 * If needed, load the texture buffer.
 * Note that the texture buffer may be invalidated when
 * an image is captures as the image capture wipes out all
 * textures.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadTextureImageDataBuffer(GraphicsPrimitive* primitive)
{
    if (m_textureImageDataName != NULL) {
        /*
         * Texture has already been loaded.
         * Do we need to update some of the texels (usually when user
         * is voxel editing)
         */
        const VoxelColorUpdate* voxelColorUpdate(primitive->getVoxelColorUpdate());
        if (voxelColorUpdate->isValid()) {
            switch (primitive->getTextureSettings().getDimensionType()) {
                case GraphicsTextureSettings::DimensionType::NONE:
                    break;
                case GraphicsTextureSettings::DimensionType::FLOAT_STR_2D:
                    CaretAssertMessage(0, "Voxel update not implemented for 2D textures");
                    break;
                case GraphicsTextureSettings::DimensionType::FLOAT_STR_3D:
                    /*
                     * Update the texture with new exels
                     */
                    loadTextureImageDataBuffer3D(primitive,
                                                 TextureLoadMode::VOXEL_COLOR_UPDATE);
                    break;
            }
            
            /*
             * Reset color update since it has been used
             */
            primitive->resetVoxelColorUpdate();
        }
        return;
    }
    
    switch (primitive->getTextureSettings().getDimensionType()) {
        case GraphicsTextureSettings::DimensionType::NONE:
            break;
        case GraphicsTextureSettings::DimensionType::FLOAT_STR_2D:
            loadTextureImageDataBuffer2D(primitive);
            break;
        case GraphicsTextureSettings::DimensionType::FLOAT_STR_3D:
            loadTextureImageDataBuffer3D(primitive,
                                         TextureLoadMode::FULL);
            break;
    }
}

/**
 * If needed, load the texture buffer for 2D data (image)
 * Note that the texture buffer may be invalidated when
 * an image is captures as the image capture wipes out all
 * textures.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
*/
void
GraphicsEngineDataOpenGL::loadTextureImageDataBuffer2D(GraphicsPrimitive* primitive)
{
    const bool useGraphicsSettingsFlag(true);
    if (useGraphicsSettingsFlag) {

        const GraphicsTextureSettings& textureSettings(primitive->getTextureSettings());
        
        const int64_t imageWidth  = textureSettings.getImageWidth();
        const int64_t imageHeight = textureSettings.getImageHeight();
        const int64_t numBytesPerPixel = primitive->getTexturePixelFormatBytesPerPixel();
        const int64_t imageNumberOfBytes = imageWidth * imageHeight * numBytesPerPixel;
        if (imageNumberOfBytes <= 0) {
            CaretLogWarning("Invalid texture (empty data) for drawing primitive with 2D texture");
            return;
        }
        
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        
        glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
        glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, textureSettings.getUnpackAlignment());
        
        const GLubyte* imageBytesPtr = textureSettings.getImageBytesPointer();
        EventGraphicsOpenGLCreateTextureName createEvent;
        EventManager::get()->sendEvent(createEvent.getPointer());
        m_textureImageDataName = createEvent.getOpenGLTextureName();
        CaretAssert(m_textureImageDataName);
        
        const GLuint openGLTextureName = m_textureImageDataName->getTextureName();
        
        glBindTexture(GL_TEXTURE_2D, openGLTextureName);
        
        bool useMipMapFlag(false);
        switch (primitive->getTextureSettings().getMipMappingType()) {
            case GraphicsTextureSettings::MipMappingType::ENABLED:
                useMipMapFlag = true;
                break;
            case GraphicsTextureSettings::MipMappingType::DISABLED:
                useMipMapFlag = false;
                break;
        }
        
        /*
         * "pixelDataFormat" is the format of the image data that
         * is pointed to by imageBytesRGBA.
         *
         * "internalFormat" is how OpenGL will store the data in
         * the texture memory.
         */
        GLenum internalFormat(GL_INVALID_VALUE);
        GLenum pixelDataFormat(GL_INVALID_VALUE);
        switch (primitive->getTextureSettings().getPixelFormatType()) {
            case GraphicsTextureSettings::PixelFormatType::NONE:
                break;
            case GraphicsTextureSettings::PixelFormatType::BGR:
                internalFormat  = GL_RGB;
                pixelDataFormat = GL_BGR;
                break;
            case GraphicsTextureSettings::PixelFormatType::BGRA:
                internalFormat  = GL_RGBA;
                pixelDataFormat = GL_BGRA;
                break;
            case GraphicsTextureSettings::PixelFormatType::BGRX:
                /*
                 * BGRX is a 32 bit Qt QImage Format.
                 * It includes an alpha that is always 255.
                 * Thus the alpha is not needed in the texture.
                 */
                internalFormat  = GL_RGB;
                pixelDataFormat = GL_BGRA;
                break;
            case GraphicsTextureSettings::PixelFormatType::RGB:
                internalFormat  = GL_RGB;
                pixelDataFormat = GL_RGB;
                break;
            case GraphicsTextureSettings::PixelFormatType::RGBA:
                internalFormat  = GL_RGBA;
                pixelDataFormat = GL_RGBA;
                break;
        }
        CaretAssert(pixelDataFormat != GL_INVALID_VALUE);
        CaretAssert((internalFormat == GL_RGB)
                    || (internalFormat == GL_RGBA));
        
        CaretLogFine("Loading Texture Primitive: "
                     + primitive->toString());
        
        /*
         * Texture compression
         */
        switch (textureSettings.getCompressionType()) {
            case GraphicsTextureSettings::CompressionType::DISABLED:
                break;
            case GraphicsTextureSettings::CompressionType::ENABLED:
                switch (internalFormat) {
                    case GL_RGB:
                        internalFormat = GL_COMPRESSED_RGB;
                        break;
                    case GL_RGBA:
                        internalFormat = GL_COMPRESSED_RGBA;
                        break;
                    default:
                        CaretAssert(0);
                        break;
                }
                break;
        }

        if (useMipMapFlag) {
            /*
             * This code seems to work if OpenGL 3.0 or later and
             * replaces gluBuild2DMipmaps()
             *
             * glTexImage2D(GL_TEXTURE_2D,     // MUST BE GL_TEXTURE_2D
             *            0,                 // level of detail 0=base, n is nth mipmap reduction
             *            GL_RGBA,           // internal format
             *            imageWidth,        // width of image
             *            imageHeight,       // height of image
             *            0,                 // border
             *            GL_RGBA,           // format of the pixel data
             *            GL_UNSIGNED_BYTE,  // data type of pixel data
             *            imageBytesRGBA);   // pointer to image data
             * glGenerateMipmap(GL_TEXTURE_2D);
             */
            
            /*
             * GL_COMPRESSED_RGBA - use for compressed textures
             * saves lots of space if images are large.
             */
            const int errorCode = gluBuild2DMipmaps(GL_TEXTURE_2D,     // MUST BE GL_TEXTURE_2D
                                                    internalFormat,    // internal format
                                                    imageWidth,        // width of image
                                                    imageHeight,       // height of image
                                                    pixelDataFormat,   // format of the pixel data
                                                    GL_UNSIGNED_BYTE,  // data type of pixel data
                                                    imageBytesPtr);    // pointer to image data
            if (errorCode != 0) {
                useMipMapFlag = false;
                
                const GLubyte* errorChars = gluErrorString(errorCode);
                if (errorChars != NULL) {
                    const QString errorText = ("ERROR building mipmaps for 2D image: "
                                               + AString((char*)errorChars));
                    CaretLogSevere(errorText);
                }
            }
        }
        
        /*
         * Note: Generating mip maps above may have failed
         * so not in an "else"
         */
        if ( ! useMipMapFlag) {
            glTexImage2D(GL_TEXTURE_2D,     // MUST BE GL_TEXTURE_2D
                         0,                 // level of detail 0=base, n is nth mipmap reduction
                         internalFormat,    // internal format
                         imageWidth,        // width of image
                         imageHeight,       // height of image
                         0,                 // border
                         pixelDataFormat,   // format of the pixel data
                         GL_UNSIGNED_BYTE,  // data type of pixel data
                         imageBytesPtr);   // pointer to image data
            
            const auto errorGL = GraphicsUtilitiesOpenGL::getOpenGLError();
            if (errorGL) {
                CaretLogSevere("OpenGL error after glTexImage2D(), width="
                               + AString::number(imageWidth)
                               + ", height="
                               + AString::number(imageHeight)
                               + ": "
                               + errorGL->getVerboseDescription());
            }
        }
        
        /*
         * Successful texture compression
         */
        switch (textureSettings.getCompressionType()) {
            case GraphicsTextureSettings::CompressionType::DISABLED:
                break;
            case GraphicsTextureSettings::CompressionType::ENABLED:
            {
                /*
                 * Reuested compression type
                 */
                AString requestedCompressionName;
                AString unusedDecimalText;
                AString unusedHexText;
                GraphicsUtilitiesOpenGL::getTextCompressionEnumInfo(internalFormat,
                                                                    requestedCompressionName,
                                                                    unusedDecimalText,
                                                                    unusedHexText);
                
                /*
                 * Actual compression type
                 */
                GLint    compressedFlag(GL_FALSE);
                GLint    level(0);
                glGetTexLevelParameteriv(GL_TEXTURE_2D,
                                         level,
                                         GL_TEXTURE_COMPRESSED,
                                         &compressedFlag);
                if (compressedFlag) {
                    GLint   compressedFormat(0);
                    GLsizei  compressedSize(0);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_INTERNAL_FORMAT, &compressedFormat);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);
                    
                    QString actualCompressionName;
                    GraphicsUtilitiesOpenGL::getTextCompressionEnumInfo(compressedFormat,
                                                                        actualCompressionName,
                                                                        unusedDecimalText,
                                                                        unusedHexText);

                    CaretLogInfo("Texture compression for level 0 reduced "
                                 + FileInformation::fileSizeToStandardUnits(imageWidth * imageHeight * 4)
                                 + " down to "
                                 + FileInformation::fileSizeToStandardUnits(compressedSize)
                                 + "\n Requested format: "
                                 + requestedCompressionName
                                 + " Actual: "
                                 + actualCompressionName);
                }
                else {
                }
            }
                break;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glPopClientAttrib();


        return;
    }
}

/**
 * If needed, load the texture buffer for 3D data (volume)
 * Note that the texture buffer may be invalidated when
 * an image is captures as the image capture wipes out all
 * textures.
 *
 * @param primitive
 *     The graphics primitive that will be drawn.
 */
void
GraphicsEngineDataOpenGL::loadTextureImageDataBuffer3D(GraphicsPrimitive* primitive,
                                                       const TextureLoadMode textureLoadMode)
{
    const GraphicsTextureSettings& textureSettings(primitive->getTextureSettings());
    
    const int64_t imageWidth(textureSettings.getImageWidth());
    const int64_t imageHeight(textureSettings.getImageHeight());
    const int64_t imageSlices(textureSettings.getImageSlices());
    const int64_t imageBytesPerPixel(primitive->getTexturePixelFormatBytesPerPixel());
    const int64_t imageNumberOfBytes = imageWidth * imageHeight * imageSlices * imageBytesPerPixel;
    if (imageNumberOfBytes <= 0) {
        CaretLogWarning("Invalid texture (empty data) for drawing primitive with 3D texture");
        return;
    }
    const uint8_t* imageDataPointer(textureSettings.getImageBytesPointer());
    if (imageDataPointer == NULL) {
        CaretLogSevere("Image data pointer is NULL for 3D texture");
        return;
    }
    
    GLint64 maxTextureSize(0);
    glGetInteger64v(GL_MAX_3D_TEXTURE_SIZE,
                    &maxTextureSize);
    AString sizeMsg;
    if (imageWidth > maxTextureSize) {
        sizeMsg.appendWithNewLine("Width="
                                  + AString::number(imageWidth)
                                  + " is too big for 3D Texture.");
    }
    if (imageHeight > maxTextureSize) {
        sizeMsg.appendWithNewLine("Height="
                                  + AString::number(imageHeight)
                                  + " is too big for 3D Texture.");
    }
    if (imageSlices > maxTextureSize) {
        sizeMsg.appendWithNewLine("Slices="
                                  + AString::number(imageSlices)
                                  + " is too big for 3D Texture.");
    }
    if ( ! sizeMsg.isEmpty()) {
        sizeMsg.appendWithNewLine("Maximum texture dimension="
                                  + AString::number(maxTextureSize));
        CaretLogWarning(sizeMsg);
        return;
    }
    
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, textureSettings.getUnpackAlignment());
    
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
    
    switch (textureLoadMode) {
        case FULL:
        {
            EventGraphicsOpenGLCreateTextureName createEvent;
            EventManager::get()->sendEvent(createEvent.getPointer());
            m_textureImageDataName = createEvent.getOpenGLTextureName();
            CaretAssert(m_textureImageDataName);
        }
            break;
        case VOXEL_COLOR_UPDATE:
            CaretAssert(m_textureImageDataName);
            break;
    }
    
    const GLuint openGLTextureName = m_textureImageDataName->getTextureName();
    
    GLenum pixelDataFormat = GL_RGBA;
    switch (textureSettings.getPixelFormatType()) {
        case GraphicsTextureSettings::PixelFormatType::NONE:
            break;
        case GraphicsTextureSettings::PixelFormatType::BGR:
            pixelDataFormat = GL_BGR;
            break;
        case GraphicsTextureSettings::PixelFormatType::BGRA:
            pixelDataFormat = GL_BGRA;
            break;
        case GraphicsTextureSettings::PixelFormatType::BGRX:
            pixelDataFormat = GL_BGRA;
            break;
        case GraphicsTextureSettings::PixelFormatType::RGB:
            pixelDataFormat = GL_RGB;
            break;
        case GraphicsTextureSettings::PixelFormatType::RGBA:
            pixelDataFormat = GL_RGBA;
            break;
    }
    
    glBindTexture(GL_TEXTURE_3D, openGLTextureName);
    
    bool useMipMapFlag(false);
    switch (textureSettings.getMipMappingType()) {
        case GraphicsTextureSettings::MipMappingType::ENABLED:
            useMipMapFlag = true;
            break;
        case GraphicsTextureSettings::MipMappingType::DISABLED:
            useMipMapFlag = false;
            break;
    }
    
    switch (textureSettings.getCompressionType()) {
        case GraphicsTextureSettings::CompressionType::DISABLED:
            break;
        case GraphicsTextureSettings::CompressionType::ENABLED:
            CaretLogWarning("Compression not implemented for 3D Textures");
            break;
    }

//    /*
//     * 31 March 2023
//     * 3D mip maps were enabled a couple of commits ago.
//     * However, disable 3D mip maps until I (John H) can test the functionality
//     * on Linux and Windows.
//     *
//     * In addition, we map be able to use OpenGL to create the mip maps and
//     * not the GLU toolkit (which is (probably) all software).
//     *
//     * These parameters to glTextParameter(i/f):
//     *   - GL_GENERATE_MIPMAP
//     *   - GL_TEXTURE_MIN_LOD
//     *   - GL_TEXTURE_MAX_LOD
//     * There is also glGenerateMipMaps in OpenGL 3.
//     *
//     * See: https://docs.gl/gl2/glTexParameter
//     */
//    {
//        minFilter = GraphicsTextureMinificationFilterEnum::LINEAR;
//        mipMap = GraphicsTextureSettings::MipMappingType::DISABLED;
//    }
//
//    BEST WAY TO GENERATE MIP MAPS instead of GLU functions:
//
//    glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, 1);
    

    if (useMipMapFlag) {
        CaretAssertMessage(0, "Mipmaps not supported (yet) for VoxelColorUpdate");
#ifdef CARET_OS_WINDOWS_MSVC
        CaretLogSevere("3D Mipmaps function gluBuild3DMipmaps() not available on system "
                       "used to compile this version of Workbench.");
#else
        /*
         * This code seems to work if OpenGL 3.0 or later and
         * replaces gluBuild2DMipmaps()
         *
         * glTexImage2D(GL_TEXTURE_2D,     // MUST BE GL_TEXTURE_2D
         *            0,                 // level of detail 0=base, n is nth mipmap reduction
         *            GL_RGBA,           // number of components
         *            imageWidth,        // width of image
         *            imageHeight,       // height of image
         *            0,                 // border
         *            GL_RGBA,           // format of the pixel data
         *            GL_UNSIGNED_BYTE,  // data type of pixel data
         *            imageBytesRGBA);   // pointer to image data
         * glGenerateMipmap(GL_TEXTURE_2D);
         */
        const int errorCode = gluBuild3DMipmaps(GL_TEXTURE_3D,     // MUST BE GL_TEXTURE_3D
                                                GL_RGBA,           // internal format
                                                imageWidth,        // width of image
                                                imageHeight,       // height of image
                                                imageSlices,       // slices of image
                                                pixelDataFormat,   // format of the pixel data
                                                GL_UNSIGNED_BYTE,  // data type of pixel data
                                                (GLubyte*)imageDataPointer);    // pointer to image data
        if (errorCode != 0) {
            useMipMapFlag = false;
            
            const GLubyte* errorChars = gluErrorString(errorCode);
            if (errorChars != NULL) {
                const QString errorText = ("ERROR building mipmaps for 3D image: "
                                           + AString((char*)errorChars));
                CaretLogSevere(errorText);
            }
        }
#endif
    }
    
    switch (textureLoadMode) {
        case FULL:
            /*
             * Note: Generating mip maps above may have failed
             * so not in an "else"
             */
            if ( ! useMipMapFlag) {
                glTexImage3D(GL_TEXTURE_3D,     // MUST BE GL_TEXTURE_3D
                             0,                 // level of detail 0=base, n is nth mipmap reduction
                             GL_RGBA,           // internal format
                             imageWidth,        // width of volume
                             imageHeight,       // height of volume
                             imageSlices,       // slices of volume
                             0,                 // border
                             pixelDataFormat,   // format of the pixel data
                             GL_UNSIGNED_BYTE,  // data type of pixel data
                             (GLubyte*)imageDataPointer);   // pointer to image data
                
                const auto errorGL = GraphicsUtilitiesOpenGL::getOpenGLError();
                if (errorGL) {
                    CaretLogSevere("OpenGL error after glTexImage3D(), width="
                                   + AString::number(imageWidth)
                                   + ", height="
                                   + AString::number(imageHeight)
                                   + ", slices="
                                   + AString::number(imageSlices)
                                   + ": "
                                   + errorGL->getVerboseDescription());
                }
            }
            break;
        case VOXEL_COLOR_UPDATE:
        {
            const VoxelColorUpdate* voxelColorUpdate(primitive->getVoxelColorUpdate());
            const std::array<uint8_t, 4> rgba(voxelColorUpdate->getRGBA());
            const int32_t numVoxels(voxelColorUpdate->getNumberOfVoxels());
            for (int32_t i = 0; i < numVoxels; i++) {
                const VoxelIJK& v(voxelColorUpdate->getVoxel(i));
                glTexSubImage3D(GL_TEXTURE_3D, // MUST BE GL_TEXTURE_3D
                                0,             // level of detail 0=base, n is nth mipmap reduction
                                v.i(),         // X-offset
                                v.j(),         // Y-offset
                                v.k(),         // Z-offset
                                1,             // width
                                1,             // height
                                1,             // depth
                                pixelDataFormat,        //format of the pixel data
                                GL_UNSIGNED_BYTE,       // data type of pixel data
                                (GLubyte*)rgba.data()); // pointer to data
                
                const auto errorGL = GraphicsUtilitiesOpenGL::getOpenGLError();
                if (errorGL) {
                    CaretLogSevere("OpenGL error after glTexImage3D(), width="
                                   + AString::number(imageWidth)
                                   + ", height="
                                   + AString::number(imageHeight)
                                   + ", slices="
                                   + AString::number(imageSlices)
                                   + ": "
                                   + errorGL->getVerboseDescription());
                }
            }
        }
//            NEED TO TURN OFF INVALIDATION OF GRA:PHICS PRIMITIVE IN ????=
            break;
    }
    
    const bool showTextureFlag(true);
    if (showTextureFlag
        && useMipMapFlag) {
        GLint maxLevels(-1);
        glGetTexParameteriv(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, &maxLevels);
        GLint maxMipMap(-1);
        for (int32_t i = 0; i < maxLevels; i++) {
            GLint textureWidth;
            glGetTexLevelParameteriv(GL_TEXTURE_3D, i, GL_TEXTURE_WIDTH, &textureWidth);
            if (textureWidth <= 0) {
                maxMipMap = i - 1;
                break;
            }
        }
        std::cout << "Max texture levels: " << maxMipMap << std::endl;
        
        /*
         * Above code might generate an OpenGL error
         */
        std::unique_ptr<GraphicsOpenGLError> openglError = GraphicsUtilitiesOpenGL::getOpenGLError();
        if (openglError) {
            CaretLogWarning("Determining mip map levels may have caused this OpenGL Error (can be ignored): "
                            + openglError->getVerboseDescription());
        }
    }
    
    glBindTexture(GL_TEXTURE_3D, 0);
    glPopClientAttrib();
}

/**
 * Draw the given graphics primitive.
 *
 * @param primitive
 *     Primitive that is drawn.
 */
void
GraphicsEngineDataOpenGL::draw(GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    if ( ! primitive->isValid()) {
        CaretLogFine("Attempting to draw invalid Graphics Primitive");
        return;
    }
    
    GraphicsPrimitive* primitiveToDraw = primitive;
    
    /* Conversion to window space creates a new primitive that must be deleted */
    std::unique_ptr<GraphicsPrimitive> lineConversionPrimitive;
    
    bool modelSpaceLineFlag = false;
    bool windowSpaceLineFlag = false;
    bool millimeterPointsFlag = false;
    bool spheresFlag = false;
    switch (primitive->m_primitiveType) {
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_LOOP:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_STRIP:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINES:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_POINTS:
            switch (primitive->m_pointSizeType) {
                case GraphicsPrimitive::PointSizeType::MILLIMETERS:
                    millimeterPointsFlag = true;
                    break;
                case GraphicsPrimitive::PointSizeType::PERCENTAGE_VIEWPORT_HEIGHT:
                    break;
                case GraphicsPrimitive::PointSizeType::PIXELS:
                    break;
            }
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES:
            break;
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINES:
            modelSpaceLineFlag = true;
            break;
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES:
            windowSpaceLineFlag = true;
            break;
        case GraphicsPrimitive::PrimitiveType::SPHERES:
            spheresFlag = true;
            break;
    }
    
    if (modelSpaceLineFlag
        || windowSpaceLineFlag) {
        if (primitive->getNumberOfVertices() == 1) {
            spheresFlag = true;
            
            uint8_t rgba[4];
            switch (primitive->getColorDataType()) {
                case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
                {
                    float rgbaFloat[4];
                    primitive->getVertexFloatRGBA(0, rgbaFloat);
                    for (int32_t i = 0; i < 4; i++) {
                        rgba[i] = static_cast<uint8_t>(rgbaFloat[i] / 255.0);
                    }
                }
                    break;
                case GraphicsPrimitive::ColorDataType::NONE:
                    break;
                case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
                    primitive->getVertexByteRGBA(0, rgba);
                    break;
            }
            float sphereXYZ[3];
            primitive->getVertexFloatXYZ(0, sphereXYZ);
            GraphicsPrimitive::LineWidthType lineWidthType;
            float lineWidth;
            primitive->getLineWidth(lineWidthType, lineWidth);
            std::unique_ptr<GraphicsPrimitiveV3f> pointPrimitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_POINTS,
                                                                                                     rgba));
            pointPrimitive->addVertex(sphereXYZ);
            
            GraphicsPrimitive::PointSizeType pointSizeType(GraphicsPrimitive::PointSizeType::PIXELS);
            switch (lineWidthType) {
                case GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT:
                    pointSizeType = GraphicsPrimitive::PointSizeType::PERCENTAGE_VIEWPORT_HEIGHT;
                    break;
                case GraphicsPrimitive::LineWidthType::PIXELS:
                    pointSizeType = GraphicsPrimitive::PointSizeType::PIXELS;
                    break;
            }
            pointPrimitive->setPointDiameter(pointSizeType,
                                             lineWidth);

            drawPrivate(PrivateDrawMode::DRAW_NORMAL,
                        pointPrimitive.get(),
                        NULL);
            return;
        }
    }
    
    if (millimeterPointsFlag) {
        /* 
         * Special case for points in millimeters 
         */
        drawPointsPrimitiveMillimeters(primitive);
    }
    else if (spheresFlag) {
        drawSpheresPrimitive(primitive);
    }
    else if (modelSpaceLineFlag
             || windowSpaceLineFlag) {
        AString errorMessage;
        primitiveToDraw = GraphicsOpenGLPolylineTriangles::convertWorkbenchLinePrimitiveTypeToOpenGL(primitive,
                                                                                                 errorMessage);
        if (primitiveToDraw == NULL) {
            /*
             * Empty error message can be ignored
             */
            if ( ! errorMessage.isEmpty()) {
                const AString msg("For developer: "
                                  + errorMessage);
#ifdef NDEBUG
                CaretLogFine(msg);
#else
                CaretLogSevere(msg);
#endif
            }
        }
        else {
            SpaceMode spaceMode = SpaceMode::WINDOW;
            if (modelSpaceLineFlag) {
                spaceMode = SpaceMode::MODEL;
            }
            else if (windowSpaceLineFlag) {
                spaceMode = SpaceMode::WINDOW;
            }
            else {
                CaretAssert(0);
            }
            
            lineConversionPrimitive.reset(primitiveToDraw);
            drawModelOrWindowSpace(spaceMode,
                                   PrivateDrawMode::DRAW_NORMAL,
                                   primitiveToDraw,
                                   NULL);
        }
    }
    else {
        drawPrivate(PrivateDrawMode::DRAW_NORMAL,
                    primitiveToDraw,
                    NULL);
    }
}

/**
 * Draw the graphics primitive in model space.
 *
 * @param spaceMode
 *     Space mode: model or window
 * @param drawMode
 *     Mode for drawing.
 * @param primitive
 *     Primitive that is drawn.
 * @param primitiveSelectionHelper
 *     Selection helper when draw mode is selection.
 */
void
GraphicsEngineDataOpenGL::drawModelOrWindowSpace(const SpaceMode spaceMode,
                                                 const PrivateDrawMode drawMode,
                                                 GraphicsPrimitive* primitive,
                                                 GraphicsPrimitiveSelectionHelper* primitiveSelectionHelper)
{
    bool windowSpaceFlag = false;
    switch (spaceMode) {
        case MODEL:
            break;
        case WINDOW:
            windowSpaceFlag = true;
            break;
    }
    
    int32_t polygonMode[2];
    int32_t viewport[4];
    saveOpenGLStateForWindowSpaceDrawing(polygonMode,
                                         viewport);
    
    if (windowSpaceFlag) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(viewport[0], viewport[0] + viewport[2],
                viewport[1], viewport[1] + viewport[3],
                0, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    
    glPolygonMode(GL_FRONT, GL_FILL);
    
    drawPrivate(drawMode,
                primitive,
                primitiveSelectionHelper);
    
    restoreOpenGLStateForWindowSpaceDrawing(polygonMode,
                                            viewport);
}

/**
 * Draw a point primitive type in millimeters (same as model space).
 *
 * @param primitive
 *     The points primitive type.
 */
void
GraphicsEngineDataOpenGL::drawPointsPrimitiveMillimeters(const GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    CaretAssert(primitive->getPrimitiveType() == GraphicsPrimitive::PrimitiveType::OPENGL_POINTS);
    CaretAssert(primitive->m_pointSizeType == GraphicsPrimitive::PointSizeType::MILLIMETERS);
    
    GraphicsPrimitive::PointSizeType sizeType;
    float sizeValue = 0.0f;
    primitive->getPointDiameter(sizeType,
                                sizeValue);
    
    const int32_t numberOfVertices = primitive->getNumberOfVertices();
    switch (primitive->m_pointSizeType) {
        case GraphicsPrimitive::PointSizeType::MILLIMETERS:
        {
            switch (primitive->m_vertexColorType) {
                case GraphicsPrimitive::VertexColorType::NONE:
                    CaretAssert(0);
                    CaretLogSevere("NONE vertex color type, should never occur when drawing points in millimeters");
                    return;
                    break;
                case GraphicsPrimitive::VertexColorType::PER_VERTEX_RGBA:
                {
                    for (int32_t i = 0; i < numberOfVertices; i++) {
                        const int32_t i3 = i * 3;
                        CaretAssertVectorIndex(primitive->m_xyz, i3 + 2);
                        const float* xyz = &primitive->m_xyz[i3];
                        const int32_t i4 = i * 4;
                        
                        uint8_t rgba[4];
                        switch (primitive->m_colorDataType) {
                            case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
                            {
                                CaretAssertVectorIndex(primitive->m_floatRGBA, i4 + 3);
                                
                                rgba[0] = static_cast<uint8_t>(primitive->m_floatRGBA[i4] * 255.0);
                                rgba[1] = static_cast<uint8_t>(primitive->m_floatRGBA[i4+1] * 255.0);
                                rgba[2] = static_cast<uint8_t>(primitive->m_floatRGBA[i4+2] * 255.0);
                                rgba[3] = static_cast<uint8_t>(primitive->m_floatRGBA[i4+3] * 255.0);
                            }
                                break;
                            case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
                                CaretAssertVectorIndex(primitive->m_unsignedByteRGBA, i4 + 3);
                                rgba[0] = primitive->m_unsignedByteRGBA[i4];
                                rgba[1] = primitive->m_unsignedByteRGBA[i4+1];
                                rgba[2] = primitive->m_unsignedByteRGBA[i4+2];
                                rgba[3] = primitive->m_unsignedByteRGBA[i4+3];
                                break;
                            case GraphicsPrimitive::ColorDataType::NONE:
                                CaretAssert(0);
                                break;
                        }
                        
                        GraphicsShape::drawSquare(xyz, rgba, sizeValue);
                    }
                }
                    break;
                case GraphicsPrimitive::VertexColorType::SOLID_RGBA:
                {
                    const int32_t numberOfVertices = primitive->getNumberOfVertices();
                    CaretAssertVectorIndex(primitive->m_xyz, (numberOfVertices - 1) * 3 + 1);
                    GraphicsShape::drawSquares(&primitive->m_xyz[0],
                                               numberOfVertices,
                                               &primitive->m_unsignedByteRGBA[0],
                                               sizeValue);
                }
                    break;
            }
        }
            break;
        case GraphicsPrimitive::PointSizeType::PERCENTAGE_VIEWPORT_HEIGHT:
        case GraphicsPrimitive::PointSizeType::PIXELS:
            CaretAssert(0);
            break;
    }
}

/**
 * Draw a sphere primitive type.
 * 
 * @param primitive
 *     The spheres primitive type.
 */
void
GraphicsEngineDataOpenGL::drawSpheresPrimitive(const GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    CaretAssert(primitive->getPrimitiveType() == GraphicsPrimitive::PrimitiveType::SPHERES);

    GraphicsPrimitive::SphereSizeType sizeType;
    float sizeValue = 0.0f;
    primitive->getSphereDiameter(sizeType,
                                 sizeValue);
    
    switch (primitive->m_vertexColorType) {
        case GraphicsPrimitive::VertexColorType::NONE:
            CaretAssert(0);
            CaretLogSevere("NONE vertex color type, should never occur when drawing spheres");
            return;
            break;
        case GraphicsPrimitive::VertexColorType::PER_VERTEX_RGBA:
        {
            const int32_t numberOfVertices = primitive->getNumberOfVertices();
            for (int32_t i = 0; i < numberOfVertices; i++) {
                const int32_t i3 = i * 3;
                CaretAssertVectorIndex(primitive->m_xyz, i3 + 2);
                const int32_t i4 = i * 4;
                
                switch (primitive->m_colorDataType) {
                    case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
                    {
                        CaretAssertVectorIndex(primitive->m_floatRGBA, i4 + 3);
                        uint8_t rgba[4] = {
                            static_cast<uint8_t>(primitive->m_floatRGBA[i4] * 255.0),
                            static_cast<uint8_t>(primitive->m_floatRGBA[i4+1] * 255.0),
                            static_cast<uint8_t>(primitive->m_floatRGBA[i4+2] * 255.0),
                            static_cast<uint8_t>(primitive->m_floatRGBA[i4+3] * 255.0)
                        };
                        GraphicsShape::drawSphereByteColor(&primitive->m_xyz[i3],
                                                           rgba,
                                                           sizeValue);
                    }
                        break;
                    case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
                        CaretAssertVectorIndex(primitive->m_unsignedByteRGBA, i4 + 3);
                        GraphicsShape::drawSphereByteColor(&primitive->m_xyz[i3],
                                                           &primitive->m_unsignedByteRGBA[i4],
                                                           sizeValue);
                        break;
                    case GraphicsPrimitive::ColorDataType::NONE:
                        CaretAssert(0);
                        break;
                }
            }
        }
            break;
        case GraphicsPrimitive::VertexColorType::SOLID_RGBA:
        {
            const int32_t numberOfVertices = primitive->getNumberOfVertices();
            CaretAssertVectorIndex(primitive->m_xyz, (numberOfVertices - 1) * 3 + 1);
            
            switch (primitive->m_colorDataType) {
                case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
                {
                    CaretAssertVectorIndex(primitive->m_floatRGBA, ((numberOfVertices * 4) - 1));
                    const uint8_t rgba[4] = {
                        static_cast<uint8_t>(primitive->m_floatRGBA[0] * 255.0),
                        static_cast<uint8_t>(primitive->m_floatRGBA[1] * 255.0),
                        static_cast<uint8_t>(primitive->m_floatRGBA[2] * 255.0),
                        static_cast<uint8_t>(primitive->m_floatRGBA[3] * 255.0)
                    };
                    GraphicsShape::drawSpheresByteColor(&primitive->m_xyz[0],
                                                        numberOfVertices,
                                                        rgba,
                                                        sizeValue);
                }
                    break;
                case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
                    CaretAssertVectorIndex(primitive->m_unsignedByteRGBA, ((numberOfVertices * 4) - 1));
                    GraphicsShape::drawSpheresByteColor(&primitive->m_xyz[0],
                                                        numberOfVertices,
                                                        &primitive->m_unsignedByteRGBA[0],
                                                        sizeValue);
                    break;
                case GraphicsPrimitive::ColorDataType::NONE:
                    CaretAssert(0);
                    break;
            }
        }
            break;
    }
}

/**
 * Save the OpenGL State for drawing in window space.
 *
 * @param polygonMode
 *    Output with the polygon mode.
 * @param viewport
 *    Output with viewport.
 */
void
GraphicsEngineDataOpenGL::saveOpenGLStateForWindowSpaceDrawing(int32_t polygonMode[2],
                                          int32_t viewport[4])
{
    glPushAttrib(GL_COLOR_BUFFER_BIT
                 | GL_ENABLE_BIT
                 | GL_HINT_BIT
                 | GL_LIGHTING_BIT
                 | GL_POLYGON_BIT);
    
    /*
     * For anti-aliasing requires sorting polygons
     * and turning of depth testing
     */
    const bool drawWithAntiAliasingFlag = false;
    if (drawWithAntiAliasingFlag) {
        glBlendFunc (GL_SRC_ALPHA_SATURATE, GL_ONE);
        glEnable(GL_BLEND);
        glHint(GL_POLYGON_SMOOTH_HINT,
               GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
    }
    
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
    glGetIntegerv(GL_POLYGON_MODE,
                  polygonMode);
    
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}

/**
 * Restore the OpenGL State from drawing in window space.
 
 * @param polygonMode
 *    Polygon mode.
 * @param viewport
 *    Viewport.
 */
void
GraphicsEngineDataOpenGL::restoreOpenGLStateForWindowSpaceDrawing(int32_t polygonMode[2],
                                                                  int32_t viewport[4])
{
    glPolygonMode(GL_FRONT, polygonMode[0]);
    glPolygonMode(GL_BACK, polygonMode[1]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    
    glPopAttrib();
}

/**
 * Draw to determine the index of primitive at the given (X, Y) location
 *
 * @param primitive
 *     Primitive that is drawn.
 * @param pixelX
 *     X location for selection.
 * @param pixelY
 *     Y location for selection.
 * @param selectedPrimitiveIndexOut
 *     Output with the index of the primitive (invalid no primitve at location
 * @param selectedPrimitiveDepthOut
 *     Output with depth at the location.
 */
void
GraphicsEngineDataOpenGL::drawWithSelection(GraphicsPrimitive* primitive,
                                            const int32_t pixelX,
                                            const int32_t pixelY,
                                            int32_t& selectedPrimitiveIndexOut,
                                            float&   selectedPrimitiveDepthOut)
{
    CaretAssert(primitive);
    
    bool modelSpaceLineFlag = false;
    bool windowSpaceLineFlag = false;
    switch (primitive->m_primitiveType) {
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_LOOP:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_STRIP:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINES:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_POINTS:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP:
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES:
            break;
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINES:
            modelSpaceLineFlag = true;
            break;
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES:
            windowSpaceLineFlag = true;
            break;
        case GraphicsPrimitive::PrimitiveType::SPHERES:
            CaretAssertMessage(0, "Not yet implemented");
            break;
    }
    
    selectedPrimitiveIndexOut = -1;
    selectedPrimitiveDepthOut = 0.0;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    /*
     * Must clear color and depth buffers
     */
    glClear(GL_COLOR_BUFFER_BIT
            | GL_DEPTH_BUFFER_BIT);
    
    if ((pixelX    >= viewport[0])
        && (pixelX <  (viewport[0] + viewport[2]))
        && (pixelY >= viewport[1])
        && (pixelY <  (viewport[1] + viewport[3]))) {
        /*
         * Saves glPixelStore parameters
         */
        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        glPushAttrib(GL_LIGHTING_BIT);
        
        glShadeModel(GL_FLAT);
        glDisable(GL_LIGHTING);
        
        std::vector<int32_t> triangleVertexIndicesToLineVertexIndices;
        std::unique_ptr<GraphicsPrimitiveSelectionHelper> selectionHelper;
        if (modelSpaceLineFlag
            || windowSpaceLineFlag) {
            AString errorMessage;
            
            std::unique_ptr<GraphicsPrimitive> primitiveToDraw(GraphicsOpenGLPolylineTriangles::convertWorkbenchLinePrimitiveTypeToOpenGL(primitive,
                                                                                                                                          &triangleVertexIndicesToLineVertexIndices,
                                                                                                                                          errorMessage));
            if (primitiveToDraw == NULL) {
                const AString msg("For developer: "
                                  + errorMessage);
#ifdef NDEBUG
                CaretLogFine(msg);
#else
                CaretLogSevere(msg);
#endif
                return;
            }
            SpaceMode spaceMode = SpaceMode::WINDOW;
            if (modelSpaceLineFlag) {
                spaceMode = SpaceMode::MODEL;
            }
            else if (windowSpaceLineFlag) {
                spaceMode = SpaceMode::WINDOW;
            }
            else {
                CaretAssert(0);
            }
            
            selectionHelper.reset(new GraphicsPrimitiveSelectionHelper(primitiveToDraw.get()));
            selectionHelper->setupSelectionBeforeDrawing();
            drawModelOrWindowSpace(spaceMode,
                                   PrivateDrawMode::DRAW_SELECTION,
                                   primitiveToDraw.get(),
                                   selectionHelper.get());
        }
        else {
            selectionHelper.reset(new GraphicsPrimitiveSelectionHelper(primitive));
            selectionHelper->setupSelectionBeforeDrawing();
            drawPrivate(PrivateDrawMode::DRAW_SELECTION,
                        primitive,
                        selectionHelper.get());
        }
        
        /*
         * QOpenGLWidget Note: The QOpenGLWidget always renders in a
         * frame buffer object (see its documentation).  This is
         * probably why calls to glReadBuffer() always cause an
         * OpenGL error.
         */
#ifdef WORKBENCH_USE_QT5_QOPENGL_WIDGET
        /* do not call glReadBuffer() */
#else
        glReadBuffer(GL_BACK);
#endif
        uint8_t pixelRGBA[4];

        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1); /* bytes */
        glReadPixels(pixelX,
                     pixelY,
                     1,
                     1,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     pixelRGBA);
        
        /*
         * Get depth from depth buffer
         */
        glPixelStorei(GL_PACK_ALIGNMENT, 4); /* float */
        glReadPixels(pixelX,
                     pixelY,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &selectedPrimitiveDepthOut);
        
        glPopAttrib();
        glPopClientAttrib();
        
        CaretAssert(selectionHelper.get());
        selectedPrimitiveIndexOut = selectionHelper->getPrimitiveIndexFromEncodedRGBA(pixelRGBA);
        
        if (modelSpaceLineFlag
            || windowSpaceLineFlag) {
            if ((selectedPrimitiveIndexOut >= 0)
                && selectedPrimitiveIndexOut < static_cast<int32_t>(triangleVertexIndicesToLineVertexIndices.size())) {
                CaretAssertVectorIndex(triangleVertexIndicesToLineVertexIndices, selectedPrimitiveIndexOut);
                selectedPrimitiveIndexOut = triangleVertexIndicesToLineVertexIndices[selectedPrimitiveIndexOut];
                
                if (selectedPrimitiveIndexOut >= primitive->getNumberOfVertices()) {
                    selectedPrimitiveIndexOut = -1;
                }
            }
            else {
                selectedPrimitiveIndexOut = -1;
            }
        }
        else {
            if (selectedPrimitiveIndexOut >= 0) {
                if (selectedPrimitiveIndexOut >= primitive->getNumberOfVertices()) {
                    selectedPrimitiveIndexOut = -1;
                }
            }
        }
    }
}

/**
 * Draw the graphics primitive.
 *
 * @param drawMode
 *     Mode for drawing.
 * @param primitive
 *     Primitive that is drawn.
 * @param primitiveSelectionHelper
 *     Selection helper when draw mode is selection.
 */
void
GraphicsEngineDataOpenGL::drawPrivate(const PrivateDrawMode drawMode,
                                      GraphicsPrimitive* primitive,
                                      GraphicsPrimitiveSelectionHelper* primitiveSelectionHelper)
{
    CaretAssert(primitive);
    if ( ! primitive->isValid()) {
        return;
    }
    
    switch (drawMode) {
        case PrivateDrawMode::DRAW_NORMAL:
            break;
        case PrivateDrawMode::DRAW_SELECTION:
            break;
    }
    
    GraphicsEngineDataOpenGL* openglData = primitive->getGraphicsEngineDataForOpenGL();
    if (openglData == NULL) {
        openglData = new GraphicsEngineDataOpenGL();
        primitive->setGraphicsEngineDataForOpenGL(openglData);
        
        openglData->loadAllBuffers(primitive);
    }
    else {
        /*
         * Coordinates may get updated.
         */
        if (openglData->m_reloadCoordinatesFlag) {
            openglData->loadCoordinateBuffer(primitive);
        }
        
        /*
         * Colors may get updated
         */
        if (openglData->m_reloadColorsFlag) {
            openglData->loadColorBuffer(primitive);
        }
        
        /*
         * Texture coordinates may get updated
         */
        if (openglData->m_reloadTextureCoordinatesFlag) {
            openglData->loadTextureCoordinateBuffer(primitive);
        }
    }
    
    openglData->loadTextureImageDataBuffer(primitive);
    
    /*
     * Primitive may want to delete its instance data to save memory
     */
    primitive->setOpenGLBuffersHaveBeenLoadedByGraphicsEngine();
    
    const GLuint coordBufferID = openglData->m_coordinateBufferObject->getBufferObjectName();
    CaretAssert(coordBufferID);
    if ( ! glIsBuffer(coordBufferID)) {
        CaretAssertMessage(0, "Coordinate buffer is INVALID");
        CaretLogSevere("Coordinate buffer is INVALID");
        return;
    }
    
    switch (drawMode) {
        case PrivateDrawMode::DRAW_NORMAL:
            break;
        case PrivateDrawMode::DRAW_SELECTION:
            break;
    }
    
    /*
     * Setup vertices for drawing
     */
    glEnableClientState(GL_VERTEX_ARRAY);
    CaretAssert(glIsBuffer(coordBufferID));
    glBindBuffer(GL_ARRAY_BUFFER,
                 coordBufferID);
    glVertexPointer(openglData->m_coordinatesPerVertex,
                    openglData->m_coordinateDataType,
                    0,
                    (GLvoid*)0);
    
    /*
     * Setup normals for drawing
     */
    if (openglData->m_normalVectorBufferObject != NULL) {
        glEnableClientState(GL_NORMAL_ARRAY);
        CaretAssert(glIsBuffer(openglData->m_normalVectorBufferObject->getBufferObjectName()));
        glBindBuffer(GL_ARRAY_BUFFER, openglData->m_normalVectorBufferObject->getBufferObjectName());
        glNormalPointer(openglData->m_normalVectorDataType, 0, (GLvoid*)0);
    }
    else {
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    
    bool hasColorFlag = false;
    switch (primitive->getColorDataType()) {
        case GraphicsPrimitive::ColorDataType::NONE:
            break;
        case GraphicsPrimitive::ColorDataType::FLOAT_RGBA:
            hasColorFlag = true;
            break;
        case GraphicsPrimitive::ColorDataType::UNSIGNED_BYTE_RGBA:
            hasColorFlag = true;
            break;
    }
    /*
     * Setup colors for drawing
     */
    GLuint localColorBufferName = 0;
    if (hasColorFlag) {
        switch (drawMode) {
            case PrivateDrawMode::DRAW_NORMAL:
            {
                glEnableClientState(GL_COLOR_ARRAY);
                CaretAssert(glIsBuffer(openglData->m_colorBufferObject->getBufferObjectName()));
                glBindBuffer(GL_ARRAY_BUFFER, openglData->m_colorBufferObject->getBufferObjectName());
                glColorPointer(openglData->m_componentsPerColor, openglData->m_colorDataType, 0, (GLvoid*)0);
            }
                break;
            case PrivateDrawMode::DRAW_SELECTION:
            {
                const int32_t numberOfVertices = primitive->m_xyz.size() / 3;
                if (numberOfVertices > 0) {
                    glGenBuffers(1, &localColorBufferName);
                    CaretAssert(localColorBufferName > 0);
                    
                    const GLint  componentsPerColor = 4;
                    const GLenum colorDataType = GL_UNSIGNED_BYTE;
                    
                    const std::vector<uint8_t> selectionRGBA = primitiveSelectionHelper->getSelectionEncodedRGBA();
                    if (static_cast<int32_t>(selectionRGBA.size() / 4) == numberOfVertices) {
                        const GLuint colorSizeBytes = selectionRGBA.size() * sizeof(GLubyte);
                        const GLvoid* colorDataPointer = (const GLvoid*)&selectionRGBA[0];
                        glBindBuffer(GL_ARRAY_BUFFER,
                                     localColorBufferName);
                        glBufferData(GL_ARRAY_BUFFER,
                                     colorSizeBytes,
                                     colorDataPointer,
                                     GL_STREAM_DRAW);
                        glEnableClientState(GL_COLOR_ARRAY);
                        glColorPointer(componentsPerColor, colorDataType, 0, (GLvoid*)0);
                    }
                }
            }
                break;
        }
    }
    
    const GraphicsTextureSettings& textureSettings(primitive->getTextureSettings());
    bool hasTexture2dFlag(false);
    bool hasTexture3dFlag(false);
    switch (textureSettings.getDimensionType()) {
        case GraphicsTextureSettings::DimensionType::NONE:
            break;
        case GraphicsTextureSettings::DimensionType::FLOAT_STR_2D:
            hasTexture2dFlag = true;
            break;
        case GraphicsTextureSettings::DimensionType::FLOAT_STR_3D:
            hasTexture3dFlag = true;
            break;
    }
    
    /*
     * Setup textures for drawing 2D
     */
    if (hasTexture2dFlag
        && (drawMode == PrivateDrawMode::DRAW_NORMAL)
        && (openglData->m_textureImageDataName->getTextureName() > 0)) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, openglData->m_textureImageDataName->getTextureName());
        
        GLenum minFilter = GL_LINEAR;
        switch (textureSettings.getMinificationFilter()) {
            case GraphicsTextureMinificationFilterEnum::LINEAR:
                minFilter = GL_LINEAR;
                break;
            case GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR:
                minFilter = GL_LINEAR_MIPMAP_LINEAR;
                break;
            case GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_NEAREST:
                minFilter = GL_LINEAR_MIPMAP_NEAREST;
                break;
            case GraphicsTextureMinificationFilterEnum::NEAREST:
                minFilter = GL_NEAREST;
                break;
            case GraphicsTextureMinificationFilterEnum::NEAREST_MIPMAP_LINEAR:
                minFilter = GL_NEAREST_MIPMAP_LINEAR;
                break;
            case GraphicsTextureMinificationFilterEnum::NEAREST_MIPMAP_NEAREST:
                minFilter = GL_NEAREST_MIPMAP_NEAREST;
                break;
        }
        
        /*
         * When mip mapping is DISABLED, cannot use any of the
         * mip mapping filters.  If you do, the texture will
         * not work and likely be all white.
         */
        switch (textureSettings.getMipMappingType()) {
            case GraphicsTextureSettings::MipMappingType::DISABLED:
                if ((minFilter != GL_LINEAR)
                    && (minFilter != GL_NEAREST)) {
                    minFilter = GL_LINEAR;
                }
                break;
            case GraphicsTextureSettings::MipMappingType::ENABLED:
                break;
        }
        
        GLenum magFilter = GL_LINEAR;
        switch (textureSettings.getMagnificationFilter()) {
            case GraphicsTextureMagnificationFilterEnum::LINEAR:
                magFilter = GL_LINEAR;
                break;
            case GraphicsTextureMagnificationFilterEnum::NEAREST:
                magFilter = GL_NEAREST;
                break;
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        
        switch (textureSettings.getWrappingType()) {
            case GraphicsTextureSettings::WrappingType::CLAMP:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                break;
            case GraphicsTextureSettings::WrappingType::CLAMP_TO_BORDER:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                break;
            case GraphicsTextureSettings::WrappingType::REPEAT:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
        }
        
        const std::array<float, 4> borderColorRGBA(textureSettings.getBorderColor());
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColorRGBA.data());

        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        CaretAssert(glIsBuffer(openglData->m_textureCoordinatesBufferObject->getBufferObjectName()));
        glBindBuffer(GL_ARRAY_BUFFER, openglData->m_textureCoordinatesBufferObject->getBufferObjectName());
        glTexCoordPointer(3, openglData->m_textureCoordinatesDataType, 0, (GLvoid*)0);
    }

    /*
     * Setup textures for drawing 3D
     */
    if (hasTexture3dFlag
        && (drawMode == PrivateDrawMode::DRAW_NORMAL)
        && (openglData->m_textureImageDataName->getTextureName() > 0)) {
        glEnable(GL_TEXTURE_3D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_3D, openglData->m_textureImageDataName->getTextureName());
        
        GLenum minFilter = GL_LINEAR;
        switch (textureSettings.getMinificationFilter()) {
            case GraphicsTextureMinificationFilterEnum::LINEAR:
                minFilter = GL_LINEAR;
                break;
            case GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR:
                minFilter = GL_LINEAR_MIPMAP_LINEAR;
                break;
            case GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_NEAREST:
                minFilter = GL_LINEAR_MIPMAP_NEAREST;
                break;
            case GraphicsTextureMinificationFilterEnum::NEAREST:
                minFilter = GL_NEAREST;
                break;
            case GraphicsTextureMinificationFilterEnum::NEAREST_MIPMAP_LINEAR:
                minFilter = GL_NEAREST_MIPMAP_LINEAR;
                break;
            case GraphicsTextureMinificationFilterEnum::NEAREST_MIPMAP_NEAREST:
                minFilter = GL_NEAREST_MIPMAP_NEAREST;
                break;
        }
        
        /*
         * When mip mapping is DISABLED, cannot use any of the
         * mip mapping filters.  If you do, the texture will
         * not work and likely be all white.
         */
        switch (textureSettings.getMipMappingType()) {
            case GraphicsTextureSettings::MipMappingType::DISABLED:
                if ((minFilter != GL_LINEAR)
                    && (minFilter != GL_NEAREST)) {
                    minFilter = GL_LINEAR;
                }
                break;
            case GraphicsTextureSettings::MipMappingType::ENABLED:
                break;
        }
        
        GLenum magFilter = GL_LINEAR;
        switch (textureSettings.getMagnificationFilter()) {
            case GraphicsTextureMagnificationFilterEnum::LINEAR:
                magFilter = GL_LINEAR;
                break;
            case GraphicsTextureMagnificationFilterEnum::NEAREST:
                magFilter = GL_NEAREST;
                break;
        }
        

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minFilter);
        
        switch (textureSettings.getWrappingType()) {
            case GraphicsTextureSettings::WrappingType::CLAMP:
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
                break;
            case GraphicsTextureSettings::WrappingType::CLAMP_TO_BORDER:
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
                break;
            case GraphicsTextureSettings::WrappingType::REPEAT:
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
                break;
        }

        const std::array<float, 4> borderColorRGBA(textureSettings.getBorderColor());
        glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, borderColorRGBA.data());
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        CaretAssert(glIsBuffer(openglData->m_textureCoordinatesBufferObject->getBufferObjectName()));
        glBindBuffer(GL_ARRAY_BUFFER, openglData->m_textureCoordinatesBufferObject->getBufferObjectName());
        glTexCoordPointer(3, openglData->m_textureCoordinatesDataType, 0, (GLvoid*)0);
    }
    
    /*
     * Unbind buffers
     */
    glBindBuffer(GL_ARRAY_BUFFER,
                 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 0);
    
    /*
     * Draw the primitive
     */
    GLenum openGLPrimitiveType = GL_INVALID_ENUM;
    switch (primitive->m_primitiveType) {
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_LOOP:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_LOOP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_MITER_JOIN:
            openGLPrimitiveType = GL_LINE_LOOP;
            glLineWidth(getLineWidthForDrawingInPixels(primitive));
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINE_STRIP:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINE_STRIP_MITER_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_MITER_JOIN:
            openGLPrimitiveType = GL_LINE_STRIP;
            glLineWidth(getLineWidthForDrawingInPixels(primitive));
            break;
        case GraphicsPrimitive::PrimitiveType::MODEL_SPACE_POLYGONAL_LINES:
        case GraphicsPrimitive::PrimitiveType::OPENGL_LINES:
        case GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES:
            openGLPrimitiveType = GL_LINES;
            glLineWidth(getLineWidthForDrawingInPixels(primitive));
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_POINTS:
            openGLPrimitiveType = GL_POINTS;
            glPointSize(getPointDiameterForDrawingInPixels(primitive));
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_FAN:
            openGLPrimitiveType = GL_TRIANGLE_FAN;
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP:
            openGLPrimitiveType = GL_TRIANGLE_STRIP;
            break;
        case GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES:
            openGLPrimitiveType = GL_TRIANGLES;
            break;
        case GraphicsPrimitive::PrimitiveType::SPHERES:
            CaretAssertMessage(0, "SPHERES are not drawn with this method");
            break;
    }
    
    CaretAssert(openGLPrimitiveType != GL_INVALID_ENUM);
    
    int32_t subsetFirstVertexIndex(-1);
    int32_t subsetVertexCount(-1);
    if (primitive->getDrawArrayIndicesSubset(subsetFirstVertexIndex,
                                             subsetVertexCount)) {
        glDrawArrays(openGLPrimitiveType,
                     subsetFirstVertexIndex,
                     subsetVertexCount);
    }
    else {
        glDrawArrays(openGLPrimitiveType,
                     0, /* first index */
                     openglData->m_arrayIndicesCount);
    }
    
    /*
     * Disable drawing
     */
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    if (localColorBufferName > 0) {
        glDeleteBuffers(1, &localColorBufferName);
    }
    
    if (hasTexture2dFlag) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    if (hasTexture3dFlag) {
        glBindTexture(GL_TEXTURE_3D, 0);
        glDisable(GL_TEXTURE_3D);
    }
}

/**
 * Get the line width in pixels for drawing lines.
 * If the primitive's line width type is a percentage
 * of viewport height, convert it to pixels.
 *
 * @param primtive
 *     The graphics primitive.
 * @return
 *     Width of line in pixels.
 */
float
GraphicsEngineDataOpenGL::getLineWidthForDrawingInPixels(const GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    float width = primitive->m_lineWidthValue;
    
    switch (primitive->m_lineWidthType) {
        case GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT:
        {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            
            width = (viewport[3] * (width / 100.0f));
        }
            break;
        case GraphicsPrimitive::LineWidthType::PIXELS:
            break;
    }
    
    return width;
}

/**
 * Get the point diameter in pixels for drawing points.
 * If the primitive's point diameter type is a percentage
 * of viewport height, convert it to pixels.
 *
 * @param primtive
 *     The graphics primitive.
 * @return
 *     Diameter of point in pixels.
 */
float
GraphicsEngineDataOpenGL::getPointDiameterForDrawingInPixels(const GraphicsPrimitive* primitive)
{
    CaretAssert(primitive);
    
    float pointSize = primitive->m_pointDiameterValue;
    
    switch (primitive->m_pointSizeType) {
        case GraphicsPrimitive::PointSizeType::MILLIMETERS:
            CaretAssert(0);  /* millimeters not convertible to pixels */
            break;
        case GraphicsPrimitive::PointSizeType::PERCENTAGE_VIEWPORT_HEIGHT:
        {
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            
            pointSize = (viewport[3] * (pointSize / 100.0f));
        }
            break;
        case GraphicsPrimitive::PointSizeType::PIXELS:
            break;
    }
    
    return pointSize;
}

