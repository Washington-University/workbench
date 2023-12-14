#ifndef __GRAPHICS_TEXTURE_RECTANGLE_H__
#define __GRAPHICS_TEXTURE_RECTANGLE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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


#include <cstdint>
#include <memory>
#include <CaretAssert.h>
#include "CaretObject.h"
#include "Vector3D.h"

namespace caret {

    class GraphicsTextureRectangle : public CaretObject {
        
    public:
        GraphicsTextureRectangle(uint8_t* rgba,
                                 const int64_t width,
                                 const int64_t height);
        
        void copy(const GraphicsTextureRectangle& sourceRectangle,
                  const int64_t sourceX,
                  const int64_t sourceY,
                  const int64_t width,
                  const int64_t height,
                  const int64_t destinationX,
                  const int64_t destinationY);
        
        void copy(const GraphicsTextureRectangle& sourceRectangle,
                  const GraphicsTextureRectangle& targetRectangle,
                  const int64_t sourceX,
                  const int64_t sourceY,
                  const int64_t width,
                  const int64_t height,
                  const int64_t destinationX,
                  const int64_t destinationY,
                  std::vector<Vector3D>& triangleXyzOut,
                  std::vector<Vector3D>& triangleStrOut);
        
        virtual ~GraphicsTextureRectangle();
        
        GraphicsTextureRectangle(const GraphicsTextureRectangle&) = delete;

        GraphicsTextureRectangle& operator=(const GraphicsTextureRectangle&) = delete;
        
        /**
         * @return Offset of text from beginning of texture memory
         * @param x
         *    X-pixel index
         * @param y
         *    Y-pixel index
         */
        inline int64_t getOffset(const int64_t x, const int64_t y) const {
            const int64_t offset((x + (y * m_width)) * 4);
            CaretAssertArrayIndex(m_rgba, m_length, offset + 3);
            return offset;
        }
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        uint8_t* m_rgba;
        
        const int64_t m_width;
        
        const int64_t m_height;
        
        const int64_t m_length;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_TEXTURE_RECTANGLE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_TEXTURE_RECTANGLE_DECLARE__

} // namespace
#endif  //__GRAPHICS_TEXTURE_RECTANGLE_H__
