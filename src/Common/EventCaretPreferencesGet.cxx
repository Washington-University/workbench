
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __EVENT_CARET_PREFERENCES_GET_DECLARE__
#include "EventCaretPreferencesGet.h"
#undef __EVENT_CARET_PREFERENCES_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventCaretPreferencesGet 
 * \brief Event to get the Caret Preferences
 * \ingroup Common
 */

/**
 * Constructor.
 */
EventCaretPreferencesGet::EventCaretPreferencesGet()
: Event(EventTypeEnum::EVENT_CARET_PREFERENCES_GET)
{
    
}

/**
 * Destructor.
 */
EventCaretPreferencesGet::~EventCaretPreferencesGet()
{
}

/**
 * Set the caret preferences.
 *
 * @param caretPreferences
 *    Pointer to Caret Preferences
 */
void
EventCaretPreferencesGet::setCaretPreferences(CaretPreferences* caretPreferences)
{
    m_caretPreferences = caretPreferences;
}

/**
 * @return The caret preferences (NULL if caret preferences are invalid)
 */
CaretPreferences*
EventCaretPreferencesGet::getCaretPreferences() const
{
    return m_caretPreferences;
}

