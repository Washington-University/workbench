
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

#define __WU_Q_EVENT_BLOCKING_FILTER_DECLARE__
#include "WuQEventBlockingFilter.h"
#undef __WU_Q_EVENT_BLOCKING_FILTER_DECLARE__

#include <CaretAssert.h>

using namespace caret;


    
/**
 * \class caret::WuQEventBlockingFilter 
 * \brief A QEvent Filter
 *
 * A QEvent Filter.  There are cases in which one wants to
 * block a specific event from being processed by a widget.
 * For example, on Mac, dragging the mouse over a combo box
 * causes a wheel event and 
 */

/**
 * Constructor.
 * @param object
 *    Parent that will own this event filter.  The parent
 *    will handle destruction of this event filter.
 */
WuQEventBlockingFilter::WuQEventBlockingFilter(QObject* parent)
: QObject(parent)
{
}

/**
 * Destructor.
 */
WuQEventBlockingFilter::~WuQEventBlockingFilter()
{
}

/**
 * Set the blocking status for the given event type.
 * @param eventType
 *    Type of event to block.
 * @param blockedStatus
 *    New status for blocking.
 */
void 
WuQEventBlockingFilter::setEventBlocked(const QEvent::Type eventType,
                                        const bool blockedStatus)
{
    const int eventAsInt = static_cast<int>(eventType);
    this->blockedEventsHashTable.insert(eventAsInt, 
                                        blockedStatus);
}

/**
 * Is the given event blocked?
 * @param eventType
 *     Type of event.
 * @return
 *     True if event is being blocked, else false.
 */
bool 
WuQEventBlockingFilter::isEventBlocked(const QEvent::Type eventType) const
{
    const int eventAsInt = static_cast<int>(eventType);
    return this->blockedEventsHashTable.value(eventAsInt, 
                                              false);
}

/**
 * Filters events for the given object.
 * @param object
 *     Object for which events are being filtered.
 * @param event
 *     Event that is examined for blocking.
 * @ @return
 *     True, meaning that this event will not receive
 *     any furthere processing.  Or false, meaning
 *     that the processing has been passed to the 
 *     parent class of the object.
 */
bool 
WuQEventBlockingFilter::eventFilter(QObject* object,
                                    QEvent* event)
{
    const int eventAsInt = static_cast<int>(event->type());
    if (this->blockedEventsHashTable.value(eventAsInt,
                                           false)) {
        return true;
    }

    return false;
}
