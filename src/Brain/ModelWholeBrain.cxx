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

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "DisplayPropertiesInformation.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventModelGetAll.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "OverlaySet.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this whole brain controller belongs.
 *
 */
ModelWholeBrain::ModelWholeBrain(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES,
                         brain)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_selectedSurfaceType[i] = SurfaceTypeEnum::ANATOMICAL;
        m_cerebellumEnabled[i] = true;
        m_leftEnabled[i] = true;
        m_rightEnabled[i] = true;
        m_leftRightSeparation[i] = 0.0;
        m_cerebellumSeparation[i] = 0.0;        
        m_volumeSlicesSelected[i].reset();
        m_overlaySet[i] = new OverlaySet(this);
    }
    
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
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_overlaySet[i];
    }
    
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
    updateController();
    
    surfaceTypesOut.clear();
    surfaceTypesOut.insert(surfaceTypesOut.end(),
                           m_availableSurfaceTypes.begin(),
                           m_availableSurfaceTypes.end());
}
/**
 *
 */

SurfaceTypeEnum::Enum 
ModelWholeBrain::getSelectedSurfaceType(const int32_t windowTabNumber)
{
    updateController();
    return m_selectedSurfaceType[windowTabNumber];    
}

/**
 * Update this controller.
 */
void 
ModelWholeBrain::updateController()
{
    /*
     * Get all model controllers to find loaded surface types.
     */
    EventModelGetAll eventGetControllers;
    EventManager::get()->sendEvent(eventGetControllers.getPointer());
    const std::vector<Model*> allControllers =
        eventGetControllers.getModels();

    /*
     * Get ALL possible surface types.
     */
    std::vector<SurfaceTypeEnum::Enum> allSurfaceTypes;
    SurfaceTypeEnum::getAllEnums(allSurfaceTypes);
    const int32_t numEnumTypes = static_cast<int32_t>(allSurfaceTypes.size());
    std::vector<bool> surfaceTypeValid(numEnumTypes, false);
    
    /*
     * Find surface types that are actually used.
     */
    for (std::vector<Model*>::const_iterator iter = allControllers.begin();
         iter != allControllers.end();
         iter++) {
        ModelSurface* surfaceController = 
            dynamic_cast<ModelSurface*>(*iter);
        if (surfaceController != NULL) {
            SurfaceTypeEnum::Enum surfaceType = surfaceController->getSurface()->getSurfaceType();
            
            for (int i = 0; i < numEnumTypes; i++) {
                if (allSurfaceTypes[i] == surfaceType) {
                    surfaceTypeValid[i] = true;
                    break;
                }
            }
        }
    }
    
    /*
     * Set the available surface types.
     */
    m_availableSurfaceTypes.clear();
    for (int i = 0; i < numEnumTypes; i++) {
        if (surfaceTypeValid[i]) {
            m_availableSurfaceTypes.push_back(allSurfaceTypes[i]);
        }
    }
    
    
    
    /*
     * Update the selected surface and volume types.
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (std::find(m_availableSurfaceTypes.begin(),
                      m_availableSurfaceTypes.end(),
                      m_selectedSurfaceType[i]) == m_availableSurfaceTypes.end()) {
            if (m_availableSurfaceTypes.empty() == false) {
                m_selectedSurfaceType[i] = m_availableSurfaceTypes[0];
            }
            else {
                m_selectedSurfaceType[i] = SurfaceTypeEnum::ANATOMICAL;
            }
        }
        
        VolumeFile* vf = getUnderlayVolumeFile(i);
        if (vf != NULL) {
            m_volumeSlicesSelected[i].updateForVolumeFile(vf);
        }
    }
}

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
    
//    /*
//     * If surface type is neither anatomical nor reconstruction,
//     * turn of the display of volume slices.
//     */
//    switch (surfaceType) {
//        case SurfaceTypeEnum::ANATOMICAL:
//            break;
//        case SurfaceTypeEnum::RECONSTRUCTION:
//            break;
//        default:
//            m_volumeSlicesSelected[windowTabNumber].setSliceAxialEnabled(false);
//            m_volumeSlicesSelected[windowTabNumber].setSliceCoronalEnabled(false);
//            m_volumeSlicesSelected[windowTabNumber].setSliceParasagittalEnabled(false);
//            break;
//    }
    updateController();
}

