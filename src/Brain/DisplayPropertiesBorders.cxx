
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

#define __DISPLAY_PROPERTIES_BORDERS_DECLARE__
#include "DisplayPropertiesBorders.h"
#undef __DISPLAY_PROPERTIES_BORDERS_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"

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
 * @param brain
 *    Brain holds these display properties.
 */
DisplayPropertiesBorders::DisplayPropertiesBorders(Brain* brain)
: DisplayProperties(brain)
{
    const float defaultPointSize = 2.0;
    const float defaultLineSize  = 1.0;
    const BorderDrawingTypeEnum::Enum defaultDrawingType = BorderDrawingTypeEnum::DRAW_AS_POINTS_SPHERES;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = false;
        m_contralateralDisplayStatusInTab[i] = false;
        m_lineWidthInTab[i] = defaultLineSize;
        m_pointSizeInTab[i] = defaultPointSize;
        m_drawingTypeInTab[i] = defaultDrawingType;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = false;
        m_contralateralDisplayStatusInDisplayGroup[i] = false;
        m_lineWidthInDisplayGroup[i]  = defaultLineSize;
        m_pointSizeInDisplayGroup[i] = defaultPointSize;
        m_drawingTypeInDisplayGroup[i] = defaultDrawingType;
    }
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
DisplayPropertiesBorders::saveToScene(const SceneAttributes& sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesBorders",
                                            1);
    
    switch (sceneAttributes.getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
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
DisplayPropertiesBorders::restoreFromScene(const SceneAttributes& sceneAttributes,
                        const SceneClass& sceneClass)
{
    switch (sceneAttributes.getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }    
}

