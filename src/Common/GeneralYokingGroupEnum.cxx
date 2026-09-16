
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

#include <algorithm>
#define __GENERAL_YOKING_GROUP_ENUM_DECLARE__
#include "GeneralYokingGroupEnum.h"
#undef __GENERAL_YOKING_GROUP_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;


/**
 * \class caret::GeneralYokingGroupEnum
 * \brief Yoking group that supports different alphabetical/numerical systems in the GUI
 *
 *
 * Using this enumerated type in the GUI with GeneralYokingGroupComboBox
 */

/*
 switch (value) {
 case GeneralYokingGroupEnum::GROUP_ONE:
 break;
 case GeneralYokingGroupEnum::GROUP_TWO:
 break;
 case GeneralYokingGroupEnum::GROUP_THREE:
 break;
 case GeneralYokingGroupEnum::GROUP_FOUR:
 break;
 case GeneralYokingGroupEnum::GROUP_FIVE:
 break;
 case GeneralYokingGroupEnum::GROUP_SIX:
 break;
 case GeneralYokingGroupEnum::GROUP_SEVEN:
 break;
 case GeneralYokingGroupEnum::GROUP_EIGHT:
 break;
 case GeneralYokingGroupEnum::GROUP_NINE:
 break;
 case GeneralYokingGroupEnum::GROUP_TEN:
 break;
 }
 */

/**
 * Constructor.
 *
 * @param enumValue
 *    An enumerated value.
 * @param name
 *    Name of enumerated value.
 * @param guiName
 *    User-friendly name for use in user-interface.
 * @param arabicNumeralText
 *    Arabic numeral text
 * @param lowerCaseRomanNumeralText
 *    Lower case roman numeral text
 * @param upperCaseRomanNumeralText
 *    Upper case roman numeral text
 * @param lowerCaseAlphabeticalText
 *    Lower case alphabetical text
 * @param upperCaseAlphabeticalText
 *    Upper case alphabetical text
 */
GeneralYokingGroupEnum::GeneralYokingGroupEnum(const Enum enumValue,
                                               const AString& name,
                                               const AString& arabicNumeralText,
                                               const AString& lowerCaseRomanNumeralText,
                                               const AString& upperCaseRomanNumeralText,
                                               const AString& lowerCaseAlphabeticalText,
                                               const AString& upperCaseAlphabeticalText)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->arabicNumeralText = arabicNumeralText;
    this->lowerCaseRomanNumeralText = lowerCaseRomanNumeralText;
    this->upperCaseRomanNumeralText = upperCaseRomanNumeralText;
    this->lowerCaseAlphabeticalText = lowerCaseAlphabeticalText;
    this->upperCaseAlphabeticalText = upperCaseAlphabeticalText;
}

/**
 * Destructor.
 */
GeneralYokingGroupEnum::~GeneralYokingGroupEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
GeneralYokingGroupEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;
    
    enumData.push_back(GeneralYokingGroupEnum(OFF,
                                              "OFF",
                                              "Off",
                                              "Off",
                                              "Off",
                                              "Off",
                                              "Off"));
    
    enumData.push_back(GeneralYokingGroupEnum(ONE,
                                              "ONE",
                                              "1",
                                              "i",
                                              "I",
                                              "a",
                                              "A"));
    enumData.push_back(GeneralYokingGroupEnum(TWO,
                                              "TWO",
                                              "2",
                                              "ii",
                                              "II",
                                              "b",
                                              "B"));
    
    enumData.push_back(GeneralYokingGroupEnum(THREE,
                                              "THREE",
                                              "3",
                                              "iii",
                                              "III",
                                              "c",
                                              "C"));
    
    enumData.push_back(GeneralYokingGroupEnum(FOUR,
                                              "FOUR",
                                              "4",
                                              "iv",
                                              "IV",
                                              "d",
                                              "D"));
    
    enumData.push_back(GeneralYokingGroupEnum(FIVE,
                                              "FIVE",
                                              "5",
                                              "v",
                                              "V",
                                              "e",
                                              "E"));
    
    enumData.push_back(GeneralYokingGroupEnum(SIX,
                                              "SIX",
                                              "6",
                                              "vi",
                                              "VI",
                                              "f",
                                              "F"));
    
    enumData.push_back(GeneralYokingGroupEnum(SEVEN,
                                              "SEVEN",
                                              "7",
                                              "vii",
                                              "VII",
                                              "g",
                                              "G"));
    
    enumData.push_back(GeneralYokingGroupEnum(EIGHT,
                                              "EIGHT",
                                              "8",
                                              "viii",
                                              "VIII",
                                              "h",
                                              "H"));
    
    enumData.push_back(GeneralYokingGroupEnum(NINE,
                                              "NINE",
                                              "9",
                                              "ix",
                                              "IX",
                                              "i",
                                              "I"));
    
    enumData.push_back(GeneralYokingGroupEnum(TEN,
                                              "TEN",
                                              "10",
                                              "x",
                                              "X",
                                              "j",
                                              "J"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GeneralYokingGroupEnum*
GeneralYokingGroupEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    
    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GeneralYokingGroupEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }
    
    return NULL;
}

