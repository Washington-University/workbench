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
#define __WU_QDIALOG_DECLARE__
#include "WuQDialog.h"
#undef __WU_QDIALOG_DECLARE__
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::WuQDialog 
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
                                    0);
    
    m_layoutLeftOfButtonBox = new QHBoxLayout();
    m_layoutLeftOfButtonBox->setContentsMargins(0, 0, 0, 0);
    
    this->buttonBox = new QDialogButtonBox(Qt::Horizontal,
                                           this);
    
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addLayout(m_layoutLeftOfButtonBox,
                            0);
    bottomLayout->addWidget(this->buttonBox,
                            1000);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(dialogLayout,
                                    0,
                                    0);
    dialogLayout->addLayout(this->userWidgetLayout);
    dialogLayout->addLayout(bottomLayout);
}
              
/**
 * destructor.
 */
WuQDialog::~WuQDialog()
{
}

/**
 * Add a widget to the left of the buttons at the bottom of the dialog.
 * More than one widget can be added and the first widget will be on 
 * the left-most side.
 * 
 * @param widget
 *    Widget to add.
 */
void
WuQDialog::addWidgetToLeftOfButtons(QWidget* widget)
{
    m_layoutLeftOfButtonBox->addWidget(widget);
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
 * Sets the given widgets to be the window's top and central widget.
 * Note: WuQDialog takes ownership of the widget pointer
 * and deletes it at the appropriate time.
 *
 * This should be called ONE and ONLY one time.
 *
 * Unless prohibited by the second parameter, the central widget
 * will be automatically placed into a scroll area if the
 * widget makes the dialog too big for the screen.
 *
 * @param topWidget
 *    The optional widget displayed at top (may be NULL).
 * @param centralWidget
 *    The central widget.
 * @param bottomWidget
 *    The optional widget displayed at bottom (may be NULL).
 * @param allowInsertingIntoScrollArea
 *    If true, a scroll area may be added around the central widget.
 */
void 
WuQDialog::setTopBottomAndCentralWidgets(QWidget* topWidget,
                                         QWidget* centralWidget,
                                         QWidget* bottomWidget,
                                         const bool allowInsertingIntoScrollArea)
{
    CaretAssert(centralWidget);
    this->setTopBottomAndCentralWidgetsInternal(topWidget,
                                                centralWidget,
                                                bottomWidget,
                                                allowInsertingIntoScrollArea);
    
}

/**
 * Sets the give widget to be the window's central widget.
 * Note: WuQDialog takes ownership of the widget pointer
 * and deletes it at the appropriate time.
 *
 * This should be called ONE and ONLY one time.
 *
 * Unless prohibited by the second parameter, the central widget
 * will be automatically placed into a scroll area if the
 * widget makes the dialog too big for the screen.
 *
 * @param centralWidget
 *    The central widget.
 * @param allowInsertingIntoScrollArea
 *    If true, a scroll area may be added around the central widget.
 */
void 
WuQDialog::setCentralWidget(QWidget* centralWidget,
                            const bool allowInsertingIntoScrollArea)
{
    CaretAssert(centralWidget);
    this->setTopBottomAndCentralWidgetsInternal(NULL,
                                                centralWidget,
                                                NULL,
                                                allowInsertingIntoScrollArea);    
}

/**
 * Sets the given widgets to be the window's top and central widget.
 * Note: WuQDialog takes ownership of the widget pointer
 * and deletes it at the appropriate time.
 *
 * This should be called ONE and ONLY one time.
 *
 * Unless prohibited by the second parameter, the central widget
 * will be automatically placed into a scroll area if the
 * widget makes the dialog too big for the screen.
 *
 * @param topWidget
 *    The widget display at top.
 * @param centralWidget
 *    The central widget.
 * @param topWidget
 *    The widget display at top.
 * @param allowInsertingIntoScrollArea
 *    If true, a scroll area may be added around the central widget.
 */
void 
WuQDialog::setTopBottomAndCentralWidgetsInternal(QWidget* topWidget,
                                                 QWidget* centralWidget,
                                                 QWidget* bottomWidget,
                                                 const bool allowInsertingIntoScrollArea)
{
    if (topWidget != NULL) {
        m_userWidgets.append(topWidget);
    }
    if (centralWidget != NULL) {
        m_userWidgets.append(centralWidget);
    }
    if (bottomWidget != NULL) {
        m_userWidgets.append(bottomWidget);
    }
    
    if (allowInsertingIntoScrollArea == false) {
        if (topWidget != NULL) {
            this->userWidgetLayout->addWidget(topWidget);
        }
        this->userWidgetLayout->addWidget(centralWidget);
        if (bottomWidget != NULL) {
            this->userWidgetLayout->addWidget(bottomWidget);
        }
        
        return;
    }
    
    /*
     * Maximum size is the size of the smallest screen.
     */
    const QSize maxSize = WuQtUtilities::getMinimumScreenSize();
    const int margin = 100;
    const int maxWidth = maxSize.width() - margin;
    int maxHeight = maxSize.height() - margin;
    
    /*
     * If there is a top/bottom widget, decrease maxmimum height by
     * height of top/bottom widget.
     */
    if (topWidget != NULL) {
        const int topHeight = topWidget->sizeHint().height();
        maxHeight -= topHeight;
    }
    if (bottomWidget != NULL) {
        const int bottomHeight = bottomWidget->sizeHint().height();
        maxHeight -= bottomHeight;
    }
    
    /*
     * Get size of central widget with a little padding.
     */
    int widgetWidth = centralWidget->sizeHint().width() + 20;
    const int widgetHeight = centralWidget->sizeHint().height() + 20;
    
    /*
     * Adjust for width of top/bottom widget.
     */
    if (topWidget != NULL) {
        const int topWidth = topWidget->sizeHint().width() + 20;
        widgetWidth = std::max(widgetWidth, topWidth);
    }
    if (bottomWidget != NULL) {
        const int bottomWidth = bottomWidget->sizeHint().width() + 20;
        widgetWidth = std::max(widgetWidth, bottomWidth);
    }
    
    /*
     * Are contents too big for window?
     */
    if ((widgetWidth > maxWidth)
        || (widgetHeight > maxHeight)) {
        /*
         * Put contents in a scroll area
         */
        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(centralWidget);
        
        /*
         * Make scroll area the dialog's widget
         */
        if (topWidget != NULL) {
            this->userWidgetLayout->addWidget(topWidget);
        }
        this->userWidgetLayout->addWidget(scrollArea);
        if (bottomWidget != NULL) {
            this->userWidgetLayout->addWidget(bottomWidget);
        }
        
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
        if (topWidget != NULL) {
            this->userWidgetLayout->addWidget(topWidget);
        }
        this->userWidgetLayout->addWidget(centralWidget);
        if (bottomWidget != NULL) {
            this->userWidgetLayout->addWidget(bottomWidget);
        }
    }    
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

/**
 * Disable the auto default property for ANY buttons
 * in this dialog.  This method must be called after
 * the subclass has added its widget(s) to the dialog.
 */
void 
WuQDialog::disableAutoDefaultForAllPushButtons()
{
    /*
     * Disable auto default for all push buttons
     */
    QList<QPushButton*> allChildPushButtons = findChildren<QPushButton*>(QRegExp(".*"));
    QListIterator<QPushButton*> allChildPushButtonsIterator(allChildPushButtons);
    while (allChildPushButtonsIterator.hasNext()) {
        QPushButton* pushButton = allChildPushButtonsIterator.next();
        //std::cout << "Disabling auto default for pushbutton: " << qPrintable(pushButton->text()) << std::endl;
        pushButton->setAutoDefault(false);
        pushButton->setDefault(false);
    }
}

/**
 * Called when a help button has been added to the dialog.
 * User must override this method when adding a help button.
 */
void 
WuQDialog::helpButtonClicked()
{
    CaretAssertMessage(0, "Help button was added to dialog but WuQDialog::helpButtonClicked() was not overriden");
}

/**
 * Called for focus events.  Since this dialog stores a pointer
 * to the overlay, we need to be aware that the overlay's parameters
 * may change or the overlay may even be deleted.  So, when
 * this dialog gains focus, validate the overlay and then update
 * the dialog.
 *
 * @param event
 *     The focus event.
 */
void
WuQDialog::focusInEvent(QFocusEvent* /*event*/)
{
    focusGained();
}

/**
 * Will be called when dialog gains focus.  User may override this
 * method to receive focus in events.
 */
void
WuQDialog::focusGained()
{
    /* nothing - intended to be overridden by users */
}


