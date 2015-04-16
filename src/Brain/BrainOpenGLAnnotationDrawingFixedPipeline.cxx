
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

#include "AnnotationFile.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "IdentificationWithColor.h"
#include "SelectionManager.h"
#include "SelectionItemAnnotation.h"
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

//    {
//        AnnotationText* at = new AnnotationText();
//        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
//        at->setTabIndex(3);
//        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
//        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
//        at->setXYZ(0.4, 0.2, 0);
//        float rgba[4];
//        CaretColorEnum::toRGBFloat(CaretColorEnum::WHITE, rgba);
//        rgba[3] = 1.0;
//        at->setForegroundColor(rgba);
//        at->setWidth2D(120);
//        at->setHeight2D(20);
//        at->setText("TAB-4-LEFT-MIDDLE,0.4, 0.2");
//        m_annotations.push_back(at);
//    }

//    {
//        AnnotationText* at = new AnnotationText();
//        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
//        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::LEFT);
//        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
//        at->setXYZ(0.0, 0.2, 0);
//        float rgba[4];
//        CaretColorEnum::toRGBFloat(CaretColorEnum::WHITE, rgba);
//        rgba[3] = 1.0;
//        at->setForegroundColor(rgba);
//        at->setWidth2D(150);
//        at->setHeight2D(20);
//        at->setText("TAB-LEFT-MIDDLE,0.0, 0.2");
//        m_annotations.push_back(at);
//    }
//    
//    {
//        AnnotationText* at = new AnnotationText();
//        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::TAB);
//        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
//        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
//        at->setXYZ(0.95, 0.6, 0);
//        at->setText("TAB-RIGHT-MIDDLE-0.95,0.6,-24-BOLD");
//        at->setBoldEnabled(true);
//        at->setFontSize(AnnotationFontSizeEnum::SIZE32);
//        float rgba[4];
//        CaretColorEnum::toRGBFloat(CaretColorEnum::BLUE, rgba);
//        rgba[3] = 1.0;
//        at->setForegroundColor(rgba);
//        at->setWidth2D(150);
//        at->setHeight2D(20);
//        m_annotations.push_back(at);
//    }
//    
//    
//    {
//        AnnotationText* at = new AnnotationText();
//        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::WINDOW);
//        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
//        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
//        at->setXYZ(0.5, 0.9, 0);
//        at->setText("WINDOW-CENTER-TOP-0.5,0.9");
//        at->setFontSize(AnnotationFontSizeEnum::SIZE32);
//        float rgba[4];
//        CaretColorEnum::toRGBFloat(CaretColorEnum::RED, rgba);
//        rgba[3] = 1.0;
//        at->setForegroundColor(rgba);
//        at->setWidth2D(150);
//        at->setHeight2D(20);
//        m_annotations.push_back(at);
//    }
//    
//    
//    {
//        AnnotationText* at = new AnnotationText();
//        at->setCoordinateSpace(AnnotationCoordinateSpaceEnum::SURFACE);
//        at->setSurfaceSpace(StructureEnum::CORTEX_RIGHT, 32492, 19703);
//        at->setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::RIGHT);
//        at->setVerticalAlignment(AnnotationTextAlignVerticalEnum::MIDDLE);
//        at->setText("RIGHT surface node 19703");
//        float rgba[4];
//        CaretColorEnum::toRGBFloat(CaretColorEnum::GREEN, rgba);
//        rgba[3] = 1.0;
//        at->setForegroundColor(rgba);
//        at->setWidth2D(150);
//        at->setHeight2D(20);
//        m_annotations.push_back(at);
//    }
    
}

/**
 * Destructor.
 */
