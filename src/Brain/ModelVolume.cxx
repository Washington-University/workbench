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
#include "DisplayPropertiesInformation.h"
#include "EventBrowserTabGet.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventManager.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "ModelVolume.h"
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
    this->initializeMembersModelVolume();
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i] = new OverlaySet(this);
    }
}

/**
 * Destructor
 */
ModelVolume::~ModelVolume()
{
    EventManager::get()->removeAllEventsFromListener(this);    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete this->overlaySet[i];
    }
}

void
ModelVolume::initializeMembersModelVolume()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->sliceViewPlane[i]         = VolumeSliceViewPlaneEnum::AXIAL;
        this->sliceViewMode[i]          = VolumeSliceViewModeEnum::ORTHOGONAL;
        this->montageNumberOfColumns[i] = 3;
        this->montageNumberOfRows[i]    = 4;
        this->montageSliceSpacing[i]    = 5;
        this->volumeSlicesSelected[i].reset();
    }
    this->lastVolumeFile = NULL;
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
    return this->sliceViewPlane[windowTabNumber];
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
    this->sliceViewPlane[windowTabNumber] = slicePlane;
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
    return this->sliceViewMode[windowTabNumber];
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
    this->sliceViewMode[windowTabNumber] = sliceViewMode;
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
    const VolumeFile* vf = this->getUnderlayVolumeFile(windowTabNumber);
    this->volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    return &this->volumeSlicesSelected[windowTabNumber];
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
    const VolumeFile* vf = this->getUnderlayVolumeFile(windowTabNumber);
    this->volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    return &this->volumeSlicesSelected[windowTabNumber];
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
    return this->montageNumberOfColumns[windowTabNumber];
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
    this->montageNumberOfColumns[windowTabNumber] = montageNumberOfColumns;
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
    return this->montageNumberOfRows[windowTabNumber];
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
    this->montageNumberOfRows[windowTabNumber] = montageNumberOfRows;
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
    return this->montageSliceSpacing[windowTabNumber];
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
    this->montageSliceSpacing[windowTabNumber] = montageSliceSpacing;
}

/**
 * Update the controller.
 * @param windowTabNumber
 *    Tab number of window.
 */
void 
ModelVolume::updateController(const int32_t windowTabNumber)
{
    VolumeFile* vf = this->getUnderlayVolumeFile(windowTabNumber);
    if (vf != NULL) {
        this->volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    }
}

/**
 * Set the selected slices to the origin.
 * @param  windowTabNumber  Window for which slices set to origin is requested.
 */
void
ModelVolume::setSlicesToOrigin(const int32_t windowTabNumber)
{
    this->volumeSlicesSelected[windowTabNumber].selectSlicesAtOrigin();
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

        if (this->getBrain()->getDisplayPropertiesInformation()->isVolumeIdentificationEnabled()) {
            const float* highlighXYZ = idLocationEvent->getXYZ();
            for (int32_t windowTabNumber = 0; 
                 windowTabNumber < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; 
                 windowTabNumber++) {
                
                float sliceXYZ[3] = {
                    highlighXYZ[0],
                    highlighXYZ[1],
                    highlighXYZ[2]
                };
                
                switch (this->getSliceViewMode(windowTabNumber)) {
                    case VolumeSliceViewModeEnum::MONTAGE:
                        /*
                         * For montage, do not allow slice in selected plane change
                         */
                        switch (this->getSliceViewPlane(windowTabNumber)) {
                            case VolumeSliceViewPlaneEnum::ALL:
                                break;
                            case VolumeSliceViewPlaneEnum::PARASAGITTAL:
                                sliceXYZ[0] = this->volumeSlicesSelected[windowTabNumber].getSliceCoordinateParasagittal();
                                break;
                            case VolumeSliceViewPlaneEnum::CORONAL:
                                sliceXYZ[1] = this->volumeSlicesSelected[windowTabNumber].getSliceCoordinateCoronal();
                                break;
                            case VolumeSliceViewPlaneEnum::AXIAL:
                                sliceXYZ[2] = this->volumeSlicesSelected[windowTabNumber].getSliceCoordinateAxial();
                                break;
                        }
                        break;
                    case VolumeSliceViewModeEnum::OBLIQUE:
                        break;
                    case VolumeSliceViewModeEnum::ORTHOGONAL:
                        break;
                }
                
                this->volumeSlicesSelected[windowTabNumber].selectSlicesAtCoordinate(sliceXYZ);
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
    CaretAssertArrayIndex(this->overlaySet, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return this->overlaySet[tabIndex];
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
    CaretAssertArrayIndex(this->overlaySet, 
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                          tabIndex);
    return this->overlaySet[tabIndex];
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelVolume::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i]->initializeOverlays();
        
        VolumeFile* vf = this->overlaySet[i]->getUnderlayVolume();
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
                        const int32_t numSlicesViewed = (this->montageNumberOfRows[i]
                                                         * this->montageNumberOfColumns[i]);
                        sliceSpacing = (sliceRange / numSlicesViewed);
                    }
                    this->montageSliceSpacing[i] = sliceSpacing;
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
    
    CaretAssertArrayIndex(this->translation,
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
    
    this->setSliceViewPlane(windowTabNumberTarget, 
                            modelVolumeSource->getSliceViewPlane(windowTabNumberSource));
    this->setSliceViewMode(windowTabNumberTarget,
                           modelVolumeSource->getSliceViewMode(windowTabNumberSource));
    this->setMontageNumberOfRows(windowTabNumberTarget,
                                 modelVolumeSource->getMontageNumberOfRows(windowTabNumberSource));
    this->setMontageNumberOfColumns(windowTabNumberTarget,
                                    modelVolumeSource->getMontageNumberOfColumns(windowTabNumberSource));
    this->setMontageSliceSpacing(windowTabNumberTarget,
                                 modelVolumeSource->getMontageSliceSpacing(windowTabNumberSource));
    
    this->getSelectedVolumeSlices(windowTabNumberTarget)->copySelections(
                                                                         *modelVolumeSource->getSelectedVolumeSlices(windowTabNumberSource));
/*
    const ModelVolume* modelVolumeSource = dynamic_cast<const ModelVolume*>(&controllerSource);
    if (modelVolumeSource == NULL) {
        return;
    }

    this->sliceViewPlane[windowTabNumberTarget] = modelVolumeSource->sliceViewPlane[windowTabNumberSource];
    this->sliceViewMode[windowTabNumberTarget] = modelVolumeSource->sliceViewMode[windowTabNumberSource];
    this->montageNumberOfRows[windowTabNumberTarget] = modelVolumeSource->montageNumberOfRows[windowTabNumberSource];
    this->montageNumberOfColumns[windowTabNumberTarget] = modelVolumeSource->montageNumberOfColumns[windowTabNumberSource];
    this->montageSliceSpacing[windowTabNumberTarget] = modelVolumeSource->montageSliceSpacing[windowTabNumberSource];
                      
    this->volumeSlicesSelected[windowTabNumberTarget].copySelections(modelVolumeSource->volumeSlicesSelected[windowTabNumberSource]);
*/}
