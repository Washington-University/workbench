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
#include "SurfaceSelectionModel.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this controller.
 *
 */
ModelSurfaceMontage::ModelSurfaceMontage(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES,
                         brain)
{
    initializeMembersModelSurfaceMontage();
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySet[i] = new OverlaySet(this);
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        leftView(i);
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
ModelSurfaceMontage::initializeMembersModelSurfaceMontage()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_leftSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT);
        m_leftSecondSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT);
        m_rightSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT);
        m_rightSecondSurfaceSelectionModel[i] = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT);
        m_dualConfigurationEnabled[i] = false;
    }
}

/**
 * @return Is dual configuration enabled?
 */
bool 
ModelSurfaceMontage::isDualConfigurationEnabled(const int tabIndex) const
{
    CaretAssertArrayIndex(m_dualConfigurationEnabled, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return m_dualConfigurationEnabled[tabIndex];
}

/**
 * Set dual configuration enabled
 * @param tabIndex
 *    Index of tab.
 * @param enabled
 *    New dual configuration status
 */
void 
ModelSurfaceMontage::setDualConfigurationEnabled(const int tabIndex,
                                                 const bool enabled)
{
    CaretAssertArrayIndex(m_dualConfigurationEnabled, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);

    m_dualConfigurationEnabled[tabIndex] = enabled;
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
    AString name = "Surface Montage";
    
    return name;
}

/**
 * Set the scaling so that the model fills the window.
 *
 */
void
ModelSurfaceMontage::setDefaultScalingToFitWindow()
{
    /*
    BoundingBox bounds;
    m_surface->getBounds(bounds);
    
    float bigY = std::max(std::abs(bounds.getMinY()), bounds.getMaxY());
    float percentScreenY = BrainOpenGL::getModelViewingHalfWindowHeight() * 0.90f;
    float scale = percentScreenY / bigY;
    m_defaultModelScaling = scale;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_setScaling(i, m_defaultModelScaling);
    }
    */
}

/**
 * Reset view.  For left and right hemispheres, the default
 * view is a lateral view.
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
ModelSurfaceMontage::resetView(const int32_t windowTabNumber)
{
    Model::resetView(windowTabNumber);
    leftView(windowTabNumber);    
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
 * For a structure model, copy the transformations from one window of
 * the structure model to another window.
 *
 * @param controllerSource        Source structure model
 * @param windowTabNumberSource   windowTabNumber of source transformation.
 * @param windowTabNumberTarget   windowTabNumber of target transformation.
 *
 */
void
ModelSurfaceMontage::copyTransformationsAndViews(const Model& controllerSource,
                                         const int32_t windowTabNumberSource,
                                         const int32_t windowTabNumberTarget)
{
    if (this == &controllerSource) {
        if (windowTabNumberSource == windowTabNumberTarget) {
            return;
        }
    }
    
    CaretAssertArrayIndex(m_translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumberTarget);
    CaretAssertArrayIndex(controllerSource->translation,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          windowTabNumberSource);
    
    Model::copyTransformationsAndViews(controllerSource, windowTabNumberSource, windowTabNumberTarget);
    
    const ModelSurfaceMontage* modelSurfaceMontage = dynamic_cast<const ModelSurfaceMontage*>(&controllerSource);
    if (modelSurfaceMontage == NULL) {
        return;
    }
  
    m_leftSurfaceSelectionModel[windowTabNumberTarget]->setSurface(
                                modelSurfaceMontage->m_leftSurfaceSelectionModel[windowTabNumberSource]->getSurface());
    
    m_leftSecondSurfaceSelectionModel[windowTabNumberTarget]->setSurface(
                                modelSurfaceMontage->m_leftSecondSurfaceSelectionModel[windowTabNumberSource]->getSurface());

    m_rightSurfaceSelectionModel[windowTabNumberTarget]->setSurface(
                                modelSurfaceMontage->m_rightSurfaceSelectionModel[windowTabNumberSource]->getSurface());
    
    m_rightSecondSurfaceSelectionModel[windowTabNumberTarget]->setSurface(
                                modelSurfaceMontage->m_rightSecondSurfaceSelectionModel[windowTabNumberSource]->getSurface());
    
    m_dualConfigurationEnabled[windowTabNumberTarget] = modelSurfaceMontage->m_dualConfigurationEnabled[windowTabNumberSource];
    
//    m_setSliceViewPlane(windowTabNumberTarget, 
//                            modelVolumeSource->getSliceViewPlane(windowTabNumberSource));
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
    /*
     * Get indices of tabs that are to be saved to scene.
     */ 
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numActiveTabs = static_cast<int32_t>(tabIndices.size()); 
    
    /*
     * Left surface
     */
    std::vector<SceneClass*> leftSurfaceSceneVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        SceneClass* leftSurfaceSceneClass = new SceneClass(("m_leftSurfaceSelectionModel["
                                                            + AString::number(tabIndex)
                                                            + "]"),
                                                           "selectedSurface",
                                                           1);
        leftSurfaceSceneClass->addInteger("tabIndex", tabIndex);
        leftSurfaceSceneClass->addChild(m_leftSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                           "m_leftSurfaceSelectionModel"));
        leftSurfaceSceneVector.push_back(leftSurfaceSceneClass);
    }
    sceneClass->addChild(new SceneClassArray("leftSurface",
                                             leftSurfaceSceneVector));
    /*
     * Left second surface
     */
    std::vector<SceneClass*> leftSecondSurfaceSceneVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        SceneClass* leftSecondSurfaceSceneClass = new SceneClass(("m_leftSecondSurfaceSelectionModel["
                                                                  + AString::number(tabIndex)
                                                                  + "]"),
                                                                 "selectedSurface",
                                                                 1);
        leftSecondSurfaceSceneClass->addInteger("tabIndex", tabIndex);
        leftSecondSurfaceSceneClass->addChild(m_leftSecondSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                                 "m_leftSecondSurfaceSelectionModel"));
        leftSecondSurfaceSceneVector.push_back(leftSecondSurfaceSceneClass);
    }
    sceneClass->addChild(new SceneClassArray("leftSecondSurface",
                                             leftSecondSurfaceSceneVector));

    /*
     * Right surface
     */
    std::vector<SceneClass*> rightSurfaceSceneVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        SceneClass* rightSurfaceSceneClass = new SceneClass(("m_rightSurfaceSelectionModel["
                                                             + AString::number(tabIndex)
                                                             + "]"),
                                                            "selectedSurface",
                                                            1);
        rightSurfaceSceneClass->addInteger("tabIndex", tabIndex);
        rightSurfaceSceneClass->addChild(m_rightSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                            "m_rightSurfaceSelectionModel"));
        rightSurfaceSceneVector.push_back(rightSurfaceSceneClass);
    }
    sceneClass->addChild(new SceneClassArray("rightSurface",
                                             rightSurfaceSceneVector));

    /*
     * Right second surface
     */
    std::vector<SceneClass*> rightSecondSurfaceSceneVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        SceneClass* rightSecondSurfaceSceneClass = new SceneClass(("m_rightSecondSurfaceSelectionModel["
                                                                   + AString::number(tabIndex)
                                                                   + "]"),
                                                                  "selectedSurface",
                                                                  1);
        rightSecondSurfaceSceneClass->addInteger("tabIndex", tabIndex);
        rightSecondSurfaceSceneClass->addChild(m_rightSecondSurfaceSelectionModel[tabIndex]->saveToScene(sceneAttributes,
                                                                                                  "m_rightSecondSurfaceSelectionModel"));
        rightSecondSurfaceSceneVector.push_back(rightSecondSurfaceSceneClass);
    }
    sceneClass->addChild(new SceneClassArray("rightSecondSurface",
                                             rightSecondSurfaceSceneVector));
    
    /*
     * Dual configuration selection
     */
    std::vector<SceneClass*> dualConfigurationSceneVector;
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        SceneClass* dualConfigurationSceneClass = new SceneClass(("m_dualConfigurationEnabled["
                                                                   + AString::number(tabIndex)
                                                                   + "]"),
                                                                  "DualConfiguration",
                                                                  1);
        dualConfigurationSceneClass->addInteger("tabIndex", tabIndex);
        dualConfigurationSceneClass->addBoolean("m_dualConfigurationEnabled", 
                                                m_dualConfigurationEnabled[tabIndex]);
        dualConfigurationSceneVector.push_back(dualConfigurationSceneClass);
    }
    sceneClass->addChild(new SceneClassArray("dualConfiguration",
                                             dualConfigurationSceneVector));
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
    /*
     * Restore left surface
     */
    const SceneClassArray* leftSurfaceArray = sceneClass->getClassArray("leftSurface");
    if (leftSurfaceArray != NULL) {
        const int numLeftSurfaces = leftSurfaceArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numLeftSurfaces; i++) {
            const SceneClass* surfaceClass = leftSurfaceArray->getClassAtIndex(i);
            const int tabIndex = surfaceClass->getIntegerValue("tabIndex");
            if (tabIndex >= 0) {
                m_leftSurfaceSelectionModel[tabIndex]->restoreFromScene(sceneAttributes, 
                                                                        surfaceClass->getClass("m_leftSurfaceSelectionModel"));
            }
        }
    }
    
    /*
     * Restore right second surface
     */
    const SceneClassArray* leftSecondSurfaceArray = sceneClass->getClassArray("leftSecondSurface");
    if (leftSecondSurfaceArray != NULL) {
        const int numLeftSecondSurfaces = leftSecondSurfaceArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numLeftSecondSurfaces; i++) {
            const SceneClass* surfaceClass = leftSecondSurfaceArray->getClassAtIndex(i);
            const int tabIndex = surfaceClass->getIntegerValue("tabIndex");
            if (tabIndex >= 0) {
                m_leftSecondSurfaceSelectionModel[tabIndex]->restoreFromScene(sceneAttributes, 
                                                                              surfaceClass->getClass("m_leftSecondSurfaceSelectionModel"));
            }
        }
    }
    
    /*
     * Restore right surface
     */
    const SceneClassArray* rightSurfaceArray = sceneClass->getClassArray("rightSurface");
    if (rightSurfaceArray != NULL) {
        const int numRightSurfaces = rightSurfaceArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numRightSurfaces; i++) {
            const SceneClass* surfaceClass = rightSurfaceArray->getClassAtIndex(i);
            const int tabIndex = surfaceClass->getIntegerValue("tabIndex");
            if (tabIndex >= 0) {
                m_rightSurfaceSelectionModel[tabIndex]->restoreFromScene(sceneAttributes, 
                                                                         surfaceClass->getClass("m_rightSurfaceSelectionModel"));
            }
        }
    }

    /*
     * Restore right second surface
     */
    const SceneClassArray* rightSecondSurfaceArray = sceneClass->getClassArray("rightSecondSurface");
    if (rightSecondSurfaceArray != NULL) {
        const int numRightSecondSurfaces = rightSecondSurfaceArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numRightSecondSurfaces; i++) {
            const SceneClass* surfaceClass = rightSecondSurfaceArray->getClassAtIndex(i);
            const int tabIndex = surfaceClass->getIntegerValue("tabIndex");
            if (tabIndex >= 0) {
                m_rightSecondSurfaceSelectionModel[tabIndex]->restoreFromScene(sceneAttributes, 
                                                                               surfaceClass->getClass("m_rightSecondSurfaceSelectionModel"));
            }
        }
    }
    
    /*
     * Restore dual configuration selection
     */
    const SceneClassArray* dualConfigurationArray = sceneClass->getClassArray("dualConfiguration");
    if (dualConfigurationArray != NULL) {
        const int numDualConfiguration = dualConfigurationArray->getNumberOfArrayElements();
        for (int32_t i = 0; i < numDualConfiguration; i++) {
            const SceneClass* dualConfigClass = dualConfigurationArray->getClassAtIndex(i);
            const int tabIndex = dualConfigClass->getIntegerValue("tabIndex");
            if (tabIndex >= 0) {
                m_dualConfigurationEnabled[tabIndex] = dualConfigClass->getBooleanValue("m_dualConfigurationEnabled",
                                                                                        false);
            }
        }
    }
}

