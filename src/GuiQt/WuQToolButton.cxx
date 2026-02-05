
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

#define __WU_Q_TOOL_BUTTON_DECLARE__
#include "WuQToolButton.h"
#undef __WU_Q_TOOL_BUTTON_DECLARE__

#include <QEvent>
#include <QImage>

#include "CaretAssert.h"
#include "EventDarkLightThemeModeChanged.h"
#include "EventDarkLightThemeModeGet.h"
#include "EventManager.h"
#include "GuiManager.h"

using namespace caret;


    
/**
 * \class caret::WuQToolButton 
 * \brief Extends QToolButton to work with light/dark mode and fixes macOS style
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
WuQToolButton::WuQToolButton(QWidget* parent)
: QToolButton(parent)
{
    setStyleOnMacOS();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED);
}

/**
 * Destructor.
 */
WuQToolButton::~WuQToolButton()
{
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
WuQToolButton::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DARK_LIGHT_THEME_MODE_CHANGED) {
        EventDarkLightThemeModeChanged* themeChangedEvent(dynamic_cast<EventDarkLightThemeModeChanged*>(event));
        CaretAssert(themeChangedEvent);
    }
}

/**
 * Set the light icon
 */
void
WuQToolButton::setLightIcon(const QIcon& icon)
{
    m_lightIcon = icon;
    
    if ( ! m_lightIcon.isNull()) {
        QPixmap pm(m_lightIcon.pixmap(32, 32));
        QImage image(pm.toImage());
        image.invertPixels(QImage::InvertRgb);
        QPixmap pmDark(QPixmap::fromImage(image));
        m_darkIcon = QIcon(pmDark);
    }
    
    m_iconsCreatedFlag = ( ! m_lightIcon.isNull());
}

/**
 * With Qt5, a toolbutton placed into a toolbar uses the
 * background of the toolbar with no border and appears
 * similar to a label.  Use a stylesheet so that
 * the button appears similar to Qt4.
 *
 */
void
WuQToolButton::setStyleOnMacOS()
{
//    style sheets cause lots of problems
//    focus on just the line box around button and icons 
    
    
    setStyleSheet(" QToolButton { } ");
    
    EventDarkLightThemeModeGet themeModeEvent;
    EventManager::get()->sendEvent(themeModeEvent.getPointer());
    CaretAssert(themeModeEvent.getEventProcessCount() > 0);
    const GuiDarkLightThemeModeEnum::Enum darkLightMode = themeModeEvent.getDarkLightThemeMode();

    switch (darkLightMode) {
        case GuiDarkLightThemeModeEnum::SYSTEM:
            CaretAssert(0);
            setStyleOnMacOSLight();
            break;
        case GuiDarkLightThemeModeEnum::DARK:
            setStyleOnMacOSDark();
            break;
        case GuiDarkLightThemeModeEnum::LIGHT:
            setStyleOnMacOSLight();
            break;
    }
}

/**
 * Set the stylesheet for "dark" theme
 */
