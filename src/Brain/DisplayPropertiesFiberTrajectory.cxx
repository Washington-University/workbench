
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

#define __DISPLAY_PROPERTIES_FIBER_TRAJECTORY_DECLARE__
#include "DisplayPropertiesFiberTrajectory.h"
#undef __DISPLAY_PROPERTIES_FIBER_TRAJECTORY_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesFiberTrajectory 
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
DisplayPropertiesFiberTrajectory::DisplayPropertiesFiberTrajectory(Brain* brain)
: DisplayProperties(brain)
{
    const int32_t thresholdStreamline = 5;
    const float maximumProportionOpacity = 0.80;
    const float minimumProportionOpacity = 0.20;
    const FiberTrajectoryDisplayModeEnum::Enum displayMode = FiberTrajectoryDisplayModeEnum::FIBER_TRAJECTORY_DISPLAY_PROPORTION;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
        
        m_displayModeInTab[i] = displayMode;
        m_proportionStreamlineInTab[i] = thresholdStreamline;
        m_maximumProportionOpacityInTab[i] = maximumProportionOpacity;
        m_minimumProportionOpacityInTab[i] = minimumProportionOpacity;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
        m_displayModeInDisplayGroup[i] = displayMode;
        m_proportionStreamlineInDisplayGroup[i] = thresholdStreamline;
        m_maximimProportionOpacityInDisplayGroup[i] = maximumProportionOpacity;
        m_minimumProportionOpacityInDisplayGroup[i] = minimumProportionOpacity;
    }

    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab",
                                              m_displayStatusInTab);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FiberTrajectoryDisplayModeEnum, FiberTrajectoryDisplayModeEnum::Enum>("m_displayModeInTab", m_displayModeInTab);
    m_sceneAssistant->addTabIndexedIntegerArray("m_proportionStreamlineInTab",
                                              m_proportionStreamlineInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_maximumProportionOpacityInTab",
                                              m_maximumProportionOpacityInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_minimumProportionOpacityInTab",
                                                m_minimumProportionOpacityInTab);
    
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup",
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_displayStatusInDisplayGroup[0]);
    
    m_sceneAssistant->addArray<FiberTrajectoryDisplayModeEnum, FiberTrajectoryDisplayModeEnum::Enum>("m_displayModeInDisplayGroup",
                                 m_displayModeInDisplayGroup,
                                 DisplayGroupEnum::NUMBER_OF_GROUPS,
                                 m_displayModeInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_proportionStreamlineInDisplayGroup",
                               m_proportionStreamlineInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_proportionStreamlineInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_maximimProportionOpacityInDisplayGroup",
                               m_maximimProportionOpacityInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_maximimProportionOpacityInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_minimumProportionOpacityInDisplayGroup",
                               m_minimumProportionOpacityInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_minimumProportionOpacityInDisplayGroup[0]);
}

/**
 * Destructor.
 */
