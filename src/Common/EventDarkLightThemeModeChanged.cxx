
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

#define __EVENT_DARK_LIGHT_THEME_MODE_CHANGED_DECLARE__
#include "EventDarkLightThemeModeChanged.h"
#undef __EVENT_DARK_LIGHT_THEME_MODE_CHANGED_DECLARE__

#include "CaretAssert.h"
#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventDarkLightThemeModeChanged 
 * \brief Event issues when dark / light theme mode changed
 * \ingroup Common
 */

/**
 * Constructor.
 * @param darkLightThemeMode
 *    The new dark / light theme mode
 */
EventDarkLightThemeModeChanged::EventDarkLightThemeModeChanged(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
: Event(EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED),
m_darkLightThemeMode(darkLightThemeMode)
{
    
}

/**
 * Destructor.
 */
EventDarkLightThemeModeChanged::~EventDarkLightThemeModeChanged()
{
}

/**
 * @return The dark / light theme mode
 */
GuiDarkLightThemeModeEnum::Enum
EventDarkLightThemeModeChanged::getDarkLightThemeMode() const
{
    return m_darkLightThemeMode;
}

