#ifndef __VOLUME_MAPPABLE_INTERFACE_H__
#define __VOLUME_MAPPABLE_INTERFACE_H__

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

#include "Vector3D.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSpace.h"
#include "VolumeToImageMappingModeEnum.h"

namespace caret {
    
    class BoundingBox;
    class CaretMappableDataFile;
    class HistologySlice;
    class MediaFile;
    class GraphicsPrimitive;
    class GraphicsPrimitiveV3fT3f;
    class TabDrawingInfo;

    /**
     * \class caret::VolumeMappableInterface
     * \brief Interface for data that is mapped to volumes
     * \ingroup FilesBase
     *
     * Defines an interface for data files that are drawn with voxel data.
     */
    class VolumeMappableInterface {
        
    protected:
        VolumeMappableInterface() { }
        
        virtual ~VolumeMappableInterface() { }
        
    private:
        VolumeMappableInterface(const VolumeMappableInterface&);
        
        VolumeMappableInterface& operator=(const VolumeMappableInterface&);
        
    public:
        /**
         * @return Instance cast to a Volume Mappable CaretMappableDataFile
         */
        virtual CaretMappableDataFile* castToVolumeMappableDataFile() = 0;
        
        /**
         * @return Instance cast to a Volume Mappable CaretMappableDataFile (const method)
         */
        virtual const CaretMappableDataFile* castToVolumeMappableDataFile() const = 0;
        
        /**
         * Get the dimensions of the volume.
         *
         * @param dimOut1
         *     First dimension (i) out.
         * @param dimOut2
         *     Second dimension (j) out.
         * @param dimOut3
         *     Third dimension (k) out.
         * @param dimTimeOut
         *     Time dimensions out (number of maps)
         * @param numComponents
         *     Number of components per voxel.
         */
        virtual void getDimensions(int64_t& dimOut1,
                                   int64_t& dimOut2,
                                   int64_t& dimOut3,
                                   int64_t& dimTimeOut,
                                   int64_t& numComponents) const = 0;
        
        /**
         * Get the dimensions of the volume.
         *
         * @param dimsOut
         *     Will contain 5 elements: (0) X-dimension, (1) Y-dimension
         * (2) Z-dimension, (3) time, (4) components.
         */
        virtual void getDimensions(std::vector<int64_t>& dimsOut) const = 0;

        void getDimensionsPCA(int64_t& dimParasagittalOut,
                              int64_t& dimCoronalOut,
                              int64_t& dimAxialOut) const;

        bool matchesDimensions(const int64_t dim1,
                               const int64_t dim2,
                               const int64_t dim3) const;
        
        void limitIndicesToValidIndices(int64_t& index1,
                                        int64_t& index2,
                                        int64_t& index3) const;
        
        /**
         * @return The number of componenents per voxel.
         */
        virtual const int64_t& getNumberOfComponents() const = 0;
        
        /**
         * Get the value of the voxel containing the given coordinate.
         *
         * @param coordinateIn
         *    The 3D coordinate
         * @param validOut
         *    If not NULL, will indicate if the coordinate (and hence the 
         *    returned value) is valid.
         * @param mapIndex
         *    Index of map.
         * @param component
         *    Voxel component.
         * @return
         *    Value of voxel containing the given coordinate.
         */
        virtual float getVoxelValue(const float* coordinateIn,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const = 0;
        
        /**
         * Get the value of the voxel containing the given coordinate.
         *
         * @param coordinateX
         *    The X coordinate
         * @param coordinateY
         *    The Y coordinate
         * @param coordinateZ
         *    The Z coordinate
         * @param validOut
         *    If not NULL, will indicate if the coordinate (and hence the
         *    returned value) is valid.
         * @param mapIndex
         *    Index of map.
         * @param component
         *    Voxel component.
         * @return
         *    Value of voxel containing the given coordinate.
         */
        virtual float getVoxelValue(const float coordinateX,
                                    const float coordinateY,
                                    const float coordinateZ,
                                    bool* validOut = NULL,
                                    const int64_t mapIndex = 0,
                                    const int64_t component = 0) const = 0;
        
        /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param coordOut1
         *     Output first (x) coordinate.
         * @param coordOut2
         *     Output first (y) coordinate.
         * @param coordOut3
         *     Output first (z) coordinate.
         */
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float& coordOut1,
                                  float& coordOut2,
                                  float& coordOut3) const = 0;
        
        /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param coordOut
         *     Output XYZ coordinates.
         */
        virtual void indexToSpace(const float& indexIn1,
                                  const float& indexIn2,
                                  const float& indexIn3,
                                  float* coordOut) const = 0;
        
        /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @return
         *     The XYZ coordinate.
         */
        Vector3D indexToSpace(const float& indexIn1,
                              const float& indexIn2,
                              const float& indexIn3) const;

