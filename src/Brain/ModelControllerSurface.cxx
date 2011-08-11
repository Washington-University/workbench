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

#include "BoundingBox.h"
#include "ModelControllerSurface.h"

#include "Brain.h"
#include "BrainOpenGL.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this surface controller belongs.
 * @param surface - surface for this controller.
 *
 */
ModelControllerSurface::ModelControllerSurface(Brain* brain,
                             Surface* surface)
    : ModelController(brain, true, true)
{
    this->initializeMembersModelControllerSurface();
    this->surface = surface;
}

/**
 * Destructor
 */
ModelControllerSurface::~ModelControllerSurface()
{
}

void
ModelControllerSurface::initializeMembersModelControllerSurface()
{
    this->surface = NULL;
}

/**
 * Get the surface in this controller.
 * @return  Surface in this controller.
 *
 */
Surface*
ModelControllerSurface::getSurface()
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
ModelControllerSurface::getNameForGUI(const bool includeStructureFlag) const
{
    return "ModelControllerSurface::getNameForGUI_NEEDS_IMPLEMENTATION";
}

/**
 * Set the scaling so that the model fills the window.
 *
 */
void
ModelControllerSurface::setDefaultScalingToFitWindow()
{
    BoundingBox bounds;
    this->surface->getBounds(bounds);
    
    float bigY = std::max(std::abs(bounds.getMinY()), bounds.getMaxY());
    float percentScreenY = BrainOpenGL::getModelViewingHalfWindowHeight() * 0.90f;
    float scale = percentScreenY / bigY;
    this->defaultModelScaling = scale;
    
    for (int32_t i = 0; i < CaretWindowEnum::NUMBER_OF_WINDOWS; i++) {
        this->setScaling(i, this->defaultModelScaling);
    }
}

