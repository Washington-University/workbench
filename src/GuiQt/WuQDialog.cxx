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

/*
 * Some code in keyPressEvent copied from QT4, original license follows
 */
/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the QtGui module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
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
#include <QScrollBar>
#include <QTimer>

#include "CaretAssert.h"
#include "CaretLogger.h"
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
    m_placeCentralWidgetInScrollAreaStatus = SCROLL_AREA_NEVER;
    m_topWidget = NULL;
    m_centralWidget = NULL;
    m_bottomWidget = NULL;
    m_firstTimeInShowMethodFlag = true;
    m_centralWidgetLayoutIndex = -1;
    m_sizeHintWidth  = -1;
    m_sizeHintHeight = -1;
    
    this->autoDefaultProcessingEnabledFlag = true;
    
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    
    this->setWindowTitle(dialogTitle);
    
    this->setFocusPolicy(Qt::ClickFocus);
    
    this->userWidgetLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(this->userWidgetLayout,
                                    0,
                                    0);
    
    m_layoutLeftOfButtonBox = new QHBoxLayout();
    m_layoutLeftOfButtonBox->setContentsMargins(0, 0, 0, 0);
    
    this->buttonBox = new QDialogButtonBox(Qt::Horizontal,
                                           this);
    QObject::connect(this->buttonBox, SIGNAL(clicked(QAbstractButton*)),
                     this, SLOT(clicked(QAbstractButton*)));
    
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addLayout(m_layoutLeftOfButtonBox,
                            0);
    bottomLayout->addWidget(this->buttonBox,
                            1000);
    
    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(dialogLayout,
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

///**
// * Set the size hint for the dialog to the given width and height.
// *
// * NOTE: If there are scrollbars added to the dialog, this size hint
// * may be ignored.
// */
//void
//WuQDialog::setDialogSizeHint(const int32_t width,
//                             const int32_t height)
//{
//    m_sizeHintWidth  = width;
//    m_sizeHintHeight = height;
//}

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
    const bool enableCaptureMenu = false;
    if (enableCaptureMenu) {
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
 * @param placeCentralWidgetInScrollAreaStatus
 *    Status of using a scroll area for the central widget
 */
void 
WuQDialog::setTopBottomAndCentralWidgets(QWidget* topWidget,
                                         QWidget* centralWidget,
                                         QWidget* bottomWidget,
                                         const ScrollAreaStatus placeCentralWidgetInScrollAreaStatus)
{
    CaretAssert(centralWidget);
    this->setTopBottomAndCentralWidgetsInternal(topWidget,
                                                centralWidget,
                                                bottomWidget,
                                                placeCentralWidgetInScrollAreaStatus);
    
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
 * @param placeCentralWidgetInScrollAreaStatus
 *    Status of using a scroll area for the central widget
 */
void 
WuQDialog::setCentralWidget(QWidget* centralWidget,
                            const ScrollAreaStatus placeCentralWidgetInScrollAreaStatus)
{
    CaretAssert(centralWidget);
    this->setTopBottomAndCentralWidgetsInternal(NULL,
                                                centralWidget,
                                                NULL,
                                                placeCentralWidgetInScrollAreaStatus);
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
 * @param placeCentralWidgetInScrollAreaStatus
 *    Status of using a scroll area for the central widget
 */
void 
WuQDialog::setTopBottomAndCentralWidgetsInternal(QWidget* topWidget,
                                                 QWidget* centralWidget,
                                                 QWidget* bottomWidget,
                                                 const ScrollAreaStatus placeCentralWidgetInScrollAreaStatus)
{
    m_topWidget = topWidget;
    m_centralWidget = centralWidget;
    m_bottomWidget = bottomWidget;
    m_placeCentralWidgetInScrollAreaStatus = placeCentralWidgetInScrollAreaStatus;
    
    if (topWidget != NULL) {
        this->userWidgetLayout->addWidget(topWidget);
    }
    m_centralWidgetLayoutIndex = userWidgetLayout->count();
    this->userWidgetLayout->addWidget(centralWidget);
    
    if (bottomWidget != NULL) {
        this->userWidgetLayout->addWidget(bottomWidget);
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

/**
 * Gets called when the dialog is to be displayed.
 */
void
WuQDialog::showEvent(QShowEvent* event)
{
    int32_t resizedDialogWidth  = -1;
    int32_t resizedDialogHeight = -1;

    if (m_firstTimeInShowMethodFlag) {
        const int32_t dialogWidth = sizeHint().width();
        const int32_t dialogHeight = sizeHint().height();
        
        /*
         * Maximum size is the size of the smallest screen.
         */
        const QSize maxSize = WuQtUtilities::getMinimumScreenSize();
        const int32_t margin = 100;
        const int32_t maximumDialogWidth = maxSize.width() - margin;
        const int32_t maximumDialogHeight = maxSize.height() - (margin * 2); // allow space top/bottom
        //const int32_t maximumDialogHeight = (maxSize.height() / 2) - margin;
        
        bool putCentralWidgetIntoScrollAreaFlag = false;
        bool testCentralWidgetForTooBig = false;
        switch (m_placeCentralWidgetInScrollAreaStatus) {
            case SCROLL_AREA_ALWAYS:
                putCentralWidgetIntoScrollAreaFlag = true;
                break;
            case SCROLL_AREA_AS_NEEDED_VERT_NO_HORIZ:
                putCentralWidgetIntoScrollAreaFlag = true;
                break;
            case SCROLL_AREA_AS_NEEDED:
                testCentralWidgetForTooBig = true;
                break;
            case SCROLL_AREA_NEVER:
                break;
        }
        
        
        if (testCentralWidgetForTooBig) {
            /*
             * Are contents too big for window?
             */
            if ((dialogWidth > maximumDialogWidth)
                || (dialogHeight > maximumDialogHeight)) {
                putCentralWidgetIntoScrollAreaFlag = true;
                resizedDialogWidth  = std::min(dialogWidth,
                                               maximumDialogWidth);
                resizedDialogHeight = std::min(dialogHeight,
                                               maximumDialogHeight);
            }
        }
        
        if (putCentralWidgetIntoScrollAreaFlag) {
            /*
             * Remove the central widget,
             * place it into a scroll area,
             * and insert the scroll area into the layout
             */
            userWidgetLayout->removeWidget(m_centralWidget);
            QScrollArea* scrollArea = new QScrollArea();
            scrollArea->setWidgetResizable(true);
            scrollArea->setWidget(m_centralWidget);
            if (m_placeCentralWidgetInScrollAreaStatus == SCROLL_AREA_AS_NEEDED_VERT_NO_HORIZ) {
                scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            }
            userWidgetLayout->insertWidget(m_centralWidgetLayoutIndex,
                                           scrollArea);
            
            /*
             * Resize the dialog.
             */
            if ((resizedDialogWidth > 0)
                && (resizedDialogHeight > 0)) {
                m_sizeHintWidth  = resizedDialogWidth;
                m_sizeHintHeight = resizedDialogHeight;
                adjustSize();
            }
            
        }
    }
    
    QDialog::showEvent(event);

    m_firstTimeInShowMethodFlag = false;
    
}

/**
 * @return The size hint.
 *
 * Overriding the size hint is the best way to set the size of a dialog.
 */
QSize
WuQDialog::sizeHint () const
{
    QSize sh = QDialog::sizeHint();
    
    if ((m_sizeHintWidth > 0)
        && (m_sizeHintHeight > 0)) {
        sh.setWidth(m_sizeHintWidth);
        sh.setHeight(m_sizeHintHeight);
        
        /*
         * Reset the size hint so that the user can resize the dialog
         * without it reverting to this size hint.
         */
        m_sizeHintWidth  = -1;
        m_sizeHintHeight = -1;
    }
    
    return sh;
}

/**
 * Set the size of the dialog for next time it is displayed.
 * This must be called BEFORE displayng the dialog.
 */
void
WuQDialog::setSizeOfDialogWhenDisplayed(const QSize& size)
{
    const int32_t w = size.width();
    const int32_t h = size.height();
    
    if ((w > 0)
        && (h > 0)) {
        m_sizeHintWidth  = w;
        m_sizeHintHeight = h;
    }
}

/**
 * A scroll area's size hint is often larger than its content widget and in
 * this case the dialog will be too large with blank space in the scroll area.
 * This method will adjust the size of the dialog which in turn will shrink
 * the size of the scroll area to better fit its content.  This should only
 * be called once for a dialog so that the user can adjust the size of the 
 * dialog, if desired.
 *
 * @param dialog
 *    Dialog whose size is adjusted.
 * @param scrollArea
 *    The scroll area in the dialog.
 */
void
WuQDialog::adjustSizeOfDialogWithScrollArea(QDialog* dialog,
                                                QScrollArea* scrollArea)
{
    /*
     * The content region of a scroll area is often too large vertically
     * so adjust the size of the dialog which will cause the scroll area
     * to approximately fit its content.
     */
    QWidget* scrollAreaContentWidget = scrollArea->widget();
    if (scrollAreaContentWidget != NULL) {
        int32_t contentHeight = scrollAreaContentWidget->sizeHint().height();
        int32_t scrollHeight = scrollArea->sizeHint().height();
        int32_t scrollBarHeight = scrollArea->horizontalScrollBar()->sizeHint().height();
        int32_t diff = (scrollHeight - (contentHeight + scrollBarHeight + 10));
        if (diff > 0) {
            QSize dialogSizeHint = dialog->sizeHint();
            dialogSizeHint.setHeight(dialogSizeHint.height() - diff);
            dialog->resize(dialogSizeHint);
        }
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
 * @param buttonRole
 *     Role of button.  NOTE: This is used for placement of buttons in
 *     the appropriate location for the operating system.  Any action,
 *     such as closing the dialog will not occur because of this button
 *     push.
 * @return
 *     QPushButton that was created.
 */
QPushButton*
WuQDialog::addUserPushButton(const AString& text,
                             const QDialogButtonBox::ButtonRole buttonRole)
{
    QPushButton* pushButton = getDialogButtonBox()->addButton(text,
                                                              buttonRole);
    return pushButton;
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
WuQDialog::DialogUserButtonResult
WuQDialog::userButtonPressed(QPushButton* userPushButton)
{
    const AString msg = ("Subclass of WuQDialog added a user pushbutton but failed to override userButtonPressed for button labeled \""
                         + userPushButton->text()
                         + "\"");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
    
    return RESULT_NONE;
}

/**
 * Called when the OK button is clicked.
 * If needed should override this to process
 * data when the OK button is clicked and then
 * call this to issue the accept signal.
 */
void
WuQDialog::okButtonClicked()
{
    if (! isModal()) {
        CaretAssertMessage(0, "WuQDialog::okButtonClicked() should never be called for a NON-modal dialog.");
    }
    
    accept();
}

/**
 * Called when the Cancel button is clicked.
 * If needed should override this to process
 * data when the Cancel button is clicked.
 * Call this to issue the reject signal.
 */
void
WuQDialog::cancelButtonClicked()
{
    if (! isModal()) {
        CaretAssertMessage(0, "WuQDialog::cancelButtonClicked() should never be called for a NON-modal dialog.");
    }
    
    reject();
}

/**
 * Called when the Apply button is pressed.
 * If needed should override this to process
 * data when the Apply button is pressed.
 */
void
WuQDialog::applyButtonClicked()
{
    if (isModal()) {
        CaretAssertMessage(0, "WuQDialog::applyButtonClicked() should never be called for a MODAL dialog.");
    }
}

/**
 * Called when the Close button is pressed.
 * If needed should override this to process
 * data when the Close button is pressed.
 */
void
WuQDialog::closeButtonClicked()
{
    if (isModal()) {
        CaretAssertMessage(0, "WuQDialog::closeButtonClicked() should never be called for a MODAL dialog.");
    }
    
    close();
}

/**
 * Called when a button is pressed.
 */
void
WuQDialog::clicked(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton standardButton = this->getDialogButtonBox()->standardButton(button);
    if (standardButton == QDialogButtonBox::Ok) {
        button->setEnabled(false);
        okButtonClicked();
        button->setEnabled(true);
    }
    else if (standardButton == QDialogButtonBox::Cancel) {
        cancelButtonClicked();
    }
    else if (standardButton == QDialogButtonBox::Apply) {
        applyButtonClicked();
    }
    else if (standardButton == QDialogButtonBox::Close) {
        closeButtonClicked();
    }
    else if (standardButton == QDialogButtonBox::Help) {
        this->helpButtonClicked();
    }
    else {
        //QPushButton* pushButton = dynamic_cast<QPushButton*>(button);
        QPushButton* pushButton = qobject_cast<QPushButton*>(button);
        CaretAssert(pushButton);
        const DialogUserButtonResult result = this->userButtonPressed(pushButton);
        switch (result) {
            case RESULT_MODAL_ACCEPT:
                accept();
                break;
            case RESULT_MODAL_REJECT:
                reject();
                break;
            case RESULT_NON_MODAL_CLOSE:
                close();
                break;
            case RESULT_NONE:
                break;
        };
    }
}


