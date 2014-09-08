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

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "EventIdentificationHighlightLocation.h"

using namespace caret;

/**
 * Constructor for identification event of location.
 *
 * @parma tabIndex
 *    Index of tab in which identification took place.  This value may
 *    be negative indicating that the identification request is not
 *    for a specific browser tab.  One source for this is the Select Brainordinate
 *    option on the Information Window.
 * @param xyz
 *    Stereotaxic location of selected item.
 */
EventIdentificationHighlightLocation::EventIdentificationHighlightLocation(const int32_t tabIndex,
                                                                           const float xyz[3],
                                                                           const LOAD_FIBER_ORIENTATION_SAMPLES_MODE loadFiberOrientationSamplesMode)
: Event(EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION),
m_tabIndex(tabIndex),
m_loadFiberOrientationSamplesMode(loadFiberOrientationSamplesMode)
{
    /* 
     * NOTE: a negative value is allowed.
     */
    CaretAssert(tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    m_xyz[0] = xyz[0];
    m_xyz[1] = xyz[1];
    m_xyz[2] = xyz[2];
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
    return m_xyz;
}

/**
 * Is the tab with the given index selected for identification operations?
 *
 * @param tabIndex
 *     Index of tab.
 * @return True if tab is selected, else false.
 */
bool
EventIdentificationHighlightLocation::isTabSelected(const int32_t tabIndex) const
{
    /*
     * All tabs?
     */
    if (m_tabIndex < 0) {
        return true;
    }
    else if (m_tabIndex == tabIndex) {
        return true;
    }
    
    return false;
}

/**
 * @return The mode for loading of fiber orientation samples.
 */
EventIdentificationHighlightLocation::LOAD_FIBER_ORIENTATION_SAMPLES_MODE
EventIdentificationHighlightLocation::getLoadFiberOrientationSamplesMode() const
{
    return m_loadFiberOrientationSamplesMode;
}


