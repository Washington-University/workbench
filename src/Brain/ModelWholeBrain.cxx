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

#define __MODEL_WHOLE_BRAIN_DEFINE__
#include "ModelWholeBrain.h"
#undef __MODEL_WHOLE_BRAIN_DEFINE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "EventSurfacesGet.h"
#include "IdentificationManager.h"
#include "ModelSurface.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this whole brain model belongs.
 *
 */
ModelWholeBrain::ModelWholeBrain(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN,
                         brain)
{
    if (s_anatomicalSurfaceTypes.empty()) {
        SurfaceTypeEnum::getAllAnatomicallyShapedEnums(s_anatomicalSurfaceTypes);
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedSurfaceType[i] = SurfaceTypeEnum::ANATOMICAL;
        m_cerebellumEnabled[i] = true;
        m_leftEnabled[i] = true;
        m_rightEnabled[i] = true;
        m_leftRightSeparation[i] = 0.0;
        m_cerebellumSeparation[i] = 0.0;
    }

    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    overlaySurfaceStructures.push_back(StructureEnum::CORTEX_LEFT);
    overlaySurfaceStructures.push_back(StructureEnum::CORTEX_RIGHT);
    overlaySurfaceStructures.push_back(StructureEnum::CEREBELLUM);
    overlaySurfaceStructures.push_back(StructureEnum::HIPPOCAMPUS_LEFT);
    overlaySurfaceStructures.push_back(StructureEnum::HIPPOCAMPUS_RIGHT);
    overlaySurfaceStructures.push_back(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT);
    overlaySurfaceStructures.push_back(StructureEnum::HIPPOCAMPUS_DENTATE_RIGHT);

    m_overlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_YES,
                                            "All View");
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<SurfaceTypeEnum,SurfaceTypeEnum::Enum>("m_selectedSurfaceType", 
                                                                                              m_selectedSurfaceType);
    m_sceneAssistant->addTabIndexedBooleanArray("m_leftEnabled", m_leftEnabled);
    m_sceneAssistant->addTabIndexedBooleanArray("m_rightEnabled", m_rightEnabled);    
    m_sceneAssistant->addTabIndexedBooleanArray("m_cerebellumEnabled", m_cerebellumEnabled);    
    m_sceneAssistant->addTabIndexedFloatArray("m_leftRightSeparation", m_leftRightSeparation);    
    m_sceneAssistant->addTabIndexedFloatArray("m_cerebellumSeparation", m_cerebellumSeparation);
}

/**
 * Destructor
 */
ModelWholeBrain::~ModelWholeBrain()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_overlaySetArray;
    
    delete m_sceneAssistant;
}

/**
 * Get the available surface types.
 * @param surfaceTypesOut
 *    Output loaded with the available surface types.
 */
void 
ModelWholeBrain::getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum>& surfaceTypesOut)
{
    updateModel();
    
    surfaceTypesOut.clear();
    surfaceTypesOut.insert(surfaceTypesOut.end(),
                           m_availableSurfaceTypes.begin(),
                           m_availableSurfaceTypes.end());
}
/**
 * Get the selected surface type for the given tab.
 *
 * @param windowTabNumber
 *    The tab.
 * @return 
 *    Surface type for the tab.
 */
SurfaceTypeEnum::Enum 
ModelWholeBrain::getSelectedSurfaceType(const int32_t windowTabNumber)
{
    updateModel();
    return m_selectedSurfaceType[windowTabNumber];    
}

/**
 * Update this model.
 */
