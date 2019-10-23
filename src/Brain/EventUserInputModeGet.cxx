
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __EVENT_USER_INPUT_MODE_GET_DECLARE__
#include "EventUserInputModeGet.h"
#undef __EVENT_USER_INPUT_MODE_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventUserInputModeGet 
 * \brief Event to get the user input mode for a window
 * \ingroup Brain
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 *  @param windowIndex
 *         Index of window for obtaining mode.
 */
EventUserInputModeGet::EventUserInputModeGet(const int32_t windowIndex)
: Event(EventTypeEnum::EVENT_GET_USER_INPUT_MODE),
m_windowIndex(windowIndex)
{
    
}

/**
 * Destructor.
 */
EventUserInputModeGet::~EventUserInputModeGet()
{
}

/**
 * @return Index of the window
 */
int32_t
EventUserInputModeGet::getWindowIndex() const
{
    return m_windowIndex;
}

/**
 * @return The user input modfe
 */
UserInputModeEnum::Enum
EventUserInputModeGet::getUserInputMode() const
{
    return m_userInputMode;
}

/**
 * Set the usert input mofe
 * @param userInputMode
 *     The user input mode
 */
void
EventUserInputModeGet::setUserInputMode(const UserInputModeEnum::Enum userInputMode)
{
    m_userInputMode = userInputMode;
}


