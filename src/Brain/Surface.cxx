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

#include "BoundingBox.h"
#include "BrainStructure.h"
#include "Surface.h"
#include "ModelControllerSurface.h"

using namespace caret;

/**
 * Constructor.
 */
Surface::Surface()
{
    this->initializeMemberSurface();
}

/**
 * Destructor.
 */
Surface::~Surface()
{
    if (this->surfaceController != NULL) {
        delete this->surfaceController;
    }
}

/**
 * Copy constructor.
 *
 * @param s
 *    Surface that is copied.
 */
Surface::Surface(const Surface& s)
: SurfaceFile(s)
{
    this->copyHelperSurface(s);
}

/**
 * Assignment operator.
 *
 * @param s
 *    Contents of "s" are assigned to this.
 */
Surface& 
Surface::operator=(const Surface& s)
{
    if (this != &s) {
        SurfaceFile::operator=(s);
        this->copyHelperSurface(s);
    }
    
    return *this;
}

/**
 * Set the brain structure for this surface.
 *
 * @param brainStructure
 *    Brain structure for this surface.
 */
void 
Surface::setBrainStructure(BrainStructure* brainStructure)
{
    this->brainStructure = brainStructure;
}

/**
 * Get the model controller for this surface.
 *
 * @return
 *    Model controller for this surface.
 */
ModelController* 
Surface::getModelController()
{
    if (this->surfaceController == NULL) {
        this->surfaceController = new ModelControllerSurface(this->getBrain(), this);
    }
    return this->surfaceController;
}

/**
 *
 */
AString 
Surface::getNameForGUI(bool includeStructureFlag) const
{
    return    "Surface::getNameForGUI_NOT_IMPLEMENTED";
}

/**
 * Get brain containing this surface.
 *
 * @param
 *    Brain containing this surface.
 */
Brain* 
Surface::getBrain()
{
    if (this->brainStructure != NULL) {
        return this->brainStructure->getBrain();
    }
    return NULL;
}

/**
 * Get the brain structure that contains this surface.
 * 
 * @return Pointer to the brain structure.
 */
BrainStructure* 
Surface::getBrainStructure()
{
    return this->brainStructure;
}

/**
 * Get the brain structure that contains this surface.
 * 
 * @return Const Pointer to the brain structure.
 */
const BrainStructure* 
Surface::getBrainStructure() const
{
    return this->brainStructure;
}

/**
 * Set a bounding box using this surface's coordinates.
 *
 * @param
 *    Bounding box that is updated.
 */
void 
Surface::getBounds(BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.set(this->getCoordinate(0), 
                       this->getNumberOfCoordinates());
}

/**
 * Initialize members of this class.
 */
void 
Surface::initializeMemberSurface()
{
    this->surfaceController = NULL;
}

/**
 * Helps with copying this surface.
 */
void 
Surface::copyHelperSurface(const Surface& s)
{
    this->brainStructure = s.brainStructure;
    
    this->initializeMemberSurface();
    this->computeNormals();
}