void
ModelWholeBrain::updateModel()
{
    /*
     * Get all of the surfaces
     */
    EventSurfacesGet surfaceEvent;
    EventManager::get()->sendEvent(surfaceEvent.getPointer());
    std::vector<Surface*> allSurfaces = surfaceEvent.getSurfaces();
    
    /*
     * Update the available surface types
     */
    m_availableSurfaceTypes.clear();
    for (std::vector<Surface*>::iterator surfIter = allSurfaces.begin();
         surfIter != allSurfaces.end();
         surfIter++) {
        m_availableSurfaceTypes.insert((*surfIter)->getSurfaceType());
    }
    
    /*
     * Set the default surface type from the available anatomical types
     */
    std::vector<SurfaceTypeEnum::Enum>::iterator defaultSurfaceTypeIter =
    std::find_first_of(s_anatomicalSurfaceTypes.begin(), s_anatomicalSurfaceTypes.end(),
                       m_availableSurfaceTypes.begin(), m_availableSurfaceTypes.end());
    const SurfaceTypeEnum::Enum defaultSurfaceType = ((defaultSurfaceTypeIter != s_anatomicalSurfaceTypes.end())
                                                      ? *defaultSurfaceTypeIter
                                                      : SurfaceTypeEnum::ANATOMICAL);
    
    /*
     * If the selected surface type in a tab is no longer valid, update 
     * selected surface type.
     */
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        if (std::find(m_availableSurfaceTypes.begin(),
                      m_availableSurfaceTypes.end(),
                      m_selectedSurfaceType[iTab]) == m_availableSurfaceTypes.end()) {
            m_selectedSurfaceType[iTab] = defaultSurfaceType;
        }
    }
    
    /*
     * Deselect any surfaces that are no longer valid.  They will get updated
     * the next time getSelected
     */
    for (int32_t tabIndex = 0; tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; tabIndex++) {
        for (std::map<std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum>, Surface*>::iterator mapIter = m_selectedSurface[tabIndex].begin();
             mapIter != m_selectedSurface[tabIndex].end();
             mapIter++) {
            if (std::find(allSurfaces.begin(),
                          allSurfaces.end(),
                          mapIter->second) == allSurfaces.end()) {
                mapIter->second = NULL;
            }
        }
    }
}

