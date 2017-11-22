#ifndef __EVENT_OPEN_G_L_OBJECT_TO_WINDOW_TRANSFORM_H__
#define __EVENT_OPEN_G_L_OBJECT_TO_WINDOW_TRANSFORM_H__

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


#include <array>
#include <memory>

#include "Event.h"
#include "Matrix4x4.h"


namespace caret {

    class EventOpenGLObjectToWindowTransform : public Event {
        
    public:
        /**
         * Space in which data is drawn
         */
        enum class SpaceType {
            /** Transformation data that is being drawn in model space (except volume)*/
            MODEL,
            /** Transformation data that is being drawn in VOLUME  model space*/
            VOLUME_SLICE_MODEL,
            /** Transformation data that is being drawn in window space*/
            WINDOW
        };
        
        EventOpenGLObjectToWindowTransform(const SpaceType spaceType);
        
        virtual ~EventOpenGLObjectToWindowTransform();

        Matrix4x4 getMatrix() const;
        
        bool isValid() const;
        
        bool transformPoint(const float objectXYZ[3],
                            float windowXYZOut[3]) const;

        void setup(const std::array<double, 16>& modelviewMatrixArray,
                   const std::array<double, 16>& projectionMatrixArray,
                   const std::array<int32_t, 4>& viewport,
                   const std::array<double, 2>& depthRange,
                   const double centerToEyeDistance);
        
        // ADD_NEW_METHODS_HERE

    private:
        EventOpenGLObjectToWindowTransform(const EventOpenGLObjectToWindowTransform&);

        EventOpenGLObjectToWindowTransform& operator=(const EventOpenGLObjectToWindowTransform&);
        
        const SpaceType m_spaceType;
        
        Matrix4x4 m_transformMatrix;
        
        std::array<double, 16> m_projectionMatrixArray;
        
        std::array<int32_t, 4> m_viewport;
        
        std::array<double, 2> m_depthRange;
        
        double m_centerToEyeDistance;
        
        bool m_validFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_OPEN_G_L_OBJECT_TO_WINDOW_TRANSFORM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_OPEN_G_L_OBJECT_TO_WINDOW_TRANSFORM_DECLARE__

} // namespace
#endif  //__EVENT_OPEN_G_L_OBJECT_TO_WINDOW_TRANSFORM_H__
