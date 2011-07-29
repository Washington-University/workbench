#ifndef __CARETWINDOW_H__
#define __CARETWINDOW_H__

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


#include "CaretObject.h"


#include <stdint.h>

#include <string>
#include <vector> 

namespace caret {

/**
 * Identifies each of Caret5's windows.
 * 
 * This is a "class" and not an "enum" because the
 * values method must not return the all windows
 * value.
 */
class CaretWindowEnum : public CaretObject {

public:
    enum Enum {
        /** ALL windows */
        WINDOW_ALL  = 0,  /* YES WINDOW_MAIN and WINDOW_ALL are both zero */
        /**The Main Window */
        WINDOW_MAIN = 0,
        /**The second viewing window. */
        WINDOW_VIEW_2 = 1,
        /**The third viewing window. */
        WINDOW_VIEW_3 = 2,
        /**The fourth viewing window. */
        WINDOW_VIEW_4 = 3,
        /**The fifth viewing window. */
        WINDOW_VIEW_5 = 4
    };
    
    enum Constants {
        NUMBER_OF_WINDOWS = 5
    };
    
    virtual ~CaretWindowEnum() { }

    bool isAllWindows() const;
    
    bool isMainWindow() const;
    
    static std::vector<CaretWindowEnum::Enum> getWindows();
    
    static std::vector<CaretWindowEnum::Enum> getWindowsExcludingAllWindows();
    
    static CaretWindowEnum::Enum fromIndex(const int32_t index, bool* isValidOut);
    
    static int32_t toIndex(const Enum e);
    
    static std::string toName(Enum e);
    
    static Enum fromName(const std::string& s, bool* isValidOut);
    
    
private:
    CaretWindowEnum(const Enum e,
                const std::string& name,
                const std::string& windowName,
                const int32_t windowIndex);
    

    Enum getEnum() const;
    
    std::string getName() const;
    
    std::string getWindowName() const;
    
    int32_t getWindowIndex() const;
    
    static std::vector<CaretWindowEnum> enumData;
    
    static void initialize();
    
    static bool initializedFlag;
    
    static CaretWindowEnum* indexToWindow(const int32_t index);
    
    static CaretWindowEnum* findData(const Enum e);
    
    Enum e;
    
    /** Enum name */
    std::string name;
    
    /** Name of window. */
    std::string windowName;

    /**window's index for arrays. */
    int32_t windowIndex;

};

#ifdef __CARET_WINDOW_DECLARE_H__
    std::vector<CaretWindowEnum> CaretWindowEnum::enumData;
    bool CaretWindowEnum::initializedFlag = false;
#endif // __CARET_WINDOW_DECLARE_H__
} // namespace

#endif // __CARETWINDOW_H__
