
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

#define __DISPLAY_PROPERTIES_FOCI_DECLARE__
#include "DisplayPropertiesFoci.h"
#undef __DISPLAY_PROPERTIES_FOCI_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::DisplayPropertiesFoci 
 * \brief Contains display properties for foci.
 */

/**
 * Constructor.
 * @param brain
 *    Brain holds these display properties.
 */
DisplayPropertiesFoci::DisplayPropertiesFoci(Brain* brain)
: DisplayProperties(brain)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_pasteOntoSurfaceInTab[i] = false;
        m_displayStatusInTab[i] = false;
        m_contralateralDisplayStatusInTab[i] = false;
        m_fociSizeInTab[i] = 4.0;
        m_coloringTypeInTab[i] = FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME;
        m_drawingTypeInTab[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_pasteOntoSurfaceInDisplayGroup[i] = false;
        m_displayStatusInDisplayGroup[i] = false;
        m_contralateralDisplayStatusInDisplayGroup[i] = false;
        m_fociSizeInDisplayGroup[i] = 4.0;
        m_coloringTypeInDisplayGroup[i] = FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME;
        m_drawingTypeInDisplayGroup[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
    }
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup", 
                                                                                                m_displayGroup);
    m_sceneAssistant->addTabIndexedBooleanArray("m_pasteOntoSurfaceInTab", 
                               m_pasteOntoSurfaceInTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_displayStatusInTab", 
                               m_displayStatusInTab);
    m_sceneAssistant->addTabIndexedBooleanArray("m_contralateralDisplayStatusInTab", 
                               m_contralateralDisplayStatusInTab);
    m_sceneAssistant->addTabIndexedFloatArray("m_fociSizeInTab", 
                               m_fociSizeInTab);
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FociColoringTypeEnum,FociColoringTypeEnum::Enum>("m_coloringTypeInTab", 
                                                                                                        m_coloringTypeInTab);
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FociDrawingTypeEnum,FociDrawingTypeEnum::Enum>("m_drawingTypeInTab", 
                                                                                                      m_drawingTypeInTab);
    
    m_sceneAssistant->addArray("m_pasteOntoSurfaceInDisplayGroup", 
                               m_pasteOntoSurfaceInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_pasteOntoSurfaceInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_displayStatusInDisplayGroup", 
                               m_displayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_displayStatusInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_contralateralDisplayStatusInDisplayGroup", 
                               m_contralateralDisplayStatusInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_contralateralDisplayStatusInDisplayGroup[0]);
    m_sceneAssistant->addArray("m_fociSizeInDisplayGroup", 
                               m_fociSizeInDisplayGroup,
                               DisplayGroupEnum::NUMBER_OF_GROUPS, 
                               m_fociSizeInDisplayGroup[0]);
    m_sceneAssistant->addArray<FociColoringTypeEnum, FociColoringTypeEnum::Enum>("m_coloringTypeInDisplayGroup",
                                                                                 m_coloringTypeInDisplayGroup,
                                                                                 DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                 FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME);
    
    m_sceneAssistant->addArray<FociDrawingTypeEnum, FociDrawingTypeEnum::Enum>("m_drawingTypeInDisplayGroup",
                                                                               m_drawingTypeInDisplayGroup,
                                                                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                               FociDrawingTypeEnum::DRAW_AS_SQUARES);
}

/**
 * Destructor.
 */
