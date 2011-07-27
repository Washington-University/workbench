/*LICENSE_START*/ 
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

#include <cassert>

#define __CARET_WINDOW_DECLARE_H__
#include "CaretWindow.h"
#undef __CARET_WINDOW_DECLARE_H__

#include "CaretObject.h"


#include <stdint.h>

#include <string>
#include <vector> 

using namespace caret;
    
/**
 * Get the enum for this caret window.
 *
 * @return Enum for this caret window.
 */
CaretWindow::Enum 
CaretWindow::getEnum() const
{
    return this->e;
}

/**
 * Get the enum name for this caret window.
 *
 * @return Enum name for this caret window.
 */
std::string 
CaretWindow::getName() const
{
    return this->name;
}

/**
 * Get the window name for this caret window.
 *
 * @return window name for this caret window.
 */
std::string 
CaretWindow::getWindowName() const
{
    return this->windowName;
}

/**
 * Get the index for this caret window.
 *
 * @return index for this caret window.
 */
int32_t 
CaretWindow::getWindowIndex() const
{
    return this->windowIndex;
}

/**
 * Is this all windows?
 * @return  true if all windows, else false.
 *
 */
bool 
CaretWindow::isAllWindows() const
{
    return (this->e == WINDOW_ALL);
}

/**
 * Is this main windoww?
 * @return  true if main window, else false.
 *
 */
bool 
CaretWindow::isMainWindow() const
{
    return (this->e == WINDOW_MAIN);
}

/**
 * Get a vector containing each of Caret's windows.
 *
 * @return  
 *    A vector containing each of caret's windows.
 */
std::vector<CaretWindow::Enum> 
CaretWindow::getWindows()
{
    CaretWindow::initialize();
    std::vector<CaretWindow::Enum> windowsOut;
    
    for (uint64_t i = 0; i < enumData.size(); i++) {
        windowsOut.push_back(enumData[i].getEnum());
    }
    
    return windowsOut;    
}

/**
 * Get a vector containing each of Caret's windows
 * except all windows.
 *
 * @return  
 *    A vector containing each of caret's windows
 *    except all windows.
 */
std::vector<CaretWindow::Enum> 
CaretWindow::getWindowsExcludingAllWindows()
{
    CaretWindow::initialize();
    std::vector<CaretWindow::Enum> windowsOut;
    
    for (uint64_t i = 0; i < enumData.size(); i++) {
        CaretWindow::Enum e = enumData[i].getEnum();
        if (e != WINDOW_ALL) {
            windowsOut.push_back(e);
        }
    }
    
    return windowsOut;
}

/**
 * Convert a window index into its corresponding
 * CaretWindow.
 * 
 * @param indx 
 *     Index of window.
 * @return  
 *     Window corresponding to index.
 *
 */
CaretWindow*
CaretWindow::indexToWindow(const int32_t indx)
{
    CaretWindow::initialize();
    for (uint64_t i = 0; i < enumData.size(); i++) {
        CaretWindow* cw = &enumData[i];
        if (cw->getWindowIndex() == indx) {
            return cw;
        }
    }
    return NULL;
}

/**
 * Convert an enum to its enum name.
 *
 * @param e
 *    The caret window enum.
 * @return
 *    The name of the enum.
 */
std::string 
CaretWindow::toName(Enum e)
{
    CaretWindow::initialize();
    
    std::string s;
    
    for (std::vector<CaretWindow>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretWindow& d = *iter;
        if (d.e == e) {
            s = d.name;
            break;
        }
    }
    
    assert(s.length() > 0);
    return s;
}

/**
 * Convert the enum or window name to its
 * corresponding enum.
 *
 * @param name
 *    Name of enum.
 * @param isValidOut
 *    If not NULL, it will indicate that a corresponding
 *    enum was found for the name.
 * @return
 *    Enum corresponding to name.
 */
CaretWindow::Enum 
CaretWindow::fromName(const std::string& name, bool* isValidOut)
{
    CaretWindow::initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<CaretWindow>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretWindow& d = *iter;
        if ((d.name == name) || (d.windowName == name)) {
            e = d.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
    
}

/**
 * Given an enum, find its corresponding data.
 *
 * @param e
 *    The enum.
 *
 * @return
 *    CaretWindow corresponding to e or NULL if no match.
 */
CaretWindow* 
CaretWindow::findData(const Enum e)
{
    CaretWindow::initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        CaretWindow* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }
    
    return NULL;
}

/**
 * Constructor.
 *
 * @param e
 *     The enum.
 * @param enumName
 *     Enum name.
 * @param windowName
 *     Window name.
 * @param windowIndex
 *     Index of window.
 */
CaretWindow::CaretWindow(const Enum e,
            const std::string& name,
            const std::string& windowName,
            const int32_t windowIndex)
{
    this->e = e;
    this->name = name;
    this->windowName = windowName;
    this->windowIndex = windowIndex;
}
 
/**
 * Initialize the ENUM data.
 */
void 
CaretWindow::initialize()
{
    if (CaretWindow::initializedFlag) {
        return;
    }
    CaretWindow::initializedFlag = true;
    
    CaretWindow::enumData.push_back(CaretWindow(WINDOW_MAIN, "WINDOW_MAIN", "Main Window", 0));
    CaretWindow::enumData.push_back(CaretWindow(WINDOW_ALL, "WINDOW_ALL", "All Windows", 0));
    CaretWindow::enumData.push_back(CaretWindow(WINDOW_VIEW_2, "WINDOW_VIEW_2", "Viewing Window 2", 1));
    CaretWindow::enumData.push_back(CaretWindow(WINDOW_VIEW_3, "WINDOW_VIEW_3", "Viewing Window 3", 2));
    CaretWindow::enumData.push_back(CaretWindow(WINDOW_VIEW_4, "WINDOW_VIEW_4", "Viewing Window 4", 3));
    CaretWindow::enumData.push_back(CaretWindow(WINDOW_VIEW_5, "WINDOW_VIEW_5", "Viewing Window 5", 4));
}

/**
 * Get the Enum corresponding to the window index.
 *
 * @param windowIndex
 *    Index of window.
 * 
 * @return
 *    enum corresponding to window index..
 */
CaretWindow::Enum 
CaretWindow::fromIndex(const int32_t windowIndex, bool* isValidOut)
{
    CaretWindow::initialize();
    assert((windowIndex >= 0) && (windowIndex < static_cast<int>(CaretWindow::enumData.size())));
    
    Enum e = WINDOW_MAIN;
    if ((windowIndex >= 0) && (windowIndex < static_cast<int>(CaretWindow::enumData.size()))) {
        e = enumData[windowIndex].getEnum();
        if (isValidOut != NULL) {
            *isValidOut = true;
        }
    }
    else {
        if (isValidOut != NULL) {
            *isValidOut = false;
        }
    }
    return e;
}

/**
 * Get the index corresponding to the enum value.
 *
 * @param e
 *    The enum..
 * 
 * @return
 *    index corresponding to enum.
 */
int32_t 
CaretWindow::toIndex(const Enum e)
{
    CaretWindow::initialize();
    const CaretWindow* cw = CaretWindow::findData(e);
    assert(cw);
    return cw->getWindowIndex();
}












