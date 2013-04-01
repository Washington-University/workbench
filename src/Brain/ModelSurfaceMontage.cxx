/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <algorithm>
#include <cmath>

#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "ModelSurfaceMontage.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "OverlaySet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePrimitive.h"
#include "SurfaceSelectionModel.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this controller.
 *
 */
ModelSurfaceMontage::ModelSurfaceMontage(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE,
                         brain)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_leftSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT);
        m_leftSecondSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT);
        m_rightSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT);
        m_rightSecondSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT);
        m_leftEnabled[i] = true;
        m_rightEnabled[i] = true;
        m_firstSurfaceEnabled[i] = false;
        m_secondSurfaceEnabled[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySet[i] = new OverlaySet(this);
    }
}

/**
 * Destructor
 */
ModelSurfaceMontage::~ModelSurfaceMontage()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_overlaySet[i];
        delete m_leftSurfaceSelectionModel[i];
        delete m_leftSecondSurfaceSelectionModel[i];
        delete m_rightSurfaceSelectionModel[i];
        delete m_rightSecondSurfaceSelectionModel[i];
    }    
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelSurfaceMontage::receiveEvent(Event* /*event*/)
{
}

void
ModelSurfaceMontage::initializeSurfaces()
{
    Surface* leftAnatSurface = NULL;
    BrainStructure* leftBrainStructure = m_brain->getBrainStructure(StructureEnum::CORTEX_LEFT,
                                                                    false);
    if (leftBrainStructure != NULL) {
        leftAnatSurface = leftBrainStructure->getVolumeInteractionSurface();
    }
    
    Surface* rightAnatSurface = NULL;
    BrainStructure* rightBrainStructure = m_brain->getBrainStructure(StructureEnum::CORTEX_RIGHT,
                                                                    false);
    if (rightBrainStructure != NULL) {
        rightAnatSurface = rightBrainStructure->getVolumeInteractionSurface();
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_leftSurfaceSelectionModel[i]->setSurfaceToType(SurfaceTypeEnum::ANATOMICAL);
        if (leftAnatSurface != NULL) {
            m_leftSurfaceSelectionModel[i]->setSurface(leftAnatSurface);
        }
        

        m_leftSecondSurfaceSelectionModel[i]->setSurfaceToType(SurfaceTypeEnum::INFLATED,
                                                               SurfaceTypeEnum::VERY_INFLATED);
        
        m_rightSurfaceSelectionModel[i]->setSurfaceToType(SurfaceTypeEnum::ANATOMICAL);
        if (rightAnatSurface != NULL) {
            m_rightSurfaceSelectionModel[i]->setSurface(rightAnatSurface);
        }

        m_rightSecondSurfaceSelectionModel[i]->setSurfaceToType(SurfaceTypeEnum::INFLATED,
                                                               SurfaceTypeEnum::VERY_INFLATED);
        m_leftEnabled[i] = true;
        m_rightEnabled[i] = true;
        m_firstSurfaceEnabled[i] = false;
        m_secondSurfaceEnabled[i] = true;
    }
}

/**
 * @return Is  enabled?
 */
bool 
ModelSurfaceMontage::isLeftEnabled(const int tabIndex) const
{
    CaretAssertArrayIndex(m_leftEnabled, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_leftEnabled[tabIndex];
}

/**
 * Set  enabled
 * @param tabIndex
 *    Index of tab.
 * @param enabled
 *    New status
 */
void 
ModelSurfaceMontage::setLeftEnabled(const int tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_leftEnabled, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);

    m_leftEnabled[tabIndex] = enabled;
}

/**
 * @return Is  enabled?
 */
