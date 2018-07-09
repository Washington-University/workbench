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

#include <array>

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
        class ObliqueSlice {
        public:
            enum class DataValueType {
                INVALID,
                CIFTI_LABEL,
                CIFTI_PALETTE,
                VOLUME_LABEL,
                VOLUME_PALETTE,
                VOLUME_RGB,
                VOLUME_RGBA
            };
            
            ObliqueSlice(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                         VolumeMappableInterface* volumeInterface,
                         const float opacity,
                         const int32_t mapIndex,
                         const int32_t numberOfRows,
                         const int32_t numberOfColumns,
                         const int32_t browserTabIndex,
                         const DisplayPropertiesLabels* displayPropertiesLabels,
                         const DisplayGroupEnum::Enum displayGroup,
                         const float originXYZ[3],
                         const float leftToRightStepXYZ[3],
                         const float bottomToTopStepXYZ[3],
                         const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType,
                         const float voxelEditingValue,
                         const bool volumeEditingDrawAllVoxelsFlag,
                         const bool    identificationModeFlag);
            
            void assignRgba(const bool volumeEditingDrawAllVoxelsFlag);
            
            void addOutlines();
            
            void loadData(const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType,
                          const float voxelEditingValue,
                          const bool volumeEditingDrawAllVoxelsFlag);
            
            bool setThresholdFileAndMap();
            
            bool getSelectionIJK(int32_t ijkOut[3]) const;
            
            void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing);
            
            bool spatialMatch(const ObliqueSlice* slice);
            
            bool compositeSlicesRGBA(const std::vector<ObliqueSlice*>& slices);
            
            VolumeMappableInterface* m_volumeInterface;
            
            const float m_opacity;
            
            const CaretMappableDataFile* m_mapFile = NULL;
            
            const int32_t m_mapIndex;
            
            const int32_t m_numberOfRows;
            
            const int32_t m_numberOfColumns;
            
            const int32_t m_browserTabIndex;
            
            const DisplayPropertiesLabels* m_displayPropertiesLabels;
            
            const DisplayGroupEnum::Enum m_displayGroup;
            
            const int32_t m_identificationX;
            
            const int32_t m_identificationY;
            
            const bool m_identificationModeFlag;
            
            CiftiMappableDataFile* m_ciftiMappableFile = NULL;
            
            VolumeFile* m_volumeFile = NULL;
            
            uint8_t m_opacityByte = 255;
            
            CiftiMappableDataFile* m_thresholdCiftiMappableFile = NULL;
            
            VolumeFile* m_thresholdVolumeFile = NULL;
            
            PaletteColorMapping* m_thresholdPaletteColorMapping = NULL;
            
            int32_t m_thresholdMapIndex = -1;
            
            float m_originXYZ[3];
            
            float m_leftToRightStepXYZ[3];
            
            float m_bottomToTopStepXYZ[3];
        
            DataValueType m_dataValueType = DataValueType::INVALID;
            
            std::vector<float> m_data;
        
            std::vector<float> m_thresholdData;
            
            std::vector<uint8_t> m_rgba;
            
            std::vector<int64_t> m_identificationIJK;
            
            int32_t m_validVoxelCount = 0;
            
            int32_t m_sliceNumberOfVoxels = 0;
            
            int32_t m_voxelNumberOfComponents = 1;
            
            std::unique_ptr<IdentificationWithColor> m_identificationHelper;
            
            int64_t m_selectionIJK[3];
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
        
        void drawObliqueSliceWithOutlines(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          Matrix4x4& transformationMatrix);
        
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
