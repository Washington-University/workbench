#ifndef __GENERAL_YOKING_GROUP_ENUM_H__
#define __GENERAL_YOKING_GROUP_ENUM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

    class GeneralYokingGroupEnum {
        
    public:
        /**
         * Enumerated values.
         */
        enum Enum {
            /** Off */
            OFF,
            /** Group One */
            ONE,
            /** Group Two */
            TWO,
            /** Group Three */
            THREE,
            /** Group Four */
            FOUR,
            /** Group Five */
            FIVE,
            /** Group Six */
            SIX,
            /** Group Seven */
            SEVEN,
            /** Group Eight */
            EIGHT,
            /** Group Nine */
            NINE,
            /** Group Ten */
            TEN
        };
        
        
        ~GeneralYokingGroupEnum();
        
        static AString toName(Enum enumValue);
        
        static Enum fromName(const AString& name, bool* isValidOut);
        
        static AString toArabicNumeralText(Enum enumValue);
        
        static AString toLowerCaseRomanNumeralText(Enum enumValue);
        
        static AString toUpperCaseRomanNumeralText(Enum enumValue);
        
        static AString tolowerCaseAlphabeticalText(Enum enumValue);
        
        static AString toUpperCaseAlphabeticalText(Enum enumValue);
        
        static int32_t toIntegerCode(Enum enumValue);
        
        static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);
        
        static void getAllEnums(std::vector<Enum>& allEnums);
        
        static void getAllNames(std::vector<AString>& allNames, const bool isSorted);
        
    private:
        GeneralYokingGroupEnum(const Enum enumValue,
                               const AString& name,
                               const AString& arabicNumeralText,
                               const AString& lowerCaseRomanNumeralText,
                               const AString& upperCaseRomanNumeralText,
                               const AString& lowerCaseAlphabeticalText,
                               const AString& upperCaseAlphabeticalText);
        
        static const GeneralYokingGroupEnum* findData(const Enum enumValue);
        
        /** Holds all instance of enum values and associated metadata */
        static std::vector<GeneralYokingGroupEnum> enumData;
        
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
        
        AString arabicNumeralText;
        AString lowerCaseRomanNumeralText;
        AString upperCaseRomanNumeralText;
        AString lowerCaseAlphabeticalText;
        AString upperCaseAlphabeticalText;
    };
#ifdef __GENERAL_YOKING_GROUP_ENUM_DECLARE__
std::vector<GeneralYokingGroupEnum> GeneralYokingGroupEnum::enumData;
bool GeneralYokingGroupEnum::initializedFlag = false;
int32_t GeneralYokingGroupEnum::integerCodeCounter = 0; 
#endif // __GENERAL_YOKING_GROUP_ENUM_DECLARE__

} // namespace
#endif  //__GENERAL_YOKING_GROUP_ENUM_H__
