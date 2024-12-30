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

#include <algorithm>
#include <cmath>

#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "Brain.h"
#include "BrainOpenGL.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventModelSurfaceGet.h"
#include "ModelSurfaceMontage.h"
#include "OverlaySet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePrimitive.h"
#include "SurfaceMontageConfigurationCerebellar.h"
#include "SurfaceMontageConfigurationCerebral.h"
#include "SurfaceMontageConfigurationFlatMaps.h"
#include "SurfaceMontageConfigurationHippocampus.h"
#include "SurfaceMontageConfigurationHippocampusFlatMaps.h"
#include "SurfaceSelectionModel.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this model.
 *
 */
ModelSurfaceMontage::ModelSurfaceMontage(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE,
                         brain)
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    validSurfaceTypes.push_back(SurfaceTypeEnum::ANATOMICAL);
    validSurfaceTypes.push_back(SurfaceTypeEnum::RECONSTRUCTION);
    validSurfaceTypes.push_back(SurfaceTypeEnum::INFLATED);
    validSurfaceTypes.push_back(SurfaceTypeEnum::VERY_INFLATED);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedConfigurationType[i] = SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION;
        
        m_cerebellarConfiguration[i] = new SurfaceMontageConfigurationCerebellar(i);
        m_cerebralConfiguration[i] = new SurfaceMontageConfigurationCerebral(i);
        m_flatMapsConfiguration[i] = new SurfaceMontageConfigurationFlatMaps(i);
        m_hippocampusConfiguration[i] = new SurfaceMontageConfigurationHippocampus(i);
        m_hippocampusFlatMapsConfiguration[i] = new SurfaceMontageConfigurationHippocampusFlatMaps(i);
    }
    
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    overlaySurfaceStructures.push_back(StructureEnum::CORTEX_LEFT);
    overlaySurfaceStructures.push_back(StructureEnum::CORTEX_RIGHT);
}

/**
 * Destructor
 */
ModelSurfaceMontage::~ModelSurfaceMontage()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_cerebellarConfiguration[i];
        delete m_cerebralConfiguration[i];
        delete m_flatMapsConfiguration[i];
        delete m_hippocampusConfiguration[i];
        delete m_hippocampusFlatMapsConfiguration[i];
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

/**
 * Initialize the selected surfaces.
 */
void
ModelSurfaceMontage::initializeSelectedSurfaces()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_cerebellarConfiguration[i]->initializeSelectedSurfaces();
        m_cerebralConfiguration[i]->initializeSelectedSurfaces();
        m_flatMapsConfiguration[i]->initializeSelectedSurfaces();
        m_hippocampusConfiguration[i]->initializeSelectedSurfaces();
        m_hippocampusFlatMapsConfiguration[i]->initializeSelectedSurfaces();
    }
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
 * displaying this model.
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
    return getSelectedConfiguration(tabIndex)->getOverlaySet();
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
    return getSelectedConfiguration(tabIndex)->getOverlaySet();
}

/**
 * Initilize the overlays for this model.
 */
