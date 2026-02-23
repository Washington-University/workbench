
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
#include "EventDarkLightColorSchemeModeChanged.h"
#include "EventDarkLightColorSchemeModeGet.h"
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
    initialize();
    
    updateForDarkLightColorScheme(getCurrentDarkLightColorSchemeMode());
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
    initialize();
    
    setDefaultAction(new WorkbenchAction(iconType,
                                         this));
    
    updateForDarkLightColorScheme(getCurrentDarkLightColorSchemeMode());
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
 * Initialize this instance
 */
void
WorkbenchToolButton::initialize()
{
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_CHANGED);
//    std::cout << (ptrdiff_t)this << " " << s_allWorkbenchButtonsCounter << std::endl;
    s_allWorkbenchToolButtons[this] = s_allWorkbenchButtonsCounter;
    ++s_allWorkbenchButtonsCounter;
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
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_COLOR_SCHEME_MODE_CHANGED) {
        EventDarkLightColorSchemeModeChanged* colorSchemeChangedEvent(dynamic_cast<EventDarkLightColorSchemeModeChanged*>(event));
        CaretAssert(colorSchemeChangedEvent);
        
        const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode(colorSchemeChangedEvent->getDarkLightColorSchemeMode());
        updateForDarkLightColorScheme(darkLightColorSchemeMode);
    }
}

void
WorkbenchToolButton::updateStyleSheet()
{
    updateForDarkLightColorScheme(getCurrentDarkLightColorSchemeMode());
}


/**
 * Update the button for the given dark / light color scheme
 *
 * @param darkLightColorSchemeMode
 *    The dark / light color scheme for the button
 */
void
WorkbenchToolButton::updateForDarkLightColorScheme(const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode)
{
#ifdef CARET_OS_MACOSX
    updateButtonForMacOS(darkLightColorSchemeMode);
#endif
}

/**
 * Set the special background color
 * @param specialBackgroundColor
 *    The special background color
 */
void
WorkbenchToolButton::setSpecialBackgroundColor(const QColor& specialBackgroundColor)
{
    m_specialBackgroundColor = specialBackgroundColor;
}

/**
 * Enabled/disable the special background color
 * @param enabled
 *    Enabled status
 */
void
WorkbenchToolButton::setSpecialBackgroundColorEnabled(const bool enabled)
{
    m_specialBackgroundColorEnabled = enabled;
    updateStyleSheet();
}

/**
 * Update the button for MacOS that does not properly decorate the button
 * @param darkLightColorSchemeMode
 *    The dark / light color scheme for the button
 */
void
WorkbenchToolButton::updateButtonForMacOS(const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode)
{
    QColor backgroundColor(52, 52, 52, 255);
    QColor borderColor(85, 85, 85, 255);
    QColor checkedPressedColor(74, 74, 74, 255);

    switch (darkLightColorSchemeMode) {
        case GuiDarkLightColorSchemeModeEnum::SYSTEM:
            CaretAssert(0);
            backgroundColor.setRgb(255, 255, 255);
            borderColor.setRgb(196, 196, 196);
            checkedPressedColor.setRgb(222, 222, 222);
            break;
        case GuiDarkLightColorSchemeModeEnum::DARK:
            backgroundColor.setRgb(52, 52, 52);
            borderColor.setRgb(85, 85, 85);
            checkedPressedColor.setRgb(74, 74, 74);
            break;
        case GuiDarkLightColorSchemeModeEnum::LIGHT:
            backgroundColor.setRgb(255, 255, 255);
            borderColor.setRgb(196, 196, 196);
            checkedPressedColor.setRgb(222, 222, 222);
            break;
    }
    
    if (m_specialBackgroundColorEnabled) {
        backgroundColor = m_specialBackgroundColor;
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
 * @return The current dark/light color scheme
 */
GuiDarkLightColorSchemeModeEnum::Enum
WorkbenchToolButton::getCurrentDarkLightColorSchemeMode() const
{
    EventDarkLightColorSchemeModeGet colorSchemeGetEvent;
    EventManager::get()->sendEvent(colorSchemeGetEvent.getPointer());
    return colorSchemeGetEvent.getDarkLightColorSchemeMode();
}

void
WorkbenchToolButton::printLeftoverWorkbenchToolButtons()
{
    if (s_allWorkbenchToolButtons.size() == 0) {
        std::cout << "All Workbench Tool Buttons were deleted" << std::endl;
    }
    for (auto& buttonAndCounter: s_allWorkbenchToolButtons) {
        WorkbenchToolButton* button(buttonAndCounter.first);
        std::cout << "Toolbutton not deleted: " << button->text()
        << " ptr: " << (ptrdiff_t)button << std::endl
        << " counter: " << buttonAndCounter.second << std::endl;
    }
}
