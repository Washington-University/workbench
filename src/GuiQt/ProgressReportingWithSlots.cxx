
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

#define __PROGRESS_REPORTING_WITH_SLOTS_DECLARE__
#include "ProgressReportingWithSlots.h"
#undef __PROGRESS_REPORTING_WITH_SLOTS_DECLARE__

using namespace caret;


    
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
ProgressReportingInterface()
{
    m_synchronizeMutex.unlock();
    m_cancelled = false;
}

/**
 * Constructor.
 */
ProgressReportingWithSlots::~ProgressReportingWithSlots()
{
    std::cout << "DELETING ~ProgressReportingWithSlots" << std::endl;
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
    /*
     * Don't let any other method in an instance of this
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);
    
    if ((minimumProgress >= 0)
        && (maximumProgress >= minimumProgress)) {
        emit reportProgressRange(minimumProgress,
                                 maximumProgress);
    }
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
    /*
     * Don't let any other method in an instance of this
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);
    
    if (progressValue >= 0) {
        emit reportProgressValue(progressValue);        
    }
}

/**
 * Used by task to set the message describing the task's activity.
 *
 * @param message
 *    Message that is displayed.
 */
void
ProgressReportingWithSlots::setProgressMessage(const AString& progressMessage)
{
    /*
     * Don't let any other method in an instance of this
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);
    
    if (progressMessage.isEmpty() == false) {
        emit reportProgressMessage(progressMessage);
    }
}

/**
 * (SLOT) Used by the user-interface to request that the task end as
 * soon as possible.
 */
void
ProgressReportingWithSlots::requestCancel()
{
    setCancelRequested();
}

/**
 * @return "true" if a request been made to cancel the task, else false.
 *
 * Used by the task to see if the task should end as soon as
 * possible.  If so, the task should clean up after itself
 * (release resources);
 */
bool
ProgressReportingWithSlots::isCancelRequested() const
{
    /*
     * Don't let any other method in an instance of this
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);

    return m_cancelled;
}

/**
 * Set the cancel request.
 */
void
ProgressReportingWithSlots::setCancelRequested()
{
    /*
     * Don't let any other method in an instance of this
     * class execute until this method completes.
     */
    QMutexLocker locker(&m_synchronizeMutex);
    
    m_cancelled = true;
}

