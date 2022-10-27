#ifndef __BRAIN_OPEN_G_L_FOCI_DRAWING_H__
#define __BRAIN_OPEN_G_L_FOCI_DRAWING_H__

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

#include "CaretObject.h"
#include "VolumeSliceViewPlaneEnum.h"

namespace caret {

    class Brain;
    class BrainOpenGLFixedPipeline;
    class HistologySlice;
    class Plane;
    class VolumeMappableInterface;
    
    class BrainOpenGLFociDrawing : public CaretObject {
        
    public:
        BrainOpenGLFociDrawing();
        
        virtual ~BrainOpenGLFociDrawing();
        
        void drawHistologyFoci(Brain* brain,
                               BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                               const HistologySlice* histologySlice,
                               const Plane& plane,
                               const float sliceThickness);

        void drawVolumeMprFoci(Brain* brain,
                               BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                               VolumeMappableInterface* underlayVolume,
                               const Plane& plane,
                               const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                               const float sliceThickness);

        void drawVolumeOrthogonalFoci(Brain* brain,
                                      BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                      VolumeMappableInterface* underlayVolume,
                                      const Plane& plane,
                                      const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                      const float sliceThickness);
        
        void drawVolumeObliqueFoci(Brain* brain,
                                   BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                   VolumeMappableInterface* underlayVolume,
                                   const Plane& plane,
                                   const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                   const float sliceThickness);

        BrainOpenGLFociDrawing(const BrainOpenGLFociDrawing&) = delete;

        BrainOpenGLFociDrawing& operator=(const BrainOpenGLFociDrawing&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        enum class DrawType {
            HISTOLOGY,
            VOLUME_MPR,
            VOLUME_OBLIQUE,
            VOLUME_ORTHOGONAL
        };
        
        void drawAllFoci(const DrawType drawType,
                         Brain* brain,
                         BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                         const HistologySlice* histologySlice,
                         VolumeMappableInterface* underlayVolume,
                         const Plane& plane,
                         const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                         const float sliceThickness);
        
        void drawSquare(const float size);
        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_FOCI_DRAWING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_FOCI_DRAWING_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_FOCI_DRAWING_H__
