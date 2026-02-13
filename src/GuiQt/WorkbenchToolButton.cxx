
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

#define __WORKBENCH_TOOL_BUTTON_DECLARE__
#include "WorkbenchToolButton.h"
#undef __WORKBENCH_TOOL_BUTTON_DECLARE__

#include <QEvent>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "WorkbenchAction.h"

using namespace caret;


    
/**
 * \class caret::WorkbenchToolButton 
 * \brief Extends QToolButton to work with light/dark mode and fixes macOS style
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param menuStatus
 *    Has menu (default NO)
 * @param parent
 *    Parent widget
 */
WorkbenchToolButton::WorkbenchToolButton(const MenuStatus menuStatus,
                                         QWidget* parent)
: QToolButton(parent),
m_menuStatus(menuStatus)
{
    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
    s_allWorkbenchToolButtons.insert(this);
    
//    std::cout << (ptrdiff_t)this << " " << s_allWorkbenchToolButtons.size() << std::endl;
//    if (s_allWorkbenchToolButtons.size() == 79) {
//        std::abort();
//    }
}

/**
 * Constructor.  Button will contain a default QAction containing the given icon type as a pixmap.
 * @param iconType
 *    Type of icon for tool button
 * @param menuStatus
 *    Has menu (default NO)
 * @param parent
 *    Parent widget
 */
WorkbenchToolButton::WorkbenchToolButton(const WorkbenchIconTypeEnum::Enum iconType,
                                         const MenuStatus menuStatus,
                                         QWidget* parent)
: QToolButton(parent),
m_menuStatus(menuStatus)
{
    QAction* action(new WorkbenchAction(iconType,
                                        this));
    setDefaultAction(action);
    
    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
    s_allWorkbenchToolButtons.insert(this);
}

/**
 * Destructor.
 */
WorkbenchToolButton::~WorkbenchToolButton()
{
    EventManager::get()->removeAllEventsFromListener(this);
    s_allWorkbenchToolButtons.erase(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
WorkbenchToolButton::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED) {
        EventDarkLightThemeModeChanged* themeChangedEvent(dynamic_cast<EventDarkLightThemeModeChanged*>(event));
        CaretAssert(themeChangedEvent);
        
        const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode(themeChangedEvent->getDarkLightThemeMode());
        updateForDarkLightTheme(darkLightThemeMode);
    }
}

void
WorkbenchToolButton::updateStyleSheet()
{
    updateForDarkLightTheme(getCurrentDarkLightThemeMode());
}


/**
 * Update the button for the given dark / light theme
 *
 * @param darkLightThemeMode
 *    The dark / light theme for the button
 */
void
WorkbenchToolButton::updateForDarkLightTheme(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
#ifdef CARET_OS_MACOSX
    updateButtonForMacOS(darkLightThemeMode);
#endif
}

/**
 * Update the button for MacOS that does not properly decorate the button
 * @param darkLightThemeMode
 *    The dark / light theme for the button
 */
void
WorkbenchToolButton::updateButtonForMacOS(const GuiDarkLightThemeModeEnum::Enum darkLightThemeMode)
{
    QColor backgroundColor(52, 52, 52, 255);
    QColor borderColor(85, 85, 85, 255);
    QColor checkedPressedColor(74, 74, 74, 255);

    switch (darkLightThemeMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            backgroundColor.setRgb(255, 255, 255);
            borderColor.setRgb(196, 196, 196);
            checkedPressedColor.setRgb(222, 222, 222);
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            backgroundColor.setRgb(52, 52, 52);
            borderColor.setRgb(85, 85, 85);
            checkedPressedColor.setRgb(74, 74, 74);
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            backgroundColor.setRgb(255, 255, 255);
            borderColor.setRgb(196, 196, 196);
            checkedPressedColor.setRgb(222, 222, 222);
            break;
    }

    QString toolButtonStyleSheet(" QToolButton { "
                                 "   background: " + backgroundColor.name() + "; ");
    switch (m_menuStatus) {
        case MenuStatus::MENU_NO:
            toolButtonStyleSheet.append("   border-style: solid; "
                                        "   border-width: 1px; "
                                        "   border-color: " + borderColor.name() + "; "
                                        "   padding-top:    2px; "
                                        "   padding-bottom: 2px; "
                                        "   padding-right:  3px; "
                                        "   padding-left:   3px; ");
            break;
        case MenuStatus::MENU_YES:
            /*
             * Border not needed for toolbutton with menu
             */
            break;
    }
    toolButtonStyleSheet.append(" } ");
    
    /*
     * Background color when button is "checked"
     */
    toolButtonStyleSheet.append(" QToolButton:checked { "
                                "   background-color: " + checkedPressedColor.name() + "; "
                                " } ");
    /*
     * Background color when button is "pressed"
     */
    toolButtonStyleSheet.append(" QToolButton:pressed { "
                                "   background-color: " + checkedPressedColor.name() + "; "
                                " } ");
    
    this->setStyleSheet(toolButtonStyleSheet);
}

/**
 * Called to show the toolbutton (overrides parent)
 * @param event
 *   The show event
 */
void
WorkbenchToolButton::showEvent(QShowEvent *event)
{
    QToolButton::showEvent(event);
}

/**
 * @return The current dark/light theme
 */
GuiDarkLightThemeModeEnum::Enum
WorkbenchToolButton::getCurrentDarkLightThemeMode() const
{
    EventDarkLightThemeModeGet themeGetEvent;
    EventManager::get()->sendEvent(themeGetEvent.getPointer());
    return themeGetEvent.getDarkLightThemeMode();
}

void
WorkbenchToolButton::printLeftoverWorkbenchToolButtons()
{
    if (s_allWorkbenchToolButtons.size() == 0) {
        std::cout << "All Workbench Tool Buttons were deleted" << std::endl;
    }
    for (WorkbenchToolButton* wa : s_allWorkbenchToolButtons) {
        std::cout << "Toolbutton not deleted: " << wa->text()
        << (ptrdiff_t)wa << std::endl;
    }
}
