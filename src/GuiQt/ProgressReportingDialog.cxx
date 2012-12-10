
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

using namespace caret;


    
/**
 * \class caret::ProgressReportingDialog 
 * \brief Dialog that displays "progress" as an event is running.
 *
 * This dialog will the display the "progress" on an event as the 
 * event executes.  As the event is running, it should send
 * EventProgressUpdate events indicating the progress of the processing.
 * If the user chooses to cancel the event, this dialog will indicate so
 * on the files EventProgressUpdate received after the Cancel button is
 * clicked.
 *
 * Use the static method "runEvent" to run an event with a progress dialog.
 */

/**
 * Constructor.
 *
 * @param initialMessage
 *    Message that is first displayed.
 * @param parent
 *    Parent on which this progress dialog is displayed.
 */
ProgressReportingDialog::ProgressReportingDialog(const QString& initialMessage,
                                                 QWidget* parent)
: QProgressDialog(initialMessage,
                  "Cancel",
                  0,
                  100,
                  parent)
{
    const int minimumTimeInMillisecondsBeforeDialogDisplayed = 1000;
    setMinimumDuration(minimumTimeInMillisecondsBeforeDialogDisplayed);
    
    m_progressReporter = new ProgressReportingWithSlots(this);
    
    QObject::connect(m_progressReporter, SIGNAL(reportProgressRange(const int, const int)),
                     this, SLOT(setRange(int, int)));
    
    QObject::connect(m_progressReporter, SIGNAL(reportProgressValue(const int)),
                     this, SLOT(setValue(int)));
    
    QObject::connect(m_progressReporter, SIGNAL(reportProgressMessage(const QString&)),
                     this, SLOT(setLabelText(const QString&)));
    
    QObject::connect(this, SIGNAL(canceled()),
                     m_progressReporter, SLOT(requestCancel()));
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
 * event completes.
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
                                  const QString& title)
{
    ProgressReportingDialog prd("Starting to load files",
                                parent);
    if (title.isEmpty() == false) {
        prd.setWindowTitle(title);
    }
    
    EventManager::get()->sendEvent(event);
    
    prd.setValue(prd.maximum());
}


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
ProgressReportingWithSlots::ProgressReportingWithSlots(QObject* parent)
: QObject(parent),
  EventListenerInterface()
{
    m_synchronizeMutex.unlock();
    m_cancelled = false;
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_PROGRESS_UPDATE);
}

/**
 * Constructor.
 */
ProgressReportingWithSlots::~ProgressReportingWithSlots()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

void
ProgressReportingWithSlots::receiveEvent(Event* event)
{
    /*
     * Don't let any other method in an instance of this 
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);
    
    if (event->getEventType() == EventTypeEnum::EVENT_PROGRESS_UPDATE) {
        EventProgressUpdate* progressEvent = dynamic_cast<EventProgressUpdate*>(event);
        CaretAssert(progressEvent);
        
        setProgressRange(progressEvent->getMinimumProgressValue(),
                         progressEvent->getMaximumProgressValue());
        setProgressValue(progressEvent->getProgressValue());
        setProgressMessage(progressEvent->getProgressMessage());
        
        std::cout << "PROGRESS: "
        << progressEvent->getProgressValue()
        << " of ("
        << progressEvent->getMinimumProgressValue()
        << ","
        << progressEvent->getMaximumProgressValue()
        << "): "
        << qPrintable(progressEvent->getProgressMessage())
        << std::endl;
        
        if (m_cancelled) {
            progressEvent->setCancelled();
        }
        
        QApplication::processEvents();
        
        progressEvent->setEventProcessed();
    }
}

/**
 * Used by task to set the range of progress reporting.
 *
 * @param minimumProgress
 *    The minimum amount reported (typically zero).
 * @param maximumProgress
 *    The maximum amount of progress.
 */
void
ProgressReportingWithSlots::setProgressRange(const int minimumProgress,
                                          const int maximumProgress)
{
//    /*
//     * Don't let any other method in an instance of this
//     * class execute until this method completes.
//     */
//    QMutexLocker locker(&m_synchronizeMutex);
    
    emit reportProgressRange(minimumProgress,
                             maximumProgress);
}

/**
 * Used by task to set the current progress.
 *
 * @param progressValue
 *    The current progress within range of the minimum and maximum.
 */
void
ProgressReportingWithSlots::setProgressValue(const int progressValue)
{
//    /*
//     * Don't let any other method in an instance of this
//     * class execute until this method completes.
//     */
//    QMutexLocker locker(&m_synchronizeMutex);

    emit reportProgressValue(progressValue);
}

/**
 * Used by task to set the message describing the task's activity.
 *
 * @param message
 *    Message that is displayed.
 */
void
ProgressReportingWithSlots::setProgressMessage(const QString& progressMessage)
{
//    /*
//     * Don't let any other method in an instance of this
//     * class execute until this method completes.
//     */
//    QMutexLocker locker(&m_synchronizeMutex);

    emit reportProgressMessage(progressMessage);
}

/**
 * Used by the user-interface to request that the task end as
 * soon as possible.
 */
void
ProgressReportingWithSlots::requestCancel()
{
    /*
     * Don't let any other method in an instance of this
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);
    
    m_cancelled = true;
}

/**
 * @return "true" if a request been made to cancel the task, else false.
 *
 * Used by the task to see if the task should end as soon as
 * possible.  If so, the task should clean up after itself
 * (release resources);
 */
//bool
//ProgressReportingWithSlots::isCancelled() const
//{
//    /*
//     * Don't let any other method in an instance of this
//     * class execute until this method completes.
//     */
//    QMutexLocker locker(&m_synchronizeMutex);
//
//    return m_cancelled;
//}