void
WuQToolButton::setStyleOnMacOSDark()
{
#ifdef CARET_OS_MACOSX
    bool hasMenuFlag = false;
    bool hasCheckableFlag = false;
    QAction* action = this->defaultAction();
    if (action != NULL) {
        if (action->menu() != NULL) {
            hasMenuFlag = true;
        }
        if (action->isCheckable()) {
            hasCheckableFlag = true;
        }
    }
    
    const QPalette palette = this->palette();
    const QPalette::ColorRole backgroundRole = this->backgroundRole();
    const QBrush backgroundBrush = palette.brush(backgroundRole);
    const QColor backgroundColor = backgroundBrush.color();
    const QColor lighterColor    = backgroundColor.lighter(125);
    const QColor darkerColor = backgroundColor.darker(100);
    const QColor slightlyLighterColor = backgroundColor.lighter(115);
    
    /*
     const QBrush backgroundBrush = palette.brush(backgroundRole);
     const QColor backgroundColor = backgroundBrush.color();
     const QColor lighterColor    = backgroundColor.lighter(100);
     const QColor darkerColor = backgroundColor.darker(125);
     const QColor slightlyDarkerColor = backgroundColor.darker(115);

     */
    /*
     * Use a stylesheet to:
     * (1) Make the background of the button lighter
     * (2) Add a border around the button that is slightly
     *     darker than the background.
     */
    QString toolButtonStyleSheet(" QToolButton { ");
//    toolButtonStyleSheet.append("   background: " + darkerColor.name() + "; ");
    if (hasMenuFlag) {
        //        toolButtonStyleSheet.append("   border-style: solid; "
        //                                    "   border-width: 1px; "
        //                                    "   border-color: " + darkerColor.name() + "; "
        //                                    "   padding-top:    6px; "
        //                                    "   padding-bottom: 6px; "
        //                                    "   padding-right:  4px; "
        //                                    "   padding-left:   3px; ");
    }
    else {
        toolButtonStyleSheet.append("   border-style: solid; "
                                    "   border-width: 1px; "
                                    "   border-color: " + lighterColor.name() + "; "
                                    "   padding-top:    2px; "
                                    "   padding-bottom: 2px; "
                                    "   padding-right:  3px; "
                                    "   padding-left:   3px; ");
    }
    toolButtonStyleSheet.append(" } ");
    
//    if (hasCheckableFlag) {
//        /*
//         * Background color when button is "checked"
//         */
//        toolButtonStyleSheet.append(" QToolButton:checked { "
//                                    "   background-color: " + slightlyLighterColor.name() + "; "
//                                    " } ");
//    }
//    else {
//        /*
//         * Background color when button is "pressed"
//         */
//        toolButtonStyleSheet.append(" QToolButton:pressed { "
//                                    "   background-color: " + slightlyLighterColor.name() + "; "
//                                    " } ");
//    }
    
    this->setStyleSheet(toolButtonStyleSheet);
#else
#endif
}

/**
 * Set the stylesheet for "light" theme
 */
void
WuQToolButton::setStyleOnMacOSLight()
{
#ifdef CARET_OS_MACOSX
    bool hasMenuFlag = false;
    bool hasCheckableFlag = false;
    QAction* action = this->defaultAction();
    if (action != NULL) {
        if (action->menu() != NULL) {
            hasMenuFlag = true;
        }
        if (action->isCheckable()) {
            hasCheckableFlag = true;
        }
    }
    
    const QPalette palette = this->palette();
    const QPalette::ColorRole backgroundRole = this->backgroundRole();
    const QBrush backgroundBrush = palette.brush(backgroundRole);
    const QColor backgroundColor = backgroundBrush.color();
    const QColor lighterColor    = backgroundColor.lighter(100);
    const QColor darkerColor = backgroundColor.darker(125);
    const QColor slightlyDarkerColor = backgroundColor.darker(115);
    
    /*
     * Use a stylesheet to:
     * (1) Make the background of the button lighter
     * (2) Add a border around the button that is slightly
     *     darker than the background.
     */
    QString toolButtonStyleSheet(" QToolButton { ");
//    toolButtonStyleSheet.append("   background: " + lighterColor.name() + "; ");
    
    if (hasMenuFlag) {
        //        toolButtonStyleSheet.append("   border-style: solid; "
        //                                    "   border-width: 1px; "
        //                                    "   border-color: " + darkerColor.name() + "; "
        //                                    "   padding-top:    6px; "
        //                                    "   padding-bottom: 6px; "
        //                                    "   padding-right:  4px; "
        //                                    "   padding-left:   3px; ");
    }
    else {
        toolButtonStyleSheet.append("   border-style: solid; "
                                    "   border-width: 1px; "
                                    "   border-color: " + darkerColor.name() + "; "
                                    "   padding-top:    2px; "
                                    "   padding-bottom: 2px; "
                                    "   padding-right:  3px; "
                                    "   padding-left:   3px; ");
    }
    toolButtonStyleSheet.append(" } ");
    
//    if (hasCheckableFlag) {
//        /*
//         * Background color when button is "checked"
//         */
//        toolButtonStyleSheet.append(" QToolButton:checked { "
//                                    "   background-color: " + slightlyDarkerColor.name() + "; "
//                                    " } ");
//    }
//    else {
//        /*
//         * Background color when button is "pressed"
//         */
//        toolButtonStyleSheet.append(" QToolButton:pressed { "
//                                    "   background-color: " + slightlyDarkerColor.name() + "; "
//                                    " } ");
//    }
    
    this->setStyleSheet(toolButtonStyleSheet);
#else
#endif
}

/**
 * Called to show the toolbutton (overrides parent)
 * @param event
 *   The show event
 */
void
WuQToolButton::showEvent(QShowEvent *event)
{
    QToolButton::showEvent(event);
}


