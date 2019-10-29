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

class QAbstractButton;
class QAction;
class QRadioButton;

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
        
        void modeRadioButtonClicked(QAbstractButton* button);
        
    private:
        UserInputModeFociWidget(const UserInputModeFociWidget&);

        UserInputModeFociWidget& operator=(const UserInputModeFociWidget&);
        
        QWidget* createModeWidget();
        
        // ADD_NEW_MEMBERS_HERE

        UserInputModeFoci* m_inputModeFoci;
        
        const int32_t m_windowIndex;
        
        QRadioButton* m_modeCreateLastIdRadioButton;
        
        QRadioButton* m_modeDeleteRadioButton;
        
        QRadioButton* m_modePropertiesRadioButton;
        
        QString m_transformToolTipText;
        
        friend class UserInputModeFoci;
    };
    
#ifdef __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__
#endif // __USER_INPUT_MODE_FOCI_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_FOCI_WIDGET__H_
