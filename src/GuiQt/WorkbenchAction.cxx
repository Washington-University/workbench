
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

#define __WORKBENCH_ACTION_DECLARE__
#include "WorkbenchAction.h"
#undef __WORKBENCH_ACTION_DECLARE__

#include <QEvent>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "WorkbenchIconTypeLoader.h"

using namespace caret;
    
/**
 * \class caret::WorkbenchAction 
 * \brief Extends QAction to work with light/dark mode for the icon
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param iconType
 *    Type of icon for tool button
 * @param parent
 *    Parent widget.  Required to ensure that this gets destroyed to event listeners removed
 */
WorkbenchAction::WorkbenchAction(const WorkbenchIconTypeEnum::Enum iconType,
                                 QObject* parent)
: QAction(parent),
m_iconType(iconType)
{
    static bool firstFlag(true);
    if (firstFlag) {
        firstFlag = false;
    }

    m_darkPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                  GuiDarkLightThemeModeEnum::DARK);
    m_lightPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                  GuiDarkLightThemeModeEnum::LIGHT);

    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
    
    s_allWorkbenchActions.insert(this);
}

/**
 * Destructor.
 */
WorkbenchAction::~WorkbenchAction()
{
    EventManager::get()->removeAllEventsFromListener(this);
    s_allWorkbenchActions.erase(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
WorkbenchAction::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED) {
        EventDarkLightThemeModeChanged* themeChangedEvent(dynamic_cast<EventDarkLightThemeModeChanged*>(event));
        CaretAssert(themeChangedEvent);
        
        const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode(themeChangedEvent->getDarkLightThemeMode());
        updateForDarkLightTheme(darkLightThemeMode);
    }
}

/**
 * Update the button for the given dark / light theme
 *
 * @param darkLightThemeMode
 *    The dark / light theme for the button
 */
void
WorkbenchAction::updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            setIcon(m_lightPixmap);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            setIcon(m_darkPixmap);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            setIcon(m_lightPixmap);
            break;
    }
}

/**
 * @return The current dark/light theme
 */
GuiDarkLightThemeModeEnum::Enum
WorkbenchAction::getCurrentDarkLightThemeMode() const
{
    EventDarkLightThemeModeGet themeGetEvent;
    EventManager::get()->sendEvent(themeGetEvent.getPointer());
    return themeGetEvent.getDarkLightThemeMode();
}

void
WorkbenchAction::printLeftoverWorkbenchActions()
{
    if (s_allWorkbenchActions.size() == 0) {
        std::cout << "All Workbench Actions were deleted" << std::endl;
    }
    for (WorkbenchAction* wa : s_allWorkbenchActions) {
        std::cout << WorkbenchIconTypeEnum::toName(wa->m_iconType) << std::endl;
    }
}

