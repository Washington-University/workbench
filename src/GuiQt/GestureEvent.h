#ifndef __GESTURE_EVENT_H__
#define __GESTURE_EVENT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "CaretObject.h"



namespace caret {

    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;

    class GestureEvent : public CaretObject {
        
    public:
        enum class State {
            END,
            START,
            UPDATE
        };
        
        enum class Type {
            PINCH,
            ROTATE
        };
        
        GestureEvent(const BrainOpenGLViewportContent* viewportContent,
                     BrainOpenGLWidget* openGLWidget,
                     const int32_t browserWindowIndex,
                     const int32_t startCenterX,
                     const int32_t startCenterY,
                     const State state,
                     const Type  type,
                     const float dataValue);

        virtual ~GestureEvent();
        
        GestureEvent(const GestureEvent& obj) = delete;

        GestureEvent& operator=(const GestureEvent& obj) = delete;
        
        BrainOpenGLViewportContent* getViewportContent() const;
        
        BrainOpenGLWidget* getOpenGLWidget() const;
        
        int32_t getBrowserWindowIndex() const;
        
        int32_t getStartCenterX() const;
        
        int32_t getStartCenterY() const;

        State getState() const;
        
        Type getType() const;
        
        void getGlobalXY(const int32_t x,
                         const int32_t y,
                         int32_t& outGlobalX,
                         int32_t& outGlobalY) const;

        float getValue() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        std::unique_ptr<BrainOpenGLViewportContent> m_viewportContent;
        
        BrainOpenGLWidget* m_openGLWidget;
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_startCenterX;
        
        const int32_t m_startCenterY;
        
        const State m_state;

        const Type m_type;
        
        const float m_dataValue;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GESTURE_EVENT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GESTURE_EVENT_DECLARE__

} // namespace
#endif  //__GESTURE_EVENT_H__
