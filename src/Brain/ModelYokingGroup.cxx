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
#include "CaretAssert.h"
#include "DisplayPropertiesInformation.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventManager.h"
#include "ModelYokingGroup.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param brain 
 *    Brain to which this yoking model belongs.
 * @param yokingType
 *    Type of yoking supported by this yoking model.
 * @param yokingName
 *    Name of this yoking model.
 */
ModelYokingGroup::ModelYokingGroup(Brain* brain,
                                   const YokingType yokingType,
                                   const AString& yokingName)
: Model(ModelTypeEnum::MODEL_TYPE_YOKING,
        YOKING_ALLOWED_NO,
        ((yokingType == YOKING_TYPE_SURFACE) ? ROTATION_ALLOWED_YES : ROTATION_ALLOWED_NO),
        brain),
  m_yokingType(yokingType),
  m_yokingName(yokingName)
{
    m_sliceViewPlane         = VolumeSliceViewPlaneEnum::AXIAL;
    m_sliceViewMode          = VolumeSliceViewModeEnum::ORTHOGONAL;
    m_montageNumberOfColumns = 3;
    m_montageNumberOfRows    = 4;
    m_montageSliceSpacing    = 5;
    m_volumeSlicesSelected.reset();
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add<VolumeSliceViewPlaneEnum, VolumeSliceViewPlaneEnum::Enum>("m_sliceViewPlane", 
                                                                                    &m_sliceViewPlane, 
                                                                                    m_sliceViewPlane);
    m_sceneAssistant->add<VolumeSliceViewModeEnum, VolumeSliceViewModeEnum::Enum>("m_sliceViewMode", 
                                                                                    &m_sliceViewMode, 
                                                                                    m_sliceViewMode);
    m_sceneAssistant->add("m_montageNumberOfColumns",
                          &m_montageNumberOfColumns);
    m_sceneAssistant->add("m_montageNumberOfRows",
                          &m_montageNumberOfRows);
    m_sceneAssistant->add("m_montageSliceSpacing",
                          &m_montageSliceSpacing);
    m_sceneAssistant->add("m_volumeSlicesSelected",
                          "VolumeSliceCoordinateSelection",
                          &m_volumeSlicesSelected);
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
}

/**
 * Destructor
 */
ModelYokingGroup::~ModelYokingGroup()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
}

/**
 * Return the name of the yoking.
 */
