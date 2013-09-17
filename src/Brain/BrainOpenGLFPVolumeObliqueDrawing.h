#ifndef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_H__
#define __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_H__

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


#include "BrainOpenGLFixedPipeline.h"
#include "CaretObject.h"



namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrowserTabContent;
    class Matrix4x4;
    
    class BrainOpenGLFPVolumeObliqueDrawing : public CaretObject {
        
    public:
        BrainOpenGLFPVolumeObliqueDrawing();
        
        virtual ~BrainOpenGLFPVolumeObliqueDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const int viewport[4]);
    private:
        enum DRAW_MODE {
            DRAW_MODE_ALL_VIEW,
            DRAW_MODE_VOLUME_VIEW_SLICE_SINGLE,
            DRAW_MODE_VOLUME_VIEW_SLICE_3D
        };
        
        /**
         * Holds values in the slice for a volume so that they 
         * can be colored all at once which is more efficient than
         * colors singles voxels many times
         */
        class VolumeSlice{
        public:
            /**
             * Constructor
             *
             * @param volumeMappableInterface
             *   Volume that contains the data values.
             */
            VolumeSlice(VolumeMappableInterface* volumeMappableInterface,
                        const int32_t mapIndex) {
                m_volumeMappableInterface = volumeMappableInterface;
                m_mapIndex = mapIndex;
                
                const int64_t sliceDim = 300;
                m_values.reserve(sliceDim * sliceDim);
            }
            
            /**
             * Add a value and return its index.
             *
             * @param value
             *     Value that is added.
             * @return
             *     The index for the value.
             */
            int64_t addValue(const float value) {
                const int64_t indx = static_cast<int64_t>(m_values.size());
                m_values.push_back(value);
                return indx;
            }
            
            /**
             * Return RGBA colors for value using the value's index
             * returned by addValue().
             *
             * @param indx
             *    Index of the value.
             * @return
             *    RGBA coloring for value.
             */
            uint8_t* getRgbaForValueByIndex(const int64_t indx) {
                CaretAssertVectorIndex(m_rgba, indx * 4);
                return &m_rgba[indx*4];
            }
            
            /**
             * Allocate colors for the voxel values
             */
            void allocateColors() {
                m_rgba.resize(m_values.size() * 4);
            }
            
            
            /**
             * Volume containing the values
             */
            VolumeMappableInterface* m_volumeMappableInterface;
            
            /**
             * Map index
             */
            int32_t m_mapIndex;
            
            /**
             * The voxel values
             */
            std::vector<float> m_values;
            
            /**
             * Coloring corresponding to the values (4 components per voxel)
             */
            std::vector<uint8_t> m_rgba;
        };

        /**
         * For each voxel, contains offsets to each layer
         */
        class VoxelToDraw {
        public:
            /**
             * Create a voxel for drawing.
             * 
             * @param center
             *    Center of voxel.
             * @param leftBottom
             *    Left bottom coordinate of voxel.
             * @param rightBottom
             *    Right bottom coordinate of voxel.
             * @param rightTop
             *    Right top coordinate of voxel.
             * @param leftTop
             *    Left top coordinate of voxel.
             */
            VoxelToDraw(const float center[3],
                        const double leftBottom[3],
                        const double rightBottom[3],
                        const double rightTop[3],
                        const double leftTop[3]) {
                m_center[0] = center[0];
                m_center[1] = center[1];
                m_center[2] = center[2];
                
                m_coordinates[0]  = leftBottom[0];
                m_coordinates[1]  = leftBottom[1];
                m_coordinates[2]  = leftBottom[2];
                m_coordinates[3]  = rightBottom[0];
                m_coordinates[4]  = rightBottom[1];
                m_coordinates[5]  = rightBottom[2];
                m_coordinates[6]  = rightTop[0];
                m_coordinates[7]  = rightTop[1];
                m_coordinates[8]  = rightTop[2];
                m_coordinates[9]  = leftTop[0];
                m_coordinates[10] = leftTop[1];
                m_coordinates[11] = leftTop[2];
                
                const int64_t numSlices = 5;
                m_sliceIndices.reserve(numSlices);
                m_sliceOffsets.reserve(numSlices);
            }
            
            /**
             * Add a value from a volume slice.
             *
             * @param sliceIndex
             *    Index of the slice.
             * @param sliceOffset
             *    Offset of value in the slice.
             */
            void addVolumeValue(const int32_t sliceIndex,
                                const int32_t sliceOffset) {
                m_sliceIndices.push_back(sliceIndex);
                m_sliceOffsets.push_back(sliceOffset);
            }
            
            /**
             * Center of voxel.
             */
            float m_center[3];
            
            /**
             * Corners of voxel
             */
            float m_coordinates[12];
            
            /*
             * Index of volume in VoxelsInSliceForVolume
             */
            std::vector<int64_t> m_sliceIndices;
            
            /**
             * Offset in values in VoxelsInSliceForVolume
             */
            std::vector<int64_t> m_sliceOffsets;
        };
        
        BrainOpenGLFPVolumeObliqueDrawing(const BrainOpenGLFPVolumeObliqueDrawing&);

        BrainOpenGLFPVolumeObliqueDrawing& operator=(const BrainOpenGLFPVolumeObliqueDrawing&);
        
        bool getVoxelCoordinateBoundsAndSpacing(const std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                                float boundsOut[6],
                                                float spacingOut[3]);
        
