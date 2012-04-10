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
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventModelGetAll.h"
#include "ModelSurface.h"
#include "ModelWholeBrain.h"
#include "OverlaySet.h"
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
    this->initializeMembersModelWholeBrain();
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i] = new OverlaySet(this);
    }
}

/**
 * Destructor
 */
ModelWholeBrain::~ModelWholeBrain()
{
    EventManager::get()->removeAllEventsFromListener(this);    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete this->overlaySet[i];
    }
}

/**
 * Initialize members of this class.
 */
void
ModelWholeBrain::initializeMembersModelWholeBrain()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectedSurfaceType[i] = SurfaceTypeEnum::ANATOMICAL;
        this->cerebellumEnabled[i] = true;
        this->leftEnabled[i] = true;
        this->rightEnabled[i] = true;
        this->leftRightSeparation[i] = 0.0;
        this->cerebellumSeparation[i] = 0.0;        
        this->volumeSlicesSelected[i].reset();
    }
}

/**
 * Get the available surface types.
 * @param surfaceTypesOut
 *    Output loaded with the available surface types.
 */
void 
ModelWholeBrain::getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum>& surfaceTypesOut)
{
    this->updateController();
    
    surfaceTypesOut.clear();
    surfaceTypesOut.insert(surfaceTypesOut.end(),
                           this->availableSurfaceTypes.begin(),
                           this->availableSurfaceTypes.end());
}
/**
 *
 */

SurfaceTypeEnum::Enum 
ModelWholeBrain::getSelectedSurfaceType(const int32_t windowTabNumber)
{
    this->updateController();
    return this->selectedSurfaceType[windowTabNumber];    
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
    this->availableSurfaceTypes.clear();
    for (int i = 0; i < numEnumTypes; i++) {
        if (surfaceTypeValid[i]) {
            this->availableSurfaceTypes.push_back(allSurfaceTypes[i]);
        }
    }
    
    
    
    /*
     * Update the selected surface and volume types.
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        if (std::find(this->availableSurfaceTypes.begin(),
                      this->availableSurfaceTypes.end(),
                      this->selectedSurfaceType[i]) == availableSurfaceTypes.end()) {
            if (this->availableSurfaceTypes.empty() == false) {
                this->selectedSurfaceType[i] = this->availableSurfaceTypes[0];
            }
            else {
                this->selectedSurfaceType[i] = SurfaceTypeEnum::ANATOMICAL;
            }
        }
        
        VolumeFile* vf = this->getUnderlayVolumeFile(i);
        if (vf != NULL) {
            this->volumeSlicesSelected[i].updateForVolumeFile(vf);
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
    this->selectedSurfaceType[windowTabNumber] = surfaceType;
    
    /*
     * If surface type is neither anatomical nor reconstruction,
     * turn of the display of volume slices.
     */
    switch (surfaceType) {
        case SurfaceTypeEnum::ANATOMICAL:
            break;
        case SurfaceTypeEnum::RECONSTRUCTION:
            break;
        default:
            this->volumeSlicesSelected[windowTabNumber].setSliceAxialEnabled(false);
            this->volumeSlicesSelected[windowTabNumber].setSliceCoronalEnabled(false);
            this->volumeSlicesSelected[windowTabNumber].setSliceParasagittalEnabled(false);
            break;
    }
    this->updateController();
}

/**
 * @return Enabled status for left cerebral cortex.
 */
bool 
ModelWholeBrain::isLeftEnabled(const int32_t windowTabNumber) const
{
    return this->leftEnabled[windowTabNumber];
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
    this->leftEnabled[windowTabNumber] = enabled;
}

/**
 * @return Enabled status for right cerebral cortex.
 */
bool 
ModelWholeBrain::isRightEnabled(const int32_t windowTabNumber) const
{
    return this->rightEnabled[windowTabNumber];    
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
    this->rightEnabled[windowTabNumber] = enabled;
}

/**
 * @return Enabled status for cerebellum.
 */
bool 
ModelWholeBrain::isCerebellumEnabled(const int32_t windowTabNumber) const
{
    return this->cerebellumEnabled[windowTabNumber];
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
    this->cerebellumEnabled[windowTabNumber] = enabled;
}

/**
 * @return The separation between the left and right surfaces.
 */
float 
ModelWholeBrain::getLeftRightSeparation(const int32_t windowTabNumber) const
{
    return this->leftRightSeparation[windowTabNumber];
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
    this->leftRightSeparation[windowTabNumber] = separation;
}

/**
 * @return The separation between the left/right surfaces.
 */
float 
ModelWholeBrain::getCerebellumSeparation(const int32_t windowTabNumber) const
{
    return this->cerebellumSeparation[windowTabNumber];
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
    this->cerebellumSeparation[windowTabNumber] = separation;
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
    this->volumeSlicesSelected[windowTabNumber].updateForVolumeFile(this->getUnderlayVolumeFile(windowTabNumber));
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
ModelWholeBrain::getSelectedVolumeSlices(const int32_t windowTabNumber) const
{
    const VolumeFile* vf = this->getUnderlayVolumeFile(windowTabNumber);
    this->volumeSlicesSelected[windowTabNumber].updateForVolumeFile(vf);
    return &this->volumeSlicesSelected[windowTabNumber];
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
    this->volumeSlicesSelected[windowTabNumber].selectSlicesAtOrigin();
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
    const SurfaceTypeEnum::Enum surfaceType = this->getSelectedSurfaceType(windowTabNumber);
    std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum> key = 
        std::make_pair(structure, 
                       surfaceType);
    
    /*
     * Get the currently selected surface.
     */
    Surface* s = this->selectedSurface[windowTabNumber][key];
    
    /*
     * See if currently selected surface is valid.
     */
    BrainStructure* brainStructure = this->brain->getBrainStructure(structure, 
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
    
    this->selectedSurface[windowTabNumber][key] = s;
    
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
    const SurfaceTypeEnum::Enum surfaceType = this->getSelectedSurfaceType(windowTabNumber);
    std::pair<StructureEnum::Enum,SurfaceTypeEnum::Enum> key = 
    std::make_pair(structure, 
                   surfaceType);
    
    this->selectedSurface[windowTabNumber][key] = surface;
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
        
        const float* highlighXYZ = idLocationEvent->getXYZ();
        
        for (int32_t windowTabNumber = 0; 
             windowTabNumber < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; 
             windowTabNumber++) {
             this->volumeSlicesSelected[windowTabNumber].selectSlicesAtCoordinate(highlighXYZ);
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
ModelWholeBrain::getOverlaySet(const int tabIndex) const
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
ModelWholeBrain::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i]->initializeOverlays();
    }
}



