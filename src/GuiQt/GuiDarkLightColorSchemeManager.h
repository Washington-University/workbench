#ifndef __GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_H__
#define __GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_H__

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

#include <QObject>

#include "EventListenerInterface.h"
#include "GuiDarkLightColorSchemeModeEnum.h"

namespace caret {

    class GuiDarkLightColorSchemeManager : public QObject, public EventListenerInterface {
        
    public:
        GuiDarkLightColorSchemeManager(QObject* parent);
        
        virtual ~GuiDarkLightColorSchemeManager();
        
        GuiDarkLightColorSchemeManager(const GuiDarkLightColorSchemeManager&) = delete;

        GuiDarkLightColorSchemeManager& operator=(const GuiDarkLightColorSchemeManager&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        void qtColorSchemeChanged(Qt::ColorScheme colorScheme);
#endif
    private:
        GuiDarkLightColorSchemeModeEnum::Enum getActiveDarkLightColorScheme() const;
        
        void applyDarkLightColorScheme();
        
        void darkLightColorSchemeChangedByPaletteChangeEventInBrowserWindow();
        
        void darkLightColorSchemeChangedByPreferencesGeneralWidget();
        
        void setCurrentDarkLightColorSchemeToValueInPreferences();
        
        GuiDarkLightColorSchemeModeEnum::Enum m_currentDarkLightColorSchemeMode = GuiDarkLightColorSchemeModeEnum::LIGHT;
        
        // ADD_NEW_MEMBERS_HERE

        friend class BrainBrowserWindow;
        friend class PreferencesGeneralWidget;
    };
    
#ifdef __GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_DECLARE__

} // namespace
#endif  //__GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_H__
