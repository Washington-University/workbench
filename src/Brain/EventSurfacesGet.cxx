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

#include "CaretAssert.h"
#include "EventSurfacesGet.h"
#include "Surface.h"

using namespace caret;

/**
 * Construct an event for getting surfaces from
 * all structures and surface types.  Methods
 * are provided for constraining the surfaces
 * to those with specified structures and/or
 * surface types.
 */
EventSurfacesGet::EventSurfacesGet()
: Event(EventTypeEnum::EVENT_SURFACES_GET)
{
}

/**
 *  Destructore.
 */
EventSurfacesGet::~EventSurfacesGet()
{
    
}

/**
 * Add a surface.  If the surface does not
 * meet any surface/structure constraints,
 * it will not be added.
 *
 * @param surface
 *    Surface that is added.
 */
void 
EventSurfacesGet::addSurface(Surface* surface)
{
    if (this->structureConstraints.empty() == false) {
        const StructureEnum::Enum structure = surface->getStructure();
        if (std::find(this->structureConstraints.begin(),
                      this->structureConstraints.end(),
                      structure) == this->structureConstraints.end()) {
            return;
        }
    }
    
    if (this->surfaceTypeConstraints.empty() == false) {        
        const SurfaceTypeEnum::Enum surfaceType = surface->getSurfaceType();
        if (std::find(this->surfaceTypeConstraints.begin(),
                      this->surfaceTypeConstraints.end(),
                      surfaceType) == this->surfaceTypeConstraints.end()) {
            return;
        }
    }
    
    this->surfaces.push_back(surface);
}

/**
 * Add a structure contraints.  If structure constraints
 * are added, only surface of the specified structures
 * are obtained.  May be called more than once for 
 * constraining to multiple surface types.
 *
 * @param structure
 *    Structure for constraining selection.
 */
void 
EventSurfacesGet::addStructureConstraint(const StructureEnum::Enum structure)
{
    this->structureConstraints.push_back(structure);
}

/**
 * Add a surface type contraints.  If surface type constraints
 * are added, only surfaces of the specified surface types
 * are obtained.  May be called more than once for constraining
 * to multiple surface types.
 *
 * @param surfaceType
 *    Surface type for constraining selection.
 */
void 
EventSurfacesGet::addSurfaceTypeConstraint(const SurfaceTypeEnum::Enum surfaceType)
{
    this->surfaceTypeConstraints.push_back(surfaceType);
}

/**
 * @return  Surfaces that were obtained.
 */
std::vector<Surface*> 
EventSurfacesGet::getSurfaces() const
{
    return this->surfaces;
}

