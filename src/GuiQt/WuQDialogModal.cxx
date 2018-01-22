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

#define __WU_Q_DIALOG_MODAL_DECLARE__
#include "WuQDialogModal.h"
#undef __WU_Q_DIALOG_MODAL_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::WuQDialogModal 
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
    m_isSaveDialogPosition = false;
    
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Ok);
    this->getDialogButtonBox()->addButton(QDialogButtonBox::Cancel);
    
//    QObject::connect(this->getDialogButtonBox(), SIGNAL(clicked(QAbstractButton*)),
//                     this, SLOT(clicked(QAbstractButton*)));
    
    this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setDefault(true);
    //this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setAutoDefault(true);
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
    
    this->setCentralWidget(centralWidget,
                           WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
WuQDialogModal::~WuQDialogModal()
{
    
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
/*
    if (text.isEmpty()) {
        this->getDialogButtonBox()->button(QDialogButtonBox::Cancel)->setDefault(true);
        this->getDialogButtonBox()->button(QDialogButtonBox::Cancel)->setAutoDefault(true);
    }
    else {
        this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setDefault(true);
        this->getDialogButtonBox()->button(QDialogButtonBox::Ok)->setAutoDefault(true);
    }
*/
}

/**
 * Set the enabled status for the OK button.
 *
 * @param enabled
 *    New enabled status for the button.
 */
void
WuQDialogModal::setOkButtonEnabled(const bool enabled)
{
    this->setStandardButtonEnabled(QDialogButtonBox::Ok,
                                   enabled);
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

/**
 * If this method is called, each time the dialog is closed, it will
 * save the position of the dialog and restore the dialog to that
 * position when the dialog is reopened.
 *
 * @param savePositionName
 *    Name used for saving the dialog's position.  If the default value
 *    (an empty string), the dialog's title is used for saving the
 *    dialog's position.  Note that is there is more than one dialog
 *    with the same name positions may not be correctly restored.
 */
void
WuQDialogModal::setSaveWindowPositionForNextTime(const AString& savePositionName)
{
    m_isSaveDialogPosition = true;
    m_saveDialogPositionName = savePositionName;
    if (m_saveDialogPositionName.isEmpty()) {
        m_saveDialogPositionName = windowTitle();
        if (m_saveDialogPositionName.isEmpty()) {
            m_isSaveDialogPosition = false;
        }
    }
}

/**
 * Shows/hides a widget.
 * Override to optionally place dialog via values passed to setDisplayedXY.
 */
void
WuQDialogModal::setVisible(bool visible)
{
    WuQDialog::setVisible(visible);
    
    if (m_isSaveDialogPosition) {
        /*
         * Find previous position of dialog.
         */
        std::map<QString, SavedPosition>::iterator iter = s_savedDialogPositions.find(m_saveDialogPositionName);
        
        if (visible) {
            if (iter != s_savedDialogPositions.end()) {
                /*
                 * Restore dialog position
                 */
                SavedPosition savedPosition = iter->second;
                if ((savedPosition.x > 0)
                    && (savedPosition.y > 0)) {
                    move(savedPosition.x,
                         savedPosition.y);
                }
                if ((savedPosition.w > 0)
                    && (savedPosition.h > 0)) {
                    resize(savedPosition.w,
                           savedPosition.h);
                }
            }
        }
        else {
            /*
             * Save position of dialog.
             */
            SavedPosition savedPosition;
            savedPosition.x = x();
            savedPosition.y = y();
            savedPosition.w = width();
            savedPosition.h = height();
            
            if (iter != s_savedDialogPositions.end()) {
                /*
                 * Replace dialog position
                 */
                iter->second = savedPosition;
            }
            else {
                /*
                 * Insert dialog position
                 */
                s_savedDialogPositions.insert(std::make_pair(m_saveDialogPositionName,
                                                             savedPosition));
            }
        }
    }
}



