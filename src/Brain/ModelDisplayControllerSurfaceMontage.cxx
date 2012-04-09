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
#include <cmath>

#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventModelDisplayControllerSurfaceGet.h"
#include "ModelDisplayControllerSurfaceMontage.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "OverlaySet.h"
#include "SurfaceSelectionModel.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this controller.
 *
 */
ModelDisplayControllerSurfaceMontage::ModelDisplayControllerSurfaceMontage(Brain* brain)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES,
                         brain)
{
    this->initializeMembersModelDisplayControllerSurfaceMontage();
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->leftView(i);
    }
}

/**
 * Destructor
 */
ModelDisplayControllerSurfaceMontage::~ModelDisplayControllerSurfaceMontage()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete this->leftSurfaceSelectionModel;
    delete this->leftSecondSurfaceSelectionModel;
    delete this->rightSurfaceSelectionModel;
    delete this->rightSecondSurfaceSelectionModel;
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelDisplayControllerSurfaceMontage::receiveEvent(Event* event)
{
}

void
ModelDisplayControllerSurfaceMontage::initializeMembersModelDisplayControllerSurfaceMontage()
{
    this->leftSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT);
    this->leftSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT);
    this->rightSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT);
    this->rightSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT);
    this->dualConfigurationEnabled = false;
}

/**
 * @return Is dual configuration enabled?
 */
bool 
ModelDisplayControllerSurfaceMontage::isDualConfigurationEnabled() const
{
    return this->dualConfigurationEnabled;
}

/**
 * Set dual configuration enabled
 * @param enabled
 *    New dual configuration status
 */
void 
ModelDisplayControllerSurfaceMontage::setDualConfigurationEnabled(const bool enabled)
{
    this->dualConfigurationEnabled = enabled;
}

/**
 * @return the left surface selection in this controller.
 */
SurfaceSelectionModel*
ModelDisplayControllerSurfaceMontage::getLeftSurfaceSelectionModel()
{
    return this->leftSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this controller.
 */
SurfaceSelectionModel*
ModelDisplayControllerSurfaceMontage::getLeftSecondSurfaceSelectionModel()
{
    return this->leftSecondSurfaceSelectionModel;
}

/**
 * @return the right surface selection in this controller.
 */
SurfaceSelectionModel*
ModelDisplayControllerSurfaceMontage::getRightSurfaceSelectionModel()
{
    return this->rightSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this controller.
 */
SurfaceSelectionModel*
ModelDisplayControllerSurfaceMontage::getRightSecondSurfaceSelectionModel()
{
    return this->rightSecondSurfaceSelectionModel;
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
ModelDisplayControllerSurfaceMontage::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "Surface Montage";
    
    if (this->dualConfigurationEnabled) {
        name = "Dual Surface Montage";
    }

    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerSurfaceMontage::getNameForBrowserTab() const
{
    AString name = "Surface Montage";
    
    if (this->dualConfigurationEnabled) {
        name = "Dual Surface Montage";
    }
    
    return name;
}

/**
 * Set the scaling so that the model fills the window.
 *
 */
void
ModelDisplayControllerSurfaceMontage::setDefaultScalingToFitWindow()
{
    /*
    BoundingBox bounds;
    this->surface->getBounds(bounds);
    
    float bigY = std::max(std::abs(bounds.getMinY()), bounds.getMaxY());
    float percentScreenY = BrainOpenGL::getModelViewingHalfWindowHeight() * 0.90f;
    float scale = percentScreenY / bigY;
    this->defaultModelScaling = scale;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->setScaling(i, this->defaultModelScaling);
    }
    */
}

/**
 * Reset view.  For left and right hemispheres, the default
 * view is a lateral view.
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
ModelDisplayControllerSurfaceMontage::resetView(const int32_t windowTabNumber)
{
    ModelDisplayController::resetView(windowTabNumber);
    this->leftView(windowTabNumber);    
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelDisplayControllerSurfaceMontage::getOverlaySet(const int tabIndex)
{
/*
    if (this->surface != NULL) {
        BrainStructure* brainStructure = this->surface->getBrainStructure();
        if (brainStructure != NULL) {
            return brainStructure->getOverlaySet(tabIndex);
        }
    }
 */   
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
ModelDisplayControllerSurfaceMontage::getOverlaySet(const int tabIndex) const
{
    /*
    if (this->surface != NULL) {
        const BrainStructure* brainStructure = this->surface->getBrainStructure();
        if (brainStructure != NULL) {
            return brainStructure->getOverlaySet(tabIndex);
        }
    }
    */
    return NULL;
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelDisplayControllerSurfaceMontage::initializeOverlays()
{
}




