#ifndef __EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_H__
#define __EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_H__

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

#include "Event.h"
#include "UserInputModeEnum.h"


namespace caret {

    class EventAnnotationGetDrawingPolyhedronSliceDepth : public Event {
        
    public:
        EventAnnotationGetDrawingPolyhedronSliceDepth(const UserInputModeEnum::Enum userInputMode,
                                                      const int32_t windowIndex);
        
        virtual ~EventAnnotationGetDrawingPolyhedronSliceDepth();
        
        EventAnnotationGetDrawingPolyhedronSliceDepth(const EventAnnotationGetDrawingPolyhedronSliceDepth&) = delete;

        EventAnnotationGetDrawingPolyhedronSliceDepth& operator=(const EventAnnotationGetDrawingPolyhedronSliceDepth&) = delete;
        
        UserInputModeEnum::Enum getUserInputMode() const;
        
        int32_t getWindowIndex() const;

        bool isNumberOfSlicesDepthValid() const;
        
        int32_t getNumberOfSlicesDepth() const;
        
        void setNumberOfSlicesDepth(const float numberOfSlicesDepth);

        bool isMillimetersDepthValid() const;
        
        float getMillimetersDepth() const;
        
        void setMillimetersDepth(const float millimetersDepth);
        
        // ADD_NEW_METHODS_HERE

    private:
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_windowIndex;
        
        float m_millimetersDepth = 1.0;
        
        bool m_millimetersDepthValidFlag = false;
        
        int32_t m_numberOfSlicesDepth = 1;
        
        bool m_numberOfSlicesDepthValidFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_GET_DRAWING_POLYHEDRON_SLICE_DEPTH_H__
