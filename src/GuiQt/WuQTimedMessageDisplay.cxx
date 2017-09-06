
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

#include <QLabel>
#include <QLayout>
#include <QTimer>

#define __WU_Q_TIMED_MESSAGE_DISPLAY_DECLARE__
#include "WuQTimedMessageDisplay.h"
#undef __WU_Q_TIMED_MESSAGE_DISPLAY_DECLARE__

#include "AString.h"
#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::WuQTimedMessageDisplay 
 * \brief Timed message display.
 *
 * Displays a buttonless dialog for a period of time.
 *
 * Use the static show() method to display a timed message display.
 */

/**
 * Constructor.
 *
 * Display a message containing the given message for the given amount
 * of time.
 *
 * @param parent
 *    Parent on which message is displayed.
 * @param displayForSeconds
 *    Message is displayed for this amount of time, in milliseconds.
 * @param message
 *    Message that is displayed.
 * @param modalFlag
 *    If true dialog is modal.
 */
WuQTimedMessageDisplay::WuQTimedMessageDisplay(QWidget* parent,
                                               const float displayForSeconds,
                                               const QString& message,
                                               const bool modalFlag)
: QDialog(parent,
          Qt::FramelessWindowHint)
{
    CaretAssertMessage(displayForSeconds > 0.0,
                       "Display time must be greater than zero.");
    
    /*
     * Modal so it blocks until done.
     */
    setModal(modalFlag);
    
    /*
     * Delete self when done.
     */
    this->setAttribute(Qt::WA_DeleteOnClose,
                       true);

    /*
     * Put message in window.
     */
    QLabel* label = new QLabel(message);
    label->setFrameStyle(QFrame::Panel
                         | QFrame::Plain);
    label->setLineWidth(2);
    label->setWordWrap(true);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->addWidget(label);
    
    /*
     * Setup a timer to call the accept(() slot when done.
     * accept() will close the dialog.
     */
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    if (modalFlag) {
        QObject::connect(timer, SIGNAL(timeout()),
                         this, SLOT(accept()));
    }
    else {
        QObject::connect(timer, SIGNAL(timeout()),
                         this, SLOT(close()));
    }
    timer->start(displayForSeconds * 1000.0);
    
    /*
     * Display directly over the parent
     */
    QPoint pos = parent->mapToGlobal(parent->pos());
    move(pos);
}

/**
 * Display a message containing the given message for the given amount
 * of time.  This method will not return until the message window closes.
 *
 * @param parent
 *    Parent on which message is displayed.
 * @param displayForSeconds
 *    Message is displayed for this amount of time, in milliseconds.
 * @param message
 *    Message that is displayed.
 */
void
WuQTimedMessageDisplay::showModal(QWidget* parent,
                                  const float displayForSeconds,
                                  const QString& message)
{
    WuQTimedMessageDisplay* md = new WuQTimedMessageDisplay(parent,
                                                            displayForSeconds,
                                                            message,
                                                            true);
    md->exec();
}

/**
 * Display a message containing the given message for the given amount
 * of time.  This method will return IMMEDIATELY and the message will
 * delete its self when closed.
 *
 * @param parent
 *    Parent on which message is displayed.
 * @param displayForSeconds
 *    Message is displayed for this amount of time, in milliseconds.
 * @param message
 *    Message that is displayed.
 */
void
WuQTimedMessageDisplay::showNonModal(QWidget* parent,
                                     const float displayForSeconds,
                                     const QString& message)
{
    WuQTimedMessageDisplay* md = new WuQTimedMessageDisplay(parent,
                                                            displayForSeconds,
                                                            message,
                                                            false);
    md->show();
    md->raise();
}

/**
 * Display a message containing the given message for the given amount
 * of time.  This method will not return until the message window closes.
 *
 * @param parent
 *    Parent on which message is displayed.
 * @param x
 *    X-coordinate of parent for display of message.
 * @param y
 *    Y-coordinate of parent for display of message (origin at bottom).
 * @param displayForSeconds
 *    Message is displayed for this amount of time, in milliseconds.
 * @param message
 *    Message that is displayed.
 */
void
WuQTimedMessageDisplay::showModal(QWidget* parent,
                                  const int32_t x,
                                  const int32_t y,
                                  const float displayForSeconds,
                                  const QString& message)
{
    WuQTimedMessageDisplay* md = new WuQTimedMessageDisplay(parent,
                                                            displayForSeconds,
                                                            message,
                                                            true);
    const int32_t originAtTopWindowY = parent->height() - y;
    QPoint globalXY = parent->mapToGlobal(QPoint(x,
                                                 originAtTopWindowY));
    md->move(globalXY);
    md->exec();
}

/**
 * Destructor.
 */
WuQTimedMessageDisplay::~WuQTimedMessageDisplay()
{
}

