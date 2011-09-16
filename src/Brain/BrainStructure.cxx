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

#include "CaretAssert.h"

#include "BrainStructure.h"
#include "EventManager.h"
#include "EventModelDisplayControllerAdd.h"
#include "EventModelDisplayControllerDelete.h"
#include "ModelDisplayControllerSurface.h"
#include "Surface.h"

#include <algorithm>

using namespace caret;

/**
 * Constructor.
 * 
 */
BrainStructure::BrainStructure(Brain* brain,
                               StructureEnum::Enum structure)
{
    this->brain = brain;
    this->structure = structure;
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    /*
     * Make a copy of all surface pointers since
     * deleting surfaces will alter the actual
     * vector that stores the surfaces.
     */
    std::vector<Surface*> allSurfaces(this->surfaces);
    
    for (uint64_t i = 0; i < allSurfaces.size(); i++) {
        this->deleteSurface(allSurfaces[i]);
    }
    
    this->surfaces.clear();
}

/**
 * Get the structure for this BrainStructure.
 *
 * @return The structure.
 */
StructureEnum::Enum 
BrainStructure::getStructure() const
{
    return this->structure;
}

/**
 * Add a surface.
 *
 * @param s
 *    Surface that is added.
 * @return
 *    true if the surface was added.  If the 
 *    surface's number of coordinates do not 
 *    match the number of nodes in this brain
 *    structure, false is returned.
 */
bool 
BrainStructure::addSurface(Surface* surface)
{
    CaretAssert(surface);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (surface->getNumberOfCoordinates() != numNodes) {
            return false;
        }
    }
    
    this->surfaces.push_back(surface);

    /*
     * Create a model controller for the surface.
     */
    ModelDisplayControllerSurface* mdcs = new ModelDisplayControllerSurface(surface);
    this->surfaceControllerMap.insert(std::make_pair(surface, mdcs));
    
    /*
     * Send the controller added event.
     */
    EventModelDisplayControllerAdd addEvent(mdcs);
    EventManager::get()->sendEvent(addEvent.getPointer());
    
    return true;
}

void 
BrainStructure::deleteSurface(Surface* surface)
{
    CaretAssert(surface);
    
    std::vector<Surface*>::iterator iter =
    std::find(this->surfaces.begin(),
              this->surfaces.end(),
              surface);
    
    CaretAssertMessage((iter != this->surfaces.end()),
                       "Trying to delete surface not in brain structure.");
    
    std::map<Surface*, ModelDisplayControllerSurface*>::iterator controllerIter = 
        this->surfaceControllerMap.find(surface);

    CaretAssertMessage((controllerIter != this->surfaceControllerMap.end()),
                       "Surface does not map to a model controller");

    ModelDisplayControllerSurface* mdcs = controllerIter->second;
    
    /*
     * Remove from surface to controller map.
     */
    this->surfaceControllerMap.erase(controllerIter);
    
    /*
     * Send the controller deleted event.
     */
    EventModelDisplayControllerDelete deleteEvent(mdcs);
    EventManager::get()->sendEvent(deleteEvent.getPointer());
    
    /*
     * Delete the controller and the surface.
     */
    delete mdcs;
    delete surface;
}

/**
 * Get the number of surfaces.
 *
 * @return
 *    Number of surfaces.
 */
int 
BrainStructure::getNumberOfSurfaces() const
{
    return static_cast<int>(this->surfaces.size());
}

/**
 * Get a surface at the specified index.
 * 
 * @param indx
 *    Index of surface.
 * @return 
 *    Surface at the specified index.
 */
Surface* 
BrainStructure::getSurface(int indx)
{
    CaretAssertVectorIndex(this->surfaces, indx);
    
    return this->surfaces[indx];
}

/**
 * Get the brain that this brain structure is in.
 */
Brain* 
BrainStructure::getBrain()
{
    return this->brain;    
}

/**
 * Get the number of nodes used by this brain structure.
 *
 * @return Number of nodes.
 */
int32_t 
BrainStructure::getNumberOfNodes() const
{
    if (this->surfaces.empty() == false) {
        return surfaces[0]->getNumberOfCoordinates();
    }
    return 0;
}