void 
ModelSurfaceMontage::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_cerebellarConfiguration[i]->getOverlaySet()->initializeOverlays();
        m_cerebralConfiguration[i]->getOverlaySet()->initializeOverlays();
        m_flatMapsConfiguration[i]->getOverlaySet()->initializeOverlays();
        m_hippocampusConfiguration[i]->getOverlaySet()->initializeOverlays();
        m_hippocampusFlatMapsConfiguration[i]->getOverlaySet()->initializeOverlays();
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
    std::vector<SurfaceSelectionModel*> selectionModels;
    
    switch (getSelectedConfigurationType(windowTabNumber)) {
        case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
        {
            SurfaceMontageConfigurationCerebellar* smcc = getCerebellarConfiguration(windowTabNumber);
            if (structure == StructureEnum::CEREBELLUM) {
                selectionModels.push_back(smcc->getFirstSurfaceSelectionModel());
                selectionModels.push_back(smcc->getSecondSurfaceSelectionModel());
            }
        }
            break;
        case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
        {
            SurfaceMontageConfigurationCerebral* smcc = getCerebralConfiguration(windowTabNumber);
            switch (structure) {
                case StructureEnum::CORTEX_LEFT:
                    selectionModels.push_back(smcc->getLeftFirstSurfaceSelectionModel());
                    selectionModels.push_back(smcc->getLeftSecondSurfaceSelectionModel());
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    selectionModels.push_back(smcc->getRightFirstSurfaceSelectionModel());
                    selectionModels.push_back(smcc->getRightSecondSurfaceSelectionModel());
                    break;
                default:
                    break;
            }
        }
            break;
        case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
        {
            SurfaceMontageConfigurationFlatMaps* smcfm = getFlatMapsConfiguration(windowTabNumber);
            switch (structure) {
                case StructureEnum::CEREBELLUM:
                    selectionModels.push_back(smcfm->getCerebellumSurfaceSelectionModel());
                    break;
                case StructureEnum::CORTEX_LEFT:
                    selectionModels.push_back(smcfm->getLeftSurfaceSelectionModel());
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    selectionModels.push_back(smcfm->getRightSurfaceSelectionModel());
                    break;
                default:
                    break;
            }
        }
            break;
        case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_CONFIGURATION:
        {
            SurfaceMontageConfigurationHippocampus* smcc = getHippocampusConfiguration(windowTabNumber);
            switch (structure) {
                case StructureEnum::CORTEX_LEFT:
                    selectionModels.push_back(smcc->getLeftFirstSurfaceSelectionModel());
                    selectionModels.push_back(smcc->getLeftSecondSurfaceSelectionModel());
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    selectionModels.push_back(smcc->getRightFirstSurfaceSelectionModel());
                    selectionModels.push_back(smcc->getRightSecondSurfaceSelectionModel());
                    break;
                default:
                    break;
            }
        }
            break;
        case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION:
        {
            SurfaceMontageConfigurationHippocampusFlatMaps* smhfc = getHippocampusFlatMapsConfiguration(windowTabNumber);
            switch (structure) {
                case StructureEnum::CORTEX_LEFT:
                    selectionModels.push_back(smhfc->getLeftFirstSurfaceSelectionModel());
                    selectionModels.push_back(smhfc->getLeftSecondSurfaceSelectionModel());
                    break;
                case StructureEnum::CORTEX_RIGHT:
                    selectionModels.push_back(smhfc->getRightFirstSurfaceSelectionModel());
                    selectionModels.push_back(smhfc->getRightSecondSurfaceSelectionModel());
                    break;
                default:
                    break;
            }
        }
            break;
    }
    
    Surface* surfaceOut = NULL;
    
    for (std::vector<SurfaceSelectionModel*>::iterator iter = selectionModels.begin();
         iter != selectionModels.end();
         iter++) {
        SurfaceSelectionModel* sm = *iter;
        if (sm != NULL) {
            surfaceOut = sm->getSurface();
            break;
        }
    }

    return surfaceOut;
}

/**
 * Get the selected configuration for the given tab.
 *
 * @param tabIndex
 *    Index of tab for the selected configuration.
 */
