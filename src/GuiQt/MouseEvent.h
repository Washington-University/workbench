#ifndef __MouseEvent_H__
#define __MouseEvent_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QMouseEvent>

#include "CaretObject.h"
#include "MouseEventTypeEnum.h"

#include <stdint.h>

class QMouseEvent;

namespace caret {
    
    /**
     * Contains information about a mouse event in the OpenGL region.
     */
    class MouseEvent : public CaretObject {
        
    public:
        MouseEvent(const MouseEventTypeEnum::Enum mouseEventType,
                   const Qt::KeyboardModifiers keyModifiers,
                   const int x,
                   const int y,
                   const int dx,
                   const int dy);
        
        MouseEvent(const QWheelEvent& event);
        
        virtual ~MouseEvent();
        
    private:
        void initializeMembersMouseEvent();
        
        MouseEvent(const MouseEvent& o);
        
        MouseEvent& operator=(const MouseEvent& o);
        
    public:
        bool isValid() const;
        
        AString toString() const;
        
        int32_t getDx() const;
        
        int32_t getDy() const;
        
        MouseEventTypeEnum::Enum getMouseEventType() const;
        
        int32_t getX() const;
        
        int32_t getY() const;
        
        int32_t getWheelRotation() const;
        
        bool isControlAndShiftKeyDown() const;
        
        bool isControlKeyDown() const;
        
        bool isShiftKeyDown() const;
        
        bool isAnyKeyDown() const;
        
        void setNoKeysDown();
        
        bool isGraphicsUpdateOneWindowRequested() const;
        
        void setGraphicsUpdateOneWindowRequested();
        
        bool isGraphicsUpdateAllWindowsRequested() const;
        
        void setGraphicsUpdateAllWindowsRequested();
        
        bool isUserInterfaceUpdateRequested() const;
        
        void setUserInterfaceUpdateRequested();
        
    private:
        MouseEventTypeEnum::Enum mouseEventType;
        
        int32_t x;
        
        int32_t y;
        
        int32_t dx;
        
        int32_t dy;

        int32_t wheelRotation;
        
        bool keyDownControlAndShift;
        
        bool keyDownControl;
        
        bool keyDownShift;
        
        bool graphicsUpdateOneWindowRequested;
        
        bool graphicsUpdateAllWindowsRequested;
        
        bool userInterfaceUpdateRequested;
        
    };
    
} // namespace

#endif // __MouseEvent_H__