        /**
         * Convert an index to space (coordinates).
         *
         * @param voxelIJK
         *     The voxel IJK
         * @return
         *     The XYZ coordinate.
         */
        Vector3D indexToSpace(const VoxelIJK& voxelIJK) const;
        
       /**
         * Convert an index to space (coordinates).
         *
         * @param indexIn
         *     IJK indices
         * @param coordOut
         *     Output XYZ coordinates.
         */
        virtual void indexToSpace(const int64_t* indexIn,
                                  float* coordOut) const = 0;
        
        /**
         * Convert a coordinate to indices.  Note that output indices
         * MAY NOT BE WITHIN THE VALID VOXEL DIMENSIONS.
         *
         * @param coordIn1
         *     First (x) input coordinate.
         * @param coordIn2
         *     Second (y) input coordinate.
         * @param coordIn3
         *     Third (z) input coordinate.
         * @param indexOut1
         *     First output index (i).
         * @param indexOut2
         *     First output index (j).
         * @param indexOut3
         *     First output index (k).
         */
        virtual void enclosingVoxel(const float& coordIn1,
                                    const float& coordIn2,
                                    const float& coordIn3,
                                    int64_t& indexOut1,
                                    int64_t& indexOut2,
                                    int64_t& indexOut3) const = 0;
        
        /**
         * Convert a coordinate to indices.  Note that output indices
         * MAY NOT BE WITHIN THE VALID VOXEL DIMENSIONS.
         *
         * @param coordIn1
         *     First (x) input coordinate.
         * @param coordIn2
         *     Second (y) input coordinate.
         */
        virtual void enclosingVoxel(const float* coordIn,
                                    int64_t* indexOut) const = 0;

        /**
         * Determine in the given voxel indices are valid (within the volume).
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param coordOut1
         *     Output first (x) coordinate.
         * @param brickIndex
         *     Time/map index (default 0).
         * @param component
         *     Voxel component (default 0).
         */
        virtual bool indexValid(const int64_t& indexIn1,
                                const int64_t& indexIn2,
                                const int64_t& indexIn3,
                                const int64_t brickIndex = 0,
                                const int64_t component = 0) const = 0;
        
        /**
         * Determine in the given voxel indices are valid (within the volume).
         *
         * @param indexIn
         *     IJK
         * @param brickIndex
         *     Time/map index (default 0).
         * @param component
         *     Voxel component (default 0).
         */
        virtual bool indexValid(const int64_t* indexIn,
                                const int64_t brickIndex = 0,
                                const int64_t component = 0) const = 0;
        
        /**
         * Get a bounding box for the voxel coordinate ranges.
         *
         * @param boundingBoxOut
         *    The output bounding box.
         */
        virtual void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const = 0;
        
        /**
         * Get a bounding box containing the non-zero voxel coordinate ranges
         * @param mapIndex
         *    Index of map
         * @param boundingBoxOut
         *    Output containing coordinate range of non-zero voxels
         */
        virtual void getNonZeroVoxelCoordinateBoundingBox(const int32_t mapIndex,
                                                   BoundingBox& boundingBoxOut) const = 0;
        
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
        void getVoxelSpacing(float& spacingOut1,
                             float& spacingOut2,
                             float& spacingOut3) const;
        
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
        void getVoxelSpacingPCA(float& spacingParasagittalXOut,
                                float& spacingCoronalYOut,
                                float& spacingAxialZOut) const;

        float getMaximumVoxelSpacing() const;
        
        /**
         * Get the voxel colors for a slice in the map.
         *
         * @param mapIndex
         *    Index of the map.
         * @param slicePlane
         *    The slice plane.
         * @param sliceIndex
         *    Index of the slice.
         * @param tabDrawingInfo
         *    Info for drawing the tab
         * @param rgbaOut
         *    Output containing the rgba values (must have been allocated
         *    by caller to sufficient count of elements in the slice).
         * @return
         *    Number of voxels with alpha greater than zero
         */
        virtual int64_t getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                    const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                    const int64_t sliceIndex,
                                                    const TabDrawingInfo& tabDrawingInfo,
                                                    uint8_t* rgbaOut) const = 0;
        
