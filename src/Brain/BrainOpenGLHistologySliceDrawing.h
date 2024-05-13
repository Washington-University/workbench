#ifndef __BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_H__
#define __BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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
#include <memory>

#include "CaretObject.h"
#include "GraphicsTextureMagnificationFilterEnum.h"
#include "GraphicsTextureMinificationFilterEnum.h"
#include "GraphicsViewport.h"
#include "HistologyOverlay.h"


namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrainOpenGLViewportContent;
    class BrowserTabContent;
    class GraphicsObjectToWindowTransform;
    class GraphicsOrthographicProjection;
    class GraphicsPrimitiveV3fT2f;
    class HistologyCoordinate;
    class HistologyOverlaySet;
    class MediaFile;
    class ModelHistology;
    class Overlay;
    class VolumeMappableInterface;
    
    class BrainOpenGLHistologySliceDrawing : public CaretObject {
        
    public:
        
        BrainOpenGLHistologySliceDrawing();
        
        virtual ~BrainOpenGLHistologySliceDrawing();
        
        void draw(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                  const BrainOpenGLViewportContent* viewportContent,
                  BrowserTabContent* browserTabContent,
                  ModelHistology* histologyModel,
                  const std::array<int32_t, 4>& viewport);
        
        BrainOpenGLHistologySliceDrawing(const BrainOpenGLHistologySliceDrawing&) = delete;

        BrainOpenGLHistologySliceDrawing& operator=(const BrainOpenGLHistologySliceDrawing&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class VolumeDrawingInfo {
        public:
            VolumeDrawingInfo(Overlay* overlay,
                              VolumeMappableInterface* volumeMappableInterface,
                              int32_t mapIndex)
            : m_overlay(overlay),
            m_volumeMappableInterface(volumeMappableInterface),
            m_mapIndex(mapIndex) { }

            Overlay* m_overlay;
            VolumeMappableInterface* m_volumeMappableInterface;
            int32_t m_mapIndex;
        };
        
        void drawModelLayers(const GraphicsOrthographicProjection& orthographicProjection,
                             const BrainOpenGLViewportContent* viewportContent,
                             const GraphicsObjectToWindowTransform* transform,
                             const Vector3D& underlayStereotaxicXYZ);
        
        void processSelection(const int32_t tabIndex,
                              const HistologyOverlay::DrawingData& drawingData,
                              GraphicsPrimitiveV3fT2f* primitive);

        bool getOrthoBounds(GraphicsOrthographicProjection& orthographicsProjectionOut);

        void drawCrosshairs(const GraphicsOrthographicProjection& orthographicsProjection,
                            const HistologyCoordinate& histologyCoordinate);
        
        void drawVolumeOverlays();
        
        void drawVolumeOverlaysOnCziImageFile(std::vector<VolumeDrawingInfo>& volumeDrawingInfo);
        
        void drawVolumeOverlaysOnImageFile(std::vector<VolumeDrawingInfo>& volumeDrawingInfo);
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing = NULL;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        GraphicsViewport m_viewport;
        
        std::vector<HistologyOverlay::DrawingData> m_mediaFilesAndDataToDraw;
        
        Vector3D m_identificationStereotaxicXYZ;
        
        bool m_identificationStereotaxicXYZValidFlag = false;
        
        static GraphicsTextureMagnificationFilterEnum::Enum s_textureMagnificationFilter;
        static GraphicsTextureMinificationFilterEnum::Enum  s_textureMinificationFilter;
        
        static constexpr bool s_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_DECLARE__
    GraphicsTextureMagnificationFilterEnum::Enum BrainOpenGLHistologySliceDrawing::s_textureMagnificationFilter = GraphicsTextureMagnificationFilterEnum::LINEAR;
    GraphicsTextureMinificationFilterEnum::Enum  BrainOpenGLHistologySliceDrawing::s_textureMinificationFilter  = GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR;
#endif // __BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_HISTOLOGY_SLICE_DRAWING_H__
