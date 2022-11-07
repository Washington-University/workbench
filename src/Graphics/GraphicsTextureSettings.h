#ifndef __GRAPHICS_TEXTURE_SETTINGS_H__
#define __GRAPHICS_TEXTURE_SETTINGS_H__

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

#include <array>
#include <memory>

#include "CaretObject.h"
#include "GraphicsTextureMagnificationFilterEnum.h"
#include "GraphicsTextureMinificationFilterEnum.h"

class QImage;

namespace caret {

    class GraphicsTextureSettings : public CaretObject {
        
    public:
        /**
         * Compression type
         */
        enum class CompressionType {
            /** Disabled */
            DISABLED,
            /** Enabled */
            ENABLED
        };
        
        /**
         * Dimension type of texture components
         */
        enum class DimensionType {
            /** No texture coordinates */
            NONE,
            /** User supplie two float values per vertex contain S and T texture coordinates, R is added by code and is always 0.  Used for 2D images */
            FLOAT_STR_2D,
            /** Three float values per vertex contain S, T, and R texture coordinates.  Used for 3D images (volumes) */
            FLOAT_STR_3D
        };
        
        /**
         * Format of pixel data
         */
        enum class PixelFormatType {
            /** None - not texture primitive*/
            NONE,
            /** Blue, green, red */
            BGR,
            /** Blue, green, red, alpha */
            BGRA,
            /**
             * Blue green, red, and an alpha that is always 255 Qt's QImage::Format_RGB32
             * From QImage Doc: The image is stored using a 32-bit RGB format (0xffRRGGBB).*/
            BGRX,
            /** Red, green, blue */
            RGB,
            /** Red, green, blue, alpha */
            RGBA
        };
        
        /*
         * Location of first pixel in the texture image
         */
        enum class PixelOrigin {
            /* None - not texture primitive */
            NONE,
            /* First pixel is at bottom left of texture image*/
            BOTTOM_LEFT,
            /* First pixel is at top left of texture image*/
            TOP_LEFT
        };
        
        /**
         * Texture wrapping type
         */
        enum class WrappingType {
            /** Clamp so max STR is 1.0 (default) */
            CLAMP,
            /** Clamp so max STR is 1.0 (default).  If no texels (voxels) are available, if pixel
             maps to area outside of the volume, the border color is used. */
            CLAMP_TO_BORDER,
            /** Repeat so max STR is greater than 1.0) */
            REPEAT
        };
        
        /**
         * Texture Mip Mapping Type
         */
        enum class MipMappingType {
            /** Mip mapping disabled */
            DISABLED,
            /** Mip mapping enabled */
            ENABLED
        };
        
        static std::shared_ptr<uint8_t> allocateImageRgbaData(const int32_t imageWidth,
                                                              const int32_t imageHeight,
                                                              const int32_t imageSlices,
                                                              int64_t* optionalNumberOfBytesOut = NULL);

        GraphicsTextureSettings();

        GraphicsTextureSettings(std::shared_ptr<uint8_t>& imageRgbaData,
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
                                const std::array<float, 4>& borderColor);
        
        GraphicsTextureSettings(const uint8_t*        imageBytesPointer,
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
                                const std::array<float, 4>& borderColor);
        
        virtual ~GraphicsTextureSettings();
        
        GraphicsTextureSettings(const GraphicsTextureSettings& obj);

        GraphicsTextureSettings& operator=(const GraphicsTextureSettings& obj);
        
        const uint8_t* getImageBytesPointer() const;
        
        int32_t getImageWidth() const;
        
        int32_t getImageHeight() const;
        
        int32_t getImageSlices() const;
        
        DimensionType  getDimensionType() const;
        
        PixelFormatType getPixelFormatType() const;
        
        PixelOrigin    getPixelOrigin() const;
        
        WrappingType   getWrappingType() const;
        
        MipMappingType getMipMappingType() const;
        
        int32_t getUnpackAlignment() const;
        
        void setUnpackAlignmnet(const int32_t unpackAlignment);
        
        CompressionType getCompressionType() const;
        
        GraphicsTextureMagnificationFilterEnum::Enum getMagnificationFilter() const;
        
        void setMagnificationFilter(const GraphicsTextureMagnificationFilterEnum::Enum magnificationFilter);
        
        GraphicsTextureMinificationFilterEnum::Enum getMinificationFilter() const;
        
        void setMinificationFilter(const GraphicsTextureMinificationFilterEnum::Enum minificationFilter);
        
        std::array<float, 4> getBorderColor() const;

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        enum class ImageDataType {
            INVALID,
            POINTER,
            SHARED_PTR
        };
        
        void copyHelperGraphicsTextureSettings(const GraphicsTextureSettings& obj);

        ImageDataType m_imageDataType = ImageDataType::INVALID;
        
        std::shared_ptr<uint8_t> m_imageRgbaData;
        
        uint8_t* m_imageBytesPointer = 0;
        
        int32_t m_imageWidth = 0;
        
        int32_t m_imageHeight = 0;
        
        int32_t m_imageSlices = 0;
        
        DimensionType  m_dimensionType = DimensionType::NONE;
        
        PixelFormatType m_pixelFormatType = PixelFormatType::BGR;
        
        PixelOrigin    m_pixelOrigin = PixelOrigin::NONE;
        
        WrappingType   m_wrappingType = WrappingType::CLAMP;
        
        MipMappingType m_mipMappingType = MipMappingType::DISABLED;
        
        CompressionType m_compressionType = CompressionType::DISABLED;
        
        GraphicsTextureMagnificationFilterEnum::Enum m_magnificationFilter = GraphicsTextureMagnificationFilterEnum::LINEAR;
        
        GraphicsTextureMinificationFilterEnum::Enum m_minificationFilter = GraphicsTextureMinificationFilterEnum::LINEAR;
        
        std::array<float, 4> m_borderColor;
        
        int32_t m_unpackAlignment = 4;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_TEXTURE_SETTINGS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_TEXTURE_SETTINGS_DECLARE__

} // namespace
#endif  //__GRAPHICS_TEXTURE_SETTINGS_H__
