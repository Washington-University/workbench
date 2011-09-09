
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

#define __SURFACE_OVERLAY_DECLARE__
#include "SurfaceOverlay.h"
#undef __SURFACE_OVERLAY_DECLARE__

using namespace caret;


/**
 * Constructor.
 */
SurfaceOverlay::SurfaceOverlay()
: CaretObject()
{
    this->selectedType = SurfaceOverlayDataTypeEnum::NONE;
    this->opacity      = 1.0;
    
    this->name = "Overlay ";
}

/**
 * Destructor.
 */
SurfaceOverlay::~SurfaceOverlay()
{
    
}

/**
 * Set the number of this overlay.
 * 
 * @param overlayIndex
 *    Index for this overlay.
 */
void 
SurfaceOverlay::setOverlayNumber(const int32_t overlayIndex)
{    
    this->name = "Overlay " + AString::number(overlayIndex + 1);
}

/**
 * Get the selected overlay type.
 
 * @return The selected overlay type.
 */
SurfaceOverlayDataTypeEnum::Enum 
SurfaceOverlay::getSelectedType() const
{
    return this->selectedType;
}

/**
 * Set the selected overlay type.
 *
 * @param selectedType
 *    New type of overlay.
 */
void 
SurfaceOverlay::setSelectedType(SurfaceOverlayDataTypeEnum::Enum selectedType)
{
    this->selectedType = selectedType;
}

/**
 * Get the opacity.
 * 
 * @return  The opacity.
 */
float 
SurfaceOverlay::getOpacity() const
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
SurfaceOverlay::setOpacity(const float opacity)
{
    this->opacity = opacity;
}

AString 
SurfaceOverlay::getName() const
{
    return this->name;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceOverlay::toString() const
{
    return "SurfaceOverlay";
}
