#ifndef __VOLUME_MAPPABLE_INTERFACE_H__
#define __VOLUME_MAPPABLE_INTERFACE_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include "VolumeSliceViewPlaneEnum.h"

namespace caret {
    
    class BoundingBox;
    
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

        /**
         * @return The number of componenents per voxel.
         */
        virtual const int64_t& getNumberOfComponents() const = 0;
        
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
         * @param indexIn
         *     IJK indices
         * @param coordOut
         *     Output XYZ coordinates.
         */
        virtual void indexToSpace(const int64_t* indexIn,
                                  float* coordOut) const = 0;
        
        /**
         * Convert a coordinate to indices.  Note that output indices
         * MAY NOT BE WITHING THE VALID VOXEL DIMENSIONS.
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
         * Get a bounding box for the voxel coordinate ranges.
         *
         * @param boundingBoxOut
         *    The output bounding box.
         */
        virtual void getVoxelSpaceBoundingBox(BoundingBox& boundingBoxOut) const = 0;
        
        /**
         * Get the voxel colors for a slice in the map.
         *
         * @param mapIndex
         *    Index of the map.
         * @param slicePlane
         *    The slice plane.
         * @param sliceIndex
         *    Index of the slice.
         * @param rgbaOut
         *    Output containing the rgba values (must have been allocated
         *    by caller to sufficient count of elements in the slice).
         */
        virtual void getVoxelColorsForSliceInMap(const int32_t mapIndex,
                                                 const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                                 const int64_t sliceIndex,
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
         * @param rgbaOut
         *     Output containing RGBA values for voxel at the given indices.
         */
        virtual void getVoxelColorInMap(const int64_t indexIn1,
                                        const int64_t indexIn2,
                                        const int64_t indexIn3,
                                        const int64_t brickIndex,
                                        uint8_t rgbaOut[4]) const = 0;
        
        /**
         * @return The type of volume file.
         */
        //virtual SubvolumeAttributes::VolumeType getType() const = 0;
        
        /**
         * Get the value of a voxel at the given indices.
         *
         * @param indexIn1
         *     First dimension (i).
         * @param indexIn2
         *     Second dimension (j).
         * @param indexIn3
         *     Third dimension (k).
         * @param brickIndex
         *     Time/map index (default 0).
         * @param component
         *     Index of the component in the voxel (default 0).
         * @return
         *     Value of date at the given voxel indices.
         */
        virtual const float& getValue(const int64_t& indexIn1,
                                      const int64_t& indexIn2,
                                      const int64_t& indexIn3,
                                      const int64_t brickIndex = 0,
                                      const int64_t component = 0) const = 0;
    };
    
#ifdef __VOLUME_MAPPABLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MAPPABLE_INTERFACE_DECLARE__
    
} // namespace
#endif  //__VOLUME_MAPPABLE_INTERFACE_H__
