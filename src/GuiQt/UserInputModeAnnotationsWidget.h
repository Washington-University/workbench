#ifndef __USER_INPUT_MODE_ANNOTATIONS_WIDGET_H__
#define __USER_INPUT_MODE_ANNOTATIONS_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include <QWidget>

#include "EventListenerInterface.h"

class QComboBox;

namespace caret {

    class UserInputModeAnnotations;
    
    class UserInputModeAnnotationsWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        UserInputModeAnnotationsWidget(UserInputModeAnnotations* inputModeAnnotations);
        
        virtual ~UserInputModeAnnotationsWidget();
        
        virtual void receiveEvent(Event* event);
        
        void updateWidget();
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void modeComboBoxSelection(int);

    private:
        UserInputModeAnnotationsWidget(const UserInputModeAnnotationsWidget&);

        UserInputModeAnnotationsWidget& operator=(const UserInputModeAnnotationsWidget&);
        
        QWidget* createModeWidget();
        
        UserInputModeAnnotations* m_inputModeAnnotations;
        
        QComboBox* m_modeComboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ANNOTATIONS_WIDGET_H__
