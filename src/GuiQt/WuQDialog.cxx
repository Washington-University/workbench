/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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
/*LICENSE_END*/

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QImage>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>

#include "CaretAssert.h"
#include "WuQDialog.h"

using namespace caret;

/**
 * \class WuQDialog 
 * \brief Base class for dialogs.
 *
 * A base class for dialogs.
 */

/**
 * constructor.
 *
 * @param dialogTitle
 *    Title for dialog.
 * @param parent
 *    Parent widget on which this dialog is displayed.
 * @param f
 *    optional Qt::WindowFlags 
 */
WuQDialog::WuQDialog(const AString& dialogTitle,
                     QWidget* parent,
                     Qt::WindowFlags f)
   : QDialog(parent, f)
{
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    
    this->setWindowTitle(dialogTitle);
    
    this->setFocusPolicy(Qt::ClickFocus);
    
    this->userWidgetLayout = new QVBoxLayout();
    
    this->buttonBox = new QDialogButtonBox(Qt::Horizontal,
                                           this);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    dialogLayout->addLayout(this->userWidgetLayout);
    dialogLayout->addWidget(this->buttonBox);
}
              
/**
 * destructor.
 */
WuQDialog::~WuQDialog()
{
}

/**
 * Allows deletion of the dialog by the windowing
 * system when the dialog is closed.  This should only
 * be true if the dialog is dynamically allocated (with
 * new) and not explicitly delete'ed by the user's code.
 * 
 * @param deleteFlag
 *    If true, dialog will be deleted by the windowing system.
 */
void 
WuQDialog::setDeleteWhenClosed(bool deleteFlag)
{
    this->setAttribute(Qt::WA_DeleteOnClose, deleteFlag);
}

      
/**
 * Set the text of a standard button.  If the
 * text is an empty string, the button is removed.
 *
 * @param button
 *    Standard button enum identifying button.
 * @param text
 *    Text for the button.
 */
void 
WuQDialog::setStandardButtonText(QDialogButtonBox::StandardButton button,
                                 const AString& text)
{
    QPushButton* pushButton = this->buttonBox->button(button);
    if (text.isEmpty()) {
        if (pushButton != NULL) {
            this->buttonBox->removeButton(pushButton);
        }
    }
    else {
        if (pushButton == NULL) {
            pushButton = this->buttonBox->addButton(button);
        }
        pushButton->setText(text);
    }    
}

/**
 * called to capture image after timeout so nothing obscures window.
 */
void 
WuQDialog::slotCaptureImageAfterTimeOut()
{
   QImage image = QPixmap::grabWindow(this->winId()).toImage();
   if (image.isNull() == false) {
      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setImage(image);
      
      QMessageBox::information(this,
          "Information",
          "An image of this dialog has been placed onto the computer's clipboard.");
   }
}

/**
 * called to capture image of window and place it on the clipboard
 */
void 
WuQDialog::slotMenuCaptureImageOfWindowToClipboard()
{ 
   //
   // Need to delay capture so that the context sensistive
   // menu closes or else the menu will be in the captured image.
   //
   QApplication::processEvents();
   QTimer::singleShot(1000, this, SLOT(slotCaptureImageAfterTimeOut()));
}

/**
 * add a capture image of window menu item to the menu.
 */
void 
WuQDialog::addImageCaptureToMenu(QMenu* menu)
{
   menu->addAction("Capture Image to Clipboard",
                   this,
                   SLOT(slotMenuCaptureImageOfWindowToClipboard()));
}

/**
 * called by parent when context menu event occurs.
 */
void 
WuQDialog::contextMenuEvent(QContextMenuEvent* cme)
{
   //
   // Popup menu for selection of pages
   // 
   QMenu menu(this);

   //
   // Add menu item for image capture
   //
   addImageCaptureToMenu(&menu);
   
   //
   // Popup the menu
   //
   menu.exec(cme->globalPos());
}

/**
 * ring the bell.
 */
void 
WuQDialog::beep()
{
   QApplication::beep();
}

/**
 * show the watch cursor.
 */
void 
WuQDialog::showWaitCursor()
{
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

/**
 * normal cursor.
 */
void 
WuQDialog::showNormalCursor()
{
   QApplication::restoreOverrideCursor();
}

/**
 * called to close.
 */
bool 
WuQDialog::close()
{
   return QDialog::close();
}

/**
 * @return  The dialog button box for adding buttons.
 */
QDialogButtonBox* 
WuQDialog::getDialogButtonBox()
{
    return this->buttonBox;
}

/**
 * Sets the give widget to be the window's central widget.
 * Note: WuQDialog takes ownership of the widget pointer
 * and deletes it at the appropriate time.
 *
 * This should be called ONE and ONLY one time.
 *
 * @param w
 *    The central widget.
 */
void 
WuQDialog::setCentralWidget(QWidget* w)
{
    this->userWidgetLayout->addWidget(w);
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
 * @return
 *     QPushButton that was created.
 */
QPushButton* 
WuQDialog::addUserPushButton(const AString& text)
{
    QPushButton* pushButton = this->buttonBox->addButton(text, QDialogButtonBox::ApplyRole);
    return pushButton;
}

/**
 * Called when a push button was added using addUserPushButton().
 * Subclasses MUST override this if user push buttons were 
 * added using addUserPushButton().
 *
 * @param userPushButton
 *    User push button that was pressed.
 */
void 
WuQDialog::userButtonPressed(QPushButton* userPushButton)
{
    CaretAssertMessage(0, "Subclass MUST override WuQDialog::userButtonPressed to process button labeled \""
                + userPushButton->text()
                + "\"");
}

