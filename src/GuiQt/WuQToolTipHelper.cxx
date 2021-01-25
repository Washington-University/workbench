
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __WU_Q_TOOL_TIP_HELPER_DECLARE__
#include "WuQToolTipHelper.h"
#undef __WU_Q_TOOL_TIP_HELPER_DECLARE__

#include <QEvent>
#include <QMenu>
#include <QToolTip>
#include <QWhatsThis>

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::WuQToolTipHelper 
 * \brief Assists with tooltip for GUI components
 * \ingroup GuiQt
 */

/**
 * @returns A tooltip helper for the given menu.  The helper will be a child of the menu
 * so that when the menu is destroyed, the helper will also be destoryed.  Thus, the
 * returned value can be ignored.
 *
 * @param menu
 *    Menu that receives a tooltip helper
 */
WuQToolTipHelper*
WuQToolTipHelper::newInstanceForMenu(QMenu* menu)
{
    CaretAssert(menu);
    
    WuQToolTipHelper* helper = new WuQToolTipHelper(Mode::MENU_TOOLTIPS,
                                                    menu);

    return helper;
}


/**
 * Constructor.
 * @param mode
 *    The mode
 * @param parent
 *    The parent gui component that uses the tool tip helper
 */
WuQToolTipHelper::WuQToolTipHelper(const Mode mode,
                                   QObject* parent)
: QObject(parent),
m_mode(mode)
{
    CaretAssert(parent);
    
    switch (mode) {
        case Mode::MENU_TOOLTIPS:
        {
            m_menu = qobject_cast<QMenu*>(parent);
            CaretAssert(m_menu);
            if (m_menu == NULL) {
                return;
            }
            
            /*
             * Show tooltip when menu action is under mouse
             */
            QObject::connect(m_menu, &QMenu::hovered,
                             this, &WuQToolTipHelper::menuActionHovered);
            
            /*
             * ToolTip may remain up after menu is closed.
             * So, hide tooltip as menu is closing.
             */
            QObject::connect(m_menu, &QMenu::aboutToHide,
                             this, &WuQToolTipHelper::hideToolTip);
        }
            break;
    }
}

/**
 * Destructor.
 */
WuQToolTipHelper::~WuQToolTipHelper()
{
}

/**
 * Slot called when the mouse is over a menu item
 * @param action
 *    Action that is under mouse.  If action has non-empty tooltip text, a tooltip is displayed near the menu
 */
void
WuQToolTipHelper::menuActionHovered(QAction* action)
{
    CaretAssert(action);
    
    AString text(action->toolTip());

    if (m_useWhatsThisFlag) {
        if (text.isEmpty()) {
            QWhatsThis::hideText();
        }
        else {
            /*
             * If an action does not have a tooltip, Qt
             * return's the action's text as a tooltip.
             * In this case, do not show tooltip.
             */
            if (action->text() == action->toolTip()) {
                QWhatsThis::hideText();
                return;
            }
            else {
                /*
                 * If tooltip is not formatted, word wrap the text
                 */
                if ( ! text.toLower().contains("<html>")) {
                    text = ("<html><body>" + text + "</body></html>"); //WuQtUtilities::createWordWrappedToolTipText(text);
                }
                
                /*
                 * Show tooltip containing scene description below mouse
                 * (note: positive Y is down in Qt coords)
                 */
                const QPoint toolTipXY(QCursor::pos()
                                       + QPoint(0, 15));
                QWhatsThis::showText(toolTipXY,
                                   text);
            }
        }
    }
    else {
        if (text.isEmpty()) {
            QToolTip::hideText();
        }
        else {
            /*
             * When an action has not been given a tooltip,
             * calling QAction::toolTip() returns QAction::text().
             * In this case, do not show tooltip since it
             * is useful (same as the menu text)
             */
            if (action->text() == action->toolTip()) {
                QToolTip::hideText();
                return;
            }
            else {
                /*
                 * If tooltip is not formatted, word wrap the text
                 */
                if ( ! text.toLower().contains("<html>")) {
                    text = ("<html><body>" + text + "</body></html>");
                }
                
                /*
                 * Show tooltip containing scene description below mouse
                 * (note: positive Y is down in Qt coords)
                 */
                const QPoint toolTipXY(QCursor::pos()
                                       + QPoint(0, 15));
                QToolTip::showText(toolTipXY,
                                   text);
            }
        }
    }
}

/**
 * Called to hide tool tip when menu is closing.
 */
void
WuQToolTipHelper::hideToolTip()
{
    if (m_useWhatsThisFlag) {
        QWhatsThis::hideText();
    }
    else {
        QToolTip::hideText();
    }
}

bool
WuQToolTipHelper::event(QEvent* event)
{
    //std::cout << "Event: " << (int)event->type() << std::endl << std::flush;
    return QObject::event(event);
}
