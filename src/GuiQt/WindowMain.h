
#ifndef __WINDOW_MAIN_H__
#define __WINDOW_MAIN_H__

/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <QMainWindow>

namespace caret {
    
    class Actions;
    class BrainOpenGLWidget;
    
    class WindowMain : public QMainWindow {
        Q_OBJECT
        
    public:
        WindowMain(const int openGLSizeX,
                   const int openGLSizeY);
        
        ~WindowMain();
        
    protected:
        void closeEvent(QCloseEvent* event);
        
    private:
        void createMenus();
        
        QMenu* createMenuFile();
        QMenu* createMenuData();
        QMenu* createMenuSurface();
        QMenu* createMenuVolume();
        QMenu* createMenuWindow();
        QMenu* createMenuHelp();
        
        Actions* actions;
        BrainOpenGLWidget* openGLWidget;
    };
    
}  // namespace

#endif // __WINDOW_MAIN_H__