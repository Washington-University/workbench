#ifndef __KEY_EVENT_H__
#define __KEY_EVENT_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
#include <stdint.h>

#include "CaretObject.h"

class QKeyEvent;

namespace caret {
    
    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;
    
    /**
     * Contains information about a key event in the OpenGL region.
     */
    class KeyEvent : public CaretObject {
        
    public:
        KeyEvent(const BrainOpenGLViewportContent* viewportContent,
                 BrainOpenGLWidget* openGLWidget,
                 const int32_t browserWindowIndex,
                 const int32_t keyCode,
                 const int32_t mouseX,
                 const int32_t mouseY,
                 const bool mouseValidFlag,
                 const bool firstKeyPressFlag,
                 const bool shiftKeyDownFlag);
        
        virtual ~KeyEvent();
        
    private:
        void initializeMembersKeyEvent();
        
        KeyEvent(const KeyEvent& o);
        
        KeyEvent& operator=(const KeyEvent& o);
        
    public:
        AString toString() const;
        
        BrainOpenGLViewportContent* getViewportContent() const;
        
        BrainOpenGLWidget* getOpenGLWidget() const;

        int32_t getBrowserWindowIndex() const;
        
        int32_t getKeyCode() const;
        
        bool isFirstKeyPressFlag() const;
        
        bool isShiftKeyDownFlag() const;
        
        bool getMouseXY(std::array<int32_t, 2>& mouseXYOut) const;
        
    private:
        BrainOpenGLViewportContent* m_viewportContent;
        
        BrainOpenGLWidget* m_openGLWidget;
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_keyCode;
        
        int32_t m_mouseX = -1;
        
        int32_t m_mouseY = -1;
        
        bool m_mouseXYValid = false;

        const bool m_firstKeyPressFlag;
        
        const bool m_shiftKeyDownFlag;
        
    };
    
} // namespace

#endif // __KEY_EVENT_H__
