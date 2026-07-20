
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
#define __NEUROGLANCER_ANNOTATION_TYPE_ENUM_DECLARE__
#include "NeuroglancerAnnotationTypeEnum.h"
#undef __NEUROGLANCER_ANNOTATION_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::NeuroglancerAnnotationTypeEnum 
 * \brief 
 *
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_neuroglancerAnnotationTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void neuroglancerAnnotationTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "NeuroglancerAnnotationTypeEnum.h"
 * 
 *     Instatiate:
 *         m_neuroglancerAnnotationTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_neuroglancerAnnotationTypeEnumComboBox->setup<NeuroglancerAnnotationTypeEnum,NeuroglancerAnnotationTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_neuroglancerAnnotationTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(neuroglancerAnnotationTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_neuroglancerAnnotationTypeEnumComboBox->setSelectedItem<NeuroglancerAnnotationTypeEnum,NeuroglancerAnnotationTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const NeuroglancerAnnotationTypeEnum::Enum VARIABLE = m_neuroglancerAnnotationTypeEnumComboBox->getSelectedItem<NeuroglancerAnnotationTypeEnum,NeuroglancerAnnotationTypeEnum::Enum>();
 * 
 */

/*
switch (value) {
    case NeuroglancerAnnotationTypeEnum::INVALID:
        break;
    case NeuroglancerAnnotationTypeEnum::AXIS_ALIGNED_BOUNDING_BOX:
        break;
    case NeuroglancerAnnotationTypeEnum::ELLIPSOID:
        break;
    case NeuroglancerAnnotationTypeEnum::LINE:
        break;
    case NeuroglancerAnnotationTypeEnum::POINT:
        break;
    case NeuroglancerAnnotationTypeEnum::POLYLINE:
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
 * @param guiName
 *    User-friendly name for use in user-interface.
 */
NeuroglancerAnnotationTypeEnum::NeuroglancerAnnotationTypeEnum(const Enum enumValue,
                           const AString& name,
                           const AString& guiName)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
NeuroglancerAnnotationTypeEnum::~NeuroglancerAnnotationTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
NeuroglancerAnnotationTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(NeuroglancerAnnotationTypeEnum(INVALID,
                                                      "INVALID",
                                                      "Invalid"));
    
    enumData.push_back(NeuroglancerAnnotationTypeEnum(AXIS_ALIGNED_BOUNDING_BOX,
                                    "AXIS_ALIGNED_BOUNDING_BOX", 
                                    "Axis Aligned Bounding Box"));
    
    enumData.push_back(NeuroglancerAnnotationTypeEnum(ELLIPSOID, 
                                    "ELLIPSOID", 
                                    "Ellipsoid"));
    
    enumData.push_back(NeuroglancerAnnotationTypeEnum(LINE, 
                                    "LINE", 
                                    "Line"));
    
    enumData.push_back(NeuroglancerAnnotationTypeEnum(POINT, 
                                    "POINT", 
                                    "Point"));
    
    enumData.push_back(NeuroglancerAnnotationTypeEnum(POLYLINE, 
                                    "POLYLINE", 
                                    "Polyline"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const NeuroglancerAnnotationTypeEnum*
NeuroglancerAnnotationTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const NeuroglancerAnnotationTypeEnum* d = &enumData[i];
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
NeuroglancerAnnotationTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const NeuroglancerAnnotationTypeEnum* enumInstance = findData(enumValue);
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
NeuroglancerAnnotationTypeEnum::Enum 
NeuroglancerAnnotationTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = NeuroglancerAnnotationTypeEnum::enumData[0].enumValue;
    
    for (std::vector<NeuroglancerAnnotationTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NeuroglancerAnnotationTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type NeuroglancerAnnotationTypeEnum"));
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
NeuroglancerAnnotationTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const NeuroglancerAnnotationTypeEnum* enumInstance = findData(enumValue);
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
NeuroglancerAnnotationTypeEnum::Enum 
NeuroglancerAnnotationTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = NeuroglancerAnnotationTypeEnum::enumData[0].enumValue;
    
    for (std::vector<NeuroglancerAnnotationTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NeuroglancerAnnotationTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type NeuroglancerAnnotationTypeEnum"));
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
NeuroglancerAnnotationTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const NeuroglancerAnnotationTypeEnum* enumInstance = findData(enumValue);
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
NeuroglancerAnnotationTypeEnum::Enum
NeuroglancerAnnotationTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = NeuroglancerAnnotationTypeEnum::enumData[0].enumValue;
    
    for (std::vector<NeuroglancerAnnotationTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NeuroglancerAnnotationTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type NeuroglancerAnnotationTypeEnum"));
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
NeuroglancerAnnotationTypeEnum::getAllEnums(std::vector<NeuroglancerAnnotationTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<NeuroglancerAnnotationTypeEnum>::iterator iter = enumData.begin();
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
NeuroglancerAnnotationTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<NeuroglancerAnnotationTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(NeuroglancerAnnotationTypeEnum::toName(iter->enumValue));
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
NeuroglancerAnnotationTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<NeuroglancerAnnotationTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(NeuroglancerAnnotationTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

