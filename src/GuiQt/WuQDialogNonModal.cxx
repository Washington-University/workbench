
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

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

