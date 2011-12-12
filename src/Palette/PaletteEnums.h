#ifndef __PALETTE_ENUMS_H
#define __PALETTE_ENUMS_H

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
#include <AString.h>
#include "CaretAssert.h"

namespace caret {
/**
 * Palette Scale Mode.
 */
class PaletteScaleModeEnum {

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


    ~PaletteScaleModeEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGuiName(Enum e);
    
    static Enum fromGuiName(const AString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);
    
    static void getAllEnums(std::vector<Enum>& enumsOut);
    
private:
    PaletteScaleModeEnum(const Enum e, const int32_t integerCode, const AString& name, const AString& guiName);

    static const PaletteScaleModeEnum* findData(const Enum e);
    
    static std::vector<PaletteScaleModeEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;
    
    AString guiName;

};

/**
 * Palette Threshold Test.
 */
class PaletteThresholdTestEnum {

public:
    /** Palette Threshold Test. */
    enum Enum {
        /** show data when value is outside the threshold values */
        THRESHOLD_TEST_SHOW_OUTSIDE,
        /** show data when value is inside the threshold values */
        THRESHOLD_TEST_SHOW_INSIDE
    };


    ~PaletteThresholdTestEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGuiName(Enum e);
    
    static Enum fromGuiName(const AString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);
    
private:
    PaletteThresholdTestEnum(const Enum e, const int32_t integerCode, const AString& name, const AString& guiName);

    static const PaletteThresholdTestEnum* findData(const Enum e);

    static std::vector<PaletteThresholdTestEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;
    
    AString guiName;
};

/**
 * Palette Threshold Type.
 */
class PaletteThresholdTypeEnum {

public:
    /**  Palette Threshold Type. */
    enum Enum {
        /** thresholding is off */
        THRESHOLD_TYPE_OFF,
        /** normal thresholding */
        THRESHOLD_TYPE_NORMAL,
        /** threshold from mapping of volume */
        THRESHOLD_TYPE_MAPPED,
        /** threshold from mapping to PALS average area */
        THRESHOLD_TYPE_MAPPED_AVERAGE_AREA
    };


    ~PaletteThresholdTypeEnum();

    static AString toName(Enum e);
    
    static Enum fromName(const AString& s, bool* isValidOut);
    
    static AString toGuiName(Enum e);
    
    static Enum fromGuiName(const AString& s, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum e);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& enumsOut);
    
private:
    PaletteThresholdTypeEnum(const Enum e, const int32_t integerCode, const AString& name, const AString& guiName);

    static const PaletteThresholdTypeEnum* findData(const Enum e);
    
    static std::vector<PaletteThresholdTypeEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;
    
    AString guiName;
};

#ifdef __PALETTE_ENUMS_DECLARE__
    std::vector<PaletteScaleModeEnum> PaletteScaleModeEnum::enumData;
    bool PaletteScaleModeEnum::initializedFlag = false;
    std::vector<PaletteThresholdTestEnum> PaletteThresholdTestEnum::enumData;
    bool PaletteThresholdTestEnum::initializedFlag = false;
    std::vector<PaletteThresholdTypeEnum> PaletteThresholdTypeEnum::enumData;
    bool PaletteThresholdTypeEnum::initializedFlag = false;
#endif // __PALETTE_ENUMS_DECLARE__

} // namespace

#endif // __PALETTE_ENUMS_H
