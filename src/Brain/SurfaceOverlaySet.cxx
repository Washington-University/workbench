
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

#define __SURFACE_OVERLAY_SET_DECLARE__
#include "SurfaceOverlaySet.h"
#undef __SURFACE_OVERLAY_SET_DECLARE__

#include "CaretAssert.h"

using namespace caret;


/**
 * Constructor.
 */
SurfaceOverlaySet::SurfaceOverlaySet()
: CaretObject()
{
    this->numberOfDisplayedOverlays = BrainConstants::MINIMUM_NUMBER_OF_SURFACE_OVERLAYS;
}

/**
 * Destructor.
 */
SurfaceOverlaySet::~SurfaceOverlaySet()
{
    
}

SurfaceOverlay* 
SurfaceOverlaySet::getPrimaryOverlay()
{
    return &this->overlays[0];
}

SurfaceOverlay* 
SurfaceOverlaySet::getUnderlay()
{
    return &this->overlays[BrainConstants::MAXIMUM_NUMBER_OF_SURFACE_OVERLAYS - 1];
}

SurfaceOverlay* 
SurfaceOverlaySet::getOverlay(const int32_t overlayNumber)
{
    CaretAssertArrayIndex(this->overlays, 
                          BrainConstants::MAXIMUM_NUMBER_OF_SURFACE_OVERLAYS, 
                          overlayNumber);
    return &this->overlays[overlayNumber];    
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceOverlaySet::toString() const
{
    return "SurfaceOverlaySet";
}

/**
 * Add a displayed overlay.  If the maximum
 * number of surface overlays is reached,
 * this method has no effect.
 */
void 
SurfaceOverlaySet::addDisplayedOverlay()
{
    this->numberOfDisplayedOverlays++;
    if (this->numberOfDisplayedOverlays > BrainConstants::MAXIMUM_NUMBER_OF_SURFACE_OVERLAYS) {
        this->numberOfDisplayedOverlays = BrainConstants::MAXIMUM_NUMBER_OF_SURFACE_OVERLAYS;
    }
}

/**
 * @return Returns the number of displayed overlays. 
 */
int32_t 
SurfaceOverlaySet::getNumberOfDisplayedOverlays() const
{
    return this->numberOfDisplayedOverlays;
}

/**
 * Remove a displayed overlay.  This method will have
 * no effect if the minimum number of overlays are
 * displayed
 *
 * @param overlayIndex 
 *    Index of overlay for removal from display.
 */
void 
SurfaceOverlaySet::removeDisplayedOverlay(const int32_t overlayIndex)
{
    
}

/**
 * Move the overlay at the given index up one level
 * (swap it with overlayIndex - 1).  This method will
 * have no effect if the overlay is the top-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved up.
 */
void 
SurfaceOverlaySet::moveDisplayedOverlayUp(const int32_t overlayIndex)
{
    
}

/**
 * Move the overlay at the given index down one level
 * (swap it with overlayIndex + 1).  This method will
 * have no effect if the overlay is the bottom-most overlay.
 *
 * @param overlayIndex 
 *    Index of overlay that is to be moved down.
 */
void 
SurfaceOverlaySet::moveDisplayedOverlayDown(const int32_t overlayIndex)
{
    
}



