#ifndef __BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_H__
#define __BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_H__

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
#include <glm/mat4x4.hpp>

#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLVolumeSliceDrawing.h"
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
    
    class BrainOpenGLVolumeTextureSliceDrawing : public CaretObject {
        
    public:
        BrainOpenGLVolumeTextureSliceDrawing();
        
        virtual ~BrainOpenGLVolumeTextureSliceDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                  const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                  const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum obliqueSliceMaskingType,
                  const int32_t viewport[4]);

        // ADD_NEW_METHODS_HERE

    private:                
        BrainOpenGLVolumeTextureSliceDrawing(const BrainOpenGLVolumeTextureSliceDrawing&);

        BrainOpenGLVolumeTextureSliceDrawing& operator=(const BrainOpenGLVolumeTextureSliceDrawing&);

        void drawPrivate(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                         BrowserTabContent* browserTabContent,
                         std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                         const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                         const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                         const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum obliqueSliceMaskingType,
                         const int32_t viewport[4]);

        void drawVolumeSlicesForAllStructuresView(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                                  const int32_t viewport[4]);
        
        void drawVolumeSliceViewPlane(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                      const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const VolumeSliceViewAllPlanesLayoutEnum::Enum allPlanesLayout,
                                      const int32_t viewport[4]);
        
        void drawVolumeSliceViewType(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                     const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                           const int32_t viewport[4]);
        
        void drawVolumeSliceViewTypeMontage(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                            const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                            const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                  const int32_t viewport[4]);
        
        void drawVolumeSliceViewProjection(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                           const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                 const float sliceCoordinates[3],
                                 const int32_t viewport[4]);
        
        void drawObliqueSlice(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                              Matrix4x4& transformationMatrix,
                              const Plane& plane);
        
        void drawObliqueSliceWithOutlines(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                          Matrix4x4& transformationMatrix);
        
        void createSlicePlaneEquation(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const float sliceCoordinates[3],
                                      Plane& planeOut);
        
        void drawAxesCrosshairsOblique(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
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
        
        void setOrthographicProjection(const BrainOpenGLVolumeSliceDrawing::AllSliceViewMode allSliceViewMode,
                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const int viewport[4]);
        
        bool getVoxelCoordinateBoundsAndSpacing(float boundsOut[6],
                                                float spacingOut[3]);
        
        void createObliqueTransformationMatrix(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                               const float sliceCoordinates[3],
                                               Matrix4x4& obliqueTransformationMatrixOut);
        
        bool getVolumeDrawingViewDependentCulling(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                  const float selectedSliceCoordinate,
                                                  const VolumeMappableInterface* volumeFile,
                                                  int64_t culledFirstVoxelIJKOut[3],
                                                  int64_t culledLastVoxelIJKOut[3],
                                                  float voxelDeltaXYZOut[3]);
        
        glm::mat4 convertMatrix4x4toGlmMat4(const Matrix4x4& matrix) const;
        
        void mat4ToOpenGLMatrix(const glm::mat4& matrixIn,
                                float matrixOut[16]) const;
        
        bool getTextureCoordinates(const VolumeMappableInterface* volumeMappableInterface,
                                   const std::array<float, 3>& xyz,
                                   const std::array<float, 3>& maxStr,
                                   std::array<float, 3>& strOut) const;
        
        bool createVolumeTexture(const VolumeMappableInterface* volumeFile,
                                 const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 const bool allowNonPowerOfTwoTextureFlag,
                                 const bool identificationTextureFlag,
                                 std::vector<uint8_t>& rgbaColorsOut,
                                 std::array<int64_t, 3>& textureDimsOut,
                                 std::array<float, 3>& maxStrOut) const;

        GLuint createTextureName(const VolumeMappableInterface* volumeMappableInterface,
                                 const bool identificationTextureFlag,
                                 const DisplayGroupEnum::Enum displayGroup,
                                 const int32_t tabIndex,
                                 std::array<float, 3>& maxStrOut) const;
        
        void setupTextureFiltering(const CaretMappableDataFile* mapFile,
                                   const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType);
        
        void processTextureVoxelIdentification(VolumeMappableInterface* volumeMappableInterface);
        
        struct TextureInfo {
            GLuint m_textureID;
            std::array<float, 3> m_maxSTR;
        };
        
        /*
         * These items will eventually be moved into the volume and cifti files
         */
        static std::map<VolumeMappableInterface*, TextureInfo> s_volumeTextureInfo;
        static std::map<VolumeMappableInterface*, TextureInfo> s_identificationTextureInfo;
        
        ModelVolume* m_modelVolume;
        
        ModelWholeBrain* m_modelWholeBrain;
        
        ModelTypeEnum::Enum m_modelType;
        
        VolumeMappableInterface* m_underlayVolume;
        
        Brain* m_brain;
        
        std::vector<std::vector<float>> m_ciftiMappableFileData;
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> m_volumeDrawInfo;
        
        BrowserTabContent* m_browserTabContent;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex;
        
        double m_lookAtCenter[3];
        
//        double m_viewingMatrix[16];
        
        double m_orthographicBounds[6];
        
        VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum m_obliqueSliceMaskingType = VolumeSliceInterpolationEdgeEffectsMaskingEnum::OFF;
        
        bool m_identificationModeFlag;
        
        // ADD_NEW_MEMBERS_HERE
    };
    
#ifdef __BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_DECLARE__
    std::map<VolumeMappableInterface*, BrainOpenGLVolumeTextureSliceDrawing::TextureInfo> BrainOpenGLVolumeTextureSliceDrawing::s_volumeTextureInfo;
    std::map<VolumeMappableInterface*, BrainOpenGLVolumeTextureSliceDrawing::TextureInfo> BrainOpenGLVolumeTextureSliceDrawing::s_identificationTextureInfo;

#endif // __BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_GL_VOLUME_TEXTURE_SLICE_DRAWING_H__
