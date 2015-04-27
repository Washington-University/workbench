
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

#include <cmath>

#define __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

#include "AnnotationArrow.h"
#include "AnnotationBox.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationOval.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "IdentificationWithColor.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
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
 * Get the window coordinate for display of the annotation.
 *
 * @param coordinate
 *     The annotation coordinate whose window coordinate is computed.
 * @param surfaceDisplayed
 *     Surface that is displayed (may be NULL !)
 * @param windowXYZOut
 *     Output containing the window coordinate.
 * @return
 *     True if the window coordinate is valid, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::getAnnotationWindowCoordinate(const AnnotationCoordinate* coordinate,
                                                                         const AnnotationCoordinateSpaceEnum::Enum annotationCoordSpace,
                                                                          const Surface* surfaceDisplayed,
                                                                          double windowXYZOut[3]) const
{
    double modelXYZ[3]  = { 0.0, 0.0, 0.0 };
    bool modelXYZValid = false;
    
    double windowXYZ[3] = { 0.0, 0.0, 0.0 };
    bool windowXYZValid = false;
    
    float annotationXYZ[3];
    coordinate->getXYZ(annotationXYZ);
    
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
                coordinate->getSurfaceSpace(structure,
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
                       m_modelSpaceModelMatrix, m_modelSpaceProjectionMatrix, m_modelSpaceViewport,
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
 * Get the bounds for a two-dimensional shape annotation.
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
BrainOpenGLAnnotationDrawingFixedPipeline::getAnnotationTwoDimShapeBounds(const AnnotationTwoDimensionalShape* annotation2D,
                                                                          const GLint viewport[4],
                                                                          const double windowXYZ[3],
                                                                          double bottomLeftOut[3],
                                                                          double bottomRightOut[3],
                                                                          double topRightOut[3],
                                                                          double topLeftOut[3]) const
{
    const double viewportWidth  = viewport[2];
    const double viewportHeight = viewport[3];
    
    bool boundsValid = false;
    const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation2D);
    if (textAnnotation != NULL) {
        m_brainOpenGLFixedPipeline->textRenderer->getBoundsForTextAtViewportCoords(*textAnnotation,
                                                                                   windowXYZ[0], windowXYZ[1], windowXYZ[2],
                                                                                   bottomLeftOut, bottomRightOut, topRightOut, topLeftOut);
        
        boundsValid       = true;
    }
    else {
        /*
         * NOTE: Annotation's height and width are 'relative' ([0.0, 1.0] percentage) of window size.
         */
        const double halfWidth  = (annotation2D->getWidth()  / 2.0) * viewportWidth;
        const double halfHeight = (annotation2D->getHeight() / 2.0) * viewportHeight;
        
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
        
        applyRotationToShape(annotation2D->getRotationAngle(),
                             bottomLeftOut,
                             bottomRightOut,
                             topRightOut,
                             topLeftOut);
        
        boundsValid       = true;
    }
    
    return boundsValid;
}

/**
 * Apply rotation to the shape's bounding coordinates.
 *
 * @param rotationAngle
 *     The rotation angle.
 * @param bottomLeftOut
 *     The bottom left corner of the annotation bounds.
 * @param bottomRightOut
 *     The bottom right corner of the annotation bounds.
 * @param topRightOut
 *     The top right corner of the annotation bounds.
 * @param topLeftOut
 *     The top left corner of the annotation bounds.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::applyRotationToShape(const double rotationAngle,
                                                                double bottomLeftOut[3],
                                                                double bottomRightOut[3],
                                                                double topRightOut[3],
                                                                double topLeftOut[3]) const
{
    if (rotationAngle != 0) {
        Matrix4x4 matrix;
        matrix.translate(-bottomLeftOut[0], -bottomLeftOut[1], -bottomLeftOut[2]);
        matrix.rotateZ(-rotationAngle);
        matrix.translate(bottomLeftOut[0], bottomLeftOut[1], bottomLeftOut[2]);
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
    }
}

void
BrainOpenGLAnnotationDrawingFixedPipeline::applyRotationToPoints(const double rotationAngle,
                                                                 const double windowXYZ[3],
                                                                 const std::vector<double*>& points3D) const
{
    if (rotationAngle == 0.0) {
        return;
    }
    
    Matrix4x4 matrix;
    matrix.translate(-windowXYZ[0], -windowXYZ[1], -windowXYZ[2]);
    matrix.rotateZ(-rotationAngle);
    matrix.translate(windowXYZ[0], windowXYZ[1], windowXYZ[2]);
    
    for (std::vector<double*>::const_iterator iter = points3D.begin();
         iter != points3D.end();
         iter++) {
        double* p3D = *iter;
        matrix.multiplyPoint3(p3D);
    }
}

/**
 * Draw the annotations in the given coordinate space.
 *
 * @param drawingCoordinateSpace
 *     Coordinate space of annotation that are drawn.
 * @param surfaceDisplayed
 *     Surface that is displayed.  May be NULL in some instances.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotations(const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
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
    glGetDoublev(GL_MODELVIEW_MATRIX,
                 m_modelSpaceModelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX,
                 m_modelSpaceProjectionMatrix);
    glGetIntegerv(GL_VIEWPORT,
                  m_modelSpaceViewport);

    /*
     * All drawing is in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, m_modelSpaceViewport[2],
            0.0, m_modelSpaceViewport[3],
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
            
            AnnotationOneDimensionalShape* oneDimAnn = dynamic_cast<AnnotationOneDimensionalShape*>(annotation);
            AnnotationTwoDimensionalShape* twoDimAnn = dynamic_cast<AnnotationTwoDimensionalShape*>(annotation);
            
            /*
             * Limit drawing of annotations to those in the
             * selected coordinate space.
             */
            const AnnotationCoordinateSpaceEnum::Enum annotationCoordinateSpace = annotation->getCoordinateSpace();
            if (annotationCoordinateSpace != drawingCoordinateSpace) {
                continue;
            }
            
            if (oneDimAnn != NULL) {
            }
            else if (twoDimAnn != NULL) {
            }
            else {
                CaretAssertMessage(0, ("Annotation is not derived from One or Two Dim Annotation classes: "
                                       + annotation->toString()));
                continue;
            }
            
            /*
             * Skip annotation in a different window
             */
            if (annotationCoordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW) {
                const int32_t annotationWindowIndex = annotation->getWindowIndex();
                if ((annotationWindowIndex < 0)
                    || (annotationWindowIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
                    CaretLogSevere("Annotation has invalid window index="
                                   + AString::number(annotationWindowIndex)
                                   + " "
                                   + annotation->toString());
                }
                if (m_brainOpenGLFixedPipeline->windowIndex != annotationWindowIndex) {
                    continue;
                }
            }
            
            /*
             * Skip annotations in a different tab
             */
            if (annotationCoordinateSpace == AnnotationCoordinateSpaceEnum::TAB) {
                const int32_t annotationTabIndex = annotation->getTabIndex();
                if ((annotationTabIndex < 0)
                    || (annotationTabIndex >= BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                    CaretLogSevere("Annotation has invalid tab index="
                                   + AString::number(annotationTabIndex)
                                   + " "
                                   + annotation->toString());
                }
                if (m_brainOpenGLFixedPipeline->windowTabIndex != annotationTabIndex) {
                    continue;
                }
            }
            
            uint8_t idColorRGBA[4] = { 0, 0, 0, 0 };
            
            if (isSelect) {
                /*
                 * Each annotations is drawn as a solid color and the color of the
                 * selected pixel identifies the annotation.
                 */
                const int32_t annotationDrawnIndex = static_cast<int32_t>(annotationsDrawnForSelection.size());
                m_brainOpenGLFixedPipeline->colorIdentification->addItem(idColorRGBA,
                                                                         SelectionItemDataTypeEnum::ANNOTATION,
                                                                         annotationDrawnIndex);
                annotationsDrawnForSelection.push_back(annotation);
            }
            
            double selectionCenterXYZ[3];
            
            switch (annotation->getType()) {
                case AnnotationTypeEnum::ARROW:
                    drawArrow(dynamic_cast<AnnotationArrow*>(annotation),
                              surfaceDisplayed,
                              idColorRGBA,
                              isSelect,
                              selectionCenterXYZ);
                    break;
                case AnnotationTypeEnum::BOX:
                    drawBox(dynamic_cast<AnnotationBox*>(annotation),
                              surfaceDisplayed,
                              idColorRGBA,
                              isSelect,
                              selectionCenterXYZ);
                    break;
                case AnnotationTypeEnum::IMAGE:
                    CaretAssertToDoFatal();
                    break;
                case AnnotationTypeEnum::LINE:
                    drawLine(dynamic_cast<AnnotationLine*>(annotation),
                             surfaceDisplayed,
                             idColorRGBA,
                             isSelect,
                             selectionCenterXYZ);
                    break;
                case AnnotationTypeEnum::OVAL:
                    drawOval(dynamic_cast<AnnotationOval*>(annotation),
                             surfaceDisplayed,
                             idColorRGBA,
                             isSelect,
                             selectionCenterXYZ);
                    break;
                case AnnotationTypeEnum::TEXT:
                    drawText(dynamic_cast<AnnotationText*>(annotation),
                             surfaceDisplayed,
                             idColorRGBA,
                             isSelect,
                             selectionCenterXYZ);
                    break;
            }
            
            if (isSelect) {
                annotationSelectionWindowXYZ.push_back(selectionCenterXYZ[0]);
                annotationSelectionWindowXYZ.push_back(selectionCenterXYZ[1]);
                annotationSelectionWindowXYZ.push_back(selectionCenterXYZ[2]);
            }
        }
    }
    
            
            
            
            
            
            
            
            
//            double windowXYZ[3]  = { 0.0, 0.0, 0.0 };
//            const bool windowXYZValid = getAnnotationWindowCoordinate(annotation,
//                                                                       surfaceDisplayed,
//                                                                       windowXYZ);
//            
//            
//            
//            if (windowXYZValid) {
//                const double windowDoubleXYZ[3] = {
//                    windowXYZ[0],
//                    windowXYZ[1],
//                    windowXYZ[2]
//                };
//                double bottomLeft[3]  = { 0.0, 0.0, 0.0 };
//                double bottomRight[3] = { 0.0, 0.0, 0.0 };
//                double topRight[3]    = { 0.0, 0.0, 0.0 };
//                double topLeft[3]     = { 0.0, 0.0, 0.0 };
//                
//                const bool boundsValid = getAnnotationBounds(annotation,
//                                                             m_modelSpaceViewport,
//                                                             windowXYZ,
//                                                             bottomLeft,
//                                                             bottomRight,
//                                                             topRight,
//                                                             topLeft);
//                if ( ! boundsValid) {
//                    continue;
//                }
//                
//                uint8_t idColorRGBA[4] = { 0, 0, 0, 0 };
//                
//                if (isSelect) {
//                    /*
//                     * Each annotations is drawn as a solid color and the color of the
//                     * selected pixel identifies the annotation.
//                     */
//                    const int32_t annotationDrawnIndex = static_cast<int32_t>(annotationsDrawnForSelection.size());
//                    m_brainOpenGLFixedPipeline->colorIdentification->addItem(idColorRGBA,
//                                                                             SelectionItemDataTypeEnum::ANNOTATION,
//                                                                             annotationDrawnIndex);
//                    annotationsDrawnForSelection.push_back(annotation);
//                    
//                    bool drawBoxFlag = true;
//                    switch (annotation->getType()) {
//                        case AnnotationTypeEnum::ARROW:
//                        case AnnotationTypeEnum::BOX:
//                        case AnnotationTypeEnum::IMAGE:
//                            break;
//                        case AnnotationTypeEnum::LINE:
//                        {
//                            drawLine(dynamic_cast<AnnotationLine*>(annotation),
//                                     surfaceDisplayed,
//                                     idColorRGBA,
//                                     isSelect);
//                            drawBoxFlag = false;
//                        }
//                            break;
//                        case AnnotationTypeEnum::OVAL:
//                        case AnnotationTypeEnum::TEXT:
//                            break;
//                    }
//                    /*
//                     * Draw a polygon that covers annotation with selection color
//                     */
//                    if (drawBoxFlag) {
//                        glColor3ubv(idColorRGBA);
//                        glBegin(GL_POLYGON);
//                        glVertex3dv(bottomLeft);
//                        glVertex3dv(bottomRight);
//                        glVertex3dv(topRight);
//                        glVertex3dv(topLeft);
//                        glEnd();
//                    }
//                    
////                    std::cout << "Bounds BL:" << qPrintable(AString::fromNumbers(bottomLeft, 3, ","))
////                    << "  TR: " << qPrintable(AString::fromNumbers(topRight, 3, ",")) << std::endl;
////                    if ((m_brainOpenGLFixedPipeline->mouseX > bottomLeft[0])
////                        && (m_brainOpenGLFixedPipeline->mouseX < topRight[0])
////                        && (m_brainOpenGLFixedPipeline->mouseY > bottomLeft[1])
////                        && (m_brainOpenGLFixedPipeline->mouseY < topRight[1])) {
////                        std::cout << "  INSIDE" << std::endl;
////                    }
//                    
//                    annotationSelectionWindowXYZ.push_back(windowXYZ[0]);
//                    annotationSelectionWindowXYZ.push_back(windowXYZ[1]);
//                    annotationSelectionWindowXYZ.push_back(windowXYZ[2]);
//                }
//                else {
//                    switch (annotation->getType()) {
//                        case AnnotationTypeEnum::ARROW:
//                        {
//                            /*
//                             * Start and end of arrow's long line
//                             */
//                            const double lineStart[3] = {
//                                (bottomLeft[0] + bottomRight[0]) / 2.0,
//                                (bottomLeft[1] + bottomRight[1]) / 2.0,
//                                (bottomLeft[2] + bottomRight[2]) / 2.0
//                            };
//                            const double lineEnd[3] = {
//                                (topLeft[0] + topRight[0]) / 2.0,
//                                (topLeft[1] + topRight[1]) / 2.0,
//                                (topLeft[2] + topRight[2]) / 2.0
//                            };
//                            
//                            /*
//                             * Length of arrow's line
//                             */
//                            const double lineLength = MathFunctions::distance3D(lineStart,
//                                                                                   lineEnd);
//                            
//                            /*
//                             * Length of arrow's right and left pointer tips
//                             */
//                            const double pointerPercent = 0.2;
//                            const double tipLength = lineLength * pointerPercent;
//                            
//                            /*
//                             * Pointer on arrow's line that is between the arrow's left and right arrow tips
//                             */
//                            double endToStartVector[3];
//                            MathFunctions::createUnitVector(lineEnd, lineStart, endToStartVector);
//                            const double arrowTipsOnLine[3] = {
//                                lineEnd[0] + endToStartVector[0] * tipLength,
//                                lineEnd[1] + endToStartVector[1] * tipLength,
//                                lineEnd[2] + endToStartVector[2] * tipLength
//                            };
//                            
//                            /*
//                             * Vector for arrow tip's on left and right
//                             */
//                            double leftRightTipOffset[3];
//                            MathFunctions::createUnitVector(bottomLeft, bottomRight, leftRightTipOffset);
//                            leftRightTipOffset[0] *= tipLength;
//                            leftRightTipOffset[1] *= tipLength;
//                            leftRightTipOffset[2] *= tipLength;
//                            
//                            /*
//                             * Tip of arrow's pointer on the right
//                             */
//                            const double rightTipEnd[3] = {
//                                arrowTipsOnLine[0] + leftRightTipOffset[0],
//                                arrowTipsOnLine[1] + leftRightTipOffset[1],
//                                arrowTipsOnLine[2] + leftRightTipOffset[2]
//                            };
//                            
//                            /*
//                             * Tip of arrow's pointer on the left
//                             */
//                            const double leftTipEnd[3] = {
//                                arrowTipsOnLine[0] - leftRightTipOffset[0],
//                                arrowTipsOnLine[1] - leftRightTipOffset[1],
//                                arrowTipsOnLine[2] - leftRightTipOffset[2]
//                            };
//                            
//                            
//                            const double lineWidth = 2.0;
//                            
//                            float backgroundRGBA[4];
//                            annotation->getBackgroundColor(backgroundRGBA);
//                            if (backgroundRGBA[3] > 0) {
//                                glColor3fv(backgroundRGBA);
//                                glLineWidth(lineWidth * 3.0);
//                                glBegin(GL_LINES);
//                                glVertex3dv(lineStart);
//                                glVertex3dv(lineEnd);
//                                glVertex3dv(lineEnd);
//                                glVertex3dv(rightTipEnd);
//                                glVertex3dv(lineEnd);
//                                glVertex3dv(leftTipEnd);
//                                glEnd();
//                            }
//                            
//                            float foregroundRGBA[4];
//                            annotation->getForegroundColor(foregroundRGBA);
//                            glColor3fv(foregroundRGBA);
//                            glLineWidth(lineWidth);
//                            glBegin(GL_LINES);
//                            glVertex3dv(lineStart);
//                            glVertex3dv(lineEnd);
//                            glVertex3dv(lineEnd);
//                            glVertex3dv(rightTipEnd);
//                            glVertex3dv(lineEnd);
//                            glVertex3dv(leftTipEnd);
//                            glEnd();
//                        }
//                            break;
//                        case AnnotationTypeEnum::BOX:
//                        {
//                            float backgroundRGBA[4];
//                            annotation->getBackgroundColor(backgroundRGBA);
//                            if (backgroundRGBA[3] > 0) {
//                                glColor3fv(backgroundRGBA);
//                                glBegin(GL_POLYGON);
//                                glVertex3dv(bottomLeft);
//                                glVertex3dv(bottomRight);
//                                glVertex3dv(topRight);
//                                glVertex3dv(topLeft);
//                                glEnd();
//                            }
//                            
//                            float foregroundRGBA[4];
//                            annotation->getForegroundColor(foregroundRGBA);
//                            glColor3fv(foregroundRGBA);
//                            glLineWidth(2.0);
//                            glBegin(GL_LINE_LOOP);
//                            glVertex3dv(bottomLeft);
//                            glVertex3dv(bottomRight);
//                            glVertex3dv(topRight);
//                            glVertex3dv(topLeft);
//                            glEnd();
//                        }
//                            break;
//                        case AnnotationTypeEnum::IMAGE:
//                            CaretAssertToDoFatal();
//                            break;
//                        case AnnotationTypeEnum::LINE:
//                        {
//                            double dummyXYZ[3];
//                            drawLine(annotation,
//                                     windowDoubleXYZ,
//                                     modelSpaceViewport[2],
//                                     modelSpaceViewport[3],
//                                     idColorRGBA,
//                                     isSelect,
//                                     dummyXYZ);
////                            float backgroundRGBA[4];
////                            annotation->getBackgroundColor(backgroundRGBA);
////                            if (backgroundRGBA[3] > 0) {
////                                glColor3fv(backgroundRGBA);
////                                glBegin(GL_POLYGON);
////                                glVertex3dv(bottomLeft);
////                                glVertex3dv(bottomRight);
////                                glVertex3dv(topRight);
////                                glVertex3dv(topLeft);
////                                glEnd();
////                            }
////                            
////                            float foregroundRGBA[4];
////                            annotation->getForegroundColor(foregroundRGBA);
////                            glColor3fv(foregroundRGBA);
////                            const double lineStart[3] = {
////                                (bottomLeft[0] + bottomRight[0]) / 2.0,
////                                (bottomLeft[1] + bottomRight[1]) / 2.0,
////                                (bottomLeft[2] + bottomRight[2]) / 2.0
////                            };
////                            const double lineEnd[3] = {
////                                (topLeft[0] + topRight[0]) / 2.0,
////                                (topLeft[1] + topRight[1]) / 2.0,
////                                (topLeft[2] + topRight[2]) / 2.0
////                            };
////                            glLineWidth(2.0);
////                            glBegin(GL_LINES);
////                            glVertex3dv(lineStart);
////                            glVertex3dv(lineEnd);
////                            glEnd();
//                        }
//                            break;
//                        case AnnotationTypeEnum::OVAL:
//                        {
//                            const double majorAxis = annotation->getWidth2D() * modelSpaceViewport[2];
//                            const double minorAxis = annotation->getHeight2D() * modelSpaceViewport[2];
//                            const double rotationAngle = annotation->getRotationAngle();
//                            
//                            glPushMatrix();
//                            glTranslatef(windowXYZ[0], windowXYZ[1], windowXYZ[2]);
//                            if (rotationAngle != 0.0) {
//                                glRotated(-rotationAngle, 0.0, 0.0, 1.0);
//                            }
//                            uint8_t backgroundRGBA[4];
//                            annotation->getBackgroundColor(backgroundRGBA);
//                            if (backgroundRGBA[3] > 0) {
//                                m_brainOpenGLFixedPipeline->drawEllipseFilled(backgroundRGBA,
//                                                                              majorAxis,
//                                                                              minorAxis);
//                            }
//                            
//                            uint8_t foregroundRGBA[4];
//                            annotation->getForegroundColor(foregroundRGBA);
//                            m_brainOpenGLFixedPipeline->drawEllipseOutline(foregroundRGBA,
//                                                                           majorAxis,
//                                                                           minorAxis);
//                            
//                            glPopMatrix();
//                        }
//                            break;
//                        case AnnotationTypeEnum::TEXT:
//                        {
//                            const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation);
//                            CaretAssert(textAnnotation);
//                            
//                                m_brainOpenGLFixedPipeline->textRenderer->drawTextAtViewportCoords(windowXYZ[0],
//                                                                                                   windowXYZ[1],
//                                                                                                   windowXYZ[2],
//                                                                                                   *textAnnotation);
//                        }
//                            break;
//                    }
//                    
//                    /*
//                     * Is this the selected annotation ?
//                     */
//                    if (annotation->isSelected()) {
//                        /*
//                         * Draw a polygon that frames annotation with foreground color
//                         */
//                        glColor3fv(annotation->getForegroundColor());
//                        glLineWidth(4.0);
//                        glBegin(GL_LINE_LOOP);
//                        glVertex3dv(bottomLeft);
//                        glVertex3dv(bottomRight);
//                        glVertex3dv(topRight);
//                        glVertex3dv(topLeft);
//                        glEnd();
//                        
//                        
//                    }
//                }
//            }
//        }
//    }
    
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

/**
 * Draw an annotation arrow.
 *
 * @param arrow
 *    Annotation arrow to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @param selectionColorRGBA
 *    Color used for selection identification.
 * @param selectionFlag
 *    True when selecting, else false.
 * @param selectionCenterXYZOut
 *    On exit contains center of annotation used by selection logic.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawArrow(const AnnotationArrow* arrow,
                                                     const Surface* surfaceDisplayed,
                                                     const uint8_t selectionColorRGBA[4],
                                                     const bool selectionFlag,
                                                     double selectionCenterXYZOut[3])
{
    CaretAssert(arrow);
    CaretAssert(arrow->getType() == AnnotationTypeEnum::ARROW);
    
    double arrowStartXYZ[3];
    double arrowEndXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(arrow->getStartCoordinate(),
                                         arrow->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         arrowStartXYZ)) {
        return;
    }
    if ( ! getAnnotationWindowCoordinate(arrow->getEndCoordinate(),
                                         arrow->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         arrowEndXYZ)) {
        return;
    }
    const double lineWidth = 2.0;
    
//    bool drawBackgroundFlag = false;
//    if (selectionFlag) {
//        glColor3ubv(selectionColorRGBA);
//        drawBackgroundFlag = true;
//    }
//    else {
//        float backgroundRGBA[4];
//        arrow->getBackgroundColor(backgroundRGBA);
//        if (backgroundRGBA[3] > 0.0) {
//            glColor3fv(backgroundRGBA);
//            drawBackgroundFlag = true;
//        }
//    }
//    if (drawBackgroundFlag) {
//        glLineWidth(lineWidth * 3.0);
//        glBegin(GL_LINES);
//        glVertex3dv(arrowStartXYZ);
//        glVertex3dv(arrowEndXYZ);
//        glEnd();
//    }
//    
//    if ( ! selectionFlag) {
//        float foregroundRGBA[4];
//        arrow->getForegroundColor(foregroundRGBA);
//        glLineWidth(lineWidth);
//        glColor3fv(foregroundRGBA);
//        glBegin(GL_LINES);
//        glVertex3dv(arrowStartXYZ);
//        glVertex3dv(arrowEndXYZ);
//        glEnd();
//    }
    
    selectionCenterXYZOut[0] = (arrowStartXYZ[0] + arrowEndXYZ[0]) / 2.0;
    selectionCenterXYZOut[1] = (arrowStartXYZ[1] + arrowEndXYZ[1]) / 2.0;
    selectionCenterXYZOut[2] = (arrowStartXYZ[2] + arrowEndXYZ[2]) / 2.0;
    //                            /*
    //                             * Start and end of arrow's long line
    //                             */
    //                            const double lineStart[3] = {
    //                                (bottomLeft[0] + bottomRight[0]) / 2.0,
    //                                (bottomLeft[1] + bottomRight[1]) / 2.0,
    //                                (bottomLeft[2] + bottomRight[2]) / 2.0
    //                            };
    //                            const double lineEnd[3] = {
    //                                (topLeft[0] + topRight[0]) / 2.0,
    //                                (topLeft[1] + topRight[1]) / 2.0,
    //                                (topLeft[2] + topRight[2]) / 2.0
    //                            };
    //
    
    /*
     * Length of arrow's line
     */
    const double lineLength = MathFunctions::distance3D(arrowStartXYZ,
                                                        arrowEndXYZ);
    
    /*
     * Length of arrow's right and left pointer tips
     */
    const double pointerPercent = 0.2;
    const double tipLength = lineLength * pointerPercent;
    
    /*
     * Pointer on arrow's line that is between the arrow's left and right arrow tips
     */
    double endToStartVector[3];
    MathFunctions::createUnitVector(arrowEndXYZ, arrowStartXYZ, endToStartVector);
    const double arrowTipsOnLine[3] = {
        arrowEndXYZ[0] + endToStartVector[0] * tipLength,
        arrowEndXYZ[1] + endToStartVector[1] * tipLength,
        arrowEndXYZ[2] + endToStartVector[2] * tipLength
    };
    
    /*
     * Vector for arrow tip's on left and right
     *
     * Create a perpendicular vector by swapping first two elements
     * and negating the second element.
     */
    double leftRightTipOffset[3] = {
        endToStartVector[0],
        endToStartVector[1],
        endToStartVector[2]
    };
    MathFunctions::normalizeVector(leftRightTipOffset);
    std::swap(leftRightTipOffset[0],
              leftRightTipOffset[1]);
    leftRightTipOffset[1] *= -1;
    leftRightTipOffset[0] *= tipLength;
    leftRightTipOffset[1] *= tipLength;
    leftRightTipOffset[2] *= tipLength;
    
    /*
     * Tip of arrow's pointer on the right
     */
    const double rightTipEnd[3] = {
        arrowTipsOnLine[0] - leftRightTipOffset[0],
        arrowTipsOnLine[1] - leftRightTipOffset[1],
        arrowTipsOnLine[2] - leftRightTipOffset[2]
    };
    
    /*
     * Tip of arrow's pointer on the left
     */
    const double leftTipEnd[3] = {
        arrowTipsOnLine[0] + leftRightTipOffset[0],
        arrowTipsOnLine[1] + leftRightTipOffset[1],
        arrowTipsOnLine[2] + leftRightTipOffset[2]
    };
    
    
//    const double lineWidth = 2.0;
    
    bool drawBackgroundFlag = false;
    if (selectionFlag) {
        glColor3ubv(selectionColorRGBA);
        drawBackgroundFlag = true;
    }
    else {
        float backgroundRGBA[4];
        arrow->getBackgroundColorRGBA(backgroundRGBA);
        if (backgroundRGBA[3] > 0.0) {
            glColor3fv(backgroundRGBA);
            drawBackgroundFlag = true;
        }
    }
    if (drawBackgroundFlag) {
        glLineWidth(lineWidth * 3.0);
        glBegin(GL_LINES);
        glVertex3dv(arrowStartXYZ);
        glVertex3dv(arrowEndXYZ);
        glVertex3dv(arrowEndXYZ);
        glVertex3dv(rightTipEnd);
        glVertex3dv(arrowEndXYZ);
        glVertex3dv(leftTipEnd);
        glEnd();
    }
    
    
    if ( ! selectionFlag) {
        float foregroundRGBA[4];
        arrow->getForegroundColorRGBA(foregroundRGBA);
        glColor3fv(foregroundRGBA);
        if (arrow->isSelected()) {
            glLineWidth(lineWidth * 2.0);
        }
        else {
            glLineWidth(lineWidth);
        }
        glBegin(GL_LINES);
        glVertex3dv(arrowStartXYZ);
        glVertex3dv(arrowEndXYZ);
        glVertex3dv(arrowEndXYZ);
        glVertex3dv(rightTipEnd);
        glVertex3dv(arrowEndXYZ);
        glVertex3dv(leftTipEnd);
        glEnd();
    }
}

