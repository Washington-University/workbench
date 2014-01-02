/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QPushButton>

#define __WU_Q_DIALOG_MODAL_DECLARE__
#include "WuQDialogModal.h"
#undef __WU_Q_DIALOG_MODAL_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::WuQDialogModal 
 * \brief Base class for modal dialogs.
 *
 * A base class for modal dialogs.
 */

/**
 * Constructs a modal dialog.  After construction,
 * use exec() to display the dialog.
 *
 * @param dialogTitle
 *    Title for dialog.
 * @param parent
 *    Parent widget on which this dialog is displayed.
 * @param f
 *    optional Qt::WindowFlags 
 */
WuQDialogModal::WuQDialogModal(const AString& dialogTitle,
                               QWidget* parent,
                               Qt::WindowFlags f)
: WuQDialog(dialogTitle,
            parent, 
            f)
{
    m_isSaveDialogPosition = false;
    
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Ok);
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Cancel);
    
    QObject::connect(this->getDialogButtonBox(), SIGNAL(clicked(QAbstractButton*)),
                     this, SLOT(clicked(QAbstractButton*)));
    
    this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setDefault(true);
    //this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setAutoDefault(true);
}

/**
 * Constructs a modal dialog.  After construction,
 * use exec() to display the dialog.
 *
 * @param dialogTitle
 *    Title for dialog.
 * @param centralWidget,
 *    Central widget that is displayed in the dialog.
 * @param parent
 *    Parent widget on which this dialog is displayed.
 * @param f
 *    optional Qt::WindowFlags 
 */
WuQDialogModal::WuQDialogModal(const AString& dialogTitle,
                               QWidget* centralWidget,
                               QWidget* parent,
                               Qt::WindowFlags f)
