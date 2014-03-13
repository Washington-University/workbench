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

#include "EventIdentificationHighlightLocation.h"

using namespace caret;

/**
 * Constructor for identification event of location.
 * @param xyz
 *    Stereotaxic location of selected item.
 */
EventIdentificationHighlightLocation::EventIdentificationHighlightLocation(const float xyz[3])
: Event(EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION)
{
    this->xyz[0] = xyz[0];
    this->xyz[1] = xyz[1];
    this->xyz[2] = xyz[2];
}

/**
 *  Destructor.
 */
EventIdentificationHighlightLocation::~EventIdentificationHighlightLocation()
{
    
}

/**
 * @return The stereotaxic location of the identification (valid for all).
 */
const float* 
EventIdentificationHighlightLocation::getXYZ() const
{
    return this->xyz;
}
