
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

#define __VOLUME_MAPPABLE_INTERFACE_DECLARE__
#include "VolumeMappableInterface.h"
#undef __VOLUME_MAPPABLE_INTERFACE_DECLARE__

#include "CaretAssert.h"
#include "VolumeSpace.h"

using namespace caret;


    
/**
 * Get the voxel spacing for each of the spatial dimensions.
 *
 * @param spacingOut1
 *    Spacing for the first dimension (typically X).
 * @param spacingOut2
 *    Spacing for the first dimension (typically Y).
 * @param spacingOut3
 *    Spacing for the first dimension (typically Z).
 */
void
VolumeMappableInterface::getVoxelSpacing(float& spacingOut1,
                                         float& spacingOut2,
                                         float& spacingOut3) const
{
    //TSC: below code will always return positive, so the calling code doesn't need to use abs() anymore
    Vector3D istep, jstep, kstep, origin;
    getVolumeSpace().getSpacingVectors(istep, jstep, kstep, origin);
    spacingOut1 = istep.length();
    spacingOut2 = jstep.length();
    spacingOut3 = kstep.length();
}

/**
 * Does this volume have these spatial dimensions?
 *
 * @param dim1
 *     First dimension.
 * @param dim2
 *     Second dimension.
 * @param dim3
 *     Third dimension.
 * @return
 *     True if this volume's spatial dimensions match the
 *     given dimensions, else false.
 */
bool
VolumeMappableInterface::matchesDimensions(const int64_t dim1,
                                           const int64_t dim2,
                                           const int64_t dim3) const
{
    std::vector<int64_t> dims;
    getDimensions(dims);
    
    if (dims.size() >= 3){
        if ((dims[0] == dim1)
            && (dims[1] == dim2)
            && (dims[2] == dim3)) {
            return true;
        }
    }
    
    return false;
}
/**
 * Adjust the given indices so that they are valid
 * in the range 0 to (volume dimension - 1)
 *
 * @param index1
 *     First index.
 * @param index2
 *     Second index.
 * @param index3
 *     Third index.
 */
void
VolumeMappableInterface::limitIndicesToValidIndices(int64_t& index1,
                                                int64_t& index2,
                                                int64_t& index3) const
{
    std::vector<int64_t> dims;
    getDimensions(dims);
    
    if (dims.size() >= 3) {
        if (index1 >= dims[0]) {
            index1 = dims[0] - 1;
        }
        if (index1 < 0) {
            index1 = 0;
        }
        
        if (index2 >= dims[1]) {
            index2 = dims[1] - 1;
        }
        if (index2 < 0) {
            index2 = 0;
        }

        if (index3 >= dims[2]) {
            index3 = dims[2] - 1;
        }
        if (index3 < 0) {
            index3 = 0;
        }
    }
}

/**
 * Get the dimensions for the parasagittal, coronal, and axial dimensions.
 * If the volume is 'plumb', these dimensions will be correct for any orientation.
 * Otherwise, if the volume is NOT plumb, parasagittal will contain first dimension,
 * coronal the second dimension, and axial the third dimension.
 * 
 * @param dimParasagittalOut
 *   Dimension for parasagittal axis
 * @param dimCoronalOut
 *   Dimension for coronal axis
 * @param dimAxialOut
 *   Dimension for axial axis
 */
void
VolumeMappableInterface::getDimensionsPCA(int64_t& dimParasagittalOut,
                                          int64_t& dimCoronalOut,
                                          int64_t& dimAxialOut) const
{
    dimParasagittalOut = 0;
    dimCoronalOut      = 0;
    dimAxialOut        = 0;
    
    std::vector<int64_t> dimensions;
    getDimensions(dimensions);
    if (dimensions.size() < 3) {
        return;
    }
    
    const VolumeSpace& volumeSpace(getVolumeSpace());
    
    VolumeSpace::OrientTypes orientation[3];
    volumeSpace.getOrientation(orientation);
    
    if (volumeSpace.isPlumb()) {
        dimParasagittalOut = -1;
        dimCoronalOut      = -1;
        dimAxialOut        = -1;
        for (int32_t i = 0; i < 3; i++) {
            switch (orientation[i]) {
                case VolumeSpace::LEFT_TO_RIGHT:
                case VolumeSpace::RIGHT_TO_LEFT:
                    dimParasagittalOut = dimensions[i];
                    break;
                case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                    dimCoronalOut = dimensions[i];
                    break;
                case VolumeSpace::INFERIOR_TO_SUPERIOR:
                case VolumeSpace::SUPERIOR_TO_INFERIOR:
                    dimAxialOut = dimensions[i];
                    break;
            }
        }
        CaretAssert(dimParasagittalOut >= 0);
        CaretAssert(dimCoronalOut      >= 0);
        CaretAssert(dimAxialOut        >= 0);
    }
    else {
        dimParasagittalOut = dimensions[0];
        dimCoronalOut      = dimensions[1];
        dimAxialOut        = dimensions[2];
    }
}
