
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

#include <algorithm>

#define __SURFACE_SELECTION_DECLARE__
#include "SurfaceSelection.h"
#undef __SURFACE_SELECTION_DECLARE__

#include "EventManager.h"
#include "EventSurfacesGet.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class SurfaceSelection 
 * \brief Maintains selection of a surface.
 *
 * Maintains selection of a surface.  If the surface 
 * becomes invalid, a different surface will become selected.
 */

/**
 * Constructor for surfaces in any structure.
 */
SurfaceSelection::SurfaceSelection()
: CaretObject()
{
    
}

/**
 * Constructor for surface from a specific structure.
 * @param structure
 *   Limit to surfaces from this structure.
 */
SurfaceSelection::SurfaceSelection(const StructureEnum::Enum structure)
: CaretObject()
{
    this->allowableStructures.push_back(structure);
}

/**
 * Destructor.
 */
SurfaceSelection::~SurfaceSelection()
{
    
}

/**
 * @return The selected surface (NULL if none)
 */
Surface* 
SurfaceSelection::getSurface()
{
    this->updateSelection();
    return this->selectedSurface;
}

/**
 * @return The selected surface (NULL if none)
 */
const Surface* 
SurfaceSelection::getSurface() const
{
    this->updateSelection();
    return this->selectedSurface;
}

/**
 * @return A vector containing surfaces available
 * for selection.
 */
std::vector<Surface*> 
SurfaceSelection::getAvailableSurfaces() const
{
    EventSurfacesGet getSurfacesEvent;
    for (int32_t i = 0; i < static_cast<int32_t>(this->allowableStructures.size()); i++) {
        getSurfacesEvent.addStructureConstraint(this->allowableStructures[i]);
    }
    EventManager::get()->sendEvent(getSurfacesEvent.getPointer());

    return getSurfacesEvent.getSurfaces();
}

/**
 * Update the selected surface.
 */
void 
SurfaceSelection::updateSelection() const
{
    std::vector<Surface*> surfaces = this->getAvailableSurfaces();
    
    if (this->selectedSurface != NULL) {
        if (std::find(surfaces.begin(),
                      surfaces.end(),
                      this->selectedSurface) == surfaces.end()) {
            this->selectedSurface = NULL;
        }
    }
    
    if (this->selectedSurface == NULL) {
        if (surfaces.empty() == false) {
            this->selectedSurface = surfaces[0];
        }
    }
}
