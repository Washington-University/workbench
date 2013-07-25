#ifndef __USER_INPUT_MODE_BORDERS__H_
#define __USER_INPUT_MODE_BORDERS__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include "UserInputModeView.h"

namespace caret {

    class Border;
    class UserInputModeBordersWidget;
    
    class UserInputModeBorders : public UserInputModeView {
        
    public:
        enum Mode {
            MODE_DRAW,
            MODE_EDIT,
            MODE_ROI
        };
        
        enum DrawOperation {
            DRAW_OPERATION_CREATE,
            DRAW_OPERATION_ERASE,
            DRAW_OPERATION_EXTEND,
            DRAW_OPERATION_REPLACE
        };
        
        enum EditOperation {
            EDIT_OPERATION_DELETE,
            EDIT_OPERATION_PROPERTIES
        };
        
        UserInputModeBorders(Border* borderBeingDrawnByOpenGL,
                             const int32_t windowIndex);
        
        virtual ~UserInputModeBorders();
        
        virtual UserInputMode getUserInputMode() const;
        
        virtual void initialize();
        
        virtual void finish();

        Mode getMode() const;
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftClickWithShift(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithAlt(const MouseEvent& mouseEvent);
        
        virtual QWidget* getWidgetForToolBar();
        
        virtual CursorEnum::Enum getCursor() const;

        virtual AString toString() const;
        

    private:
        /*
         * Some private methods are accessed by this friend class
         */
        friend class UserInputModeBordersWidget;
        
        UserInputModeBorders(const UserInputModeBorders&);

        UserInputModeBorders& operator=(const UserInputModeBorders&);
        
        void setMode(const Mode mode);
        
        DrawOperation getDrawOperation() const;
        
        void setDrawOperation(const DrawOperation drawOperation);
        
        EditOperation getEditOperation() const;
        
        void setEditOperation(const EditOperation editOperation);
        
        void drawPointAtMouseXY(BrainOpenGLWidget* openGLWidget,
                                const int32_t mouseX,
                                const int32_t mouseY);
        
        void drawOperationFinish();
        
        void drawOperationUndo();
        
        void drawOperationReset();
        
        void updateAfterBordersChanged();
        
        UserInputModeBordersWidget* borderToolsWidget;
        
        Mode mode;
        
        DrawOperation drawOperation;
        
        EditOperation editOperation;
        
        /** 
         * Pointer to border drawn by OpenGL.  Since owned
         * by OpenGL, DO NOT delete this!!!
         */
        Border* borderBeingDrawnByOpenGL;
        
        int32_t windowIndex;
    };
#ifdef __USER_INPUT_MODE_BORDERS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_BORDERS_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_BORDERS__H_