///**
// * Update this model.
// */
//void 
//ModelWholeBrain::updateModel()
//{
//    /*
//     * Get all model to find loaded surface types.
//     */
//    EventModelGetAll eventGetModels;
//    EventManager::get()->sendEvent(eventGetModels.getPointer());
//    const std::vector<Model*> allModels =
//        eventGetModels.getModels();
//
//    /*
//     * Get ALL possible surface types.
//     */
//    std::vector<SurfaceTypeEnum::Enum> allSurfaceTypes;
//    SurfaceTypeEnum::getAllEnums(allSurfaceTypes);
//    const int32_t numEnumTypes = static_cast<int32_t>(allSurfaceTypes.size());
//    std::vector<bool> surfaceTypeValid(numEnumTypes, false);
//    
//    /*
//     * Find surface types that are actually used.
//     */
//    for (std::vector<Model*>::const_iterator iter = allModels.begin();
//         iter != allModels.end();
//         iter++) {
//        ModelSurface* surfaceModel =
//            dynamic_cast<ModelSurface*>(*iter);
//        if (surfaceModel != NULL) {
//            SurfaceTypeEnum::Enum surfaceType = surfaceModel->getSurface()->getSurfaceType();
//            
//            for (int i = 0; i < numEnumTypes; i++) {
//                if (allSurfaceTypes[i] == surfaceType) {
//                    surfaceTypeValid[i] = true;
//                    break;
//                }
//            }
//        }
//    }
//    
//    /*
//     * Set the available surface types.
//     */
//    m_availableSurfaceTypes.clear();
//    for (int i = 0; i < numEnumTypes; i++) {
//        if (surfaceTypeValid[i]) {
//            m_availableSurfaceTypes.push_back(allSurfaceTypes[i]);
//        }
//    }
//    
//    
//    
//    /*
//     * Update the selected surface and volume types.
//     */
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        if (std::find(m_availableSurfaceTypes.begin(),
//                      m_availableSurfaceTypes.end(),
//                      m_selectedSurfaceType[i]) == m_availableSurfaceTypes.end()) {
//            if (m_availableSurfaceTypes.empty() == false) {
//                m_selectedSurfaceType[i] = m_availableSurfaceTypes[0];
//            }
//            else {
//                m_selectedSurfaceType[i] = SurfaceTypeEnum::ANATOMICAL;
//            }
//        }
//        
//        VolumeMappableInterface* vf = getUnderlayVolumeFile(i);
//        if (vf != NULL) {
////            m_volumeSlicesSelected[i].updateForVolumeFile(vf);
//        }
//    }
//}

/**
 * Set the selected surface type.
 * @param windowTabNumber
 *    Index of window tab.
 * @param surfaceType
 *    New surface type.
 */
void 
ModelWholeBrain::setSelectedSurfaceType(const int32_t windowTabNumber,
                                                         const SurfaceTypeEnum::Enum surfaceType)
{
    m_selectedSurfaceType[windowTabNumber] = surfaceType;
    updateModel();
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
ModelWholeBrain::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "Whole Brain";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelWholeBrain::getNameForBrowserTab() const
{
    return "All ";
}

/**
 * Get the bottom-most active volume in the given window tab.
 * @param windowTabNumber 
 *    Tab number for content.
 * @return 
 *    Bottom-most volume or NULL if not available (such as 
 *    when all overlay are not volumes or they are disabled).
 */
VolumeMappableInterface* 
ModelWholeBrain::getUnderlayVolumeFile(const int32_t windowTabNumber) const
{
    VolumeMappableInterface* vf = NULL;
    
    EventBrowserTabGet getBrowserTabEvent(windowTabNumber);
    EventManager::get()->sendEvent(getBrowserTabEvent.getPointer());
    BrowserTabContent* btc = getBrowserTabEvent.getBrowserTab();
    if (btc != NULL) {
        OverlaySet* overlaySet = btc->getOverlaySet();
        if (overlaySet != NULL) {
            vf = overlaySet->getUnderlayVolume();
        }
    }
    
    return vf;
}

/**
 * @return Return the surfaces displayed in the given tab.
 * @param windowTabIndex
 *    THe tab.
 */
std::vector<Surface*>
ModelWholeBrain::getSelectedSurfaces(const int32_t windowTabIndex)
{
    std::vector<Surface*> surfaces;
    
    /*
     * Get the surfaces.
     */
    Brain* brain = getBrain();
    const int32_t numberOfBrainStructures = brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numberOfBrainStructures; i++) {
        BrainStructure* brainStructure = brain->getBrainStructure(i);
        const StructureEnum::Enum structure = brainStructure->getStructure();
        Surface* surface = getSelectedSurface(structure,
                                              windowTabIndex);
        surfaces.push_back(surface);
    }

    return surfaces;
}


/**
 * Get the surface for the given structure in the given tab that is for
 * the currently selected surface type.
 *
 * @param structure
 *    Structure for the surface
 * @param windowTabNumber
 *    Tab number of window.
 * @param Pointer to selected surface for given structure or NULL if not available.
 */
Surface* 
ModelWholeBrain::getSelectedSurface(const StructureEnum::Enum structure,
                                                     const int32_t windowTabNumber)

{
    const SurfaceTypeEnum::Enum surfaceType = getSelectedSurfaceType(windowTabNumber);
    std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum> key = 
        std::make_pair(structure, 
                       surfaceType);
    
    /*
     * Get the currently selected surface.
     */
    Surface* s = m_selectedSurface[windowTabNumber][key];
    
    /*
     * See if currently selected surface is valid.
     */
    BrainStructure* brainStructure = m_brain->getBrainStructure(structure, 
                                                                    false);
    if (brainStructure == NULL) {
        return NULL;
    }
    
    std::vector<Surface*> surfaces;
    brainStructure->getSurfacesOfType(surfaceType,
                                      surfaces);
    if (std::find(surfaces.begin(),
                  surfaces.end(),
                  s) == surfaces.end()) {
        s = NULL;
    }
    
    /*
     * If no selected surface, use first surface.
     */
    if (s == NULL) {
        if (surfaces.empty() == false) {
            s = surfaces[0];
        }
    }
    
    m_selectedSurface[windowTabNumber][key] = s;
    
    return s;
}

/**
 * Set the selected surface for the given structure in the given window tab 
 * that is for the currently selected surface type.
 * surface type.
 *
 * @param structure
 *    Structure for the surface
 * @param windowTabNumber
 *    Tab number of window.
 * @param surface
 *    Newly selected surface.
 */
void 
ModelWholeBrain::setSelectedSurface(const StructureEnum::Enum structure,
                                                          const int32_t windowTabNumber,
                                                          Surface* surface)
{
    const SurfaceTypeEnum::Enum surfaceType = getSelectedSurfaceType(windowTabNumber);
    std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum> key = 
    std::make_pair(structure, 
                   surfaceType);
    
    m_selectedSurface[windowTabNumber][key] = surface;
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
ModelWholeBrain::receiveEvent(Event* /*event*/)
{
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelWholeBrain::getOverlaySet(const int tabIndex)
{
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelWholeBrain::getOverlaySet(const int tabIndex) const
{
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Initilize the overlays for this model.
 */
void 
ModelWholeBrain::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
}

/**
 * Initialize the selected surfaces.
 */
void
ModelWholeBrain::initializeSelectedSurfaces()
{
    std::vector<SurfaceTypeEnum::Enum> surfaceTypes;
    getAvailableSurfaceTypes(surfaceTypes);
    
    if (std::find(surfaceTypes.begin(),
                  surfaceTypes.end(),
                  SurfaceTypeEnum::ANATOMICAL) != surfaceTypes.end()) {
        for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
            setSelectedSurfaceType(iTab,
                                   SurfaceTypeEnum::ANATOMICAL);
        }
    }
    
    const int32_t numberOfStructures = m_brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numberOfStructures; i++) {
        const BrainStructure* brainStructure = m_brain->getBrainStructure(i);
        Surface* surface = const_cast<Surface*>(brainStructure->getPrimaryAnatomicalSurface());
        if (surface != NULL) {
            const StructureEnum::Enum structure = brainStructure->getStructure();
            for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
                setSelectedSurface(structure,
                                   iTab,
                                   surface);
            }
        }
    }
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
ModelWholeBrain::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numTabs = static_cast<int32_t>(tabIndices.size());
    
    std::vector<SceneClass*> classesForSelectedSurfaceArray;
    for (int32_t i = 0; i < numTabs; i++) {
        const int32_t tabIndex = tabIndices[i];
        const SurfaceTypeEnum::Enum selectedSurfaceType = getSelectedSurfaceType(tabIndex);
        
        /*
         * Updates selected surfaces
         */
        getSelectedSurfaces(tabIndex);
         
        for (std::map<std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum>, Surface*>::iterator mapIter = m_selectedSurface[tabIndex].begin();
             mapIter != m_selectedSurface[tabIndex].end();
             mapIter++) {
            std::pair<StructureEnum::Enum, SurfaceTypeEnum::Enum> structureSurfaceType = mapIter->first;
            const SurfaceTypeEnum::Enum surfaceType = structureSurfaceType.second;
            
            if (surfaceType == selectedSurfaceType) {
                Surface* surface = mapIter->second;
                if (surface != NULL) {
                    const StructureEnum::Enum structure = structureSurfaceType.first;
                    
                    const AString name = ("m_selectedSurface[" + AString::number(tabIndex) + "]");
                    SceneClass* surfaceClass = new SceneClass(name,
                                                              "SurfaceSelectionMap",
                                                              1);
                    surfaceClass->addInteger("tabIndex", tabIndex);
                    surfaceClass->addEnumeratedType<StructureEnum,StructureEnum::Enum>("structure",
                                                                                       structure);
                    surfaceClass->addEnumeratedType<SurfaceTypeEnum,SurfaceTypeEnum::Enum>("surfaceType",
                                                                                           surfaceType);
                    surfaceClass->addString("surfaceName",
                                            surface->getFileNameNoPath());
                    
                    classesForSelectedSurfaceArray.push_back(surfaceClass);
                }
            }
        }
    }
    sceneClass->addChild(new SceneClassArray("m_selectedSurface",
                                             classesForSelectedSurfaceArray));
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
ModelWholeBrain::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /*
     * Restore selected surface
     */
    const SceneClassArray* surfaceSelectionArray = sceneClass->getClassArray("m_selectedSurface");
    if (surfaceSelectionArray != NULL) {
        const int32_t numClasses = surfaceSelectionArray->getNumberOfArrayElements();
        for (int32_t ica = 0; ica < numClasses; ica++) {
            const SceneClass* surfaceClass = surfaceSelectionArray->getClassAtIndex(ica);
            const int32_t tabIndex = surfaceClass->getIntegerValue("tabIndex", -1);
            const StructureEnum::Enum structure = 
                surfaceClass->getEnumeratedTypeValue<StructureEnum,StructureEnum::Enum>("structure", 
                                                                                        StructureEnum::INVALID);
            const SurfaceTypeEnum::Enum surfaceType = 
                surfaceClass->getEnumeratedTypeValue<SurfaceTypeEnum,SurfaceTypeEnum::Enum>("surfaceType", 
                                                                                            SurfaceTypeEnum::UNKNOWN);
            const AString surfaceName = surfaceClass->getStringValue("surfaceName",
                                                                             "");
            if ((tabIndex >= 0) 
                && (structure != StructureEnum::INVALID)
                && (surfaceType != SurfaceTypeEnum::UNKNOWN)
                && (surfaceName.isEmpty() == false)) {
                BrainStructure* brainStructure = getBrain()->getBrainStructure(structure, false);
                if (brainStructure != NULL) {
                    const int32_t numSurfaces = brainStructure->getNumberOfSurfaces();
                    for (int32_t i = 0; i < numSurfaces; i++) {
                        Surface* surface = brainStructure->getSurface(i);
                        const AString loadedSurfaceName = surface->getFileName();
                        if (loadedSurfaceName.endsWith(surfaceName)) {
                            setSelectedSurfaceType(tabIndex, 
                                                   surfaceType);
                            setSelectedSurface(structure, 
                                               tabIndex, 
                                               surface);
                            break;
                        }
                    }
                }
            }
        }
    }

    /*
     * Need to do after
     */
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
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
ModelWholeBrain::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);

    CaretAssertArrayIndex(m_selectedSurfaceType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, sourceTabIndex);
    CaretAssertArrayIndex(m_selectedSurfaceType, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, destinationTabIndex);
    
    m_selectedSurface[destinationTabIndex] = m_selectedSurface[sourceTabIndex];
    m_selectedSurfaceType[destinationTabIndex] = m_selectedSurfaceType[sourceTabIndex];
    m_leftEnabled[destinationTabIndex] = m_leftEnabled[sourceTabIndex];
    m_rightEnabled[destinationTabIndex] = m_rightEnabled[sourceTabIndex];
    m_cerebellumEnabled[destinationTabIndex] = m_cerebellumEnabled[sourceTabIndex];
    m_leftRightSeparation[destinationTabIndex] = m_leftRightSeparation[sourceTabIndex];
    m_cerebellumSeparation[destinationTabIndex] = m_cerebellumSeparation[sourceTabIndex];
    m_overlaySetArray->copyOverlaySet(sourceTabIndex,
                                      destinationTabIndex);
    
}