BrainOpenGLAnnotationDrawingFixedPipeline::~BrainOpenGLAnnotationDrawingFixedPipeline()
{
//    for (std::vector<Annotation*>::iterator iter = m_annotations.begin();
//         iter != m_annotations.end();
//         iter++) {
//        delete *iter;
//    }
//    m_annotations.clear();
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
 * Get the window coordinate for display of the annotation.
 *
 * @param modelSpaceModelMatrix
 *     The OpenGL modeling matrix.
 * @param modelSpaceProjectionMatrix
 *     The OpenGL projection matrix.
 * @param modelSpaceViewport
 *     The OpenGL viewport.
 * @param annotation
 *     The annotation whose window coordinate is computed.
 * @param surfaceDisplayed
 *     Surface that is displayed (may be NULL !)
 * @param windowXYZOut
 *     Output containing the window coordinate.
 * @return
 *     True if the window coordinate is valid, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::getAnnotationWindowCoordinates(GLdouble modelSpaceModelMatrix[16],
                                                                          GLdouble modelSpaceProjectionMatrix[16],
                                                                          GLint    modelSpaceViewport[4],
                                                                          const Annotation* annotation,
                                                                          const Surface* surfaceDisplayed,
                                                                          float windowXYZOut[3]) const
{
    float modelXYZ[3]  = { 0.0, 0.0, 0.0 };
    bool modelXYZValid = false;
    
    double windowXYZ[3] = { 0.0, 0.0, 0.0 };
    bool windowXYZValid = false;
    
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
    
    if (modelXYZValid) {
        /*
         * Convert model space coordinates to window coordinates
         * as all annotations are drawn in window coordinates.
         */
        GLdouble windowX, windowY, windowZ;
        if (gluProject(modelXYZ[0], modelXYZ[1], modelXYZ[2],
                       modelSpaceModelMatrix, modelSpaceProjectionMatrix, modelSpaceViewport,
                       &windowX, &windowY, &windowZ) == GL_TRUE) {
            windowXYZ[0] = windowX;
            windowXYZ[1] = windowY;
            windowXYZ[2] = windowZ;
            modelXYZValid = false;
            windowXYZValid = true;
        }
        else {
            CaretLogSevere("Failed to convert model coordinates to window coordinates for annotation identification.");
        }
    }
    
    if (windowXYZValid) {
        windowXYZOut[0] = windowXYZ[0];
        windowXYZOut[1] = windowXYZ[1];
        windowXYZOut[2] = windowXYZ[2];
    }
    
    return windowXYZValid;
}

/**
 * Get the bounds for an annotation.
 *
 * @param annotation
 *     The annotation whose bounds is computed.
 * @param viewport
 *     The current OpenGL Viewport (x, y, w, h)
 * @param windowXYZ
 *     Window coordinates of the annotation.
 * @param bottomLeftOut
 *     The bottom left corner of the annotation bounds.
 * @param bottomRightOut
 *     The bottom right corner of the annotation bounds.
 * @param topRightOut
 *     The top right corner of the annotation bounds.
 * @param topLeftOut
 *     The top left corner of the annotation bounds.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::getAnnotationBounds(const Annotation* annotation,
                                                               const GLint viewport[4],
                                                               const float windowXYZ[3],
                                                               double bottomLeftOut[3],
                                                               double bottomRightOut[3],
                                                               double topRightOut[3],
                                                               double topLeftOut[3]) const
{
    /*
     * Note: Width and height are in range 0.0 to 1.0 for all types
     * EXCEPT for text.
     */
//    double halfWidth  = 0.0;
//    double halfHeight = 0.0;
//    switch (annotation->getShapeDimension()) {
//        case AnnotationShapeDimensionEnum::ONE_DIMENSIONAL:
//            halfWidth  = 4.0;
//            halfHeight = annotation->getLength1D();
//            break;
//        case AnnotationShapeDimensionEnum::TWO_DIMENSIONAL:
//            halfWidth  = annotation->getWidth2D()  / 2.0;
//            halfHeight = annotation->getHeight2D() / 2.0;
//            break;
//    }
//    if (annotation->getType() != AnnotationTypeEnum::TEXT) {
//        halfWidth  *= modelSpaceViewport[2];
//        halfHeight *= modelSpaceViewport[3];
//    }
//    
//    double bottomLeft[3]  = { windowXYZ[0] - halfWidth, windowXYZ[1] - halfHeight, 0.0 };
//    double bottomRight[3] = { windowXYZ[0] + halfWidth, windowXYZ[1] - halfHeight, 0.0 };
//    double topRight[3]    = { windowXYZ[0] + halfWidth, windowXYZ[1] + halfHeight, 0.0 };
//    double topLeft[3]     = { windowXYZ[0] - halfWidth, windowXYZ[1] + halfHeight, 0.0 };
    
    const double viewportWidth  = viewport[2];
    const double viewportHeight = viewport[3];
    
    bool boundsValid = false;
    switch (annotation->getType()) {
        case AnnotationTypeEnum::ARROW:
        case AnnotationTypeEnum::LINE:
        {
            /*
             * NOTE: Annotation's height and width are 'relative' (percentage) of window size.
             */
            const double halfWidth = 2.0;
            const double height    = annotation->getLength1D() * viewportHeight;
            
            bottomLeftOut[0]  = windowXYZ[0] - halfWidth;
            bottomLeftOut[1]  = windowXYZ[1];
            bottomLeftOut[2]  = windowXYZ[2];
            bottomRightOut[0] = windowXYZ[0] + halfWidth;
            bottomRightOut[1] = windowXYZ[1];
            bottomRightOut[2] = windowXYZ[2];
            topRightOut[0]    = windowXYZ[0] + halfWidth;
            topRightOut[1]    = windowXYZ[1] + height;
            topRightOut[2]    = windowXYZ[2];
            topLeftOut[0]     = windowXYZ[0] - halfWidth;
            topLeftOut[1]     = windowXYZ[1] + height;
            topLeftOut[2]     = windowXYZ[2];
            
            boundsValid       = true;
        }
            break;
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::OVAL:
        {
            /*
             * NOTE: Annotation's height and width are 'relative' (percentage) of window size.
             */
            const double halfWidth  = (annotation->getWidth2D()  / 2.0) * viewportWidth;
            const double halfHeight = (annotation->getHeight2D() / 2.0) * viewportHeight;
            
            bottomLeftOut[0]  = windowXYZ[0] - halfWidth;
            bottomLeftOut[1]  = windowXYZ[1] - halfHeight;
            bottomLeftOut[2]  = windowXYZ[2];
            bottomRightOut[0] = windowXYZ[0] + halfWidth;
            bottomRightOut[1] = windowXYZ[1] - halfHeight;
            bottomRightOut[2] = windowXYZ[2];
            topRightOut[0]    = windowXYZ[0] + halfWidth;
            topRightOut[1]    = windowXYZ[1] + halfHeight;
            topRightOut[2]    = windowXYZ[2];
            topLeftOut[0]     = windowXYZ[0] - halfWidth;
            topLeftOut[1]     = windowXYZ[1] + halfHeight;
            topLeftOut[2]     = windowXYZ[2];
            
            boundsValid       = true;
        }
            break;
        case AnnotationTypeEnum::IMAGE:
            CaretAssertToDoFatal();
            break;
        case AnnotationTypeEnum::SHAPE:
            break;
        case AnnotationTypeEnum::TEXT:
        {
            double xMin = 0.0;
            double xMax = 0.0;
            double yMin = 0.0;
            double yMax = 0.0;
            const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation);
            CaretAssert(textAnnotation);
            m_brainOpenGLFixedPipeline->textRenderer->getBoundsForTextAtViewportCoords(*textAnnotation,
                                                                                       windowXYZ[0], windowXYZ[1], windowXYZ[2],
                                                                                       xMin, xMax, yMin, yMax);
            bottomLeftOut[0]  = xMin;
            bottomLeftOut[1]  = yMin;
            bottomLeftOut[2]  = windowXYZ[2];
            bottomRightOut[0] = xMax;
            bottomRightOut[1] = yMin;
            bottomRightOut[2] = windowXYZ[2];
            topRightOut[0]    = xMax;
            topRightOut[1]    = yMax;
            topRightOut[2]    = windowXYZ[2];
            topLeftOut[0]     = xMin;
            topLeftOut[1]     = yMax;
            topLeftOut[2]     = windowXYZ[2];
            
            boundsValid       = true;
        }
            break;
    }
    
    return boundsValid;
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
    if (m_brainOpenGLFixedPipeline->m_brain == NULL) {
        return;
    }
    
    SelectionItemAnnotation* annotationID = m_brainOpenGLFixedPipeline->m_brain->getSelectionManager()->getAnnotationIdentification();
    
    GLint savedShadeModel;
    glGetIntegerv(GL_SHADE_MODEL,
                  &savedShadeModel);
    /*
     * Check for a 'selection' type mode
     */
    bool isSelect = false;
    switch (m_brainOpenGLFixedPipeline->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (annotationID->isEnabledForSelection()) {
                isSelect = true;
            }
            else {
                return;
            }
            
            /*
             * Need flag shading for identification
             */
            glShadeModel(GL_FLAT);
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    /*
     * When selection is performed, annoations in model space need
     * to be converted to window coordinates.  However, when 
     * selecting, all annotations are drawn in WINDOW SPACE 
     * as a rectangle in a solid color so that the color selector
     * can be used.
     *
     * So, when selecting:
     *    (1) Save the matrices and viewewport if drawing in
     *        model space.
     *    (2) Setup matrices for pixel (window) coordinates.
     *        Since we are changing the matrices, they must
     *        be saved as is done in (1).
     */
    GLdouble modelSpaceModelMatrix[16];
    GLdouble modelSpaceProjectionMatrix[16];
    GLint    modelSpaceViewport[4];
    
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 modelSpaceModelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX,
                 modelSpaceProjectionMatrix);
    glGetIntegerv(GL_VIEWPORT,
                  modelSpaceViewport);

    /*
     * All drawing is in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, modelSpaceViewport[2],
            0.0, modelSpaceViewport[3],
            -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /*
     * When selecting, clear out all previous drawing
     * since we identify via colors in each pixel.
     */
    if (isSelect) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    /*
     * Used when a selection operations is performed
     * to assist with finding the selected annotation.
     */
    std::vector<double> annotationSelectionWindowXYZ;
    std::vector<Annotation*> annotationsDrawnForSelection;
    
    std::vector<AnnotationFile*> allAnnotationFiles;
    m_brainOpenGLFixedPipeline->m_brain->getAllAnnotationFiles(allAnnotationFiles);
    for (std::vector<AnnotationFile*>::iterator fileIter = allAnnotationFiles.begin();
         fileIter != allAnnotationFiles.end();
         fileIter++) {
        AnnotationFile* annotationFile = *fileIter;
        const std::vector<Annotation*> allAnnotations = annotationFile->getAllAnnotations();
        
        const int32_t annotationCount = static_cast<int32_t>(allAnnotations.size());
        for (int32_t iAnn = 0; iAnn < annotationCount; iAnn++) {
            CaretAssertVectorIndex(allAnnotations, iAnn);
            Annotation* annotation = allAnnotations[iAnn];
            CaretAssert(annotation);
            
            /*
             * Limit drawing of annotations to those in the
             * selected coordinate space.
             */
            if (annotation->getCoordinateSpace() != coordinateSpace) {
                continue;
            }
            
            /*
             * Skip annotations in a different tab
             */
            if (annotation->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB) {
                if (m_brainOpenGLFixedPipeline->windowTabIndex != annotation->getTabIndex()) {
                    continue;
                }
            }
            
            float windowXYZ[3]  = { 0.0, 0.0, 0.0 };
            const bool windowXYZValid = getAnnotationWindowCoordinates(modelSpaceModelMatrix,
                                                                       modelSpaceProjectionMatrix,
                                                                       modelSpaceViewport,
                                                                       annotation,
                                                                       surfaceDisplayed,
                                                                       windowXYZ);
            
            
//            float modelXYZ[3]  = { 0.0, 0.0, 0.0 };
//            bool modelXYZValid = false;
//            
//            float annotationXYZ[3];
//            annotation->getXYZ(annotationXYZ);
//            const AnnotationCoordinateSpaceEnum::Enum annotationCoordSpace = annotation->getCoordinateSpace();
//            
//            switch (annotationCoordSpace) {
//                case AnnotationCoordinateSpaceEnum::MODEL:
//                    modelXYZ[0] = annotationXYZ[0];
//                    modelXYZ[1] = annotationXYZ[1];
//                    modelXYZ[2] = annotationXYZ[2];
//                    modelXYZValid = true;
//                    break;
//                case AnnotationCoordinateSpaceEnum::PIXELS:
//                    windowXYZ[0] = annotationXYZ[0];
//                    windowXYZ[1] = annotationXYZ[1];
//                    windowXYZ[2] = annotationXYZ[2];
//                    windowXYZValid = true;
//                    break;
//                case AnnotationCoordinateSpaceEnum::SURFACE:
//                    if (surfaceDisplayed != NULL) {
//                        StructureEnum::Enum structure = StructureEnum::INVALID;
//                        int32_t surfaceNumberOfNodes  = -1;
//                        int32_t surfaceNodeIndex      = -1;
//                        annotation->getSurfaceSpace(structure,
//                                                    surfaceNumberOfNodes,
//                                                    surfaceNodeIndex);
//                        if ((surfaceDisplayed->getStructure() == structure)
//                            && (surfaceDisplayed->getNumberOfNodes() == surfaceNumberOfNodes)) {
//                            if ((surfaceNodeIndex >= 0)
//                                && (surfaceNodeIndex < surfaceNumberOfNodes)) {
//                                float nodeXYZ[3];
//                                surfaceDisplayed->getCoordinate(surfaceNodeIndex,
//                                                                nodeXYZ);
//                                modelXYZ[0] = nodeXYZ[0];
//                                modelXYZ[1] = nodeXYZ[1];
//                                modelXYZ[2] = nodeXYZ[2];
//                                modelXYZValid = true;
//                            }
//                        }
//                    }
//                    break;
//                case AnnotationCoordinateSpaceEnum::TAB:
//                {
//                    GLint viewport[4];
//                    glGetIntegerv(GL_VIEWPORT,
//                                  viewport);
//                    windowXYZ[0] = viewport[2] * annotationXYZ[0];
//                    windowXYZ[1] = viewport[3] * annotationXYZ[1];
//                    windowXYZ[2] = 0.0;
//                    windowXYZValid = true;
//                }
//                    break;
//                case AnnotationCoordinateSpaceEnum::WINDOW:
//                {
//                    GLint viewport[4];
//                    glGetIntegerv(GL_VIEWPORT,
//                                  viewport);
//                    windowXYZ[0] = viewport[2] * annotationXYZ[0];
//                    windowXYZ[1] = viewport[3] * annotationXYZ[1];
//                    windowXYZ[2] = 0.0;
//                    windowXYZValid = true;
//                }
//                    break;
//            }
//            
//            if (modelXYZValid) {
//                /*
//                 * Convert model space coordinates to window coordinates
//                 * as all annotations are drawn in window coordinates.
//                 */
//                GLdouble windowX, windowY, windowZ;
//                if (gluProject(modelXYZ[0], modelXYZ[1], modelXYZ[2],
//                               modelSpaceModelMatrix, modelSpaceProjectionMatrix, modelSpaceViewport,
//                               &windowX, &windowY, &windowZ) == GL_TRUE) {
//                    windowXYZ[0] = windowX;
//                    windowXYZ[1] = windowY;
//                    windowXYZ[2] = windowZ;
//                    modelXYZValid = false;
//                    windowXYZValid = true;
//                }
//                else {
//                    CaretLogSevere("Failed to convert model coordinates to window coordinates for annotation identification.");
//                    continue;
//                }
//            }
            
            if (windowXYZValid) {
                double bottomLeft[3]  = { 0.0, 0.0, 0.0 };
                double bottomRight[3] = { 0.0, 0.0, 0.0 };
                double topRight[3]    = { 0.0, 0.0, 0.0 };
                double topLeft[3]     = { 0.0, 0.0, 0.0 };
                
                const bool boundsValid = getAnnotationBounds(annotation,
                                                             modelSpaceViewport,
                                                             windowXYZ,
                                                             bottomLeft,
                                                             bottomRight,
                                                             topRight,
                                                             topLeft);
                if ( ! boundsValid) {
                    continue;
                }
//                /*
//                 * Note: Width and height are in range 0.0 to 1.0 for all types
//                 * EXCEPT for text.
//                 */
//                double halfWidth  = 0.0;
//                double halfHeight = 0.0;
//                switch (annotation->getShapeDimension()) {
//                    case AnnotationShapeDimensionEnum::ONE_DIMENSIONAL:
//                        halfWidth  = 4.0;
//                        halfHeight = annotation->getLength1D();
//                        break;
//                    case AnnotationShapeDimensionEnum::TWO_DIMENSIONAL:
//                        halfWidth  = annotation->getWidth2D()  / 2.0;
//                        halfHeight = annotation->getHeight2D() / 2.0;
//                        break;
//                }
//                if (annotation->getType() != AnnotationTypeEnum::TEXT) {
//                    halfWidth  *= modelSpaceViewport[2];
//                    halfHeight *= modelSpaceViewport[3];
//                }
//                
//                double bottomLeft[3]  = { windowXYZ[0] - halfWidth, windowXYZ[1] - halfHeight, 0.0 };
//                double bottomRight[3] = { windowXYZ[0] + halfWidth, windowXYZ[1] - halfHeight, 0.0 };
//                double topRight[3]    = { windowXYZ[0] + halfWidth, windowXYZ[1] + halfHeight, 0.0 };
//                double topLeft[3]     = { windowXYZ[0] - halfWidth, windowXYZ[1] + halfHeight, 0.0 };
//                switch (annotation->getType()) {
//                    case AnnotationTypeEnum::ARROW:
//                        break;
//                    case AnnotationTypeEnum::BOX:
//                        break;
//                    case AnnotationTypeEnum::IMAGE:
//                        break;
//                    case AnnotationTypeEnum::LINE:
//                        break;
//                    case AnnotationTypeEnum::OVAL:
//                        break;
//                    case AnnotationTypeEnum::SHAPE:
//                        break;
//                    case AnnotationTypeEnum::TEXT:
//                    {
//                        double xMin = 0.0;
//                        double xMax = 0.0;
//                        double yMin = 0.0;
//                        double yMax = 0.0;
//                        const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation);
//                        CaretAssert(textAnnotation);
//                        m_brainOpenGLFixedPipeline->textRenderer->getBoundsForTextAtViewportCoords(*textAnnotation,
//                                                                                                   windowXYZ[0], windowXYZ[1], windowXYZ[2],
//                                                                                                   xMin, xMax, yMin, yMax);
//                        bottomLeft[0]  = xMin;
//                        bottomLeft[1]  = yMin;
//                        bottomLeft[2]  = 0.0;
//                        bottomRight[0] = xMax;
//                        bottomRight[1] = yMin;
//                        bottomRight[2] = 0.0;
//                        topRight[0]    = xMax;
//                        topRight[1]    = yMax;
//                        topRight[2]    = 0.0;
//                        topLeft[0]     = xMin;
//                        topLeft[1]     = yMax;
//                        topLeft[2]     = 0.0;
//                    }
//                        break;
//                }
                
                if (isSelect) {
                    /*
                     * Each annotations is drawn as a solid color and the color of the
                     * selected pixel identifies the annotation.
                     */
                    uint8_t idColorRGBA[4];
                    const int32_t annotationDrawnIndex = static_cast<int32_t>(annotationsDrawnForSelection.size());
                    m_brainOpenGLFixedPipeline->colorIdentification->addItem(idColorRGBA,
                                                                             SelectionItemDataTypeEnum::ANNOTATION,
                                                                             annotationDrawnIndex);
                    annotationsDrawnForSelection.push_back(annotation);
                    
                    /*
                     * Draw a polygon that covers annotation with selection color
                     */
                    glColor3ubv(idColorRGBA);
                    glBegin(GL_POLYGON);
                    glVertex3dv(bottomLeft);
                    glVertex3dv(bottomRight);
                    glVertex3dv(topRight);
                    glVertex3dv(topLeft);
                    glEnd();
                    
//                    std::cout << "Bounds BL:" << qPrintable(AString::fromNumbers(bottomLeft, 3, ","))
//                    << "  TR: " << qPrintable(AString::fromNumbers(topRight, 3, ",")) << std::endl;
//                    if ((m_brainOpenGLFixedPipeline->mouseX > bottomLeft[0])
//                        && (m_brainOpenGLFixedPipeline->mouseX < topRight[0])
//                        && (m_brainOpenGLFixedPipeline->mouseY > bottomLeft[1])
//                        && (m_brainOpenGLFixedPipeline->mouseY < topRight[1])) {
//                        std::cout << "  INSIDE" << std::endl;
//                    }
                    
                    annotationSelectionWindowXYZ.push_back(windowXYZ[0]);
                    annotationSelectionWindowXYZ.push_back(windowXYZ[1]);
                    annotationSelectionWindowXYZ.push_back(windowXYZ[2]);
                }
                else {
                    switch (annotation->getType()) {
                        case AnnotationTypeEnum::ARROW:
                            CaretAssertToDoFatal();
                            break;
                        case AnnotationTypeEnum::BOX:
                        {
                            float backgroundRGBA[4];
                            annotation->getBackgroundColor(backgroundRGBA);
                            if (backgroundRGBA[3] > 0) {
                                glColor3fv(backgroundRGBA);
                                glBegin(GL_POLYGON);
                                glVertex3dv(bottomLeft);
                                glVertex3dv(bottomRight);
                                glVertex3dv(topRight);
                                glVertex3dv(topLeft);
                                glEnd();
                            }
                            
                            float foregroundRGBA[4];
                            annotation->getForegroundColor(foregroundRGBA);
                            glColor3fv(foregroundRGBA);
                            glLineWidth(2.0);
                            glBegin(GL_LINE_LOOP);
                            glVertex3dv(bottomLeft);
                            glVertex3dv(bottomRight);
                            glVertex3dv(topRight);
                            glVertex3dv(topLeft);
                            glEnd();
                        }
                            break;
                        case AnnotationTypeEnum::IMAGE:
                            CaretAssertToDoFatal();
                            break;
                        case AnnotationTypeEnum::LINE:
                        {
                            float backgroundRGBA[4];
                            annotation->getBackgroundColor(backgroundRGBA);
                            if (backgroundRGBA[3] > 0) {
                                glColor3fv(backgroundRGBA);
                                glBegin(GL_POLYGON);
                                glVertex3dv(bottomLeft);
                                glVertex3dv(bottomRight);
                                glVertex3dv(topRight);
                                glVertex3dv(topLeft);
                                glEnd();
                            }
                            
                            float foregroundRGBA[4];
                            annotation->getForegroundColor(foregroundRGBA);
                            glColor3fv(foregroundRGBA);
                            const double lineStart[3] = {
                                (bottomLeft[0] + bottomRight[0]) / 2.0,
                                (bottomLeft[1] + bottomRight[1]) / 2.0,
                                (bottomLeft[2] + bottomRight[2]) / 2.0
                            };
                            const double lineEnd[3] = {
                                (topLeft[0] + topRight[0]) / 2.0,
                                (topLeft[1] + topRight[1]) / 2.0,
                                (topLeft[2] + topRight[2]) / 2.0
                            };
                            glLineWidth(2.0);
                            glBegin(GL_LINES);
                            glVertex3dv(lineStart);
                            glVertex3dv(lineEnd);
                            glEnd();
                        }
                            break;
                        case AnnotationTypeEnum::OVAL:
                            CaretAssertToDoFatal();
                            break;
                        case AnnotationTypeEnum::SHAPE:
                            CaretAssertToDoFatal();
                            break;
                        case AnnotationTypeEnum::TEXT:
                        {
                            const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation);
                            CaretAssert(textAnnotation);
                            
//                            if (modelXYZValid) {
//                                m_brainOpenGLFixedPipeline->textRenderer->drawTextAtModelCoords(modelXYZ,
//                                                                                                *textAnnotation);
//                            }
//                            else if (windowXYZValid) {
                                m_brainOpenGLFixedPipeline->textRenderer->drawTextAtViewportCoords(windowXYZ[0],
                                                                                                   windowXYZ[1],
                                                                                                   windowXYZ[2],
                                                                                                   *textAnnotation);
//                            }
                        }
                            break;
                    }
                    
                    /*
                     * Is this the selected annotation ?
                     */
                    if (annotation->isSelected()) {
                        /*
                         * Draw a polygon that frames annotation with foreground color
                         */
                        glColor3fv(annotation->getForegroundColor());
                        glLineWidth(4.0);
                        glBegin(GL_LINE_LOOP);
                        glVertex3dv(bottomLeft);
                        glVertex3dv(bottomRight);
                        glVertex3dv(topRight);
                        glVertex3dv(topLeft);
                        glEnd();
                        
                        
                    }
                }
            }
        }
    }
    
    if (isSelect) {
        CaretAssert(annotationID);
        CaretAssert((annotationsDrawnForSelection.size() * 3) == annotationSelectionWindowXYZ.size());
        int32_t annotationIndex = -1;
        float depth = -1.0;
        m_brainOpenGLFixedPipeline->getIndexFromColorSelection(SelectionItemDataTypeEnum::ANNOTATION,
                                                               m_brainOpenGLFixedPipeline->mouseX,
                                                               m_brainOpenGLFixedPipeline->mouseY,
                                                               annotationIndex,
                                                               depth);
        
        
        if (annotationIndex >= 0) {
            if (annotationID != NULL) {
                if (annotationID->isOtherScreenDepthCloserToViewer(depth)) {
                    CaretAssertVectorIndex(annotationsDrawnForSelection, annotationIndex);
                    annotationID->setAnnotation(annotationsDrawnForSelection[annotationIndex]);
                    annotationID->setBrain(m_brainOpenGLFixedPipeline->m_brain);
                    CaretAssertVectorIndex(annotationSelectionWindowXYZ, annotationIndex * 3 + 2);
                    annotationID->setScreenXYZ(&annotationSelectionWindowXYZ[annotationIndex * 3]);
                    annotationID->setScreenDepth(depth);
                    CaretLogFine("Selected Annotation: " + annotationsDrawnForSelection[annotationIndex]->toString());
                }
                else {
                    CaretLogFine("Rejecting Selected Triangle but still using: " + annotationsDrawnForSelection[annotationIndex]->toString());
                }
            }
        }
        
    }
    
    glShadeModel(savedShadeModel);
    
    /*
     * Restore the matrices since we were drawing in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