DisplayPropertiesFiberTrajectory::~DisplayPropertiesFiberTrajectory()
{
    
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesFiberTrajectory::reset()
{
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesFiberTrajectory::update()
{
    
}

/**
 * Copy the trajectory display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void 
DisplayPropertiesFiberTrajectory::copyDisplayProperties(const int32_t sourceTabIndex,
                                                     const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    
    m_displayStatusInTab[targetTabIndex] = m_displayStatusInTab[sourceTabIndex];
    m_displayModeInTab[targetTabIndex] = m_displayModeInTab[sourceTabIndex];
    m_proportionStreamlineInTab[targetTabIndex] = m_proportionStreamlineInTab[sourceTabIndex];
    m_maximumProportionOpacityInTab[targetTabIndex] = m_maximumProportionOpacityInTab[sourceTabIndex];
    m_minimumProportionOpacityInTab[targetTabIndex] = m_minimumProportionOpacityInTab[sourceTabIndex];
}

/**
 * @return  Display status of trajectory.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesFiberTrajectory::isDisplayed(const DisplayGroupEnum::Enum  displayGroup,
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
 * Set the display status for trajectory for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesFiberTrajectory::setDisplayed(const DisplayGroupEnum::Enum  displayGroup,
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
DisplayPropertiesFiberTrajectory::getDisplayGroupForTab(const int32_t browserTabIndex) const
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
DisplayPropertiesFiberTrajectory::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The display mode.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
FiberTrajectoryDisplayModeEnum::Enum
DisplayPropertiesFiberTrajectory::getDisplayMode(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayModeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayModeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_displayModeInTab[tabIndex];
    }
    return m_displayModeInDisplayGroup[displayGroup];    
}

/**
 * Set the display mode to the given value.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayMode
 *     New value for display mode.
 */
void
DisplayPropertiesFiberTrajectory::setDisplayMode(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 const FiberTrajectoryDisplayModeEnum::Enum displayMode)
{
    CaretAssertArrayIndex(m_displayModeInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayModeInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_displayModeInTab[tabIndex] = displayMode;
    }
    else {
        m_displayModeInDisplayGroup[displayGroup] = displayMode;
    }
}

/**
 * @return The proportion streamline count
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
int32_t
DisplayPropertiesFiberTrajectory::getProportionStreamline(const DisplayGroupEnum::Enum  displayGroup,
                                                 const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_proportionStreamlineInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_proportionStreamlineInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_proportionStreamlineInTab[tabIndex];
    }
    return m_proportionStreamlineInDisplayGroup[displayGroup];
}

/**
 * Set the proportion streamline count.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param pointSize
 *     New value for below limit.
 */
void
DisplayPropertiesFiberTrajectory::setProportionStreamline(const DisplayGroupEnum::Enum  displayGroup,
                                                 const int32_t tabIndex,
                                                 const int32_t proportionStreamline)
{
    CaretAssertArrayIndex(m_proportionStreamlineInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_proportionStreamlineInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_proportionStreamlineInTab[tabIndex] = proportionStreamline;
    }
    else {
        m_proportionStreamlineInDisplayGroup[displayGroup] = proportionStreamline;
    }
}

/**
 * @return The proporation maximum opacity.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberTrajectory::getProportionMaximumOpacity(const DisplayGroupEnum::Enum  displayGroup,
                                                const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_maximimProportionOpacityInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_maximumProportionOpacityInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_maximumProportionOpacityInTab[tabIndex];
    }
    return m_maximimProportionOpacityInDisplayGroup[displayGroup];
}

/**
 * Set the proporation maximum opacity.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param minimumMagnitude
 *     New value for minimum magnitude.
 */
void
DisplayPropertiesFiberTrajectory::setProportionMaximumOpacity(const DisplayGroupEnum::Enum  displayGroup,
                                                const int32_t tabIndex,
                                                const float minimumMagnitude)
{
    CaretAssertArrayIndex(m_maximimProportionOpacityInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_maximumProportionOpacityInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_maximumProportionOpacityInTab[tabIndex] = minimumMagnitude;
    }
    else {
        m_maximimProportionOpacityInDisplayGroup[displayGroup] = minimumMagnitude;
    }
}

/**
 * @return The proporation minimum opacity.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
float
DisplayPropertiesFiberTrajectory::getProportionMinimumOpacity(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_minimumProportionOpacityInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_minimumProportionOpacityInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_minimumProportionOpacityInTab[tabIndex];
    }
    return m_minimumProportionOpacityInDisplayGroup[displayGroup];
}

/**
 * Set the proporation minimum opacity.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param lengthMultiplier
 *     New value for length multiplier
 */
void
DisplayPropertiesFiberTrajectory::setProportionMinimumOpacity(const DisplayGroupEnum::Enum  displayGroup,
                                                       const int32_t tabIndex,
                                                       const float lengthMultiplier)
{
    CaretAssertArrayIndex(m_minimumProportionOpacityInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_minimumProportionOpacityInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_minimumProportionOpacityInTab[tabIndex] = lengthMultiplier;
    }
    else {
        m_minimumProportionOpacityInDisplayGroup[displayGroup] = lengthMultiplier;
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
DisplayPropertiesFiberTrajectory::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesFiberTrajectory",
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
DisplayPropertiesFiberTrajectory::restoreFromScene(const SceneAttributes* sceneAttributes,
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

