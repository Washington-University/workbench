
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

#define __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

#include "AnnotationText.h"
#include "BrainOpenGLFixedPipeline.h"
#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::BrainOpenGLAnnotationDrawingFixedPipeline 
 * \brief OpenGL Fixed Pipeline drawing of Annotations.
 * \ingroup Brain
 */

/**
 * Constructor.
 * 
 * @param brainOpenGLFixedPipeline
 *    Fixed pipeline drawing.
 */
BrainOpenGLAnnotationDrawingFixedPipeline::BrainOpenGLAnnotationDrawingFixedPipeline(BrainOpenGLFixedPipeline* brainOpenGLFixedPipeline)
: CaretObject(),
m_brainOpenGLFixedPipeline(brainOpenGLFixedPipeline)
{
    CaretAssert(brainOpenGLFixedPipeline);
}

/**
 * Destructor.
 */
BrainOpenGLAnnotationDrawingFixedPipeline::~BrainOpenGLAnnotationDrawingFixedPipeline()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLAnnotationDrawingFixedPipeline::toString() const
{
    return "BrainOpenGLAnnotationDrawingFixedPipeline";
}

/**
 * Draw the annotations.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawWindowAnnotations()
{
//    AnnotationText at;
//    at.setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
//    at.setHorizontalAlignment(AnnotationAlignHorizontalEnum::RIGHT);
//    at.setVerticalAlignment(AnnotationAlignVerticalEnum::TOP);
//    at.setXYZ(150, 100, 0);
//    at.setText("AaBbCcDd");
//    
//    drawTextAnnotation(at);
}

/**
 * Draw a text annotation.
 *
 * @param textAnnotation
 *     Text annotation that is drawn.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawTextAnnotation(const AnnotationText& textAnnotation)
{
    const float* xyz = textAnnotation.getXYZ();
    m_brainOpenGLFixedPipeline->drawAnnotationText(textAnnotation);
}
