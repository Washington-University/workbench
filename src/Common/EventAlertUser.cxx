
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __EVENT_ALERT_USER_DECLARE__
#include "EventAlertUser.h"
#undef __EVENT_ALERT_USER_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventAlertUser 
 * \brief Event that alerts the user to a problem
 * \ingroup Common
 *
 * If there is a problem in non-GUI code, it may be desirable to inform
 * the user of a problem.  However, when this type of problem occurs,
 * passing this information "up the call stack" may be problematic and
 * using an exception may not be possible or desired.
 *
 * In these instance this event class may be used.  When there is a GUI,
 * this event is received by the GuiManager and a pop-up is presented
 * to the user.  If there is not a GUI, the EventManager will not send
 * this event and instead, log the message at the severe level.
 */

/**
 * Constructor for an alert message.
 */
EventAlertUser::EventAlertUser(const AString& message)
: Event(EventTypeEnum::EVENT_ALERT_USER),
m_message(message)
{
    
}

/**
 * Destructor.
 */
EventAlertUser::~EventAlertUser()
{
}

/**
 * @return The message for the alert.
 */
AString
EventAlertUser::getMessage() const
{
    return m_message;
}



