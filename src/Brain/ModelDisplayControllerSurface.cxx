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

#include "BrowserTabContent.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "ModelDisplayControllerSurface.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param surface - surface for this controller.
 *
 */
ModelDisplayControllerSurface::ModelDisplayControllerSurface(Surface* surface)
: ModelDisplayController(ModelDisplayControllerTypeEnum::MODEL_TYPE_SURFACE,
                         YOKING_ALLOWED_YES,
                         ROTATION_ALLOWED_YES)
{
    CaretAssert(surface);
    this->initializeMembersModelDisplayControllerSurface();
    this->surface = surface;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->lateralView(i);
    }
}

/**
 * Destructor
 */
ModelDisplayControllerSurface::~ModelDisplayControllerSurface()
{
}

void
ModelDisplayControllerSurface::initializeMembersModelDisplayControllerSurface()
{
    this->surface = NULL;
}

/**
 * Get the surface in this controller.
 * @return  Surface in this controller.
 */
Surface*
ModelDisplayControllerSurface::getSurface()
{
    return this->surface;
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
ModelDisplayControllerSurface::getNameForGUI(const bool includeStructureFlag) const
{
    AString name;
    if (includeStructureFlag) {
        const StructureEnum::Enum structure = this->surface->getStructure();
        name += StructureEnum::toGuiName(structure);
        name += " ";
    }
    name += this->surface->getFileNameNoPath();
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelDisplayControllerSurface::getNameForBrowserTab() const
{
    const StructureEnum::Enum structure = this->surface->getStructure();
    const AString name = StructureEnum::toGuiName(structure);
    return name;
}

/**
 * Set the scaling so that the model fills the window.
 *
 */
void
ModelDisplayControllerSurface::setDefaultScalingToFitWindow()
{
    BoundingBox bounds;
    this->surface->getBounds(bounds);
    
    float bigY = std::max(std::abs(bounds.getMinY()), bounds.getMaxY());
    float percentScreenY = BrainOpenGL::getModelViewingHalfWindowHeight() * 0.90f;
    float scale = percentScreenY / bigY;
    this->defaultModelScaling = scale;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->setScaling(i, this->defaultModelScaling);
    }
}

/**
 * Reset view.  For left and right hemispheres, the default
 * view is a lateral view.
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
ModelDisplayControllerSurface::resetView(const int32_t windowTabNumber)
{
    ModelDisplayController::resetView(windowTabNumber);
    this->lateralView(windowTabNumber);    
}

/**
 * Switch to a lateral view.  This method only affects
 * surfaces that are the left or right cerebral cortex and
 * are not flat.
 *
 * @param  windowTabNumber  Window for which view is requested
 * reset the view.
 */
void
ModelDisplayControllerSurface::lateralView(const int32_t windowTabNumber)
{
    if (this->surface->getSurfaceType() != SurfaceTypeEnum::SURFACE_TYPE_FLAT) {
        switch (this->surface->getStructure()) {
            case StructureEnum::CORTEX_LEFT:
                this->leftView(windowTabNumber);
                break;
            case StructureEnum::CORTEX_RIGHT:
                this->rightView(windowTabNumber);
                break;
            default:
                break;
        }
    }
}



