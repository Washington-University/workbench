#ifndef __USER_INPUT_MODE_FOCI__H_
#define __USER_INPUT_MODE_FOCI__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretObject.h"
#include "UserInputReceiverInterface.h"

namespace caret {

    class BrainOpenGLViewportContent;
    class BrainOpenGLWidget;
    class MouseEvent;
    class UserInputModeFociWidget;
    
    class UserInputModeFoci : public CaretObject, public UserInputReceiverInterface {
        
    public:
        enum Mode {
            MODE_CREATE,
            MODE_EDIT
        };
        
        enum EditOperation {
            EDIT_OPERATION_DELETE,
            EDIT_OPERATION_PROPERTIES
        };
        
        UserInputModeFoci(const int32_t windowIndex);
        
        virtual ~UserInputModeFoci();
        
        void processMouseEvent(MouseEvent* mouseEvent,
                               BrainOpenGLViewportContent* viewportContent,
                               BrainOpenGLWidget* openGLWidget);
        
        UserInputMode getUserInputMode() const;
        
        void initialize();
        
        void finish();
        
        QWidget* getWidgetForToolBar();
        
        virtual CursorEnum::Enum getCursor() const;
        
        Mode getMode() const;
        
        void setMode(const Mode mode);
        
        EditOperation getEditOperation() const;
        
        void setEditOperation(const EditOperation editOperation);
        
    private:
        UserInputModeFoci(const UserInputModeFoci&);

        UserInputModeFoci& operator=(const UserInputModeFoci&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        // ADD_NEW_MEMBERS_HERE
        
        const int32_t m_windowIndex;

        UserInputModeFociWidget* m_inputModeFociWidget;
        
        Mode m_mode;
        
        EditOperation m_editOperation;
    };
    
#ifdef __USER_INPUT_MODE_FOCI_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_FOCI_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_FOCI__H_