/**
 * @return Enabled status for left cerebral cortex.
 */
bool 
ModelWholeBrain::isLeftEnabled(const int32_t windowTabNumber) const
{
    return m_leftEnabled[windowTabNumber];
}

/**
 * Set the enabled status for the left hemisphere.
 * @param windowTabNumber
 *    Index of window tab.
 * @param enabled
 *    New enabled status.
 */
void 
ModelWholeBrain::setLeftEnabled(const int32_t windowTabNumber,
                                                 const bool enabled)
{
    m_leftEnabled[windowTabNumber] = enabled;
}

/**
 * @return Enabled status for right cerebral cortex.
 */
bool 
ModelWholeBrain::isRightEnabled(const int32_t windowTabNumber) const
{
    return m_rightEnabled[windowTabNumber];    
}

/**
 * Set the enabled status for the right hemisphere.
 * @param windowTabNumber
 *    Index of window tab.
 * @param enabled
 *    New enabled status.
 */
void 
ModelWholeBrain::setRightEnabled(const int32_t windowTabNumber,
                                                  const bool enabled)
{
    m_rightEnabled[windowTabNumber] = enabled;
}

/**
 * @return Enabled status for cerebellum.
 */
bool 
ModelWholeBrain::isCerebellumEnabled(const int32_t windowTabNumber) const
{
    return m_cerebellumEnabled[windowTabNumber];
}

/**
 * Set the enabled status for the cerebellum.
 * @param windowTabNumber
 *    Index of window tab.
 * @param enabled
 *    New enabled status.
 */
void 
ModelWholeBrain::setCerebellumEnabled(const int32_t windowTabNumber,
                                                       const bool enabled)
{
    m_cerebellumEnabled[windowTabNumber] = enabled;
}

/**
 * @return The separation between the left and right surfaces.
 */
float 
ModelWholeBrain::getLeftRightSeparation(const int32_t windowTabNumber) const
{
    return m_leftRightSeparation[windowTabNumber];
}

/**
 * Set the separation between the cerebellum and the left/right surfaces.
 * @param windowTabNumber
 *     Index of window tab.
 * @param separation
 *     New value for separation.
 */
void 
ModelWholeBrain::setLeftRightSeparation(const int32_t windowTabNumber,
                            const float separation)
{
    m_leftRightSeparation[windowTabNumber] = separation;
}

/**
 * @return The separation between the left/right surfaces.
 */
float 
ModelWholeBrain::getCerebellumSeparation(const int32_t windowTabNumber) const
{
    return m_cerebellumSeparation[windowTabNumber];
}

/**
 * Set the separation between the cerebellum and the eft and right surfaces.
 * @param windowTabNumber
 *     Index of window tab.
 * @param separation
 *     New value for separation.
 */
