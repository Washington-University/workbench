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
 * @param stereotaxicXYZ
 *    Stereotaxic location of selected item.
 * @param voxelCenterXYZ
 *    Stereotaxic XYZ but moved to the center of a voxel on which identification was performed.  I
 *    If identification was NOT on a volume this is the same as stereotaxciXYZ
 */
EventIdentificationHighlightLocation::EventIdentificationHighlightLocation(const int32_t tabIndex,
                                                                           const Vector3D& stereotaxicXYZ,
                                                                           const Vector3D& voxelCenterXYZ,
                                                                           const LOAD_FIBER_ORIENTATION_SAMPLES_MODE loadFiberOrientationSamplesMode)
: Event(EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION),
m_tabIndex(tabIndex),
m_loadFiberOrientationSamplesMode(loadFiberOrientationSamplesMode)
{
    /* 
     * NOTE: a negative value is allowed.
     */
    CaretAssert(tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    
    m_stereotaxicXYZ = stereotaxicXYZ;
    m_voxelCenterXYZ = voxelCenterXYZ;
}

/**
 *  Destructor.
 */
EventIdentificationHighlightLocation::~EventIdentificationHighlightLocation()
{
    
}

/**
 * @return The stereotaxic XYZ of the identification (valid for all).
 */
const Vector3D
EventIdentificationHighlightLocation::getStereotaxicXYZ() const
{
    return m_stereotaxicXYZ;
}

/**
 * @return The  XYZ at the center of the voxel containing the stereotaxic XYZ.
 * If identification was NOT on a volume this is the same as getStereotaxicXYZ
 */
const Vector3D
EventIdentificationHighlightLocation::getVoxelCenterXYZ() const
{
    return m_voxelCenterXYZ;
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


