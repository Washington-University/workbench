#ifndef __USER_INPUT_MODE_FOCI_WIDGET__H_
#define __USER_INPUT_MODE_FOCI_WIDGET__H_

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

#include <stdint.h>
#include <QWidget>

#include "AString.h"

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
        
    private slots:
        void createNewFocusActionTriggered();
        
        void createLastIdentificationFocusActionTriggered();
        
        void editOperationActionTriggered(QAction*);
        
        void modeComboBoxSelection(int);
        
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
        
        QActionGroup* m_editOperationActionGroup;
        
        QWidget* m_createOperationWidget;
        
        QWidget* m_editOperationWidget;
        
        QWidget* m_taskOperationWidget;
        
        QStackedWidget* m_operationStackedWidget;
        
        QString m_transformToolTipText;
        
        friend class UserInputModeFoci;
    };
    
#ifdef __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__
#endif // __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_FOCI_WIDGET__H_
