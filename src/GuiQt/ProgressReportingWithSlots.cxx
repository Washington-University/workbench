
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

#define __PROGRESS_REPORTING_WITH_SLOTS_DECLARE__
#include "ProgressReportingWithSlots.h"
#undef __PROGRESS_REPORTING_WITH_SLOTS_DECLARE__

using namespace caret;


    
/**
 * \class caret::ProgressReportingWithSlots
 * \brief Interfaces between the ProgressReportingDialog and the Workbench event system
 * \ingroup GuiQt
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
#if QT_VERSION >= 0x050000
#else
    m_synchronizeMutex.unlock();
#endif
    m_cancelled = false;
}

/**
 * Constructor.
 */
ProgressReportingWithSlots::~ProgressReportingWithSlots()
{
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

