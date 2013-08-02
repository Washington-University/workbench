#ifndef __USERNAME_PASSWORD_WIDGET_H__
#define __USERNAME_PASSWORD_WIDGET_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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
