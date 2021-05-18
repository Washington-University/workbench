
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
#define __GRAPHICS_TEXTURE_MINIFICATION_FILTER_ENUM_DECLARE__
#include "GraphicsTextureMinificationFilterEnum.h"
#undef __GRAPHICS_TEXTURE_MINIFICATION_FILTER_ENUM_DECLARE__

#include "CaretAssert.h"

using namespace caret;

    
/**
 * \class caret::GraphicsTextureMinificationFilterEnum 
 * \brief Filtering when a screen pixel is larger than texture texel (pixel maps to a group of texels)
 *
 * Using this enumerated type in the GUI with an EnumComboBoxTemplate
 * 
 * Header File (.h)
 *     Forward declare the data type:
 *         class EnumComboBoxTemplate;
 * 
 *     Declare the member:
 *         EnumComboBoxTemplate* m_graphicsTextureMinificationFilterEnumComboBox;
 * 
 *     Declare a slot that is called when user changes selection
 *         private slots:
 *             void graphicsTextureMinificationFilterEnumComboBoxItemActivated();
 * 
 * Implementation File (.cxx)
 *     Include the header files
 *         #include "EnumComboBoxTemplate.h"
 *         #include "GraphicsTextureMinificationFilterEnum.h"
 * 
 *     Instatiate:
 *         m_graphicsTextureMinificationFilterEnumComboBox = new EnumComboBoxTemplate(this);
 *         m_graphicsTextureMinificationFilterEnumComboBox->setup<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
 * 
 *     Get notified when the user changes the selection: 
 *         QObject::connect(m_graphicsTextureMinificationFilterEnumComboBox, SIGNAL(itemActivated()),
 *                          this, SLOT(graphicsTextureMinificationFilterEnumComboBoxItemActivated()));
 * 
 *     Update the selection:
 *         m_graphicsTextureMinificationFilterEnumComboBox->setSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>(NEW_VALUE);
 * 
 *     Read the selection:
 *         const GraphicsTextureMinificationFilterEnum::Enum VARIABLE = m_graphicsTextureMinificationFilterEnumComboBox->getSelectedItem<GraphicsTextureMinificationFilterEnum,GraphicsTextureMinificationFilterEnum::Enum>();
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
GraphicsTextureMinificationFilterEnum::GraphicsTextureMinificationFilterEnum(const Enum enumValue,
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
GraphicsTextureMinificationFilterEnum::~GraphicsTextureMinificationFilterEnum()
{
}

/**
 * Initialize the enumerated metadata.
 */
void
GraphicsTextureMinificationFilterEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(GraphicsTextureMinificationFilterEnum(NEAREST,
                                                             "NEAREST",
                                                             "Nearest"));
    
    enumData.push_back(GraphicsTextureMinificationFilterEnum(LINEAR,
                                                             "LINEAR",
                                                             "Linear"));
    
    enumData.push_back(GraphicsTextureMinificationFilterEnum(NEAREST_MIPMAP_NEAREST,
                                                             "NEAREST_MIPMAP_NEAREST",
                                                             "Nearest Mipmap Nearest"));
    
    enumData.push_back(GraphicsTextureMinificationFilterEnum(LINEAR_MIPMAP_NEAREST,
                                                             "LINEAR_MIPMAP_NEAREST",
                                                             "Linear Mipmap Nearest"));
    
    enumData.push_back(GraphicsTextureMinificationFilterEnum(NEAREST_MIPMAP_LINEAR,
                                                             "NEAREST_MIPMAP_LINEAR",
                                                             "Nearest Mipmap Linear"));
    
    enumData.push_back(GraphicsTextureMinificationFilterEnum(LINEAR_MIPMAP_LINEAR,
                                                             "LINEAR_MIPMAP_LINEAR",
                                                             "Linear Mipmap Linear"));

}

