
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
 */
DisplayPropertiesFoci::DisplayPropertiesFoci()
: DisplayProperties()
{
    const CaretColorEnum::Enum defaultColor = CaretColorEnum::BLACK;

    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_pasteOntoSurfaceInTab[i] = false;
        m_displayStatusInTab[i] = false;
        m_contralateralDisplayStatusInTab[i] = false;
        m_fociSizeInTab[i] = 4.0;
        m_coloringTypeInTab[i] = FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME;
        m_drawingTypeInTab[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
        m_standardColorTypeInTab[i] = defaultColor;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_pasteOntoSurfaceInDisplayGroup[i] = false;
        m_displayStatusInDisplayGroup[i] = false;
        m_contralateralDisplayStatusInDisplayGroup[i] = false;
        m_fociSizeInDisplayGroup[i] = 4.0;
        m_coloringTypeInDisplayGroup[i] = FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME;
        m_drawingTypeInDisplayGroup[i] = FociDrawingTypeEnum::DRAW_AS_SQUARES;
        m_standardColorTypeInDisplayGroup[i] = defaultColor;
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
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<FeatureColoringTypeEnum,FeatureColoringTypeEnum::Enum>("m_coloringTypeInTab", 
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
    m_sceneAssistant->addArray<FeatureColoringTypeEnum, FeatureColoringTypeEnum::Enum>("m_coloringTypeInDisplayGroup",
                                                                                 m_coloringTypeInDisplayGroup,
                                                                                 DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                                 FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME);
    
    m_sceneAssistant->addArray<FociDrawingTypeEnum, FociDrawingTypeEnum::Enum>("m_drawingTypeInDisplayGroup",
                                                                               m_drawingTypeInDisplayGroup,
                                                                               DisplayGroupEnum::NUMBER_OF_GROUPS,
                                                                               FociDrawingTypeEnum::DRAW_AS_SQUARES);
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
    m_standardColorTypeInTab[targetTabIndex] = m_standardColorTypeInTab[sourceTabIndex];
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
FeatureColoringTypeEnum::Enum 
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
DisplayPropertiesFoci::getStandardColorType(const DisplayGroupEnum::Enum displayGroup,
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
DisplayPropertiesFoci::setStandardColorType(const DisplayGroupEnum::Enum displayGroup,
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
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
}

