#ifndef __USER_INPUT_TILE_TABS_CONTEXT_MENU_H__
#define __USER_INPUT_TILE_TABS_CONTEXT_MENU_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#include <QMenu>

#include "EventBrowserWindowTileTabOperation.h"

class QWidget;

namespace caret {

    class BrainOpenGLViewportContent;
    class MouseEvent;
    
    class UserInputTileTabsContextMenu : public QMenu {
        
        Q_OBJECT

    public:
        UserInputTileTabsContextMenu(const MouseEvent& mouseEvent,
                                     QWidget* parentWidget,
                                     BrainOpenGLViewportContent* viewportContent);
        
        virtual ~UserInputTileTabsContextMenu();

        bool isValid() const;

        // ADD_NEW_METHODS_HERE

    private slots:
        void actionTriggered(QAction* action);
        
    private:
        UserInputTileTabsContextMenu(const UserInputTileTabsContextMenu&);

        UserInputTileTabsContextMenu& operator=(const UserInputTileTabsContextMenu&);
        
        void addItemToMenu(const QString& text,
                           const EventBrowserWindowTileTabOperation::Operation operation);
        
        QWidget* m_parentWidget;
        
        const int32_t m_windowIndex;
        
        const int32_t m_tabIndex;

        int32_t m_windowViewport[4];
        
        int32_t m_mouseWindowX = 0;
        
        int32_t m_mouseWindowY = 0;
        
        QAction* m_createNewGridTabBeforeAction = nullptr;
        
        QAction* m_createNewGridTabAfterAction = nullptr;
        
        QAction* m_createNewManualTabAction = nullptr;
        
        QAction* m_orderBringToFrontAction = nullptr;
        
        QAction* m_orderBringForwardAction = nullptr;
        
        QAction* m_orderSendToBackAction = nullptr;
        
        QAction* m_orderSendBackwardAction = nullptr;
        
        QAction* m_selectTabAction = nullptr;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_TILE_TABS_CONTEXT_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_TILE_TABS_CONTEXT_MENU_DECLARE__

} // namespace
#endif  //__USER_INPUT_TILE_TABS_CONTEXT_MENU_H__
