#ifndef __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_H__
#define __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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


#include <map>
#include <memory>

#include "BrainOpenGLFixedPipeline.h"
#include "CaretObject.h"
#include "Plane.h"
#include "SelectionItemVolumeMprCrosshair.h"
#include "Vector3D.h"
#include "VolumeSliceDrawingTypeEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class BrowserTabContent;
    class BrainOpenGLFixedPipeline;
    class GraphicsPrimitiveV3f;
    class GraphicsPrimitiveV3fC4ub;
    class GraphicsPrimitiveV3fT2f;
    class GraphicsPrimitiveV3fT3f;
    class GraphicsViewport;
    class Matrix4x4;
    class ModelVolume;
    
    class BrainOpenGLVolumeMprThreeDrawing : public CaretObject {
        
    public:
        BrainOpenGLVolumeMprThreeDrawing();
        
        virtual ~BrainOpenGLVolumeMprThreeDrawing();
        
        BrainOpenGLVolumeMprThreeDrawing(const BrainOpenGLVolumeMprThreeDrawing&) = delete;

        BrainOpenGLVolumeMprThreeDrawing& operator=(const BrainOpenGLVolumeMprThreeDrawing&) = delete;
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const GraphicsViewport& viewport);

        static void deleteStaticMembers();
        
        // ADD_NEW_METHODS_HERE
        
    private:
//        class Float3 : public std::array<float, 3> {
//        public:
//            Float3() {
//                this->fill(0.0);
//            }
//        };
        
        void drawVolumeSliceViewType(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                     const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                     const GraphicsViewport& viewport);
        
        void drawVolumeSliceViewProjection(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const std::array<float, 3>& sliceCoordinates,
                                           const GraphicsViewport& viewport);
        
        void setOrthographicProjection(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const GraphicsViewport& viewport);
        
        Plane createSlicePlaneEquation(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                       const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                       const std::array<float, 3>& sliceCoordinates);

        void setViewingTransformation(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const Plane& plane);

        void drawSliceWithPrimitive(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                    const std::array<float, 3>& sliceCoordinates,
                                    const GraphicsViewport& viewport);
        
        bool getTextureCoordinates(const VolumeMappableInterface* volumeMappableInterface,
                                   const std::array<float, 3>& xyz,
                                   const std::array<float, 3>& maxStr,
                                   std::array<float, 3>& strOut) const;
        
        void performVoxelIdentification(const Matrix4x4& viewRotationMatrix,
                                        VolumeMappableInterface* volumeInterface,
                                        const GraphicsPrimitiveV3fT3f* primitive,
                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                        const GraphicsViewport& viewport,
                                        const std::array<float, 3> bottomLeft,
                                        const std::array<float, 3> bottomRight,
                                        const std::array<float, 3> topRight,
                                        const std::array<float, 3> topLeft,
                                        const float mouseX,
                                        const float mouseY);
        
        void performImageIdentification(const Matrix4x4& viewRotationMatrix,
                                        VolumeMappableInterface* volumeInterface,
                                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                        const GraphicsViewport& viewport,
                                        const std::array<float, 3>& bottomLeft,
                                        const std::array<float, 3>& bottomRight,
                                        const std::array<float, 3>& topRight,
                                        const std::array<float, 3>& topLeft,
                                        const float mouseX,
                                        const float mouseY);

        void getMouseViewportXY(const GraphicsViewport& viewport,
                                const float mouseX,
                                const float mouseY,
                                float& outViewportMouseX,
                                float& outViewportMouseY) const;
        
        void getMouseViewportNormalizedXY(const GraphicsViewport& viewport,
                                          const float mouseX,
                                          const float mouseY,
                                          float& outViewportNormalizedMouseX,
                                          float& outViewportNormalizedMouseY) const;
        
        void addCrosshairSection(GraphicsPrimitiveV3fC4ub* primitiveSliceCrosshair,
                                 GraphicsPrimitiveV3fC4ub* primitiveRotateCrosshair,
                                 const float xStart,
                                 const float yStart,
                                 const float xEnd,
                                 const float yEnd,
                                 const std::array<uint8_t, 4>& rgba,
                                 const float gapLengthPixels,
                                 std::vector<SelectionItemVolumeMprCrosshair::Axis>& sliceSelectionIndices,
                                 std::vector<SelectionItemVolumeMprCrosshair::Axis>& rotateSelectionIndices,
                                 const SelectionItemVolumeMprCrosshair::Axis sliceAxisID,
                                 const SelectionItemVolumeMprCrosshair::Axis rotationAxisID);

        void drawCrosshairs(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                            const std::array<float, 3>& sliceCoordinates,
                            const GraphicsViewport& viewport);
        
        void drawPanningCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                   const std::array<float, 3>& crossHairXYZ,
                                   const GraphicsViewport& viewport);
        
        void drawRotationCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                    const std::array<float, 3>& crossHairXYZ,
                                    const GraphicsViewport& viewport);
        
        void drawAxisLabels(const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                            const GraphicsViewport& viewport);

        std::array<uint8_t, 4> getAxisColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const;
        
        void getViewportModelCoordinates(const GraphicsViewport& viewport,
                                         const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                         std::array<float, 3>& bottomLeftOut,
                                         std::array<float, 3>& bottomRightOut,
                                         std::array<float, 3>& topRightOut,
                                         std::array<float, 3>& topLeftOut) const;

        Matrix4x4 createViewTransformationMatrix(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const std::array<float, 3>& sliceCoordinates);

        static GraphicsPrimitiveV3fT2f* getIdentificationPrimitive(const std::array<float, 3>& bottomLeft,
                                                                   const std::array<float, 3>& bottomRight,
                                                                   const std::array<float, 3>& topRight,
                                                                   const std::array<float, 3>& topLeft);

        void drawLayers(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                        const Plane& slicePlane,
                        const std::array<float, 3>& sliceCoordinates);
        
        void drawVolumeSliceViewTypeMontage(const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                            const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const GraphicsViewport& viewport);

        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing = NULL;

        BrowserTabContent* m_browserTabContent = NULL;
        
        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> m_volumeDrawInfo;
        
        Brain* m_brain = NULL;
        
        ModelVolume* m_modelVolume = NULL;
        
        VolumeMappableInterface* m_underlayVolume = NULL;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex = -1;
        
        std::array<double, 6> m_orthographicBounds;
        
        std::array<double, 3> m_lookAtCenterXYZ;
        
        bool m_identificationModeFlag = false;
        
        bool m_allSliceViewFlag = false;
        
        static std::unique_ptr<GraphicsPrimitiveV3fT2f> s_identificationPrimitive;
        
        static std::map<std::array<uint8_t, 3>, std::array<int32_t, 2>> s_idRgbToIJ;
        
        static float s_idNumRows;
        
        static float s_idNumCols;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_DECLARE__
    std::unique_ptr<GraphicsPrimitiveV3fT2f> BrainOpenGLVolumeMprThreeDrawing::s_identificationPrimitive;
    
    std::map<std::array<uint8_t, 3>, std::array<int32_t, 2>> BrainOpenGLVolumeMprThreeDrawing::s_idRgbToIJ;
    
    float BrainOpenGLVolumeMprThreeDrawing::s_idNumRows = 0.0;
    
    float BrainOpenGLVolumeMprThreeDrawing::s_idNumCols = 0.0;
#endif // __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_H__
