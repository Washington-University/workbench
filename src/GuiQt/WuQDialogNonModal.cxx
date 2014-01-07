
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

#define __WU_Q_DIALOG_NON_MODAL_DECLARE__
#include "WuQDialogNonModal.h"
#undef __WU_Q_DIALOG_NON_MODAL_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
using namespace caret;


    
/**
 * \class caret::WuQDialogNonModal 
 * \brief Base class for non-modal dialogs.
 *
 * A base class for non-modal dialogs.
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
WuQDialogNonModal::WuQDialogNonModal(const AString& dialogTitle,
                                     QWidget* parent,
                                     Qt::WindowFlags f)
: WuQDialog(dialogTitle,
            parent,
            f)
{
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Apply);
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Close);
    
    QObject::connect(this->getDialogButtonBox(), SIGNAL(clicked(QAbstractButton*)),
                     this, SLOT(clicked(QAbstractButton*)));
    
    this->getDialogButtonBox()->button(QDialogButtonBox::Apply)->setDefault(false);
    this->getDialogButtonBox()->button(QDialogButtonBox::Apply)->setAutoDefault(false);
    this->getDialogButtonBox()->button(QDialogButtonBox::Close)->setDefault(false);
    this->getDialogButtonBox()->button(QDialogButtonBox::Close)->setAutoDefault(false);
    
    m_isPositionRestoredWhenReopened = false;
    m_positionWhenClosedValid = false;
}

/**
 * Destructor.
 */
WuQDialogNonModal::~WuQDialogNonModal()
{
    
}

/**
 * Gets called when the dialog is closing.
 * Overriden so that position of dialog 
 * can be saved.
 */
void 
WuQDialogNonModal::closeEvent(QCloseEvent* event)
{
    if (m_isPositionRestoredWhenReopened) {
        /*
         * Save position and size of dialog so that 
         * when it is shown next time, it will be
         * in the position and size as when it was
         * closed.
         */
        m_positionWhenClosedValid = true;
        m_positionWhenClosed = this->pos();
        m_sizeWhenClosed = this->size();
    }
    
    WuQDialog::closeEvent(event);
    
    emit dialogWasClosed();
}

/**
 * Gets called when the dialog is to be displayed.
 */
void 
WuQDialogNonModal::showEvent(QShowEvent* event)
{
    if (m_isPositionRestoredWhenReopened) {
        if (m_positionWhenClosedValid) {
            /*
             * Restore the dialog in the position and size that it
             * was in when closed.  Use move() for position and
             * the size hint for the size.
             */
            this->move(m_positionWhenClosed);
            const int32_t w = m_sizeWhenClosed.width();
            const int32_t h = m_sizeWhenClosed.height();
            if ((w > 0)
                && (h > 0)) {
                this->setDialogSizeHint(w,
                                        h);
                adjustSize();
            }
        }
    }
    
    WuQDialog::showEvent(event);
}


/**
 * This slot can be called and it simply calls
 * applyButtonPressed.  This slot can be connected
 * to GUI components.
 */
void 
WuQDialogNonModal::apply()
{
    this->applyButtonPressed();
}

/**
 * Called when a button is pressed.
 */
void 
WuQDialogNonModal::clicked(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton standardButton = this->getDialogButtonBox()->standardButton(button);
    if (standardButton == QDialogButtonBox::Apply) {
        this->applyButtonPressed();
    }
    else if (standardButton == QDialogButtonBox::Close) {
        this->closeButtonPressed();
    }
    else if (standardButton == QDialogButtonBox::Help) {
        this->helpButtonClicked();
    }
    else {
        QPushButton* pushButton = dynamic_cast<QPushButton*>(button);
        CaretAssert(pushButton);
        const NonModalDialogUserButtonResult result = this->userButtonPressed(pushButton);        
        switch (result) {
            case RESULT_CLOSE:
                close();
                break;
            case RESULT_NONE:
                break;
        };
    }

    
    
//    QDialogButtonBox::ButtonRole buttonRole = this->getDialogButtonBox()->buttonRole(button);
//    
//    if (buttonRole == QDialogButtonBox::ApplyRole) {
//        this->applyButtonPressed();
//    }
//    else if (buttonRole == QDialogButtonBox::RejectRole) {
//        this->closeButtonPressed();
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
WuQDialogNonModal::NonModalDialogUserButtonResult 
WuQDialogNonModal::userButtonPressed(QPushButton* userPushButton)
{
    const AString msg = ("Subclass of WuQDialogNonModal added a user pushbutton but failed to override userButtonPressed for button labeled \""
                         + userPushButton->text()
                         + "\"");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
    
    return RESULT_NONE;    
}

/**
 * Called when the Apply button is pressed.
 * If needed should override this to process
 * data when the Apply button is pressed.
 */
void 
WuQDialogNonModal::applyButtonPressed()
{
    
}

/**
 * Called when the Close button is pressed.
 * If needed should override this to process
 * data when the Close button is pressed.
 */
void 
WuQDialogNonModal::closeButtonPressed()
{
    this->close();
}

/**
 * Set the Apply button to the given text.  If the text
 * is zero length, the Apply button is removed.
 *
 * @text
 *    Text for OK button.
 */
void 
WuQDialogNonModal::setApplyButtonText(const AString& text)
{
    this->setStandardButtonText(QDialogButtonBox::Apply, text);
}

/**
 * Set the Close button to the given text.  If the text
 * is zero length, the Close button is removed.
 *
 * @text
 *    Text for OK button.
 */
void 
WuQDialogNonModal::setCloseButtonText(const AString& text)
{
    this->setStandardButtonText(QDialogButtonBox::Close, text);
}

/**
 * If the given parameter is true, save the position of this
 * dialog when it is closed.  Next time window is displayed 
 * in the current session, use the position at the time the
 * dialog was closed.
 * @param saveIt
 *   If true save the position for next time.
 */
void 
WuQDialogNonModal::setSaveWindowPositionForNextTime(const bool saveIt)
{
    m_isPositionRestoredWhenReopened = saveIt;
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
WuQDialogNonModal::addUserPushButton(const AString& text,
                             const QDialogButtonBox::ButtonRole buttonRole)
{
    QPushButton* pushButton = getDialogButtonBox()->addButton(text, 
                                                              buttonRole);
    return pushButton;
}


