#ifndef __USER_INPUT_MODE_IMAGE_WIDGET__H_
#define __USER_INPUT_MODE_IMAGE_WIDGET__H_

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
class QActionGroup;
class QRadioButton;
class QToolButton;

namespace caret {

    class UserInputModeImage;
    
    class UserInputModeImageWidget : public QWidget {
        
        Q_OBJECT

    public:
        UserInputModeImageWidget(UserInputModeImage* inputModeImage,
                                const int32_t windowIndex,
                                QWidget* parent = 0);
        
        virtual ~UserInputModeImageWidget();
        
        void updateWidget();
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void convertActionTriggered();
        
        void deleteAllActionTriggered();
        
        void addDeleteRadioButtonClicked(QAbstractButton* button);

    private:
        UserInputModeImageWidget(const UserInputModeImageWidget&);

        UserInputModeImageWidget& operator=(const UserInputModeImageWidget&);
        
        // ADD_NEW_MEMBERS_HERE

        UserInputModeImage* m_inputModeImage;
        
        const int32_t m_windowIndex;
        
        QRadioButton* m_addControlPointRadioButton;
        
        QRadioButton* m_deleteControlPointRadioButton;
        
        QToolButton* m_deleteAllToolButton;
        
        QToolButton* m_convertToolButton;
        
        QString m_transformToolTipText;
        
        friend class UserInputModeImage;
    };
    
#ifdef __USER_INPUT_MODE_IMAGE_WIDGET_DECLARE__
#endif // __USER_INPUT_MODE_IMAGE_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_IMAGE_WIDGET__H_
