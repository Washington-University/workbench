
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

#define __SURFACE_SELECTION_MODEL_DECLARE__
#include "SurfaceSelectionModel.h"
#undef __SURFACE_SELECTION_MODEL_DECLARE__

#include "BrainStructure.h"
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
SurfaceSelectionModel::SurfaceSelectionModel()
: CaretObject()
{
    this->mode = MODE_STRUCTURE;
}

/**
 * Constructor for surface from a specific structure.
 * @param structure
 *   Limit to surfaces from this structure.
 */
SurfaceSelectionModel::SurfaceSelectionModel(const StructureEnum::Enum structure)
: CaretObject()
{
    this->allowableStructures.push_back(structure);
    this->mode = MODE_STRUCTURE;
}

/**
 * Constructor for surface from a brain structure.
 * WARNING: If the brain structure becomes invalid, any further
 * use of this instance may cause a crash.
 *
 * @param brainStructure
 *   Only surfaces in brain structure are available.
 */
SurfaceSelectionModel::SurfaceSelectionModel(BrainStructure* brainStructure)
{
    this->brainStructure = brainStructure;
    this->mode = MODE_BRAIN_STRUCTURE;
}

/**
 * Constructor for surface of a specific type.
 * @param surfaceType
 *   Limit to surfaces from this of these types.
 */
SurfaceSelectionModel::SurfaceSelectionModel(const SurfaceTypeEnum::Enum surfaceType)
{
    this->allowableSurfaceTypes.push_back(surfaceType);
    this->mode = MODE_SURFACE_TYPE;
}

/**
 * Destructor.
 */
SurfaceSelectionModel::~SurfaceSelectionModel()
{
    
}

/**
 * @return The selected surface (NULL if none)
 */
Surface* 
SurfaceSelectionModel::getSurface()
{
    this->updateSelection();
    return this->selectedSurface;
}

/**
 * @return The selected surface (NULL if none)
 */
const Surface* 
SurfaceSelectionModel::getSurface() const
{
    this->updateSelection();
    return this->selectedSurface;
}

/**
 * Set the selected surface.
 * @param surface
 *   New seleted surface.
 */
void 
SurfaceSelectionModel::setSurface(Surface* surface)
{
    this->selectedSurface = surface;
}

/**
 * @return A vector containing surfaces available
 * for selection.
 */
std::vector<Surface*> 
SurfaceSelectionModel::getAvailableSurfaces() const
{
    std::vector<Surface*> surfaces;
    
    switch (mode) {
        case MODE_BRAIN_STRUCTURE:
        {
            const int32_t numSurfaces = this->brainStructure->getNumberOfSurfaces();
            for (int32_t i = 0; i < numSurfaces; i++) {
                surfaces.push_back(this->brainStructure->getSurface(i));
            }
        }
            break;
        case MODE_STRUCTURE:
        {
            EventSurfacesGet getSurfacesEvent;
            for (int32_t i = 0; i < static_cast<int32_t>(this->allowableStructures.size()); i++) {
                getSurfacesEvent.addStructureConstraint(this->allowableStructures[i]);
            }
            EventManager::get()->sendEvent(getSurfacesEvent.getPointer());
            surfaces = getSurfacesEvent.getSurfaces();
        }
            break;
        case MODE_SURFACE_TYPE:
        {
            EventSurfacesGet getSurfacesEvent;
            EventManager::get()->sendEvent(getSurfacesEvent.getPointer());
            std::vector<Surface*> allSurfaces = getSurfacesEvent.getSurfaces();
            for (std::vector<Surface*>::iterator iter = allSurfaces.begin();
                 iter != allSurfaces.end();
                 iter++) {
                Surface* s = *iter;
                const SurfaceTypeEnum::Enum surfaceType = s->getSurfaceType();
                if (std::find(this->allowableSurfaceTypes.begin(),
                              this->allowableSurfaceTypes.end(),
                              surfaceType) != this->allowableSurfaceTypes.end()) {
                    surfaces.push_back(s);
                }
            }
        }
            break;
            
    }
    

    return surfaces;
}

/**
 * Update the selected surface.
 */
void 
SurfaceSelectionModel::updateSelection() const
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
            switch (mode) {
                case MODE_BRAIN_STRUCTURE:
                    this->selectedSurface = this->brainStructure->getVolumeInteractionSurface();
                    break;
                case MODE_STRUCTURE:
                    break;
                case MODE_SURFACE_TYPE:
                    break;
            }
            
            if (this->selectedSurface == NULL) {
                this->selectedSurface = surfaces[0];
            }
        }
    }
}
