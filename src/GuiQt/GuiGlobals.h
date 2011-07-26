
#ifndef __GUI_GLOBALS_H__
#define __GUI_GLOBALS_H__

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

namespace caret {

    class Actions;
    class Brain;
    class WindowMain;
    
    /**
     * Global variables.
     */
    class GuiGlobals {
        
    private:
        GuiGlobals() { }
        ~GuiGlobals() { }
        GuiGlobals(const GuiGlobals&);
        GuiGlobals& operator=(const GuiGlobals&);
        
    public:
        static Actions* getActions();
        
        static Brain* getBrain();
        
        static WindowMain* getMainWindow();
        
        static void setMainWindow(WindowMain* mainWindow);
        
    private:
        static Brain* brain;
        static WindowMain* mainWindow;
        static Actions* actions;
    };
    
#ifdef __GUI_GLOBALS_DEFINE__
    Actions* GuiGlobals::actions = NULL;
    Brain* GuiGlobals::brain = 0;
    WindowMain* GuiGlobals::mainWindow = 0;
#endif __GUI_GLOBALS_DEFINE__
    
} // namespace

#endif // __GUI_GLOBALS_H__
