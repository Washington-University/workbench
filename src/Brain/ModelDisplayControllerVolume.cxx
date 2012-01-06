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
#include "Overlay.h"
#include "OverlaySet.h"
#include "ModelDisplayControllerVolume.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this volume controller belongs.
 *
 */
ModelDisplayControllerVolume::ModelDisplayControllerVolume(Brain* brain)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_VOLUME_SLICES,
                         YOKING_ALLOWED_NO,
                         ROTATION_ALLOWED_NO,
                         brain)
{
    this->initializeMembersModelDisplayControllerVolume();
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
}

/**
 * Destructor
 */
ModelDisplayControllerVolume::~ModelDisplayControllerVolume()
{
    EventManager::get()->removeAllEventsFromListener(this);    
}

void
ModelDisplayControllerVolume::initializeMembersModelDisplayControllerVolume()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->sliceViewPlane[i]         = VolumeSliceViewPlaneEnum::AXIAL;
        this->sliceViewMode[i]          = VolumeSliceViewModeEnum::ORTHOGONAL;
        this->montageNumberOfColumns[i] = 3;
        this->montageNumberOfRows[i]    = 3;
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
ModelDisplayControllerVolume::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "Volume";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerVolume::getNameForBrowserTab() const
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
ModelDisplayControllerVolume::getUnderlayVolumeFile(const int32_t windowTabNumber) const
{
    VolumeFile* vf = NULL;
    
    EventBrowserTabGet getBrowserTabEvent(windowTabNumber);
    EventManager::get()->sendEvent(getBrowserTabEvent.getPointer());
    BrowserTabContent* btc = getBrowserTabEvent.getBrowserTab();
    if (btc != NULL) {
        OverlaySet* overlaySet = btc->getOverlaySet();
        vf = overlaySet->getUnderlayVolume(btc);
        if (vf == NULL) {
            vf = overlaySet->setUnderlayToVolume(btc);
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
ModelDisplayControllerVolume::getSliceViewPlane(const int32_t windowTabNumber) const
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
ModelDisplayControllerVolume::setSliceViewPlane(const int32_t windowTabNumber,
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
ModelDisplayControllerVolume::getSliceViewMode(const int32_t windowTabNumber) const
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
ModelDisplayControllerVolume::setSliceViewMode(const int32_t windowTabNumber,
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
ModelDisplayControllerVolume::getSelectedVolumeSlices(const int32_t windowTabNumber)
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
ModelDisplayControllerVolume::getSelectedVolumeSlices(const int32_t windowTabNumber) const
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
ModelDisplayControllerVolume::getMontageNumberOfColumns(const int32_t windowTabNumber) const
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
ModelDisplayControllerVolume::setMontageNumberOfColumns(const int32_t windowTabNumber,
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
ModelDisplayControllerVolume::getMontageNumberOfRows(const int32_t windowTabNumber) const
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
ModelDisplayControllerVolume::setMontageNumberOfRows(const int32_t windowTabNumber,
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
ModelDisplayControllerVolume::getMontageSliceSpacing(const int32_t windowTabNumber) const
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
ModelDisplayControllerVolume::setMontageSliceSpacing(const int32_t windowTabNumber,
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
ModelDisplayControllerVolume::updateController(const int32_t windowTabNumber)
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
ModelDisplayControllerVolume::setSlicesToOrigin(const int32_t windowTabNumber)
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
ModelDisplayControllerVolume::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION) {
        EventIdentificationHighlightLocation* idLocationEvent =
        dynamic_cast<EventIdentificationHighlightLocation*>(event);
        CaretAssert(idLocationEvent);

        const float* highlighXYZ = idLocationEvent->getXYZ();
        
        for (int32_t windowTabNumber = 0; 
             windowTabNumber < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; 
             windowTabNumber++) {
             this->volumeSlicesSelected[windowTabNumber].selectSlicesAtCoordinate(highlighXYZ);
        }
        
        idLocationEvent->setEventProcessed();
    }
}


