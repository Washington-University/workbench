#ifndef __USER_INPUT_MODE_FOCI_WIDGET__H_
#define __USER_INPUT_MODE_FOCI_WIDGET__H_

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

#include <stdint.h>
#include <QWidget>

class QAction;
class QActionGroup;
class QComboBox;
class QStackedWidget;

namespace caret {

    class Focus;
    class FociFile;
    class UserInputModeFoci;
    
    class UserInputModeFociWidget : public QWidget {
        
        Q_OBJECT

    public:
        UserInputModeFociWidget(UserInputModeFoci* inputModeFoci,
                                const int32_t windowIndex,
                                QWidget* parent = 0);
        
        virtual ~UserInputModeFociWidget();
        
        void updateWidget();
        
        // ADD_NEW_METHODS_HERE
        
        static void deleteStaticMembers();
        
    private slots:
        void createOperationActionTriggered(QAction*);
        
        void editOperationActionTriggered(QAction*);
        
        void modeComboBoxSelection(int);
        
        void taskProjectFociTriggered();
        
    private:
        UserInputModeFociWidget(const UserInputModeFociWidget&);

        UserInputModeFociWidget& operator=(const UserInputModeFociWidget&);
        
        QWidget* createModeWidget();
        
        QWidget* createCreateOperationWidget();
        
        QWidget* createEditOperationWidget();
        
        QWidget* createTaskOperationWidget();
        
        void setActionGroupByActionData(QActionGroup* actionGroup,
                                        const int dataInteger);
        

        // ADD_NEW_MEMBERS_HERE

        UserInputModeFoci* m_inputModeFoci;
        
        const int32_t m_windowIndex;
        
        QComboBox* m_modeComboBox;
        
        QAction* m_createFociAction;
        
        QActionGroup* m_createOperationActionGroup;
        
        QActionGroup* m_editOperationActionGroup;
        
        QWidget* m_createOperationWidget;
        
        QWidget* m_editOperationWidget;
        
        QWidget* m_taskOperationWidget;
        
        QStackedWidget* m_operationStackedWidget;
        
        static FociFile* s_previousFociFile;
        static Focus* s_previousFocus;
    };
    
#ifdef __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__
    FociFile* UserInputModeFociWidget::s_previousFociFile = NULL;
    Focus* UserInputModeFociWidget::s_previousFocus = NULL;
#endif // __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_FOCI_WIDGET__H_
