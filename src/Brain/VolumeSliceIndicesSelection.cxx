
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

#define __VOLUME_SLICE_INDICES_SELECTION_DECLARE__
#include "VolumeSliceIndicesSelection.h"
#undef __VOLUME_SLICE_INDICES_SELECTION_DECLARE__

#include "CaretAssert.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class VolumeSliceIndicesSelection 
 * \brief Provides a mechanism for volume slice selection.
 *
 * This class provides a mechanism for selection of volume
 * slices indices and is used for display of volume slices
 * in the various viewers.
 */

/**
 * Constructor.
 */
VolumeSliceIndicesSelection::VolumeSliceIndicesSelection()
: CaretObject()
{
    this->reset();
}

/**
 * Destructor.
 */
VolumeSliceIndicesSelection::~VolumeSliceIndicesSelection()
{
    
}

/**
 * Reset the slices.
 */
void 
VolumeSliceIndicesSelection::reset()
{
    this->sliceIndexAxial        = 0;
    this->sliceIndexCoronal      = 0;
    this->sliceIndexParasagittal = 0;
    
    this->sliceEnabledAxial        = true;
    this->sliceEnabledCoronal      = true;
    this->sliceEnabledParasagittal = true;
    
    this->initializedFlag = false;
}

/**
 * Update the slices indices so that they are valid for
 * the given VolumeFile.
 * @param volumeFile
 *   File for which slice indices are made valid.
 */
void 
VolumeSliceIndicesSelection::updateForVolumeFile(/*const*/ VolumeFile* volumeFile)
{
    CaretAssert(volumeFile);
    
    int64_t dimI, dimJ, dimK, numMaps, numComponents;
    volumeFile->getDimensions(dimI, dimJ, dimK, numMaps, numComponents);
    
    if (this->sliceIndexParasagittal >= dimI) {
        this->sliceIndexParasagittal = dimI - 1;
    }
    if (this->sliceIndexParasagittal < 0) {
        this->sliceIndexParasagittal = 0;
    }
    
    if (this->sliceIndexCoronal >= dimJ) {
        this->sliceIndexCoronal = dimJ - 1;
    }
    if (this->sliceIndexCoronal < 0) {
        this->sliceIndexCoronal = 0;
    }
    
    if (this->sliceIndexAxial >= dimK) {
        this->sliceIndexAxial = dimK - 1;
    }
    if (this->sliceIndexAxial < 0) {
        this->sliceIndexAxial = 0;
    }
    
    if (this->initializedFlag == false) {
        this->initializedFlag = true;
        this->selectSlicesAtOrigin(volumeFile);
    }
}

/**
 * Set the slice indices so that they are at the origin of
 * the given volume.
 * @param volumeFile
 *     Volume file whose origin is used for setting slice indices.
 */
void 
VolumeSliceIndicesSelection::selectSlicesAtOrigin(/*const*/ VolumeFile* volumeFile)
{
    CaretAssert(volumeFile);

    volumeFile->closestVoxel(0.0, 
                             0.0, 
                             0.0, 
                             this->sliceIndexParasagittal, 
                             this->sliceIndexCoronal, 
                             this->sliceIndexAxial);
    
    this->updateForVolumeFile(volumeFile);
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeSliceIndicesSelection::toString() const
{
    const AString msg = 
    "VolumeSliceIndicesSelection=(" 
    + AString::number(this->sliceIndexParasagittal)
    + ", "
    + AString::number(this->sliceIndexCoronal)
    + ", "
    + AString::number(this->sliceIndexAxial)
    + ")";
    
    return msg;
}

/**
 * Return the axial slice index.
 * @return
 *   Axial slice index.
 */
int64_t 
VolumeSliceIndicesSelection::getSliceIndexAxial() const
{    
    return this->sliceIndexAxial;
}

/**
 * Set the axial slice index.
 * @param 
 *    New value for axial slice index.
 */
void 
VolumeSliceIndicesSelection::setSliceIndexAxial(const int64_t sliceIndexAxial)
{    
    this->sliceIndexAxial = sliceIndexAxial;
}

/**
 * Return the coronal slice index.
 * @return
 *   Coronal slice index.
 */
int64_t 
VolumeSliceIndicesSelection::getSliceIndexCoronal() const
{    
    return this->sliceIndexCoronal;
}


/**
 * Set the coronal slice index.
 * @param 
 *    New value for coronal slice index.
 */
void 
VolumeSliceIndicesSelection::setSliceIndexCoronal(const int64_t sliceIndexCoronal)
{    
    this->sliceIndexCoronal = sliceIndexCoronal;
}

/**
 * Return the parasagittal slice index.
 * @return
 *   Parasagittal slice index.
 */
int64_t 
VolumeSliceIndicesSelection::getSliceIndexParasagittal() const
{
    return this->sliceIndexParasagittal;
}

/**
 * Set the parasagittal slice index.
 * @param 
 *    New value for parasagittal slice index.
 */
void 
VolumeSliceIndicesSelection::setSliceIndexParasagittal(const int64_t sliceIndexParasagittal)
{    
    this->sliceIndexParasagittal = sliceIndexParasagittal;
}

/**
 * Is the parasagittal slice enabled?
 * @return
 *    Enabled status of parasagittal slice.
 */
bool 
VolumeSliceIndicesSelection::isSliceParasagittalEnabled() const
{
    return this->sliceEnabledParasagittal;
}

/**
 * Set the enabled status of the parasagittal slice.
 * @param sliceEnabledParasagittal 
 *    New enabled status.
 */ 
void 
VolumeSliceIndicesSelection::setSliceParasagittalEnabled(const bool sliceEnabledParasagittal)
{
    this->sliceEnabledParasagittal = sliceEnabledParasagittal;
}

/**
 * Is the coronal slice enabled?
 * @return
 *    Enabled status of coronal slice.
 */
bool 
VolumeSliceIndicesSelection::isSliceCoronalEnabled() const
{
    return this->sliceEnabledCoronal;
}

/**
 * Set the enabled status of the coronal slice.
 * @param sliceEnabledCoronal 
 *    New enabled status.
 */ 
void 
VolumeSliceIndicesSelection::setSliceCoronalEnabled(const bool sliceEnabledCoronal)
{
    this->sliceEnabledCoronal = sliceEnabledCoronal;
}

/**
 * Is the axial slice enabled?
 * @return
 *    Enabled status of axial slice.
 */
bool 
VolumeSliceIndicesSelection::isSliceAxialEnabled() const
{
    return this->sliceEnabledAxial;
}

/**
 * Set the enabled status of the axial slice.
 * @param sliceEnabledParasagittal 
 *    New enabled status.
 */ 
void 
VolumeSliceIndicesSelection::setSliceAxialEnabled(const bool sliceEnabledAxial)
{
    this->sliceEnabledAxial = sliceEnabledAxial;
}
