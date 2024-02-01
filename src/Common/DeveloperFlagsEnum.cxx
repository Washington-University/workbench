
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#define __DEVELOPER_FLAGS_ENUM_DECLARE__
#include "DeveloperFlagsEnum.h"
#undef __DEVELOPER_FLAGS_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::DeveloperFlagsEnum 
 * \brief Flags used during development.
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_developerFlagsEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void developerFlagsEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "DeveloperFlagsEnum.h"
 * 
 *     Instatiate:
 *         m_developerFlagsEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_developerFlagsEnumComboBox->setup<DeveloperFlagsEnum,DeveloperFlagsEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_developerFlagsEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(developerFlagsEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_developerFlagsEnumComboBox->setSelectedItem<DeveloperFlagsEnum,DeveloperFlagsEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const DeveloperFlagsEnum::Enum VARIABLE = m_developerFlagsEnumComboBox->getSelectedItem<DeveloperFlagsEnum,DeveloperFlagsEnum::Enum>();
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
 * @param checkable
 *    Checkable status (NO, YES)
 * @param defaultValue
 *    Default value for flag
 */
DeveloperFlagsEnum::DeveloperFlagsEnum(const Enum enumValue,
                                       const AString& name,
                                       const AString& guiName,
                                       const CheckableEnum checkable,
                                       const bool defaultValue)
{
    this->enumValue = enumValue;
    this->integerCode = integerCodeCounter++;
    this->name = name;
    this->guiName = guiName;
    this->checkable = checkable;
    this->flagStatus = defaultValue;
}

/**
 * Destructor.
 */
DeveloperFlagsEnum::~DeveloperFlagsEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
DeveloperFlagsEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    std::vector<DeveloperFlagsEnum> checkableItems;
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_UNUSED,
                                                "DEVELOPER_FLAG_UNUSED",
                                                "Developer Flag Unused",
                                                CheckableEnum::YES,
                                                false));
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_FLIP_PALETTE_NOT_DATA,
                                                "DEVELOPER_FLAG_FLIP_PALETTE_NOT_DATA",
                                                "Drawing: Flip Palette Not Data",
                                                CheckableEnum::YES,
                                                false));
    
    checkableItems.push_back(DeveloperFlagsEnum(DELELOPER_FLAG_TEXTURE_ANATOMY_VOLUME_SMOOTH,
                                                "DELELOPER_FLAG_TEXTURE_ANATOMY_VOLUME_SMOOTH",
                                                "Volume: MPR Smooth Anatomy Volume Voxels",
                                                CheckableEnum::YES,
                                                true));

    checkableItems.push_back(DeveloperFlagsEnum(DELELOPER_FLAG_TEXTURE_FUNCTIONAL_VOLUME_SMOOTH,
                                                "DELELOPER_FLAG_TEXTURE_FUNCTIONAL_VOLUME_SMOOTH",
                                                "Volume: MPR Smooth Functional Volume Voxels",
                                                CheckableEnum::YES,
                                                false));
    
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_SURFACE_BUFFER,
                                                "DEVELOPER_FLAG_SURFACE_BUFFER",
                                                "Drawing: Draw Surfaces Using Buffers",
                                                CheckableEnum::YES,
                                                false));
    
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_VOXEL_CUBES_TEST,
                                                "DEVELOPER_FLAG_VOXEL_CUBES_TEST",
                                                "Drawing: All View Voxel Cubes Outside Faces",
                                                CheckableEnum::YES,
                                                true));
    
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_CHART_OPENGL_LINES,
                                                "DEVELOPER_FLAG_CHART_OPENGL_LINES",
                                                "Drawing: Draw Chart Lines with OpenGL Lines",
                                                CheckableEnum::YES,
                                                true));
    
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_BLENDING,
                                                "DEVELOPER_FLAG_BLENDING",
                                                "Drawing: Separate RGB / Alpha Opacity",
                                                CheckableEnum::YES,
                                                true));
    
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_MPR_THREE_SLICES_CHANGED_JUMP_FIX,
                                                "DEVELOPER_FLAG_MPR_THREE_SLICES_CHANGED_JUMP_FIX",
                                                "MPR Three: Fix Selected Slices Changed Jumping",
                                                CheckableEnum::YES,
                                                true));
    
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_VOXEL_EDIT,
                                                "DEVELOPER_FLAG_VOXEL_EDIT",
                                                "Voxel Edit Performance",
                                                CheckableEnum::YES,
                                                true));
    
#ifdef HAVE_WEBKIT
    checkableItems.push_back(DeveloperFlagsEnum(DEVELOPER_FLAG_BALSA,
                                                "DEVELOPER_FLAG_BALSA",
                                                "Visit BALSA...",
                                                CheckableEnum::NO,
                                                false));
#endif
    
    std::vector<DeveloperFlagsEnum> notCheckableItems;

    enumData.insert(enumData.end(),
                    checkableItems.begin(), checkableItems.end());
    enumData.insert(enumData.end(),
                    notCheckableItems.begin(), notCheckableItems.end());
}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
DeveloperFlagsEnum*
DeveloperFlagsEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        DeveloperFlagsEnum* d = &enumData[i];
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
DeveloperFlagsEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
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
DeveloperFlagsEnum::Enum 
DeveloperFlagsEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DeveloperFlagsEnum::enumData[0].enumValue;
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DeveloperFlagsEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + "failed to match enumerated value for type DeveloperFlagsEnum"));
    }
    return enumValue;
}

