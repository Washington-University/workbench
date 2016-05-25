
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#define __PROGRESS_REPORTING_FROM_EVENT_DECLARE__
#include "ProgressReportingFromEvent.h"
#undef __PROGRESS_REPORTING_FROM_EVENT_DECLARE__

#include <QApplication>

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventProgressUpdate.h"

using namespace caret;


    
//=============================================================================
/**
 * \class caret::ProgressReportingFromEvent
 * \brief Interfaces between the ProgressReportingDialog and the Workbench event system
 *
 * Listens for EventProgressUpdate events and then updates the progress
 * dialog using signals and slots.  Using signals and slots should allow
 * the progress events to be sent from a thread that is not the GUI thread.
 */

/**
 * Constructor.
 *
 * @parent
 *    Parent of this instance.
 */
ProgressReportingFromEvent::ProgressReportingFromEvent(QObject* parent)
: ProgressReportingWithSlots(parent)
{
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_PROGRESS_UPDATE);
    
    m_eventReceivingEnabled = true;
}

/**
 * Destructor.
 */
ProgressReportingFromEvent::~ProgressReportingFromEvent()
{
    EventManager::get()->removeAllEventsFromListener(this);
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
ProgressReportingFromEvent::setEventReceivingEnabled(bool enabled)
{
    m_eventReceivingEnabled = enabled;
}

/**
 * @return Is event receiving enabled?  The default is true.
 */
bool
ProgressReportingFromEvent::isEventReceivingEnabled() const
{
    return m_eventReceivingEnabled;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
ProgressReportingFromEvent::receiveEvent(Event* event)
{
    if ( ! m_eventReceivingEnabled) {
        return;
    }
    
    if (event->getEventType() == EventTypeEnum::EVENT_PROGRESS_UPDATE) {
        EventProgressUpdate* progressEvent = dynamic_cast<EventProgressUpdate*>(event);
        CaretAssert(progressEvent);
        
        const int minProg = progressEvent->getMinimumProgressValue();
        const int maxProg = progressEvent->getMaximumProgressValue();
        const int progValue = progressEvent->getProgressValue();
        const AString progMessage = progressEvent->getProgressMessage();
        
        /*
         * Note: It appears that the content of the progress dialog
         * only changes when the progress value is changed.  So, set
         * the message prior to the progress value so that if the message
         * changes, the message in the progress dialog will be updated
         * when the progress value changes.
         */
        if (progMessage.isEmpty() == false) {
            setProgressMessage(progMessage);
        }
        if ((minProg >= 0) && (maxProg >= minProg)) {
            setProgressRange(minProg,
                             maxProg);
        }
        if (progValue >= 0) {
            setProgressValue(progValue);
        }
        
        if (isCancelRequested()) {
            progressEvent->setCancelled();
        }
        
        QApplication::processEvents();
        
        progressEvent->setEventProcessed();
    }
}

