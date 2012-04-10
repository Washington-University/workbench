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
#include "EventModelSurfaceGet.h"
#include "ModelSurfaceMontage.h"

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
ModelSurfaceMontage::ModelSurfaceMontage(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES,
                         brain)
{
    this->initializeMembersModelSurfaceMontage();
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i] = new OverlaySet(this);
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->leftView(i);
    }
}

/**
 * Destructor
 */
ModelSurfaceMontage::~ModelSurfaceMontage()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete this->overlaySet[i];
    }
    
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
ModelSurfaceMontage::receiveEvent(Event* event)
{
}

void
ModelSurfaceMontage::initializeMembersModelSurfaceMontage()
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
ModelSurfaceMontage::isDualConfigurationEnabled() const
{
    return this->dualConfigurationEnabled;
}

/**
 * Set dual configuration enabled
 * @param enabled
 *    New dual configuration status
 */
void 
ModelSurfaceMontage::setDualConfigurationEnabled(const bool enabled)
{
    this->dualConfigurationEnabled = enabled;
}

/**
 * @return the left surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getLeftSurfaceSelectionModel()
{
    return this->leftSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getLeftSecondSurfaceSelectionModel()
{
    return this->leftSecondSurfaceSelectionModel;
}

/**
 * @return the right surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getRightSurfaceSelectionModel()
{
    return this->rightSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this controller.
 */
SurfaceSelectionModel*
ModelSurfaceMontage::getRightSecondSurfaceSelectionModel()
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
ModelSurfaceMontage::getNameForGUI(const bool /*includeStructureFlag*/) const
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
ModelSurfaceMontage::getNameForBrowserTab() const
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
ModelSurfaceMontage::setDefaultScalingToFitWindow()
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
ModelSurfaceMontage::resetView(const int32_t windowTabNumber)
{
    Model::resetView(windowTabNumber);
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
ModelSurfaceMontage::getOverlaySet(const int tabIndex)
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
ModelSurfaceMontage::getOverlaySet(const int tabIndex) const
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
ModelSurfaceMontage::initializeOverlays()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->overlaySet[i]->initializeOverlays();
    }
}
