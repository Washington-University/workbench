
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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
#define __ZARR_ROW_COLUMN_MAJOR_ORDER_TYPE_ENUM_DECLARE__
#include "ZarrRowColumnMajorOrderTypeEnum.h"
#undef __ZARR_ROW_COLUMN_MAJOR_ORDER_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::ZarrRowColumnMajorOrderTypeEnum 
 * \brief 
 *
 * https://zarr-specs.readthedocs.io/en/latest/v2/v2.0.html
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_zarrChunkBytesOrderingEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void zarrChunkBytesOrderingEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "ZarrRowColumnMajorOrderTypeEnum.h"
 * 
 *     Instatiate:
 *         m_zarrChunkBytesOrderingEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_zarrChunkBytesOrderingEnumComboBox->setup<ZarrRowColumnMajorOrderTypeEnum,ZarrRowColumnMajorOrderTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_zarrChunkBytesOrderingEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(zarrChunkBytesOrderingEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_zarrChunkBytesOrderingEnumComboBox->setSelectedItem<ZarrRowColumnMajorOrderTypeEnum,ZarrRowColumnMajorOrderTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const ZarrRowColumnMajorOrderTypeEnum::Enum VARIABLE = m_zarrChunkBytesOrderingEnumComboBox->getSelectedItem<ZarrRowColumnMajorOrderTypeEnum,ZarrRowColumnMajorOrderTypeEnum::Enum>();
 * 
 */

/*
switch (value) {
    case ZarrRowColumnMajorOrderTypeEnum::UNKNOWN:
        break;
    case ZarrRowColumnMajorOrderTypeEnum::COLUMN_MAJOR:
        break;
    case ZarrRowColumnMajorOrderTypeEnum::ROW_MAJOR:
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
 * @param encoding
 *    Encoding used in ZARR file
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
ZarrRowColumnMajorOrderTypeEnum::ZarrRowColumnMajorOrderTypeEnum(const Enum enumValue,
                                                       const AString& name,
                                                       const AString& encoding,
                                                       const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->encoding = encoding;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
ZarrRowColumnMajorOrderTypeEnum::~ZarrRowColumnMajorOrderTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
ZarrRowColumnMajorOrderTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(ZarrRowColumnMajorOrderTypeEnum(UNKNOWN,
                                                  "UNKNOWN",
                                                  "",
                                                  "Unknown"));
    
    enumData.push_back(ZarrRowColumnMajorOrderTypeEnum(COLUMN_MAJOR,
                                                  "COLUMN_MAJOR",
                                                  "F", /* From spec: “F” means column-major order, i.e., the first dimension varies fastest. */
                                                  "Column Major")); /* Spec: https://zarr-specs.readthedocs.io/en/latest/v2/v2.0.html */
    
    enumData.push_back(ZarrRowColumnMajorOrderTypeEnum(ROW_MAJOR,
                                                  "ROW_MAJOR",
                                                  "C", /* From Spec: “C” means row-major order, i.e., the last dimension varies fastest */
                                                  "Row Major"));

}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const ZarrRowColumnMajorOrderTypeEnum*
ZarrRowColumnMajorOrderTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const ZarrRowColumnMajorOrderTypeEnum* d = &enumData[i];
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
ZarrRowColumnMajorOrderTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ZarrRowColumnMajorOrderTypeEnum* enumInstance = findData(enumValue);
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
ZarrRowColumnMajorOrderTypeEnum::Enum 
ZarrRowColumnMajorOrderTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrRowColumnMajorOrderTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrRowColumnMajorOrderTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type ZarrRowColumnMajorOrderTypeEnum"));
    }
    return enumValue;
}

/**
 * Get a chunk bytes ordering encoding of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return
 *     Encoding representing enumerated value.
 */
AString
ZarrRowColumnMajorOrderTypeEnum::toEncoding(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ZarrRowColumnMajorOrderTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->encoding;
}

/**
 * Get an enumerated value corresponding to its encoding.
 * @param encoding
 *     Encoding of enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return
 *     Enumerated value.
 */
ZarrRowColumnMajorOrderTypeEnum::Enum
ZarrRowColumnMajorOrderTypeEnum::fromEncoding(const AString& encoding, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrRowColumnMajorOrderTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrRowColumnMajorOrderTypeEnum& d = *iter;
        if (d.encoding == encoding) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("Encoding " + encoding + " failed to match enumerated value for type ZarrRowColumnMajorOrderTypeEnum"));
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
ZarrRowColumnMajorOrderTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ZarrRowColumnMajorOrderTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->guiName;
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
ZarrRowColumnMajorOrderTypeEnum::Enum 
ZarrRowColumnMajorOrderTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrRowColumnMajorOrderTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrRowColumnMajorOrderTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type ZarrRowColumnMajorOrderTypeEnum"));
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
ZarrRowColumnMajorOrderTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const ZarrRowColumnMajorOrderTypeEnum* enumInstance = findData(enumValue);
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
ZarrRowColumnMajorOrderTypeEnum::Enum
ZarrRowColumnMajorOrderTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrRowColumnMajorOrderTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrRowColumnMajorOrderTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type ZarrRowColumnMajorOrderTypeEnum"));
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
ZarrRowColumnMajorOrderTypeEnum::getAllEnums(std::vector<ZarrRowColumnMajorOrderTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
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
ZarrRowColumnMajorOrderTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(ZarrRowColumnMajorOrderTypeEnum::toName(iter->enumValue));
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
ZarrRowColumnMajorOrderTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<ZarrRowColumnMajorOrderTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(ZarrRowColumnMajorOrderTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