DisplayPropertiesFoci::~DisplayPropertiesFoci()
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
DisplayPropertiesFoci::copyDisplayProperties(const int32_t sourceTabIndex,
                                                 const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
    
    m_coloringTypeInTab[targetTabIndex]     = m_coloringTypeInTab[sourceTabIndex];
    m_contralateralDisplayStatusInTab[targetTabIndex] = m_contralateralDisplayStatusInTab[sourceTabIndex];
    m_displayStatusInTab[targetTabIndex]    = m_displayStatusInTab[sourceTabIndex];
    m_drawingTypeInTab[targetTabIndex]      = m_drawingTypeInTab[sourceTabIndex];
    m_fociSizeInTab[targetTabIndex]         = m_fociSizeInTab[sourceTabIndex];
    m_pasteOntoSurfaceInTab[targetTabIndex] = m_pasteOntoSurfaceInTab[sourceTabIndex];
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesFoci::reset()
{
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        m_displayStatus[i] = true;
//        m_contralateralDisplayStatus[i] = false;
//        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_ALL_WINDOWS;
//        m_pasteOntoSurface[i] = false;
//    }
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesFoci::update()
{
    
}

/**
 * @return  Display status of foci.
 * @param displayGroup
 *     Display group.
 */
bool 
DisplayPropertiesFoci::isDisplayed(const DisplayGroupEnum::Enum displayGroup,
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
 * Set the display status for foci.
 * @param displayGroup
 *     Display group.
 * @param displayStatus
 *    New status.
 */
void 
DisplayPropertiesFoci::setDisplayed(const DisplayGroupEnum::Enum displayGroup,
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
 * @return  Contralateral display status of foci.
 * @param displayGroup
 *     Display group.
 * @param browserTabIndex
 *    Index of browser tab.
 */
bool 
DisplayPropertiesFoci::isContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                                const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_contralateralDisplayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_contralateralDisplayStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_contralateralDisplayStatusInTab[tabIndex];
    }
    return m_contralateralDisplayStatusInDisplayGroup[displayGroup];
}

/**
 * Set the contralateral display status for foci.
 * @param displayGroup
 *     Display group.
 * @param contralateralDisplayStatus
 *    New status.
 */
void 
DisplayPropertiesFoci::setContralateralDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                    const bool contralateralDisplayStatus)
{
    CaretAssertArrayIndex(m_contralateralDisplayStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_contralateralDisplayStatusInTab, 
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
DisplayPropertiesFoci::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(m_displayGroup, 
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
DisplayPropertiesFoci::setDisplayGroupForTab(const int32_t browserTabIndex,
                                          const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(m_displayGroup, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * @return The foci size.
 * @param displayGroup
 *     Display group.
 */
float 
DisplayPropertiesFoci::getFociSize(const DisplayGroupEnum::Enum displayGroup,
                                   const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_fociSizeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fociSizeInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex); 
        return m_fociSizeInTab[tabIndex];
    }
    return m_fociSizeInDisplayGroup[displayGroup];
}

/**
 * Set the foci size to the given value.
 * @param displayGroup
 *     Display group.
 * @param fociSize
 *     New value for foci size.
 */
void 
DisplayPropertiesFoci::setFociSize(const DisplayGroupEnum::Enum displayGroup,
                                   const int32_t tabIndex,
                                   const float fociSize)
{
    CaretAssertArrayIndex(m_fociSizeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_fociSizeInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        m_fociSizeInTab[tabIndex] = fociSize;
    }
    else {
        m_fociSizeInDisplayGroup[displayGroup] = fociSize;
    }
}

/**
 * @return The coloring type.
 * @param displayGroup
 *     Display group.
 */
FociColoringTypeEnum::Enum 
DisplayPropertiesFoci::getColoringType(const DisplayGroupEnum::Enum displayGroup,
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
DisplayPropertiesFoci::setColoringType(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex,
                                       const FociColoringTypeEnum::Enum coloringType)
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
 * @param displayGroup
 *     Display group.
 * @return The drawing type.
 */
FociDrawingTypeEnum::Enum 
DisplayPropertiesFoci::getDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_drawingTypeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_drawingTypeInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_drawingTypeInTab[tabIndex];
    }
    return m_drawingTypeInDisplayGroup[displayGroup];
}

/**
 * Set the drawing type to the given value.
 * @param displayGroup
 *     Display group.
 * @param drawingType
 *     New value for drawing type.
 */
void 
DisplayPropertiesFoci::setDrawingType(const DisplayGroupEnum::Enum displayGroup,
                                      const int32_t tabIndex,
                                      const FociDrawingTypeEnum::Enum drawingType)
{
    CaretAssertArrayIndex(m_drawingTypeInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_drawingTypeInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        m_drawingTypeInTab[tabIndex] = drawingType;
    }
    else {
        m_drawingTypeInDisplayGroup[displayGroup] = drawingType;
    }
}

/**
 * Set paste onto surface so the foci are placed directly on the surface.
 * @param displayGroup
 *     Display group.
 * @param enabled
 *   True if pasting foci onto surface is enabled.
 */
void 
DisplayPropertiesFoci::setPasteOntoSurface(const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const bool enabled)
{
    CaretAssertArrayIndex(m_pasteOntoSurfaceInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_pasteOntoSurfaceInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex); 
        m_pasteOntoSurfaceInTab[tabIndex] = enabled;
    }
    else {
        m_pasteOntoSurfaceInDisplayGroup[displayGroup] = enabled; 
    }
}

/**
 * @param displayGroup
 *     Display group.
 * @return True if foci are pasted onto surface.
 */
bool 
DisplayPropertiesFoci::isPasteOntoSurface(const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex) const
{
   CaretAssertArrayIndex(m_pasteOntoSurfaceInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_pasteOntoSurfaceInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);  
        return m_pasteOntoSurfaceInTab[tabIndex];
    }
    return m_pasteOntoSurfaceInDisplayGroup[displayGroup];
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
DisplayPropertiesFoci::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesFoci",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
//    sceneClass->addEnumerateTypeArrayForTabIndices<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup", 
//                                         m_displayGroup, 
//                                         tabIndices);
//    sceneClass->addEnumerateTypeArrayForTabIndices<FociColoringTypeEnum,FociColoringTypeEnum::Enum>("m_coloringTypeInTab", 
//                                                                                            m_coloringTypeInTab, 
//                                                                                            tabIndices);
//    sceneClass->addEnumerateTypeArrayForTabIndices<FociDrawingTypeEnum,FociDrawingTypeEnum::Enum>("m_drawingTypeInTab", 
//                                                                                            m_drawingTypeInTab, 
//                                                                                            tabIndices);

    
    
//    AString tabStringArray[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        tabStringArray[i] = DisplayGroupEnum::toName(m_displayGroup[i]);
//    }
//    sceneClass->addEnumeratedTypeArray("m_displayGroup", 
//                                       tabStringArray, 
//                                       BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
//    
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        tabStringArray[i] = FociColoringTypeEnum::toName(m_coloringTypeInTab[i]);
//    }
//    sceneClass->addEnumeratedTypeArray("m_coloringTypeInTab", 
//                                       tabStringArray, 
//                                       BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
//
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        tabStringArray[i] = FociDrawingTypeEnum::toName(m_drawingTypeInTab[i]);
//    }
//    sceneClass->addEnumeratedTypeArray("m_drawingTypeInTab", 
//                                       tabStringArray, 
//                                       BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    
//    sceneClass->addEnumerateTypeArray<FociColoringTypeEnum,FociColoringTypeEnum::Enum>("m_coloringTypeInDisplayGroup", 
//                                                                                       m_coloringTypeInDisplayGroup, 
//                                                                                       DisplayGroupEnum::NUMBER_OF_GROUPS);
//    sceneClass->addEnumerateTypeArray<FociDrawingTypeEnum,FociDrawingTypeEnum::Enum>("m_drawingTypeInDisplayGroup", 
//                                                                                       m_drawingTypeInDisplayGroup, 
//                                                                                       DisplayGroupEnum::NUMBER_OF_GROUPS);
    
//    AString displayGroupStringArray[DisplayGroupEnum::NUMBER_OF_GROUPS];
//    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
//        displayGroupStringArray[i] = FociColoringTypeEnum::toName(m_coloringTypeInDisplayGroup[i]);
//    }
//    sceneClass->addEnumeratedTypeArray("m_coloringTypeInDisplayGroup", 
//                                       displayGroupStringArray, 
//                                       DisplayGroupEnum::NUMBER_OF_GROUPS);
//    
//    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
//        displayGroupStringArray[i] = FociDrawingTypeEnum::toName(m_drawingTypeInDisplayGroup[i]);
//    }
//    sceneClass->addEnumeratedTypeArray("m_drawingTypeInDisplayGroup", 
//                                       displayGroupStringArray, 
//                                       DisplayGroupEnum::NUMBER_OF_GROUPS);
    
    switch (sceneAttributes->getSceneType()) {
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
DisplayPropertiesFoci::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    
//    const AString defaultDrawingTypeString = FociDrawingTypeEnum::toName(FociDrawingTypeEnum::DRAW_AS_SQUARES);
//    const AString defaultColoringTypeString = FociColoringTypeEnum::toName(FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME);
//    
//    AString tabStringArray[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
//    sceneClass->getEnumeratedTypeArrayValue("m_displayGroup", 
//                                           tabStringArray, 
//                                           BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
//                                           DisplayGroupEnum::toName(DisplayGroupEnum::getDefaultValue()));
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        bool isValid = false;
//        m_displayGroup[i] = DisplayGroupEnum::fromName(tabStringArray[i],
//                                                                &isValid);
//        if (isValid == false) {
//            m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
//        }
//    }
//    
//    sceneClass->getEnumeratedTypeArrayValue("m_coloringTypeInTab", 
//                                           tabStringArray, 
//                                           BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
//                                           defaultColoringTypeString);
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        bool isValid = false;
//        m_coloringTypeInTab[i] = FociColoringTypeEnum::fromName(tabStringArray[i],
//                                                       &isValid);
//        if (isValid == false) {
//            m_coloringTypeInTab[i] = FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME;
//        }
//    }
//    
//    sceneClass->getEnumeratedTypeArrayValue("m_drawingTypeInTab", 
//                                           tabStringArray, 
//                                           BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
//                                           defaultDrawingTypeString);
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        bool isValid = false;
//        m_drawingTypeInTab[i] = FociDrawingTypeEnum::fromName(tabStringArray[i],
//                                                                &isValid);
//        if (isValid == false) {
//            m_drawingTypeInTab[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
//        }
//    }
//    sceneClass->getEnumerateTypeArray<FociColoringTypeEnum,FociColoringTypeEnum::Enum>("m_coloringTypeInDisplayGroup", 
//                                         m_coloringTypeInDisplayGroup, 
//                                         DisplayGroupEnum::NUMBER_OF_GROUPS, 
//                                         FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME);
//    
//    sceneClass->getEnumerateTypeArray<FociDrawingTypeEnum,FociDrawingTypeEnum::Enum>("m_drawingTypeInDisplayGroup", 
//                                                                                       m_drawingTypeInDisplayGroup, 
//                                                                                       DisplayGroupEnum::NUMBER_OF_GROUPS, 
//                                                                                       FociDrawingTypeEnum::DRAW_AS_SQUARES);
    
    
//    AString displayGroupStringArray[DisplayGroupEnum::NUMBER_OF_GROUPS];
//    sceneClass->getEnumeratedTypeArrayValue("m_coloringTypeInDisplayGroup", 
//                                           displayGroupStringArray, 
//                                           DisplayGroupEnum::NUMBER_OF_GROUPS, 
//                                           defaultColoringTypeString);
//    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
//        bool isValid = false;
//        m_coloringTypeInDisplayGroup[i] = FociColoringTypeEnum::fromName(displayGroupStringArray[i],
//                                                                         &isValid);
//        if (isValid == false) {
//            m_coloringTypeInDisplayGroup[i] = FociColoringTypeEnum::FOCI_COLORING_TYPE_NAME;
//        }
//    }
//    
//    sceneClass->getEnumeratedTypeArrayValue("m_drawingTypeInDisplayGroup", 
//                                           displayGroupStringArray, 
//                                           DisplayGroupEnum::NUMBER_OF_GROUPS, 
//                                           defaultColoringTypeString);
//    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
//        bool isValid = false;
//        m_drawingTypeInDisplayGroup[i] = FociDrawingTypeEnum::fromName(displayGroupStringArray[i],
//                                                                         &isValid);
//        if (isValid == false) {
//            m_drawingTypeInDisplayGroup[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
//        }
//    }
    
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
}

