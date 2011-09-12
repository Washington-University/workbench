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
 *
 */
AString 
Surface::getNameForGUI(bool includeStructureFlag) const
{
    return    "Surface::getNameForGUI_NOT_IMPLEMENTED";
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
}

/**
 * Helps with copying this surface.
 */
void 
Surface::copyHelperSurface(const Surface& s)
{
    this->initializeMemberSurface();
    this->computeNormals();
}
