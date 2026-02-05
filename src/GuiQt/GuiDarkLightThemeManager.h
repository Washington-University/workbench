#ifndef __GUI_DARK_LIGHT_THEME_MANAGER_H__
#define __GUI_DARK_LIGHT_THEME_MANAGER_H__

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

#include "CaretObject.h"

#include "EventListenerInterface.h"
#include "GuiDarkLightThemeModeEnum.h"

namespace caret {

    class GuiDarkLightThemeManager : public CaretObject, public EventListenerInterface {
        
    public:
        GuiDarkLightThemeManager();
        
        virtual ~GuiDarkLightThemeManager();
        
        GuiDarkLightThemeManager(const GuiDarkLightThemeManager&) = delete;

        GuiDarkLightThemeManager& operator=(const GuiDarkLightThemeManager&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual void receiveEvent(Event* event);

    private:
        GuiDarkLightThemeModeEnum::Enum getActiveDarkLightTheme() const;
        
        void applyDarkLightThemeOnMacOS();
        
        void darkLightThemeChangedByPaletteChangeEventInBrowserWindow();
        
        void darkLightThemeChangedByPreferencesGeneralWidget();
        
        void setCurrentDarkLightThemeToValueInPreferences();
        
        GuiDarkLightThemeModeEnum::Enum m_currentDarkLightThemeMode = GuiDarkLightThemeModeEnum::LIGHT;
        
        // ADD_NEW_MEMBERS_HERE

        friend class BrainBrowserWindow;
        friend class PreferencesGeneralWidget;
    };
    
#ifdef __GUI_DARK_LIGHT_THEME_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GUI_DARK_LIGHT_THEME_MANAGER_DECLARE__

} // namespace
#endif  //__GUI_DARK_LIGHT_THEME_MANAGER_H__
