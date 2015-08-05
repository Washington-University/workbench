
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#define __DISPLAY_PROPERTIES_BORDERS_DECLARE__
#include "DisplayPropertiesBorders.h"
#undef __DISPLAY_PROPERTIES_BORDERS_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesBorders 
 * \brief Contains display properties for borders.
 *
 * Border display properties are available for every tab and also a 
 * few 'display groups'.  A number of methods in this class accept 
 * both display group and tab index parameters.  When the display 
 * group is set to 'Tab', the tab index is used meaning that the
 * attribute requeted/sent is for use with a specifc tab.  For an
 * other display group value, the attribute is for a display group
 * and the tab index is ignored.
 */

/**
 * Constructor.
 */
DisplayPropertiesBorders::DisplayPropertiesBorders()
: DisplayProperties()
{
    const float defaultPointSize = 2.0;
    const float defaultLineSize  = 1.0;
    const BorderDrawingTypeEnum::Enum defaultDrawingType = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
    const FeatureColoringTypeEnum::Enum defaultColoringType = FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME;
    const CaretColorEnum::Enum defaultColor = CaretColorEnum::BLACK;
    const float defaultUnstretchedLinesLength = 5.0;
    const bool defaultUnstretchedLinesSelection = true;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
        m_contralateralDisplayStatusInTab[i] = false;
        m_lineWidthInTab[i] = defaultLineSize;
        m_pointSizeInTab[i] = defaultPointSize;
        m_coloringTypeInTab[i] = defaultColoringType;
        m_drawingTypeInTab[i] = defaultDrawingType;
        m_standardColorTypeInTab[i] = defaultColor;
        m_unstretchedLinesLengthInTab[i] = defaultUnstretchedLinesLength;
        m_unstretchedLinesStatusInTab[i] = defaultUnstretchedLinesSelection;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
        m_contralateralDisplayStatusInDisplayGroup[i] = false;
        m_lineWidthInDisplayGroup[i]  = defaultLineSize;
        m_pointSizeInDisplayGroup[i] = defaultPointSize;
        m_coloringTypeInDisplayGroup[i] = defaultColoringType;
        m_drawingTypeInDisplayGroup[i] = defaultDrawingType;
        m_standardColorTypeInDisplayGroup[i] = defaultColor;
        m_unstretchedLinesLengthInDisplayGroup[i] = defaultUnstretchedLinesLength;
        m_unstretchedLinesStatusInDisplayGroup[i] = defaultUnstretchedLinesSelection;
    }

    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab",
                                              m_displayStatusInTab);
    
    m_sceneAssistant->addTabIndexedBooleanArray("m_contralateralDisplayStatusInTab", 
                               m_contralateralDisplayStatusInTab);
    
    m_sceneAssistant->addTabIndexedFloatArray("m_lineWidthInTab", 
                               m_lineWidthInTab);
    
    m_sceneAssistant->addTabIndexedFloatArray("m_pointSizeInTab", 
                               m_pointSizeInTab);
    
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup", 
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_displayStatusInDisplayGroup[0]);
    
    m_sceneAssistant->addArray("m_contralateralDisplayStatusInDisplayGroup", 
                               m_contralateralDisplayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_contralateralDisplayStatusInDisplayGroup[0]);
    
    m_sceneAssistant->addArray("m_lineWidthInDisplayGroup", 
                               m_lineWidthInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               defaultLineSize);
    
    m_sceneAssistant->addArray("m_pointSizeInDisplayGroup", 
                               m_pointSizeInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               defaultPointSize);
    
    
    m_sceneAssistant->addArray<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>("m_coloringTypeInDisplayGroup",
                                                                                   m_coloringTypeInDisplayGroup,
                                                                                   DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                   defaultColoringType);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>("m_coloringTypeInTab",
                                                                                                           m_coloringTypeInTab);
    
    m_sceneAssistant->addArray<BorderDrawingTypeEnum, BorderDrawingTypeEnum::Enum>("m_drawingTypeInDisplayGroup",
                                                                                   m_drawingTypeInDisplayGroup,
                                                                                   DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                   BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<BorderDrawingTypeEnum, BorderDrawingTypeEnum::Enum>("m_drawingTypeInTab",
                                                                                                           m_drawingTypeInTab);
    
    m_sceneAssistant->addTabIndexedFloatArray("m_unstretchedLinesLengthInTab",
                                              m_unstretchedLinesLengthInTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_unstretchedLinesStatusInTab",
                                                m_unstretchedLinesStatusInTab);
    m_sceneAssistant->addArray("m_unstretchedLinesLengthInDisplayGroup",
                               m_unstretchedLinesLengthInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               defaultUnstretchedLinesLength);
    m_sceneAssistant->addArray("m_unstretchedLinesStatusInDisplayGroup",
                               m_unstretchedLinesStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               defaultUnstretchedLinesSelection);
    
    
    m_sceneAssistant->addArray<CaretColorEnum, CaretColorEnum::Enum>("m_standardColorTypeInDisplayGroup",
                                                                     m_standardColorTypeInDisplayGroup,
                                                                     DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                     defaultColor);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<CaretColorEnum, CaretColorEnum::Enum>("m_standardColorTypeInTab",
                                                                                             m_standardColorTypeInTab);
}