bool
ModelSurfaceMontage::isRightEnabled(const int tabIndex) const
{
    CaretAssertArrayIndex(m_rightEnabled,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_rightEnabled[tabIndex];
}

/**
 * Set  enabled
 * @param tabIndex
 *    Index of tab.
 * @param enabled
 *    New status
 */
void
ModelSurfaceMontage::setRightEnabled(const int tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_rightEnabled,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    
    m_rightEnabled[tabIndex] = enabled;
}

/**
 * @return Is  enabled?
 */
bool
ModelSurfaceMontage::isFirstSurfaceEnabled(const int tabIndex) const
{
    CaretAssertArrayIndex(m_firstSurfaceEnabled,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_firstSurfaceEnabled[tabIndex];
}

/**
 * Set  enabled
 * @param tabIndex
 *    Index of tab.
 * @param enabled
 *    New status
 */
void
ModelSurfaceMontage::setFirstSurfaceEnabled(const int tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_firstSurfaceEnabled,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    
    m_firstSurfaceEnabled[tabIndex] = enabled;
}

/**
 * @return Is  enabled?
 */
bool
ModelSurfaceMontage::isSecondSurfaceEnabled(const int tabIndex) const
{
    CaretAssertArrayIndex(m_secondSurfaceEnabled,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_secondSurfaceEnabled[tabIndex];
}

/**
 * Set  enabled
 * @param tabIndex
 *    Index of tab.
 * @param enabled
 *    New status
 */
void
ModelSurfaceMontage::setSecondSurfaceEnabled(const int tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_secondSurfaceEnabled,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    
    m_secondSurfaceEnabled[tabIndex] = enabled;
}

/**
 * @param tabIndex
 *    Index of tab.
 * @return the left surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getLeftSurfaceSelectionModel(const int tabIndex)
{
    CaretAssertArrayIndex(m_leftSurfaceSelectionModel, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_leftSurfaceSelectionModel[tabIndex];
}

/**
 * @param tabIndex
 *    Index of tab.
 * @return the left second surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getLeftSecondSurfaceSelectionModel(const int tabIndex)
{
    CaretAssertArrayIndex(m_leftSecondSurfaceSelectionModel, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_leftSecondSurfaceSelectionModel[tabIndex];
}

/**
 * @param tabIndex
 *    Index of tab.
 * @return the right surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getRightSurfaceSelectionModel(const int tabIndex)
{
    CaretAssertArrayIndex(m_rightSurfaceSelectionModel, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_rightSurfaceSelectionModel[tabIndex];
}

/**
 * @param tabIndex
 *    Index of tab.
 * @return the right second surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getRightSecondSurfaceSelectionModel(const int tabIndex)
{
    CaretAssertArrayIndex(m_rightSecondSurfaceSelectionModel, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_rightSecondSurfaceSelectionModel[tabIndex];
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelSurfaceMontage::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "Surface Montage";
    
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelSurfaceMontage::getNameForBrowserTab() const
{
    AString name = "Montage";
    
    return name;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelSurfaceMontage::getOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(m_overlaySet, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_overlaySet[tabIndex];
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelSurfaceMontage::getOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_overlaySet, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_overlaySet[tabIndex];
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelSurfaceMontage::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySet[i]->initializeOverlays();
    }
}

/**
 * Get a surface for the given struture in the given tab.  Since there
 * may be one surface of the given structure, the returned surface
 * may be different in future calls based upon the surfaces the user
 * has chosen for display.
 *
 * @param structure
 *    Structure for the surface
 * @param windowTabNumber
 *    Tab number of window.
 * @param Pointer to selected surface for given structure or NULL if not available.
 */
Surface* 
ModelSurfaceMontage::getSelectedSurface(const StructureEnum::Enum structure,
                                    const int32_t windowTabNumber)

{
    SurfaceSelectionModel* selectionModel = NULL;
    switch (structure) {
        case StructureEnum::CORTEX_LEFT:
            selectionModel = getLeftSurfaceSelectionModel(windowTabNumber);
            break;
        case StructureEnum::CORTEX_RIGHT:
            selectionModel = getRightSurfaceSelectionModel(windowTabNumber);
            break;
        default:
            break;
    }
    
    Surface* surface = NULL;
    if (selectionModel != NULL) {
        surface = selectionModel->getSurface();
    }
    return surface;
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void 
ModelSurfaceMontage::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    sceneClass->addInteger("montageVersion", 2);
    
    /*
     * Get indices of tabs that are to be saved to scene.
     */ 
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numActiveTabs = static_cast<int32_t>(tabIndices.size()); 
    
    /*
     * Surfaces
     */
    SceneObjectMapIntegerKey* leftSurfaceMap = new SceneObjectMapIntegerKey("m_leftSurfaceSelectionModel",
                                                                  SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* leftSecondSurfaceMap = new SceneObjectMapIntegerKey("m_leftSecondSurfaceSelectionModel",
                                                                SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* rightSurfaceMap = new SceneObjectMapIntegerKey("m_rightSurfaceSelectionModel",
                                                                SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* rightSecondSurfaceMap = new SceneObjectMapIntegerKey("m_rightSecondSurfaceSelectionModel",
                                                                SceneObjectDataTypeEnum::SCENE_CLASS);
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        const AString tabString = ("[" + AString::number(tabIndex) + "]");

        leftSurfaceMap->addClass(tabIndex,
                                 m_leftSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes, 
                                                                                    "m_leftSurfaceSelectionModel" + tabString));
        leftSecondSurfaceMap->addClass(tabIndex,
                                 m_leftSecondSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes, 
                                                                                    "m_leftSecondSurfaceSelectionModel" + tabString));
        rightSurfaceMap->addClass(tabIndex,
                                 m_rightSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes, 
                                                                                    "m_rightSurfaceSelectionModel" + tabString));
        rightSecondSurfaceMap->addClass(tabIndex,
                                 m_rightSecondSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes, 
                                                                                    "m_rightSecondSurfaceSelectionModel" + tabString));
    }
    sceneClass->addChild(leftSurfaceMap);
    sceneClass->addChild(leftSecondSurfaceMap);
    sceneClass->addChild(rightSurfaceMap);
    sceneClass->addChild(rightSecondSurfaceMap);
    
    /*
     * Selections
     */
    SceneObjectMapIntegerKey* rightEnabledMap = new SceneObjectMapIntegerKey("m_rightEnabled",
                                                                       SceneObjectDataTypeEnum::SCENE_BOOLEAN);
    SceneObjectMapIntegerKey* leftEnabledMap = new SceneObjectMapIntegerKey("m_leftEnabled",
                                                                                  SceneObjectDataTypeEnum::SCENE_BOOLEAN);
    SceneObjectMapIntegerKey* firstSurfaceEnabledMap = new SceneObjectMapIntegerKey("m_firstSurfaceEnabled",
                                                                                  SceneObjectDataTypeEnum::SCENE_BOOLEAN);
    SceneObjectMapIntegerKey* secondSurfaceEnabledMap = new SceneObjectMapIntegerKey("m_secondSurfaceEnabled",
                                                                                  SceneObjectDataTypeEnum::SCENE_BOOLEAN);
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        rightEnabledMap->addBoolean(tabIndex,
                                         m_rightEnabled[tabIndex]);
        leftEnabledMap->addBoolean(tabIndex,
                                         m_leftEnabled[tabIndex]);
        firstSurfaceEnabledMap->addBoolean(tabIndex,
                                         m_firstSurfaceEnabled[tabIndex]);
        secondSurfaceEnabledMap->addBoolean(tabIndex,
                                         m_secondSurfaceEnabled[tabIndex]);
    }
    sceneClass->addChild(rightEnabledMap);
    sceneClass->addChild(leftEnabledMap);
    sceneClass->addChild(firstSurfaceEnabledMap);
    sceneClass->addChild(secondSurfaceEnabledMap);
}

