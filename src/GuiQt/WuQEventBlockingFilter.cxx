
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

#define __WU_Q_EVENT_BLOCKING_FILTER_DECLARE__
#include "WuQEventBlockingFilter.h"
#undef __WU_Q_EVENT_BLOCKING_FILTER_DECLARE__

#include <QComboBox>

#include <CaretAssert.h>

using namespace caret;


    
/**
 * \class caret::WuQEventBlockingFilter 
 * \brief A QEvent Filter
 *
 * A QEvent Filter.  There are cases in which one wants to
 * block a specific event from being processed by widgets.
 * For example, on Mac, dragging the mouse over a combo box
 * causes a wheel event that unintentionally changes the 
 * value of the combo box.
 *
 * To use an instance of this blocking filter, (1) Create
 * an instance of this class, (2) call setEventBlocked() which
 * the event enumerated type that is to be blocked, (3) pass
 * the instance of this class to the widget's 
 * installEventFilter() method.
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
 * Add a wheel blocking filter to a combo box (on mac only).  On a Mac,
 * if the mouse pointer is moved across a combo box (particularly when
 * the user is using a Track Pad), Qt will get this event and apply it
 * as a wheel event to the combo box changing the selection in the 
 * combo box.
 *
 * @param comboBox
 *    Combo box that has its wheel event blocked.
 */
#ifdef CARET_OS_MACOSX
void
WuQEventBlockingFilter::blockMouseWheelEventInMacComboBox(QComboBox* comboBox)
{
    /*
     * Attach an event filter that blocks wheel events in the combo box if Mac
     */
    WuQEventBlockingFilter* comboBoxWheelEventBlockingFilter = new WuQEventBlockingFilter(comboBox);
    comboBoxWheelEventBlockingFilter->setEventBlocked(QEvent::Wheel,
                                                      true);
    comboBox->installEventFilter(comboBoxWheelEventBlockingFilter);
}
#else
    void WuQEventBlockingFilter::blockMouseWheelEventInMacComboBox(QComboBox*) { }
#endif // CARET_OS_MACOSX

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

    /*
     * Let parent do the filtering
     */
    return QObject::eventFilter(object, 
                                event);
}
