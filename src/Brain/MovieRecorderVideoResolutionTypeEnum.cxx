
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

#include <algorithm>
#define __MOVIE_RECORDER_VIDEO_RESOLUTION_TYPE_ENUM_DECLARE__
#include "MovieRecorderVideoResolutionTypeEnum.h"
#undef __MOVIE_RECORDER_VIDEO_RESOLUTION_TYPE_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::MovieRecorderVideoResolutionTypeEnum
 * \brief Resolutions for movie recording
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_MovieRecorderVideoResolutionTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void MovieRecorderVideoResolutionTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "MovieRecorderVideoResolutionTypeEnum.h"
 * 
 *     Instatiate:
 *         m_MovieRecorderVideoResolutionTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_MovieRecorderVideoResolutionTypeEnumComboBox->setup<MovieRecorderVideoResolutionTypeEnum,MovieRecorderVideoResolutionTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_MovieRecorderVideoResolutionTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(MovieRecorderVideoResolutionTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_MovieRecorderVideoResolutionTypeEnumComboBox->setSelectedItem<MovieRecorderVideoResolutionTypeEnum,MovieRecorderVideoResolutionTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const MovieRecorderVideoResolutionTypeEnum::Enum VARIABLE = m_MovieRecorderVideoResolutionTypeEnumComboBox->getSelectedItem<MovieRecorderVideoResolutionTypeEnum,MovieRecorderVideoResolutionTypeEnum::Enum>();
 * 
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
MovieRecorderVideoResolutionTypeEnum::MovieRecorderVideoResolutionTypeEnum(const Enum enumValue,
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
MovieRecorderVideoResolutionTypeEnum::~MovieRecorderVideoResolutionTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
MovieRecorderVideoResolutionTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(MovieRecorderVideoResolutionTypeEnum(CUSTOM,
                                                            "CUSTOM",
                                                            "Custom"));
    
    enumData.push_back(MovieRecorderVideoResolutionTypeEnum(UHD_3840_2160,
                                                            "UHD_3840_2160",
                                                            "UHD (3840x2160)"));
    
    enumData.push_back(MovieRecorderVideoResolutionTypeEnum(FULL_HD_1920_1080,
                                                            "FULL_HD_1920_1080",
                                                            "Full HD (1920x1080)"));
    
    enumData.push_back(MovieRecorderVideoResolutionTypeEnum(HD_1280_720,
                                                            "HD_1280_720",
                                                            "HD Ready (1280x720)"));
    
    enumData.push_back(MovieRecorderVideoResolutionTypeEnum(SD_640_480,
                                                            "SD_640_480",
                                                            "SD (640x480)"));

}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const MovieRecorderVideoResolutionTypeEnum*
MovieRecorderVideoResolutionTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const MovieRecorderVideoResolutionTypeEnum* d = &enumData[i];
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
MovieRecorderVideoResolutionTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const MovieRecorderVideoResolutionTypeEnum* enumInstance = findData(enumValue);
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
MovieRecorderVideoResolutionTypeEnum::Enum
MovieRecorderVideoResolutionTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = MovieRecorderVideoResolutionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<MovieRecorderVideoResolutionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const MovieRecorderVideoResolutionTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type MovieRecorderVideoResolutionTypeEnum"));
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
MovieRecorderVideoResolutionTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const MovieRecorderVideoResolutionTypeEnum* enumInstance = findData(enumValue);
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
MovieRecorderVideoResolutionTypeEnum::Enum
MovieRecorderVideoResolutionTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = MovieRecorderVideoResolutionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<MovieRecorderVideoResolutionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const MovieRecorderVideoResolutionTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type MovieRecorderVideoResolutionTypeEnum"));
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
MovieRecorderVideoResolutionTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const MovieRecorderVideoResolutionTypeEnum* enumInstance = findData(enumValue);
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
MovieRecorderVideoResolutionTypeEnum::Enum
MovieRecorderVideoResolutionTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = MovieRecorderVideoResolutionTypeEnum::enumData[0].enumValue;
    
    for (std::vector<MovieRecorderVideoResolutionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const MovieRecorderVideoResolutionTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type MovieRecorderVideoResolutionTypeEnum"));
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
MovieRecorderVideoResolutionTypeEnum::getAllEnums(std::vector<MovieRecorderVideoResolutionTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<MovieRecorderVideoResolutionTypeEnum>::iterator iter = enumData.begin();
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
MovieRecorderVideoResolutionTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<MovieRecorderVideoResolutionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(MovieRecorderVideoResolutionTypeEnum::toName(iter->enumValue));
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
MovieRecorderVideoResolutionTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<MovieRecorderVideoResolutionTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(MovieRecorderVideoResolutionTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Get the width and height for given enumerated value.  Output will be zeros
 * for custom.
 *
 * @param enumValue
 *     Enumerated value.
 * @param widthOut
 *     Output containing width
 * @param heightOut
 *     Output containing height
 */
void
MovieRecorderVideoResolutionTypeEnum::getWidthAndHeight(const Enum enumValue,
                                                        int32_t& widthOut,
                                                        int32_t& heightOut)
{
    switch (enumValue) {
        case CUSTOM:
            break;
        case FULL_HD_1920_1080:
            widthOut  = 1920;
            heightOut = 1080;
            break;
        case HD_1280_720:
            widthOut  = 1280;
            heightOut = 720;
            break;
        case SD_640_480:
            widthOut  = 640;
            heightOut = 480;
            break;
        case UHD_3840_2160:
            widthOut  = 3840;
            heightOut = 2160;
            break;
    }
}

