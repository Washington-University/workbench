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
#include "BrowserTabContent.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
#include "EventModelDisplayControllerGetAll.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerWholeBrain.h"
#include "OverlaySet.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this surface controller belongs.
 * @param surface - surface for this controller.
 *
 */
ModelDisplayControllerWholeBrain::ModelDisplayControllerWholeBrain(Brain* brain)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_WHOLE_BRAIN,
                         YOKING_ALLOWED_NO,
                         ROTATION_ALLOWED_YES)
{
    this->brain = brain;
    this->initializeMembersModelDisplayControllerWholeBrain();
}

/**
 * Destructor
 */
ModelDisplayControllerWholeBrain::~ModelDisplayControllerWholeBrain()
{
}

/**
 * Initialize members of this class.
 */
void
ModelDisplayControllerWholeBrain::initializeMembersModelDisplayControllerWholeBrain()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectedSurfaceType[i] = SurfaceTypeEnum::SURFACE_TYPE_ANATOMICAL;
        this->cerebellumEnabled[i] = true;
        this->leftEnabled[i] = true;
        this->rightEnabled[i] = true;
        this->leftRightSeparation[i] = 0.0;
        this->cerebellumSeparation[i] = 0.0;        
        this->volumeSlicesSelected[i].reset();
    }
}

/**
 * Get the brain that created this controller.
 * @return The brain.
 */
Brain*
ModelDisplayControllerWholeBrain::getBrain()
{
    return this->brain;
}

/**
 * Get the available surface types.
 * @param surfaceTypesOut
 *    Output loaded with the available surface types.
 */
void 
ModelDisplayControllerWholeBrain::getAvailableSurfaceTypes(std::vector<SurfaceTypeEnum::Enum>& surfaceTypesOut)
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
ModelDisplayControllerWholeBrain::getSelectedSurfaceType(const int32_t windowTabNumber)
{
    this->updateController();
    return this->selectedSurfaceType[windowTabNumber];    
}

/**
 * Update this controller.
 */
void 
ModelDisplayControllerWholeBrain::updateController()
{
    /*
     * Get all model controllers to find loaded surface types.
     */
    EventModelDisplayControllerGetAll eventGetControllers;
    EventManager::get()->sendEvent(eventGetControllers.getPointer());
    const std::vector<ModelDisplayController*> allControllers =
        eventGetControllers.getModelDisplayControllers();

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
    for (std::vector<ModelDisplayController*>::const_iterator iter = allControllers.begin();
         iter != allControllers.end();
         iter++) {
        ModelDisplayControllerSurface* surfaceController = 
            dynamic_cast<ModelDisplayControllerSurface*>(*iter);
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
                this->selectedSurfaceType[i] = SurfaceTypeEnum::SURFACE_TYPE_ANATOMICAL;
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
ModelDisplayControllerWholeBrain::setSelectedSurfaceType(const int32_t windowTabNumber,
                                                         const SurfaceTypeEnum::Enum surfaceType)
{
    this->selectedSurfaceType[windowTabNumber] = surfaceType;
    this->updateController();
}

/**
 * @return Enabled status for left cerebral cortex.
 */
bool 
ModelDisplayControllerWholeBrain::isLeftEnabled(const int32_t windowTabNumber) const
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
ModelDisplayControllerWholeBrain::setLeftEnabled(const int32_t windowTabNumber,
                                                 const bool enabled)
{
    this->leftEnabled[windowTabNumber] = enabled;
}

/**
 * @return Enabled status for right cerebral cortex.
 */
bool 
ModelDisplayControllerWholeBrain::isRightEnabled(const int32_t windowTabNumber) const
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
ModelDisplayControllerWholeBrain::setRightEnabled(const int32_t windowTabNumber,
                                                  const bool enabled)
{
    this->rightEnabled[windowTabNumber] = enabled;
}

/**
 * @return Enabled status for cerebellum.
 */
bool 
ModelDisplayControllerWholeBrain::isCerebellumEnabled(const int32_t windowTabNumber) const
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
ModelDisplayControllerWholeBrain::setCerebellumEnabled(const int32_t windowTabNumber,
                                                       const bool enabled)
{
    this->cerebellumEnabled[windowTabNumber] = enabled;
}

/**
 * @return The separation between the left and right surfaces.
 */
float 
ModelDisplayControllerWholeBrain::getLeftRightSeparation(const int32_t windowTabNumber) const
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
ModelDisplayControllerWholeBrain::setLeftRightSeparation(const int32_t windowTabNumber,
                            const float separation)
{
    this->leftRightSeparation[windowTabNumber] = separation;
}

/**
 * @return The separation between the left/right surfaces.
 */
float 
ModelDisplayControllerWholeBrain::getCerebellumSeparation(const int32_t windowTabNumber) const
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
ModelDisplayControllerWholeBrain::setCerebellumSeparation(const int32_t windowTabNumber,
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
VolumeSliceIndicesSelection* 
ModelDisplayControllerWholeBrain::getSelectedVolumeSlices(const int32_t windowTabNumber)
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
const VolumeSliceIndicesSelection* 
ModelDisplayControllerWholeBrain::getSelectedVolumeSlices(const int32_t windowTabNumber) const
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
ModelDisplayControllerWholeBrain::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "Whole Brain";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerWholeBrain::getNameForBrowserTab() const
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
ModelDisplayControllerWholeBrain::getUnderlayVolumeFile(const int32_t windowTabNumber) const
{
    VolumeFile* vf = NULL;
    
    EventBrowserTabGet getBrowserTabEvent(windowTabNumber);
    EventManager::get()->sendEvent(getBrowserTabEvent.getPointer());
    BrowserTabContent* btc = getBrowserTabEvent.getBrowserTab();
    if (btc != NULL) {
        OverlaySet* overlaySet = btc->getOverlaySet();
        vf = overlaySet->getUnderlayVolume(btc);
        
    }
    
    return vf;
}


/**
 * Set the selected slices to the origin.
 * @param  windowTabNumber  Window for which slices set to origin is requested.
 */
void
ModelDisplayControllerWholeBrain::setSlicesToOrigin(const int32_t windowTabNumber)
{
    VolumeFile* vf = this->getUnderlayVolumeFile(windowTabNumber);
    if (vf != NULL) {
        this->volumeSlicesSelected[windowTabNumber].selectSlicesAtOrigin(vf);
    }
}