/**
 * Draw an annotation box.
 *
 * @param box
 *    box to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @param selectionColorRGBA
 *    Color used for selection identification.
 * @param selectionFlag
 *    True when selecting, else false.
 * @param selectionCenterXYZOut
 *    On exit contains center of annotation used by selection logic.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawBox(const AnnotationBox* box,
                                                   const Surface* surfaceDisplayed,
                                                   const uint8_t selectionColorRGBA[4],
                                                   const bool selectionFlag,
                                                   double selectionCenterXYZOut[3])
{
    CaretAssert(box);
    CaretAssert(box->getType() == AnnotationTypeEnum::BOX);
    bool drawBackgroundFlag = false;
    
    if (selectionFlag) {
        drawBackgroundFlag = true;
        glColor3ubv(selectionColorRGBA);
    }
    else {
        float backgroundRGBA[4];
        box->getBackgroundColorRGBA(backgroundRGBA);
        if (backgroundRGBA[3] > 0.0) {
            glColor3fv(backgroundRGBA);
            drawBackgroundFlag = true;
        }
    }
    
    
    double windowXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(box->getCoordinate(),
                                         box->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    double bottomLeft[3];
    double bottomRight[3];
    double topRight[3];
    double topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(box, m_modelSpaceViewport, windowXYZ,
                               bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    if (drawBackgroundFlag) {
        glBegin(GL_POLYGON);
        glVertex3dv(bottomLeft);
        glVertex3dv(bottomRight);
        glVertex3dv(topRight);
        glVertex3dv(topLeft);
        glEnd();
        
        selectionCenterXYZOut[0] = (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0;
        selectionCenterXYZOut[1] = (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0;
        selectionCenterXYZOut[2] = (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0;
    }
    
    if ( ! selectionFlag) {
        float foregroundRGBA[4];
        box->getForegroundColorRGBA(foregroundRGBA);
        glColor3fv(foregroundRGBA);
        if (box->isSelected()) {
            glLineWidth(4.0);
        }
        else {
            glLineWidth(2.0);
        }
        glBegin(GL_LINE_LOOP);
        glVertex3dv(bottomLeft);
        glVertex3dv(bottomRight);
        glVertex3dv(topRight);
        glVertex3dv(topLeft);
        glEnd();
    }
}

/**
 * Draw an annotation oval.
 *
 * @param oval
 *    Annotation oval to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @param selectionColorRGBA
 *    Color used for selection identification.
 * @param selectionFlag
 *    True when selecting, else false.
 * @param selectionCenterXYZOut
 *    On exit contains center of annotation used by selection logic.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawOval(const AnnotationOval* oval,
                                                    const Surface* surfaceDisplayed,
                                                    const uint8_t selectionColorRGBA[4],
                                                    const bool selectionFlag,
                                                    double selectionCenterXYZOut[3])
{
    CaretAssert(oval);
    CaretAssert(oval->getType() == AnnotationTypeEnum::OVAL);
    
    bool drawBackgroundFlag = false;
    
    uint8_t backgroundRGBA[4];
    if (selectionFlag) {
        drawBackgroundFlag = true;
        backgroundRGBA[0] = selectionColorRGBA[0];
        backgroundRGBA[1] = selectionColorRGBA[1];
        backgroundRGBA[2] = selectionColorRGBA[2];
        backgroundRGBA[3] = 255;
    }
    else {
        oval->getBackgroundColorRGBA(backgroundRGBA);
        if (backgroundRGBA[3] > 0) {
            drawBackgroundFlag = true;
        }
    }
    
    
    double windowXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(oval->getCoordinate(),
                                         oval->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    double bottomLeft[3];
    double bottomRight[3];
    double topRight[3];
    double topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(oval, m_modelSpaceViewport, windowXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    const double majorAxis = oval->getWidth() * m_modelSpaceViewport[2];
    const double minorAxis = oval->getHeight() * m_modelSpaceViewport[2];
    const double rotationAngle = oval->getRotationAngle();
    
    glPushMatrix();
    glTranslatef(windowXYZ[0], windowXYZ[1], windowXYZ[2]);
    if (rotationAngle != 0.0) {
        glRotated(-rotationAngle, 0.0, 0.0, 1.0);
    }
    
    if (drawBackgroundFlag) {
        m_brainOpenGLFixedPipeline->drawEllipseFilled(backgroundRGBA,
                                                      majorAxis,
                                                      minorAxis);
        
        selectionCenterXYZOut[0] = (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0;
        selectionCenterXYZOut[1] = (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0;
        selectionCenterXYZOut[2] = (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0;
    }
    
    if ( ! selectionFlag) {
        uint8_t foregroundRGBA[4];
        oval->getForegroundColorRGBA(foregroundRGBA);
        if (oval->isSelected()) {
            m_brainOpenGLFixedPipeline->drawEllipseOutline(foregroundRGBA,
                                                           majorAxis+2,
                                                           minorAxis+2);
        }
        else {
            m_brainOpenGLFixedPipeline->drawEllipseOutline(foregroundRGBA,
                                                           majorAxis,
                                                           minorAxis);
        }
    }
    
    glPopMatrix();
}

/**
 * Draw an annotation text.
 *
 * @param text
 *    Annotation text to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @param selectionColorRGBA
 *    Color used for selection identification.
 * @param selectionFlag
 *    True when selecting, else false.
 * @param selectionCenterXYZOut
 *    On exit contains center of annotation used by selection logic.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawText(const AnnotationText* text,
                                                    const Surface* surfaceDisplayed,
                                                    const uint8_t selectionColorRGBA[4],
                                                    const bool selectionFlag,
                                                    double selectionCenterXYZOut[3])
{
    CaretAssert(text);
    CaretAssert(text->getType() == AnnotationTypeEnum::TEXT);
    
    bool drawBackgroundFlag = false;
    
    if (selectionFlag) {
        drawBackgroundFlag = true;
        glColor3ubv(selectionColorRGBA);
    }
    else {
        float backgroundRGBA[4];
        text->getBackgroundColorRGBA(backgroundRGBA);
        if (backgroundRGBA[3] > 0.0) {
            glColor3fv(backgroundRGBA);
            drawBackgroundFlag = true;
        }
    }
    

    double windowXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(text->getCoordinate(),
                                         text->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    double bottomLeft[3];
    double bottomRight[3];
    double topRight[3];
    double topLeft[3];
    m_brainOpenGLFixedPipeline->textRenderer->getBoundsForTextAtViewportCoords(*text,
                                                                               windowXYZ[0], windowXYZ[1], windowXYZ[2],
                                                                               bottomLeft, bottomRight, topRight, topLeft);
    
    if (drawBackgroundFlag) {
        glBegin(GL_POLYGON);
        glVertex3dv(bottomLeft);
        glVertex3dv(bottomRight);
        glVertex3dv(topRight);
        glVertex3dv(topLeft);
        glEnd();
        
        selectionCenterXYZOut[0] = (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0;
        selectionCenterXYZOut[1] = (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0;
        selectionCenterXYZOut[2] = (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0;
    }
    
    if ( ! selectionFlag) {
        m_brainOpenGLFixedPipeline->textRenderer->drawTextAtViewportCoords(windowXYZ[0],
                                                                           windowXYZ[1],
                                                                           windowXYZ[2],
                                                                           *text);
        
        if (text->isSelected()) {
            float rgba[4];
            text->getForegroundColorRGBA(rgba);
            glColor4fv(rgba);
            glLineWidth(3.0);
            glBegin(GL_LINE_LOOP);
            glVertex3dv(bottomLeft);
            glVertex3dv(bottomRight);
            glVertex3dv(topRight);
            glVertex3dv(topLeft);
            glEnd();
        }
    }
}

/**
 * Draw an annotation line.
 *
 * @param line
 *    Annotation line to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @param selectionColorRGBA
 *    Color used for selection identification.
 * @param selectionFlag
 *    True when selecting, else false.
 * @param selectionCenterXYZOut
 *    On exit contains center of annotation used by selection logic.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawLine(const AnnotationLine* line,
                                                    const Surface* surfaceDisplayed,
                                                    const uint8_t selectionColorRGBA[4],
                                                    const bool selectionFlag,
                                                    double selectionCenterXYZOut[3])
{
    CaretAssert(line);
    CaretAssert(line->getType() == AnnotationTypeEnum::LINE);
    
    double lineStartXYZ[3];
    double lineEndXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(line->getStartCoordinate(),
                                         line->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         lineStartXYZ)) {
        return;
    }
    if ( ! getAnnotationWindowCoordinate(line->getEndCoordinate(),
                                         line->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         lineEndXYZ)) {
        return;
    }
    const double lineWidth = 2.0;
    
    bool drawBackgroundFlag = false;
    if (selectionFlag) {
        glColor3ubv(selectionColorRGBA);
        drawBackgroundFlag = true;

        selectionCenterXYZOut[0] = (lineStartXYZ[0] + lineEndXYZ[0]) / 2.0;
        selectionCenterXYZOut[1] = (lineStartXYZ[1] + lineEndXYZ[1]) / 2.0;
        selectionCenterXYZOut[2] = (lineStartXYZ[2] + lineEndXYZ[2]) / 2.0;
    }
    else {
        float backgroundRGBA[4];
        line->getBackgroundColorRGBA(backgroundRGBA);
        if (backgroundRGBA[3] > 0.0) {
            glColor3fv(backgroundRGBA);
            drawBackgroundFlag = true;
        }
    }
    if (drawBackgroundFlag) {
        glLineWidth(lineWidth * 3.0);
        glBegin(GL_LINES);
        glVertex3dv(lineStartXYZ);
        glVertex3dv(lineEndXYZ);
        glEnd();
    }
    
    if ( ! selectionFlag) {
        float foregroundRGBA[4];
        line->getForegroundColorRGBA(foregroundRGBA);
        if (line->isSelected()) {
            glLineWidth(3.0);
        }
        else {
            glLineWidth(lineWidth);
        }
        glColor3fv(foregroundRGBA);
        glBegin(GL_LINES);
        glVertex3dv(lineStartXYZ);
        glVertex3dv(lineEndXYZ);
        glEnd();
    }
    

//    CaretAssert(line->getType() == AnnotationTypeEnum::LINE);
//    
//    /*
//     * NOTE: Annotation's length is 'relative' ([0.0, 1.0] percentage) of window size.
//     */
//    const double height    = line->getLength1D() * viewportWidth;
//    
////    const double lineStartXYZ[3] = {
////        windowStartXYZ[0],
////        windowStartXYZ[1],
////        windowStartXYZ[2]
////    };
//    
//    const double angleRadians = MathFunctions::toRadians(90.0 - line->getRotationAngle());
//    const double xEnd = std::cos(angleRadians) * viewportWidth * line->getLength1D();
//    const double yEnd = std::sin(angleRadians) * viewportHeight * line->getLength1D();
//    double lineEndXYZ[3] = {
//        windowStartXYZ[0] + xEnd,
//        windowStartXYZ[1] + yEnd,
//        windowStartXYZ[2]
//    };
//    
////    std::vector<double*> points;
////    points.push_back(lineStartXYZ);
////    points.push_back(lineEndXYZ);
////    
////    applyRotationToPoints(line->getRotationAngle(),
////                          windowStartXYZ,
////                          points);
//
//    const double lineWidth = 2.0;
//    
//    bool drawBackgroundFlag = false;
//    if (selectionFlag) {
//        glColor3ubv(selectionColorRGBA);
//        drawBackgroundFlag = true;
//    }
//    else {
//        float backgroundRGBA[4];
//        line->getBackgroundColor(backgroundRGBA);
//        if (backgroundRGBA[3] > 0.0) {
//            glColor3fv(backgroundRGBA);
//            drawBackgroundFlag = true;
//        }
//    }
//    if (drawBackgroundFlag) {
//        glLineWidth(lineWidth * 3.0);
//        glBegin(GL_LINES);
//        glVertex3dv(windowStartXYZ);
//        glVertex3dv(lineEndXYZ);
//        glEnd();
//    }
//    
//    if ( ! selectionFlag) {
//        float foregroundRGBA[4];
//        line->getForegroundColor(foregroundRGBA);
//        glLineWidth(lineWidth);
//        glColor3fv(foregroundRGBA);
//        glBegin(GL_LINES);
//        glVertex3dv(windowStartXYZ);
//        glVertex3dv(lineEndXYZ);
//        glEnd();
//    }
//    
//    windowEndXYZOut[0] = lineEndXYZ[0];
//    windowEndXYZOut[1] = lineEndXYZ[1];
//    windowEndXYZOut[2] = lineEndXYZ[2];
}

