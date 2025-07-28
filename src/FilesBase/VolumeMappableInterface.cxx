
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
#include "DataFile.h"
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
 * Get the voxel spacing for parasagittal (X), coronal (Y), and axial (Z)
 *
 * @param spacingParasagittalXOut
 *    Spacing for the first dimension (typically X).
 * @param spacingCoronalYOut
 *    Spacing for the first dimension (typically Y).
 * @param spacingAxialZOut
 *    Spacing for the first dimension (typically Z).
 */
void
VolumeMappableInterface::getVoxelSpacingPCA(float& spacingParasagittalXOut,
                                            float& spacingCoronalYOut,
                                            float& spacingAxialZOut) const
{
    spacingParasagittalXOut = 0.0;
    spacingCoronalYOut      = 0.0;
    spacingAxialZOut        = 0.0;
    
    Vector3D spacing;
    getVoxelSpacing(spacing[0], spacing[1], spacing[2]);
    
    VolumeSpace::OrientTypes orientation[3];
    getVolumeSpace().getOrientation(orientation);
    
    if (getVolumeSpace().isPlumb()) {
        for (int32_t i = 0; i < 3; i++) {
            switch (orientation[i]) {
                case VolumeSpace::LEFT_TO_RIGHT:
                case VolumeSpace::RIGHT_TO_LEFT:
                    spacingParasagittalXOut = spacing[i];
                    break;
                case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                    spacingCoronalYOut = spacing[i];
                    break;
                case VolumeSpace::INFERIOR_TO_SUPERIOR:
                case VolumeSpace::SUPERIOR_TO_INFERIOR:
                    spacingAxialZOut = spacing[i];
                    break;
            }
        }
    }
    else {
        spacingParasagittalXOut = spacing[0];
        spacingCoronalYOut      = spacing[1];
        spacingAxialZOut        = spacing[2];
    }
}

/**
 * @return The maximum voxel spacing
 */
