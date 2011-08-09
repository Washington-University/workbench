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
#include "CaretWindowEnum.h"
#undef __CARET_WINDOW_DECLARE_H__

#include "CaretObject.h"


#include <stdint.h>

#include <QString>
#include <vector> 

using namespace caret;
    
/**
 * Get the enum for this caret window.
 *
 * @return Enum for this caret window.
 */
CaretWindowEnum::Enum 
CaretWindowEnum::getEnum() const
{
    return this->e;
}

/**
 * Get the enum name for this caret window.
 *
 * @return Enum name for this caret window.
 */
QString 
CaretWindowEnum::getName() const
{
    return this->name;
}

/**
 * Get the window name for this caret window.
 *
 * @return window name for this caret window.
 */
QString 
CaretWindowEnum::getWindowName() const
{
    return this->windowName;
}

/**
 * Get the index for this caret window.
 *
 * @return index for this caret window.
 */
int32_t 
CaretWindowEnum::getWindowIndex() const
{
    return this->windowIndex;
}

/**
 * Is this all windows?
 * @return  true if all windows, else false.
 *
 */
bool 
CaretWindowEnum::isAllWindows() const
{
    return (this->e == WINDOW_ALL);
}

/**
 * Is this main windoww?
 * @return  true if main window, else false.
 *
 */
bool 
CaretWindowEnum::isMainWindow() const
{
    return (this->e == WINDOW_MAIN);
}

/**
 * Get a vector containing each of Caret's windows.
 *
 * @return  
 *    A vector containing each of caret's windows.
 */
std::vector<CaretWindowEnum::Enum> 
CaretWindowEnum::getWindows()
{
    CaretWindowEnum::initialize();
    std::vector<CaretWindowEnum::Enum> windowsOut;
    
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
std::vector<CaretWindowEnum::Enum> 
CaretWindowEnum::getWindowsExcludingAllWindows()
{
    CaretWindowEnum::initialize();
    std::vector<CaretWindowEnum::Enum> windowsOut;
    
    for (uint64_t i = 0; i < enumData.size(); i++) {
        CaretWindowEnum::Enum e = enumData[i].getEnum();
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
CaretWindowEnum*
CaretWindowEnum::indexToWindow(const int32_t indx)
{
    CaretWindowEnum::initialize();
    for (uint64_t i = 0; i < enumData.size(); i++) {
        CaretWindowEnum* cw = &enumData[i];
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
QString 
CaretWindowEnum::toName(Enum e)
{
    CaretWindowEnum::initialize();
    
    QString s;
    
    for (std::vector<CaretWindowEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretWindowEnum& d = *iter;
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
CaretWindowEnum::Enum 
CaretWindowEnum::fromName(const QString& name, bool* isValidOut)
{
    CaretWindowEnum::initialize();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<CaretWindowEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretWindowEnum& d = *iter;
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
CaretWindowEnum* 
CaretWindowEnum::findData(const Enum e)
{
    CaretWindowEnum::initialize();

    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        CaretWindowEnum* d = &enumData[i];
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
CaretWindowEnum::CaretWindowEnum(const Enum e,
            const QString& name,
            const QString& windowName,
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
CaretWindowEnum::initialize()
{
    if (CaretWindowEnum::initializedFlag) {
        return;
    }
    CaretWindowEnum::initializedFlag = true;
    
    CaretWindowEnum::enumData.push_back(CaretWindowEnum(WINDOW_MAIN, "WINDOW_MAIN", "Main Window", 0));
    CaretWindowEnum::enumData.push_back(CaretWindowEnum(WINDOW_ALL, "WINDOW_ALL", "All Windows", 0));
    CaretWindowEnum::enumData.push_back(CaretWindowEnum(WINDOW_VIEW_2, "WINDOW_VIEW_2", "Viewing Window 2", 1));
    CaretWindowEnum::enumData.push_back(CaretWindowEnum(WINDOW_VIEW_3, "WINDOW_VIEW_3", "Viewing Window 3", 2));
    CaretWindowEnum::enumData.push_back(CaretWindowEnum(WINDOW_VIEW_4, "WINDOW_VIEW_4", "Viewing Window 4", 3));
    CaretWindowEnum::enumData.push_back(CaretWindowEnum(WINDOW_VIEW_5, "WINDOW_VIEW_5", "Viewing Window 5", 4));
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
CaretWindowEnum::Enum 
CaretWindowEnum::fromIndex(const int32_t windowIndex, bool* isValidOut)
{
    CaretWindowEnum::initialize();
    assert((windowIndex >= 0) && (windowIndex < static_cast<int>(CaretWindowEnum::enumData.size())));
    
    Enum e = WINDOW_MAIN;
    if ((windowIndex >= 0) && (windowIndex < static_cast<int>(CaretWindowEnum::enumData.size()))) {
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
CaretWindowEnum::toIndex(const Enum e)
{
    CaretWindowEnum::initialize();
    const CaretWindowEnum* cw = CaretWindowEnum::findData(e);
    assert(cw);
    return cw->getWindowIndex();
}












