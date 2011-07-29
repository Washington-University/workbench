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

#include <cassert>

#include "BrainStructure.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 */
BrainStructure::BrainStructure(Brain* brain)
{
    this->brain = brain;
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    for (uint64_t i = 0; i < this->surfaces.size(); i++) {
        delete this->surfaces[i];
    }
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
BrainStructure::addSurface(Surface* s)
{
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (s->getNumberOfCoordinates() != numNodes) {
            return false;
        }
    }
    
    s->setBrainStructure(this);
    this->surfaces.push_back(s);

    numNodes = this->getNumberOfNodes();
    const uint64_t numColorComponents = numNodes * 4;
    
    if (numColorComponents != this->nodeColoring.size()) {
        this->nodeColoring.resize(numColorComponents);
        
        for (int64_t i = 0; i < numNodes; i++) {
            const int64_t i4 = i * 4;
            this->nodeColoring[i4]   = 0.75f;
            this->nodeColoring[i4+1] = 0.75f;
            this->nodeColoring[i4+2] = 0.75f;
            this->nodeColoring[i4+3] = 1.0f;
        }
    }
    return true;
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
    assert((indx >= 0) && (indx < this->getNumberOfSurfaces()));
    
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

/**
 * Get the coloring for a node.
 *
 * @param nodeIndex
 *    Index of node for color components.
 * @return
 *    A pointer to 4 elements that are the 
 *    red, green, blue, and alpha components
 *    each of which ranges zero to one.
 */
const float* 
BrainStructure::getNodeColor(int32_t nodeIndex) const
{
    assert((nodeIndex >= 0) && (nodeIndex < this->getNumberOfNodes()));

    return &this->nodeColoring[nodeIndex * 4];
}

