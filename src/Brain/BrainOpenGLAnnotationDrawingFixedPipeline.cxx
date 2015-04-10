
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
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "Surface.h"

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

    {
        AnnotationText* at = new AnnotationText();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        at->setXYZ(0.0, 0.2, 0);
        float rgba[4];
        CaretColorEnum::toRGBFloat(CaretColorEnum::WHITE, rgba);
        rgba[3] = 1.0;
        at->setForegroundColor(rgba);
        at->setText("TAB-LEFT-MIDDLE,0.0, 0.2");
        m_annotations.push_back(at);
    }
    
    {
        AnnotationText* at = new AnnotationText();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        at->setXYZ(0.95, 0.6, 0);
        at->setText("TAB-RIGHT-MIDDLE-0.95,0.6,-24-BOLD");
        at->setBoldEnabled(true);
        at->setFontSize(AnnotationFontSizeEnum::SIZE32);
        float rgba[4];
        CaretColorEnum::toRGBFloat(CaretColorEnum::BLUE, rgba);
        rgba[3] = 1.0;
        at->setForegroundColor(rgba);
        m_annotations.push_back(at);
    }
    
    
    {
        AnnotationText* at = new AnnotationText();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        at->setXYZ(0.5, 0.9, 0);
        at->setText("WINDOW-CENTER-TOP-0.5,0.9");
        at->setFontSize(AnnotationFontSizeEnum::SIZE32);
        float rgba[4];
        CaretColorEnum::toRGBFloat(CaretColorEnum::RED, rgba);
        rgba[3] = 1.0;
        at->setForegroundColor(rgba);
        m_annotations.push_back(at);
    }
    
    
    {
        AnnotationText* at = new AnnotationText();
        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
        at->setSurfaceSpace(StructureEnum::CORTEX_RIGHT, 32492, 19703);
        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
        at->setText("RIGHT surface node 19703");
        float rgba[4];
        CaretColorEnum::toRGBFloat(CaretColorEnum::GREEN, rgba);
        rgba[3] = 1.0;
        at->setForegroundColor(rgba);
        m_annotations.push_back(at);
    }
    
}

/**
 * Destructor.
 */
BrainOpenGLAnnotationDrawingFixedPipeline::~BrainOpenGLAnnotationDrawingFixedPipeline()
{
    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        delete *iter;
    }
    m_annotations.clear();
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
 * Draw the annotations in the given coordinate space.
 *
 * @param coordinateSpace
 *     Coordinate space of annotation that are drawn.
 * @param surfaceDisplayed
 *     Surface that is displayed.  May be NULL in some instances.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotations(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                           const Surface* surfaceDisplayed)
{
    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
         iter != m_annotations.end();
         iter++) {
        Annotation* annotation = *iter;
        CaretAssert(annotation);
        
        /*
         * Limit drawing of annotations to those in the
         * selected coordinate space.
         */
        if (annotation->getCoordinateSpace() != coordinateSpace) {
            continue;
        }
        
        float windowXYZ[3]  = { 0.0, 0.0, 0.0 };
        bool windowXYZValid = false;
        
        float modelXYZ[3]  = { 0.0, 0.0, 0.0 };
        bool modelXYZValid = false;
        
        float annotationXYZ[3];
        annotation->getXYZ(annotationXYZ);
        const AnnotationCoordinateSpaceEnum::Enum annotationCoordSpace = annotation->getCoordinateSpace();
        
        switch (annotationCoordSpace) {
            case AnnotationCoordinateSpaceEnum::MODEL:
                modelXYZ[0] = annotationXYZ[0];
                modelXYZ[1] = annotationXYZ[1];
                modelXYZ[2] = annotationXYZ[2];
                modelXYZValid = true;
                break;
            case AnnotationCoordinateSpaceEnum::PIXELS:
                windowXYZ[0] = annotationXYZ[0];
                windowXYZ[1] = annotationXYZ[1];
                windowXYZ[2] = annotationXYZ[2];
                windowXYZValid = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                if (surfaceDisplayed != NULL) {
                    StructureEnum::Enum structure = StructureEnum::INVALID;
                    int32_t surfaceNumberOfNodes  = -1;
                    int32_t surfaceNodeIndex      = -1;
                    annotation->getSurfaceSpace(structure,
                                                surfaceNumberOfNodes,
                                                surfaceNodeIndex);
                    if ((surfaceDisplayed->getStructure() == structure)
                        && (surfaceDisplayed->getNumberOfNodes() == surfaceNumberOfNodes)) {
                        if ((surfaceNodeIndex >= 0)
                            && (surfaceNodeIndex < surfaceNumberOfNodes)) {
                            float nodeXYZ[3];
                            surfaceDisplayed->getCoordinate(surfaceNodeIndex,
                                                            nodeXYZ);
                            modelXYZ[0] = nodeXYZ[0];
                            modelXYZ[1] = nodeXYZ[1];
                            modelXYZ[2] = nodeXYZ[2];
                            modelXYZValid = true;
                        }
                    }
                }
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
            {
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT,
                              viewport);
                windowXYZ[0] = viewport[2] * annotationXYZ[0];
                windowXYZ[1] = viewport[3] * annotationXYZ[1];
                windowXYZ[2] = 0.0;
                windowXYZValid = true;
            }
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
            {
                GLint viewport[4];
                glGetIntegerv(GL_VIEWPORT,
                              viewport);
                windowXYZ[0] = viewport[2] * annotationXYZ[0];
                windowXYZ[1] = viewport[3] * annotationXYZ[1];
                windowXYZ[2] = 0.0;
                windowXYZValid = true;
            }
                break;
        }

        if (modelXYZValid
            || windowXYZValid) {
            switch (annotation->getType()) {
                case AnnotationTypeEnum::ARROW:
                    break;
                case AnnotationTypeEnum::BOX:
                    break;
                case AnnotationTypeEnum::IMAGE:
                    break;
                case AnnotationTypeEnum::LINE:
                    break;
                case AnnotationTypeEnum::OVAL:
                    break;
                case AnnotationTypeEnum::SHAPE:
                    break;
                case AnnotationTypeEnum::TEXT:
                {
                    const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation);
                    CaretAssert(textAnnotation);
                    
                    if (modelXYZValid) {
                        m_brainOpenGLFixedPipeline->drawTextAtModelCoords(modelXYZ,
                                                                          *textAnnotation);
                    }
                    else if (windowXYZValid) {
                        m_brainOpenGLFixedPipeline->drawTextAtViewportCoords(windowXYZ[0],
                                                                             windowXYZ[1],
                                                                             windowXYZ[2],
                                                                             *textAnnotation);
                    }
                }
                    break;
            }
        }
    }
}
