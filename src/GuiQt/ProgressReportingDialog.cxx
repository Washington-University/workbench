
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

#define __PROGRESS_REPORTING_DIALOG_DECLARE__
#include "ProgressReportingDialog.h"
#undef __PROGRESS_REPORTING_DIALOG_DECLARE__

#include <QApplication>
#include <QMutexLocker>

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"
#include "ProgressReportingWithSlots.h"

using namespace caret;


    
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
 * @param progressReporter
 *    Concrete instance of ProgressReportingInterface that is used to report
 *    progress.
 * @param title
 *    Title for dialog.
 * @param initialMessage
 *    Message that is first displayed.
 * @param parent
 *    Parent on which this progress dialog is displayed.
 * @param f
 *    Window flags.
 */
//ProgressReportingDialog::ProgressReportingDialog(ProgressReportingWithSlots* progressReporter,
//                                                 const AString& title,
//                                                 const AString& initialMessage,
//                                                 QWidget* parent,
//                                                 Qt::WindowFlags f)
//: QProgressDialog(initialMessage,
//                  "Cancel",
//                  50,
//                  100,
//                  parent,
//                  f)
//{
//    CaretAssert(progressReporter);
//    m_progressReporter = progressReporter;
//    progressReporter->setParent(this);
//    
//    if (title.isEmpty() == false) {
//        setWindowTitle(title);
//    }
//    
//    const int minimumTimeInMillisecondsBeforeDialogDisplayed = 0;
//    setMinimumDuration(minimumTimeInMillisecondsBeforeDialogDisplayed);
//    
//    QObject::connect(progressReporter, SIGNAL(reportProgressRange(const int, const int)),
//                     this, SLOT(setRange(int, int)));
//    
//    QObject::connect(progressReporter, SIGNAL(reportProgressValue(const int)),
//                     this, SLOT(setValue(int)));
//    
//    QObject::connect(progressReporter, SIGNAL(reportProgressMessage(const QString&)),
//                     this, SLOT(setLabelText(const QString&)));
//    
//    QObject::connect(this, SIGNAL(canceled()),
//                     progressReporter, SLOT(requestCancel()));
//}

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
                  50,
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
    /**
     * Note: Do not need to delete "m_progressReporter" because it
     * is a child of this dialog and Qt will delete all children of
     * the dialog.
     */
    //delete m_progressReporter;
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
//    ProgressReportingFromEvent* progressReporterWithEvent = new ProgressReportingFromEvent(0);
//    
//    ProgressReportingDialog prd(progressReporterWithEvent,
//                                title,
//                                "",
//                                parent);
    ProgressReportingDialog prd(title,
                                "",
                                parent);
    prd.setValue(0);
    
    EventManager::get()->sendEvent(event);
    
    prd.setValue(prd.maximum());
}

/**
 * Run, using a signal, in a progress dialog.  Dialog will close after
 * event completes.
 *
 * @param parent
 *    Widget on which the dialog is displayed.
 * @param title
 *    If not empty, title is in window's title bar
 * @param receiver
 *    Receiver of the signal.
 * @param method
 *    Method that is called in the receiver.  It must accept one
 * parameters that is a pointer to ProgressReportingInterface.
 * eg:   slotName(ProgressReportingInterface*)
 * The user should then start the task and use the ProgressReportingInterface
 * to update the progress.  When the method finishes, executation will return
 * to this static method and processing will remove the progress dialog.
 */
//void
//ProgressReportingDialog::run(QWidget* parent,
//                             const AString& title,
//                             QObject* receiver,
//                             const char* method)
//{
//    ProgressReportingWithSlots* progressReporterWithSlots = new ProgressReportingWithSlots(0);
//    
//    ProgressReportingDialog prd(progressReporterWithSlots,
//                                title,
//                                "",
//                                parent);
//    
//    QObject::connect(&prd, SIGNAL(startWithProgress(ProgressReportingInterface*)),
//                     receiver, method);
//    
//    prd.setValue(prd.maximum());
//}

/**
 * Show a progress dialog.  Use the returned progress interface to 
 * update the contents of the dialog.  When the progress value
 * is equal to the maximum progress value, the dialog will close
 * and and the progress interface that was returned will be deleted.
 *
 * @param parent
 *    Widget on which the dialog is displayed.
 * @param title
 *    If not empty, title is in window's title bar
 * @return
 *    An instance of a progress reporting interface.  DO NOT delete
 *    this object.  It will be deleted when the progress value is
 *    equal to the maximum progress value.
 */
//ProgressReportingInterface*
//ProgressReportingDialog::showProgressDialog(QWidget* parent,
//                                            const AString& title)
//{
//    ProgressReportingWithSlots* progressReporterWithSlots = new ProgressReportingWithSlots(0);
//    
//    ProgressReportingDialog* prd = new ProgressReportingDialog(progressReporterWithSlots,
//                                                               title,
//                                                               "",
//                                                               parent);
////    prd->setAttribute(Qt::WA_DeleteOnClose);
//    
//    if (title.isEmpty() == false) {
//        prd->setWindowTitle(title);
//    }
//    
//    return progressReporterWithSlots;
//}



//=============================================================================
/**
 * \class caret::ProgressReportingWithSlots
 * \brief Interfaces between the ProgressReportingDialog and the Workbench event system
 *
 * Listens for EventProgressUpdate events and then updates the progress
 * dialog using signals and slots.  Using signals and slots should allow 
 * the progress events to be sent from a thread that is not the GUI thread.
 */

/**
 * Constructor.
 */
ProgressReportingFromEvent::ProgressReportingFromEvent(QObject* parent)
: ProgressReportingWithSlots(parent),
  EventListenerInterface()
{
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_PROGRESS_UPDATE);
}

/**
 * Constructor.
 */
ProgressReportingFromEvent::~ProgressReportingFromEvent()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

void
ProgressReportingFromEvent::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_PROGRESS_UPDATE) {
        EventProgressUpdate* progressEvent = dynamic_cast<EventProgressUpdate*>(event);
        CaretAssert(progressEvent);
        
        const int minProg = progressEvent->getMinimumProgressValue();
        const int maxProg = progressEvent->getMaximumProgressValue();
        const int progValue = progressEvent->getProgressValue();
        const AString progMessage = progressEvent->getProgressMessage();
        
        if ((minProg >= 0) && (maxProg >= minProg)) {
            setProgressRange(minProg,
                                     maxProg);
        }
        if (progValue >= 0) {
            setProgressValue(progValue);
        }
        if (progMessage.isEmpty() == false) {
            setProgressMessage(progMessage);
        }
        
        if (isCancelRequested()) {
            progressEvent->setCancelled();
        }
        
        QApplication::processEvents();
        
        progressEvent->setEventProcessed();
    }
}

