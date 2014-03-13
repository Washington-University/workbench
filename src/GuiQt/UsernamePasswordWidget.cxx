
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

#define __USERNAME_PASSWORD_WIDGET_DECLARE__
#include "UsernamePasswordWidget.h"
#undef __USERNAME_PASSWORD_WIDGET_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "SessionManager.h"
#include "WuQDialogModal.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::UsernamePasswordWidget 
 * \brief Widget for username and password with saving to preferences.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent widget.
 */
UsernamePasswordWidget::UsernamePasswordWidget(QWidget* parent)
: QWidget(parent)
{
    QLabel* usernameLabel = new QLabel("User Name: ");
    m_usernameLineEdit = new QLineEdit();
    m_usernameLineEdit->setFixedWidth(200);
    
    QLabel* passwordLabel = new QLabel("Password: ");
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setFixedWidth(200);
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    
    m_savePasswordToPreferencesCheckBox = new QCheckBox("Save Password to Preferences");
    QObject::connect(m_savePasswordToPreferencesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(savePasswordToPreferencesClicked(bool)));
    
    int row = 0;
    QGridLayout* loginGridLayout = new QGridLayout(this);
    loginGridLayout->setColumnStretch(0, 0);
    loginGridLayout->setColumnStretch(1, 100);
    loginGridLayout->addWidget(usernameLabel, row, 0);
    loginGridLayout->addWidget(m_usernameLineEdit, row, 1);
    row++;
    loginGridLayout->addWidget(passwordLabel, row, 0);
    loginGridLayout->addWidget(m_passwordLineEdit, row, 1);
    row++;
    loginGridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                               row, 0, 1, 2);
    row++;
    loginGridLayout->addWidget(m_savePasswordToPreferencesCheckBox,
                               row, 0, 1, 2, Qt::AlignLeft);
    row++;

    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    if (s_previousUsernamePassword.m_firstTime) {
        s_previousUsernamePassword.m_firstTime = false;
        
        AString userName;
        AString password;
        prefs->getRemoteFileUserNameAndPassword(userName,
                                                password);
        s_previousUsernamePassword.m_username = userName;
        if (prefs->isRemoteFilePasswordSaved()) {
            s_previousUsernamePassword.m_password = password;
        }
        else {
            s_previousUsernamePassword.m_password = "";
        }
    }
    m_usernameLineEdit->setText(s_previousUsernamePassword.m_username);
    m_passwordLineEdit->setText(s_previousUsernamePassword.m_password);
    m_savePasswordToPreferencesCheckBox->setChecked(prefs->isRemoteFilePasswordSaved());
}

/**
 * Destructor.
 */
UsernamePasswordWidget::~UsernamePasswordWidget()
{
}

/**
 * Popup a modal dialog for the username and password.
 *
 * @param parent
 *    Parent on which dialog is displayed.
 * @param title
 *    Title of dialog.
 * @param message
 *    Optional message shown in dialog.
 * @param usernameOut
 *    The username.
 * @param passwordOut
 *    The password.
 * @return
 *    True if the user pressed the OK button, else false.
 */
bool
UsernamePasswordWidget::getUserNameAndPasswordInDialog(QWidget* parent,
                                                       const AString& title,
                                                       const AString& message,
                                                       AString& usernameOut,
                                                       AString& passwordOut)
{
    WuQDialogModal dialog(title,
                          parent);
    dialog.setWindowTitle(title);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    if (message.isEmpty() == false) {
        QLabel* messageLabel = new QLabel(message);
        messageLabel->setWordWrap(true);
        dialogLayout->addWidget(messageLabel);
    }
    UsernamePasswordWidget* userNameAndPasswordWidget = new UsernamePasswordWidget();
    dialogLayout->addWidget(userNameAndPasswordWidget);
    
    dialog.setCentralWidget(dialogWidget,
                            WuQDialog::SCROLL_AREA_NEVER);
    
    if (dialog.exec() == WuQDialogModal::Accepted) {
        userNameAndPasswordWidget->getUsernameAndPassword(usernameOut,
                                                          passwordOut);
        return true;
    }
    
    return false;
}


/**
 * Get the username and password.  If the both the username and password
 * are valid (returns true) and "Save Password to Preferences" is checked,
 * the username and password are written to the user's preferences.  If it is
 * not checked, only user username is written to preferences.
 *
 * @param usernameOut
 *    The username.
 * @param passwordOut
 *    The password.
 * @return
 *    True if both the username and password are non-empty, else false.
 */
bool
UsernamePasswordWidget::getUsernameAndPassword(AString& usernameOut,
                                               AString& passwordOut)
{
    usernameOut = m_usernameLineEdit->text().trimmed();
    passwordOut = m_passwordLineEdit->text().trimmed();
    
    if (usernameOut.isEmpty()) {
        return false;
    }
    else if (passwordOut.isEmpty()) {
        return false;
    }
    
    const bool savePassword = m_savePasswordToPreferencesCheckBox->isChecked();
    
    CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
    prefs->setRemoteFilePasswordSaved(savePassword);
    if (savePassword) {
        prefs->setRemoteFileUserNameAndPassword(usernameOut,
                                                passwordOut);
    }
    else {
        prefs->setRemoteFileUserNameAndPassword(usernameOut,
                                                "");
    }
    
    s_previousUsernamePassword.m_username = usernameOut;
    s_previousUsernamePassword.m_password = passwordOut;
    
    return true;
}

/**
 * Called when save password to preferences checkbox value is changed
 * by the user.
 *
 * @param status
 *   New status of save password to preferences checkbox.
 */
void
UsernamePasswordWidget::savePasswordToPreferencesClicked(bool status)
{
    if (status) {
        const QString msg = ("The Workbench preferences are stored in a file somewhere in your "
                             "home directory and the location depends upon your operating "
                             "system.  This is not a secure file and it may be possible "
                             "other users to access this file and find your "
                             "open location password.  Unchceck the box if you do not want "
                             "your password saved within your preferences.");
        WuQMessageBox::informationOk(m_savePasswordToPreferencesCheckBox,
                                     WuQtUtilities::createWordWrappedToolTipText(msg));
    }
}