/**
 * Destructor.
 */
DisplayPropertiesBorders::~DisplayPropertiesBorders()
{
    
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesBorders::reset()
{
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        this->displayStatus[i] = false;
//        this->contralateralDisplayStatus[i] = false;
//        this->displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
//    }
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesBorders::update()
{
    
}

/**
 * Copy the border display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void 
DisplayPropertiesBorders::copyDisplayProperties(const int32_t sourceTabIndex,
                                                     const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    
    this->m_contralateralDisplayStatusInTab[targetTabIndex] = this->m_contralateralDisplayStatusInTab[sourceTabIndex];
    this->m_displayStatusInTab[targetTabIndex] = this->m_displayStatusInTab[sourceTabIndex];
    this->m_drawingTypeInTab[targetTabIndex] = this->m_drawingTypeInTab[sourceTabIndex];
    this->m_lineWidthInTab[targetTabIndex]   = this->m_lineWidthInTab[sourceTabIndex];
    this->m_pointSizeInTab[targetTabIndex]   = this->m_pointSizeInTab[sourceTabIndex];
    this->m_unstretchedLinesLengthInTab[targetTabIndex] = this->m_unstretchedLinesLengthInTab[sourceTabIndex];
    this->m_standardColorTypeInDisplayGroup[targetTabIndex] = this->m_standardColorTypeInDisplayGroup[sourceTabIndex];
}

/**
 * @return  Display status of borders.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesBorders::isDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                      const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_displayStatusInTab[tabIndex];
    }
    return m_displayStatusInDisplayGroup[displayGroup];
}

/**
 * Set the display status for borders for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                       const int32_t tabIndex,
                                       const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);   
        m_displayStatusInTab[tabIndex] = displayStatus;
    }
    else {
        m_displayStatusInDisplayGroup[displayGroup] = displayStatus;
    }
}

/**
 * @return  Contralateral display status of borders.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesBorders::isContralateralDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                                   const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_contralateralDisplayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_contralateralDisplayStatusInTab[tabIndex];
    }
    return m_contralateralDisplayStatusInDisplayGroup[displayGroup];
}

/**
 * Set the contralateral display status for borders.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesBorders::setContralateralDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                                    const int32_t tabIndex,
                                                    const bool contralateralDisplayStatus)
{
    CaretAssertArrayIndex(m_contralateralDisplayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);        
        m_contralateralDisplayStatusInTab[tabIndex] = contralateralDisplayStatus;
    }
    else {
        m_contralateralDisplayStatusInDisplayGroup[displayGroup] = contralateralDisplayStatus;
    }
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum 
DisplayPropertiesBorders::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void 
DisplayPropertiesBorders::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The point size.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float 
DisplayPropertiesBorders::getPointSize(const DisplayGroupEnum::Enum  displayGroup,
                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_pointSizeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);    
        return m_pointSizeInTab[tabIndex];
    }
    return m_pointSizeInDisplayGroup[displayGroup];
}

/**
 * Set the point size to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param pointSize
 *     New value for point size.
 */
void 
DisplayPropertiesBorders::setPointSize(const DisplayGroupEnum::Enum  displayGroup,
                                       const int32_t tabIndex,
                                       const float pointSize)
{
    CaretAssertArrayIndex(m_pointSizeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);        
        m_pointSizeInTab[tabIndex] = pointSize;
    }
    else {
        m_pointSizeInDisplayGroup[displayGroup] = pointSize;
    }
}

/**
 * @return The line width.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float 
DisplayPropertiesBorders::getLineWidth(const DisplayGroupEnum::Enum  displayGroup,
                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_lineWidthInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_lineWidthInTab[tabIndex];
    }
    return m_lineWidthInDisplayGroup[displayGroup];
}

/**
 * Set the line width to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param lineWidth
 *     New value for line width.
 */
void 
DisplayPropertiesBorders::setLineWidth(const DisplayGroupEnum::Enum  displayGroup,
                                       const int32_t tabIndex,
                                       const float lineWidth)
{
    CaretAssertArrayIndex(m_lineWidthInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);   
        m_lineWidthInTab[tabIndex] = lineWidth;
    }
    else {
        m_lineWidthInDisplayGroup[displayGroup] = lineWidth;
    }
}

/**
 * @return  Is unstretched lines enabled?
 *
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool
DisplayPropertiesBorders::isUnstretchedLinesEnabled(const DisplayGroupEnum::Enum displayGroup,
                               const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_unstretchedLinesStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_unstretchedLinesStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_unstretchedLinesStatusInTab[tabIndex];
    }
    return m_unstretchedLinesStatusInDisplayGroup[displayGroup];
}

/**
 * Set the unstretched lines status to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param unstretchedLinesEnabled
 *     New value for line unstretched lines status.
 */
