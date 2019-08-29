#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_H__

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



namespace caret {

    class AnnotationBrowserTab;
    class BrainBrowserWindowToolBar;
    
    class BrainBrowserWindowToolBarTabPopUpMenu : public QMenu {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarTabPopUpMenu(BrainBrowserWindowToolBar* toolBar,
                                              const int32_t tabIndexUnderMouse,
                                              QWidget* parent = 0);
        
        virtual ~BrainBrowserWindowToolBarTabPopUpMenu();
        

        // ADD_NEW_METHODS_HERE
        
    private slots:
        void menuItemSelected(QAction* action);
        
    private:
        enum class MenuItem {
            NONE,
            MANUAL_LAYOUT_SELECT_FOR_EDITING,
            MANUAL_LAYOUT_SET_VISIBLE,
            CREATE_NEW_TAB_BEFORE,
            CREATE_NEW_TAB_AFTER,
            DUPLICATE_TAB_AT_BEGINNING,
            DUPLICATE_TAB_BEFORE,
            DUPLICATE_TAB_AFTER,
            DUPLICATE_TAB_AT_END,
            MOVE_TAB_TO_BEGINNING,
            MOVE_TAB_BEFORE,
            MOVE_TAB_AFTER,
            MOVE_TAB_TO_END,
            DELETE_TAB
        };
        
        BrainBrowserWindowToolBarTabPopUpMenu(const BrainBrowserWindowToolBarTabPopUpMenu&);

        BrainBrowserWindowToolBarTabPopUpMenu& operator=(const BrainBrowserWindowToolBarTabPopUpMenu&);
        
        void addItem(const MenuItem menuItem,
                     const QString& overrideMenuItemText = "");
        
        bool isEnabled(const MenuItem menuItem) const;
        
        BrainBrowserWindowToolBar* m_toolBar;
        
        int32_t m_tabIndexUnderMouse;
        
        int32_t m_numberOfTabs;
        
        int32_t m_activeTabIndex;
        
        int32_t m_browserTabContentIndex = -1;
        
        int32_t m_browserWindowIndex = -1;
        
        AnnotationBrowserTab* m_selectedBrowserTabAnnotation = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_TAB_POP_UP_MENU_H__