//        void drawAxis(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
//                      const Matrix4x4& rotationMatrix);
        
        void drawSlice(Brain* brain,
                       BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                       BrowserTabContent* browserTabContent,
                       std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                       const VolumeSliceViewPlaneEnum::Enum slicePlane,
                       const DRAW_MODE drawMode);
        
        void drawSliceVoxelsWithTransform(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                          std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          std::vector<int32_t>& identificationIndices,
                                          const PaletteFile* paletteFile,
                                          const int32_t idPerVoxelCount,
                                          const Matrix4x4& transformationMatrix,
                                          const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex,
                                          const float screenBounds[4],
                                          const float sliceNormalVector[3],
                                          const float voxelSize,
                                          const float zoom,
                                          const bool isSelectionMode);
        
        void drawSliceVoxelsModelCoordInterpolation(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                          std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          std::vector<int32_t>& identificationIndices,
                                          const PaletteFile* paletteFile,
                                          const int32_t idPerVoxelCount,
                                          const Matrix4x4& transformationMatrix,
                                          const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex,
                                          const float screenBounds[4],
                                          const float sliceNormalVector[3],
                                          const float voxelSize,
                                                    const float zoom,
                                          const bool isSelectionMode);
        
        void drawSlicesForAllView(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                  BrowserTabContent* browserTabContent,
                                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                  const int viewport[4],
                                  const DRAW_MODE drawMode);
        
        void drawSliceForSliceView(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                   BrowserTabContent* browserTabContent,
                                   std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                                   const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                   const int viewport[4]);
        
        void drawQuads(const std::vector<float>& coordinates,
                       const std::vector<float>& normals,
                       const std::vector<uint8_t>& rgbaColors);
        
        void drawQuadsImmediateMode(const std::vector<float>& coordinates,
                       const std::vector<float>& normals,
                       const std::vector<uint8_t>& rgbaColors);
        
        void drawQuadsVertexArrays(const std::vector<float>& coordinates,
                       const std::vector<float>& normals,
                       const std::vector<uint8_t>& rgbaColors);
        
        void drawLines(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                       const std::vector<float>& coordinates,
                       const std::vector<uint8_t>& rgbaColors,
                       const float thickness);
        
        void drawSurfaces(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                          BrowserTabContent* browserTabContent,
                          const int viewport[4]);
        
        void drawSurfaceOutline(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                BrowserTabContent* browserTabContent,
                                const Plane& plane);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_H__
