
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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
#define __CARET_FIVE_FILE_TYPE_ENUM_DECLARE__
#include "CaretFiveFileTypeEnum.h"
#undef __CARET_FIVE_FILE_TYPE_ENUM_DECLARE__

#include <QStringList>

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::CaretFiveFileTypeEnum 
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
 *         EnumComboBoxTemplate* m_caretFiveFileTypeEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void caretFiveFileTypeEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "CaretFiveFileTypeEnum.h"
 * 
 *     Instatiate:
 *         m_caretFiveFileTypeEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_caretFiveFileTypeEnumComboBox->setup<CaretFiveFileTypeEnum,CaretFiveFileTypeEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_caretFiveFileTypeEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(caretFiveFileTypeEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_caretFiveFileTypeEnumComboBox->setSelectedItem<CaretFiveFileTypeEnum,CaretFiveFileTypeEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const CaretFiveFileTypeEnum::Enum VARIABLE = m_caretFiveFileTypeEnumComboBox->getSelectedItem<CaretFiveFileTypeEnum,CaretFiveFileTypeEnum::Enum>();
 * 
 */

/*
switch (value) {
    case CaretFiveFileTypeEnum::AREACOLOR:
        break;
    case CaretFiveFileTypeEnum::AREALESTIMATION:
        break;
    case CaretFiveFileTypeEnum::BORDERCOLOR:
        break;
    case CaretFiveFileTypeEnum::BORDER:
        break;
    case CaretFiveFileTypeEnum::BORDERPROJECTION:
        break;
    case CaretFiveFileTypeEnum::CELLCOLOR:
        break;
    case CaretFiveFileTypeEnum::CELL:
        break;
    case CaretFiveFileTypeEnum::CELLPROJECTION:
        break;
    case CaretFiveFileTypeEnum::CONTOURCELLCOLOR:
        break;
    case CaretFiveFileTypeEnum::CONTOURCELL:
        break;
    case CaretFiveFileTypeEnum::CONTOUR:
        break;
    case CaretFiveFileTypeEnum::COORDINATE:
        break;
    case CaretFiveFileTypeEnum::FOCICOLOR:
        break;
    case CaretFiveFileTypeEnum::FOCI:
        break;
    case CaretFiveFileTypeEnum::FOCIPROJECTION:
        break;
    case CaretFiveFileTypeEnum::LATLON:
        break;
    case CaretFiveFileTypeEnum::MATRIX:
        break;
    case CaretFiveFileTypeEnum::METRIC:
        break;
    case CaretFiveFileTypeEnum::PAINT:
        break;
    case CaretFiveFileTypeEnum::SECTION:
        break;
    case CaretFiveFileTypeEnum::SHAPE:
        break;
    case CaretFiveFileTypeEnum::SPEC:
        break;
    case CaretFiveFileTypeEnum::TOPOLOGY:
        break;
    case CaretFiveFileTypeEnum::VOLUME:
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
 * @param extension
 *    Extension for file.
 */
CaretFiveFileTypeEnum::CaretFiveFileTypeEnum(const Enum enumValue,
                                             const AString& name,
                                             const AString& guiName,
                                             const AString& extension,
                                             const AString& extensionTwo,
                                             const AString& extensionThree,
                                             const AString& extensionFour,
                                             const AString& extensionFive)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->extensions.push_back(extension);
    if ( ! extensionTwo.isEmpty()) {
        this->extensions.push_back(extensionTwo);
    }
    if ( ! extensionThree.isEmpty()) {
        this->extensions.push_back(extensionThree);
    }
    if ( ! extensionFour.isEmpty()) {
        this->extensions.push_back(extensionFour);
    }
    if ( ! extensionFive.isEmpty()) {
        this->extensions.push_back(extensionFive);
    }
}

/**
 * Destructor.
 */
