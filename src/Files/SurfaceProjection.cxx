
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __SURFACE_PROJECTION_DECLARE__
#include "SurfaceProjection.h"
#undef __SURFACE_PROJECTION_DECLARE__

using namespace caret;


    
/**
 * \class caret::SurfaceProjection 
 * \brief Abstract class for Surface Projections.
 */

/**
 * Constructor.
 */
SurfaceProjection::SurfaceProjection()
: CaretObjectTracksModification()
{
    this->projectionSurfaceNumberOfNodes = 0;
}

/**
 * Destructor.
 */
SurfaceProjection::~SurfaceProjection()
{
    
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SurfaceProjection::SurfaceProjection(const SurfaceProjection& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperSurfaceProjection(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SurfaceProjection&
SurfaceProjection::operator=(const SurfaceProjection& obj)
{
    if (this != &obj) {
        CaretObjectTracksModification::operator=(obj);
        this->copyHelperSurfaceProjection(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SurfaceProjection::copyHelperSurfaceProjection(const SurfaceProjection& obj)
{
    this->projectionSurfaceNumberOfNodes = obj.projectionSurfaceNumberOfNodes;
}

/**
 * @return The number of nodes in the surface to which this projection is made.
 */
int32_t 
SurfaceProjection::getProjectionSurfaceNumberOfNodes() const
{
    return this->projectionSurfaceNumberOfNodes;
}

/**
 * Set the number of nodes in the surface to which this projection is made.
 * @param projectionSurfaceNumberOfNodes
 *    Number of nodes in the surface.
 */
void 
SurfaceProjection::setProjectionSurfaceNumberOfNodes(const int projectionSurfaceNumberOfNodes)
{
    this->projectionSurfaceNumberOfNodes = projectionSurfaceNumberOfNodes;
}

/**
 * @return a string describing the projection 
 */
AString
SurfaceProjection::toString() const
{
    return CaretObjectTracksModification::toString();
}



