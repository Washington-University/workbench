
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include <QVersionNumber>

#define __DISPLAY_HIGH_DPI_MODE_ENUM_DECLARE__
#include "DisplayHighDpiModeEnum.h"
#undef __DISPLAY_HIGH_DPI_MODE_ENUM_DECLARE__

#include "ApplicationInformation.h"
#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::DisplayHighDpiModeEnum 
 * \brief Enumerated type for enabling high DPI display
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_displayHighDpiModeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void displayHighDpiModeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "DisplayHighDpiModeEnum.h"
 * 
 *     Instatiate:
 *         m_displayHighDpiModeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_displayHighDpiModeEnumComboBox->setup<DisplayHighDpiModeEnum,DisplayHighDpiModeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_displayHighDpiModeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(displayHighDpiModeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_displayHighDpiModeEnumComboBox->setSelectedItem<DisplayHighDpiModeEnum,DisplayHighDpiModeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const DisplayHighDpiModeEnum::Enum VARIABLE = m_displayHighDpiModeEnumComboBox->getSelectedItem<DisplayHighDpiModeEnum,DisplayHighDpiModeEnum::Enum>();
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
DisplayHighDpiModeEnum::DisplayHighDpiModeEnum(const Enum enumValue,
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
DisplayHighDpiModeEnum::~DisplayHighDpiModeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
DisplayHighDpiModeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(DisplayHighDpiModeEnum(DPI_OFF,
                                              "DPI_OFF",
                                              "Off"));
    
    enumData.push_back(DisplayHighDpiModeEnum(DPI_ON,
                                              "DPI_ON",
                                              "On"));
    
    enumData.push_back(DisplayHighDpiModeEnum(DPI_AUTO,
                                              "DPI_AUTO",
                                              "Auto"));

}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const DisplayHighDpiModeEnum*
DisplayHighDpiModeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const DisplayHighDpiModeEnum* d = &enumData[i];
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
DisplayHighDpiModeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DisplayHighDpiModeEnum* enumInstance = findData(enumValue);
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
DisplayHighDpiModeEnum::Enum 
DisplayHighDpiModeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultValue();
    
    for (std::vector<DisplayHighDpiModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DisplayHighDpiModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type DisplayHighDpiModeEnum"));
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
DisplayHighDpiModeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DisplayHighDpiModeEnum* enumInstance = findData(enumValue);
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
DisplayHighDpiModeEnum::Enum 
DisplayHighDpiModeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultValue();
    
    for (std::vector<DisplayHighDpiModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DisplayHighDpiModeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type DisplayHighDpiModeEnum"));
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
DisplayHighDpiModeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DisplayHighDpiModeEnum* enumInstance = findData(enumValue);
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
DisplayHighDpiModeEnum::Enum
DisplayHighDpiModeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = getDefaultValue();
    
    for (std::vector<DisplayHighDpiModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DisplayHighDpiModeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type DisplayHighDpiModeEnum"));
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
DisplayHighDpiModeEnum::getAllEnums(std::vector<DisplayHighDpiModeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<DisplayHighDpiModeEnum>::iterator iter = enumData.begin();
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
DisplayHighDpiModeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<DisplayHighDpiModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(DisplayHighDpiModeEnum::toName(iter->enumValue));
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
DisplayHighDpiModeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<DisplayHighDpiModeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(DisplayHighDpiModeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return The default value for this enum
 */
DisplayHighDpiModeEnum::Enum
DisplayHighDpiModeEnum::getDefaultValue()
{
    return DisplayHighDpiModeEnum::DPI_AUTO;
}

/**
 * @return Tooltip for GUI
 */
AString
DisplayHighDpiModeEnum::getToolTip()
{
    AString modesList("<ul>");
    std::vector<DisplayHighDpiModeEnum::Enum> allEnums;
    getAllEnums(allEnums);
    for (auto& e : allEnums) {
        modesList.append("<li> " + DisplayHighDpiModeEnum::toGuiName(e) + " - ");
        switch (e) {
            case DPI_AUTO:
                modesList.append("High DPI is enabled by examining operating system and Qt Version");
                break;
            case DPI_OFF:
                modesList.append("High DPI is off");
                break;
            case DPI_ON:
                modesList.append("High DPI is on");
                break;
        }
    }
    modesList.append("</ul>");
    
    AString s("<html><body>"
              + modesList
              + "<p>"
              "wb_view may not display correctly on some High DPI (retina) displays.&nbsp;&nbsp;"
              "On these displays, the models (surfaces, volumes, etc.) may appear only in the bottom "
              "left quadrant of the window.&nbsp;&nbsp;Enabling High DPI fixes this problem.&nbsp;&nbsp;In <i>Auto</i> mode, "
              "the operating system and version of Qt are examined and will enable the High DPI correction "
              "when necessary.&nbsp;&nbsp;In the event <i>Auto</i> does not function correctly, the other modes "
              "are available to enable or disable the High DPI correction."
              "<p>"
              "High DPI is needed for most MacBooks and is dependent upon the Qt version (software used "
              "for the user-interface).&nbsp;&nbsp;We do not know the exact Qt version, but sometime after 5.7 and "
              "no later than 5.14.&nbsp;&nbsp;This version of wb_view is using Qt "
              + QString(qVersion())
              + "."
              "</body></html>");
    return s;
}

/**
 * @return True if High DPI should be enabled for Auto Mode
 */
bool
DisplayHighDpiModeEnum::isHighDpiEnabledForAutoMode()
{
    if ( ! s_isHighDpiEnabledForAutoModeValid) {
        s_isHighDpiEnabledForAutoModeValid = true;
        
        /**
         * @return The Qt Library (runtime) version number
         */
        QVersionNumber qtLibraryVersionNumber = QVersionNumber::fromString(qVersion());
        
        /**
         * Use the operating system and Qt version for enabling High DPI in Auto Mode
         */
        s_isHighDpiEnabledForAutoMode = false;
#ifdef CARET_OS_MACOSX
        /*
         * Enable for Qt 6 and Qt 5.14 or later
         */
        if (qtLibraryVersionNumber.majorVersion() > 5) {
            s_isHighDpiEnabledForAutoMode = true;
        }
        else if (qtLibraryVersionNumber.majorVersion() == 5) {
            if (qtLibraryVersionNumber.minorVersion() >= 14) {
                s_isHighDpiEnabledForAutoMode = true;
            }
        }
#endif // CARET_OS_MACOSX
    }
    
    return s_isHighDpiEnabledForAutoMode;
}



