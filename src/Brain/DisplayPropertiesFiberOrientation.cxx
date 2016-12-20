
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

#define __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__
#include "DisplayPropertiesFiberOrientation.h"
#undef __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesFiberOrientation 
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
DisplayPropertiesFiberOrientation::DisplayPropertiesFiberOrientation()
: DisplayProperties()
{
    const float aboveLimit = 0.63;
    const float belowLimit = -0.63;
    const float minimumMagnitude = 0.05;
    const bool drawWithMagnitude = true;
    const float lengthMultiplier = 6.0;
    const float fanMultiplier = 3.0;
    const FiberOrientationColoringTypeEnum::Enum coloringType = FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB;
    const FiberOrientationSymbolTypeEnum::Enum symbolType = FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES;
    const bool displaySphereOrientions = false;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
        m_aboveLimitInTab[i] = aboveLimit;
        m_belowLimitInTab[i] = belowLimit;
        m_minimumMagnitudeInTab[i] = minimumMagnitude;
        m_drawWithMagnitudeInTab[i] = drawWithMagnitude;
        m_lengthMultiplierInTab[i] = lengthMultiplier;
        m_fiberColoringTypeInTab[i] = coloringType;
        m_fiberSymbolTypeInTab[i] = symbolType;
        m_fanMultiplierInTab[i] = fanMultiplier;
        m_displaySphereOrientationsInTab[i] = displaySphereOrientions;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
        m_aboveLimitInDisplayGroup[i] = aboveLimit;
        m_belowLimitInDisplayGroup[i] = belowLimit;
        m_minimumMagnitudeInDisplayGroup[i] = minimumMagnitude;
        m_drawWithMagnitudeInDisplayGroup[i] = drawWithMagnitude;
        m_lengthMultiplierInDisplayGroup[i] = lengthMultiplier;
        m_fiberColoringTypeInDisplayGroup[i] = coloringType;
        m_fiberSymbolTypeInDisplayGroup[i] = symbolType;
        m_fanMultiplierInDisplayGroup[i] = fanMultiplier;
        m_displaySphereOrientationsInDisplayGroup[i] = displaySphereOrientions;
    }

    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab",
                                              m_displayStatusInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_aboveLimitInTab",
                                              m_aboveLimitInTab);
    
    m_sceneAssistant->addTabIndexedFloatArray("m_belowLimitInTab",
                                              m_belowLimitInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_minimumMagnitudeInTab",
                                              m_minimumMagnitudeInTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_drawWithMagnitudeInTab",
                                                m_drawWithMagnitudeInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_lengthMultiplierInTab",
                                              m_lengthMultiplierInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_fanMultiplierInTab",
                                              m_fanMultiplierInTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displaySphereOrientationsInTab",
                                                m_displaySphereOrientationsInTab);
    m_sceneAssistant->addArray<FiberOrientationColoringTypeEnum, FiberOrientationColoringTypeEnum::Enum>("m_drawingTypeInDisplayGroup",
                                                                                                         m_fiberColoringTypeInDisplayGroup,
                                                                                                         DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                                         FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB);
    
    m_sceneAssistant->addArray<FiberOrientationSymbolTypeEnum, FiberOrientationSymbolTypeEnum::Enum>("m_fiberSymbolTypeInDisplayGroup",
                                                                                                         m_fiberSymbolTypeInDisplayGroup,
                                                                                                         DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                                         FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES);
    
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup",
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_displayStatusInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_aboveLimitInDisplayGroup",
                               m_aboveLimitInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_aboveLimitInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_belowLimitInDisplayGroup",
                               m_belowLimitInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_belowLimitInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_minimumMagnitudeInDisplayGroup",
                               m_minimumMagnitudeInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_minimumMagnitudeInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_drawWithMagnitudeInDisplayGroup",
                               m_drawWithMagnitudeInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_drawWithMagnitudeInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_lengthMultiplierInDisplayGroup",
                               m_lengthMultiplierInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_lengthMultiplierInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_fanMultiplierInDisplayGroup",
                               m_fanMultiplierInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_fanMultiplierInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_displaySphereOrientationsInDisplayGroup",
                               m_displaySphereOrientationsInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_displaySphereOrientationsInDisplayGroup[0]);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FiberOrientationColoringTypeEnum, FiberOrientationColoringTypeEnum::Enum>("m_fiberColoringTypeInTab",
                                                                                                           m_fiberColoringTypeInTab);
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FiberOrientationSymbolTypeEnum, FiberOrientationSymbolTypeEnum::Enum>("m_fiberSymbolTypeInTab",
                                                                                                                                 m_fiberSymbolTypeInTab);
}

/**
 * Destructor.
 */
