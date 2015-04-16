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


#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretObject.h"
#include "CaretOpenGLInclude.h"



namespace caret {

    class Annotation;
    class AnnotationText;
    class BrainOpenGLFixedPipeline;
    class Surface;
    
    class BrainOpenGLAnnotationDrawingFixedPipeline : public CaretObject {
        
    public:
        BrainOpenGLAnnotationDrawingFixedPipeline(BrainOpenGLFixedPipeline* brainOpenGLFixedPipeline);
        
        virtual ~BrainOpenGLAnnotationDrawingFixedPipeline();
        
        void drawAnnotations(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                             const Surface* surfaceDisplayed);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        BrainOpenGLAnnotationDrawingFixedPipeline(const BrainOpenGLAnnotationDrawingFixedPipeline&);

        BrainOpenGLAnnotationDrawingFixedPipeline& operator=(const BrainOpenGLAnnotationDrawingFixedPipeline&);
        
        BrainOpenGLFixedPipeline* m_brainOpenGLFixedPipeline;
        
        bool getAnnotationWindowCoordinates(GLdouble modelSpaceModelMatrix[16],
                                            GLdouble modelSpaceProjectionMatrix[16],
                                            GLint    modelSpaceViewport[4],
                                            const Annotation* annotation,
                                            const Surface* surfaceDisplayed,
                                            float windowXYZOut[3]) const;
        
        bool getAnnotationBounds(const Annotation* annotation,
                                 const GLint viewport[4],
                                 const float windowXYZ[3],
                                 double bottomLeftOut[3],
                                 double bottomRightOut[3],
                                 double topRightOut[3],
                                 double topLeftOut[3]) const;
        
        //        std::vector<Annotation*> m_annotations;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

} // namespace
#endif  //__BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_H__