CaretFiveFileTypeEnum::~CaretFiveFileTypeEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
CaretFiveFileTypeEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(CaretFiveFileTypeEnum(INVALID,
                                    "INVALID",
                                    "Invalid",
                                             ".invalid"));
    
    enumData.push_back(CaretFiveFileTypeEnum(AREACOLOR,
                                             "AREACOLOR",
                                             "Areacolor",
                                             ".areacolor"));
    
    enumData.push_back(CaretFiveFileTypeEnum(AREALESTIMATION,
                                    "AREALESTIMATION", 
                                    "Arealestimation",
                                             ".areal_estimation"));
    
    enumData.push_back(CaretFiveFileTypeEnum(BORDERCOLOR, 
                                    "BORDERCOLOR", 
                                    "Bordercolor",
                                             ".bordercolor"));
    
    enumData.push_back(CaretFiveFileTypeEnum(BORDER, 
                                    "BORDER", 
                                    "Border",
                                             ".border"));
    
    enumData.push_back(CaretFiveFileTypeEnum(BORDERPROJECTION, 
                                    "BORDERPROJECTION", 
                                    "Borderprojection",
                                             ".borderproj"));
    
    enumData.push_back(CaretFiveFileTypeEnum(CELLCOLOR, 
                                    "CELLCOLOR", 
                                    "Cellcolor",
                                             ".cell_color"));
    
    enumData.push_back(CaretFiveFileTypeEnum(CELL, 
                                    "CELL", 
                                    "Cell",
                                             ".cell"));
    
    enumData.push_back(CaretFiveFileTypeEnum(CELLPROJECTION, 
                                    "CELLPROJECTION", 
                                    "Cellprojection",
                                             ".cellproj"));
    
    enumData.push_back(CaretFiveFileTypeEnum(CONTOURCELLCOLOR, 
                                    "CONTOURCELLCOLOR", 
                                    "Contourcellcolor",
                                             ".contour_cell_color"));
    
    enumData.push_back(CaretFiveFileTypeEnum(CONTOURCELL, 
                                    "CONTOURCELL", 
                                    "Contourcell",
                                             ".contour_cells"));
    
    enumData.push_back(CaretFiveFileTypeEnum(CONTOUR, 
                                    "CONTOUR", 
                                    "Contour",
                                             ".contours"));
    
    enumData.push_back(CaretFiveFileTypeEnum(COORDINATE, 
                                    "COORDINATE", 
                                    "Coordinate",
                                             ".coord"));
    
    enumData.push_back(CaretFiveFileTypeEnum(FOCICOLOR, 
                                    "FOCICOLOR", 
                                    "Focicolor",
                                             ".foci_color"));
    
    enumData.push_back(CaretFiveFileTypeEnum(FOCI, 
                                    "FOCI", 
                                    "Foci",
                                             ".foci"));
    
    enumData.push_back(CaretFiveFileTypeEnum(FOCIPROJECTION, 
                                    "FOCIPROJECTION", 
                                    "Fociprojection",
                                             ".fociproj"));
    
    enumData.push_back(CaretFiveFileTypeEnum(LATLON, 
                                    "LATLON", 
                                    "Latlon",
                                             ".latlon"));
    
    enumData.push_back(CaretFiveFileTypeEnum(MATRIX, 
                                    "MATRIX", 
                                    "Matrix",
                                             ".matrix"));
    
    enumData.push_back(CaretFiveFileTypeEnum(METRIC, 
                                    "METRIC", 
                                    "Metric",
                                             ".metric"));
    
    enumData.push_back(CaretFiveFileTypeEnum(PAINT, 
                                    "PAINT", 
                                    "Paint",
                                             ".paint"));
    
    enumData.push_back(CaretFiveFileTypeEnum(SECTION, 
                                    "SECTION", 
                                    "Section",
                                             ".section"));
    
    enumData.push_back(CaretFiveFileTypeEnum(SHAPE, 
                                    "SHAPE", 
                                    "Shape",
                                             ".surface_shape"));
    
    enumData.push_back(CaretFiveFileTypeEnum(SPEC, 
                                    "SPEC", 
                                    "Spec",
                                             ".spec"));
    
    enumData.push_back(CaretFiveFileTypeEnum(TOPOLOGY, 
                                    "TOPOLOGY", 
                                    "Topology",
                                             ".topo"));
    
    enumData.push_back(CaretFiveFileTypeEnum(VOLUME, 
                                    "VOLUME", 
                                    "Volume",
                                             ".afni|.HEAD|.nii|.nii.gz"));
    
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const CaretFiveFileTypeEnum*
CaretFiveFileTypeEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const CaretFiveFileTypeEnum* d = &enumData[i];
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
CaretFiveFileTypeEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CaretFiveFileTypeEnum* enumInstance = findData(enumValue);
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
CaretFiveFileTypeEnum::Enum 
CaretFiveFileTypeEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CaretFiveFileTypeEnum::enumData[0].enumValue;
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretFiveFileTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type CaretFiveFileTypeEnum"));
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
CaretFiveFileTypeEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const CaretFiveFileTypeEnum* enumInstance = findData(enumValue);
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
CaretFiveFileTypeEnum::Enum 
CaretFiveFileTypeEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CaretFiveFileTypeEnum::enumData[0].enumValue;
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretFiveFileTypeEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type CaretFiveFileTypeEnum"));
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
CaretFiveFileTypeEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const CaretFiveFileTypeEnum* enumInstance = findData(enumValue);
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
CaretFiveFileTypeEnum::Enum
CaretFiveFileTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = CaretFiveFileTypeEnum::enumData[0].enumValue;
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const CaretFiveFileTypeEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type CaretFiveFileTypeEnum"));
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
CaretFiveFileTypeEnum::getAllEnums(std::vector<CaretFiveFileTypeEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
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
CaretFiveFileTypeEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(CaretFiveFileTypeEnum::toName(iter->enumValue));
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
CaretFiveFileTypeEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(CaretFiveFileTypeEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return File extension for given enum
 * @param enumValue
 *    The enumerated value
 */
AString
CaretFiveFileTypeEnum::toExtension(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const CaretFiveFileTypeEnum* enumInstance = findData(enumValue);
    CaretAssert( ! enumInstance->extensions.empty());
    return enumInstance->extensions[0];
    return "Invalid";
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
CaretFiveFileTypeEnum::Enum
CaretFiveFileTypeEnum::fromFilename(const AString& filename, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = INVALID;
    
    for (std::vector<CaretFiveFileTypeEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        QStringList exts;
        const CaretFiveFileTypeEnum& enumInstance = *iter;
        for (int32_t i = 0; i < static_cast<int32_t>(enumInstance.extensions.size()); i++) {
            if (filename.endsWith(enumInstance.extensions[i])) {
                enumValue = enumInstance.enumValue;
                validFlag = true;
                break;
            }
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    else if ( ! validFlag) {
        CaretAssertMessage(0, AString("Filename: " + filename + " failed to match enumerated value for type CaretFiveFileTypeEnum"));
    }
    return enumValue;
}

/**
 * Get the file filter text for use in a QFileDialog for READING files
 *
 * @param enumValue
 *     Enumerated type for file filter.
 * @return
 *     Text containing file filter.
 */
AString
CaretFiveFileTypeEnum::toQFileDialogFilterForReading(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const CaretFiveFileTypeEnum* enumInstance = findData(enumValue);
    
    AString filterText(enumInstance->guiName
                       + " Files (");
    
    bool firstTime(true);
    for (auto& ext : enumInstance->extensions) {
        if ( ! firstTime) {
            filterText += " ";
        }
        filterText += ("*" + ext);
        
        firstTime = false;
    }
    filterText += ")";

    return filterText;
}