DisplayPropertiesFiberOrientation::~DisplayPropertiesFiberOrientation()
{
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesFiberOrientation::reset()
{
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesFiberOrientation::update()
{
    
}

/**
 * Copy the fiber orientation display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void 
DisplayPropertiesFiberOrientation::copyDisplayProperties(const int32_t sourceTabIndex,
                                                     const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    
    m_displayStatusInTab[targetTabIndex] = m_displayStatusInTab[sourceTabIndex];
    m_aboveLimitInTab[targetTabIndex] = m_aboveLimitInTab[sourceTabIndex];
    m_belowLimitInTab[targetTabIndex] = m_belowLimitInTab[sourceTabIndex];
    m_minimumMagnitudeInTab[targetTabIndex] = m_minimumMagnitudeInTab[sourceTabIndex];
    m_drawWithMagnitudeInTab[targetTabIndex] = m_drawWithMagnitudeInTab[sourceTabIndex];
    m_lengthMultiplierInTab[targetTabIndex] = m_lengthMultiplierInTab[sourceTabIndex];
    m_fiberColoringTypeInTab[targetTabIndex] = m_fiberColoringTypeInTab[sourceTabIndex];
    m_fanMultiplierInTab[targetTabIndex] = m_fanMultiplierInTab[sourceTabIndex];
}

/**
 * @return  Display status of fiber orientations.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesFiberOrientation::isDisplayed(const DisplayGroupEnum::Enum  displayGroup,
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
 * Set the display status for fiber orientations for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesFiberOrientation::setDisplayed(const DisplayGroupEnum::Enum  displayGroup,
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
 * @return  Display status of sphere orientations.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool
DisplayPropertiesFiberOrientation::isSphereOrientationsDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                   const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displaySphereOrientationsInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displaySphereOrientationsInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_displaySphereOrientationsInTab[tabIndex];
    }
    return m_displaySphereOrientationsInDisplayGroup[displayGroup];
}

/**
 * Set the display status for sphere orientations for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void
DisplayPropertiesFiberOrientation::setSphereOrientationsDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex,
                                    const bool displaySphereOrientations)
{
    CaretAssertArrayIndex(m_displaySphereOrientationsInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displaySphereOrientationsInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_displaySphereOrientationsInTab[tabIndex] = displaySphereOrientations;
    }
    else {
        m_displaySphereOrientationsInDisplayGroup[displayGroup] = displaySphereOrientations;
    }
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum 
DisplayPropertiesFiberOrientation::getDisplayGroupForTab(const int32_t browserTabIndex) const
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
DisplayPropertiesFiberOrientation::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return Draw with magnitude status.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool
DisplayPropertiesFiberOrientation::isDrawWithMagnitude(const DisplayGroupEnum::Enum  displayGroup,
                                               const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_drawWithMagnitudeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_drawWithMagnitudeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_drawWithMagnitudeInTab[tabIndex];
    }
    return m_drawWithMagnitudeInDisplayGroup[displayGroup];
}

/**
 * Set the draw with magnitude status for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param drawWithMagnitude
 *    New status.
 */
void
DisplayPropertiesFiberOrientation::setDrawWithMagnitude(const DisplayGroupEnum::Enum  displayGroup,
                                                const int32_t tabIndex,
                                                const bool drawWithMagnitude)
{
    CaretAssertArrayIndex(m_drawWithMagnitudeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_drawWithMagnitudeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_drawWithMagnitudeInTab[tabIndex] = drawWithMagnitude;
    }
    else {
        m_drawWithMagnitudeInDisplayGroup[displayGroup] = drawWithMagnitude;
    }
}

/**
 * @return The Above limit.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float 
DisplayPropertiesFiberOrientation::getAboveLimit(const DisplayGroupEnum::Enum  displayGroup,
                                                 const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_aboveLimitInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_aboveLimitInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);    
        return m_aboveLimitInTab[tabIndex];
    }
    return m_aboveLimitInDisplayGroup[displayGroup];
}

/**
 * Set the above limit to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param aboveLimit
 *     New value for above limit.
 */
void 
DisplayPropertiesFiberOrientation::setAboveLimit(const DisplayGroupEnum::Enum  displayGroup,
                                       const int32_t tabIndex,
                                       const float aboveLimit)
{
    CaretAssertArrayIndex(m_aboveLimitInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_aboveLimitInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);        
        m_aboveLimitInTab[tabIndex] = aboveLimit;
    }
    else {
        m_aboveLimitInDisplayGroup[displayGroup] = aboveLimit;
    }
}

/**
 * @return The below limit.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberOrientation::getBelowLimit(const DisplayGroupEnum::Enum  displayGroup,
                                                 const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_belowLimitInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_belowLimitInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_belowLimitInTab[tabIndex];
    }
    return m_belowLimitInDisplayGroup[displayGroup];
}

/**
 * Set the below limit to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param pointSize
 *     New value for below limit.
 */
void
DisplayPropertiesFiberOrientation::setBelowLimit(const DisplayGroupEnum::Enum  displayGroup,
                                                 const int32_t tabIndex,
                                                 const float belowLimit)
{
    CaretAssertArrayIndex(m_belowLimitInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_belowLimitInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_belowLimitInTab[tabIndex] = belowLimit;
    }
    else {
        m_belowLimitInDisplayGroup[displayGroup] = belowLimit;
    }
}

/**
 * Set the above and below limits for all display groups and tabs.
 * @param
 *    Value for all above limits.
 * @param 
 *    Value for all below limits.
 */
void
DisplayPropertiesFiberOrientation::setAboveAndBelowLimitsForAll(const float aboveLimit,
                                                                const float belowLimit)
{    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_aboveLimitInTab[i] = aboveLimit;
        m_belowLimitInTab[i] = belowLimit;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_aboveLimitInDisplayGroup[i] = aboveLimit;
        m_belowLimitInDisplayGroup[i] = belowLimit;
    }
}

