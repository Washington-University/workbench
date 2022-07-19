#ifndef __GRAPHICS_PRIMITIVE_V3F_T2F_H__
#define __GRAPHICS_PRIMITIVE_V3F_T2F_H__

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



#include <memory>

#include "GraphicsPrimitive.h"



namespace caret {

    class GraphicsPrimitiveV3fT2f : public GraphicsPrimitive {
        
    public:
        GraphicsPrimitiveV3fT2f(const PrimitiveType primitiveType,
                                const uint8_t* imageBytesPtr,
                                const int32_t imageWidth,
                                const int32_t imageHeight,
                                const int32_t imageRowStride,
                                const GraphicsTextureSettings::PixelFormatType texturePixelFormatType,
                                const GraphicsTextureSettings::PixelOrigin texturePixelOrigin,
                                const GraphicsTextureSettings::WrappingType textureWrappingType,
                                const GraphicsTextureSettings::MipMappingType textureMipMappingType,
                                const GraphicsTextureMagnificationFilterEnum::Enum textureMagnificationFilter,
                                const GraphicsTextureMinificationFilterEnum::Enum textureMinificationFilter,
                                const std::array<float, 4>& textureBorderColorRGBA);
        
        virtual ~GraphicsPrimitiveV3fT2f();
        
        GraphicsPrimitiveV3fT2f(const GraphicsPrimitiveV3fT2f& obj);

        void addVertex(const float xyz[3],
                       const float st[2]);

        void addVertex(const float x,
                       const float y,
                       const float z,
                       const float s,
                       const float t);
        
        void addVertex(const float x,
                       const float y,
                       const float s,
                       const float t);

        virtual GraphicsPrimitive* clone() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        GraphicsPrimitiveV3fT2f& operator=(const GraphicsPrimitiveV3fT2f& obj);
        
        void copyHelperGraphicsPrimitiveV3fT2f(const GraphicsPrimitiveV3fT2f& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_PRIMITIVE_V3F_T2F_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_PRIMITIVE_V3F_T2F_DECLARE__

} // namespace
#endif  //__GRAPHICS_PRIMITIVE_V3F_T2F_H__
