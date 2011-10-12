
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

#define __OVERLAY_DECLARE__
#include "Overlay.h"
#undef __OVERLAY_DECLARE__

#include "CaretAssert.h"
#include "SurfaceOverlay.h"

using namespace caret;


/**
 * \class Overlay
 * \brief  Base class for overlays.
 */

/**
 * Constructor.
 */
Overlay::Overlay()
: CaretObject()
{
    this->opacity = 1.0;
    
    this->name = "Overlay ";
    this->enabled = true;
}

/**
 * Destructor.
 */
Overlay::~Overlay()
{
    
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
Overlay::setOverlayNumber(const int32_t overlayIndex)
{    
    this->name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
Overlay::getOpacity() const
{
    return this->opacity;
}

/**
 * Set the opacity.
 *
 * @param opacity
 *    New value for opacity.
 */
void 
Overlay::setOpacity(const float opacity)
{
    this->opacity = opacity;
}

AString 
Overlay::getName() const
{
    return this->name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
Overlay::toString() const
{
    return "Overlay";
}

/**
 * @return Enabled status for this surface overlay.
 */
bool 
Overlay::isEnabled() const
{
    return this->enabled;
}

/**
 * Set the enabled status for this surface overlay.
 * @param enabled
 *    New status.
 */
void 
Overlay::setEnabled(const bool enabled)
{
    this->enabled = enabled;
}

/**
 * Copy the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
Overlay::copyData(const Overlay* overlay)
{
    this->opacity = overlay->opacity;
    this->enabled = overlay->enabled;
    
    this->copyOverlayData(overlay);
}

/**
 * Swap the data from the given overlay to this overlay.
 * @param overlay
 *    Overlay from which data is transferred.
 */
void 
Overlay::swapData(Overlay* overlay)
{
    SurfaceOverlay* surfaceOverlay = dynamic_cast<SurfaceOverlay*>(overlay);
    
    if (surfaceOverlay != NULL) {
        SurfaceOverlay swapOverlay;
        swapOverlay.copyData(surfaceOverlay);
        
        surfaceOverlay->copyData(this);
        this->copyData(&swapOverlay);
    }
    else {
        CaretAssertMessage(0, "Unrecognized overlay type");
    }
/*
    Overlay swapOverlay;
    swapOverlay.copyData(overlay);
    
    overlay->copyData(this);
    this->copyData(&swapOverlay);
*/
}


