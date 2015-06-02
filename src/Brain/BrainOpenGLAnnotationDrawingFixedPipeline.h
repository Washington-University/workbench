#ifndef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__
#define __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include <stdint.h>

#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretObject.h"
#include "CaretOpenGLInclude.h"
#include "Plane.h"


namespace caret {

    class Annotation;
    class AnnotationTwoDimensionalShape;
    class AnnotationBox;
    class AnnotationCoordinate;
    class AnnotationLine;
    class AnnotationOval;
    class AnnotationText;
    class BrainOpenGLFixedPipeline;
    class Surface;
    
    class BrainOpenGLAnnotationDrawingFixedPipeline : public CaretObject {
        
    public:
        BrainOpenGLAnnotationDrawingFixedPipeline(BrainOpenGLFixedPipeline* brainOpenGLFixedPipeline);
        
        virtual ~BrainOpenGLAnnotationDrawingFixedPipeline();
        
        void drawAnnotations(const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                             const Surface* surfaceDisplayed);

        void drawModelSpaceAnnotationsOnVolumeSlice(const Plane& plane);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        BrainOpenGLAnnotationDrawingFixedPipeline(const BrainOpenGLAnnotationDrawingFixedPipeline&);

        BrainOpenGLAnnotationDrawingFixedPipeline& operator=(const BrainOpenGLAnnotationDrawingFixedPipeline&);
        
        BrainOpenGLFixedPipeline* m_brainOpenGLFixedPipeline;
        
        bool getAnnotationWindowCoordinate(const AnnotationCoordinate* coordinate,
                                           const AnnotationCoordinateSpaceEnum::Enum annotationCoordSpace,
                                            const Surface* surfaceDisplayed,
                                            float windowXYZOut[3]) const;
        
        bool getAnnotationTwoDimShapeBounds(const AnnotationTwoDimensionalShape* annotation2D,
                                 const GLint viewport[4],
                                 const float windowXYZ[3],
                                 float bottomLeftOut[3],
                                 float bottomRightOut[3],
                                 float topRightOut[3],
                                 float topLeftOut[3]) const;
        
        void applyRotationToShape(const float rotationAngle,
                                  const float rotationPoint[3],
                                  float bottomLeftOut[3],
                                  float bottomRightOut[3],
                                  float topRightOut[3],
                                  float topLeftOut[3]) const;
        
        void drawBox(const AnnotationBox* box,
                       const Surface* surfaceDisplayed,
                       const uint8_t selectionColorRGBA[4],
                     const bool selectionFlag,
                     float selectionCenterXYZOut[3]);
        
        void drawLine(const AnnotationLine* line,
                      const Surface* surfaceDisplayed,
                      const uint8_t selectionColorRGBA[4],
                      const bool selectionFlag,
                      float selectionCenterXYZOut[3]);
        
        void drawOval(const AnnotationOval* oval,
                      const Surface* surfaceDisplayed,
                      const uint8_t selectionColorRGBA[4],
                      const bool selectionFlag,
                      float selectionCenterXYZOut[3]);
        
        void drawText(const AnnotationText* text,
                       const Surface* surfaceDisplayed,
                       const uint8_t selectionColorRGBA[4],
                      const bool selectionFlag,
                      float selectionCenterXYZOut[3]);
        
        void drawSelectorSquare(const float xyz[3],
                                const float halfWidthHeight,
                                const float rotationAngle);
        
        void drawAnnotationTwoDimSelector(const float bottomLeft[3],
                                          const float bottomRight[3],
                                          const float topRight[3],
                                          const float topLeft[3],
                                          const float lineThickness,
                                          const float rotationAngle);

        void drawAnnotationOneDimSelector(const float firstPoint[3],
                                          const float secondPoint[3],
                                          const float lineThickness);
        
        bool isDrawnWithDepthTesting(const Annotation* annotation);
        
        bool setDepthTestingStatus(const bool newDepthTestingStatus);
        
        /** OpenGL Model Matrix */
        GLdouble m_modelSpaceModelMatrix[16];
        
        /** OpenGL Projection Matrix */
        GLdouble m_modelSpaceProjectionMatrix[16];
        
        /** OpenGL Viewport */
        GLint    m_modelSpaceViewport[4];
        
        /** volume space plane */
        Plane m_volumeSpacePlane;
        
        bool m_volumeSpacePlaneValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__