/**
 * Find the data for and enumerated value.
 * @param enumValue
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const GraphicsTextureMinificationFilterEnum*
GraphicsTextureMinificationFilterEnum::findData(const Enum enumValue)
{
    if (initializedFlag == false) initialize();

    size_t num = enumData.size();
    for (size_t i = 0; i < num; i++) {
        const GraphicsTextureMinificationFilterEnum* d = &enumData[i];
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
GraphicsTextureMinificationFilterEnum::toName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GraphicsTextureMinificationFilterEnum* enumInstance = findData(enumValue);
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
GraphicsTextureMinificationFilterEnum::Enum 
GraphicsTextureMinificationFilterEnum::fromName(const AString& name, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GraphicsTextureMinificationFilterEnum::enumData[0].enumValue;
    
    for (std::vector<GraphicsTextureMinificationFilterEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GraphicsTextureMinificationFilterEnum& d = *iter;
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
        CaretAssertMessage(0, AString("Name " + name + " failed to match enumerated value for type GraphicsTextureMinificationFilterEnum"));
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
GraphicsTextureMinificationFilterEnum::toGuiName(Enum enumValue) {
    if (initializedFlag == false) initialize();
    
    const GraphicsTextureMinificationFilterEnum* enumInstance = findData(enumValue);
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
GraphicsTextureMinificationFilterEnum::Enum 
GraphicsTextureMinificationFilterEnum::fromGuiName(const AString& guiName, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GraphicsTextureMinificationFilterEnum::enumData[0].enumValue;
    
    for (std::vector<GraphicsTextureMinificationFilterEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GraphicsTextureMinificationFilterEnum& d = *iter;
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
        CaretAssertMessage(0, AString("guiName " + guiName + " failed to match enumerated value for type GraphicsTextureMinificationFilterEnum"));
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
GraphicsTextureMinificationFilterEnum::toIntegerCode(Enum enumValue)
{
    if (initializedFlag == false) initialize();
    const GraphicsTextureMinificationFilterEnum* enumInstance = findData(enumValue);
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
GraphicsTextureMinificationFilterEnum::Enum
GraphicsTextureMinificationFilterEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    if (initializedFlag == false) initialize();
    
    bool validFlag = false;
    Enum enumValue = GraphicsTextureMinificationFilterEnum::enumData[0].enumValue;
    
    for (std::vector<GraphicsTextureMinificationFilterEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const GraphicsTextureMinificationFilterEnum& enumInstance = *iter;
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
        CaretAssertMessage(0, AString("Integer code " + AString::number(integerCode) + " failed to match enumerated value for type GraphicsTextureMinificationFilterEnum"));
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
GraphicsTextureMinificationFilterEnum::getAllEnums(std::vector<GraphicsTextureMinificationFilterEnum::Enum>& allEnums)
{
    if (initializedFlag == false) initialize();
    
    allEnums.clear();
    
    for (std::vector<GraphicsTextureMinificationFilterEnum>::iterator iter = enumData.begin();
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
GraphicsTextureMinificationFilterEnum::getAllNames(std::vector<AString>& allNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allNames.clear();
    
    for (std::vector<GraphicsTextureMinificationFilterEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allNames.push_back(GraphicsTextureMinificationFilterEnum::toName(iter->enumValue));
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
GraphicsTextureMinificationFilterEnum::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)
{
    if (initializedFlag == false) initialize();
    
    allGuiNames.clear();
    
    for (std::vector<GraphicsTextureMinificationFilterEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        allGuiNames.push_back(GraphicsTextureMinificationFilterEnum::toGuiName(iter->enumValue));
    }
    
    if (isSorted) {
        std::sort(allGuiNames.begin(), allGuiNames.end());
    }
}

/**
 * @return Tooltip describing filter types
 */
AString
GraphicsTextureMinificationFilterEnum::toToolTip()
{
    AString text;
    
    text += ("The minification filter is applied when the screen pixel is greater in area than "
             "the texture texel such that one pixel overlays a group of texels.  Affects "
             "drawing when zoomed out (image made smaller).\n");
    
    std::vector<GraphicsTextureMinificationFilterEnum::Enum> allEnums;
    GraphicsTextureMinificationFilterEnum::getAllEnums(allEnums);
    
    for (const auto& e : allEnums) {
        text += (" * " + toName(e) + " - ");
        switch (e) {
            case NEAREST:
                text += ("Use nearest texel from original image");
                break;
            case LINEAR:
                text += ("Weighted average of nearest four texels from original image");
                break;
            case NEAREST_MIPMAP_NEAREST:
                text += ("Use nearest texel from mipmap with texels that most closely match size of pixel");
                break;
            case LINEAR_MIPMAP_NEAREST:
                text += ("Weighted averge of nearest four texels from most closely matching mipmap");
                break;
            case NEAREST_MIPMAP_LINEAR:
                text += ("Use weighted average of nearest texel from two most closely matching mipmaps");
                break;
            case LINEAR_MIPMAP_LINEAR:
                text += ("Weighted averge of the results from a weighted average of the nearest four texels from two most closely matching mipmaps");
                break;
        }
        text += ("\n");
    }
    return text;
}

