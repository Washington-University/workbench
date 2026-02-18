
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

#define __WORKBENCH_LABEL_DECLARE__
#include "WorkbenchLabel.h"
#undef __WORKBENCH_LABEL_DECLARE__

#include "CaretAssert.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "WorkbenchAction.h"
#include "WorkbenchIconTypeLoader.h"

using namespace caret;


    
/**
 * \class caret::WorkbenchLabel 
 * \brief Extends QLabel and handles dark / light update for a workbench icon
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param iconType
 *    Type of icon for tool button
 * @param parent
 *    Parent widget.  Required to ensure that this gets destroyed to event listeners removed
 */
WorkbenchLabel::WorkbenchLabel(const WorkbenchIconTypeEnum::Enum iconType,
                               QWidget* parent)
: QLabel(parent),
m_iconType(iconType)
{
    if (WorkbenchAction::isColorIcon(iconType)) {
        /*
         * Color icons are the same for both light and dark
         */
        m_darkPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                      GuiDarkLightThemeModeEnum::LIGHT);
    }
    else {
        m_darkPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                      GuiDarkLightThemeModeEnum::DARK);
    }
    m_lightPixmap = WorkbenchIconTypeLoader::loadPixmapForIconType(iconType,
                                                                   GuiDarkLightThemeModeEnum::LIGHT);
    
    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
}

/**
 * Destructor.
 */
WorkbenchLabel::~WorkbenchLabel()
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
WorkbenchLabel::receiveEvent(Event* event)
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
WorkbenchLabel::updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            setPixmap(m_lightPixmap);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            setPixmap(m_darkPixmap);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            setPixmap(m_lightPixmap);
            break;
    }
    
    setAlignment(Qt::AlignCenter);
}

/**
 * @return The current dark/light theme
 */
GuiDarkLightThemeModeEnum::Enum
WorkbenchLabel::getCurrentDarkLightThemeMode() const
{
    EventDarkLightThemeModeGet themeGetEvent;
    EventManager::get()->sendEvent(themeGetEvent.getPointer());
    return themeGetEvent.getDarkLightThemeMode();
}

