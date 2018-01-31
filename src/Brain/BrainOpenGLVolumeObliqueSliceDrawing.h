#ifndef __BRAIN_OPEN_GL_VOLUME_OBLIQUE_SLICE_DRAWING_H__
#define __BRAIN_OPEN_GL_VOLUME_OBLIQUE_SLICE_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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
#include "DisplayGroupEnum.h"
#include "ModelTypeEnum.h"
#include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceDrawingTypeEnum.h"
#include "VolumeSliceViewAllPlanesLayoutEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class Brain;
    class BrowserTabContent;
    class CiftiMappableDataFile;
    class Matrix4x4;
    class ModelVolume;
    class ModelWholeBrain;
    class PaletteFile;
    class Plane;
    class VolumeMappableInterface;
    
    class BrainOpenGLVolumeObliqueSliceDrawing : public CaretObject {
        
    public:
        BrainOpenGLVolumeObliqueSliceDrawing();
        
        virtual ~BrainOpenGLVolumeObliqueSliceDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                  const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                  const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum obliqueSliceMaskingType,
                  const int32_t viewport[4]);

        // ADD_NEW_METHODS_HERE

    private:
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
                        const int32_t mapIndex,
                        const float opacity);
            
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
             * Add values for RGBA and returns its index.
             *
             * @param value
             *     Value that is added.
             * @return
             *     The index for the value.
             */
            int64_t addValuesRGBA(const float values[4]);
            
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
             * Opacity
             */
            float m_opacity;
            
            /**
             * The voxel values for single scalar or red if RGBA volume
             */
            std::vector<float> m_values;
            
            /** Voxel values for green in RGBA */
            std::vector<float> m_valuesGreen;
            
            /** Voxel values for blue in RGBA */
            std::vector<float> m_valuesBlue;
            
            /** Voxel values for alpha in RGBA */
            std::vector<float> m_valuesAlpha;
            
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
            VoxelToDraw(const float center[3],
                        const double leftBottom[3],
                        const double rightBottom[3],
                        const double rightTop[3],
                        const double leftTop[3]);
            
            void getDiffXYZ(float dxyzOut[3]) const;
            
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
        
        BrainOpenGLVolumeObliqueSliceDrawing(const BrainOpenGLVolumeObliqueSliceDrawing&);

        BrainOpenGLVolumeObliqueSliceDrawing& operator=(const BrainOpenGLVolumeObliqueSliceDrawing&);
        
        void drawVolumeSlicesForAllStructuresView(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                  const int32_t viewport[4]);
        
        void drawVolumeSliceViewPlane(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                      const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                      const int32_t viewport[4]);
        
        void drawVolumeSliceViewType(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                           const int32_t viewport[4]);
        
        void drawVolumeSliceViewTypeMontage(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                            const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                  const int32_t viewport[4]);
        
        void drawVolumeSliceViewProjection(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                 const float sliceCoordinates[3],
                                 const int32_t viewport[4]);
        
        void drawObliqueSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                              Matrix4x4& transformationMatrix,
                              const Plane& plane);
        
        void drawOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                 const float sliceCoordinates[3],
                                 const Plane& plane);
        
        void drawOrthogonalSliceWithCulling(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const float sliceCoordinates[3],
                                            const Plane& plane);
        
        void createSlicePlaneEquation(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const float sliceCoordinates[3],
                                      Plane& planeOut);
        
        void drawAxesCrosshairsOrthoAndOblique(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                               const float sliceCoordinates[3],
                                               const bool drawCrosshairsFlag,
                                               const bool drawCrosshairLabelsFlag);
        
        void setVolumeSliceViewingAndModelingTransformations(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                             const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                             const Plane& plane,
                                                             const float sliceCoordinates[3]);
        
        void getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                          float rgbaOut[4]) const;
        
        void drawLayers(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                        const Plane& slicePlane,
                        const float sliceCoordinates[3]);
        
