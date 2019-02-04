#ifndef __WB_MACRO_CUSTOM_DATA_TYPE_ENUM_H__
#define __WB_MACRO_CUSTOM_DATA_TYPE_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

class WbMacroCustomDataTypeEnum {

public:
    /**
     * Enumerated values.
     */
    enum Enum {
        /** */
        SURFACE
    };


    ~WbMacroCustomDataTypeEnum();

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
    WbMacroCustomDataTypeEnum(const Enum enumValue,
                 const AString& name,
                 const AString& guiName);

    static const WbMacroCustomDataTypeEnum* findData(const Enum enumValue);

    /** Holds all instance of enum values and associated metadata */
    static std::vector<WbMacroCustomDataTypeEnum> enumData;

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

#ifdef __WB_MACRO_CUSTOM_DATA_TYPE_ENUM_DECLARE__
std::vector<WbMacroCustomDataTypeEnum> WbMacroCustomDataTypeEnum::enumData;
bool WbMacroCustomDataTypeEnum::initializedFlag = false;
int32_t WbMacroCustomDataTypeEnum::integerCodeCounter = 0;
#endif // __WB_MACRO_CUSTOM_DATA_TYPE_ENUM_DECLARE__

} // namespace
#endif  //__WB_MACRO_CUSTOM_DATA_TYPE_ENUM_H__
