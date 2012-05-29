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

#include "CaretAssert.h"
#include "ModelYokingGroup.h"

using namespace caret;

/**
 * Constructor.
 * @param brain - brain to which this volume controller belongs.
 *
 */
ModelYokingGroup::ModelYokingGroup(Brain* brain,
                         const AString& yokingName)
: Model(ModelTypeEnum::MODEL_TYPE_YOKING,
                         YOKING_ALLOWED_NO,
                         ROTATION_ALLOWED_YES,
                         brain)
{
    this->initializeMembersModelYoking();
    this->yokingName = yokingName;
}

/**
 * Destructor
 */
ModelYokingGroup::~ModelYokingGroup()
{
}

/**
 * Return the name of the yoking.
 */
AString 
ModelYokingGroup::getYokingName() const
{
    return this->yokingName;
}

void
ModelYokingGroup::initializeMembersModelYoking()
{
        this->sliceViewPlane         = VolumeSliceViewPlaneEnum::AXIAL;
        this->sliceViewMode          = VolumeSliceViewModeEnum::ORTHOGONAL;
        this->montageNumberOfColumns = 3;
        this->montageNumberOfRows    = 4;
        this->montageSliceSpacing    = 5;
        this->volumeSlicesSelected.reset();
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelYokingGroup::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    return "ModelYoking";
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model controller.
 */
AString 
ModelYokingGroup::getNameForBrowserTab() const
{
    return "ModelYoking";
}


/**
 * Return the for axis mode in the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return Axis mode.
 *   
 */
VolumeSliceViewPlaneEnum::Enum 
ModelYokingGroup::getSliceViewPlane(const int32_t /*windowTabNumber*/) const
{    
    return this->sliceViewPlane;
}

/**
 * Set the axis mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param slicePlane
 *    New value for slice plane.
 */
void 
ModelYokingGroup::setSliceViewPlane(const int32_t /*windowTabNumber*/,
                      VolumeSliceViewPlaneEnum::Enum slicePlane)
{   
    this->sliceViewPlane = slicePlane;
}

/**
 * Return the view mode for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   View mode.
 */
VolumeSliceViewModeEnum::Enum 
ModelYokingGroup::getSliceViewMode(const int32_t /*windowTabNumber*/) const
{    
    return this->sliceViewMode;
}

/**
 * Set the view mode in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param sliceViewMode
 *    New value for view mode
 */
void 
ModelYokingGroup::setSliceViewMode(const int32_t /*windowTabNumber*/,
                      VolumeSliceViewModeEnum::Enum sliceViewMode)
{    
    this->sliceViewMode = sliceViewMode;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
VolumeSliceCoordinateSelection* 
ModelYokingGroup::getSelectedVolumeSlices(const int32_t /*windowTabNumber*/)
{
    return &this->volumeSlicesSelected;
}

/**
 * Return the volume slice selection.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Volume slice selection for tab.
 */
const VolumeSliceCoordinateSelection* 
ModelYokingGroup::getSelectedVolumeSlices(const int32_t /*windowTabNumber*/) const
{
    return &this->volumeSlicesSelected;
}



/**
 * Return the montage number of columns for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of columns 
 */
int32_t 
ModelYokingGroup::getMontageNumberOfColumns(const int32_t /*windowTabNumber*/) const
{    
    return this->montageNumberOfColumns;
}


/**
 * Set the montage number of columns in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageNumberOfColumns
 *    New value for montage number of columns 
 */
void 
ModelYokingGroup::setMontageNumberOfColumns(const int32_t /*windowTabNumber*/,
                               const int32_t montageNumberOfColumns)
{    
    this->montageNumberOfColumns = montageNumberOfColumns;
}

/**
 * Return the montage number of rows for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage number of rows
 */
int32_t 
ModelYokingGroup::getMontageNumberOfRows(const int32_t /*windowTabNumber*/) const
{
    return this->montageNumberOfRows;
}

/**
 * Set the montage number of rows in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageNumberOfRows
 *    New value for montage number of rows 
 */
void 
ModelYokingGroup::setMontageNumberOfRows(const int32_t /*windowTabNumber*/,
                            const int32_t montageNumberOfRows)
{    
    this->montageNumberOfRows = montageNumberOfRows;
}

/**
 * Return the montage slice spacing for the given window tab.
 * @param windowTabNumber
 *   Tab Number of window.
 * @return
 *   Montage slice spacing.
 */
int32_t 
ModelYokingGroup::getMontageSliceSpacing(const int32_t /*windowTabNumber*/) const
{    
    return this->montageSliceSpacing;
}

/**
 * Set the montage slice spacing in the given window tab.
 * @param windowTabNumber
 *    Tab number of window.
 * @param montageSliceSpacing
 *    New value for montage slice spacing 
 */
void 
ModelYokingGroup::setMontageSliceSpacing(const int32_t /*windowTabNumber*/,
                            const int32_t montageSliceSpacing)
{
    this->montageSliceSpacing = montageSliceSpacing;
}

/**
 * Set the selected slices to the origin.
 * @param  windowTabNumber  Window for which slices set to origin is requested.
 */
void
ModelYokingGroup::setSlicesToOrigin(const int32_t /*windowTabNumber*/)
{
    this->volumeSlicesSelected.selectSlicesAtOrigin();
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
ModelYokingGroup::getOverlaySet(const int /*tabIndex*/)
{
    CaretAssertMessage(0, "NEVER should be called.");
    return NULL;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
ModelYokingGroup::getOverlaySet(const int /*tabIndex*/) const
{
    CaretAssertMessage(0, "NEVER should be called.");
    return NULL;
}

/**
 * Initilize the overlays for this controller.
 */
void 
ModelYokingGroup::initializeOverlays()
{
    CaretAssertMessage(0, "NEVER should be called.");
}



