
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#define __CARDINAL_DIRECTION_ENUM_DECLARE__
#include "CardinalDirectionEnum.h"
#undef __CARDINAL_DIRECTION_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::CardinalDirectionEnum 
 * \brief Enumerated type for cardinal direction
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_cardinalDirectionEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void cardinalDirectionEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "CardinalDirectionEnum.h"
 * 
 *     Instatiate:
 *         m_cardinalDirectionEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_cardinalDirectionEnumComboBox->setup<CardinalDirectionEnum,CardinalDirectionEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_cardinalDirectionEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(cardinalDirectionEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_cardinalDirectionEnumComboBox->setSelectedItem<CardinalDirectionEnum,CardinalDirectionEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const CardinalDirectionEnum::Enum VARIABLE = m_cardinalDirectionEnumComboBox->getSelectedItem<CardinalDirectionEnum,CardinalDirectionEnum::Enum>();
 * 
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
 * @param guiShortName
 *    Short User-friendly name for use in user-interface
 */
CardinalDirectionEnum::CardinalDirectionEnum(const Enum enumValue,
                                             const AString& name,
                                             const AString& guiName,
                                             const AString& guiShortName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->guiShortName = guiShortName;
}

/**
 * Destructor.
 */
CardinalDirectionEnum::~CardinalDirectionEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
CardinalDirectionEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(CardinalDirectionEnum(AUTO,
                                             "AUTO",
                                             "Auto",
                                             "A"));
    
    enumData.push_back(CardinalDirectionEnum(NORTH,
                                             "NORTH",
                                             "North",
                                             "N"));
    
    enumData.push_back(CardinalDirectionEnum(NORTHEAST,
                                             "NORTHEAST",
                                             "Northeast",
                                             "NE"));
    
    enumData.push_back(CardinalDirectionEnum(EAST,
                                             "EAST",
                                             "East",
                                             "E"));
    
    enumData.push_back(CardinalDirectionEnum(SOUTHEAST,
                                             "SOUTHEAST",
                                             "Southeast",
                                             "SE"));
    
    enumData.push_back(CardinalDirectionEnum(SOUTH,
                                             "SOUTH",
                                             "South",
                                             "S"));
    
    enumData.push_back(CardinalDirectionEnum(SOUTHWEST,
                                             "SOUTHWEST",
                                             "Southwest",
                                             "SW"));
    
    enumData.push_back(CardinalDirectionEnum(WEST,
                                             "WEST",
                                             "West",
                                             "W"));
    
    enumData.push_back(CardinalDirectionEnum(NORTHWEST,
                                             "NORTHWEST",
                                             "Northwest",
                                             "NW"));
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const CardinalDirectionEnum*
CardinalDirectionEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const CardinalDirectionEnum* d = &enumData[i];
        if (d->enumValue == enumValue) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
CardinalDirectionEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CardinalDirectionEnum* enumInstance = findData(enumValue);
    return enumInstance->name;
}

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
CardinalDirectionEnum::Enum 
CardinalDirectionEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CardinalDirectionEnum::enumData[0].enumValue;
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CardinalDirectionEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type CardinalDirectionEnum"));
    }
    return enumValue;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
CardinalDirectionEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CardinalDirectionEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
}

/**
 * Get a short GUI string representation of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString
CardinalDirectionEnum::toGuiShortName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CardinalDirectionEnum* enumInstance = findData(enumValue);
    return enumInstance->guiShortName;
}

/**
 * Get an enumerated value corresponding to its GUI name.
 * @param s 
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return 
 *     Enumerated value.
 */
CardinalDirectionEnum::Enum 
CardinalDirectionEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CardinalDirectionEnum::enumData[0].enumValue;
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CardinalDirectionEnum& d = *iter;
        if (d.guiName == guiName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type CardinalDirectionEnum"));
    }
    return enumValue;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t
CardinalDirectionEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const CardinalDirectionEnum* enumInstance = findData(enumValue);
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
CardinalDirectionEnum::Enum
CardinalDirectionEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CardinalDirectionEnum::enumData[0].enumValue;
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CardinalDirectionEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type CardinalDirectionEnum"));
    }
    return enumValue;
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 * AUTO is excluded unless it is in the options
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 * @param options
 *     Options that control if AUTO is included.
 */
void
CardinalDirectionEnum::getAllEnums(std::vector<Enum>& allEnums,
                                   const std::set<Options>& options)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        switch (iter->enumValue) {
            case AUTO:
                if (options.find(Options::INCLUDE_AUTO) == options.end()) {
                    continue;
                }
                break;
            default:
                break;
        }
        
        allEnums.push_back(iter->enumValue);
    }
}


/**
 * Get all of the enumerated type values EXCEPT for AUTO.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
CardinalDirectionEnum::getAllEnums(std::vector<CardinalDirectionEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        if (iter->enumValue == CardinalDirectionEnum::AUTO) {
            continue;
        }
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
CardinalDirectionEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(CardinalDirectionEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}

/**
 * Get all of the GUI names of the enumerated type values.
 *
 * @param allNames
 *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
CardinalDirectionEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<CardinalDirectionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(CardinalDirectionEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