        /**
         * Get voxel coloring for a set of voxels.
         *
         * @param mapIndex
         *     Index of map.
         * @param firstVoxelIJK
         *    IJK Indices of first voxel
         * @param rowStepIJK
         *    IJK Step for moving to next row.
         * @param columnStepIJK
         *    IJK Step for moving to next column.
         * @param numberOfRows
         *    Number of rows.
         * @param numberOfColumns
         *    Number of columns.
         * @param tabDrawingInfo
         *    Info for drawing the tab
         * @param rgbaOut
         *    RGBA color components out.
         * @return
         *    Number of voxels with alpha greater than zero
         */
        virtual int64_t getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                    const int64_t firstVoxelIJK[3],
                                                    const int64_t rowStepIJK[3],
                                                    const int64_t columnStepIJK[3],
                                                    const int64_t numberOfRows,
                                                    const int64_t numberOfColumns,
                                                    const TabDrawingInfo& tabDrawingInfo,
                                                    uint8_t* rgbaOut) const = 0;
        
        /**
         * Get the voxel colors for a sub slice in the map.
         *
         * @param mapIndex
         *    Index of the map.
         * @param slicePlane
         *    The slice plane.
         * @param sliceIndex
         *    Index of the slice.
         * @param firstCornerVoxelIndex
         *    Indices of voxel for first corner of sub-slice (inclusive).
         * @param lastCornerVoxelIndex
         *    Indices of voxel for last corner of sub-slice (inclusive).
         * @param voxelCountIJK
         *    Voxel counts for each axis.
         * @param tabDrawingInfo
         *    Info for drawing the tab
         * @param rgbaOut
         *    Output containing the rgba values (must have been allocated
         *    by caller to sufficient count of elements in the slice).
         * @return
         *    Number of voxels with alpha greater than zero
         */
        virtual int64_t getVoxelColorsForSubSliceInMap(const int32_t mapIndex,
                                                       const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                       const int64_t sliceIndex,
                                                       const int64_t firstCornerVoxelIndex[3],
                                                       const int64_t lastCornerVoxelIndex[3],
                                                       const int64_t voxelCountIJK[3],
                                                       const TabDrawingInfo& tabDrawingInfo,
                                                       uint8_t* rgbaOut) const = 0;
        
        /**
         * Get the voxel coloring for the voxel at the given indices.
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param brickIndex
         *     Time/map index.
         * @param tabDrawingInfo
         *    Info for drawing the tab
         * @param rgbaOut
         *     Output containing RGBA values for voxel at the given indices.
         */
        virtual void getVoxelColorInMap(const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t brickIndex,
                                        const TabDrawingInfo& tabDrawingInfo,
                                        uint8_t rgbaOut[4]) const = 0;
        
        /**
         * Get the voxel coloring for the voxel at the given indices.
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param brickIndex
         *     Time/map index.
         * @param rgbaOut
         *     Output containing RGBA values for voxel at the given indices.
         */
        virtual void getVoxelColorInMap(const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t tabIndex,
                                        uint8_t rgbaOut[4]) const = 0;
        
        virtual GraphicsPrimitiveV3fT3f* getVolumeDrawingTriangleStripPrimitive(const int32_t mapIndex,
                                                                                const TabDrawingInfo& tabDrawingInfo) const = 0;
        
        virtual GraphicsPrimitiveV3fT3f* getVolumeDrawingTriangleFanPrimitive(const int32_t mapIndex,
                                                                              const TabDrawingInfo& tabDrawingInfo) const = 0;
        
        virtual GraphicsPrimitiveV3fT3f* getVolumeDrawingTrianglesPrimitive(const int32_t mapIndex,
                                                                            const TabDrawingInfo& tabDrawingInfo) const = 0;

        virtual GraphicsPrimitive* getHistologyImageIntersectionPrimitive(const int32_t mapIndex,
                                                                          const TabDrawingInfo& tabDrawingInfo,
                                                                          const MediaFile* mediaFile,
                                                                          const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                          const float volumeSliceThickness,
                                                                          AString& errorMessageOut) const = 0;
        
        virtual std::vector<GraphicsPrimitive*> getHistologySliceIntersectionPrimitive(const int32_t mapIndex,
                                                                                       const TabDrawingInfo& tabDrawingInfo,
                                                                                       const HistologySlice* histologySlice,
                                                                                       const VolumeToImageMappingModeEnum::Enum volumeMappingMode,
                                                                                       const float volumeSliceThickness,
                                                                                       AString& errorMessageOut) const = 0;

        /**
         * Get the volume space object, so we have access to all functions associated with volume spaces
         */
        virtual const VolumeSpace& getVolumeSpace() const = 0;
        
        bool isThin() const;
        
        bool isSingleSlice() const;
        
        int32_t getSingleSliceDimensionIndex() const;
        
        VolumeSliceViewPlaneEnum::Enum getSingleSliceViewPlane() const;
        
        void getSingleSliceCornersXYZ(Vector3D& bottomLeftXYZ,
                                      Vector3D& bottomRightXYZ,
                                      Vector3D& topRightXYZ,
                                      Vector3D& topLeftXYZ) const;
        
        void getSingleSliceCornersIJK(VoxelIJK& bottomLeftIJK,
                                      VoxelIJK& bottomRightIJK,
                                      VoxelIJK& topRightIJK,
                                      VoxelIJK& topLeftIJK) const;
        
        Vector3D getSingleSliceCenterXYZ() const;
        
        Vector3D getSingleSliceNormalVector() const;
    };
    
#ifdef __VOLUME_MAPPABLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MAPPABLE_INTERFACE_DECLARE__
    
} // namespace
#endif  //__VOLUME_MAPPABLE_INTERFACE_H__
