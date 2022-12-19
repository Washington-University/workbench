#ifndef __RECENT_SCENE_MENU_H__
#define __RECENT_SCENE_MENU_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

    class BrainBrowserWindow;
    
    class RecentSceneMenu : public QMenu {
        
        Q_OBJECT

    public:
        RecentSceneMenu(BrainBrowserWindow* brainBrowserWindow,
                        QWidget* parentWidget = 0);
        
        virtual ~RecentSceneMenu();
        
        RecentSceneMenu(const RecentSceneMenu&) = delete;

        RecentSceneMenu& operator=(const RecentSceneMenu&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void menuTriggered(QAction* action);
        
        void menuAboutToShow();
        
    private:
        BrainBrowserWindow* m_brainBrowserWindow;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_SCENE_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECENT_SCENE_MENU_DECLARE__

} // namespace
#endif  //__RECENT_SCENE_MENU_H__