: WuQDialog(dialogTitle,
            parent, 
            f)
{
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Ok);
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Cancel);
    
    QObject::connect(this->getDialogButtonBox(), SIGNAL(clicked(QAbstractButton*)),
                     this, SLOT(clicked(QAbstractButton*)));
    
    this->setCentralWidget(centralWidget,
                           WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
WuQDialogModal::~WuQDialogModal()
{
    
}

/**
 * Called when a button is pressed.
 */
void 
WuQDialogModal::clicked(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton standardButton = this->getDialogButtonBox()->standardButton(button);
    if (standardButton == QDialogButtonBox::Ok) {
        this->okButtonClicked();
    }
    else if (standardButton == QDialogButtonBox::Cancel) {
        this->cancelButtonClicked();
    }
    else if (standardButton == QDialogButtonBox::Help) {
        this->helpButtonClicked();
    }
    else {
        QPushButton* pushButton = dynamic_cast<QPushButton*>(button);
        CaretAssert(pushButton);
        const ModalDialogUserButtonResult result = this->userButtonPressed(pushButton);        
        switch (result) {
            case RESULT_ACCEPT:
                accept();
                break;
            case RESULT_REJECT:
                reject();
                break;
            case RESULT_NONE:
                break;
        };
    }
    
    
//    QDialogButtonBox::ButtonRole buttonRole = this->getDialogButtonBox()->buttonRole(button);
//    
//    if (buttonRole == QDialogButtonBox::AcceptRole) {
//        this->okButtonClicked();
//    }
//    else if (buttonRole == QDialogButtonBox::RejectRole) {
//        this->cancelButtonClicked();
//    }
//    else {
//        QPushButton* pushButton = dynamic_cast<QPushButton*>(button);
//        CaretAssert(pushButton);
//        this->userButtonPressed(pushButton);
//    }
}

/**
 * Called when a push button was added using addUserPushButton().
 * Subclasses MUST override this if user push buttons were 
 * added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 * @return 
 *    The result that indicates action that should be taken
 *    as a result of the button being pressed.
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"
WuQDialogModal::ModalDialogUserButtonResult
WuQDialogModal::userButtonPressed(QPushButton* userPushButton)
{
    CaretAssertMessage(0, "Subclass of WuQDialogModal added a user pushbutton but failed to override userButtonPressed for button labeled \""
                       + userPushButton->text()
                       + "\"");

    return RESULT_NONE;    
}

/**
 * Called when the OK button is clicked.
 * If needed should override this to process
 * data when the OK button is clicked and then
 * call this to issue the accept signal.
 */
void 
WuQDialogModal::okButtonClicked()
{
    this->accept();
}

/**
 * Called when the Cancel button is clicked.
 * If needed should override this to process
 * data when the Cancel button is clicked.
 * Call this to issue the reject signal.
 */
void 
WuQDialogModal::cancelButtonClicked()
{
    this->reject();
}

/**
 * Set the OK button to the given text.  If the text
 * is zero length, the OK button is removed.
 *
 * @text
 *    Text for OK button.
 */
void 
WuQDialogModal::setOkButtonText(const AString& text)
{
    this->setStandardButtonText(QDialogButtonBox::Ok, text);
/*
    if (text.isEmpty()) {
        this->getDialogButtonBox()->button(QDialogButtonBox::Cancel)->setDefault(true);
        this->getDialogButtonBox()->button(QDialogButtonBox::Cancel)->setAutoDefault(true);
    }
    else {
        this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setDefault(true);
        this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setAutoDefault(true);
    }
*/
}

/**
 * Set the Cancel button to the given text.  If the text
 * is zero length, the Cancel button is removed.
 *
 * @text
 *    Text for OK button.
 */
void 
WuQDialogModal::setCancelButtonText(const AString& text)
{
    this->setStandardButtonText(QDialogButtonBox::Cancel, text);
}

/**
 * Adds a button to the dialog.  When the button is
 * pressed, userButtonPressed(QPushButton*) will be
 * called with the button that was created and returned
 * by this method.  The subclass of the dialog MUST
 * override userButtonPressed(QPushButton*).
 *
 * @param text
 *     Text for the pushbutton.
 * @param buttonRole
 *     Role of button.  NOTE: This is used for placement of buttons in
 *     the appropriate location for the operating system.  Any action,
 *     such as closing the dialog will not occur because of this button
 *     push.
 * @return
 *     QPushButton that was created.
 */
QPushButton* 
WuQDialogModal::addUserPushButton(const AString& text,
                             const QDialogButtonBox::ButtonRole buttonRole)
{
    QPushButton* pushButton = getDialogButtonBox()->addButton(text, 
                                                              buttonRole);
    return pushButton;
}

/**
 * If this method is called, each time the dialog is closed, it will
 * save the position of the dialog and restore the dialog to that
 * position when the dialog is reopened.
 *
 * @param savePositionName
 *    Name used for saving the dialog's position.  If the default value
 *    (an empty string), the dialog's title is used for saving the
 *    dialog's position.  Note that is there is more than one dialog
 *    with the same name positions may not be correctly restored.
 */
void
WuQDialogModal::setSaveWindowPositionForNextTime(const AString& savePositionName)
{
    m_isSaveDialogPosition = true;
    m_saveDialogPositionName = savePositionName;
    if (m_saveDialogPositionName.isEmpty()) {
        m_saveDialogPositionName = windowTitle();
        if (m_saveDialogPositionName.isEmpty()) {
            m_isSaveDialogPosition = false;
        }
    }
}

/**
 * Shows/hides a widget.
 * Override to optionally place dialog via values passed to setDisplayedXY.
 */
void
WuQDialogModal::setVisible(bool visible)
{
    WuQDialog::setVisible(visible);
    
    if (m_isSaveDialogPosition) {
        /*
         * Find previous position of dialog.
         */
        std::map<QString, SavedPosition>::iterator iter = s_savedDialogPositions.find(m_saveDialogPositionName);
        
        if (visible) {
            if (iter != s_savedDialogPositions.end()) {
                /*
                 * Restore dialog position
                 */
                SavedPosition savedPosition = iter->second;
                if ((savedPosition.x > 0)
                    && (savedPosition.y > 0)) {
                    move(savedPosition.x,
                         savedPosition.y);
                }
                if ((savedPosition.w > 0)
                    && (savedPosition.h > 0)) {
                    resize(savedPosition.w,
                           savedPosition.h);
                }
            }
        }
        else {
            /*
             * Save position of dialog.
             */
            SavedPosition savedPosition;
            savedPosition.x = x();
            savedPosition.y = y();
            savedPosition.w = width();
            savedPosition.h = height();
            
            if (iter != s_savedDialogPositions.end()) {
                /*
                 * Replace dialog position
                 */
                iter->second = savedPosition;
            }
            else {
                /*
                 * Insert dialog position
                 */
                s_savedDialogPositions.insert(std::make_pair(m_saveDialogPositionName,
                                                             savedPosition));
            }
        }
    }
}