AString 
ModelYokingGroup::getYokingName() const
{
    return m_yokingName;
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
ModelYokingGroup::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "ModelYoking";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelYokingGroup::getNameForBrowserTab() const
{
    return "ModelYoking";
}


/**
 * Return the for axis mode in the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return Axis mode.
 *   
 */
VolumeSliceViewPlaneEnum::Enum 
ModelYokingGroup::getSliceViewPlane(const int32_t /*windowTabNumber*/) const
{    
    return m_sliceViewPlane;
}

/**
 * Set the axis mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param slicePlane
 *    New value for slice plane.
 */
void 
ModelYokingGroup::setSliceViewPlane(const int32_t /*windowTabNumber*/,
                      VolumeSliceViewPlaneEnum::Enum slicePlane)
{   
    m_sliceViewPlane = slicePlane;
}

/**
 * Return the view mode for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   View mode.
 */
VolumeSliceViewModeEnum::Enum 
ModelYokingGroup::getSliceViewMode(const int32_t /*windowTabNumber*/) const
{    
    return m_sliceViewMode;
}

/**
 * Set the view mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceViewMode
 *    New value for view mode
 */
void 
ModelYokingGroup::setSliceViewMode(const int32_t /*windowTabNumber*/,
                      VolumeSliceViewModeEnum::Enum sliceViewMode)
{    
    m_sliceViewMode = sliceViewMode;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
VolumeSliceCoordinateSelection* 
ModelYokingGroup::getSelectedVolumeSlices(const int32_t /*windowTabNumber*/)
{
    return &m_volumeSlicesSelected;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
const VolumeSliceCoordinateSelection* 
ModelYokingGroup::getSelectedVolumeSlices(const int32_t /*windowTabNumber*/) const
{
    return &m_volumeSlicesSelected;
}



/**
 * Return the montage number of columns for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of columns 
 */
int32_t 
ModelYokingGroup::getMontageNumberOfColumns(const int32_t /*windowTabNumber*/) const
{    
    return m_montageNumberOfColumns;
}


/**
 * Set the montage number of columns in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageNumberOfColumns
 *    New value for montage number of columns 
 */
void 
ModelYokingGroup::setMontageNumberOfColumns(const int32_t /*windowTabNumber*/,
                               const int32_t montageNumberOfColumns)
{    
    m_montageNumberOfColumns = montageNumberOfColumns;
}

/**
 * Return the montage number of rows for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of rows
 */
int32_t 
ModelYokingGroup::getMontageNumberOfRows(const int32_t /*windowTabNumber*/) const
{
    return m_montageNumberOfRows;
}

/**
 * Set the montage number of rows in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageNumberOfRows
 *    New value for montage number of rows 
 */
void 
ModelYokingGroup::setMontageNumberOfRows(const int32_t /*windowTabNumber*/,
                            const int32_t montageNumberOfRows)
{    
    m_montageNumberOfRows = montageNumberOfRows;
}

/**
 * Return the montage slice spacing for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage slice spacing.
 */
int32_t 
ModelYokingGroup::getMontageSliceSpacing(const int32_t /*windowTabNumber*/) const
{    
    return m_montageSliceSpacing;
}

/**
 * Set the montage slice spacing in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageSliceSpacing
 *    New value for montage slice spacing 
 */
void 
ModelYokingGroup::setMontageSliceSpacing(const int32_t /*windowTabNumber*/,
                            const int32_t montageSliceSpacing)
{
    m_montageSliceSpacing = montageSliceSpacing;
}

/**
 * Set the selected slices to the origin.
 * @param  windowTabNumber  Window for which slices set to origin is requested.
 */
void
ModelYokingGroup::setSlicesToOrigin(const int32_t /*windowTabNumber*/)
{
    m_volumeSlicesSelected.selectSlicesAtOrigin();
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelYokingGroup::getOverlaySet(const int /*tabIndex*/)
{
    CaretAssertMessage(0, "NEVER should be called.");
    return NULL;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelYokingGroup::getOverlaySet(const int /*tabIndex*/) const
{
    CaretAssertMessage(0, "NEVER should be called.");
    return NULL;
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelYokingGroup::initializeOverlays()
{
    CaretAssertMessage(0, "NEVER should be called.");
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
ModelYokingGroup::copyTransformationsAndViews(const Model& controllerSource,
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
}

/**
 * @return The type of yoking.
 */
ModelYokingGroup::YokingType 
ModelYokingGroup::getYokingType() const
{
    return m_yokingType;
}

/**
 * @return Is this surface yoking?
 */
bool 
ModelYokingGroup::isSurfaceYoking() const
{
    return (m_yokingType == YOKING_TYPE_SURFACE);
}

/**
 * @return Is this volume yoking?
 */
bool 
ModelYokingGroup::isVolumeYoking() const
{
    return (m_yokingType == YOKING_TYPE_VOLUME);
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
ModelYokingGroup::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idLocationEvent =
        dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idLocationEvent);
        
        if (getBrain()->getDisplayPropertiesInformation()->isVolumeIdentificationEnabled()) {
            const float* highlighXYZ = idLocationEvent->getXYZ();
            float sliceXYZ[3] = {
                highlighXYZ[0],
                highlighXYZ[1],
                highlighXYZ[2]
            };
            
            switch (m_sliceViewMode) {
                case VolumeSliceViewModeEnum::MONTAGE:
                    /*
                     * For montage, do not allow slice in selected plane change
                     */
                    switch (m_sliceViewPlane) {
                        case VolumeSliceViewPlaneEnum::ALL:
                            break;
                        case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                            sliceXYZ[0] = m_volumeSlicesSelected.getSliceCoordinateParasagittal();
                            break;
                        case VolumeSliceViewPlaneEnum::CORONAL:
                            sliceXYZ[1] = m_volumeSlicesSelected.getSliceCoordinateCoronal();
                            break;
                        case VolumeSliceViewPlaneEnum::AXIAL:
                            sliceXYZ[2] = m_volumeSlicesSelected.getSliceCoordinateAxial();
                            break;
                    }
                    break;
                case VolumeSliceViewModeEnum::OBLIQUE:
                    break;
                case VolumeSliceViewModeEnum::ORTHOGONAL:
                    break;
            }
            
            m_volumeSlicesSelected.selectSlicesAtCoordinate(sliceXYZ);
        }
        
        idLocationEvent->setEventProcessed();
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
ModelYokingGroup::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                      SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
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
ModelYokingGroup::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
}