float
VolumeMappableInterface::getMaximumVoxelSpacing() const
{
    float maxSpacing(1.0);
    
    float sp, sc, sa;
    getVoxelSpacing(sp, sc, sa);
    maxSpacing = std::max(sp, std::max(sc, sa));
    
    if (maxSpacing == 0.0) {
        maxSpacing = 1.0;
    }

    return maxSpacing;
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

/**
 * Convert an index to space (coordinates).
 *
 * @param indexIn1
 *     First dimension (i).
 * @param indexIn2
 *     Second dimension (j).
 * @param indexIn3
 *     Third dimension (k).
 * @return coordOut
 *     The XYZ coordinate.
 */
Vector3D
VolumeMappableInterface::indexToSpace(const float& indexIn1,
                                      const float& indexIn2,
                                      const float& indexIn3) const
{
    Vector3D xyz;
    
    indexToSpace(indexIn1,
                 indexIn2,
                 indexIn3,
                 xyz);
    return xyz;
}

/**
 * Convert an index to space (coordinates).
 *
 * @param voxelIJK
 *     The voxel IJK
 * @return
 *     The XYZ coordinate.
 */
Vector3D 
VolumeMappableInterface::indexToSpace(const VoxelIJK& voxelIJK) const
{
    return indexToSpace(voxelIJK.i(),
                        voxelIJK.j(),
                        voxelIJK.k());
}

/**
 * @return True if this volume is thin
 * (a spatial dimension is 2 or fewer slices).
 */
bool 
VolumeMappableInterface::isThin() const
{
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    if ((dimI <= 2)
        || (dimJ <= 2)
        || (dimK <= 2)) {
        return true;
    }
    return false;
}


/**
 * @return True if the volume is a single slice (one of the voxel dimensions is one
 * and the other two dimensions are greater than one)
 */
bool 
VolumeMappableInterface::isSingleSlice() const
{
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    if ((dimI == 1)
        || (dimJ == 1)
        || (dimK == 1)) {
        return true;
    }

    return false;
}

/**
 * @return 0, 1, 2 indicate dimension that is single slice.
 * -1 indicates volume IS NOT a single slice.
 */
int32_t 
VolumeMappableInterface::getSingleSliceDimensionIndex() const
{
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    
    int32_t singleSliceDimension(2);
    
    if (dimI == 1) {
        if ((dimJ > 1) && (dimK > 1)) {
            singleSliceDimension = 0;
        }
    }
    else if (dimJ == 1) {
        if ((dimI > 1) && (dimK > 1)) {
            singleSliceDimension = 1;
        }
    }
    else if (dimK == 1) {
        if ((dimI > 1) && (dimJ > 1)) {
            singleSliceDimension = 2;
        }
    }
    
    return singleSliceDimension;
}

/**
 * @return Slice view plane for viewing single slice.
 * Returns ALL if volume IS NOT a single slice.
 */
VolumeSliceViewPlaneEnum::Enum 
VolumeMappableInterface::getSingleSliceViewPlane() const
{
    VolumeSliceViewPlaneEnum::Enum sliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    
    const int32_t singleSliceDim(getSingleSliceDimensionIndex());
    switch (singleSliceDim) {
        case 0:
            sliceViewPlane = VolumeSliceViewPlaneEnum::PARASAGITTAL;
            break;
        case 1:
            sliceViewPlane = VolumeSliceViewPlaneEnum::CORONAL;
            break;
        case 2:
            sliceViewPlane = VolumeSliceViewPlaneEnum::AXIAL;
            break;
        default:
            sliceViewPlane = VolumeSliceViewPlaneEnum::ALL;
            break;
    }
    
    return sliceViewPlane;
}

/**
 * Get the single slice volume corners for drawing
 * @param bottomLeftIJK
 *    Output with IJK at bottom left
 * @param bottomRightIJK
 *    Output with IJK at bottom right
 * @param topRightIJK
 *    Output with IJK at top right
 * @param topLeftIJK
 *    Output with IJK at top left
 */
void
VolumeMappableInterface::getSingleSliceCornersIJK(VoxelIJK& bottomLeftIJK,
                                                  VoxelIJK& bottomRightIJK,
                                                  VoxelIJK& topRightIJK,
                                                  VoxelIJK& topLeftIJK) const
{
    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
    const int64_t iMax(dimI - 1);
    const int64_t jMax(dimJ - 1);
    const int64_t kMax(dimK - 1);
    
    Vector3D xyzBL, xyzBR, xyzTL, xyzTR;
    switch (getSingleSliceDimensionIndex()) {
        case 0:
            bottomLeftIJK  = VoxelIJK(0, 0, 0);
            bottomRightIJK = VoxelIJK(0, jMax, 0);
            topLeftIJK     = VoxelIJK(0, 0, kMax);
            topRightIJK    = VoxelIJK(0, jMax, kMax);
            break;
        case 1:
            bottomLeftIJK  = VoxelIJK(0, 0, 0);
            bottomRightIJK = VoxelIJK(iMax, 0, 0);
            topLeftIJK     = VoxelIJK(0, 0, kMax);
            topRightIJK    = VoxelIJK(iMax, 0, kMax);
            break;
        case 2:
        default:
            bottomLeftIJK  = VoxelIJK(0, 0, 0);
            bottomRightIJK = VoxelIJK(iMax, 0, 0);
            topLeftIJK     = VoxelIJK(0, jMax, 0);
            topRightIJK    = VoxelIJK(iMax, jMax, 0);
            break;
//        default:
//            CaretAssert(0);
            break;
    }
}


/**
 * Get the single slice volume corners for drawing
 * @param bottomLeftXYZ
 *    Output with XYZ at bottom left
 * @param bottomRightXYZ
 *    Output with XYZ at bottom right
 * @param topRightXYZ
 *    Output with XYZ at top right
 * @param topLeftXYZ
 *    Output with XYZ at top left
 */
void
VolumeMappableInterface::getSingleSliceCornersXYZ(Vector3D& bottomLeftXYZ,
                                                  Vector3D& bottomRightXYZ,
                                                  Vector3D& topRightXYZ,
                                                  Vector3D& topLeftXYZ) const
{
//    int64_t dimI(0), dimJ(0), dimK(0), dimTime(0), dimComp(0);
//    getDimensions(dimI, dimJ, dimK, dimTime, dimComp);
//    const int64_t iMax(dimI - 1);
//    const int64_t jMax(dimJ - 1);
//    const int64_t kMax(dimK - 1);
//
//    Vector3D xyzBL, xyzBR, xyzTL, xyzTR;
//    switch (getSingleSliceDimensionIndex()) {
//        case 0:
//            xyzBL = indexToSpace(0, 0, 0);
//            xyzBR = indexToSpace(0, jMax, 0);
//            xyzTL = indexToSpace(0, 0, kMax);
//            xyzTR = indexToSpace(0, jMax, kMax);
//            break;
//        case 1:
//            xyzBL = indexToSpace(0, 0, 0);
//            xyzBR = indexToSpace(iMax, 0, 0);
//            xyzTL = indexToSpace(0, 0, kMax);
//            xyzTR = indexToSpace(iMax, 0, kMax);
//            break;
//        case 2:
//            xyzBL = indexToSpace(0, 0, 0);
//            xyzBR = indexToSpace(iMax, 0, 0);
//            xyzTL = indexToSpace(0, jMax, 0);
//            xyzTR = indexToSpace(iMax, jMax, 0);
//            break;
//        default:
//            CaretAssert(0);
//            break;
//    }
//    
//    std::vector<Vector3D> cornersXYZ;
//    cornersXYZ.push_back(xyzBL);
//    cornersXYZ.push_back(xyzBR);
//    cornersXYZ.push_back(xyzTR);
//    cornersXYZ.push_back(xyzTL);
//    return cornersXYZ;
    
    VoxelIJK bottomLeftIJK;
    VoxelIJK bottomRightIJK;
    VoxelIJK topRightIJK;
    VoxelIJK topLeftIJK;
    getSingleSliceCornersIJK(bottomLeftIJK, 
                             bottomRightIJK,
                             topRightIJK,
                             topLeftIJK);
    topLeftXYZ     = indexToSpace(topLeftIJK);
    topRightXYZ    = indexToSpace(topRightIJK);
    bottomRightXYZ = indexToSpace(bottomRightIJK);
    bottomLeftXYZ  = indexToSpace(bottomLeftIJK);
}

/**
 * @return Normal vector for a single-slice volume
 */
Vector3D
VolumeMappableInterface::getSingleSliceNormalVector() const
{
    Vector3D bottomLeftXYZ;
    Vector3D bottomRightXYZ;
    Vector3D topRightXYZ;
    Vector3D topLeftXYZ;
    
    getSingleSliceCornersXYZ(bottomLeftXYZ,
                             bottomRightXYZ,
                             topRightXYZ,
                             topLeftXYZ);
    
    const Vector3D v1(topLeftXYZ - bottomLeftXYZ);
    const Vector3D v2(bottomRightXYZ - bottomLeftXYZ);
    const Vector3D normalVector(v1.cross(v2).normal());
    return normalVector;
}

/**
 * @return XYZ at center of slice
 */
Vector3D
VolumeMappableInterface::getSingleSliceCenterXYZ() const
{
    Vector3D centerXYZ(0.0, 0.0, 0.0);
    
    int64_t dimI, dimJ, dimK, dimTime, dimComponents;
    getDimensions(dimI, dimJ, dimK, dimTime, dimComponents);
    if ((dimI >= 1) && (dimJ >= 1) && (dimK >= 1)) {
        const int64_t indexI(dimI / 2);
        const int64_t indexJ(dimJ / 2);
        const int64_t indexK(dimK / 2);
        if (indexValid(indexI, indexJ, indexK)) {
            centerXYZ = indexToSpace(indexI, indexJ, indexK);
        }
    }

    
    return centerXYZ;
}
