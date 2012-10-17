
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

#include <cmath>

#define __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__
#include "DisplayPropertiesFiberOrientation.h"
#undef __DISPLAY_PROPERTIES_FIBER_ORIENTATION_DECLARE__

#include "Brain.h"
#include "CaretAssert.h"
#include "CiftiFiberOrientationAdapter.h"
#include "ConnectivityLoaderFile.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeFile.h"

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
    m_sceneAssistant->addArray("m_displaySphereOrientationsInTab",
                               m_displaySphereOrientationsInTab,
                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                               m_displaySphereOrientationsInTab[0]);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FiberOrientationColoringTypeEnum, FiberOrientationColoringTypeEnum::Enum>("m_fiberColoringTypeInTab",
                                                                                                           m_fiberColoringTypeInTab);
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FiberOrientationSymbolTypeEnum, FiberOrientationSymbolTypeEnum::Enum>("m_fiberSymbolTypeInTab",
                                                                                                                                 m_fiberSymbolTypeInTab);
    m_sampleVolumesLoadAttemptValid = false;
    m_sampleVolumesValid = false;
    for (int32_t i = 0; i < 3; i++) {
        m_sampleMagnitudeVolumes[i] = NULL;
        m_sampleThetaVolumes[i] = NULL;
        m_samplePhiVolumes[i] = NULL;
    }
    
    m_lastIdentificationValid = false;
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
}

/**
 * Destructor.
 */
