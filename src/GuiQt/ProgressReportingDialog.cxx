
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

#define __PROGRESS_REPORTING_DIALOG_DECLARE__
#include "ProgressReportingDialog.h"
#undef __PROGRESS_REPORTING_DIALOG_DECLARE__

#include <QApplication>
#include <QMutexLocker>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "ProgressReportingFromEvent.h"
#include "ProgressReportingWithSlots.h"

using namespace caret;

/**
 * \class caret::ProgressReportingDialog
 * \brief Progress Report Dialog
 * \ingroup GuiQt
 */
    
/**
 * \class caret::ProgressReportingDialog 
 * \brief Dialog that displays "progress" as an event is running.
 *
 * This dialog will the display the "progress" on a task as the 
 * task executes.  As the task is running, it should send
 * EventProgressUpdate events indicating the progress of the processing.
 * If the user chooses to cancel the event, this dialog will indicate so
 * on the first EventProgressUpdate received after the Cancel button is
 * clicked.  The task should query the ProgressReportingDialog::isCancelled()
 * after each progress update is sent.
 *
 * Tasks may send the EventProgressUpdate event from other threads.  
 * When the event is received signals and slots are used to update 
 * the contents of the dialog instead of directly updating the dialog's
 * content.  Qt's signals and slots mechanism supports signals connecting
 * to slots that are in a different thread.
 *
 * Use the static method "runEvent" to run an event with a progress dialog.
 *
 * For other cases, use the public constructor.  Create an instance of the
 * progress dialog and then start the desired task.  The progress dialog
 * will automatically close when the progress value equals the maximum
 * progress value or when the progress dialog goes out of scope.
 */

/**
 * Constructor.
 *
 * User will need to send EventProgressUpdate events to update
 * this progress dialog.
 *
 * @param title
 *    Title for dialog.
 * @param initialMessage
 *    Message that is first displayed.
 * @param parent
 *    Parent on which this progress dialog is displayed.
 * @param f
 *    Window flags.
 */
ProgressReportingDialog::ProgressReportingDialog(const AString& title,
                                                 const AString& initialMessage,
                                                 QWidget* parent,
                                                 Qt::WindowFlags f)
: QProgressDialog(initialMessage,
                  "Cancel",
                  0,
                  100,
                  parent,
                  f)
{
    ProgressReportingFromEvent* progressFromEvent = new ProgressReportingFromEvent(this);
    m_progressReporter = progressFromEvent;
    
    if (title.isEmpty() == false) {
        setWindowTitle(title);
    }
    
    const int minimumTimeInMillisecondsBeforeDialogDisplayed = 0;
    setMinimumDuration(minimumTimeInMillisecondsBeforeDialogDisplayed);
    
    QObject::connect(progressFromEvent, SIGNAL(reportProgressRange(const int, const int)),
                     this, SLOT(setRange(int, int)));
    
    QObject::connect(progressFromEvent, SIGNAL(reportProgressValue(const int)),
                     this, SLOT(setValue(int)));
    
    QObject::connect(progressFromEvent, SIGNAL(reportProgressMessage(const QString&)),
                     this, SLOT(setLabelText(const QString&)));
    
    QObject::connect(this, SIGNAL(canceled()),
                     progressFromEvent, SLOT(requestCancel()));
}

/**
 * Destructor.
 */
ProgressReportingDialog::~ProgressReportingDialog()
{
}

/**
 * Run the event in a progress dialog.  Dialog will close after the 
 * event completes.  Progress is updated each time a 
 * EventProgressUpdate is received.
 *
 * @param event
 *    Event that is executed.
 * @param parent
 *    Widget on which the dialog is displayed.
 * @param title
 *    If not empty, title is in window's title bar
 */
void
ProgressReportingDialog::runEvent(Event* event,
                                  QWidget* parent,
                                  const AString& title)
{
    ProgressReportingDialog prd(title,
                                "",
                                parent);
    prd.setValue(0);
    
    EventManager::get()->sendEvent(event);
    
    prd.setValue(prd.maximum());
}

/**
 * Set event receiving enabled.  This method is used to enable
 * and disable the receiving of EventProgressUpdate events.
 * The default is true.
 *
 * @param enabled
 *     New status for receiving the events.
 */
void
ProgressReportingDialog::setEventReceivingEnabled(bool status)
{
    ProgressReportingFromEvent* progFromEvent = dynamic_cast<ProgressReportingFromEvent*>(m_progressReporter);
    if (progFromEvent != NULL) {
        progFromEvent->setEventReceivingEnabled(status);
    }
}


