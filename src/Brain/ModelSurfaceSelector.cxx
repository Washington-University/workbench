
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__
#include "ModelSurfaceSelector.h"
#undef __MODEL_DISPLAY_CONTROLLER_SURFACE_SELECTOR_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "ModelSurface.h"
#include "Surface.h"
#include <algorithm>

using namespace caret;

/**
 * \class ModelSurfaceSelector
 * \brief Maintains selection of surface controller
 *
 * Maintains selection of a surface controller with the ability to limit the
 * surface controllers to those from a specific brain structure.
 */

/**
 * Constructor.
 */
ModelSurfaceSelector::ModelSurfaceSelector()
: CaretObject()
{
    this->defaultStructure = StructureEnum::ALL;
    this->selectedStructure = StructureEnum::ALL;
    this->selectedSurfaceController = NULL;
}

/**
 * Destructor.
 */
ModelSurfaceSelector::~ModelSurfaceSelector()
{
    
}

/**
 * @return The selected surface controller.
 */
ModelSurface* 
ModelSurfaceSelector::getSelectedSurfaceController()
{
    this->updateSelector();
    return this->selectedSurfaceController;
}

/**
 * @return The selected structure.
 */
StructureEnum::Enum 
ModelSurfaceSelector::getSelectedStructure()
{
    this->updateSelector();
    return this->selectedStructure;
}

/**
 * Set the selected surface controller.
 * 
 * @param surfaceController
 *    Controller that is selected.
 */
void 
ModelSurfaceSelector::setSelectedSurfaceController(
                                                               ModelSurface* surfaceController)
{
    this->selectedSurfaceController = surfaceController;
    if (this->selectedStructure != StructureEnum::ALL) {
        this->selectedStructure = surfaceController->getSurface()->getStructure();
    }
    this->updateSelector();
}

/**
 * Set the selected structure.
 * 
 * @param selectedStructure
 *    Structure that is selected.
 */
void 
ModelSurfaceSelector::setSelectedStructure(const StructureEnum::Enum selectedStructure)
{
    this->selectedStructure = selectedStructure;
    this->updateSelector();
}

/**
 * Get the structures available for selection.
 * 
 * @param selectableStructuresOut
 *    Output containing structures that can be selected.
 */ 
void 
ModelSurfaceSelector::getSelectableStructures(
                             std::vector<StructureEnum::Enum>& selectableStructuresOut) const
{
    selectableStructuresOut.clear();
    selectableStructuresOut.insert(selectableStructuresOut.end(),
                                  this->availableStructures.begin(),
                                  this->availableStructures.end());
}

/**
 * Get the surface controllers available for selection.
 * 
 * @param selectableSurfaceControllersOut
 *    Output containing surface controllers that can be selected.
 */ 
void 
ModelSurfaceSelector::getSelectableSurfaceControllers(
                        std::vector<ModelSurface*>& selectableSurfaceControllersOut) const
{
    selectableSurfaceControllersOut.clear();
    selectableSurfaceControllersOut.insert(selectableSurfaceControllersOut.end(),
                                           this->availableSurfaceControllers.begin(),
                                           this->availableSurfaceControllers.end());
}

/**
 * Update the selector with the available surface controllers.
 */
void 
ModelSurfaceSelector::updateSelector(const std::vector<Model*> modelDisplayControllers)
{
    this->allSurfaceControllers.clear();
    for (std::vector<Model*>::const_iterator iter = modelDisplayControllers.begin();
         iter != modelDisplayControllers.end();
         iter++) {
        ModelSurface* surfaceController =
        dynamic_cast<ModelSurface*>(*iter);
        if (surfaceController != NULL) {
            this->allSurfaceControllers.push_back(surfaceController);
        }
    }
    
    this->updateSelector();
}

/**
 * Update the selector with the available surface controllers.
 */