//        void drawSurfaceOutline(const Plane& plane);
        
        void drawVolumeSliceFoci(const Plane& plane);
        
        void drawAxesCrosshairs(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                const float sliceCoordinates[3]);
        
        bool getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                   float& minSpacingOut,
                                   float& maxSpacingOut) const;
        
        void drawSquare(const float size);
        
        void drawOrientationAxes(const int viewport[4]);
        
        void setOrthographicProjection(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const int viewport[4]);
        
        void drawOrthogonalSliceVoxels(const float sliceNormalVector[3],
                                       const float coordinate[3],
                                       const float rowStep[3],
                                       const float columnStep[3],
                                       const int64_t numberOfColumns,
                                       const int64_t numberOfRows,
                                       const std::vector<uint8_t>& sliceRGBA,
                                       const int64_t validVoxelCount,
                                       const VolumeMappableInterface* volumeInterface,
                                       const int32_t volumeIndex,
                                       const int32_t mapIndex,
                                       const uint8_t sliceOpacity);
        
        void drawOrthogonalSliceVoxelsSingleQuads(const float sliceNormalVector[3],
                                       const float coordinate[3],
                                       const float rowStep[3],
                                       const float columnStep[3],
                                       const int64_t numberOfColumns,
                                       const int64_t numberOfRows,
                                       const std::vector<uint8_t>& sliceRGBA,
                                       const VolumeMappableInterface* volumeInterface,
                                       const int32_t volumeIndex,
                                       const int32_t mapIndex,
                                       const uint8_t sliceOpacity);
        
        void drawOrthogonalSliceVoxelsQuadIndicesAndStrips(const float sliceNormalVector[3],
                                                           const float coordinate[3],
                                                           const float rowStep[3],
                                                           const float columnStep[3],
                                                           const int64_t numberOfColumns,
                                                           const int64_t numberOfRows,
                                                           const std::vector<uint8_t>& sliceRGBA,
                                                           const VolumeMappableInterface* volumeInterface,
                                                           const int32_t volumeIndex,
                                                           const int32_t mapIndex,
                                                           const uint8_t sliceOpacity);
        
        bool getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                float spacingOut[3]);
        
        void createObliqueTransformationMatrix(const float sliceCoordinates[3],
                                               Matrix4x4& obliqueTransformationMatrixOut);
        
        void addVoxelToIdentification(const int32_t volumeIndex,
                                      const int32_t mapIndex,
                                      const int32_t voxelI,
                                      const int32_t voxelJ,
                                      const int32_t voxelK,
                                      const float voxelDiffXYZ[3],
                                      uint8_t rgbaForColorIdentificationOut[4]);
        
        bool getVolumeDrawingViewDependentCulling(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                  const float selectedSliceCoordinate,
                                                  const VolumeMappableInterface* volumeFile,
                                                  int64_t culledFirstVoxelIJKOut[3],
                                                  int64_t culledLastVoxelIJKOut[3],
                                                  float voxelDeltaXYZOut[3]);
        
        void processIdentification();
        
        void resetIdentification();
        
        ModelVolume* m_modelVolume;
        
        ModelWholeBrain* m_modelWholeBrain;
        
        ModelTypeEnum::Enum m_modelType;
        
        VolumeMappableInterface* m_underlayVolume;
        
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
        
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum m_obliqueSliceMaskingType = VolumeSliceInterpolationEdgeEffectsMaskingEnum::OFF;
        
        std::vector<int32_t> m_identificationIndices;
        
        bool m_identificationModeFlag;
        
        static const int32_t IDENTIFICATION_INDICES_PER_VOXEL;
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __BRAIN_OPEN_GL_VOLUME_OBLIQUE_SLICE_DRAWING_DECLARE__
    const int32_t BrainOpenGLVolumeObliqueSliceDrawing::IDENTIFICATION_INDICES_PER_VOXEL = 8;
#endif // __BRAIN_OPEN_GL_VOLUME_OBLIQUE_SLICE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_GL_VOLUME_OBLIQUE_SLICE_DRAWING_H__
