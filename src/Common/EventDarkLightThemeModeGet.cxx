
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __EVENT_DARK_LIGHT_THEME_MODE_GET_DECLARE__
#include "EventDarkLightThemeModeGet.h"
#undef __EVENT_DARK_LIGHT_THEME_MODE_GET_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDarkLightThemeModeGet 
 * \brief Get the active dark light theme mode
 * \ingroup Common
 */

/**
 * Constructor.
 */
EventDarkLightThemeModeGet::EventDarkLightThemeModeGet()
: Event(EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_GET)
{
    
}

/**
 * Destructor.
 */
EventDarkLightThemeModeGet::~EventDarkLightThemeModeGet()
{
}

/**
 * @return The dark / light theme mode
 */
GuiDarkLightThemeModeEnum::Enum
EventDarkLightThemeModeGet::getDarkLightThemeMode() const
{
    if ( ! m_modeSetFlag) {
        CaretAssertMessage(0, "Mode has not been set, m_modeSetFlag is false");
    }
    return m_mode;
}

/**
 * Set the dark / light theme mode
 * @param mode
 *    The dark / light theme mode
 */
void
EventDarkLightThemeModeGet::setDarkLightThemeMode(const GuiDarkLightThemeModeEnum::Enum mode)
{
    m_mode = mode;
    m_modeSetFlag = true;
}

