#ifndef __MAC_DOCK_MENU_H__
#define __MAC_DOCK_MENU_H__

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



namespace caret {

    class MacDockMenu : public QMenu {
        
        Q_OBJECT

    public:
        MacDockMenu();
        
        virtual ~MacDockMenu();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void browserWindowActionSelected(const int32_t browserWindowIndex);
        
        void menuAboutToShow();
        
        void startNewWbViewMenuItemTriggered();
        
    private:
        MacDockMenu(const MacDockMenu&);

        MacDockMenu& operator=(const MacDockMenu&);
        
        void addBrowserWindowActions();
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MAC_DOCK_MENU_DECLARE__
#endif // __MAC_DOCK_MENU_DECLARE__

} // namespace
#endif  //__MAC_DOCK_MENU_H__
