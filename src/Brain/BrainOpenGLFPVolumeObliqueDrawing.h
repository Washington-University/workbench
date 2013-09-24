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
    class CiftiMappableDataFile;
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
                        const int32_t mapIndex);
            
            /**
             * Add a value and return its index.
             *
             * @param value
             *     Value that is added.
             * @return
             *     The index for the value.
             */
            int64_t addValue(const float value);
            
            /**
             * Return RGBA colors for value using the value's index
             * returned by addValue().
             *
             * @param indx
             *    Index of the value.
             * @return
             *    RGBA coloring for value.
             */
            uint8_t* getRgbaForValueByIndex(const int64_t indx);
            
            /**
             * Allocate colors for the voxel values
             */
            void allocateColors();
            
            /**
             * Volume containing the values
             */
            VolumeMappableInterface* m_volumeMappableInterface;
            
            /**
             * If not NULL, it is a VolumeFile
             */
            VolumeFile* m_volumeFile;
            
            /**
             * If not NULL, it is a Cifti Mappable Data File
             */
            CiftiMappableDataFile* m_ciftiMappableDataFile;
            
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
                        const double leftTop[3]);
            /**
             * Add a value from a volume slice.
             *
             * @param sliceIndex
             *    Index of the slice.
             * @param sliceOffset
             *    Offset of value in the slice.
             */
            void addVolumeValue(const int64_t sliceIndex,
                                const int64_t sliceOffset);
            
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
        
        bool getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                float spacingOut[3]);
        
//        void drawAxis(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
//                      const Matrix4x4& rotationMatrix);
        
        void drawSlice(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                       const DRAW_MODE drawMode);
        
        void drawSliceVoxels(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          std::vector<int32_t>& identificationIndices,
                                          const int32_t idPerVoxelCount,
                                          const Matrix4x4& transformationMatrix,
                                          const float screenBounds[4],
                                          const float sliceNormalVector[3],
                                          const float voxelSize,
                                                    const float zoom,
                                          const bool isSelectionMode);
        
        void drawSlicesForAllSlicesView(const int viewport[4],
                                  const DRAW_MODE drawMode);
        
        void drawSliceForSliceView(const VolumeSliceViewPlaneEnum::Enum slicePlane,
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
        
        void drawQuadsVertexBuffers(const std::vector<float>& coordinates,
                                    const std::vector<float>& normals,
                                    const std::vector<uint8_t>& rgbaColors);
        
        void drawLines(const std::vector<float>& coordinates,
                       const std::vector<uint8_t>& rgbaColors,
                       const float thickness);
        
        void drawSquare(const float size);
        
        void drawSurfaces(const int viewport[4]);
        
        void drawSurfaceOutline(const Plane& plane);
        
        void drawVolumeSliceFoci(const Plane& plane);
        
        void setOrthographicBounds(const DRAW_MODE drawMode);

        // ADD_NEW_MEMBERS_HERE

        Brain* m_brain;
        
        std::vector<std::vector<float> > m_ciftiMappableFileData;

        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> m_volumeDrawInfo;
        
        BrowserTabContent* m_browserTabContent;
        
        PaletteFile* m_paletteFile;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        double m_orthographicBounds[6];
        
    };
    
#ifdef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_H__
