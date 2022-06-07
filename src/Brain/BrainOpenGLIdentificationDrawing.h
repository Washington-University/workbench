#ifndef __BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_H__
#define __BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_H__

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



#include <memory>

#include "BrainOpenGLFixedPipeline.h"
#include "CaretObject.h"
#include "IdentifiedItemUniversalTypeEnum.h"


namespace caret {

    class BrainOpenGLFixedPipeline;
    class BrowserTabContent;
    class ClippingPlaneGroup;
    class IdentificationManager;
    class IdentifiedItemUniversal;
    class MediaFile;
    class Plane;
    class SelectionManager;
    class VolumeMappableInterface;
    
    class BrainOpenGLIdentificationDrawing : public CaretObject {
        
    public:
        BrainOpenGLIdentificationDrawing(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                         Brain* brain,
                                         BrowserTabContent* browserTabContent,
                                         const BrainOpenGLFixedPipeline::Mode drawingMode);
        
        virtual ~BrainOpenGLIdentificationDrawing();
        
        BrainOpenGLIdentificationDrawing(const BrainOpenGLIdentificationDrawing&) = delete;

        BrainOpenGLIdentificationDrawing& operator=(const BrainOpenGLIdentificationDrawing&) = delete;

        void drawMediaFileIdentificationSymbols(const MediaFile* mediaFile,
                                                const Plane& plane,
                                                const float mediaThickness,
                                                const float viewingZoom,
                                                const float viewportHeight);
        
        void drawSurfaceIdentificationSymbols(const Surface* surface,
                                              const float viewingZoom,
                                              const float viewportHeight);

        void drawVolumeIdentificationSymbols(const VolumeMappableInterface* volume,
                                             const int32_t mapIndex,
                                             const Plane& plane,
                                             const float sliceThickness,
                                             const float viewingZoom,
                                             const float viewportHeight);

        void drawVolumeIntensity2dIdentificationSymbols(const VolumeMappableInterface* volume,
                                                        const int32_t mapIndex,
                                                        const float viewingZoom,
                                                        const float viewportHeight);
        
        void drawVolumeIntensity3dIdentificationSymbols(const VolumeMappableInterface* volume,
                                                        const int32_t mapIndex,
                                                        const float viewingZoom,
                                                        const float viewportHeight);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void drawIdentificationSymbols(const IdentifiedItemUniversalTypeEnum::Enum drawingOnType,
                                       const Surface* surface,
                                       const MediaFile* mediaFile,
                                       const VolumeMappableInterface* volume,
                                       const Plane& plane,
                                       const float planeThickness,
                                       const float viewingZoom,
                                       const float viewportHeight,
                                       const float surfaceOrVolumeMaximumDimension);

        float getVolumeMaximumCoordinateDimension(const VolumeMappableInterface* volume,
                                                  const int32_t mapIndex) const;

        std::vector<const IdentifiedItemUniversal*> getIdentifiedItems();
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing = NULL;
        
        Brain* m_brain;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        const BrainOpenGLFixedPipeline::Mode m_drawingMode;
        
        IdentificationManager* m_idManager = NULL;

        SelectionManager* m_selectionManager = NULL;
        
        ClippingPlaneGroup* m_clippingPlaneGroup = NULL;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_H__
