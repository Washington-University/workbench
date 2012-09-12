
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 * @param brain
 *    Brain holds these display properties.
 */
DisplayPropertiesFiberOrientation::DisplayPropertiesFiberOrientation(Brain* brain)
: DisplayProperties(brain)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
        m_aboveLimitInTab[i] = 1.05;
        m_belowLimitInTab[i] = -1.05;
        m_minimumMagnitudeInTab[i] = 0.05;
        m_drawWithMagnitudeInTab[i] = true;
        m_magnitudeMultiplierInTab[i] = 10.0;
        m_fiberColoringTypeInTab[i] = FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB;
        m_fiberSymbolTypeInTab[i] = FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
        m_aboveLimitInDisplayGroup[i] = 1.05;
        m_belowLimitInDisplayGroup[i] = -1.05;
        m_minimumMagnitudeInDisplayGroup[i] = 0.05;
        m_drawWithMagnitudeInDisplayGroup[i] = true;
        m_magnitudeMultiplierInDisplayGroup[i] = 10.0;
        m_fiberColoringTypeInDisplayGroup[i] = FiberOrientationColoringTypeEnum::FIBER_COLORING_XYZ_AS_RGB;
        m_fiberSymbolTypeInDisplayGroup[i] = FiberOrientationSymbolTypeEnum::FIBER_SYMBOL_LINES;
    }

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
    m_sceneAssistant->addTabIndexedFloatArray("m_magnitudeMultiplierInTab",
                                              m_magnitudeMultiplierInTab);
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
    m_sceneAssistant->addArray("m_magnitudeMultiplierInDisplayGroup",
                               m_magnitudeMultiplierInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_magnitudeMultiplierInDisplayGroup[0]);
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
 * Copy the border display properties from one tab to another.
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
    m_magnitudeMultiplierInTab[targetTabIndex] = m_magnitudeMultiplierInTab[sourceTabIndex];
    m_fiberColoringTypeInTab[targetTabIndex] = m_fiberColoringTypeInTab[sourceTabIndex];
}

/**
 * @return  Display status of borders.
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
 * Set the display status for borders for the given display group.
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
 * @return The magnitude multiplier.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberOrientation::getMagnitudeMultiplier(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_magnitudeMultiplierInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_magnitudeMultiplierInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_magnitudeMultiplierInTab[tabIndex];
    }
    return m_magnitudeMultiplierInDisplayGroup[displayGroup];
}

/**
 * Set the magnitude multiplier to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param magnitudeMultiplier
 *     New value for magnitude multiplier
 */
void
DisplayPropertiesFiberOrientation::setMagnitudeMultiplier(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex,
                                                       const float magnitudeMultiplier)
{
    CaretAssertArrayIndex(m_magnitudeMultiplierInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_magnitudeMultiplierInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_magnitudeMultiplierInTab[tabIndex] = magnitudeMultiplier;
    }
    else {
        m_magnitudeMultiplierInDisplayGroup[displayGroup] = magnitudeMultiplier;
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
    
//    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
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