void 
ModelWholeBrain::setCerebellumSeparation(const int32_t windowTabNumber,
                                                         const float separation)
{
    m_cerebellumSeparation[windowTabNumber] = separation;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
VolumeSliceCoordinateSelection* 
ModelWholeBrain::getSelectedVolumeSlices(const int32_t windowTabNumber)
{
    m_volumeSlicesSelected[windowTabNumber].updateForVolumeFile(getUnderlayVolumeFile(windowTabNumber));
    return &m_volumeSlicesSelected[windowTabNumber];
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
const VolumeSliceCoordinateSelection* 
ModelWholeBrain::getSelectedVolumeSlices(const int32_t windowTabNumber) const
{
    const VolumeFile* vf = getUnderlayVolumeFile(windowTabNumber);
    m_volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    return &m_volumeSlicesSelected[windowTabNumber];
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
 * displaying this model controller.
 */
AString 
ModelWholeBrain::getNameForBrowserTab() const
{
    return "Whole Brain";
}

/**
 * Get the bottom-most active volume in the given window tab.
 * @param windowTabNumber 
 *    Tab number for content.
 * @return 
 *    Bottom-most volume or NULL if not available (such as 
 *    when all overlay are not volumes or they are disabled).
 */
VolumeFile* 
ModelWholeBrain::getUnderlayVolumeFile(const int32_t windowTabNumber) const
{
    VolumeFile* vf = NULL;
    
    EventBrowserTabGet getBrowserTabEvent(windowTabNumber);
    EventManager::get()->sendEvent(getBrowserTabEvent.getPointer());
    BrowserTabContent* btc = getBrowserTabEvent.getBrowserTab();
    if (btc != NULL) {
        OverlaySet* overlaySet = btc->getOverlaySet();
        vf = overlaySet->getUnderlayVolume();
    }
    
    return vf;
}

/**
 * Set the selected slices to the origin.
 * @param  windowTabNumber  Window for which slices set to origin is requested.
 */
void
ModelWholeBrain::setSlicesToOrigin(const int32_t windowTabNumber)
{
    m_volumeSlicesSelected[windowTabNumber].selectSlicesAtOrigin();
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
ModelWholeBrain::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idLocationEvent =
        dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idLocationEvent);
        
        if (getBrain()->getDisplayPropertiesInformation()->isVolumeIdentificationEnabled()) {
            const float* highlighXYZ = idLocationEvent->getXYZ();
        
            for (int32_t windowTabNumber = 0; 
                 windowTabNumber < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; 
                 windowTabNumber++) {
                m_volumeSlicesSelected[windowTabNumber].selectSlicesAtCoordinate(highlighXYZ);
            }
        }
        
        idLocationEvent->setEventProcessed();
    }
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
ModelWholeBrain::getOverlaySet(const int tabIndex) const
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
ModelWholeBrain::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySet[i]->initializeOverlays();
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
    
    SceneObjectMapIntegerKey* volumeSliceMap = new SceneObjectMapIntegerKey("m_volumeSlicesSelected",
                                                                            SceneObjectDataTypeEnum::SCENE_CLASS);
    for (int32_t i = 0; i < numTabs; i++) {
        const int32_t tabIndex = tabIndices[i];
        const AString name = ("m_volumeSlicesSelected[" + AString::number(tabIndex) + "]");
        volumeSliceMap->addClass(tabIndex, m_volumeSlicesSelected[tabIndex].saveToScene(sceneAttributes, name));
    }
    sceneClass->addChild(volumeSliceMap);
    
    std::vector<SceneClass*> classesForSelectedSurfaceArray;
    for (int32_t i = 0; i < numTabs; i++) {
        const int32_t tabIndex = tabIndices[i];
         
        for (std::map<std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum>, Surface*>::iterator mapIter = m_selectedSurface[tabIndex].begin();
             mapIter != m_selectedSurface[tabIndex].end();
             mapIter++) {
            std::pair<StructureEnum::Enum, SurfaceTypeEnum::Enum> structureSurfaceType = mapIter->first;
            Surface* surface = mapIter->second;
            if (surface != NULL) {
                const StructureEnum::Enum structure = structureSurfaceType.first;
                const SurfaceTypeEnum::Enum surfaceType = structureSurfaceType.second;
                
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
     * Restore selected volume slices (Needs to be done after surface restoration)
     */
    const SceneObjectMapIntegerKey* volumeSliceMap = sceneClass->getMapIntegerKey("m_volumeSlicesSelected");
    if (volumeSliceMap != NULL) {
        const std::vector<int32_t> keys = volumeSliceMap->getKeys();
        const int32_t numKeys = static_cast<int32_t>(keys.size());
        for (int32_t i = 0; i < numKeys; i++) {
            const int32_t tabIndex = keys[i];
            const SceneClass* selectedSlicesClass = dynamic_cast<const SceneClass*>(volumeSliceMap->getObject(tabIndex));
            m_volumeSlicesSelected[tabIndex].restoreFromScene(sceneAttributes, selectedSlicesClass);
        }
    }
    
    /*
     * Need tpo do after
     */
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
}

