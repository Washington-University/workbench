
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
#define __ZARR_COMPRESSOR_TYPE_ENUM_DECLARE__
#include "ZarrCompressorTypeEnum.h"
#undef __ZARR_COMPRESSOR_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::ZarrCompressorTypeEnum 
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
 *         EnumComboBoxTemplate* m_zarrCompressorTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void zarrCompressorTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "ZarrCompressorTypeEnum.h"
 * 
 *     Instatiate:
 *         m_zarrCompressorTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_zarrCompressorTypeEnumComboBox->setup<ZarrCompressorTypeEnum,ZarrCompressorTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_zarrCompressorTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(zarrCompressorTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_zarrCompressorTypeEnumComboBox->setSelectedItem<ZarrCompressorTypeEnum,ZarrCompressorTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const ZarrCompressorTypeEnum::Enum VARIABLE = m_zarrCompressorTypeEnumComboBox->getSelectedItem<ZarrCompressorTypeEnum,ZarrCompressorTypeEnum::Enum>();
 * 
 */

/*
switch (value) {
    case ZarrCompressorTypeEnum::NO_COMPRESSOR:
        break;
    case ZarrCompressorTypeEnum::BLOSC:
        break;
    case ZarrCompressorTypeEnum::BYTES:
        break;
    case ZarrCompressorTypeEnum::CRC32C:
        break;
    case ZarrCompressorTypeEnum::GZIP:
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
 *
 * @param encodingName
 *    Name of encoding
 */
ZarrCompressorTypeEnum::ZarrCompressorTypeEnum(const Enum enumValue,
                           const AString& name,
                           const AString& encodingName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->encodingName = encodingName;
}

/**
 * Destructor.
 */
ZarrCompressorTypeEnum::~ZarrCompressorTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
ZarrCompressorTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(ZarrCompressorTypeEnum(NO_COMPRESSOR, 
                                    "NO_COMPRESSOR", 
                                    "No Compressor"));
    
    enumData.push_back(ZarrCompressorTypeEnum(BLOSC, 
                                    "BLOSC", 
                                    "blosc"));
    
    enumData.push_back(ZarrCompressorTypeEnum(BYTES, 
                                    "BYTES", 
                                    "bytes"));
    
    enumData.push_back(ZarrCompressorTypeEnum(CRC32C, 
                                    "CRC32C", 
                                    "crc32c"));
    
    enumData.push_back(ZarrCompressorTypeEnum(GZIP, 
                                    "GZIP", 
                                    "gzip"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const ZarrCompressorTypeEnum*
ZarrCompressorTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const ZarrCompressorTypeEnum* d = &enumData[i];
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
ZarrCompressorTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ZarrCompressorTypeEnum* enumInstance = findData(enumValue);
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
ZarrCompressorTypeEnum::Enum 
ZarrCompressorTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrCompressorTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrCompressorTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrCompressorTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type ZarrCompressorTypeEnum"));
    }
    return enumValue;
}

/**
 * Get the encoding name of the enumerated type.
 * @param enumValue
 *     Enumerated value.
 * @return 
 *     String representing encoding.
 */
AString 
ZarrCompressorTypeEnum::toEncodingName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const ZarrCompressorTypeEnum* enumInstance = findData(enumValue);
    return enumInstance->encodingName;
}

/**
 * Get an enumerated value corresponding to its encoding name
 * @param s
 *     Name of enumerated value.
 * @param isValidOut 
 *     If not NULL, it is set indicating that a
 *     enum value exists for the encoding name.
 * @return 
 *     Enumerated value.
 */
ZarrCompressorTypeEnum::Enum 
ZarrCompressorTypeEnum::fromEncodingName(const AString& encodingName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrCompressorTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrCompressorTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrCompressorTypeEnum& d = *iter;
        if (d.encodingName == encodingName) {
            enumValue = d.enumValue;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if (validFlag == false) {
        CaretAssertMessage(0, AString("encoding name " + encodingName + " failed to match enumerated value for type ZarrCompressorTypeEnum"));
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
ZarrCompressorTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const ZarrCompressorTypeEnum* enumInstance = findData(enumValue);
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
ZarrCompressorTypeEnum::Enum
ZarrCompressorTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = ZarrCompressorTypeEnum::enumData[0].enumValue;
    
    for (std::vector<ZarrCompressorTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const ZarrCompressorTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type ZarrCompressorTypeEnum"));
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
ZarrCompressorTypeEnum::getAllEnums(std::vector<ZarrCompressorTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<ZarrCompressorTypeEnum>::iterator iter = enumData.begin();
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
ZarrCompressorTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<ZarrCompressorTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(ZarrCompressorTypeEnum::toName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allNames.begin(), allNames.end());
    }
}

/**
 * Get all of the encoding names of the enumerated type values.
 *
 * @param allEncodingNames
 *     A vector that is OUTPUT containing all of the encoding names of the enumerated values.
 * @param isSorted
 *     If true, the names are sorted in alphabetical order.
 */
void
ZarrCompressorTypeEnum::getAllEncodingNames(std::vector<AString>& allEncodingNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allEncodingNames.clear();
    
    for (std::vector<ZarrCompressorTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEncodingNames.push_back(ZarrCompressorTypeEnum::toEncodingName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allEncodingNames.begin(), allEncodingNames.end());
    }
}