void 
ModelSurfaceSelector::updateSelector()
{
    bool haveCortexLeft = false;
    bool haveCortexRight = false;
    bool haveCerebellum = false;
    
    /*
     * Find the ALL surface controllers and structures
     */
    for (std::vector<ModelSurface*>::const_iterator iter = allSurfaceControllers.begin();
         iter != allSurfaceControllers.end();
         iter++) {
        ModelSurface* surfaceController = *iter;
        const Surface* surface = surfaceController->getSurface();
        const StructureEnum::Enum structure = surface->getStructure();
        
        switch (structure) {
            case StructureEnum::CEREBELLUM:
                haveCerebellum = true;
                break;
            case StructureEnum::CORTEX_LEFT:
                haveCortexLeft = true;
                break;
            case StructureEnum::CORTEX_RIGHT:
                haveCortexRight = true;
                break;
            default:
                break;
        }
    }
    
    /*
     * Determine which structures are available.
     */
    this->availableStructures.clear();
    this->availableStructures.push_back(StructureEnum::ALL);
    if (haveCerebellum) {
        this->availableStructures.push_back(StructureEnum::CEREBELLUM);    
    }
    if (haveCortexLeft) {
        this->availableStructures.push_back(StructureEnum::CORTEX_LEFT);    
    }
    if (haveCortexRight) {
        this->availableStructures.push_back(StructureEnum::CORTEX_RIGHT);    
    }
    
    /*
     * Update the structure selection.
     */
    if (std::find(this->availableStructures.begin(),
                  this->availableStructures.end(),
                  this->selectedStructure) == this->availableStructures.end()) {
        if (this->availableStructures.empty() == false) {
            this->selectedStructure = this->availableStructures[0];
        }
        else {
            this->selectedStructure = defaultStructure;
        }
    }
    
    /*
     * Update the available surface controllers.
     */
    this->availableSurfaceControllers.clear();
    for (std::vector<ModelSurface*>::iterator iter = allSurfaceControllers.begin();
         iter != allSurfaceControllers.end();
         iter++) {
        ModelSurface* surfaceController = *iter;
        const Surface* surface = surfaceController->getSurface();
        const StructureEnum::Enum structure = surface->getStructure();
        
        bool useIt = false;
        if (this->selectedStructure == StructureEnum::ALL) {
            useIt = true;
        }
        else if (this->selectedStructure == structure) {
            useIt = true;
        }
            
        if (useIt) {
            this->availableSurfaceControllers.push_back(surfaceController);
        }
    }
    
    /*
     * Update the surface selection.
     */
    if (std::find(this->availableSurfaceControllers.begin(),
                  this->availableSurfaceControllers.end(),
                  this->selectedSurfaceController) == this->availableSurfaceControllers.end()) {
        /*
         * Selected controller is not found.
         */
        this->selectedSurfaceController = NULL;

        /*
         * First, see if a previous controller for structure still exists, if so, use it.
         */
        std::map<StructureEnum::Enum, ModelSurface*>::iterator iter =
        this->previousSelectedSurfaceController.find(this->selectedStructure);
        if (iter != this->previousSelectedSurfaceController.end()) {
            ModelSurface* previousController = iter->second;
            if (std::find(this->availableSurfaceControllers.begin(),
                          this->availableSurfaceControllers.end(),
                          previousController) != this->availableSurfaceControllers.end()) {
                this->selectedSurfaceController = previousController;
            }
        }

        /*
         * Still not found?
         */
        if (this->selectedSurfaceController == NULL) {
            /*
             * Default to first
             */
            if (this->availableSurfaceControllers.empty() == false) {
                this->selectedSurfaceController = this->availableSurfaceControllers[0];
                
                /*
                 * Try to find and used the volume interaction surface.
                 */
                Brain* brain = this->selectedSurfaceController->getBrain();
                if (brain != NULL) {
                    BrainStructure* brainStructure = brain->getBrainStructure(this->selectedStructure, false);
                    if (brainStructure != NULL) {
                        const Surface* volInteractSurface = brainStructure->getVolumeInteractionSurface();
                        if (volInteractSurface != NULL) {
                            const int numSurfaceControllers = static_cast<int32_t>(this->availableSurfaceControllers.size());
                            for (int32_t i = 0; i < numSurfaceControllers; i++) {
                                if (this->availableSurfaceControllers[i]->getSurface() == volInteractSurface) {
                                    this->selectedSurfaceController = this->availableSurfaceControllers[i];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /*
     * Save controller for retrieval later.
     */
    if (this->selectedSurfaceController != NULL) {
        this->previousSelectedSurfaceController[this->selectedStructure] = this->selectedSurfaceController;
    }
}
/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ModelSurfaceSelector::toString() const
{
    AString msg = "selectedStructure="
    + StructureEnum::toName(this->selectedStructure)
    + "selectedSurface=";
    if (this->selectedSurfaceController != NULL) {
        msg += this->selectedSurfaceController->getNameForGUI(false);
    }
    return msg;
}

