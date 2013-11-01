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
#include "VolumeSliceViewModeEnum.h"


namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrowserTabContent;
    class CiftiMappableDataFile;
    class Matrix4x4;
    
    class BrainOpenGLVolumeSliceDrawing : public CaretObject {
        
    public:
        BrainOpenGLVolumeSliceDrawing();
        
        virtual ~BrainOpenGLVolumeSliceDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const VolumeSliceViewModeEnum::Enum sliceViewMode,
                  const int viewport[4]);
    private:
        enum DRAW_MODE {
            DRAW_MODE_ALL_STRUCTURES_VIEW,
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
        
        BrainOpenGLVolumeSliceDrawing(const BrainOpenGLVolumeSliceDrawing&);

        BrainOpenGLVolumeSliceDrawing& operator=(const BrainOpenGLVolumeSliceDrawing&);
        
        bool getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                float spacingOut[3]);
        
        bool getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                   float& minSpacingOut,
                                   float& maxSpacingOut) const;
        
        void drawOrthogonalSlice(const DRAW_MODE drawMode,
                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                 const Plane& plane,
                                 const int32_t montageSliceIndex);
        
        void drawOrthogonalSliceVoxels(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const float sliceNormalVector[3],
                                       const int64_t selectedSliceIndices[3],
                                       const float coordinate[3],
                                       const float rowStep[3],
                                       const float columnStep[3],
                                       const int64_t numberOfColumns,
                                       const int64_t numberOfRows,
                                       const std::vector<uint8_t>& sliceRGBA,
                                       const int32_t volumeIndex,
                                       const int32_t mapIndex,
                                       const uint8_t sliceOpacity);
        
        void drawObliqueSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                              const Plane& plane,
                              const DRAW_MODE drawMode,
                             const Matrix4x4& transformationMatrix,
                             const float zoom);
        
        void drawSlicesForAllStructuresView(const int viewport[4]);
        
        void drawAllThreeSlicesForVolumeSliceView(const int viewport[4]);
        
        void drawSliceForSliceView(const VolumeSliceViewPlaneEnum::Enum slicePlane,
                                      const DRAW_MODE drawMode,
                                      const int32_t montageSliceIndex,
                                      const int viewport[4]);
        void drawSliceMontage(const int viewport[4]);
        
        void drawSquare(const float size);
        
        void drawSurfaceOutline(const Plane& plane);
        
        void drawVolumeSliceFoci(const Plane& plane);
        
        void setOrthographicProjection(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const int viewport[4]);
        
        void createObliqueTransformationMatrix(Matrix4x4& obliqueTransformationMatrixOut);
        
        void createSlicePlaneEquation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const int32_t montageSliceIndex,
                                      Plane& planeOut);
        
        void setVolumeSliceViewingAndModelingTransformations(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const Plane& plane);
        
//        void setVolumeSliceModelingTransformation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
//                                                  const DRAW_MODE drawMode,
//                                                  Plane& planeOut,
//                                                  const int32_t montageSliceIndex,
//                                                  Matrix4x4& obliqueTransformationMatrixOut);
        
        void drawDebugSquare();
        
        void drawLayers(const Plane& slicePlane,
                        const VolumeMappableInterface* volume,
                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                        const DRAW_MODE drawMode);
        
        void drawAxesCrosshairs(const VolumeMappableInterface* volume,
                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane);
        
//        void drawAxesCrosshairsOblique(const Plane& slicePlane,
//                                       const Matrix4x4& transformationMatrix,
//                                const VolumeMappableInterface* volume,
//                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
//                                const DRAW_MODE drawMode);
        
//        void drawAxesCrosshairsOrthogonal(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
//                                          const VolumeMappableInterface* volume,
//                                          const bool drawCrosshairsFlag,
//                                          const bool drawCrosshairLabelsFlag);
        
        void drawAxesCrosshairsOrthoAndOblique(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          const VolumeSliceViewModeEnum::Enum sliceViewMode,
                                          const VolumeMappableInterface* volume,
                                          const bool drawCrosshairsFlag,
                                          const bool drawCrosshairLabelsFlag);
        
        void getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                          float rgbaOut[4]) const;
        
        void getAxesTextLabelsXYZ(const float axesStartXYZ[3],
                                  const float axesEndXYZ[3],
                                  float axesTextStartXYZ[3],
                                  float axesTextEndXYZ[3]) const;
        
        void resetIdentification();
        
        void addVoxelToIdentification(const int32_t volumeIndex,
                                      const int32_t mapIndex,
                                      const int32_t voxelI,
                                      const int32_t voxelJ,
                                      const int32_t voxelK,
                                      uint8_t rgbaForColorIdentificationOut[4]);
        
        void drawOrientationAxes(const int viewport[4],
                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane);
        
        void processIdentification();
        
        // ADD_NEW_MEMBERS_HERE

        Brain* m_brain;
        
        std::vector<std::vector<float> > m_ciftiMappableFileData;

        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> m_volumeDrawInfo;
        
        BrowserTabContent* m_browserTabContent;
        
        PaletteFile* m_paletteFile;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        double m_lookAtCenter[3];
        
        double m_viewingMatrix[16];
        
        double m_orthographicBounds[6];
        
        VolumeSliceViewModeEnum::Enum m_sliceViewMode;
        
        std::vector<int32_t> m_identificationIndices;
        
        bool m_identificationModeFlag;
        
        static const int32_t IDENTIFICATION_INDICES_PER_VOXEL;
    };
    
#ifdef __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__
    const int32_t BrainOpenGLVolumeSliceDrawing::IDENTIFICATION_INDICES_PER_VOXEL = 5;
#endif // __BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_F_P_VOLUME_OBLIQUE_DRAWING_H__
