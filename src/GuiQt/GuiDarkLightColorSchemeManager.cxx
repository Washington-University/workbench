
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

#include <QGuiApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <QStyleHints>
#endif

#define __GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_DECLARE__
#include "GuiDarkLightColorSchemeManager.h"
#undef __GUI_DARK_LIGHT_COLOR_SCHEME_MANAGER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EventDarkLightColorSchemeModeGet.h"
#include "EventDarkLightColorSchemeModeChanged.h"
#include "EventManager.h"
#ifdef CARET_OS_MACOSX
#include "MacDarkTheme.h"
#endif
#include "SessionManager.h"


using namespace caret;
    
/**
 * \class caret::GuiDarkLightColorSchemeManager 
 * \brief Manages dark / light color scheme processing
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent widget
 */
GuiDarkLightColorSchemeManager::GuiDarkLightColorSchemeManager(QObject* parent)
: QObject(parent)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    QStyleHints* styleHints(QGuiApplication::styleHints());
    CaretAssert(styleHints);
    QObject::connect(styleHints, &QStyleHints::colorSchemeChanged,
                     this, &GuiDarkLightColorSchemeManager::qtColorSchemeChanged);
#endif
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_CHANGED);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_GET);
    
    setCurrentDarkLightColorSchemeToValueInPreferences();
    
    /*
     * This results in the applications starting with the
     * color scheme that user has in preferences
     */
    applyDarkLightColorScheme();
    
#ifdef WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_QT
    CaretLogInfo("Dark/Light Color Scheme: Qt");
#elif WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_MACOS
    CaretLogInfo("Dark/Light Color Scheme: macOS");
#else
    CaretLogInfo("Dark/Light Color Scheme: None");
#endif
}

/**
 * Destructor.
 */
GuiDarkLightColorSchemeManager::~GuiDarkLightColorSchemeManager()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GuiDarkLightColorSchemeManager::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_GET) {
        EventDarkLightColorSchemeModeGet* colorSchemeModeGetEvent = dynamic_cast<EventDarkLightColorSchemeModeGet*>(event);
        CaretAssert(colorSchemeModeGetEvent);
        colorSchemeModeGetEvent->setDarkLightColorSchemeMode(getActiveDarkLightColorScheme());
        
        event->setEventProcessed();
    }
    
    /*
     * Note: EventDarkLightColorSchemeModeChanged is sent by this class
     * when widgets need to be notified that the them has changed.
     */
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
/**
 * Gets called when the qt color scheme is changed.  This will probably occur
 * if the operating system requests a change such as macOS at dusk/dawn.
 * @param colorScheme
 *    New color scheme.
 */
void
GuiDarkLightColorSchemeManager::qtColorSchemeChanged(Qt::ColorScheme colorScheme)
{
    GuiDarkLightColorSchemeModeEnum::Enum wbColorScheme(GuiDarkLightColorSchemeModeEnum::SYSTEM);
    AString text("Qt Color Scheme Changed: Invalid");
    switch (colorScheme) {
        case Qt::ColorScheme::Unknown:
            wbColorScheme = GuiDarkLightColorSchemeModeEnum::SYSTEM;
            text = "Qt: Color Scheme Changed: Unknown";
            break;
        case Qt::ColorScheme::Light:
            wbColorScheme = GuiDarkLightColorSchemeModeEnum::LIGHT;
            text = "Qt: Color Scheme Changed: Light";
            break;
        case Qt::ColorScheme::Dark:
            wbColorScheme = GuiDarkLightColorSchemeModeEnum::DARK;
            text = "Qt: Color Scheme Changed: Dark";
            break;
    }
    
    CaretLogFine(text);
    
    CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
    CaretAssert(prefs);
    prefs->setDarkLightColorSchemeMode(wbColorScheme);
    
#ifdef WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_QT
    applyDarkLightColorScheme();
    
    EventManager::get()->sendEvent(EventDarkLightColorSchemeModeChanged(getActiveDarkLightColorScheme()).getPointer());
#endif
}
#endif

/**
 * @return Returns either DARK or LIGHT only,  never SYSTEM
 */
