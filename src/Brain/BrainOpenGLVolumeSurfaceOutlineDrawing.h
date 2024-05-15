#ifndef __BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_H__
#define __BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#include "CaretColorEnum.h"
#include "CaretObject.h"
#include "ModelTypeEnum.h"
#include "VolumeSliceProjectionTypeEnum.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "VolumeSurfaceOutlineModel.h"

namespace caret {
    
    class BrainOpenGLFixedPipeline;
    class GraphicsPrimitive;
    class HistologySlice;
    class HistologySlicesFile;
    class Matrix4x4;
    class Plane;
    class SurfaceFile;
    class VolumeMappableInterface;
    class VolumeSurfaceOutlineModel;
    class VolumeSurfaceOutlineModelCacheKey;
    class VolumeSurfaceOutlineSetModel;
    
    class BrainOpenGLVolumeSurfaceOutlineDrawing : public CaretObject {
        
    public:
        BrainOpenGLVolumeSurfaceOutlineDrawing();
        
        virtual ~BrainOpenGLVolumeSurfaceOutlineDrawing();
        
        BrainOpenGLVolumeSurfaceOutlineDrawing(const BrainOpenGLVolumeSurfaceOutlineDrawing&) = delete;
        
        BrainOpenGLVolumeSurfaceOutlineDrawing& operator=(const BrainOpenGLVolumeSurfaceOutlineDrawing&) = delete;
        
        void drawSurfaceOutline(const HistologySlicesFile* histologySlicesFile,
                                const HistologySlice* histologySlice,
                                const Vector3D& sliceXYZ,
                                VolumeSurfaceOutlineSetModel* outlineSet,
                                BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                const bool useNegativePolygonOffsetFlag);
        
        void drawSurfaceOutline(const VolumeMappableInterface* underlayVolume,
                                const ModelTypeEnum::Enum modelType,
                                const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                const float sliceXYZ[3],
                                const Plane& plane,
                                const Matrix4x4& displayTransformMatrix,
                                const bool displayTransformMatrixValidFlag,
                                VolumeSurfaceOutlineSetModel* outlineSet,
                                BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                const bool useNegativePolygonOffsetFlag);
        
        
        // ADD_NEW_METHODS_HERE
        
        virtual AString toString() const;
        
    private:
        void drawSurfaceOutlineCached(const HistologySlicesFile* histologySlicesFile,
                                      const HistologySlice* histologySlice,
                                      const VolumeMappableInterface* underlayVolume,
                                      const ModelTypeEnum::Enum modelType,
                                      const Plane& plane,
                                      VolumeSurfaceOutlineModelCacheKey& outlineCacheKey,
                                      VolumeSurfaceOutlineModel* outlineModel,
                                      BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                      const bool useNegativePolygonOffsetFlag);
        
        void drawSurfaceOutlineCachedOnVolume(const VolumeMappableInterface* underlayVolume,
                                              const ModelTypeEnum::Enum modelType,
                                              const VolumeSliceProjectionTypeEnum::Enum sliceProjectionType,
                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                              const float sliceXYZ[3],
                                              const Plane& plane,
                                              VolumeSurfaceOutlineModel* outlineModel,
                                              BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                              const bool useNegativePolygonOffsetFlag);
        
        void drawSurfaceOutlineNotCached(const VolumeMappableInterface* underlayVolume,
                                         const ModelTypeEnum::Enum modelType,
                                         const Plane& plane,
                                         const Matrix4x4& displayTransformMatrix,
                                         const bool displayTransformMatrixValidFlag,
                                         VolumeSurfaceOutlineModel* outlineModel,
                                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                         const bool useNegativePolygonOffsetFlag);
        
        void projectContoursToHistologySlice(const HistologySlice* histologySlice,
                                             std::vector<GraphicsPrimitive*>& contourPrimitives);
        
        void computeDepthNumStepsAndStepSize(const float sliceSpacingMM,
                                             const float slicePlaneDepth,
                                             const float userOutlineSeparation,
                                             int32_t& numStepsOut,
                                             float& depthStartOut,
                                             float& depthStepSizeOut);
        
        void createContours(const SurfaceFile* surface,
                            const Plane& plane,
                            const float sliceSpacingMM,
                            const CaretColorEnum::Enum caretColor,
                            const float* vertexColoringRGBA,
                            const float opacity,
                            const float contourThicknessMillimeters,
                            const float slicePlaneDepth,
                            const float userOutlineSeparation,
                            std::vector<GraphicsPrimitive*>& contourPrimitives);
        
        float getSeparation(const VolumeSurfaceOutlineModel* outline) const;
        
        // ADD_NEW_MEMBERS_HERE
        
    };
    
#ifdef __BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_DECLARE__
    
} // namespace
#endif  //__BRAIN_OPEN_G_L_VOLUME_SURFACE_OUTLINE_DRAWING_H__

