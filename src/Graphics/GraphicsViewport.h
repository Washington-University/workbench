#ifndef __GRAPHICS_VIEWPORT_H__
#define __GRAPHICS_VIEWPORT_H__

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


#include <array>
#include <memory>

#include "CaretObject.h"
#include "Vector3D.h"


namespace caret {

    class GraphicsViewport : public CaretObject {
        
    public:
        static GraphicsViewport newInstanceCurrentViewport();
        
        GraphicsViewport();
        
        GraphicsViewport(const int32_t viewport[4]);
        
        GraphicsViewport(const std::array<int32_t, 4>& viewport);
        
        GraphicsViewport(const int32_t x,
                         const int32_t y,
                         const int32_t width,
                         const int32_t height);
        
        virtual ~GraphicsViewport();
        
        GraphicsViewport(const GraphicsViewport& obj);

        GraphicsViewport& operator=(const GraphicsViewport& obj);
        
        bool operator==(const GraphicsViewport& obj) const;
        
        std::array<int32_t, 4> getViewport() const;

        std::array<float, 4> getViewportF() const;
        
        bool containsWindowXY(const int32_t windowX,
                              const int32_t windowY) const;
        
        bool containsWindowXY(const Vector3D& windowXY) const;
        
        /** @return X of viewport */
        int32_t getX() const { return m_viewport[0]; }
        
        /** @return X of viewport as float */
        float getXF() const { return m_viewport[0]; }
        
        /** @return Y of viewport */
        int32_t getY() const { return m_viewport[1]; }
        
        /** @return Y of viewport as float */
        float getYF() const { return m_viewport[1]; }
        
        /** @return Width of viewport */
        int32_t getWidth() const { return m_viewport[2]; }
        
        /** @return Width of viewport as float */
        float getWidthF() const { return m_viewport[2]; }
        
        /** @return Height of viewport */
        int32_t getHeight() const { return m_viewport[3]; }
        
        /** @return Height of viewport as float */
        float getHeightF() const { return m_viewport[3]; }
        
        /** @return Left (x) of viewport */
        int32_t getLeft() const { return m_viewport[0]; }
        
        /** @return Left (x) of viewport as float */
        float getLeftF() const { return m_viewport[0]; }
        
        /** @return Bottom (y) of viewport */
        int32_t getBottom() const { return m_viewport[1]; }
        
        /** @return Bottom (y)  of viewport as float */
        float getBottomF() const { return m_viewport[1]; }
        // ADD_NEW_METHODS_HERE

        /** @return RIght (x) of viewport */
        int32_t getRight() const { return (m_viewport[0] + m_viewport[2]); }
        
        /** @return Right (x) of viewport as float */
        float getRightF() const { return (m_viewport[0] + m_viewport[2]); }
        
        /** @return Top (y) of viewport */
        int32_t getTop() const { return (m_viewport[1] + m_viewport[3]); }
        
        /** @return Top (y)  of viewport as float */
        float getTopF() const { return (m_viewport[1] + m_viewport[3]); }
        
        /** @return X-Center  of viewport  */
        int32_t getCenterX() const { return ((getLeft() + getRight()) / 2); }
        
        /** @return X-Center  of viewport as float */
        float getCenterXF() const { return ((getLeftF() + getRightF()) / 2.0); }
        
        /** @return Y-Center  of viewport  */
        int32_t getCenterY() const { return ((getBottom() + getTop()) / 2); }
        
        /** @return Y-Center  of viewport as float */
        float getCenterYF() const { return ((getBottomF() + getTopF()) / 2.0); }
        
        /**
         * @return Bottom left corner of viewport
         */
        Vector3D getBottomLeft() const {
            return Vector3D(m_viewport[0],
                            m_viewport[1],
                            0.0);
        }

        /**
         * @return Bottom right corner of viewport
         */
        Vector3D getBottomRight() const {
            return Vector3D(m_viewport[0] + m_viewport[2],
                            m_viewport[1],
                            0.0);
        }
        
        /**
         * @return Top right corner of viewport
         */
        Vector3D getTopRight() const {
            return Vector3D(m_viewport[0] + m_viewport[2],
                            m_viewport[1] + m_viewport[3],
                            0.0);
        }
        
        /**
         * @return Top left corner of viewport
         */
        Vector3D getTopLeft() const {
            return Vector3D(m_viewport[0],
                            m_viewport[1] + m_viewport[3],
                            0.0);
        }
        
        bool isValid() const;
        
       // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
    private:
        void copyHelperGraphicsViewport(const GraphicsViewport& obj);

        std::array<int32_t, 4> m_viewport;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_VIEWPORT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_VIEWPORT_DECLARE__

} // namespace
#endif  //__GRAPHICS_VIEWPORT_H__
