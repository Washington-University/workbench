#ifndef __EVENT_DARK_LIGHT_THEME_MODE_CHANGED_H__
#define __EVENT_DARK_LIGHT_THEME_MODE_CHANGED_H__

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



#include <memory>

#include "Event.h"
#include "GuiDarkLightThemeModeEnum.h"


namespace caret {

    class EventDarkLightThemeModeChanged : public Event {
        
    public:
        EventDarkLightThemeModeChanged(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode);
        
        virtual ~EventDarkLightThemeModeChanged();
        
        EventDarkLightThemeModeChanged(const EventDarkLightThemeModeChanged&) = delete;

        EventDarkLightThemeModeChanged& operator=(const EventDarkLightThemeModeChanged&) = delete;
        
        GuiDarkLightThemeModeEnum::Enum getDarkLightThemeMode() const;

        // ADD_NEW_METHODS_HERE

    private:
        const GuiDarkLightThemeModeEnum::Enum m_darkLightThemeMode;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DARK_LIGHT_THEME_MODE_CHANGED_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DARK_LIGHT_THEME_MODE_CHANGED_DECLARE__

} // namespace
#endif  //__EVENT_DARK_LIGHT_THEME_MODE_CHANGED_H__
