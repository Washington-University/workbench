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
    AString msg;
    if (includeStructureFlag) {
        msg += StructureEnum::toGuiName(this->getStructure());
        msg += " ";
    }
    msg += SurfaceTypeEnum::toGuiName(this->getSurfaceType());
    msg += " ";
    msg += this->getFileNameNoPath();
    return msg;
}

/**
 * Set a bounding box using this surface's coordinates.
 *
 * @param boundingBoxOut
 *    Bounding box that is updated.
 */
void 
Surface::getBounds(BoundingBox& boundingBoxOut) const
{
    boundingBoxOut.set(this->getCoordinate(0), 
                       this->getNumberOfNodes());
}

/**
 * Initialize members of this class.
 */
void 
Surface::initializeMemberSurface()
{
    this->brainStructureIdentifier = -1;
}

/**
 * Helps with copying this surface.
 */
void 
Surface::copyHelperSurface(const Surface& /*s*/)
{
    this->initializeMemberSurface();
    this->computeNormals();
}

/**
 * Get the identifier of the brain structure that
 * contains this surface.  If negative, then
 * this surface is not within a BrainStructure.
 * @return The brain structure identifier.
 */
int64_t 
Surface::getBrainStructureIdentifier() const
{
    return this->brainStructureIdentifier;
}

/**
 * Set the identifier of the brain structure that contains
 * this surface.
 *
 * @param brainStructureIdentifier
 *    New value for brain structure identifier.
 */
void 
Surface::setBrainStructureIdentifier(const int64_t brainStructureIdentifier)
{
    this->brainStructureIdentifier = brainStructureIdentifier;
}


