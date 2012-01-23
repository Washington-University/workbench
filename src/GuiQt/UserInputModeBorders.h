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


#include "CaretObject.h"
#include "UserInputReceiverInterface.h"

namespace caret {

    class Border;
    class UserInputModeBordersWidget;
    
    class UserInputModeBorders : public CaretObject, public UserInputReceiverInterface {
        
    public:
        enum Mode {
            MODE_CREATE,
            MODE_REVISE,
            MODE_SELECT
        };
        
        enum CreateOperation {
            CREATE_OPERATION_DRAW,
            CREATE_OPERATION_TRANSFORM
        };
        
        enum ReviseOperation {
            REVISE_OPERATION_ERASE,
            REVISE_OPERATION_EXTEND,
            REVISE_OPERATION_REPLACE,
            REVISE_OPERATION_DELETE,  
            REVISE_OPERATION_REVERSE
        };
        
        enum SelectOperation {
            SELECT_CLASS,
            SELECT_NAME
        };
        
        UserInputModeBorders(Border* borderBeingDrawnByOpenGL,
                             const int32_t windowIndex);
        
        virtual ~UserInputModeBorders();
        
        void processMouseEvent(MouseEvent* mouseEvent,
                               BrowserTabContent* browserTabContent,
                               BrainOpenGLWidget* openGLWidget);
        
        UserInputMode getUserInputMode() const;
        
        void initialize();
        
        void finish();

        QWidget* getWidgetForToolBar();
        
        Mode getMode() const;
        
        void setMode(const Mode mode);
        
        CreateOperation getCreateOperation() const;
        
        void setCreateOperation(const CreateOperation createOperation);
        
        ReviseOperation getReviseOperation() const;
        
        void setReviseOperation(const ReviseOperation reviseOperation);
        
        SelectOperation getSelectOperation() const;
        
        void setSelectOperation(const SelectOperation selectOperation);
        
    private:
        UserInputModeBorders(const UserInputModeBorders&);

        UserInputModeBorders& operator=(const UserInputModeBorders&);
        
        void drawPointAtMouseXY(BrainOpenGLWidget* openGLWidget,
                                const int32_t mouseX,
                                const int32_t mouseY);
        
    public:
        virtual AString toString() const;
        
    private:
        void createOperationFinish();
        
        void createOperationUndo();
        
        void createOperationReset();
        
        UserInputModeBordersWidget* borderToolsWidget;
        
        Mode mode;
        
        CreateOperation createOperation;
        
        ReviseOperation reviseOperation;
        
        SelectOperation selectOperation;
        
        /** 
         * Pointer to border drawn by OpenGL.  Since owned
         * by OpenGL, DO NOT delete this!!!
         */
        Border* borderBeingDrawnByOpenGL;
        
        int32_t windowIndex;
        
        friend class UserInputModeBordersWidget;
    };
#ifdef __USER_INPUT_MODE_BORDERS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_BORDERS_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_BORDERS__H_