/**
 * Restore information specific to the type of model from the scene.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void 
ModelSurfaceMontage::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    const int32_t montageVersion = sceneClass->getIntegerValue("montageVersion", 1);
    
    /*
     * Restore left surface
     */
    const SceneObjectMapIntegerKey* leftSurfaceMap = sceneClass->getMapIntegerKey("m_leftSurfaceSelectionModel");
    if (leftSurfaceMap != NULL) {
        const std::map<int32_t, SceneObject*>& surfaceMap = leftSurfaceMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = surfaceMap.begin();
             iter != surfaceMap.end();
             iter++) {
            const int32_t key = iter->first;
            const SceneClass* surfaceClass = dynamic_cast<const SceneClass*>(iter->second);
            m_leftSurfaceSelectionModel[key]->restoreFromScene(sceneAttributes, 
                                                               surfaceClass);
        }
    }
    
    /*
     * Restore left second surface
     */
    const SceneObjectMapIntegerKey* leftSecondSurfaceMap = sceneClass->getMapIntegerKey("m_leftSecondSurfaceSelectionModel");
    if (leftSecondSurfaceMap != NULL) {
        const std::map<int32_t, SceneObject*>& surfaceMap = leftSecondSurfaceMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = surfaceMap.begin();
             iter != surfaceMap.end();
             iter++) {
            const int32_t key = iter->first;
            const SceneClass* surfaceClass = dynamic_cast<const SceneClass*>(iter->second);
            m_leftSecondSurfaceSelectionModel[key]->restoreFromScene(sceneAttributes, 
                                                               surfaceClass);
        }
    }
    
    /*
     * Restore right surface
     */
    const SceneObjectMapIntegerKey* rightSurfaceMap = sceneClass->getMapIntegerKey("m_rightSurfaceSelectionModel");
    if (rightSurfaceMap != NULL) {
        const std::map<int32_t, SceneObject*>& surfaceMap = rightSurfaceMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = surfaceMap.begin();
             iter != surfaceMap.end();
             iter++) {
            const int32_t key = iter->first;
            const SceneClass* surfaceClass = dynamic_cast<const SceneClass*>(iter->second);
            m_rightSurfaceSelectionModel[key]->restoreFromScene(sceneAttributes, 
                                                               surfaceClass);
        }
    }
    
    /*
     * Restore right second surface
     */
    const SceneObjectMapIntegerKey* rightSecondSurfaceMap = sceneClass->getMapIntegerKey("m_rightSecondSurfaceSelectionModel");
    if (rightSecondSurfaceMap != NULL) {
        const std::map<int32_t, SceneObject*>& surfaceMap = rightSecondSurfaceMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = surfaceMap.begin();
             iter != surfaceMap.end();
             iter++) {
            const int32_t key = iter->first;
            const SceneClass* surfaceClass = dynamic_cast<const SceneClass*>(iter->second);
            m_rightSecondSurfaceSelectionModel[key]->restoreFromScene(sceneAttributes, 
                                                                     surfaceClass);
        }
    }
    
    
    if (montageVersion <= 1) {
        /*
         * Version 1 had only dual option so enable items added in version 2
         */
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            m_leftEnabled[i] = true;
            m_rightEnabled[i] = true;
            m_firstSurfaceEnabled[i] = true;
            m_secondSurfaceEnabled[i] = false;
        }
        
        /*
         * Restore dual configuration as second surface
         */
        const SceneObjectMapIntegerKey* dualConfigurationMap = sceneClass->getMapIntegerKey("m_dualConfigurationEnabled");
        if (dualConfigurationMap != NULL) {
            const std::map<int32_t, SceneObject*>& dataMap = dualConfigurationMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
                 iter != dataMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
                m_secondSurfaceEnabled[key] = primitive->booleanValue();
            }
        }
    }
    else {
        /*
         *  Restore left enabled
         */
        const SceneObjectMapIntegerKey* leftEnabledMap = sceneClass->getMapIntegerKey("m_leftEnabled");
        if (leftEnabledMap != NULL) {
            const std::map<int32_t, SceneObject*>& dataMap = leftEnabledMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
                 iter != dataMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
                m_leftEnabled[key] = primitive->booleanValue();
            }
        }

        /*
         * Restore right enabled
         */
        const SceneObjectMapIntegerKey* rightEnabledMap = sceneClass->getMapIntegerKey("m_rightEnabled");
        if (rightEnabledMap != NULL) {
            const std::map<int32_t, SceneObject*>& dataMap = rightEnabledMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
                 iter != dataMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
                m_rightEnabled[key] = primitive->booleanValue();
            }
        }
        
        /*
         * Restore first surface enabled
         */
        const SceneObjectMapIntegerKey* firstSurfaceEnabledMap = sceneClass->getMapIntegerKey("m_firstSurfaceEnabled");
        if (firstSurfaceEnabledMap != NULL) {
            const std::map<int32_t, SceneObject*>& dataMap = firstSurfaceEnabledMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
                 iter != dataMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
                m_firstSurfaceEnabled[key] = primitive->booleanValue();
            }
        }
        
        /*
         * Restore second surface enabled
         */
        const SceneObjectMapIntegerKey* secondSurfaceEnabledMap = sceneClass->getMapIntegerKey("m_secondSurfaceEnabled");
        if (secondSurfaceEnabledMap != NULL) {
            const std::map<int32_t, SceneObject*>& dataMap = secondSurfaceEnabledMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
                 iter != dataMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
                m_secondSurfaceEnabled[key] = primitive->booleanValue();
            }
        }
    }
}

/**
 * Set the montage viewports that were drawn for the given tab.
 * 
 * @param tabIndex
 *    Index of the tab.
 * @param montageViewports
 *    The viewports/models that were drawn.
 */
void
ModelSurfaceMontage::setMontageViewports(const int32_t tabIndex,
                                         const std::vector<SurfaceMontageViewport>& montageViewports)
{
    m_montageViewports[tabIndex] = montageViewports;
}

/**
 * Get the montage viewports that were drawn for the given tab.
 *
 * @param tabIndex
 *    Index of the tab.
 * @param montageViewportsOut
 *    The viewports/models that were drawn.
 */
void
ModelSurfaceMontage::getMontageViewports(const int32_t tabIndex,
                                         std::vector<SurfaceMontageViewport>& montageViewportsOut)
{
    montageViewportsOut = m_montageViewports[tabIndex];
}


