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
#include <QScrollArea>
#include <QTimer>

#include "CaretAssert.h"
#include "WuQDialog.h"
#include "WuQtUtilities.h"

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
    this->autoDefaultProcessingEnabledFlag = true;
    
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    
    this->setWindowTitle(dialogTitle);
    
    this->setFocusPolicy(Qt::ClickFocus);
    
    this->userWidgetLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutMargins(this->userWidgetLayout,
                                    0,
                                    0,
                                    0);
    
    this->buttonBox = new QDialogButtonBox(Qt::Horizontal,
                                           this);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(dialogLayout,
                                    0,
                                    0,
                                    0);
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
 * Called by parent when a key press event occurs
 *
 * This code is taken from QDialog::keyPressEvent.
 * so it may need to be updated with new version of 
 * Qt.
 *
 * It is used to disable the AutoDault button 
 * property which triggers a button click when
 * the return key is pressed on a dialog.
 */ 
void 
WuQDialog::keyPressEvent(QKeyEvent* e)
{
    if (autoDefaultProcessingEnabledFlag) {
        QDialog::keyPressEvent(e);
        return;
    }
    
    bool acceptedEvent = false;
    
    //   Calls reject() if Escape is pressed. Simulates a button
    //   click for the default button if Enter is pressed. Move focus
    //   for the arrow keys. Ignore the rest.
#ifdef Q_WS_MAC
    if(e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Period) {
    } else 
#endif
        if (!e->modifiers() || (e->modifiers() & Qt::KeypadModifier && e->key() == Qt::Key_Enter)) {
            switch (e->key()) {
                case Qt::Key_Enter:
                case Qt::Key_Return:
                    e->accept();
                    acceptedEvent = true;
                    break;
                case Qt::Key_Escape:
                    break;
                default:
                    return;
            }
        }
    
    if (acceptedEvent == false) {
        QDialog::keyPressEvent(e);
    }
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
 * Unless prohibited by the second parameter, the widget
 * will be automatically placed into a scroll area if the
 * widget makes the dialog too big for the screen.
 *
 * @param widget
 *    The central widget.
 */
void 
WuQDialog::setCentralWidget(QWidget* widget,
                            const bool allowInsertingIntoScrollArea)
{
    if (allowInsertingIntoScrollArea == false) {
        this->userWidgetLayout->addWidget(widget);
        return;
    }
    
    const QSize maxSize = WuQtUtilities::getMinimumScreenSize();
    const int margin = 100;
    const int maxWidth = maxSize.width() - margin;
    const int maxHeight = maxSize.height() - margin;
    
    const int widgetWidth = widget->sizeHint().width() + 20;
    const int widgetHeight = widget->sizeHint().height() + 20;
    
    /**
     * Are contents too big for window?
     */
    if ((widgetWidth > maxWidth)
        || (widgetHeight > maxHeight)) {
        /*
         * Put contents in a scroll area
         */
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(widget);

        /*
         * Make scroll area the dialog's widget
         */
        this->userWidgetLayout->addWidget(scrollArea);

        /*
         * Estimate size for dialog
         */
        const int width = std::min(widgetWidth, maxWidth);
        const int height = std::min(widgetHeight, maxHeight);
        
        /*
         * Resize the dialog.
         */
        this->resize(width, 
                     height);
    }
    else {        
        this->userWidgetLayout->addWidget(widget);
    }
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

/**
 * Enable auto default button processing. 
 * Often it is very annoying so use this method
 * withe enable == false, to disable it.
 * 
 * @param enabled
 *   New status for auto default processing.
 */
void 
WuQDialog::setAutoDefaultButtonProcessing(bool enabled)
{
    this->autoDefaultProcessingEnabledFlag = enabled;
}


