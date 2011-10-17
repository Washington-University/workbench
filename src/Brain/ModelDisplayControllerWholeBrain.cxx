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
#include "EventManager.h"
#include "EventModelDisplayControllerGetAll.h"
#include "ModelDisplayControllerSurface.h"
#include "ModelDisplayControllerWholeBrain.h"
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
        
        this->sliceIndexAxial[i]        = 0;
        this->sliceIndexCoronal[i]      = 0;
        this->sliceIndexParasagittal[i] = 0;
        
        this->sliceEnabledAxial[i] = true;
        this->sliceEnabledCoronal[i] = true;
        this->sliceEnabledParasagittal[i] = true;
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
     * Update the selected surface type.
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
 * Return the axial slice index for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Axial slice index.
 */
int64_t 
ModelDisplayControllerWholeBrain::getSliceIndexAxial(const int32_t windowTabNumber) const
{    
    return this->sliceIndexAxial[windowTabNumber];
}

/**
 * Set the axial slice index in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param 
 *    New value for axial slice index.
 */
void 
ModelDisplayControllerWholeBrain::setSliceIndexAxial(const int32_t windowTabNumber,
                                                 const int64_t sliceIndexAxial)
{    
    this->sliceIndexAxial[windowTabNumber] = sliceIndexAxial;
}

/**
 * Return the coronal slice index for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Coronal slice index.
 */
int64_t 
ModelDisplayControllerWholeBrain::getSliceIndexCoronal(const int32_t windowTabNumber) const
{    
    return this->sliceIndexCoronal[windowTabNumber];
}


/**
 * Set the coronal slice index in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param 
 *    New value for coronal slice index.
 */
void 
ModelDisplayControllerWholeBrain::setSliceIndexCoronal(const int32_t windowTabNumber,
                                                   const int64_t sliceIndexCoronal)
{    
    this->sliceIndexCoronal[windowTabNumber] = sliceIndexCoronal;
}

/**
 * Return the parasagittal slice index for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Parasagittal slice index.
 */
int64_t 
ModelDisplayControllerWholeBrain::getSliceIndexParagittal(const int32_t windowTabNumber) const
{
    return this->sliceIndexParasagittal[windowTabNumber];
}

/**
 * Set the parasagittal slice index in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param 
 *    New value for parasagittal slice index.
 */
void 
ModelDisplayControllerWholeBrain::setSliceIndexParasagittal(const int32_t windowTabNumber,
                                                        const int64_t sliceIndexParasagittal)
{    
    this->sliceIndexParasagittal[windowTabNumber] = sliceIndexParasagittal;
}

/**
 * Is the parasagittal slice enabled?
 * @param windowTabNumber
 *    Tab number of window.
 * @return
 *    Enabled status of parasagittal slice.
 */
bool 
ModelDisplayControllerWholeBrain::isSliceParasagittalEnabled(const int32_t windowTabNumber) const
{
    return this->sliceEnabledParasagittal[windowTabNumber];
}

/**
 * Set the enabled status of the parasagittal slice.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceEnabledParasagittal 
 *    New enabled status.
 */ 
void 
ModelDisplayControllerWholeBrain::setSliceParasagittalEnabled(const int32_t windowTabNumber,
                                                              const bool sliceEnabledParasagittal)
{
    this->sliceEnabledParasagittal[windowTabNumber] = sliceEnabledParasagittal;
}

/**
 * Is the coronal slice enabled?
 * @param windowTabNumber
 *    Tab number of window.
 * @return
 *    Enabled status of coronal slice.
 */
bool 
ModelDisplayControllerWholeBrain::isSliceCoronalEnabled(const int32_t windowTabNumber) const
{
    return this->sliceEnabledCoronal[windowTabNumber];
}

/**
 * Set the enabled status of the coronal slice.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceEnabledCoronal 
 *    New enabled status.
 */ 
void 
ModelDisplayControllerWholeBrain::setSliceCoronalEnabled(const int32_t windowTabNumber,
                                                              const bool sliceEnabledCoronal)
{
    this->sliceEnabledCoronal[windowTabNumber] = sliceEnabledCoronal;
}

/**
 * Is the axial slice enabled?
 * @param windowTabNumber
 *    Tab number of window.
 * @return
 *    Enabled status of axial slice.
 */
bool 
ModelDisplayControllerWholeBrain::isSliceAxialEnabled(const int32_t windowTabNumber) const
{
    return this->sliceEnabledAxial[windowTabNumber];
}

/**
 * Set the enabled status of the axial slice.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceEnabledParasagittal 
 *    New enabled status.
 */ 
void 
ModelDisplayControllerWholeBrain::setSliceAxialEnabled(const int32_t windowTabNumber,
                                                              const bool sliceEnabledAxial)
{
    this->sliceEnabledAxial[windowTabNumber] = sliceEnabledAxial;
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
ModelDisplayControllerWholeBrain::getNameForGUI(const bool includeStructureFlag) const
{
    return "ModelDisplayControllerWholeBrain::getNameForGUI_NEEDS_IMPLEMENTATION";
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

VolumeFile* 
ModelDisplayControllerWholeBrain::getVolumeFile()
{
    VolumeFile* vf = NULL;
    if (this->brain->getNumberOfVolumeFiles() > 0) {
        vf = this->brain->getVolumeFile(0);
    }
    return vf;
}

