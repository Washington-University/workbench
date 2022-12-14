#ifndef __USER_INPUT_MODE_FOCI__H_
#define __USER_INPUT_MODE_FOCI__H_

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


#include "CaretObject.h"
#include "UserInputModeView.h"

namespace caret {

    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;
    class MouseEvent;
    class Surface;
    class UserInputModeFociWidget;
    
    class UserInputModeFoci : public UserInputModeView {
        
    public:
        enum Mode {
            MODE_CREATE_AT_ID,
            MODE_DELETE,
            MODE_EDIT
        };
        
        UserInputModeFoci(const int32_t browserIndexIndex);
        
        virtual ~UserInputModeFoci();
        
        virtual void initialize();
        
        virtual void finish();
        
        virtual void update();
        
        virtual CursorEnum::Enum getCursor() const;
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
    private:
        /*
         * Note some private methods are accessed by the 
         * friend UserInputModeFociWidget.
         */
        friend class UserInputModeFociWidget;
        
        
        UserInputModeFoci(const UserInputModeFoci&);

        UserInputModeFoci& operator=(const UserInputModeFoci&);
        
        Mode getMode() const;
        
        void setMode(const Mode mode);
        
        void updateAfterFociChanged();
        
        Surface* getAnatomicalSurfaceForSurface(Surface* surface);
        
        // ADD_NEW_MEMBERS_HERE
        
        UserInputModeFociWidget* m_inputModeFociWidget;
        
        Mode m_mode;
    };
    
#ifdef __USER_INPUT_MODE_FOCI_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_FOCI_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_FOCI__H_