AString
DeveloperFlagsEnum::toToolTip(Enum enumValue)
{
    AString toolTip;
    
    switch (enumValue) {
        case DELELOPER_FLAG_TEXTURE_ANATOMY_VOLUME_SMOOTH:
            toolTip = ("Smooth MPR volume anatomy volume drawing voxels");
            break;
        case DELELOPER_FLAG_TEXTURE_FUNCTIONAL_VOLUME_SMOOTH:
            toolTip = ("Smooth MPR volume functional volume drawing voxels");
            break;
        case DEVELOPER_FLAG_SURFACE_BUFFER:
            toolTip = ("Draw surface using buffers (improved performance)");
            break;
        case DEVELOPER_FLAG_BALSA:
            toolTip = ("Load BALSA web page in Qt's WebKit (for demonstration only");
            break;
        case DEVELOPER_FLAG_BLENDING:
            toolTip = ("Separately blend RGB and Alpha components so Alpha is always 1.0 in frame buffer"
                       " (fixes coloring problems and colors in some captured image formats)");
            break;
        case DEVELOPER_FLAG_CHART_OPENGL_LINES:
            toolTip = ("Draw chart lines using OpenGL lines instead of polylines "
                       "(faster but OpenGL lines have limited line width).  This setting "
                       "DOES NOT affect files currently loaded.  Any files that produce "
                       "line charts must be reloaded.");
            break;
        case DEVELOPER_FLAG_FLIP_PALETTE_NOT_DATA:
            toolTip = ("When Invert Data is selected on Overlay and Map Settings Dialog,  "
                       "flip the palette instead of flipping the data (not recommended)");
            break;
        case DEVELOPER_FLAG_MPR_THREE_SLICES_CHANGED_JUMP_FIX:
            toolTip = ("MPR Three: Prevents slices from jumping if the selected slices are changed "
                       " and there is non-zero rotation.");
            break;
        case DEVELOPER_FLAG_VOXEL_EDIT:
            toolTip = ("Voxel edit performance improvements");
            break;
        case DEVELOPER_FLAG_UNUSED:
            toolTip = "Unused";
            break;
        case DEVELOPER_FLAG_VOXEL_CUBES_TEST:
            toolTip = ("For voxel cubes in ALL view, only draw \"outside faces\" "
                       "(faster but causes problem when opacity is less than one so disable "
                       "in that instance)");
            break;
    }
    
    return toolTip;
}

/**
 * Get a GUI string representation of the enumerated type.
 * @param enumValue 
 *     Enumerated value.
 * @return 
 *     String representing enumerated value.
 */
AString 
DeveloperFlagsEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
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
DeveloperFlagsEnum::Enum 
DeveloperFlagsEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DeveloperFlagsEnum::enumData[0].enumValue;
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DeveloperFlagsEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + "failed to match enumerated value for type DeveloperFlagsEnum"));
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
DeveloperFlagsEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
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
DeveloperFlagsEnum::Enum
DeveloperFlagsEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = DeveloperFlagsEnum::enumData[0].enumValue;
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const DeveloperFlagsEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + "failed to match enumerated value for type DeveloperFlagsEnum"));
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
DeveloperFlagsEnum::getAllEnums(std::vector<DeveloperFlagsEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allEnums.push_back(iter->enumValue);
    }
}

/**
 * Get all of the enumerated type values.  The values can be used
 * as parameters to toXXX() methods to get associated metadata.
 *
 * @param allEnums
 *     A vector that is OUTPUT containing all of the enumerated values.
 */
void
DeveloperFlagsEnum::getAllEnumsSortedByGuiName(std::vector<Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();

    std::vector<AString> allNames;
    const bool sortedFlag(true);
    getAllGuiNames(allNames, sortedFlag);
    
    for (auto& name : allNames) {
        bool validFlag(false);
        const Enum enumValue(fromGuiName(name, &validFlag));
        CaretAssert(validFlag);
        if (validFlag) {
            allEnums.push_back(enumValue);
        }
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
DeveloperFlagsEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(DeveloperFlagsEnum::toName(iter->enumValue));
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
DeveloperFlagsEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<DeveloperFlagsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(DeveloperFlagsEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * Is the developer flag set?
 *
 * @param enumValue
 *     Enum value for flag
 * @return
 *     True/False status for flag.
 */
bool
DeveloperFlagsEnum::isFlag(const Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const DeveloperFlagsEnum* enumInstance = findData(enumValue);
    return enumInstance->flagStatus;
}

/**
 * Set the developer flag.
 *
 * @param enumValue
 *     Enum value for flag
 * @param flagStatus
 *     True/False status for flag.
 */
void
DeveloperFlagsEnum::setFlag(const Enum enumValue,
                            const bool flagStatus)
{
    if (initializedFlag == false) initialize();
    DeveloperFlagsEnum* enumInstance = findData(enumValue);
    enumInstance->flagStatus = flagStatus;
}

/**
 * @return True if the developer flag is checkable
 */
bool
DeveloperFlagsEnum::isCheckable(const Enum enumValue)
{
    bool checkableStatus(false);
    
    if (initializedFlag == false) initialize();
    DeveloperFlagsEnum* enumInstance = findData(enumValue);
    switch (enumInstance->checkable) {
        case CheckableEnum::NO:
            checkableStatus = false;
            break;
        case CheckableEnum::YES:
            checkableStatus = true;
            break;
    }
    
    return checkableStatus;
}


