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

#include "Brain.h"
#include "BrowserTabContent.h"
#include "EventBrowserTabGet.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventManager.h"
#include "IdentificationManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "ModelVolume.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneClassArray.h"
#include "SceneEnumeratedType.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePrimitive.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this volume controller belongs.
 *
 */
ModelVolume::ModelVolume(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_NO,
                         brain)
{
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySet[i] = new OverlaySet(this);
        m_sliceViewPlane[i]         = VolumeSliceViewPlaneEnum::AXIAL;
        m_sliceViewMode[i]          = VolumeSliceViewModeEnum::ORTHOGONAL;
        m_montageNumberOfColumns[i] = 3;
        m_montageNumberOfRows[i]    = 4;
        m_montageSliceSpacing[i]    = 5;
        m_volumeSlicesSelected[i].reset();
    }
    m_lastVolumeFile = NULL;
    
    /*
     * Scene note: overlaySet is restored by parent class
     */ 
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<VolumeSliceViewPlaneEnum, VolumeSliceViewPlaneEnum::Enum>("m_sliceViewPlane", m_sliceViewPlane);
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<VolumeSliceViewModeEnum, VolumeSliceViewModeEnum::Enum>("m_sliceViewMode", m_sliceViewMode);
    m_sceneAssistant->addTabIndexedIntegerArray("m_montageNumberOfRows", m_montageNumberOfRows);
    m_sceneAssistant->addTabIndexedIntegerArray("m_montageNumberOfColumns", m_montageNumberOfColumns);
    m_sceneAssistant->addTabIndexedIntegerArray("m_montageSliceSpacing", m_montageSliceSpacing);
}

/**
 * Destructor
 */
ModelVolume::~ModelVolume()
{
    EventManager::get()->removeAllEventsFromListener(this);    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_overlaySet[i];
    }
    
    delete m_sceneAssistant;
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelVolume::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "Volume";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelVolume::getNameForBrowserTab() const
{
    return "Volume";
}

/**
 * Get the bottom-most active volume in the given window tab.
 * If no overlay is set to volume data, one will be set to a 
 * volume if there is a volume loaded.
 * @param windowTabNumber 
 *    Tab number for content.
 * @return 
 *    Bottom-most volume or NULL if no volumes available.
 */
VolumeFile* 
ModelVolume::getUnderlayVolumeFile(const int32_t windowTabNumber) const
{
    VolumeFile* vf = NULL;
    
    EventBrowserTabGet getBrowserTabEvent(windowTabNumber);
    EventManager::get()->sendEvent(getBrowserTabEvent.getPointer());
    BrowserTabContent* btc = getBrowserTabEvent.getBrowserTab();
    if (btc != NULL) {
        OverlaySet* overlaySet = btc->getOverlaySet();
        vf = overlaySet->getUnderlayVolume();
        if (vf == NULL) {
            vf = overlaySet->setUnderlayToVolume();
        }
    }
    
    return vf;
}

/**
 * Return the for axis mode in the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return Axis mode.
 *   
 */
VolumeSliceViewPlaneEnum::Enum 
ModelVolume::getSliceViewPlane(const int32_t windowTabNumber) const
{    
    return m_sliceViewPlane[windowTabNumber];
}

/**
 * Set the axis mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param slicePlane
 *    New value for slice plane.
 */
void 
ModelVolume::setSliceViewPlane(const int32_t windowTabNumber,
                      VolumeSliceViewPlaneEnum::Enum slicePlane)
{   
    m_sliceViewPlane[windowTabNumber] = slicePlane;
}

/**
 * Return the view mode for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   View mode.
 */
VolumeSliceViewModeEnum::Enum 
ModelVolume::getSliceViewMode(const int32_t windowTabNumber) const
{    
    return m_sliceViewMode[windowTabNumber];
}

/**
 * Set the view mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceViewMode
 *    New value for view mode
 */