SurfaceMontageConfigurationAbstract*
ModelSurfaceMontage::getSelectedConfiguration(const int32_t tabIndex)
{
    switch (getSelectedConfigurationType(tabIndex)) {
        case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
            CaretAssertArrayIndex(m_cerebellarConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            return m_cerebellarConfiguration[tabIndex];
            break;
        case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            return m_cerebralConfiguration[tabIndex];
            break;
        case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
            CaretAssertArrayIndex(m_flatMapsConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            return m_flatMapsConfiguration[tabIndex];
            break;
        case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_CONFIGURATION:
            CaretAssertArrayIndex(m_hippocampusConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            return m_hippocampusConfiguration[tabIndex];
            break;
        case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION:
            CaretAssertArrayIndex(m_hippocampusFlatMapsConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            return m_hippocampusFlatMapsConfiguration[tabIndex];
            break;
    }

    return NULL;
}


/**
 * Get the selected configuration for the given tab.
 *
 * @param tabIndex
 *    Index of tab for the selected configuration.
 */
const SurfaceMontageConfigurationAbstract*
ModelSurfaceMontage::getSelectedConfiguration(const int32_t tabIndex) const
{
    ModelSurfaceMontage* msm = const_cast<ModelSurfaceMontage*>(this);
    return msm->getSelectedConfiguration(tabIndex);
}

/**
 * @return The type of configuration in the given tab.
 *
 * @param tabIndex
 *    Index of the tab.
 */
SurfaceMontageConfigurationTypeEnum::Enum
ModelSurfaceMontage::getSelectedConfigurationType(const int32_t tabIndex) const
{
    /*
     * Find valid configurations
     */
    std::vector< SurfaceMontageConfigurationTypeEnum::Enum> validTypes;
    if (m_cerebellarConfiguration[tabIndex]->isValid()) {
        validTypes.push_back(SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION);
    }
    if (m_cerebralConfiguration[tabIndex]->isValid()) {
        validTypes.push_back(SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION);
    }
    if (m_flatMapsConfiguration[tabIndex]->isValid()) {
        validTypes.push_back(SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION);
    }
    if (m_hippocampusConfiguration[tabIndex]->isValid()) {
        validTypes.push_back(SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_CONFIGURATION);
    }
    if (m_hippocampusFlatMapsConfiguration[tabIndex]->isValid()) {
        validTypes.push_back(SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION);
    }

    /*
     * Verify selected type is valid
     */
    bool validTypeSelected = false;
    switch (m_selectedConfigurationType[tabIndex]) {
        case SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION:
            CaretAssertArrayIndex(m_cerebellarConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            if (m_cerebellarConfiguration[tabIndex]->isValid()) {
                validTypeSelected = true;
            }
            break;
        case SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION:
            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            if (m_cerebralConfiguration[tabIndex]->isValid()) {
                validTypeSelected = true;
            }
            break;
        case SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION:
            CaretAssertArrayIndex(m_flatMapsConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            if (m_flatMapsConfiguration[tabIndex]->isValid()) {
                validTypeSelected = true;
            }
            break;
        case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_CONFIGURATION:
            CaretAssertArrayIndex(m_hippocampusConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            if (m_hippocampusConfiguration[tabIndex]->isValid()) {
                validTypeSelected = true;
            }
            break;
        case SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION:
            CaretAssertArrayIndex(m_hippocampusFlatMapsConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  tabIndex);
            if (m_hippocampusFlatMapsConfiguration[tabIndex]->isValid()) {
                validTypeSelected = true;
            }
            break;
    }
    
    /*
     * If configuration type selected is not valid, choose another
     * configuration type.
     */
    if ( ! validTypeSelected) {
        if ( ! validTypes.empty()) {
            m_selectedConfigurationType[tabIndex] = validTypes[0];
        }
    }
    
    return m_selectedConfigurationType[tabIndex];
}

/**
 * Set type of configuration in the given tab.
 *
 * @param tabIndex
 *    Index of the tab.
 * @param configurationType
 *    New configuration type for the tab index.   
 */
void
ModelSurfaceMontage::setSelectedConfigurationType(const int32_t tabIndex,
                                                  const SurfaceMontageConfigurationTypeEnum::Enum configurationType)
{
    m_selectedConfigurationType[tabIndex] = configurationType;
}

/**
 * Get the cerebellar configuration in the given tab.
 * 
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Cerebellar configuration.
 */
SurfaceMontageConfigurationCerebellar *
ModelSurfaceMontage::getCerebellarConfiguration(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_cerebellarConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_cerebellarConfiguration[tabIndex];
}

/**
 * Get the cerebellar configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Cerebellar configuration.
 */
const SurfaceMontageConfigurationCerebellar*
ModelSurfaceMontage::getCerebellarConfiguration(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_cerebellarConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_cerebellarConfiguration[tabIndex];
}

/**
 * Get the cerebral configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Cerebral configuration.
 */
SurfaceMontageConfigurationCerebral *
ModelSurfaceMontage::getCerebralConfiguration(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_cerebralConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_cerebralConfiguration[tabIndex];
}

/**
 * Get the cerebral configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Cerebral configuration.
 */
const SurfaceMontageConfigurationCerebral*
ModelSurfaceMontage::getCerebralConfiguration(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_cerebralConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_cerebralConfiguration[tabIndex];
}

/**
 * Get the flat maps configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Flat maps configuration.
 */
SurfaceMontageConfigurationFlatMaps *
ModelSurfaceMontage::getFlatMapsConfiguration(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_flatMapsConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_flatMapsConfiguration[tabIndex];
}

/**
 * Get the flat maps configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    Flat maps configuration.
 */
const SurfaceMontageConfigurationFlatMaps*
ModelSurfaceMontage::getFlatMapsConfiguration(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_flatMapsConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_flatMapsConfiguration[tabIndex];
}

/**
 * Get the hippocampus configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    hippocampus configuration.
 */
SurfaceMontageConfigurationHippocampus* 
ModelSurfaceMontage::getHippocampusConfiguration(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_hippocampusConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_hippocampusConfiguration[tabIndex];
}

/**
 * Get the hippocampus configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    hippocampus configuration.
 */
const SurfaceMontageConfigurationHippocampus*
ModelSurfaceMontage::getHippocampusConfiguration(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_hippocampusConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_hippocampusConfiguration[tabIndex];
}

/**
 * Get the hippocampus flat configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    hippocampus flat configuration.
 */
SurfaceMontageConfigurationHippocampusFlatMaps*
ModelSurfaceMontage::getHippocampusFlatMapsConfiguration(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_hippocampusFlatMapsConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_hippocampusFlatMapsConfiguration[tabIndex];
}

/**
 * Get the hippocampus flat configuration in the given tab.
 *
 * @param tabIndex
 *    Index of tab.
 * @return
 *    hippocampus flat configuration.
 */
const SurfaceMontageConfigurationHippocampusFlatMaps*
ModelSurfaceMontage::getHippocampusFlatMapsConfiguration(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_hippocampusFlatMapsConfiguration,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_hippocampusFlatMapsConfiguration[tabIndex];
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
    sceneClass->addInteger("montageVersion", 3);
    
    SceneObjectMapIntegerKey* cerebellarConfigurationMap = new SceneObjectMapIntegerKey("m_cerebellarConfiguration",
                                                                            SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* cerebralConfigurationMap = new SceneObjectMapIntegerKey("m_cerebralConfiguration",
                                                                                  SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* flatConfigurationMap = new SceneObjectMapIntegerKey("m_flatMapsConfiguration",
                                                                             SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* hippoConfigurationMap = new SceneObjectMapIntegerKey("m_hippocampusConfiguration",
                                                                                  SceneObjectDataTypeEnum::SCENE_CLASS);
    SceneObjectMapIntegerKey* hippoFlatConfigurationMap = new SceneObjectMapIntegerKey("m_hippocampusFlatMapsConfiguration",
                                                                                   SceneObjectDataTypeEnum::SCENE_CLASS);


    /*
     * Get indices of tabs that are to be saved to scene.
     */
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numActiveTabs = static_cast<int32_t>(tabIndices.size());
    
    sceneClass->addEnumeratedTypeArrayForTabIndices<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>("m_selectedConfigurationType",
                                                                                                                      m_selectedConfigurationType,
                                                                                                                      tabIndices);

    for (int32_t i = 0; i < numActiveTabs; i++) {
        const int32_t tabIndex = tabIndices[i];
        const AString tabString = ("[" + AString::number(tabIndex) + "]");
        
        cerebellarConfigurationMap->addClass(tabIndex,
                                             m_cerebellarConfiguration[tabIndex]->saveToScene(sceneAttributes,
                                                              "m_cerebellarConfiguration" + tabString));

        cerebralConfigurationMap->addClass(tabIndex,
                                           m_cerebralConfiguration[tabIndex]->saveToScene(sceneAttributes,
                                                                                          "m_cerebralConfiguration" + tabString));
        flatConfigurationMap->addClass(tabIndex,
                                           m_flatMapsConfiguration[tabIndex]->saveToScene(sceneAttributes,
                                                                                          "m_flatMapsConfiguration" + tabString));
        hippoConfigurationMap->addClass(tabIndex,
                                       m_hippocampusConfiguration[tabIndex]->saveToScene(sceneAttributes,
                                                                                      "m_hippocampusConfiguration" + tabString));
        hippoFlatConfigurationMap->addClass(tabIndex,
                                        m_hippocampusFlatMapsConfiguration[tabIndex]->saveToScene(sceneAttributes,
                                                                                          "m_hippocampusFlatMapsConfiguration" + tabString));
    }
    
    sceneClass->addChild(cerebellarConfigurationMap);
    sceneClass->addChild(cerebralConfigurationMap);
    sceneClass->addChild(flatConfigurationMap);
    sceneClass->addChild(hippoConfigurationMap);
    sceneClass->addChild(hippoFlatConfigurationMap);
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
    
    if (montageVersion >= 3) {
        /*
         * Restore Cerebellum
         */
        const SceneObjectMapIntegerKey* cerebellumMap = sceneClass->getMapIntegerKey("m_cerebellarConfiguration");
        if (cerebellumMap != NULL) {
            const std::map<int32_t, SceneObject*>& structureMap = cerebellumMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = structureMap.begin();
                 iter != structureMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const SceneClass* cerebellarSceneClass = dynamic_cast<const SceneClass*>(iter->second);
                cerebellarSceneClass->setRestored(true); // prevents "failed to restore"
                
                CaretAssertArrayIndex(m_cerebellarConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                
                m_cerebellarConfiguration[key]->restoreFromScene(sceneAttributes,
                                                                 cerebellarSceneClass);
            }
        }

        /*
         * Restore Cortex
         */
        const SceneObjectMapIntegerKey* cerebralMap = sceneClass->getMapIntegerKey("m_cerebralConfiguration");
        if (cerebellumMap != NULL) {
            const std::map<int32_t, SceneObject*>& structureMap = cerebralMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = structureMap.begin();
                 iter != structureMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const SceneClass* cerebralSceneClass = dynamic_cast<const SceneClass*>(iter->second);
                cerebralSceneClass->setRestored(true);// prevents "failed to restore"
                CaretAssertArrayIndex(m_cerebralConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                
                m_cerebralConfiguration[key]->restoreFromScene(sceneAttributes,
                                                                 cerebralSceneClass);
            }
        }
    
        /*
         * Restore flat maps
         */
        const SceneObjectMapIntegerKey* flatMap = sceneClass->getMapIntegerKey("m_flatMapsConfiguration");
        if (cerebellumMap != NULL) {
            const std::map<int32_t, SceneObject*>& structureMap = flatMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = structureMap.begin();
                 iter != structureMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const SceneClass* flatClass = dynamic_cast<const SceneClass*>(iter->second);
                flatClass->setRestored(true); // prevents "failed to restore"
                
                CaretAssertArrayIndex(m_flatMapsConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                
                m_flatMapsConfiguration[key]->restoreFromScene(sceneAttributes,
                                                                 flatClass);
            }
        }
        
        /*
         * Restore Hippocampus
         */
        const SceneObjectMapIntegerKey* hippoMap = sceneClass->getMapIntegerKey("m_hippocampusConfiguration");
        if (hippoMap != NULL) {
            const std::map<int32_t, SceneObject*>& structureMap = hippoMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = structureMap.begin();
                 iter != structureMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const SceneClass* hippoSceneClass = dynamic_cast<const SceneClass*>(iter->second);
                hippoSceneClass->setRestored(true);// prevents "failed to restore"
                CaretAssertArrayIndex(m_hippocampusConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                
                m_hippocampusConfiguration[key]->restoreFromScene(sceneAttributes,
                                                                  hippoSceneClass);
            }
        }

        /*
         * Restore Hippocampus Flat Maps
         */
        const SceneObjectMapIntegerKey* hippoFlatMap = sceneClass->getMapIntegerKey("m_hippocampusFlatMapsConfiguration");
        if (hippoFlatMap != NULL) {
            const std::map<int32_t, SceneObject*>& structureMap = hippoFlatMap->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = structureMap.begin();
                 iter != structureMap.end();
                 iter++) {
                const int32_t key = iter->first;
                const SceneClass* hippoFlatSceneClass = dynamic_cast<const SceneClass*>(iter->second);
                hippoFlatSceneClass->setRestored(true);// prevents "failed to restore"
                CaretAssertArrayIndex(m_hippocampusFlatMapsConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                
                m_hippocampusFlatMapsConfiguration[key]->restoreFromScene(sceneAttributes,
                                                                          hippoFlatSceneClass);
            }
        }
        
        sceneClass->getEnumerateTypeArrayForTabIndices<SurfaceMontageConfigurationTypeEnum,SurfaceMontageConfigurationTypeEnum::Enum>("m_selectedConfigurationType",
                                                                                                                                      m_selectedConfigurationType);
    }
    else {
        restoreFromSceneVersionTwoAndEarlier(sceneAttributes,
                                             sceneClass,
                                             montageVersion);
        
        return;
    }
    
}

/**
 * Restore information from the ModelSurfaceMontage that existed prior
 * to the cerebellar, cerebral, and flat montage configurations.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which information is restored.
 */
void
ModelSurfaceMontage::restoreFromSceneVersionTwoAndEarlier(const SceneAttributes* sceneAttributes,
                                                          const SceneClass* sceneClass,
                                                          const int32_t montageVersion)
{
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
            
            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  key);
            SurfaceSelectionModel* ssm = m_cerebralConfiguration[key]->getLeftFirstSurfaceSelectionModel();
            
            ssm->restoreFromScene(sceneAttributes,
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

            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  key);
            SurfaceSelectionModel* ssm = m_cerebralConfiguration[key]->getLeftSecondSurfaceSelectionModel();
            
            ssm->restoreFromScene(sceneAttributes,
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

            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  key);
            SurfaceSelectionModel* ssm = m_cerebralConfiguration[key]->getRightFirstSurfaceSelectionModel();
            
            ssm->restoreFromScene(sceneAttributes,
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
            
            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  key);
            SurfaceSelectionModel* ssm = m_cerebralConfiguration[key]->getRightSecondSurfaceSelectionModel();
            
            ssm->restoreFromScene(sceneAttributes,
                                  surfaceClass);
        }
    }
    
    
    if (montageVersion <= 1) {
        /*
         * Version 1 had only dual option so enable items added in version 2
         */
        for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
            CaretAssertArrayIndex(m_cerebralConfiguration,
                                  BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                  i);
            m_cerebralConfiguration[i]->setLeftEnabled(true);
            m_cerebralConfiguration[i]->setRightEnabled(true);
            m_cerebralConfiguration[i]->setFirstSurfaceEnabled(true);
            m_cerebralConfiguration[i]->setSecondSurfaceEnabled(true);
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
                CaretAssertArrayIndex(m_cerebralConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                m_cerebralConfiguration[key]->setSecondSurfaceEnabled(primitive->booleanValue());
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
                CaretAssertArrayIndex(m_cerebralConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                m_cerebralConfiguration[key]->setLeftEnabled(primitive->booleanValue());
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
                CaretAssertArrayIndex(m_cerebralConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                m_cerebralConfiguration[key]->setRightEnabled(primitive->booleanValue());
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
                CaretAssertArrayIndex(m_cerebralConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                m_cerebralConfiguration[key]->setFirstSurfaceEnabled(primitive->booleanValue());
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
                CaretAssertArrayIndex(m_cerebralConfiguration,
                                      BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                                      key);
                m_cerebralConfiguration[key]->setSecondSurfaceEnabled(primitive->booleanValue());
            }
        }
    }
    
    /*
     * Previous surface montage did not have lateral/medial selections
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        CaretAssertArrayIndex(m_cerebralConfiguration,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              i);
        m_cerebralConfiguration[i]->setLateralEnabled(true);
        m_cerebralConfiguration[i]->setMedialEnabled(true);
    }
}

/**
 * Get the number of rows and columns in the displayed surface montage.
 *
 * @param tabIndex
 *     Index of tab.
 * @param numberOfRowsOut
 *     Number of rows in the displayed montage.
 * @param numberOfColumnsOut
 *     Number of columns in the displayed montage.
 */
void
ModelSurfaceMontage::getSurfaceMontageNumberOfRowsAndColumns(const int32_t tabIndex,
                                                             int32_t& numberOfRowsOut,
                                                             int32_t& numberOfColumnsOut) const
{
    numberOfRowsOut    = 1;
    numberOfColumnsOut = 1;
    
    ModelSurfaceMontage* nonConstMSM = const_cast<ModelSurfaceMontage*>(this);
    std::vector<const SurfaceMontageViewport*> surfaceMontageViewports;
    nonConstMSM->getSurfaceMontageViewportsForTransformation(tabIndex,
                                                             surfaceMontageViewports);
    SurfaceMontageViewport::getNumberOfRowsAndColumns(surfaceMontageViewports,
                                                      numberOfRowsOut,
                                                      numberOfColumnsOut);
}

/**
 * Get the montage viewports for drawing by OpenGL.  The montage viewports
 * will be updated prior to returning them.   OpenGL will update
 * the viewing dimensions (x, y, width, height) in the returned montage
 * viewports.
 *
 * @param tabIndex
 *    Tab for which viewports are requested.
 * @param surfaceMontageViewports
 *    The montage viewports.
 */
void
ModelSurfaceMontage::getSurfaceMontageViewportsForDrawing(const int32_t tabIndex,
                                                          std::vector<SurfaceMontageViewport*>& surfaceMontageViewports)
{
    SurfaceMontageConfigurationAbstract* config = getSelectedConfiguration(tabIndex);
    if (config != NULL) {
        config->getSurfaceMontageViewportsForDrawing(surfaceMontageViewports);
    }
    else {
        surfaceMontageViewports.clear();
    }
}

/**
 * Get the montage viewports that will be used by the mouse transformations.
 *
 * @param tabIndex
 *    Tab for which viewports are requested.
 * @param surfaceMontageViewports
 *    The montage viewports.
 */
void
ModelSurfaceMontage::getSurfaceMontageViewportsForTransformation(const int32_t tabIndex,
                                                                 std::vector<const SurfaceMontageViewport*>& surfaceMontageViewports) const
{
    const SurfaceMontageConfigurationAbstract* config = getSelectedConfiguration(tabIndex);
    if (config != NULL) {
        config->getSurfaceMontageViewportsForTransformation(surfaceMontageViewports);
    }
    else {
        surfaceMontageViewports.clear();
    }
}

/**
 * @return A string describing the model.
 */
AString
ModelSurfaceMontage::toString() const
{
    AString msg;
    
    msg.appendWithNewLine("Surface Montage: ");
    
    return msg;
}

/**
 * Get a text description of the window's content.
 *
 * @param tabIndex
 *    Index of the tab for content description.
 * @param descriptionOut
 *    Description of the window's content.
 */
void
ModelSurfaceMontage::getDescriptionOfContent(const int32_t tabIndex,
                                             PlainTextStringBuilder& descriptionOut) const
{
    const SurfaceMontageConfigurationAbstract* config = getSelectedConfiguration(tabIndex);
    if (config != NULL) {
        config->getDescriptionOfContent(descriptionOut);
    }
}

/**
 * Copy the tab content from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ModelSurfaceMontage::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    CaretAssertArrayIndex(m_selectedConfigurationType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, sourceTabIndex);
    CaretAssertArrayIndex(m_selectedConfigurationType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, destinationTabIndex);
    
    m_cerebellarConfiguration[destinationTabIndex]->copyConfiguration(m_cerebellarConfiguration[sourceTabIndex]);
    m_cerebralConfiguration[destinationTabIndex]->copyConfiguration(m_cerebralConfiguration[sourceTabIndex]);
    m_flatMapsConfiguration[destinationTabIndex]->copyConfiguration(m_flatMapsConfiguration[sourceTabIndex]);
    m_hippocampusConfiguration[destinationTabIndex]->copyConfiguration(m_hippocampusConfiguration[sourceTabIndex]);
    m_hippocampusFlatMapsConfiguration[destinationTabIndex]->copyConfiguration(m_hippocampusFlatMapsConfiguration[sourceTabIndex]);
    m_selectedConfigurationType[destinationTabIndex] = m_selectedConfigurationType[sourceTabIndex];
}