DisplayPropertiesFiberOrientation::~DisplayPropertiesFiberOrientation()
{    
    EventManager::get()->removeAllEventsFromListener(this);
    resetPrivate();
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesFiberOrientation::reset()
{
    resetPrivate();
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesFiberOrientation::resetPrivate()
{
    m_sampleVolumesLoadAttemptValid = false;
    m_sampleVolumesValid = false;
    for (int32_t i = 0; i < 3; i++) {
        if (m_sampleMagnitudeVolumes[i] != NULL) {
            delete m_sampleMagnitudeVolumes[i];
            m_sampleMagnitudeVolumes[i] = NULL;
        }
        if (m_sampleThetaVolumes[i] != NULL) {
            delete m_sampleThetaVolumes[i];
            m_sampleThetaVolumes[i] = NULL;
        }
        if (m_samplePhiVolumes[i] != NULL) {
            delete m_samplePhiVolumes[i];
            m_samplePhiVolumes[i] = NULL;
        }
    }
    m_lastIdentificationValid = false;
}



/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesFiberOrientation::update()
{
    
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
DisplayPropertiesFiberOrientation::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idEvent = dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idEvent);
        
        const float* xyz = idEvent->getXYZ();
        if (xyz != NULL) {
            m_lastIdentificationValid = true;
            m_lastIdentificationXYZ[0] = xyz[0];
            m_lastIdentificationXYZ[1] = xyz[1];
            m_lastIdentificationXYZ[2] = xyz[2];
//            std::cout << "XYZ is: " << qPrintable(AString::fromNumbers(xyz, 3, ",")) << std::endl;
        }
    }
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

/**
 * Get the fiber orientation vectors for display on a sphere.
 * @param xVectors
 *    Vectors for X-orientation.
 * @param yVectors
 *    Vectors for Y-orientation.
 * @param zVectors
 *    Vectors for Z-orientation.
 * @param fiberOrientation
 *    The nearby fiber orientation
 * @param errorMessageOut
 *    Will contain any error messages.
 *    This error message will only be set in some cases when there is an
 *    error.
 * @return
 *    True if data is valid, else false.  
 */
bool
DisplayPropertiesFiberOrientation::getSphericalOrientationVectors(std::vector<OrientationVector>& xVectors,
                                                                  std::vector<OrientationVector>& yVectors,
                                                                  std::vector<OrientationVector>& zVectors,
                                                                  FiberOrientation* &fiberOrientationOut,
                                                                  AString& errorMessageOut)
{
    errorMessageOut = "";
    fiberOrientationOut = NULL;
    
    if (m_lastIdentificationValid) {
        if (loadSphericalOrientationVolumes(errorMessageOut) == false) {
            return false;
        }

        if (m_brain->getNumberOfConnectivityFiberOrientationFiles() > 0) {
            ConnectivityLoaderFile* clf = m_brain->getConnectivityFiberOrientationFile(0);
            CiftiFiberOrientationAdapter* cfoa = clf->getFiberOrientationAdapter();
            FiberOrientation* nearestFiberOrientation =
                cfoa->getFiberOrientationNearestCoordinate(m_lastIdentificationXYZ, 3.0);
            if (nearestFiberOrientation != NULL) {
                fiberOrientationOut = nearestFiberOrientation;
            }
        }
        
        int64_t ijk[3];
        m_sampleThetaVolumes[0]->enclosingVoxel(m_lastIdentificationXYZ,
                                                    ijk);
        if (m_sampleThetaVolumes[0]->indexValid(ijk)) {
            std::vector<int64_t> dims;
            m_sampleThetaVolumes[0]->getDimensions(dims);
            
            const int64_t numberOfOrientations = dims[3];
            xVectors.resize(numberOfOrientations);
            yVectors.resize(numberOfOrientations);
            zVectors.resize(numberOfOrientations);
            
            for (int32_t iAxis = 0; iAxis < 3; iAxis++) {
                for (int64_t iOrient = 0; iOrient < numberOfOrientations; iOrient++) {
                    const float theta = m_sampleThetaVolumes[iAxis]->getValue(ijk[0],
                                                                              ijk[1],
                                                                              ijk[2],
                                                                              iOrient,
                                                                              0);
                    const float phi = m_samplePhiVolumes[iAxis]->getValue(ijk[0],
                                                                              ijk[1],
                                                                              ijk[2],
                                                                              iOrient,
                                                                              0);
                    
                    const float magnitude = m_sampleMagnitudeVolumes[iAxis]->getValue(ijk[0],
                                                                                      ijk[1],
                                                                                      ijk[2],
                                                                                      iOrient,
                                                                                      0);
                    
                    switch (iAxis) {
                        case 0:
                        {
                            OrientationVector& ov = xVectors[iOrient];
                            ov.vector[0] = -std::sin(theta) * std::cos(phi);
                            ov.vector[1] =  std::sin(theta) * std::sin(phi);
                            ov.vector[2] =  std::cos(theta);
                            ov.magnitude = magnitude;
                            ov.setColor();
                        }
                            break;
                        case 1:
                        {
                            OrientationVector& ov = yVectors[iOrient];
                            ov.vector[0] = -std::sin(theta) * std::cos(phi);
                            ov.vector[1] =  std::sin(theta) * std::sin(phi);
                            ov.vector[2] =  std::cos(theta);
                            ov.magnitude = magnitude;
                            ov.setColor();
                        }
                            break;
                        case 2:
                        {
                            OrientationVector& ov = zVectors[iOrient];
                            ov.vector[0] = -std::sin(theta) * std::cos(phi);
                            ov.vector[1] =  std::sin(theta) * std::sin(phi);
                            ov.vector[2] =  std::cos(theta);
                            ov.magnitude = magnitude;
                            ov.setColor();
                        }
                            break;
                    }
                }
            }
            
            return true;
        }
    }
    
    return false;
}

/**
 * Get the volumes containing the spherical orienations.
 *
 * @param magnitudeVolumesOut
 *    The volumes containing the magnitudes.
 * @param phiAngleVolumesOut
 *    The volumes containing the phi angles.
 * @param thetaAngleVolumesOut
 *    The volumes containing the theta angles.
 *
 */
bool
DisplayPropertiesFiberOrientation::loadSphericalOrientationVolumes(AString& errorMessageOut)
{
    errorMessageOut = "";

    FileInformation specFileInfo(m_brain->getSpecFileName());
    const AString directoryName = specFileInfo.getPathName();
    
    if (m_sampleVolumesValid == false) {
        if (m_sampleVolumesLoadAttemptValid == false) {
            const AString filePrefix = "merged_";
            const AString fileSuffix = "samples.nii.gz";
            
            std::vector<VolumeFile*> allVolumes;
            
            for (int32_t i = 0; i < 3; i++) {
                m_sampleMagnitudeVolumes[i] = new VolumeFile();
                m_samplePhiVolumes[i]       = new VolumeFile();
                m_sampleThetaVolumes[i]     = new VolumeFile();
                
                const AString fileNumber = AString::number(i + 1);
                
                try {
                    const AString magFileName = (filePrefix
                                                 + "f"
                                                 + fileNumber
                                                 + fileSuffix);
                    FileInformation magFileInfo(directoryName,
                                            magFileName);
                    const AString magFilePath = magFileInfo.getFilePath();
                    m_sampleMagnitudeVolumes[i]->readFile(magFilePath);
                    allVolumes.push_back(m_sampleMagnitudeVolumes[i]);
                }
                catch (const DataFileException& dfe) {
                    if (errorMessageOut.isEmpty() == false) {
                        errorMessageOut += "\n";
                    }
                    errorMessageOut += dfe.whatString();
                }
                
                try {
                    const AString phiFileName = (filePrefix
                                                 + "ph"
                                                 + fileNumber
                                                 + fileSuffix);
                    FileInformation phiFileInfo(directoryName,
                                                phiFileName);
                    const AString phiFilePath = phiFileInfo.getFilePath();
                    m_samplePhiVolumes[i]->readFile(phiFilePath);
                    allVolumes.push_back(m_samplePhiVolumes[i]);
                }
                catch (const DataFileException& dfe) {
                    if (errorMessageOut.isEmpty() == false) {
                        errorMessageOut += "\n";
                    }
                    errorMessageOut += dfe.whatString();
                }
                
                try {
                    const AString thetaFileName = (filePrefix
                                                   + "th"
                                                   + fileNumber
                                                   + fileSuffix);
                    FileInformation thetaFileInfo(directoryName,
                                                thetaFileName);
                    const AString thetaFilePath = thetaFileInfo.getFilePath();
                    m_sampleThetaVolumes[i]->readFile(thetaFilePath);
                    allVolumes.push_back(m_sampleThetaVolumes[i]);
                }
                catch (const DataFileException& dfe) {
                    if (errorMessageOut.isEmpty() == false) {
                        errorMessageOut += "\n";
                    }
                    errorMessageOut += dfe.whatString();
                }
            }
            
            if (errorMessageOut.isEmpty()) {
                std::vector<int64_t> dims;
                for (std::vector<VolumeFile*>::iterator iter = allVolumes.begin();
                     iter != allVolumes.end();
                     iter++) {
                    VolumeFile* vf = *iter;
                    std::vector<int64_t> volDims;
                    vf->getDimensions(volDims);
                    
                    if (dims.empty()) {
                        dims = volDims;
                    }
                    else if (dims != volDims) {
                        errorMessageOut += "ERROR: Sample volumes have mis-matched dimensions";
                    }
                }
                m_sampleVolumesValid = true;
            }
            
            m_sampleVolumesLoadAttemptValid = true;
        }
    }
    if (m_sampleVolumesValid) {
        return true;
    }
    
    return false;
}



