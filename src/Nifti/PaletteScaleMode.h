#ifndef __PALETTESCALEMODE_H
#define __PALETTESCALEMODE_H

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


#include <stdint.h>
#include <vector>
#include <string>

namespace caret {

/**
 * Palette Scale Mode.
 */
class PaletteScaleMode {

public:
    /**  Palette Scale Mode. */
    enum Enum {
        /** Auto Scale   */
        MODE_AUTO_SCALE,
        /** Auto Scale Percentage   */
        MODE_AUTO_SCALE_PERCENTAGE,
        /** User Scale */
        MODE_USER_SCALE
    };


    ~PaletteScaleMode();

    Enum getEnum() const;

    int32_t getIntegerCode() const;

    std::string getName() const;

    std::string getGuiName() const;
    
    static std::string toString(Enum e, bool* isValidOut);
    
    static Enum fromString(const std::string& s, bool* isValidOut);
    
    static const PaletteScaleMode* findData(const Enum e);
    
private:
    PaletteScaleMode(const Enum e, const int32_t integerCode, const std::string& name, const std::string& guiName);

    static std::vector<PaletteScaleMode> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    std::string name;
    
    std::string guiName;

};

#ifdef __PALETTE_SCALE_DECLARE__
    std::vector<PaletteScaleMode> PaletteScaleMode::enumData;
    bool PaletteScaleMode::initializedFlag = false;
#endif // __PALETTE_SCALE_DECLARE__

} // namespace

#endif // __PALETTESCALEMODE_H
