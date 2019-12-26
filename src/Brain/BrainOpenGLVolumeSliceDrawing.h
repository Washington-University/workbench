#ifndef __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_H__
#define __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_H__

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
    class Plane;
    class VolumeMappableInterface;
    class VolumeSurfaceOutlineSetModel;
    
    class BrainOpenGLVolumeSliceDrawing : public CaretObject {
        
    public:
        /**
         * Indicates drawing volume sclice "ALL that shows
         * axial, coronal, and parasagittal at same time
         */
        enum class AllSliceViewMode {
            ALL_YES,
            ALL_NO
        };
        
        BrainOpenGLVolumeSliceDrawing();
        
        virtual ~BrainOpenGLVolumeSliceDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                  const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                  const int32_t viewport[4]);
        
        static void setOrthographicProjection(const AllSliceViewMode allSliceViewMode,
                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                              const BoundingBox& boundingBox,
                                              const float zoomFactor,
                                              const int viewport[4],
                                              double orthographicBoundsOut[6]);
        
        static void drawSurfaceOutline(const VolumeMappableInterface* underlayVolume,
                                       const ModelTypeEnum::Enum modelType,
                                       const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const float sliceXYZ[3],
                                       const Plane& plane,
                                       VolumeSurfaceOutlineSetModel* outlineSet,
                                       BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                       const bool useNegativePolygonOffsetFlag);
        
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
        
        BrainOpenGLVolumeSliceDrawing(const BrainOpenGLVolumeSliceDrawing&);
        
        BrainOpenGLVolumeSliceDrawing& operator=(const BrainOpenGLVolumeSliceDrawing&);
        
        void drawVolumeSlicesForAllStructuresView(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                  const int32_t viewport[4]);
        
        void drawVolumeSliceViewPlane(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                      const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                      const int32_t viewport[4]);
        
        void drawVolumeSliceViewType(const AllSliceViewMode allSliceViewMode,
                                     const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                     const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                     const int32_t viewport[4]);
        
        void drawVolumeSliceViewTypeMontage(const AllSliceViewMode allSliceViewMode,
                                            const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                            const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const int32_t viewport[4]);
        
        void drawVolumeSliceViewProjection(const AllSliceViewMode allSliceViewMode,
                                           const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const float sliceCoordinates[3],
                                           const int32_t viewport[4]);
        
        void drawOrthogonalSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                 const float sliceCoordinates[3],
                                 const Plane& plane);
        
        void drawOrthogonalSliceAllView(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                        const float sliceCoordinates[3],
                                        const Plane& plane);
        
        void drawOrthogonalSliceWithCulling(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const float sliceCoordinates[3],
                                            const Plane& plane);
        
        void createSlicePlaneEquation(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const float sliceCoordinates[3],
                                      Plane& planeOut);
        
        void drawAxesCrosshairsOrtho(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                     const float sliceCoordinates[3],
                                     const bool drawCrosshairsFlag,
                                     const bool drawCrosshairLabelsFlag);

        void setVolumeSliceViewingAndModelingTransformations(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                             const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                             const Plane& plane);
        
        void getAxesColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                          float rgbaOut[4]) const;
        
        void drawLayers(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                        const Plane& slicePlane,
                        const float sliceCoordinates[3]);
        
        static void drawSurfaceOutlineCached(const VolumeMappableInterface* underlayVolume,
                                             const ModelTypeEnum::Enum modelType,
                                             const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                             const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                             const float sliceXYZ[3],
                                             const Plane& plane,
                                             VolumeSurfaceOutlineSetModel* outlineSet,
                                             BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                             const bool useNegativePolygonOffsetFlag);

        static void drawSurfaceOutlineNotCached(const ModelTypeEnum::Enum modelType,
                                                const Plane& plane,
                                                VolumeSurfaceOutlineSetModel* outlineSet,
                                                BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                const bool useNegativePolygonOffsetFlag);

        void drawVolumeSliceFoci(const Plane& plane);
        
        void drawAxesCrosshairs(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                const float sliceCoordinates[3]);
        
        bool getMinMaxVoxelSpacing(const VolumeMappableInterface* volume,
                                   float& minSpacingOut,
                                   float& maxSpacingOut) const;
        
        void drawSquare(const float size);
        
        void setOrthographicProjection(const AllSliceViewMode allSliceViewMode,
                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
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
        
        void drawIdentificationSymbols(const Plane& plane);
        
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
        
        void showBrainordinateHighlightRegionOfInterest(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                        const float sliceCoordinates[3],
                                                        const float sliceNormalVector[3]);
        
        void processIdentification(const bool doNotReplaceUnderlayFlag);
        
        void resetIdentification();
        
        ModelTypeEnum::Enum m_modelType;
        
        ModelVolume* m_modelVolume;
        
        ModelWholeBrain* m_modelWholeBrain;
        
        VolumeMappableInterface* m_underlayVolume;
        
        Brain* m_brain;
        
        std::vector<std::vector<float> > m_ciftiMappableFileData;
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> m_volumeDrawInfo;
        
        BrowserTabContent* m_browserTabContent;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        double m_lookAtCenter[3];
        
        double m_viewingMatrix[16];
        
        double m_orthographicBounds[6];
        
        std::vector<int32_t> m_identificationIndices;
        
        bool m_identificationModeFlag;
        
        static const int32_t IDENTIFICATION_INDICES_PER_VOXEL;
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__
    const int32_t BrainOpenGLVolumeSliceDrawing::IDENTIFICATION_INDICES_PER_VOXEL = 8;
#endif // __BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_DECLARE__
    
} // namespace
#endif  //__BRAIN_OPEN_GL_VOLUME_SLICE_DRAWING_H__
