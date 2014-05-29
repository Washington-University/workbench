#ifndef __OLD_BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_H__
#define __OLD_BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_H__

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


#include "BrainOpenGLFixedPipeline.h"
#include "CaretObject.h"
#include "VolumeSliceViewModeEnum.h"


namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrowserTabContent;
    class CiftiMappableDataFile;
    class Matrix4x4;
    
    class OLD_BrainOpenGLVolumeSliceDrawing : public CaretObject {
        
    public:
        OLD_BrainOpenGLVolumeSliceDrawing();
        
        virtual ~OLD_BrainOpenGLVolumeSliceDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const VolumeSliceViewModeEnum::Enum sliceViewMode,
                  const int viewport[4]);
    protected:
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
        
        OLD_BrainOpenGLVolumeSliceDrawing(const OLD_BrainOpenGLVolumeSliceDrawing&);

        OLD_BrainOpenGLVolumeSliceDrawing& operator=(const OLD_BrainOpenGLVolumeSliceDrawing&);
        
    private:
        bool getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                float spacingOut[3]);
        
        bool getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                   float& minSpacingOut,
                                   float& maxSpacingOut) const;
        
        void drawOrthogonalSlice(const DRAW_MODE drawMode,
                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                 const Plane& plane,
                                 const int32_t montageSliceIndex);
        
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
        
        void createObliqueTransformationMatrix(Matrix4x4& obliqueTransformationMatrixOut);
        
        void createSlicePlaneEquation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const int32_t montageSliceIndex,
                                      Plane& planeOut);
        
        void setVolumeSliceViewingAndModelingTransformations(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const Plane& plane);
        
        void drawDebugSquare();
        
        void drawLayers(const Plane& slicePlane,
                        const VolumeMappableInterface* volume,
                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                        const DRAW_MODE drawMode);
        
        void drawAxesCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane);
        
        void getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                          float rgbaOut[4]) const;
        
        void getAxesTextLabelsXYZ(const float axesStartXYZ[3],
                                  const float axesEndXYZ[3],
                                  float axesTextStartXYZ[3],
                                  float axesTextEndXYZ[3]) const;
        
        void addVoxelToIdentification(const int32_t volumeIndex,
                                      const int32_t mapIndex,
                                      const int32_t voxelI,
                                      const int32_t voxelJ,
                                      const int32_t voxelK,
                                      uint8_t rgbaForColorIdentificationOut[4]);
        
        void processIdentification();
        
        void resetIdentification();
        
        void drawAxesCrosshairsOrthoAndOblique(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                               const VolumeSliceViewModeEnum::Enum sliceViewMode,
                                               const bool drawCrosshairsFlag,
                                               const bool drawCrosshairLabelsFlag);
        
        void drawOrientationAxes(const int viewport[4],
                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane);
        
        void setOrthographicProjection(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const int viewport[4]);
        
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
    
#ifdef __OLD_BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__
    const int32_t OLD_BrainOpenGLVolumeSliceDrawing::IDENTIFICATION_INDICES_PER_VOXEL = 5;
#endif // __OLD_BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__

} // namespace
#endif  //__OLD_BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_H__
