#ifndef __GRAPHICS_ORTHOGRAPHIC_PROJECTION_H__
#define __GRAPHICS_ORTHOGRAPHIC_PROJECTION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

namespace caret {

    class GraphicsOrthographicProjection : public CaretObject {
        
    public:
        GraphicsOrthographicProjection();
        
        GraphicsOrthographicProjection(const double left,
                                       const double right,
                                       const double bottom,
                                       const double top,
                                       const double near,
                                       const double far);
        
        virtual ~GraphicsOrthographicProjection();
        
        GraphicsOrthographicProjection(const GraphicsOrthographicProjection& obj);

        GraphicsOrthographicProjection& operator=(const GraphicsOrthographicProjection& obj);
        
        bool operator==(const GraphicsOrthographicProjection& obj) const;
        
        void applyWithOpenGL() const;
        
        bool isValid() const;
        
        bool isValid2D() const;
        
        double getHeight() const;
        
        double getWidth() const;
        
        std::array<double, 6> getAsDoubleArray() const;
        
        std::array<float, 6> getAsFloatArray() const;
        
        double getLeft() const { return m_left; }
        
        double getRight() const { return m_right; }
        
        double getBottom() const { return m_bottom; }
        
        double getTop() const { return m_top; }
        
        double getNear() const { return m_near; }
        
        double getFar() const { return m_far; }

        void resetToInvalid();
        
        void set(const double left,
                 const double right,
                 const double bottom,
                 const double top,
                 const double near,
                 const double far);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperGraphicsOrthographicProjection(const GraphicsOrthographicProjection& obj);

        double m_left = 0.0;
        
        double m_right = 0.0;
        
        double m_bottom = 0.0;
        
        double m_top = 0.0;
        
        double m_near = 0.0;
        
        double m_far = 0.0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GRAPHICS_ORTHOGRAPHIC_PROJECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GRAPHICS_ORTHOGRAPHIC_PROJECTION_DECLARE__

} // namespace
#endif  //__GRAPHICS_ORTHOGRAPHIC_PROJECTION_H__