/**
 * @return The minimum magnitude.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberOrientation::getMinimumMagnitude(const DisplayGroupEnum::Enum  displayGroup,
                                                const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_minimumMagnitudeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_minimumMagnitudeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_minimumMagnitudeInTab[tabIndex];
    }
    return m_minimumMagnitudeInDisplayGroup[displayGroup];
}

/**
 * Set the minimum magnitude to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param minimumMagnitude
 *     New value for minimum magnitude.
 */
void
DisplayPropertiesFiberOrientation::setMinimumMagnitude(const DisplayGroupEnum::Enum  displayGroup,
                                                const int32_t tabIndex,
                                                const float minimumMagnitude)
{
    CaretAssertArrayIndex(m_minimumMagnitudeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_minimumMagnitudeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_minimumMagnitudeInTab[tabIndex] = minimumMagnitude;
    }
    else {
        m_minimumMagnitudeInDisplayGroup[displayGroup] = minimumMagnitude;
    }
}

/**
 * @return The length multiplier.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberOrientation::getLengthMultiplier(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_lengthMultiplierInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_lengthMultiplierInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_lengthMultiplierInTab[tabIndex];
    }
    return m_lengthMultiplierInDisplayGroup[displayGroup];
}

/**
 * Set the length multiplier to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param lengthMultiplier
 *     New value for length multiplier
 */
void
DisplayPropertiesFiberOrientation::setLengthMultiplier(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex,
                                                       const float lengthMultiplier)
{
    CaretAssertArrayIndex(m_lengthMultiplierInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_lengthMultiplierInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_lengthMultiplierInTab[tabIndex] = lengthMultiplier;
    }
    else {
        m_lengthMultiplierInDisplayGroup[displayGroup] = lengthMultiplier;
    }
}

/**
 * @return The coloring type.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
FiberOrientationColoringTypeEnum::Enum
DisplayPropertiesFiberOrientation::getColoringType(const DisplayGroupEnum::Enum  displayGroup,
                                                  const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_fiberColoringTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fiberColoringTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_fiberColoringTypeInTab[tabIndex];
    }
    return m_fiberColoringTypeInDisplayGroup[displayGroup];
}

/**
 * Set the coloring type to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param coloringType
 *     New value for coloring type.
 */
void
DisplayPropertiesFiberOrientation::setColoringType(const DisplayGroupEnum::Enum  displayGroup,
                                                  const int32_t tabIndex,
                                                  const FiberOrientationColoringTypeEnum::Enum coloringType)
{
    CaretAssertArrayIndex(m_fiberColoringTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fiberColoringTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_fiberColoringTypeInTab[tabIndex] = coloringType;
    }
    else {
        m_fiberColoringTypeInDisplayGroup[displayGroup] = coloringType;
    }
}

/**
 * @return The symbol type.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
FiberOrientationSymbolTypeEnum::Enum
DisplayPropertiesFiberOrientation::getSymbolType(const DisplayGroupEnum::Enum  displayGroup,
                                                   const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_fiberSymbolTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fiberSymbolTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_fiberSymbolTypeInTab[tabIndex];
    }
    return m_fiberSymbolTypeInDisplayGroup[displayGroup];
}

/**
 * Set the symbol type to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param symbolType
 *     New value for symbol type.
 */
void
DisplayPropertiesFiberOrientation::setSymbolType(const DisplayGroupEnum::Enum  displayGroup,
                                                   const int32_t tabIndex,
                                                   const FiberOrientationSymbolTypeEnum::Enum symbolType)
{
    CaretAssertArrayIndex(m_fiberSymbolTypeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fiberSymbolTypeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_fiberSymbolTypeInTab[tabIndex] = symbolType;
    }
    else {
        m_fiberSymbolTypeInDisplayGroup[displayGroup] = symbolType;
    }
}



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
DisplayPropertiesFiberOrientation::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesFiberOrientation",
                                            1);
    
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
DisplayPropertiesFiberOrientation::restoreFromScene(const SceneAttributes* sceneAttributes,
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

/**
 * @return The fan multiplier.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberOrientation::getFanMultiplier(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_fanMultiplierInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fanMultiplierInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_fanMultiplierInTab[tabIndex];
    }
    return m_fanMultiplierInDisplayGroup[displayGroup];
}

/**
 * Set the fan multiplier to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param lengthMultiplier
 *     New value for fan multiplier
 */
void
DisplayPropertiesFiberOrientation::setFanMultiplier(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex,
                                                       const float fanMultiplier)
{
    CaretAssertArrayIndex(m_fanMultiplierInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fanMultiplierInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_fanMultiplierInTab[tabIndex] = fanMultiplier;
    }
    else {
        m_fanMultiplierInDisplayGroup[displayGroup] = fanMultiplier;
    }
}