/**
 * @return
 *     Text for
 * @param enumValue
 *     Enumerated value.
 */
AString
GeneralYokingGroupEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}


/*
 static AString toArabicNumeralText(Enum enumValue);
 
 static AString toLowerCaseRomanNumeralText(Enum enumValue);
 
 static AString toUpperCaseRomanNumeralText(Enum enumValue);
 
 static AString tolowerCaseAlphabeticalText(Enum enumValue);
 
 static AString toUpperCaseAlphabeticalText(Enum enumValue);
 
 
 */
/**
 * Get an enumerated value corresponding to its name.
 * @param name
 *     Name of enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return
 *     Enumerated value.
 */
GeneralYokingGroupEnum::Enum
GeneralYokingGroupEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GeneralYokingGroupEnum::enumData[0].enumValue;
    
    for (std::vector<GeneralYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GeneralYokingGroupEnum& d = *iter;
        if (d.name == name) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type GeneralYokingGroupEnum"));
    }
    return enumValue;
}

/**
 * @return
 *     Arabic numeral text for the given enum value
 * @param enumValue
 *     Enumerated value.
 */
AString
GeneralYokingGroupEnum::toArabicNumeralText(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->arabicNumeralText;
}

/**
 * @return
 *     Lower case roman numeral text for the given enum value
 * @param enumValue
 *     Enumerated value.
 */
AString
GeneralYokingGroupEnum::toLowerCaseRomanNumeralText(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->lowerCaseRomanNumeralText;
}

/**
 * @return
 *     Upper case roman numeral text for the given enum value
 * @param enumValue
 *     Enumerated value.
 */
AString
GeneralYokingGroupEnum::toUpperCaseRomanNumeralText(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->upperCaseRomanNumeralText;
}

/**
 * @return
 *     Lower case alphabetical text for the given enum value
 * @param enumValue
 *     Enumerated value.
 */
AString
GeneralYokingGroupEnum::tolowerCaseAlphabeticalText(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->lowerCaseAlphabeticalText;
}

/**
 * @return
 *     Upper case alphabetical text for the given enum value
 * @param enumValue
 *     Enumerated value.
 */
AString
GeneralYokingGroupEnum::toUpperCaseAlphabeticalText(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->upperCaseAlphabeticalText;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
GeneralYokingGroupEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const GeneralYokingGroupEnum* enumInstance = findData(enumValue);
    return enumInstance->integerCode;
}

/**
 * Find the data type corresponding to an integer code.
 *
 * @param integerCode
 *     Integer code for enum.
 * @param isValidOut
 *     If not NULL, on exit isValidOut will indicate if
 *     integer code is valid.
 * @return
 *     Enum for integer code.
 */
GeneralYokingGroupEnum::Enum
GeneralYokingGroupEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GeneralYokingGroupEnum::enumData[0].enumValue;
    
    for (std::vector<GeneralYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GeneralYokingGroupEnum& enumInstance = *iter;
        if (enumInstance.integerCode == integerCode) {
            enumValue = enumInstance.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type GeneralYokingGroupEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
GeneralYokingGroupEnum::getAllEnums(std::vector<GeneralYokingGroupEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<GeneralYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

/**
 * Get all of the names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
GeneralYokingGroupEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<GeneralYokingGroupEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(GeneralYokingGroupEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}


