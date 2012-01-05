
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

using namespace caret;


    
/**
 * \class WuQDialogNonModal 
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
}

/**
 * Destructor.
 */
WuQDialogNonModal::~WuQDialogNonModal()
{
    
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
    QDialogButtonBox::ButtonRole buttonRole = this->getDialogButtonBox()->buttonRole(button);
    
    if (buttonRole == QDialogButtonBox::ApplyRole) {
        this->applyButtonPressed();
    }
    else if (buttonRole == QDialogButtonBox::RejectRole) {
        this->closeButtonPressed();
    }
    else {
        CaretAssertMessage(0, "Invalid button role: " + buttonRole);
    }
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