void 
ModelVolume::setSliceViewMode(const int32_t windowTabNumber,
                      VolumeSliceViewModeEnum::Enum sliceViewMode)
{    
    m_sliceViewMode[windowTabNumber] = sliceViewMode;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
VolumeSliceCoordinateSelection* 
ModelVolume::getSelectedVolumeSlices(const int32_t windowTabNumber)
{
    const VolumeFile* vf = getUnderlayVolumeFile(windowTabNumber);
    m_volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
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
ModelVolume::getSelectedVolumeSlices(const int32_t windowTabNumber) const
{
    const VolumeFile* vf = getUnderlayVolumeFile(windowTabNumber);
    m_volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    return &m_volumeSlicesSelected[windowTabNumber];
}



/**
 * Return the montage number of columns for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of columns 
 */
int32_t 
ModelVolume::getMontageNumberOfColumns(const int32_t windowTabNumber) const
{    
    return m_montageNumberOfColumns[windowTabNumber];
}


/**
 * Set the montage number of columns in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageNumberOfColumns
 *    New value for montage number of columns 
 */
void 
ModelVolume::setMontageNumberOfColumns(const int32_t windowTabNumber,
                               const int32_t montageNumberOfColumns)
{    
    m_montageNumberOfColumns[windowTabNumber] = montageNumberOfColumns;
}

/**
 * Return the montage number of rows for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of rows
 */
int32_t 
ModelVolume::getMontageNumberOfRows(const int32_t windowTabNumber) const
{
    return m_montageNumberOfRows[windowTabNumber];
}

/**
 * Set the montage number of rows in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageNumberOfRows
 *    New value for montage number of rows 
 */
void 
ModelVolume::setMontageNumberOfRows(const int32_t windowTabNumber,
                            const int32_t montageNumberOfRows)
{    
    m_montageNumberOfRows[windowTabNumber] = montageNumberOfRows;
}

/**
 * Return the montage slice spacing for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage slice spacing.
 */
int32_t 
ModelVolume::getMontageSliceSpacing(const int32_t windowTabNumber) const
{    
    return m_montageSliceSpacing[windowTabNumber];
}

/**
 * Set the montage slice spacing in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageSliceSpacing
 *    New value for montage slice spacing 
 */
void 
ModelVolume::setMontageSliceSpacing(const int32_t windowTabNumber,
                            const int32_t montageSliceSpacing)
{
    m_montageSliceSpacing[windowTabNumber] = montageSliceSpacing;
}

/**
 * Update the controller.
 * @param windowTabNumber
 *    Tab number of window.
 */
void 
ModelVolume::updateController(const int32_t windowTabNumber)
{
    VolumeFile* vf = getUnderlayVolumeFile(windowTabNumber);
    if (vf != NULL) {
        m_volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    }
}

/**
 * Set the selected slices to the origin.
 * @param  windowTabNumber  Window for which slices set to origin is requested.
 */
void
ModelVolume::setSlicesToOrigin(const int32_t windowTabNumber)
{
    m_volumeSlicesSelected[windowTabNumber].selectSlicesAtOrigin();
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
ModelVolume::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idLocationEvent =
        dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idLocationEvent);

        if (getBrain()->getIdentificationManager()->isVolumeIdentificationEnabled()) {
            const float* highlighXYZ = idLocationEvent->getXYZ();
            for (int32_t windowTabNumber = 0; 
                 windowTabNumber < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; 
                 windowTabNumber++) {
                
                float sliceXYZ[3] = {
                    highlighXYZ[0],
                    highlighXYZ[1],
                    highlighXYZ[2]
                };
                
                switch (getSliceViewMode(windowTabNumber)) {
                    case VolumeSliceViewModeEnum::MONTAGE:
                        /*
                         * For montage, do not allow slice in selected plane change
                         */
                        switch (getSliceViewPlane(windowTabNumber)) {
                            case VolumeSliceViewPlaneEnum::ALL:
                                break;
                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                sliceXYZ[0] = m_volumeSlicesSelected[windowTabNumber].getSliceCoordinateParasagittal();
                                break;
                            case VolumeSliceViewPlaneEnum::CORONAL:
                                sliceXYZ[1] = m_volumeSlicesSelected[windowTabNumber].getSliceCoordinateCoronal();
                                break;
                            case VolumeSliceViewPlaneEnum::AXIAL:
                                sliceXYZ[2] = m_volumeSlicesSelected[windowTabNumber].getSliceCoordinateAxial();
                                break;
                        }
                        break;
                    case VolumeSliceViewModeEnum::OBLIQUE:
                        break;
                    case VolumeSliceViewModeEnum::ORTHOGONAL:
                        break;
                }
                
                m_volumeSlicesSelected[windowTabNumber].selectSlicesAtCoordinate(sliceXYZ);
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
ModelVolume::getOverlaySet(const int tabIndex)
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
ModelVolume::getOverlaySet(const int tabIndex) const
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
ModelVolume::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_overlaySet[i]->initializeOverlays();
        
        VolumeFile* vf = m_overlaySet[i]->getUnderlayVolume();
        if (vf != NULL) {
            /*
             * Set montage slice spacing based upon slices
             * in the Z-axis.
             */
            std::vector<int64_t> dimensions;
            vf->getDimensions(dimensions);
            
            if (dimensions.size() >= 3) {
                const int32_t dimZ = dimensions[2];
                if (dimZ > 0) {
                    const int32_t maxZ = static_cast<int32_t>(dimZ * 0.90);
                    const int32_t minZ = static_cast<int32_t>(dimZ * 0.10);
                    const int32_t sliceRange = (maxZ - minZ);
                    int32_t sliceSpacing = 1;
                    if (sliceRange > 0) {
                        const int32_t numSlicesViewed = (m_montageNumberOfRows[i]
                                                         * m_montageNumberOfColumns[i]);
                        sliceSpacing = (sliceRange / numSlicesViewed);
                    }
                    m_montageSliceSpacing[i] = sliceSpacing;
                }
            }
        }
    }
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
ModelVolume::copyTransformationsAndViews(const Model& controllerSource,
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

    const ModelVolumeInterface* modelVolumeSource = dynamic_cast<const ModelVolumeInterface*>(&controllerSource);
    if (modelVolumeSource == NULL) {
        return;
    }
    
    setSliceViewPlane(windowTabNumberTarget, 
                            modelVolumeSource->getSliceViewPlane(windowTabNumberSource));
    setSliceViewMode(windowTabNumberTarget,
                           modelVolumeSource->getSliceViewMode(windowTabNumberSource));
    setMontageNumberOfRows(windowTabNumberTarget,
                                 modelVolumeSource->getMontageNumberOfRows(windowTabNumberSource));
    setMontageNumberOfColumns(windowTabNumberTarget,
                                    modelVolumeSource->getMontageNumberOfColumns(windowTabNumberSource));
    setMontageSliceSpacing(windowTabNumberTarget,
                                 modelVolumeSource->getMontageSliceSpacing(windowTabNumberSource));
    
    getSelectedVolumeSlices(windowTabNumberTarget)->copySelections(
                                                                         *modelVolumeSource->getSelectedVolumeSlices(windowTabNumberSource));
/*
    const ModelVolume* modelVolumeSource = dynamic_cast<const ModelVolume*>(&controllerSource);
    if (modelVolumeSource == NULL) {
        return;
    }

    m_sliceViewPlane[windowTabNumberTarget] = modelVolumeSource->sliceViewPlane[windowTabNumberSource];
    m_sliceViewMode[windowTabNumberTarget] = modelVolumeSource->sliceViewMode[windowTabNumberSource];
    m_montageNumberOfRows[windowTabNumberTarget] = modelVolumeSource->montageNumberOfRows[windowTabNumberSource];
    m_montageNumberOfColumns[windowTabNumberTarget] = modelVolumeSource->montageNumberOfColumns[windowTabNumberSource];
    m_montageSliceSpacing[windowTabNumberTarget] = modelVolumeSource->montageSliceSpacing[windowTabNumberSource];
                      
    m_volumeSlicesSelected[windowTabNumberTarget].copySelections(modelVolumeSource->volumeSlicesSelected[windowTabNumberSource]);
*/
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
ModelVolume::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    /*
     * Get indices of tabs that are to be saved to scene.
     */ 
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    const int32_t numActiveTabs = static_cast<int32_t>(tabIndices.size()); 
    
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
//    /*
//     * View mode
//     */
//    SceneObjectMapIntegerKey* sliceViewModeMap = new SceneObjectMapIntegerKey("m_sliceViewMode",
//                                                                  SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE);
//    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
//        const int32_t tabIndex = tabIndices[iat];
//        sliceViewModeMap->addEnumeratedType(tabIndex, 
//                                            VolumeSliceViewModeEnum::toName(m_sliceViewMode[tabIndex]));
//    }
//    sceneClass->addChild(sliceViewModeMap);
//    
//    /*
//     * View plane
//     */
//    SceneObjectMapIntegerKey* sliceViewPlaneMap = new SceneObjectMapIntegerKey("m_sliceViewPlane",
//                                                                  SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE);
//    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
//        const int32_t tabIndex = tabIndices[iat];
//        sliceViewPlaneMap->addEnumeratedType(tabIndex, 
//                                            VolumeSliceViewPlaneEnum::toName(m_sliceViewPlane[tabIndex]));
//    }
//    sceneClass->addChild(sliceViewPlaneMap);
//    
//    /*
//     * Montage rows
//     */
//    SceneObjectMapIntegerKey* montageRowMap = new SceneObjectMapIntegerKey("m_montageNumberOfRows",
//                                                               SceneObjectDataTypeEnum::SCENE_INTEGER);
//    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
//        const int32_t tabIndex = tabIndices[iat];
//        montageRowMap->addInteger(tabIndex, 
//                                  m_montageNumberOfRows[tabIndex]);
//    }
//    sceneClass->addChild(montageRowMap);
//    
//    /*
//     * Montage columns
//     */
//    SceneObjectMapIntegerKey* montageColumnMap = new SceneObjectMapIntegerKey("m_montageNumberOfColumns",
//                                                               SceneObjectDataTypeEnum::SCENE_INTEGER);
//    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
//        const int32_t tabIndex = tabIndices[iat];
//        montageColumnMap->addInteger(tabIndex, 
//                                     m_montageNumberOfColumns[tabIndex]);
//    }
//    sceneClass->addChild(montageColumnMap);
//    
//    /*
//     * Montage slice spacing
//     */
//    SceneObjectMapIntegerKey* montageSliceSpacingMap = new SceneObjectMapIntegerKey("m_montageSliceSpacing",
//                                                                  SceneObjectDataTypeEnum::SCENE_INTEGER);
//    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
//        const int32_t tabIndex = tabIndices[iat];
//        montageSliceSpacingMap->addInteger(tabIndex, 
//                                     m_montageSliceSpacing[tabIndex]);
//    }
//    sceneClass->addChild(montageSliceSpacingMap);
    
    /*
     * Slice selection
     */
    SceneObjectMapIntegerKey* sliceSelectionMap = new SceneObjectMapIntegerKey("m_volumeSlicesSelected",
                                                                   SceneObjectDataTypeEnum::SCENE_CLASS);
    for (int32_t iat = 0; iat < numActiveTabs; iat++) {
        const int32_t tabIndex = tabIndices[iat];
        sliceSelectionMap->addClass(tabIndex, 
                                    m_volumeSlicesSelected[tabIndex].saveToScene(sceneAttributes, 
                                                                                 ("m_volumeSlicesSelected["
                                                                                  + AString::number(tabIndex)
                                                                                  + "]")));
    }
    sceneClass->addChild(sliceSelectionMap);
    
    
    //    m_sceneAssistant->saveMembers(sceneAttributes, 
    //                                  sceneClass);
    //    
    //    sceneClass->addString("m_selectedMapFile",
    //                          m_selectedMapFile->getFileNameNoPath());
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
ModelVolume::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    /*
     * View mode
     */
//    const SceneObjectMapIntegerKey* sliceViewModeMap = sceneClass->getMapIntegerKey("m_sliceViewMode");
//    if (sliceViewModeMap != NULL) {
//        const std::map<int32_t, SceneObject*>& sliceViewData = sliceViewModeMap->getMap();
//        for (std::map<int32_t, SceneObject*>::const_iterator iter = sliceViewData.begin();
//             iter != sliceViewData.end();
//             iter++) {
//            const int32_t key = iter->first;
//            const SceneEnumeratedType* enumType = dynamic_cast<SceneEnumeratedType*>(iter->second);
//            m_sliceViewMode[key] = VolumeSliceViewModeEnum::fromName(enumType->stringValue(), 
//                                                                     NULL);
//        }
//    }
//    
//    /*
//     * View plane
//     */
//    const SceneObjectMapIntegerKey* sliceViewPlaneMap = sceneClass->getMapIntegerKey("m_sliceViewPlane");
//    if (sliceViewPlaneMap != NULL) {
//        const std::map<int32_t, SceneObject*>& slicePlaneData = sliceViewPlaneMap->getMap();
//        for (std::map<int32_t, SceneObject*>::const_iterator iter = slicePlaneData.begin();
//             iter != slicePlaneData.end();
//             iter++) {
//            const int32_t key = iter->first;
//            const SceneEnumeratedType* enumType = dynamic_cast<SceneEnumeratedType*>(iter->second);
//            m_sliceViewPlane[key] = VolumeSliceViewPlaneEnum::fromName(enumType->stringValue(), 
//                                                                     NULL);
//        }
//    }
//
//    /*
//     * Montage rows
//     */
//    const SceneObjectMapIntegerKey* montageRowMap = sceneClass->getMapIntegerKey("m_montageNumberOfRows");
//    if (montageRowMap != NULL) {
//        const std::map<int32_t, SceneObject*>& montageRowData = montageRowMap->getMap();
//        for (std::map<int32_t, SceneObject*>::const_iterator iter = montageRowData.begin();
//             iter != montageRowData.end();
//             iter++) {
//            const int32_t key = iter->first;
//            const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
//            m_montageNumberOfRows[key] = primitive->integerValue();
//        }
//    }
//    
//    /*
//     * Montage columns
//     */
//    const SceneObjectMapIntegerKey* montageColMap = sceneClass->getMapIntegerKey("m_montageNumberOfColumns");
//    if (montageColMap != NULL) {
//        const std::map<int32_t, SceneObject*>& montageColData = montageColMap->getMap();
//        for (std::map<int32_t, SceneObject*>::const_iterator iter = montageColData.begin();
//             iter != montageColData.end();
//             iter++) {
//            const int32_t key = iter->first;
//            const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
//            m_montageNumberOfColumns[key] = primitive->integerValue();
//        }
//    }
//
//    
//    /*
//     * Montage slice spacing
//     */
//    const SceneObjectMapIntegerKey* montageSliceSpaceMap = sceneClass->getMapIntegerKey("m_montageSliceSpacing");
//    if (montageSliceSpaceMap) {
//        const std::map<int32_t, SceneObject*>& montageSliceSpaceData = montageSliceSpaceMap->getMap();
//        for (std::map<int32_t, SceneObject*>::const_iterator iter = montageSliceSpaceData.begin();
//             iter != montageSliceSpaceData.end();
//             iter++) {
//            const int32_t key = iter->first;
//            const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
//            m_montageSliceSpacing[key] = primitive->integerValue();
//        }
//    }
    
    /*
     * Slice selection
     */
    const SceneObjectMapIntegerKey* sliceSelectionMap = sceneClass->getMapIntegerKey("m_volumeSlicesSelected");
    if (sliceSelectionMap != NULL) {
        const std::map<int32_t, SceneObject*>& sliceSelectionData = sliceSelectionMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = sliceSelectionData.begin();
             iter != sliceSelectionData.end();
             iter++) {
            const int32_t key = iter->first;
            const SceneClass* slicesClass = dynamic_cast<const SceneClass*>(iter->second);
            m_volumeSlicesSelected[key].restoreFromScene(sceneAttributes, slicesClass);
        }
    }
}

