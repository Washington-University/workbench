#ifndef __CARET_COLOR_ENUM__H_
#define __CARET_COLOR_ENUM__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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


#include <stdint.h>
#include <vector>
#include "AString.h"

namespace caret {

class CaretColorEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** No coloring */
        NONE,
        /** Custom coloring */
        CUSTOM,
        /** AQUA */
        AQUA,
        /** Black */
        BLACK,
        /** Blue */
        BLUE,
        /** Fuchsia */
        FUCHSIA,
        /** Gray */
        GRAY,
        /** Green */
        GREEN,
        /** Lime */
        LIME,
        /** Maroon */
        MAROON,
        /** Navy Blue */
        NAVY,
        /** Olive */
        OLIVE,
        /** Purple */
        PURPLE,
        /** Red */
        RED,
        /** Silver */
        SILVER,
        /** Teal */
        TEAL,
        /** White */
        WHITE,
        /** Yellow */
        YELLOW
    };

    enum ColorOptions {
        OPTION_NO_OPTIONS = 0,
        OPTION_INCLUDE_CUSTOM_COLOR = 1,
        OPTION_INCLUDE_NONE_COLOR = 2
    };
    
    ~CaretColorEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allColorEnums);

    static void getColorEnums(std::vector<Enum>& allColorEnums);
    
    static void getColorEnumsNoBlackOrWhite(std::vector<Enum>& allColorEnums);
    
    static void getColorAndOptionalEnums(std::vector<Enum>& allEnums,
                                         const int64_t colorOptions);
    
//    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);
//
//    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

    static const float* toRGBA(Enum enumValue);
    
    static void toRGBAFloat(Enum enumValue,
                            float rgbaOut[4]);
    
    static void toRGBAByte(Enum enumValue,
                           uint8_t rgbaOut[4]);
    
private:
    CaretColorEnum(const Enum enumValue, 
                   const AString& name,
                   const AString& guiName,
                   const float red,
                   const float green,
                   const float blue);

    static const CaretColorEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<CaretColorEnum> enumData;

    /** Initialize instances that contain the enum values and metadata */
    static void initialize();

    /** Indicates instance of enum values and metadata have been initialized */
    static bool initializedFlag;
    
    /** Auto generated integer codes */
    static int32_t integerCodeCounter;
    
    /** The enumerated type value for an instance */
    Enum enumValue;

    /** The integer code associated with an enumerated value */
    int32_t integerCode;

    /** The name, a text string that is identical to the enumerated value */
    AString name;
    
    /** A user-friendly name that is displayed in the GUI */
    AString guiName;
    
    /** RGB color components */
    float rgba[4];
};

#ifdef __CARET_COLOR_ENUM_DECLARE__
std::vector<CaretColorEnum> CaretColorEnum::enumData;
bool CaretColorEnum::initializedFlag = false;
int32_t CaretColorEnum::integerCodeCounter = 0; 
#endif // __CARET_COLOR_ENUM_DECLARE__

} // namespace
#endif  //__CARET_COLOR_ENUM__H_
