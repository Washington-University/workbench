#ifndef __BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_H__
#define __BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#include "CaretObject.h"
#include "Vector3D.h"

namespace caret {
    class BrainOpenGLFixedPipeline;
    class Plane;
    class Surface;
    class VolumeSurfaceOutlineSetModel;

    class BrainOpenGLVolumeSurfaceClippedOutlineDrawing : public CaretObject {
        
    public:
        BrainOpenGLVolumeSurfaceClippedOutlineDrawing(const Plane& plane,
                                                      const Vector3D& pointOnPlane,
                                                      const VolumeSurfaceOutlineSetModel* outlineSet,
                                                      BrainOpenGLFixedPipeline* fixedPipelineDrawing);
        
        virtual ~BrainOpenGLVolumeSurfaceClippedOutlineDrawing();
        
        BrainOpenGLVolumeSurfaceClippedOutlineDrawing(const BrainOpenGLVolumeSurfaceClippedOutlineDrawing&) = delete;

        BrainOpenGLVolumeSurfaceClippedOutlineDrawing& operator=(const BrainOpenGLVolumeSurfaceClippedOutlineDrawing&) = delete;
        
        void drawSurfaceOutline();

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void getClippingPlanes(const float slicePlaneDepth,
                               Plane& planeOneOut,
                               Plane& planeTwoOut) const;
        
        void drawSurfaceTrianglesWithVertexArrays(const Surface* surface,
                                                  const float* nodeColoringRGBA,
                                                  const float solidRGBA[4]) const;
        const Plane& m_plane;
        
        const Vector3D m_pointOnPlane;
        
        const VolumeSurfaceOutlineSetModel* m_outlineSet;
        
        BrainOpenGLFixedPipeline* m_fixedPipelineDrawing;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_VOLUME_SURFACE_CLIPPED_OUTLINE_DRAWING_H__
