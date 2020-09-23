
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __CARET_RESULT_DIALOG_DECLARE__
#include "CaretResultDialog.h"
#undef __CARET_RESULT_DIALOG_DECLARE__

#include <QMessageBox>

#include "CaretAssert.h"
#include "CaretResult.h"

using namespace caret;


    
/**
 * \class caret::CaretResultDialog 
 * \brief Convenience for display of a 'CaretResult' instance in a message box
 * \ingroup GuiQt
 *
 */

/**
 * Run the 'dialog'.  A dialog is displayed if there is an error or if the description is not empty.
 * @param caretResult
 *    The caret result instance
 * @param parent
 *    Parent for any dialog that is dispalyed.
 * @return True if the CaretResult is NO error, else false.
 */
bool
CaretResultDialog::run(const std::unique_ptr<CaretResult>& caretResult,
                       QWidget* parent)
{
    QString description = caretResult->getErrorDescription();
    
    QString dialogTitle;
    QMessageBox::Icon dialogIcon(QMessageBox::NoIcon);
    bool successFlag(false);
    bool showDialogFlag(false);
    switch (caretResult->getErrorStatusCode()) {
        case CaretResult::GENERIC_ERROR:
            showDialogFlag = true;
            if (description.isEmpty()) {
                description = "Unknown error occurred.";
            }
            dialogIcon = QMessageBox::Critical;
            break;
        case CaretResult::NO_ERROR:
            successFlag = true;
            if ( ! description.isEmpty()) {
                showDialogFlag = true;
            }
            dialogIcon = QMessageBox::Information;
            break;
    }
    
    if (showDialogFlag) {
        QMessageBox msgBox(dialogIcon,
                           dialogTitle,
                           description,
                           QMessageBox::Ok,
                           parent);
        msgBox.exec();
    }
    return successFlag;
}

/**
 * Run the 'dialog'.  A dialog is displayed if there is an error or if the description is not empty.
 * @param caretResult
 *    The caret result instance
 * @param parent
 *    Parent for any dialog that is dispalyed.
 * @return True if the result instance is no error, else false.
 */
bool
CaretResultDialog::isError(const std::unique_ptr<CaretResult>& caretResult,
                           QWidget* parent)
{
    return ( ! CaretResultDialog::run(caretResult,
                                      parent));
}

/**
 * Run the 'dialog'.  A dialog is displayed if there is an error or if the description is not empty.
 * @param caretResult
 *    The caret result instance
 * @param parent
 *    Parent for any dialog that is dispalyed.
 * @return True if the result instance is no error, else false.
 */
bool
CaretResultDialog::isSuccess(const std::unique_ptr<CaretResult>& caretResult,
                             QWidget* parent)
{
    return CaretResultDialog::run(caretResult,
                                  parent);
}


