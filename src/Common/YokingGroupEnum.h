#ifndef __YOKING_GROUP_ENUM_H__
#define __YOKING_GROUP_ENUM_H__

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

class YokingGroupEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** Off */
        YOKING_GROUP_OFF,
        /** Group A */
        YOKING_GROUP_A,
        /** Group B */
        YOKING_GROUP_B,
        /** Group C */
        YOKING_GROUP_C,
        /** Group D */
        YOKING_GROUP_D,
        /** Group E */
        YOKING_GROUP_E,
        /** Group F */
        YOKING_GROUP_F,
        /** Group G */
        YOKING_GROUP_G,
        /** Group H */
        YOKING_GROUP_H,
        /** Group I */
        YOKING_GROUP_I,
        /** Group J */
        YOKING_GROUP_J,
        /** Group K */
        YOKING_GROUP_K,
        /** Group L */
        YOKING_GROUP_L,
        /** Group  M*/
        YOKING_GROUP_M,
        /** Group N */
        YOKING_GROUP_N,
        /** Group O */
        YOKING_GROUP_O,
        /** Group P */
        YOKING_GROUP_P,
        /** Group Q */
        YOKING_GROUP_Q,
        /** Group R */
        YOKING_GROUP_R,
        /** Group S */
        YOKING_GROUP_S,
        /** Group T */
        YOKING_GROUP_T,
        /** Group U */
        YOKING_GROUP_U,
        /** Group V */
        YOKING_GROUP_V,
        /** Group W */
        YOKING_GROUP_W,
        /** Group X */
        YOKING_GROUP_X,
        /** Group Y */
        YOKING_GROUP_Y,
        /** Group Z */
        YOKING_GROUP_Z
    };


    ~YokingGroupEnum();

    static AString toName(Enum enumValue);
    
    static Enum fromName(const AString& name, bool* isValidOut);
    
    static AString toGuiName(Enum enumValue);
    
    static Enum fromGuiName(const AString& guiName, bool* isValidOut);
    
    static int32_t toIntegerCode(Enum enumValue);
    
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

    static void getAllEnums(std::vector<Enum>& allEnums);

    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);

    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);

private:
    YokingGroupEnum(const Enum enumValue, 
                 const AString& name,
                 const AString& guiName);

    static const YokingGroupEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<YokingGroupEnum> enumData;

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
};

#ifdef __YOKING_GROUP_ENUM_DECLARE__
std::vector<YokingGroupEnum> YokingGroupEnum::enumData;
bool YokingGroupEnum::initializedFlag = false;
int32_t YokingGroupEnum::integerCodeCounter = 0; 
#endif // __YOKING_GROUP_ENUM_DECLARE__

} // namespace
#endif  //__YOKING_GROUP_ENUM_H__
