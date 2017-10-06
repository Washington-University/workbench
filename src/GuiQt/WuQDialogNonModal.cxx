
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

#include <QPushButton>

#define __WU_Q_DIALOG_NON_MODAL_DECLARE__
#include "WuQDialogNonModal.h"
#undef __WU_Q_DIALOG_NON_MODAL_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
using namespace caret;


    
/**
 * \class caret::WuQDialogNonModal 
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
    
//    QObject::connect(this->getDialogButtonBox(), SIGNAL(clicked(QAbstractButton*)),
//                     this, SLOT(clicked(QAbstractButton*)));
    
    this->getDialogButtonBox()->button(QDialogButtonBox::Apply)->setDefault(false);
    this->getDialogButtonBox()->button(QDialogButtonBox::Apply)->setAutoDefault(false);
    this->getDialogButtonBox()->button(QDialogButtonBox::Close)->setDefault(false);
    this->getDialogButtonBox()->button(QDialogButtonBox::Close)->setAutoDefault(false);
    
    m_isPositionRestoredWhenReopened = false;
    m_positionWhenClosedValid = false;
}

/**
 * Destructor.
 */
WuQDialogNonModal::~WuQDialogNonModal()
{
    
}

/**
 * Show the dialog.
 *
 * There are a number of methods for 'showing' a Qt dialog.
 * Some of them will show the dialog for the first time.  
 * If the dialog is 'behind' the main window, not all
 * of the methods put the dialog in front of the window.
 * This methods uses several of the Qt methods to ensure
 * that the dialog is moved in front of a main window.
 */
void
WuQDialogNonModal::showDialog()
{
    this->setVisible(true);
    this->show();
    this->activateWindow();
    this->raise();
}


/**
 * May be called requesting the dialog to update its content
 * Subclasses should override.
 */
void
WuQDialogNonModal::updateDialog()
{
#ifdef DEBUG
    QString text(getObjectName() + " should override updateDialog");
    CaretLogWarning(text);
#endif
}

/**
 * Gets called when the dialog is closing.
 * Overriden so that position of dialog 
 * can be saved.
 */
void 
WuQDialogNonModal::closeEvent(QCloseEvent* /*event*/)
{
//    if (m_isPositionRestoredWhenReopened) {
//        /*
//         * Save position and size of dialog so that 
//         * when it is shown next time, it will be
//         * in the position and size as when it was
//         * closed.
//         */
//        m_positionWhenClosedValid = true;
//        m_positionWhenClosed = this->pos();
//        m_sizeWhenClosed = this->size();
//    }
//    
//    WuQDialog::closeEvent(event);
    
    emit dialogWasClosed();
}

/**
 * Gets called when the dialog is to be displayed.
 */
void 
WuQDialogNonModal::showEvent(QShowEvent* event)
{
//    if (m_isPositionRestoredWhenReopened) {
//        if (m_positionWhenClosedValid) {
//            /*
//             * Restore the dialog in the position and size that it
//             * was in when closed.  Use move() for position and
//             * the size hint for the size.
//             */
//            this->move(m_positionWhenClosed);
//            this->resize(m_sizeWhenClosed);
////            const int32_t w = m_sizeWhenClosed.width();
////            const int32_t h = m_sizeWhenClosed.height();
////            if ((w > 0)
////                && (h > 0)) {
////                this->setDialogSizeHint(w,
////                                        h);
////                adjustSize();
////            }
//        }
//    }
    
    WuQDialog::showEvent(event);
}


/**
 * This slot can be called and it simply calls
 * applyButtonClicked.  This slot can be connected
 * to GUI components.
 */
void 
WuQDialogNonModal::apply()
{
    this->applyButtonClicked();
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

/**
 * If the given parameter is true, save the position of this
 * dialog when it is closed.  Next time window is displayed 
 * in the current session, use the position at the time the
 * dialog was closed.
 * @param saveIt
 *   If true save the position for next time.
 */
void 
WuQDialogNonModal::setSaveWindowPositionForNextTime(const bool saveIt)
{
    m_isPositionRestoredWhenReopened = saveIt;
}