GuiDarkLightColorSchemeModeEnum::Enum
GuiDarkLightColorSchemeManager::getActiveDarkLightColorScheme() const
{
    GuiDarkLightColorSchemeModeEnum::Enum activeDarkLightColorSchemeMode = m_currentDarkLightColorSchemeMode;
    switch (activeDarkLightColorSchemeMode) {
        case GuiDarkLightColorSchemeModeEnum::DARK:
            break;
        case GuiDarkLightColorSchemeModeEnum::LIGHT:
            break;
        case GuiDarkLightColorSchemeModeEnum::SYSTEM:
#ifdef WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_MACOS
            if (macIsInDarkTheme()) {
                activeDarkLightColorSchemeMode = GuiDarkLightColorSchemeModeEnum::DARK;
            }
            else {
                activeDarkLightColorSchemeMode = GuiDarkLightColorSchemeModeEnum::LIGHT;
            }
#else
            /*
             * Use LIGHT on Linux and Windows
             */
            activeDarkLightColorSchemeMode = GuiDarkLightColorSchemeModeEnum::LIGHT;
#endif
            break;
    }
    
    return activeDarkLightColorSchemeMode;
}

/**
 * Apply the dark / light color scheme on macOS.
 * It tells macOS which color scheme to use when creating user-interface components
 */
void
GuiDarkLightColorSchemeManager::applyDarkLightColorScheme()
{
#ifdef WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_MACOS
    switch (m_currentDarkLightColorSchemeMode) {
        case GuiDarkLightColorSchemeModeEnum::SYSTEM:
            macSetToAutoTheme();
            break;
        case GuiDarkLightColorSchemeModeEnum::DARK:
            macSetToDarkTheme();
            break;
        case GuiDarkLightColorSchemeModeEnum::LIGHT:
            macSetToLightTheme();
            break;
    }
#endif
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#ifdef WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_QT
    QStyleHints* styleHints(QGuiApplication::styleHints());
    CaretAssert(styleHints);

    /*
     * Block signals from QStyleHints or else this method
     * will get called again.  The signal only is sent
     * if the style changes so it won't cause an infinite
     * loop but still best to avoid the possibility of it.
     */
    QSignalBlocker blocker(styleHints);
    switch (m_currentDarkLightColorSchemeMode) {
        case GuiDarkLightColorSchemeModeEnum::SYSTEM:
            styleHints->setColorScheme(Qt::ColorScheme::Unknown);
            break;
        case GuiDarkLightColorSchemeModeEnum::DARK:
            styleHints->setColorScheme(Qt::ColorScheme::Dark);
            break;
        case GuiDarkLightColorSchemeModeEnum::LIGHT:
            styleHints->setColorScheme(Qt::ColorScheme::Light);
            break;
    }
#endif
#endif
}

/**
 * Called when the dark light color scheme is changed by a Qt's PaletteChange event received by BrainBrowserWindow
 */
void
GuiDarkLightColorSchemeManager::darkLightColorSchemeChangedByPaletteChangeEventInBrowserWindow()
{
    /*
     * Note preferences values is NOT changed.
     * The PaletteChange is sent by the macOS operating system to the
     * BrainBrowserWindow at dusk/dawn
     */
    CaretLogFine("Dark light changed by macOS Palette Event");
#ifdef WORKBENCH_DARK_LIGHT_COLOR_SCHEME_USE_MACOS
    applyDarkLightColorScheme();
    
    EventManager::get()->sendEvent(EventDarkLightColorSchemeModeChanged(getActiveDarkLightColorScheme()).getPointer());
#endif
}


/**
 * Called when the dark light color scheme is changed by the user on the Preferences -> General widget
 */
void
GuiDarkLightColorSchemeManager::darkLightColorSchemeChangedByPreferencesGeneralWidget()
{
    CaretLogFine("Dark light changed by user on preferences widget");
    
    setCurrentDarkLightColorSchemeToValueInPreferences();
    
    applyDarkLightColorScheme();
    
    EventManager::get()->sendEvent(EventDarkLightColorSchemeModeChanged(getActiveDarkLightColorScheme()).getPointer());
}

/**
 * Set the current dark light color scheme to the value in the user's preferences
 */
void
GuiDarkLightColorSchemeManager::setCurrentDarkLightColorSchemeToValueInPreferences()
{
    CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
    m_currentDarkLightColorSchemeMode = prefs->getDarkLightColorSchemeMode();
}






