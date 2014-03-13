#ifndef __USERNAME_PASSWORD_WIDGET_H__
#define __USERNAME_PASSWORD_WIDGET_H__

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


#include <QWidget>

#include "AString.h"

class QLineEdit;
class QCheckBox;

namespace caret {
    
    class UsernamePasswordWidget : public QWidget {
        
        Q_OBJECT

    public:
        UsernamePasswordWidget(QWidget* parent = 0);
        
        virtual ~UsernamePasswordWidget();
        
        bool getUsernameAndPassword(AString& usernameOut,
                                    AString& passwordOut);
        
        static bool getUserNameAndPasswordInDialog(QWidget* parent,
                                                   const AString& title,
                                                   const AString& message,
                                                   AString& usernameOut,
                                                   AString& passwordOut);
        
    private:
        UsernamePasswordWidget(const UsernamePasswordWidget&);

        UsernamePasswordWidget& operator=(const UsernamePasswordWidget&);
        
    private slots:
        void savePasswordToPreferencesClicked(bool);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        class PreviousUsernamePassword {
        public:
            PreviousUsernamePassword() {
                m_username = "";
                m_password = "";
                m_firstTime = true;
            }
            
            AString m_username;
            AString m_password;
            bool m_firstTime;
        };
        
        // ADD_NEW_MEMBERS_HERE

        QLineEdit* m_usernameLineEdit;
        QLineEdit* m_passwordLineEdit;
        
        QCheckBox* m_savePasswordToPreferencesCheckBox;
        
        static PreviousUsernamePassword s_previousUsernamePassword;
    };
    
#ifdef __USERNAME_PASSWORD_WIDGET_DECLARE__
    UsernamePasswordWidget::PreviousUsernamePassword UsernamePasswordWidget::s_previousUsernamePassword;
#endif // __USERNAME_PASSWORD_WIDGET_DECLARE__

} // namespace
#endif  //__USERNAME_PASSWORD_WIDGET_H__
