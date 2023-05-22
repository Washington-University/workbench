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
#include "MprVirtualSliceView.h"
#include "Plane.h"
#include "SelectionItemVolumeMprCrosshair.h"
#include "Vector3D.h"
#include "VolumeMprViewModeEnum.h"
#include "VolumeMprOrientationModeEnum.h"
#include "VolumeSliceDrawingTypeEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class Brain;
    class BrowserTabContent;
    class BrainOpenGLFixedPipeline;
    class BrainOpenGLViewportContent;
    class GraphicsPrimitive;
    class GraphicsPrimitiveV3f;
    class GraphicsPrimitiveV3fC4ub;
    class GraphicsPrimitiveV3fT2f;
    class GraphicsPrimitiveV3fT3f;
    class GraphicsViewport;
    class Matrix4x4;
    class ModelVolume;
    
    class BrainOpenGLVolumeMprThreeDrawing : public CaretObject {
        
    public:
        class SliceInfo {
        public:
            SliceInfo() { }
            
            MprVirtualSliceView m_mprSliceView;
            
            AString toString(const AString& indentation = "") const;
        };
        
        BrainOpenGLVolumeMprThreeDrawing();
        
        virtual ~BrainOpenGLVolumeMprThreeDrawing();
        
        BrainOpenGLVolumeMprThreeDrawing(const BrainOpenGLVolumeMprThreeDrawing&) = delete;

        BrainOpenGLVolumeMprThreeDrawing& operator=(const BrainOpenGLVolumeMprThreeDrawing&) = delete;
        
        static void getOrthographicProjection(const BoundingBox& boundingBox,
                                              const float zoomFactor,
                                              const GraphicsViewport& viewport,
                                              double orthographicBoundsOut[6]);

        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  const BrainOpenGLViewportContent* viewportContent,
                  BrowserTabContent* browserTabContent,
                  std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo>& volumeDrawInfo,
                  const GraphicsViewport& viewport);

        // ADD_NEW_METHODS_HERE
        
    private:
        enum class BrainModelMode {
            INVALID,
            ALL_3D,
            VOLUME_2D
        };
        
        SliceInfo createSliceInfo(const BrowserTabContent* browserTabContent,
                                  const VolumeMappableInterface* underlayVolume,
                                  const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                  const Vector3D& sliceCoordinates) const;
        
        SliceInfo createSliceInfo3D() const;
        
        void drawSliceView(const BrainOpenGLViewportContent* viewportContent,
                           BrowserTabContent* browserTabContent,
                           const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                           const GraphicsViewport& viewport);

        void drawWholeBrainView(const BrainOpenGLViewportContent* viewportContent,
                                const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                const GraphicsViewport& viewport);

        void drawVolumeSliceViewType(const BrainOpenGLViewportContent* viewportContent,
                                     const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                     const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                     const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                     const GraphicsViewport& viewport);
        
        void drawVolumeSliceViewProjection(const BrainOpenGLViewportContent* viewportContent,
                                           const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                           const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const Vector3D& sliceCoordinates,
                                           const GraphicsViewport& viewport,
                                           const bool updateGraphicsObjectToWindowTransformFlag);
        
        void setOrthographicProjection(const GraphicsViewport& viewport);
        
        void setViewingTransformation(const VolumeMappableInterface* volume,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const MprVirtualSliceView& mprSliceView,
                                      const Vector3D& selectedSliceXYZ);

        void drawSliceIntensityProjection2D(const MprVirtualSliceView& mprSliceView,
                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const Vector3D& sliceCoordinates,
                                            const GraphicsViewport& viewport);

        void drawSliceIntensityProjection3D(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const Vector3D& sliceCoordinates,
                                            const GraphicsViewport& viewport);


        void drawSliceWithPrimitive(const MprVirtualSliceView& mprSliceView,
                                    const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                    const Vector3D& sliceCoordinates,
                                    const GraphicsViewport& viewport,
                                    const bool enabledBlendingFlag,
                                    const bool drawAttributesFlag,
                                    const bool drawIntensitySliceBackgroundFlag);
        
        void performTriangleIdentification(const GraphicsPrimitive* slicePrimitive,
                                           const MprVirtualSliceView& mprSliceView,
                                           VolumeMappableInterface* volumeInterface,
                                           const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                           const GraphicsViewport& viewport,
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

        void drawCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                            const Vector3D& sliceCoordinates,
                            const GraphicsViewport& viewport);
        
        void drawPanningCrosshairs(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                   const Vector3D& crossHairXYZ,
                                   const GraphicsViewport& viewport);
        
        void drawAxisLabels(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                            const GraphicsViewport& viewport);

        std::array<uint8_t, 4> getAxisColor(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const;
        
        void drawLayers(const VolumeMappableInterface* underlayVolume,
                        const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                        const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                        const Plane& slicePlane,
                        const Vector3D& sliceCoordinates,
                        const float sliceThickness);
        
        void drawVolumeSliceViewTypeMontage(const BrainOpenGLViewportContent* viewportContent,
                                            const VolumeSliceDrawingTypeEnum::Enum sliceDrawingType,
                                            const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                            const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                            const GraphicsViewport& viewport);

        float getSignedSliceThickness(const VolumeMappableInterface* volumeInterface,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane) const;
        
        std::vector<Vector3D> getVolumeRayIntersections(VolumeMappableInterface* volume,
                                                        const Vector3D& rayOrigin,
                                                        const Vector3D& rayVector) const;
        
        bool getVolumeSideIntersection(const VolumeMappableInterface* volume,
                                       const int64_t aIJK[],
                                       const int64_t bIJK[],
                                       const int64_t cIJK[],
                                       const int64_t dIJK[],
                                       const float rayOrigin[3],
                                       const float rayVector[3],
                                       const AString& sideName,
                                       Vector3D& intersectionXYZOut) const;

        void drawIntensityBackgroundSlice(const GraphicsPrimitive* volumePrimitive) const;
        
        std::vector<std::pair<VolumeMappableInterface*,int32_t>> getIntensityVolumeFilesAndMapIndices() const;
        
        void performIntensityIdentification(const MprVirtualSliceView& mprSliceView,
                                            VolumeMappableInterface* volume);
        
        float getVoxelSize(const VolumeMappableInterface* volume) const;
        
        void applySliceThicknessToIntersections(const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                const Vector3D& sliceCoordinates,
                                                std::vector<Vector3D>& rayVolumeIntersections) const;
        
        void setupIntensityModeBlending(const int32_t numSlicesDrawn);
        
        void drawAllViewRotationAxes(const BrowserTabContent* browserTabContent,
                                     const VolumeMappableInterface* underlayVolume,
                                     const int32_t viewport[4]);

        std::vector<Vector3D> getVolumeCorners(const VolumeMappableInterface* volume) const;

        bool setPrimtiveCoordinates(const MprVirtualSliceView& mprSliceView,
                                    const VolumeMappableInterface* volume,
                                    const Vector3D& sliceOffset,
                                    GraphicsPrimitiveV3fT3f* primitive);
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing = NULL;

        BrowserTabContent* m_browserTabContent = NULL;
        
        std::vector<BrainOpenGLFixedPipeline::VolumeDrawInfo> m_volumeDrawInfo;
        
        Brain* m_brain = NULL;
        
        BrainModelMode m_brainModelMode = BrainModelMode::INVALID;
        
        DisplayGroupEnum::Enum m_displayGroup;
        
        int32_t m_tabIndex = -1;
        
        VolumeMprViewModeEnum::Enum m_mprViewMode = VolumeMprViewModeEnum::MULTI_PLANAR_RECONSTRUCTION;
        
        VolumeMprOrientationModeEnum::Enum m_orientationMode = VolumeMprOrientationModeEnum::NEUROLOGICAL;

        std::array<double, 6> m_orthographicBounds;
        
        bool m_identificationModeFlag = false;
        
        bool m_axialCoronalParaSliceViewFlag = false;
        
        static float s_idNumRows;
        
        static float s_idNumCols;
        
        static constexpr bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_DECLARE__
    float BrainOpenGLVolumeMprThreeDrawing::s_idNumRows = 0.0;
    
    float BrainOpenGLVolumeMprThreeDrawing::s_idNumCols = 0.0;
#endif // __BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VOLUME_MPR_THREE_DRAWING_H__