void
DisplayPropertiesBorders::setUnstretchedLinesEnabled(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                const bool unstretchedLinesEnabled)
{
    CaretAssertArrayIndex(m_unstretchedLinesStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_unstretchedLinesStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_unstretchedLinesStatusInTab[tabIndex] = unstretchedLinesEnabled;
    }
    else {
        m_unstretchedLinesStatusInDisplayGroup[displayGroup] = unstretchedLinesEnabled;
    }
}

/**
 * @return  Get unstretched lines length
 *
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesBorders::getUnstretchedLinesLength(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_unstretchedLinesLengthInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_unstretchedLinesLengthInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_unstretchedLinesLengthInTab[tabIndex];
    }
    return m_unstretchedLinesLengthInDisplayGroup[displayGroup];
}

/**
 * Set the unstretched lines length to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param unstretchedLinesLength
 *     New value for unstretched lines length.
 */
void
DisplayPropertiesBorders::setUnstretchedLinesLength(const DisplayGroupEnum::Enum displayGroup,
                               const int32_t tabIndex,
                               const float unstretchedLinesLength)
{
    CaretAssertArrayIndex(m_unstretchedLinesLengthInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_unstretchedLinesLengthInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_unstretchedLinesLengthInTab[tabIndex] = unstretchedLinesLength;
    }
    else {
        m_unstretchedLinesLengthInDisplayGroup[displayGroup] = unstretchedLinesLength;
    }
}


/**
 * @return The drawing type.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
BorderDrawingTypeEnum::Enum 
DisplayPropertiesBorders::getDrawingType(const DisplayGroupEnum::Enum  displayGroup,
                                         const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_drawingTypeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);   
        return m_drawingTypeInTab[tabIndex];
    }
    return m_drawingTypeInDisplayGroup[displayGroup];
}

/**
 * Set the drawing type to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param drawingType
 *     New value for drawing type.
 */
void 
DisplayPropertiesBorders::setDrawingType(const DisplayGroupEnum::Enum  displayGroup,
                                         const int32_t tabIndex,
                                         const BorderDrawingTypeEnum::Enum drawingType)
{
    CaretAssertArrayIndex(m_drawingTypeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        m_drawingTypeInTab[tabIndex] = drawingType;
    }
    else {
        m_drawingTypeInDisplayGroup[displayGroup] = drawingType;
    }
}

/**
 * @return The coloring type.
 * @param displayGroup
 *     Display group.
 */
FeatureColoringTypeEnum::Enum
DisplayPropertiesBorders::getColoringType(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_coloringTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_coloringTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_coloringTypeInTab[tabIndex];
    }
    return m_coloringTypeInDisplayGroup[displayGroup];
}

/**
 * Set the coloring type.
 * @param displayGroup
 *     Display group.
 * @param coloringType
 *    New value for coloring type.
 */
void
DisplayPropertiesBorders::setColoringType(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const FeatureColoringTypeEnum::Enum coloringType)
{
    CaretAssertArrayIndex(m_coloringTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_coloringTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_coloringTypeInTab[tabIndex] = coloringType;
    }
    else {
        m_coloringTypeInDisplayGroup[displayGroup] = coloringType;
    }
}






/**
 * @return The standard caret coloring type.
 * @param displayGroup
 *     Display group.
 */
CaretColorEnum::Enum
DisplayPropertiesBorders::getStandardColorType(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_standardColorTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_standardColorTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_standardColorTypeInTab[tabIndex];
    }
    return m_standardColorTypeInDisplayGroup[displayGroup];
}

/**
 * Set the caret coloring type.
 * @param displayGroup
 *     Display group.
 * @param color
 *    New color for coloring type.
 */
void
DisplayPropertiesBorders::setStandardColorType(const DisplayGroupEnum::Enum displayGroup,
                                               const int32_t tabIndex,
                                               const CaretColorEnum::Enum color)
{
    CaretAssertArrayIndex(m_standardColorTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_standardColorTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_standardColorTypeInTab[tabIndex] = color;
    }
    else {
        m_standardColorTypeInDisplayGroup[displayGroup] = color;
    }
}

//template <class ET>
//const std::vector<AString> enumArrayToStrings(const ET enumArray[],
//                                              const std::vector<int32_t>& tabIndices)
//{
//    std::vector<AString> stringVector;
//
//    const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
//    for (int32_t i = 0; i < numTabs; i++) {
//        const AString stringValue = ET::nameToString(enumArray[i],
//                                                    false);
//        stringVector.push_back(stringValue);
//    }
//    return stringVector;
//}
//
//template <class T, class ET>
//class EnumConvert {
//public:
//    static std::vector<AString> enumArrayToStringsForTabIndices(const ET enumArray[],
//                                                                const std::vector<int32_t>& tabIndices)
//    {
//        std::vector<AString> stringVector;
//        
//        const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
//        for (int32_t i = 0; i < numTabs; i++) {
//            const int32_t tabIndex = tabIndices[i];
//            const AString stringValue = T::toName(enumArray[tabIndex]);
//            stringVector.push_back(stringValue);
//        }
//        return stringVector;
//    }
//};

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
DisplayPropertiesBorders::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesBorders",
                                            1);
    
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
DisplayPropertiesBorders::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);

    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
}

