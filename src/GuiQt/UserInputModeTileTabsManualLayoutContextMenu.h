#ifndef __USER_INPUT_MODE_TILE_TABS_MANUAL_LAYOUT_CONTEXT_MENU_H__
#define __USER_INPUT_MODE_TILE_TABS_MANUAL_LAYOUT_CONTEXT_MENU_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include <QMenu>

#include "AnnotationGroupingModeEnum.h"
#include "EventBrowserWindowTileTabOperation.h"
#include "MouseEvent.h"

namespace caret {

    class AnnotationBrowserTab;
    class BrainOpenGLWidget;
    class BrowserTabContent;
    class UserInputModeTileTabsManualLayout;

    class UserInputModeTileTabsManualLayoutContextMenu : public QMenu {
        
        Q_OBJECT

    public:
        UserInputModeTileTabsManualLayoutContextMenu(UserInputModeTileTabsManualLayout* userInputTileTabsManualLayout,
                                            const MouseEvent& mouseEvent,
                                            BrowserTabContent* browserTabContent,
                                            BrainOpenGLWidget* parentOpenGLWidget);
        
        virtual ~UserInputModeTileTabsManualLayoutContextMenu();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void deselectAllAnnotations();
        
        void selectAllAnnotations();
        
        void applyGroupingGroup();
        
        void applyGroupingRegroup();
        
        void applyGroupingUngroup();
        
        void applyOrderBringToFront();
        
        void applyOrderBringForward();
        
        void applyOrderSendToBack();
        
        void applyOrderSendBackward();
        
        void processExpandTabMenuItem();
        
        void processInsertNewTabMenuItem();
        
    private:
        UserInputModeTileTabsManualLayoutContextMenu(const UserInputModeTileTabsManualLayoutContextMenu&);

        UserInputModeTileTabsManualLayoutContextMenu& operator=(const UserInputModeTileTabsManualLayoutContextMenu&);
        
        void applyGrouping(const AnnotationGroupingModeEnum::Enum grouping);
        
        void processWindowTileTabOperation(const EventBrowserWindowTileTabOperation::Operation operation);
        
        UserInputModeTileTabsManualLayout* m_userInputTileTabsManualLayout = NULL;
        
        /*
         * NOT a reference.  Need to COPY as its source may be deleted.
         */
        const MouseEvent m_mouseEvent;
        
        BrowserTabContent* m_browserTabContent;
        
        BrainOpenGLWidget* m_parentOpenGLWidget;
        
        std::vector<AnnotationBrowserTab*> m_selectedBrowserTabAnnotations;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_TILE_TABS_MANUAL_LAYOUT_CONTEXT_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_TILE_TABS_MANUAL_LAYOUT_CONTEXT_MENU_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_TILE_TABS_MANUAL_LAYOUT_CONTEXT_MENU_H__
