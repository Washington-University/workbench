
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

#define __WU_Q_DIALOG_MODAL_DECLARE__
#include "WuQDialogModal.h"
#undef __WU_Q_DIALOG_MODAL_DECLARE__

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class WuQDialogModal 
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
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Ok);
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Cancel);
    
    QObject::connect(this->getDialogButtonBox(), SIGNAL(clicked(QAbstractButton*)),
                     this, SLOT(clicked(QAbstractButton*)));
    
    this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setDefault(true);
    this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setAutoDefault(true);
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
    
    this->setCentralWidget(centralWidget);
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
    QDialogButtonBox::ButtonRole buttonRole = this->getDialogButtonBox()->buttonRole(button);
    
    if (buttonRole == QDialogButtonBox::AcceptRole) {
        this->okButtonPressed();
    }
    else if (buttonRole == QDialogButtonBox::RejectRole) {
        this->cancelButtonPressed();
    }
    else {
        QPushButton* pushButton = dynamic_cast<QPushButton*>(button);
        CaretAssert(pushButton);
        this->userButtonPressed(pushButton);
    }
}

/**
 * Called when the OK button is pressed.
 * If needed should override this to process
 * data when the OK button is pressed and then
 * call this to issue the accept signal.
 */
void 
WuQDialogModal::okButtonPressed()
{
    this->accept();
}

/**
 * Called when the Cancel button is pressed.
 * If needed should override this to process
 * data when the Cancel button is pressed.
 * Call this to issue the reject signal.
 */
void 
WuQDialogModal::cancelButtonPressed()
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

    if (text.isEmpty()) {
        this->getDialogButtonBox()->button(QDialogButtonBox::Cancel)->setDefault(true);
        this->getDialogButtonBox()->button(QDialogButtonBox::Cancel)->setAutoDefault(true);
    }
    else {
        this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setDefault(true);
        this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setAutoDefault(true);
    }
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

