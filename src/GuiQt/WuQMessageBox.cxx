
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

