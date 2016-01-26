
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

#include "CaretAssert.h"

#include "WuQMessageBox.h"

using namespace caret;

/**
 * Constructor.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 */
WuQMessageBox::WuQMessageBox(QWidget* parent)
: QMessageBox(parent)
{
    
}

/**
 * Destructor.
 */
WuQMessageBox::~WuQMessageBox()
{
    
}

/**
 * Display a message box with the buttons Save, Discard,
 * and Cancel.  Pressing the enter key is the equivalent of
 * pressing the Save button.
 *
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @param informativeText
 *    Displayed below 'text' if this is not empty.
 * @return
 *    QMessageBox::Save, QMessageBox::Discard, or QMessageBox::Cancel.
 */
QMessageBox::StandardButton 
WuQMessageBox::saveDiscardCancel(QWidget* parent,
                                 const QString& text,
                                 const QString& informativeText)
{
    
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    if (informativeText.isEmpty() == false) {
        msgBox.setInformativeText(informativeText);
    }
    msgBox.addButton(QMessageBox::Save);
    msgBox.addButton(QMessageBox::Discard);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setEscapeButton(QMessageBox::Cancel);
    
    QMessageBox::StandardButton buttonPressed = 
        static_cast<QMessageBox::StandardButton>(msgBox.exec());
    
    switch (buttonPressed) {
        case QMessageBox::Save:
        case QMessageBox::Discard:
        case QMessageBox::Cancel:
            break;
        default:
            CaretAssert(0);
    }
    
    return buttonPressed;
}

/**
 * Display a warning message box with Close and Cancel
 * buttons.  Pressing the enter key is the equivalent
 * of pressing the Close button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @param informativeText
 *    Displayed below 'text' if this is not empty.
 * @return
 *    true if the Close button was pressed else false
 *    if the cancel button was pressed.
 */
bool
WuQMessageBox::warningCloseCancel(QWidget* parent,
                                  const QString& text,
                                  const QString& informativeText)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    if (informativeText.isEmpty() == false) {
        msgBox.setInformativeText(informativeText);
    }
    msgBox.addButton(QMessageBox::Close);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Close);
    msgBox.setEscapeButton(QMessageBox::Cancel);

    QMessageBox::StandardButton buttonPressed =
    static_cast<QMessageBox::StandardButton>(msgBox.exec());
    
    bool closePressed = false;
    
    switch (buttonPressed) {
        case QMessageBox::Close:
            closePressed = true;
            break;
        case QMessageBox::Cancel:
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    return closePressed;
}

/**
 * Display a warning message box with Ok and Cancel
 * buttons.  Pressing the enter key is the equivalent
 * of pressing the Ok button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @param informativeText
 *    Displayed below 'text' if this is not empty.
 * @return
 *    true if the Ok button was pressed else false
 *    if the cancel button was pressed.
 */
bool
WuQMessageBox::warningOkCancel(QWidget* parent,
                               const QString& text,
                               const QString& informativeText)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    if (informativeText.isEmpty() == false) {
        msgBox.setInformativeText(informativeText);
    }
    msgBox.addButton(QMessageBox::Ok);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Cancel);
    
    QMessageBox::StandardButton buttonPressed =
    static_cast<QMessageBox::StandardButton>(msgBox.exec());
    
    bool okPressed = false;

    switch (buttonPressed) {
        case QMessageBox::Ok:
            okPressed = true;
            break;
        case QMessageBox::Cancel:
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    return okPressed;
}

/**
 * Display a warning message box with Yes and No
 * buttons.  Pressing the enter key is the equivalent
 * of pressing the Yes button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @param informativeText
 *    Displayed below 'text' if this is not empty.
 * @return
 *    true if the Yes button was pressed else false
 *    if the No button was pressed.
 */
bool
WuQMessageBox::warningYesNo(QWidget* parent,
                               const QString& text,
                               const QString& informativeText)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    if (informativeText.isEmpty() == false) {
        msgBox.setInformativeText(informativeText);
    }
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setEscapeButton(QMessageBox::No);
    
    QMessageBox::StandardButton buttonPressed =
    static_cast<QMessageBox::StandardButton>(msgBox.exec());
    
    bool yesPressed = false;
    
    switch (buttonPressed) {
        case QMessageBox::Yes:
            yesPressed = true;
            break;
        case QMessageBox::No:
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    return yesPressed;
}

/**
 * Display an error message box with the
 * given text and an OK button.
 *
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 */
void
WuQMessageBox::warningOk(QWidget* parent,
                         const QString& text)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    
    msgBox.exec();
}

/**
 * Display a warning message box with Ok and Cancel
 * buttons.  Pressing the enter key is the equivalent
 * of pressing the Ok button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @return
 *    true if the Ok button was pressed else false
 *    if the cancel button was pressed.
 */
bool
WuQMessageBox::warningOkCancel(QWidget* parent,
                               const QString& text)
{
    return WuQMessageBox::warningOkCancel(parent, text, "");
}

/**
 * Display a warning message box with Yes and No
 * buttons.  Pressing the enter key is the equivalent
 * of pressing the Yes button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @return
 *    true if the Yes button was pressed else false
 *    if the No button was pressed.
 */
bool
WuQMessageBox::warningYesNo(QWidget* parent,
                               const QString& text)
{
    return WuQMessageBox::warningYesNo(parent, text, "");
}

/**
 * Display a warning message box with Yes, No, and Cancel
 * buttons.  Pressing the enter key is the equivalent
 * of pressing the Yes button.
 *
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 * @return
 *    true if the Yes button was pressed else false
 *    if the No button was pressed.
 */
WuQMessageBox::YesNoCancelResult
WuQMessageBox::warningYesNoCancel(QWidget* parent,
                                  const QString& text,
                                  const QString& informativeText)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    if (informativeText.isEmpty() == false) {
        msgBox.setInformativeText(informativeText);
    }
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setEscapeButton(QMessageBox::No);
    
    QMessageBox::StandardButton buttonPressed =
    static_cast<QMessageBox::StandardButton>(msgBox.exec());
    
    YesNoCancelResult result = RESULT_CANCEL;
    switch (buttonPressed) {
        case QMessageBox::Yes:
            result = RESULT_YES;
            break;
        case QMessageBox::No:
            result = RESULT_NO;
            break;
        case QMessageBox::Cancel:
            result = RESULT_CANCEL;
            break;
        default:
            CaretAssert(0);
            break;
    }
    
    return result;
}


/**
 * Display an information message box with the
 * given text and an OK button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 */
void
WuQMessageBox::informationOk(QWidget* parent,
                             const QString& text)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    
    msgBox.exec();
}

/**
 * Display an error message box with the
 * given text and an OK button.
 * 
 * @param parent
 *    Parent on which message box is displayed.
 * @param text
 *    Message that is displayed.
 */
void
WuQMessageBox::errorOk(QWidget* parent,
                             const QString& text)
{
    QMessageBox msgBox(parent);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("");
    msgBox.setText(text);
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    
    msgBox.exec();
}

