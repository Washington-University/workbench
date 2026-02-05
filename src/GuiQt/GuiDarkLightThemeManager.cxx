
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

#define __GUI_DARK_LIGHT_THEME_MANAGER_DECLARE__
#include "GuiDarkLightThemeManager.h"
#undef __GUI_DARK_LIGHT_THEME_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventManager.h"
#ifdef CARET_OS_MACOSX
#include "MacDarkTheme.h"
#endif
#include "SessionManager.h"


using namespace caret;
    
/**
 * \class caret::GuiDarkLightThemeManager 
 * \brief Manages dark / light theme processing
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
GuiDarkLightThemeManager::GuiDarkLightThemeManager()
: CaretObject()
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_GET);
    
    setCurrentDarkLightThemeToValueInPreferences();
    
    /*
     * This results in the applications starting with the
     * theme that user has in preferences
     */
    applyDarkLightThemeOnMacOS();
}

/**
 * Destructor.
 */
GuiDarkLightThemeManager::~GuiDarkLightThemeManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
GuiDarkLightThemeManager::toString() const
{
    return "GuiDarkLightThemeManager";
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GuiDarkLightThemeManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_GET) {
        EventDarkLightThemeModeGet* themeModeGetEvent = dynamic_cast<EventDarkLightThemeModeGet*>(event);
        CaretAssert(themeModeGetEvent);
        themeModeGetEvent->setDarkLightThemeMode(getActiveDarkLightTheme());
        
        event->setEventProcessed();
    }
    
    /*
     * Note: EventDarkLightThemeModeChanged is sent by this class
     * when widgets need to be notified that the them has changed.
     */
}

/**
 * @return Returns either DARK or LIGHT only,  never SYSTEM
 */
GuiDarkLightThemeModeEnum::Enum
GuiDarkLightThemeManager::getActiveDarkLightTheme() const
{
    GuiDarkLightThemeModeEnum::Enum activeDarkLightThemeMode = m_currentDarkLightThemeMode;
    switch (activeDarkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::DARK:
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            break;
        case GuiDarkLightThemeModeEnum::SYSTEM:
#ifdef CARET_OS_MACOSX
            if (macIsInDarkTheme()) {
                activeDarkLightThemeMode = GuiDarkLightThemeModeEnum::DARK;
            }
            else {
                activeDarkLightThemeMode = GuiDarkLightThemeModeEnum::LIGHT;
            }
#else
            /*
             * Use LIGHT on Linux and Windows
             */
            activeDarkLightThemeMode = GuiDarkLightThemeModeEnum::LIGHT;
#endif
            break;
    }
    
    return activeDarkLightThemeMode;
}

/**
 * Apply the dark / light theme on macOS.
 * It tells macOS which theme to use when creating user-interface components
 */
void
GuiDarkLightThemeManager::applyDarkLightThemeOnMacOS()
{
#ifdef CARET_OS_MACOSX
    switch (m_currentDarkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            macSetToAutoTheme();
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            macSetToDarkTheme();
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            macSetToLightTheme();
            break;
    }
#endif
}

/**
 * Called when the dark light theme is changed by a Qt's PaletteChange event received by BrainBrowserWindow
 */
void
GuiDarkLightThemeManager::darkLightThemeChangedByPaletteChangeEventInBrowserWindow()
{
    /*
     * Note preferences values is NOT changed.
     * The PaletteChange is sent by the macOS operating system to the
     * BrainBrowserWindow at dusk/dawn
     */
    
    applyDarkLightThemeOnMacOS();
    
    EventManager::get()->sendEvent(EventDarkLightThemeModeChanged(getActiveDarkLightTheme()).getPointer());
}


/**
 * Called when the dark light theme is changed by the user on the Preferences -> General widget
 */
void
GuiDarkLightThemeManager::darkLightThemeChangedByPreferencesGeneralWidget()
{
    setCurrentDarkLightThemeToValueInPreferences();
    
    applyDarkLightThemeOnMacOS();
    
    EventManager::get()->sendEvent(EventDarkLightThemeModeChanged(getActiveDarkLightTheme()).getPointer());
}

/**
 * Set the current dark light theme to the value in the user's preferences
 */
void
GuiDarkLightThemeManager::setCurrentDarkLightThemeToValueInPreferences()
{
    CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
    m_currentDarkLightThemeMode = prefs->getDarkLightThemeMode();
}






