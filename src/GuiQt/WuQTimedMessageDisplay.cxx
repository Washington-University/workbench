
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 */
WuQTimedMessageDisplay::WuQTimedMessageDisplay(QWidget* parent,
                                               const float displayForSeconds,
                                               const QString& message)
: QDialog(parent,
          Qt::FramelessWindowHint)
{
    CaretAssertMessage(displayForSeconds > 0.0,
                       "Display time must be greater than zero.");
    
    /*
     * Modal so it blocks until done.
     */
    setModal(true);
    
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
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(3, 3, 3, 3);
    layout->addWidget(label);
    
    /*
     * Setup a timer to call the accept(() slot when done.
     * accept() will close the dialog.
     */
    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, SIGNAL(timeout()),
                     this, SLOT(accept()));
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
WuQTimedMessageDisplay::show(QWidget* parent,
                             const float displayForSeconds,
                             const QString& message)
{
    WuQTimedMessageDisplay* md = new WuQTimedMessageDisplay(parent,
                                                            displayForSeconds,
                                                            message);
    md->exec();
}

/**
 * Destructor.
 */
WuQTimedMessageDisplay::~WuQTimedMessageDisplay()
{
    
}

