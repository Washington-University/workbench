
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

#include <QColor>

#define __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

#include "AnnotationBox.h"
#include "AnnotationColorBar.h"
#include "AnnotationColorBarSection.h"
#include "AnnotationColorBarNumericText.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationOval.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLPrimitiveDrawing.h"
#include "BrainOpenGLShapeRing.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "BrainOpenGLTextureManager.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventBrowserTabGet.h"
#include "EventManager.h"
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
m_brainOpenGLFixedPipeline(brainOpenGLFixedPipeline),
m_inputs(NULL),
m_volumeSpacePlaneValid(false),
m_volumeSliceThickness(0.0)
{
    CaretAssert(brainOpenGLFixedPipeline);
    
    m_dummyAnnotationFile = new AnnotationFile();
    m_dummyAnnotationFile->setFileName("DummyFileForDrawing"
                                       + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::ANNOTATION));
    m_rotationHandleCircle = new BrainOpenGLShapeRing(20,
                                                      0.7,
                                                      1.0);

}

/**
 * Destructor.
 */
BrainOpenGLAnnotationDrawingFixedPipeline::~BrainOpenGLAnnotationDrawingFixedPipeline()
{
    delete m_rotationHandleCircle;
    delete m_dummyAnnotationFile;
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
 * Convert a viewport coordinate to an OpenGL window coordinate.
 *
 * @param viewportXYZ
 *     Viewport coordinate
 * @param openGLXYZOut
 *     Output OpenGL coordinate.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::viewportToOpenGLWindowCoordinate(const float viewportXYZ[3],
                                                                            float openGLXYZOut[3]) const
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    openGLXYZOut[0] = viewport[2] * (viewportXYZ[0] / 100.0);
    openGLXYZOut[1] = viewport[3] * (viewportXYZ[1] / 100.0);
    openGLXYZOut[2] = (-viewportXYZ[2] / 100.0);
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
                                                                          float windowXYZOut[3]) const
{
    float stereotaxicXYZ[3]  = { 0.0, 0.0, 0.0 };
    bool stereotaxicXYZValid = false;
    
    float windowXYZ[3] = { 0.0, 0.0, 0.0 };
    bool windowXYZValid = false;
    
    float annotationXYZ[3];
    coordinate->getXYZ(annotationXYZ);
    
    switch (annotationCoordSpace) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            stereotaxicXYZ[0] = annotationXYZ[0];
            stereotaxicXYZ[1] = annotationXYZ[1];
            stereotaxicXYZ[2] = annotationXYZ[2];
            stereotaxicXYZValid = true;
            
            if (m_volumeSpacePlaneValid) {
                float xyzFloat[3] = {
                    stereotaxicXYZ[0],
                    stereotaxicXYZ[1],
                    stereotaxicXYZ[2]
                };
                const float distToPlaneAbs = std::fabs(m_volumeSpacePlane.signedDistanceToPlane(xyzFloat));
                const float halfSliceThickness = ((m_volumeSliceThickness > 0.0)
                                                  ? (m_volumeSliceThickness / 2.0)
                                                  : 1.0);
                if (distToPlaneAbs < halfSliceThickness) { //1.5) {
                    stereotaxicXYZValid = true;
                    
                    float projectedPoint[3];
                    m_volumeSpacePlane.projectPointToPlane(stereotaxicXYZ, projectedPoint);
                }
                else {
                    stereotaxicXYZValid = false;
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            windowXYZ[0] = annotationXYZ[0];
            windowXYZ[1] = annotationXYZ[1];
            windowXYZ[2] = annotationXYZ[2];
            windowXYZValid = true;
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            if (surfaceDisplayed != NULL) {
                StructureEnum::Enum annotationStructure = StructureEnum::INVALID;
                int32_t annotationNumberOfNodes  = -1;
                int32_t annotationNodeIndex      = -1;
                float annotationOffsetLength     = AnnotationCoordinate::getDefaultSurfaceOffsetLength();
                AnnotationSurfaceOffsetVectorTypeEnum::Enum annotationOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
                coordinate->getSurfaceSpace(annotationStructure,
                                            annotationNumberOfNodes,
                                            annotationNodeIndex,
                                            annotationOffsetLength,
                                            annotationOffsetVector);
                
                const StructureEnum::Enum surfaceStructure = surfaceDisplayed->getStructure();
                const int32_t surfaceNumberOfNodes = surfaceDisplayed->getNumberOfNodes();
                if ((surfaceStructure == annotationStructure)
                    && (surfaceNumberOfNodes == annotationNumberOfNodes)) {
                    if ((annotationNodeIndex >= 0)
                        && (annotationNodeIndex < surfaceNumberOfNodes)) {
                        float nodeXYZ[3];
                        surfaceDisplayed->getCoordinate(annotationNodeIndex,
                                                        nodeXYZ);
                        stereotaxicXYZ[0] = nodeXYZ[0];
                        stereotaxicXYZ[1] = nodeXYZ[1];
                        stereotaxicXYZ[2] = nodeXYZ[2];
                        
                        
                        
                        float offsetUnitVector[3] = { 0.0, 0.0, 0.0 };
//                        const bool useNormalVectorForOffsetFlag = false;
//                        if (useNormalVectorForOffsetFlag) {
//                            const float* normalVector = surfaceDisplayed->getNormalVector(annotationNodeIndex);
//                            offsetUnitVector[0] = normalVector[0];
//                            offsetUnitVector[1] = normalVector[1];
//                            offsetUnitVector[2] = normalVector[2];
//                        }
//                        else {
//                            BoundingBox boundingBox;
//                            surfaceDisplayed->getBounds(boundingBox);
//                            float surfaceCenter[3] = { 0.0, 0.0, 0.0 };
//                            boundingBox.getCenter(surfaceCenter);
//                            
//                            MathFunctions::subtractVectors(nodeXYZ,
//                                                           surfaceCenter,
//                                                           offsetUnitVector);
//                            MathFunctions::normalizeVector(offsetUnitVector);
//                        }
                        
                        /*
                         * For a flat surface, ALWAYS use the normal vector.
                         * Using the centroid will not work as there is no
                         * "z depth" so it will incorrectly offset in the XY-plane.
                         *
                         */
                        if (surfaceDisplayed->getSurfaceType() == SurfaceTypeEnum::FLAT) {
                            annotationOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::SURACE_NORMAL;
                        }
                        
                        switch (annotationOffsetVector) {
                            case AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX:
                            {
                                BoundingBox boundingBox;
                                surfaceDisplayed->getBounds(boundingBox);
                                float surfaceCenter[3] = { 0.0, 0.0, 0.0 };
                                boundingBox.getCenter(surfaceCenter);
                                
                                MathFunctions::subtractVectors(nodeXYZ,
                                                               surfaceCenter,
                                                               offsetUnitVector);
                                MathFunctions::normalizeVector(offsetUnitVector);
                            }                                break;
                            case AnnotationSurfaceOffsetVectorTypeEnum::SURACE_NORMAL:
                                const float* normalVector = surfaceDisplayed->getNormalVector(annotationNodeIndex);
                                offsetUnitVector[0] = normalVector[0];
                                offsetUnitVector[1] = normalVector[1];
                                offsetUnitVector[2] = normalVector[2];
                                break;
                        }
                        
                        stereotaxicXYZ[0] += (offsetUnitVector[0] * annotationOffsetLength);
                        stereotaxicXYZ[1] += (offsetUnitVector[1] * annotationOffsetLength);
                        stereotaxicXYZ[2] += (offsetUnitVector[2] * annotationOffsetLength);
                        stereotaxicXYZValid = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            viewportToOpenGLWindowCoordinate(annotationXYZ, windowXYZ);
            windowXYZValid = true;
//        {
//            GLint viewport[4];
//            glGetIntegerv(GL_VIEWPORT,
//                          viewport);
//            windowXYZ[0] = viewport[2] * (annotationXYZ[0] / 100.0);
//            windowXYZ[1] = viewport[3] * (annotationXYZ[1] / 100.0);
////            windowXYZ[2] = annotationXYZ[2] / 100.0;
//            windowXYZ[2] = toWindowZ(annotationXYZ[2]);
//            windowXYZValid = true;
//        }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            viewportToOpenGLWindowCoordinate(annotationXYZ, windowXYZ);
            windowXYZValid = true;
//        {
//            GLint viewport[4];
//            glGetIntegerv(GL_VIEWPORT,
//                          viewport);
//            windowXYZ[0] = viewport[2] * (annotationXYZ[0] / 100.0);
//            windowXYZ[1] = viewport[3] * (annotationXYZ[1] / 100.0);
////            windowXYZ[2] = annotationXYZ[2] / 100.0;
//            windowXYZ[2] = toWindowZ(annotationXYZ[2]);
//            windowXYZValid = true;
//        }
            break;
    }
    
    if (stereotaxicXYZValid) {
        /*
         * Convert model space coordinates to window coordinates
         * as all annotations are drawn in window coordinates.
         */
        
        if (convertModelToWindowCoordinate(stereotaxicXYZ, windowXYZ)) {
            stereotaxicXYZValid  = false;
            windowXYZValid = true;
        }
        else {
            CaretLogSevere("Failed to convert model coordinates to window coordinates for annotation drawing.");
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
 * Convert the given model coordinate into a window coordinate.
 *
 * @param modelXYZ
 *    The model coordinate.
 * @param windowXYZOut
 *    The window coordinate
 * @return true if the conversion is valid, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::convertModelToWindowCoordinate(const float modelXYZ[3],
                                                                          float windowXYZOut[3]) const
{
    /*
     * Convert model space coordinates to window coordinates
     * as all annotations are drawn in window coordinates.
     */
    GLdouble modelDoubleXYZ[3] = { modelXYZ[0], modelXYZ[1], modelXYZ[2] };
    GLdouble windowX, windowY, windowZ;
    if (gluProject(modelDoubleXYZ[0], modelDoubleXYZ[1], modelDoubleXYZ[2],
                   m_modelSpaceModelMatrix, m_modelSpaceProjectionMatrix, m_modelSpaceViewport,
                   &windowX, &windowY, &windowZ) == GL_TRUE) {
        windowXYZOut[0] = windowX - m_modelSpaceViewport[0];
        windowXYZOut[1] = windowY - m_modelSpaceViewport[1];
        
        /*
         * From OpenGL Programming Guide 3rd Ed, p 133:
         *
         * If the near value is 1.0 and the far value is 3.0,
         * objects must have z-coordinates between -1.0 and -3.0 in order to be visible.
         * So, negate the Z-value to be negative.
         */
        windowXYZOut[2] = -windowZ;
        
        if ((m_modelSpaceProjectionMatrix[0] != 0.0)
            && (m_modelSpaceProjectionMatrix[5] != 0.0)
            && (m_modelSpaceProjectionMatrix[10] != 0.0)) {
            /*
             * From http://lektiondestages.blogspot.com/2013/11/decompose-opengl-projection-matrix.html
             */
            float nearValue   =  (1.0f + m_modelSpaceProjectionMatrix[14]) / m_modelSpaceProjectionMatrix[10];
            float farValue    = -(1.0f - m_modelSpaceProjectionMatrix[14]) / m_modelSpaceProjectionMatrix[10];
            //GLfloat bottom =  (1.0f - m_modelSpaceProjectionMatrix[13]) / m_modelSpaceProjectionMatrix[5];
            //GLfloat top    = -(1.0f + m_modelSpaceProjectionMatrix[13]) / m_modelSpaceProjectionMatrix[5];
            float left   = -(1.0f + m_modelSpaceProjectionMatrix[12]) / m_modelSpaceProjectionMatrix[0];
            float right  =  (1.0f - m_modelSpaceProjectionMatrix[12]) / m_modelSpaceProjectionMatrix[0];

            /*
             * Depending upon view, near may be positive and far negative
             */
            const float farNearRange = std::fabs(farValue - nearValue);
            if ((m_inputs->m_centerToEyeDistance > 0.0)
                && (farNearRange > 0.0)) {
                /*
                 * Using gluLookAt moves the eye away from the center which
                 * causes the window Z to also move.  Thus, we need to remove
                 * this amount from the window's Z-coordinate.
                 */
                const float eyeAdjustment = (m_inputs->m_centerToEyeDistance / farNearRange);
                if (m_volumeSpacePlaneValid) {
                    windowXYZOut[2] = -(windowZ - eyeAdjustment);
                }
                else if (left > right) {
                    windowXYZOut[2] = -(windowZ - eyeAdjustment);
                }
                else {
                    windowXYZOut[2] = -(windowZ + eyeAdjustment);
                }
//                std::cout << "Z adjusted for eye/near/far=" << windowXYZOut[2] << std::endl;
            }
        }
        
        return true;
    }
    
    return false;
}

/**
 * Get the bounds for a two-dimensional shape annotation.
 *
 * @param annotation
 *     The annotation whose bounds is computed.
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
                                                                          const float windowXYZ[3],
                                                                          float bottomLeftOut[3],
                                                                          float bottomRightOut[3],
                                                                          float topRightOut[3],
                                                                          float topLeftOut[3]) const
{
    float viewportWidth  = m_modelSpaceViewport[2];
    float viewportHeight = m_modelSpaceViewport[3];
//    if (annotation2D->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
//        viewportWidth  = m_inputs->m_tabViewport[2];
//        viewportHeight = m_inputs->m_tabViewport[3];
//    }
    
    /*
     * Only use text characters when the text is NOT empty
     */
    const AnnotationText* textAnnotation = dynamic_cast<const AnnotationText*>(annotation2D);
    bool textFlag = false;
    if (textAnnotation != NULL) {
        if ( ! textAnnotation->getText().isEmpty()) {
            textFlag = true;
        }
    }
    
    bool boundsValid = false;
    if (textFlag) {
        m_brainOpenGLFixedPipeline->getTextRenderer()->getBoundsForTextAtViewportCoords(*textAnnotation,
                                                                                   windowXYZ[0], windowXYZ[1], windowXYZ[2],
                                                                                        viewportHeight,
                                                                                   bottomLeftOut, bottomRightOut, topRightOut, topLeftOut);
        
        boundsValid = true;
    }
    else {
        boundsValid = annotation2D->getShapeBounds(viewportWidth,
                                                   viewportHeight,
                                                   windowXYZ,
                                                   bottomLeftOut,
                                                   bottomRightOut,
                                                   topRightOut,
                                                   topLeftOut);
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
BrainOpenGLAnnotationDrawingFixedPipeline::applyRotationToShape(const float rotationAngle,
                                                                const float rotationPoint[3],
                                                                float bottomLeftOut[3],
                                                                float bottomRightOut[3],
                                                                float topRightOut[3],
                                                                float topLeftOut[3]) const
{
    if (rotationAngle != 0) {
        Matrix4x4 matrix;
        matrix.translate(-rotationPoint[0], -rotationPoint[1], -rotationPoint[2]);
        matrix.rotateZ(-rotationAngle);
        matrix.translate(rotationPoint[0], rotationPoint[1], rotationPoint[2]);
        matrix.multiplyPoint3(bottomLeftOut);
        matrix.multiplyPoint3(bottomRightOut);
        matrix.multiplyPoint3(topRightOut);
        matrix.multiplyPoint3(topLeftOut);
    }
}

/**
 * Draw model space annotations on the volume slice with the given plane.
 *
 * @param inputs
 *     Inputs for drawing annotations.
 * @param plane
 *     The volume slice's plane.
 * @param sliceThickness
 *     Thickness of volume slice
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawModelSpaceAnnotationsOnVolumeSlice(Inputs* inputs,
                                                                                  const Plane& plane,
                                                                                  const float sliceThickness)
{
    CaretAssert(inputs);
    m_inputs = inputs;
    m_volumeSpacePlaneValid = false;
    
    if (plane.isValidPlane()) {
        m_volumeSpacePlane = plane;
        m_volumeSpacePlaneValid = true;
        
        std::vector<AnnotationColorBar*> colorBars;
        drawAnnotationsInternal(AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                colorBars,
                                NULL,
                                sliceThickness);
    }
    
    m_volumeSpacePlaneValid = false;
    m_inputs = NULL;
}

/**
 * Draw the annotations in the given coordinate space.
 *
 * @param inputs
 *     Inputs for drawing annotations.
 * @param drawingCoordinateSpace
 *     Coordinate space of annotation that are drawn.
 * @param colorbars
 *     Colorbars that will be drawn.
 * @param surfaceDisplayed
 *     In not NULL, surface no which annotations are drawn.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotations(Inputs* inputs,
                                                           const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                                                           std::vector<AnnotationColorBar*>& colorBars,
                                                           const Surface* surfaceDisplayed)
{
    CaretAssert(inputs);
    m_inputs = inputs;
    
    m_volumeSpacePlaneValid = false;
    
    const float sliceThickness      = 0.0;
    drawAnnotationsInternal(drawingCoordinateSpace,
                            colorBars,
                            surfaceDisplayed,
                            sliceThickness);
    
    m_inputs = NULL;
}

/**
 * Draw the annotations in the given coordinate space.
 *
 * @param drawingCoordinateSpace
 *     Coordinate space of annotation that are drawn.
 * @param surfaceDisplayed
 *     Surface that is displayed.  May be NULL in some instances.
 * @param colorbars
 *     Colorbars that will be drawn.
 * @param surfaceDisplayed
 *     In not NULL, surface no which annotations are drawn.
 * @param sliceThickness
 *     Thickness of volume slice
 * @param centerToEyeDistance
 *     Distance from center to eye using in gluLookAt().
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationsInternal(const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                                                                   std::vector<AnnotationColorBar*>& colorBars,
                                                                   const Surface* surfaceDisplayed,
                                                                   const float sliceThickness)
{
    if (m_inputs->m_brain == NULL) {
        return;
    }
    
    const DisplayPropertiesAnnotation* dpa = m_inputs->m_brain->getDisplayPropertiesAnnotation();
    if ( ! dpa->isDisplayAnnotations()) {
        return;
    }
    
    m_volumeSliceThickness  = sliceThickness;
    
    setSelectionBoxColor();
    
    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("At beginning of annotation drawing in space "
                                                           + AnnotationCoordinateSpaceEnum::toName(drawingCoordinateSpace)));
    
    AnnotationManager* annotationManager = m_inputs->m_brain->getAnnotationManager();
    
    /*
     * When user is drawing an annotation by dragging the mouse, it is always in window space.
     */
    const Annotation* annotationBeingDrawn = ((drawingCoordinateSpace == AnnotationCoordinateSpaceEnum::WINDOW)
                                              ? annotationManager->getAnnotationBeingDrawnInWindow(m_inputs->m_windowIndex)
                                              : NULL);
    
    bool drawAnnotationsFromFilesFlag = true;
    
    switch (drawingCoordinateSpace) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            CaretAssertMessage(0, "Never draw annotations in pixel space.");
            return;
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            switch (m_inputs->m_windowDrawingMode) {
                case Inputs::WINDOW_DRAWING_NO:
                    drawAnnotationsFromFilesFlag = false;
                    break;
                case Inputs::WINDOW_DRAWING_YES:
                    break;
            }
//            if ( ! m_inputs->m_drawWindowAnnotationsFlag) {
//                drawAnnotationsFromFilesFlag = false;
//            }
            break;
    }
    
    /*
     * Note: When window annotations are being drawn, the
     * tab index is invalid so it must be ignored.
     */
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::DISPLAY_GROUP_A;
    if (drawingCoordinateSpace != AnnotationCoordinateSpaceEnum::WINDOW) {
        displayGroup = dpa->getDisplayGroupForTab(m_inputs->m_tabIndex);
    }
    
    SelectionItemAnnotation* annotationID = m_inputs->m_brain->getSelectionManager()->getAnnotationIdentification();
    
    GLint savedShadeModel = 0;
    GLboolean savedLightingEnabled = GL_FALSE;
    startOpenGLForDrawing(&savedShadeModel,
                          &savedLightingEnabled);
    
    /*
     * Check for a 'selection' type mode
     */
    bool idReturnFlag = false;
    m_selectionModeFlag = false;
    m_selectionInfo.clear();
    switch (m_inputs->m_drawingMode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (annotationID->isEnabledForSelection()) {
                m_selectionModeFlag = true;
            }
            else {
                idReturnFlag = true;
            }
            
            /*
             * Need flag shading for identification
             */
            glShadeModel(GL_FLAT);
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            idReturnFlag = true;
            break;
    }
    
    if (idReturnFlag) {
        endOpenGLForDrawing(savedShadeModel,
                            savedLightingEnabled);
        return;
    }
    
    /*
     * When selecting, clear out all previous drawing
     * since we identify via colors in each pixel.
     */
    if (m_selectionModeFlag) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    
    std::vector<AnnotationFile*> allAnnotationFiles;
    if (drawAnnotationsFromFilesFlag) {
        m_inputs->m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allAnnotationFiles);
    }

    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("Before draw annotations loop in space: "
                                                           + AnnotationCoordinateSpaceEnum::toName(drawingCoordinateSpace)));
    
    /*
     * Draw annotations from all files.
     * NOTE: iFile == numAnnFiles, the annotation colorbars are drawn
     */
    const int32_t numAnnFiles = static_cast<int32_t>(allAnnotationFiles.size());
    for (int32_t iFile = 0; iFile <= numAnnFiles; iFile++) {
        AnnotationFile* annotationFile = NULL;
        std::vector<Annotation*> annotationsFromFile;
        if (iFile == numAnnFiles) {
            /*
             * Use the dummy file when drawing annotation color
             * bars since they do not belong to a file.
             */
            annotationFile = m_dummyAnnotationFile;
            
            switch (drawingCoordinateSpace) {
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                case AnnotationCoordinateSpaceEnum::WINDOW:
                {
                    /*
                     * Note: Positions are in percentages ranging [0.0, 100.0]
                     */
                    float x = 14.0;
                    float y = 4.0;
                    bool firstColorBarFlag = true;
                    if ( ! colorBars.empty()) {
                        for (std::vector<AnnotationColorBar*>::iterator cbIter = colorBars.begin();
                             cbIter != colorBars.end();
                             cbIter++) {
                            AnnotationColorBar* cb = *cbIter;
                            if (cb->getCoordinateSpace() == drawingCoordinateSpace) {
                                switch  (cb->getPositionMode()) {
                                    case AnnotationColorBarPositionModeEnum::AUTOMATIC:
                                    {
                                        /*
                                         * Note: Y is incremented twice.  Once to move colorbar
                                         * so that the colorbars bottom is just above the previous
                                         * colorbar or bottom of screen.  Second time to move the
                                         * Y to the top of this annotation.
                                         */
                                        const float halfHeight = cb->getHeight() / 2.0;
                                        if (firstColorBarFlag) {
                                            firstColorBarFlag = false;
                                            y = 4;
                                            if (halfHeight > y) {
                                                y = halfHeight;
                                            }
                                        }
                                        else {
                                            y += halfHeight;
                                        }
                                        
                                        float xyz[3];
                                        cb->getCoordinate()->getXYZ(xyz);
                                        xyz[0] = x;
                                        xyz[1] = y;
                                        cb->getCoordinate()->setXYZ(xyz);
                                        y += halfHeight;
                                    }
                                        break;
                                    case AnnotationColorBarPositionModeEnum::MANUAL:
                                        break;
                                }
                            }
                        }
                        annotationsFromFile.insert(annotationsFromFile.end(),
                                                   colorBars.begin(),
                                                   colorBars.end());
                    }
                }
                    break;
            }
        }
        else {
            CaretAssertVectorIndex(allAnnotationFiles, iFile);
            annotationFile = allAnnotationFiles[iFile];
            annotationFile->getAllAnnotations(annotationsFromFile);
        }
        
        const int32_t annotationCount = static_cast<int32_t>(annotationsFromFile.size());
        for (int32_t iAnn = 0; iAnn < annotationCount; iAnn++) {
            CaretAssertVectorIndex(annotationsFromFile, iAnn);
            Annotation* annotation = annotationsFromFile[iAnn];
            CaretAssert(annotation);
            
            bool drawItFlag = false;
            if (annotation->getType() == AnnotationTypeEnum::COLOR_BAR) {
                AnnotationColorBar* colorBar = dynamic_cast<AnnotationColorBar*>(annotation);
                CaretAssert(colorBar);
                drawItFlag = colorBar->isDisplayed();
            }
            else {
                switch (annotation->getItemDisplaySelected(displayGroup, m_inputs->m_tabIndex)) {
                    case TriStateSelectionStatusEnum::PARTIALLY_SELECTED:
                        CaretAssertMessage(0, "An annotation should never be partially selected");
                        break;
                    case TriStateSelectionStatusEnum::SELECTED:
                        drawItFlag = true;
                        break;
                    case TriStateSelectionStatusEnum::UNSELECTED:
                        break;
                }
            }
            
            if ( ! drawItFlag) {
                continue;
            }
            
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
                if (m_inputs->m_windowIndex != annotationWindowIndex) {
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
                if (m_inputs->m_tabIndex != annotationTabIndex) {
                    continue;
                }
            }
            
            drawAnnotation(annotationFile,
                           annotation,
                           surfaceDisplayed);
        }
    }
    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("After draw annotations loop in space: "
                                                           + AnnotationCoordinateSpaceEnum::toName(drawingCoordinateSpace)));
    
    if (m_selectionModeFlag) {
        CaretAssert(annotationID);
        int32_t annotationIndex = -1;
        float depth = -1.0;
        m_brainOpenGLFixedPipeline->getIndexFromColorSelection(SelectionItemDataTypeEnum::ANNOTATION,
                                                               m_brainOpenGLFixedPipeline->mouseX,
                                                               m_brainOpenGLFixedPipeline->mouseY,
                                                               annotationIndex,
                                                               depth);
        
        
        if (annotationIndex >= 0) {
            if (annotationID != NULL) {
                CaretAssertVectorIndex(m_selectionInfo, annotationIndex);
                const SelectionInfo& selectionInfo = m_selectionInfo[annotationIndex];
                
                if (annotationID->isOtherScreenDepthCloserToViewer(depth)) {
                    
                    annotationID->setAnnotation(selectionInfo.m_annotationFile,
                                                selectionInfo.m_annotation,
                                                selectionInfo.m_sizingHandle);
                    annotationID->setBrain(m_inputs->m_brain);
                    annotationID->setScreenXYZ(selectionInfo.m_windowXYZ);
                    annotationID->setScreenDepth(depth);
                    CaretLogFine("Selected Annotation: " + annotationID->toString());
                }
            }
        }
    }
    else {
        /*
         * Annotation being drawn by the user.
         */
        m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL,
                                                        "Start of annotation drawn by user model space.");
        if (annotationBeingDrawn != NULL) {
            if (annotationBeingDrawn->getType() == AnnotationTypeEnum::TEXT) {
                const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(annotationBeingDrawn);
                CaretAssert(textAnn);
                
                AnnotationBox box(AnnotationAttributesDefaultTypeEnum::NORMAL);
                box.applyCoordinatesSizeAndRotationFromOther(textAnn);
                box.applyColoringFromOther(textAnn);

                drawAnnotation(m_dummyAnnotationFile,
                               &box,
                               surfaceDisplayed);
            }
            else {
                drawAnnotation(m_dummyAnnotationFile,
                               const_cast<Annotation*>(annotationBeingDrawn),
                               surfaceDisplayed);
            }
        }
        m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL,
                                                        "End of annotation drawn by user model space.");
    }
    

    endOpenGLForDrawing(savedShadeModel,
                        savedLightingEnabled);
    
    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("At end of annotation drawing in space "
                                                           + AnnotationCoordinateSpaceEnum::toName(drawingCoordinateSpace)));
}

/**
 * Sets OpenGL attributes before drawing annotations.
 *
 * @param savedShadeModelOut
 *      Current shading model is saved to this.
 * @param savedLightingEnabledOut
 *      Current lighting enabled status is saved to this.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::startOpenGLForDrawing(GLint* savedShadeModelOut,
                                                                 GLboolean* savedLightingEnabledOut)
{
    glGetIntegerv(GL_SHADE_MODEL,
                  savedShadeModelOut);

    glGetBooleanv(GL_LIGHTING,
                  savedLightingEnabledOut);
    
    glDisable(GL_LIGHTING);
    
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
    
    GLdouble depthRange[2];
    glGetDoublev(GL_DEPTH_RANGE,
                 depthRange);
    
    /*
     * All drawing is in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, m_modelSpaceViewport[2],
            0.0, m_modelSpaceViewport[3],
            depthRange[0], depthRange[1]);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /*
     * Enable anti-aliasing for lines and polygons
     */
    m_brainOpenGLFixedPipeline->enableLineAntiAliasing();
}

/**
 * Restores OpenGL attributes after drawing annotations.
 *
 * @param savedShadeModel
 *    Saved shading model that is restored
 * @param savedLightingEnabled
 *    Saved lighting enabled that is restored
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::endOpenGLForDrawing(GLint savedShadeModel,
                                                               GLboolean savedLightingEnabled)
{
    /*
     * Disable anti-aliasing for lines
     */
    m_brainOpenGLFixedPipeline->disableLineAntiAliasing();
    
    if (savedLightingEnabled) {
        glEnable(GL_LIGHTING);
    }
    else {
        glDisable(GL_LIGHTING);
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
 * Get color used for identification when drawing an annotation.
 *
 * @param identificationColorOut
 *     Color components encoding identification.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::getIdentificationColor(uint8_t identificationColorOut[4])
{
    const int32_t annotationDrawnIndex = static_cast<int32_t>(m_selectionInfo.size());
    m_brainOpenGLFixedPipeline->colorIdentification->addItem(identificationColorOut,
                                                             SelectionItemDataTypeEnum::ANNOTATION,
                                                             annotationDrawnIndex);
}

/**
 * Draw an annotation.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotation(AnnotationFile* annotationFile,
                                                          Annotation* annotation,
                                                          const Surface* surfaceDisplayed)
{
    CaretAssert(annotation);
    
    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
            drawBox(annotationFile,
                    dynamic_cast<AnnotationBox*>(annotation),
                    surfaceDisplayed);
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            drawColorBar(annotationFile,
                         dynamic_cast<AnnotationColorBar*>(annotation));
            break;
        case AnnotationTypeEnum::IMAGE:
            drawImage(annotationFile,
                      dynamic_cast<AnnotationImage*>(annotation),
                      surfaceDisplayed);
            break;
        case AnnotationTypeEnum::LINE:
            drawLine(annotationFile,
                     dynamic_cast<AnnotationLine*>(annotation),
                     surfaceDisplayed);
            break;
        case AnnotationTypeEnum::OVAL:
            drawOval(annotationFile,
                     dynamic_cast<AnnotationOval*>(annotation),
                     surfaceDisplayed);
            break;
        case AnnotationTypeEnum::TEXT:
            drawText(annotationFile,
                     dynamic_cast<AnnotationText*>(annotation),
                     surfaceDisplayed);
            break;
    }
}


/**
 * Draw an annotation box.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param box
 *    box to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawBox(AnnotationFile* annotationFile,
                                                   AnnotationBox* box,
                                                   const Surface* surfaceDisplayed)
{
    CaretAssert(box);
    CaretAssert(box->getType() == AnnotationTypeEnum::BOX);
    
    float windowXYZ[3];
    if ( ! getAnnotationWindowCoordinate(box->getCoordinate(),
                                         box->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(box, windowXYZ,
                               bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0
    };
    
    const float outlineWidth = box->getLineWidth();
    
    const bool depthTestFlag = isDrawnWithDepthTesting(box);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);

    std::vector<float> coords;
    coords.insert(coords.end(), bottomLeft,  bottomLeft + 3);
    coords.insert(coords.end(), bottomRight, bottomRight + 3);
    coords.insert(coords.end(), topRight,    topRight + 3);
    coords.insert(coords.end(), topLeft,     topLeft + 3);
    
    std::vector<float> dummyNormals;
    
    float backgroundRGBA[4];
    box->getBackgroundColorRGBA(backgroundRGBA);
    float foregroundRGBA[4];
    box->getLineColorRGBA(foregroundRGBA);

    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0.0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                         dummyNormals,
                                                         selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                const float slightlyThicker = 2.0;
                BrainOpenGLPrimitiveDrawing::drawLineLoop(coords,
                                                          selectionColorRGBA,
                                                          outlineWidth + slightlyThicker);
            }
            
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    box,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    selectionCenterXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                         dummyNormals,
                                                         backgroundRGBA);
            }
            
            if (drawForegroundFlag) {
                const bool drawOutlineWithPolygonsFlag = true;
                if (drawOutlineWithPolygonsFlag) {
                    BrainOpenGLPrimitiveDrawing::drawRectangleOutline(bottomLeft, bottomRight, topRight, topLeft,
                                                                      outlineWidth,
                                                                      foregroundRGBA);
                }
                else {
                    const float tempRGBA[4] = { 1.0, 1.0, 0.0, 1.0 };
                    BrainOpenGLPrimitiveDrawing::drawLineLoop(coords,
                                                              tempRGBA, //foregroundRGBA,
                                                              outlineWidth);
                }
            }
        }
        if (box->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimSizingHandles(annotationFile,
                                              box,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              outlineWidth,
                                              box->getRotationAngle());
        }
    }

    setDepthTestingStatus(savedDepthTestStatus);
}

/**
 * Draw an annotation box.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param colorBar
 *    Color bar to draw.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawColorBar(AnnotationFile* annotationFile,
                                                        AnnotationColorBar* colorBar)
{
    CaretAssert(colorBar);
    CaretAssert(colorBar->getType() == AnnotationTypeEnum::COLOR_BAR);
    
    float windowXYZ[3];
    if ( ! getAnnotationWindowCoordinate(colorBar->getCoordinate(),
                                         colorBar->getCoordinateSpace(),
                                         NULL,
                                         windowXYZ)) {
        return;
    }
    
//    /*
//     * NO SELECTION OF COLOR BAR ANNOTATIONS
//     */
//    if (m_selectionModeFlag) {
//        return;
//    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(colorBar, windowXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0
    };
    
    const float outlineWidth = colorBar->getLineWidth();
    
    const bool depthTestFlag = isDrawnWithDepthTesting(colorBar);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    std::vector<float> coords;
    coords.insert(coords.end(), bottomLeft,  bottomLeft + 3);
    coords.insert(coords.end(), bottomRight, bottomRight + 3);
    coords.insert(coords.end(), topRight,    topRight + 3);
    coords.insert(coords.end(), topLeft,     topLeft + 3);
    
    std::vector<float> dummyNormals;
    
    float backgroundRGBA[4];
    colorBar->getBackgroundColorRGBA(backgroundRGBA);
    float foregroundRGBA[4];
    colorBar->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0.0);
    
    if (m_selectionModeFlag) {
        uint8_t selectionColorRGBA[4];
        getIdentificationColor(selectionColorRGBA);
        BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                 dummyNormals,
                                                 selectionColorRGBA);
        m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                colorBar,
                                                AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                selectionCenterXYZ));
    }
    else {
        if (drawBackgroundFlag) {
            float bgBottomLeft[3];
            float bgBottomRight[3];
            float bgTopRight[3];
            float bgTopLeft[3];
            for (int32_t i = 0; i < 3; i++) {
                bgBottomLeft[i]  = bottomLeft[i];
                bgBottomRight[i] = bottomRight[i];
                bgTopLeft[i]     = topLeft[i];
                bgTopRight[i]    = topRight[i];
            }
            expandBox(bgBottomLeft, bgBottomRight, bgTopRight, bgTopLeft, 2, 0);
            
            std::vector<float> bgCoords;
            bgCoords.insert(bgCoords.end(), bgBottomLeft,  bgBottomLeft + 3);
            bgCoords.insert(bgCoords.end(), bgBottomRight, bgBottomRight + 3);
            bgCoords.insert(bgCoords.end(), bgTopRight,    bgTopRight + 3);
            bgCoords.insert(bgCoords.end(), bgTopLeft,     bgTopLeft + 3);
            
            BrainOpenGLPrimitiveDrawing::drawPolygon(bgCoords,
                                                     dummyNormals,
                                                     backgroundRGBA);
        }
        
        /*
         * The user sets the total height of the colorbar and the
         * height of the text.
         *
         * There are three items in the colorbar from top to bottom:
         *    (1) The numeric values
         *    (2) The tick marks
         *    (3) The palettes color sections
         */
        const float totalHeightPercent = colorBar->getHeight();
        float textHeightPercent        = colorBar->getFontPercentViewportSize();
        float ticksMarksHeightPercent  = totalHeightPercent * 0.10;
        float sectionsHeightPercent    = totalHeightPercent - (ticksMarksHeightPercent - textHeightPercent);
        
        if (sectionsHeightPercent <= 0.0) {
            sectionsHeightPercent    = totalHeightPercent * 0.10;
            textHeightPercent = totalHeightPercent - sectionsHeightPercent;
        }
        
        const float viewportHeight = m_modelSpaceViewport[3];
        
        /*
         * Text is aligned at the top of the characters
         */
        const float totalHeightPixels = (viewportHeight
                                         * (totalHeightPercent / 100.0));
        const float textOffsetFromTopPixels = 2;
        const float textHeightPixels = (viewportHeight
                                        * (textHeightPercent / 100.0));
        const float tickMarksHeightPixels = (viewportHeight
                                             * (ticksMarksHeightPercent / 100.0));
        const float sectionsHeightPixels = (totalHeightPixels
                                            - (textHeightPixels
                                               + textOffsetFromTopPixels
                                               + tickMarksHeightPixels));
        
        const bool debugFlag = false;
        if (debugFlag) {
            std::cout << "Color bar heights (pixels) " << std::endl;
            std::cout << "   Total:       " << totalHeightPixels << std::endl;
            std::cout << "   Text:        " << textHeightPixels << std::endl;
            std::cout << "   Text Offset: " << textOffsetFromTopPixels << std::endl;
            std::cout << "   Ticks:       " << tickMarksHeightPixels << std::endl;
        }
        
        drawColorBarSections(colorBar,
                             bottomLeft,
                             bottomRight,
                             topRight,
                             topLeft,
                             sectionsHeightPixels);
        
        drawColorBarText(colorBar,
                         bottomLeft,
                         bottomRight,
                         topRight,
                         topLeft,
                         textHeightPixels,
                         textOffsetFromTopPixels);
        
        /*
         * If 'extendTickMarksIntoColorBarPixels' is greater than zero,
         * the tickmarks will extend down and into the colorbar
         */
        const float extendTickMarksIntoColorBarPixels = tickMarksHeightPixels;
        const float tickMarksOffsetFromBotom = sectionsHeightPixels - extendTickMarksIntoColorBarPixels;
        const float totalTickMarksHeightPixels = tickMarksHeightPixels + extendTickMarksIntoColorBarPixels;
        drawColorBarTickMarks(colorBar,
                              bottomLeft,
                              bottomRight,
                              topRight,
                              topLeft,
                              totalTickMarksHeightPixels,
                              tickMarksOffsetFromBotom);
    }
    if (colorBar->isSelectedForEditing(m_inputs->m_windowIndex)) {
        drawAnnotationTwoDimSizingHandles(annotationFile,
                                          colorBar,
                                          bottomLeft,
                                          bottomRight,
                                          topRight,
                                          topLeft,
                                          outlineWidth,
                                          colorBar->getRotationAngle());
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
}

/**
 * Draw the color bar's tick marks
 *
 * @param colorBar
 *     Colorbar whose tick marks are drawn.
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * @param tickMarksHeightInPixels
 *     Height of the tick marks in pixels.
 * @param offsetFromBottomInPixels
 *     Offset of tick marks from the bottom
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawColorBarTickMarks(const AnnotationColorBar* colorBar,
                                                                 const float bottomLeftIn[3],
                                                                 const float bottomRightIn[3],
                                                                 const float topRightIn[3],
                                                                 const float topLeftIn[3],
                                                                 const float tickMarksHeightInPixels,
                                                                 const float offsetFromBottomInPixels)
{
    if ( ! colorBar->isShowTickMarksSelected()) {
        return;
    }
    
    if (tickMarksHeightInPixels < 1.0) {
        return;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    for (int32_t i = 0; i < 3; i++) {
        bottomLeft[i]  = bottomLeftIn[i];
        bottomRight[i] = bottomRightIn[i];
        topRight[i]    = topRightIn[i];
        topLeft[i]     = topLeftIn[i];
    }


    /*
     * Shrink the box that bounds the color bar in X so that start
     * and end ticks are at the ends of the color bar.
     */
    const float tickThickness = 2.0;
    expandBox(bottomLeft, bottomRight, topRight, topLeft, (-tickThickness / 2.0), 0);
    
    float bottomToTopUnitVector[3];
    MathFunctions::createUnitVector(bottomLeft,
                                    topLeft,
                                    bottomToTopUnitVector);
    const float xBottomLeft = (bottomLeft[0] + (bottomToTopUnitVector[0] * offsetFromBottomInPixels));
    const float yBottomLeft = (bottomLeft[1] + (bottomToTopUnitVector[1] * offsetFromBottomInPixels));
    
    const float xTopLeft  = (xBottomLeft + (bottomToTopUnitVector[0] * tickMarksHeightInPixels));
    const float yTopLeft  = (yBottomLeft + (bottomToTopUnitVector[1] * tickMarksHeightInPixels));
    
    const float xBottomRight = (bottomRight[0] + (bottomToTopUnitVector[0] * offsetFromBottomInPixels));
    const float yBottomRight = (bottomRight[1] + (bottomToTopUnitVector[1] * offsetFromBottomInPixels));
    
    const float xTopRight = (xBottomRight + (bottomToTopUnitVector[0] * tickMarksHeightInPixels));
    const float yTopRight = (yBottomRight + (bottomToTopUnitVector[1] * tickMarksHeightInPixels));
    
    std::vector<ColorBarLine> colorBarLines;

    float rgba[4];
    colorBar->getTextColorRGBA(rgba);
    
    const float z = 0.0;

    
    /*
     * Horizontal line at top of tick marks
     */
    const bool showHorizontalLineFlag = false;
    if (showHorizontalLineFlag) {
        std::vector<float> lineCoords;
        lineCoords.push_back(xTopLeft);
        lineCoords.push_back(yTopLeft);
        lineCoords.push_back(z);
        lineCoords.push_back(xTopRight);
        lineCoords.push_back(yTopRight);
        lineCoords.push_back(z);
        
        colorBarLines.push_back(ColorBarLine(lineCoords,
                                             rgba));
    }
    
    float leftToRightVector[3];
    MathFunctions::subtractVectors(topRight,
                                   topLeft,
                                   leftToRightVector);
    
    const float dx = leftToRightVector[0];
    const float dy = leftToRightVector[1];
    
    /*
     * Tickmarks for numeric text
     */
    const int32_t numText = colorBar->getNumberOfNumericText();
    for (int32_t i = 0; i < numText; i++) {
        const AnnotationColorBarNumericText* numericText = colorBar->getNumericText(i);
        const float scalar = numericText->getScalar();
        
        const float tickTopX    = xTopLeft    + (dx * scalar);
        const float tickTopY    = yTopLeft    + (dy * scalar);
        const float tickBottomX = xBottomLeft + (dx * scalar);
        const float tickBottomY = yBottomLeft + (dy * scalar);
        
        std::vector<float> lineCoords;
        lineCoords.push_back(tickTopX);
        lineCoords.push_back(tickTopY);
        lineCoords.push_back(z);
        lineCoords.push_back(tickBottomX);
        lineCoords.push_back(tickBottomY);
        lineCoords.push_back(z);

        colorBarLines.push_back(ColorBarLine(lineCoords,
                                             rgba));
    }
    
    /*
     * Draw lines and use polygon offset to ensure above color bar
     */
    glPolygonOffset(1.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_LINE);
    for (std::vector<ColorBarLine>::iterator iter = colorBarLines.begin();
         iter != colorBarLines.end();
         iter++) {
        CaretAssert(iter->m_lineCoords.size() == 6);
        BrainOpenGLPrimitiveDrawing::drawLines(iter->m_lineCoords,
                                               iter->m_rgba,
                                               tickThickness);
    }
    glDisable(GL_POLYGON_OFFSET_LINE);
}


/**
 * Draw the color bar's sections (the actual color bar)
 *
 * @param colorBar
 *     Colorbar whose sections are drawn.
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * @param sectionsHeightInPixels
 *     Height for drawing the sections in pixels.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawColorBarSections(const AnnotationColorBar* colorBar,
                                                                const float bottomLeft[3],
                                                                const float bottomRight[3],
                                                                const float*  /* const float topRight[3]*/,
                                                                const float topLeft[3],
                                                                const float sectionsHeightInPixels)
{
    std::vector<float> normals;
    for (int32_t i = 0; i < 4; i++) {
        normals.push_back(0.0);
        normals.push_back(0.0);
        normals.push_back(1.0);
    }
    
    std::vector<ColorBarLine> colorBarLines;
    
    float bottomToTopUnitVector[3];
    MathFunctions::createUnitVector(bottomLeft,
                                    topLeft,
                                    bottomToTopUnitVector);
    
    const float xTopLeft  = (bottomLeft[0] + (bottomToTopUnitVector[0] * sectionsHeightInPixels));
    const float yTopLeft  = (bottomLeft[1] + (bottomToTopUnitVector[1] * sectionsHeightInPixels));
    
    const float xTopRight = (bottomRight[0] + (bottomToTopUnitVector[0] * sectionsHeightInPixels));
    const float yTopRight = (bottomRight[1] + (bottomToTopUnitVector[1] * sectionsHeightInPixels));
    
    const float dx        = xTopRight - xTopLeft;
    const float dy        = yTopRight - yTopLeft;
    
    const float xBottomLeft = bottomLeft[0];
    const float yBottomLeft = bottomLeft[1];

    float minScalar = 0.0;
    float maxScalar = 0.0;
    colorBar->getScalarMinimumAndMaximumValues(minScalar,
                                               maxScalar);
    const float dScalar = maxScalar - minScalar;
    
    const bool printDebugFlag = false;
    if (printDebugFlag) {
        std::cout << qPrintable(QString("minScalar %1, maxScalar %2, dScalar %3").arg(minScalar).arg(maxScalar).arg(dScalar)) << std::endl;
    }
    
    const float z = 0.0;

    const int32_t numSections = colorBar->getNumberOfSections();
    
    for (int32_t iSect = 0; iSect < numSections; iSect++) {
        const AnnotationColorBarSection* section = colorBar->getSection(iSect);
        const float startScalar = section->getStartScalar();
        const float endScalar   = section->getEndScalar();
        
        
        float startNormalizedScalar = startScalar;
        float endNormalizedScalar   = endScalar;
        if (dScalar > 0.0) {
            startNormalizedScalar = (startScalar - minScalar) / dScalar;
            endNormalizedScalar   = (endScalar - minScalar)   / dScalar;
        }
        
        const float blX = xBottomLeft + (dx * startNormalizedScalar);
        const float blY = yBottomLeft + (dy * startNormalizedScalar);
        const float tlX = xTopLeft    + (dx * startNormalizedScalar);
        const float tlY = yTopLeft    + (dy * startNormalizedScalar);

        if (startScalar == endScalar) {
            std::vector<float> lineCoords;
            lineCoords.push_back(blX);
            lineCoords.push_back(blY);
            lineCoords.push_back(z);
            lineCoords.push_back(tlX);
            lineCoords.push_back(tlY);
            lineCoords.push_back(z);
            
            colorBarLines.push_back(ColorBarLine(lineCoords,
                                                 section->getStartRGBA()));
        }
        else {
            const float brX = xBottomLeft + (dx * endNormalizedScalar);
            const float brY = yBottomLeft + (dy * endNormalizedScalar);
            const float trX = xTopLeft    + (dx * endNormalizedScalar);
            const float trY = yTopLeft    + (dy * endNormalizedScalar);
            
            
            std::vector<float> coords;
            coords.push_back(blX);
            coords.push_back(blY);
            coords.push_back(z);
            coords.push_back(brX);
            coords.push_back(brY);
            coords.push_back(z);
            coords.push_back(trX);
            coords.push_back(trY);
            coords.push_back(z);
            coords.push_back(tlX);
            coords.push_back(tlY);
            coords.push_back(z);
            
            const float* rgbaLeft  = section->getStartRGBA();
            const float* rgbaRight = section->getEndRGBA();
            std::vector<float> rgbaColors;
            rgbaColors.insert(rgbaColors.end(), rgbaLeft, rgbaLeft + 4);
            rgbaColors.insert(rgbaColors.end(), rgbaRight, rgbaRight + 4);
            rgbaColors.insert(rgbaColors.end(), rgbaRight, rgbaRight + 4);
            rgbaColors.insert(rgbaColors.end(), rgbaLeft, rgbaLeft + 4);
            
            BrainOpenGLPrimitiveDrawing::drawQuads(coords,
                                                   normals,
                                                   rgbaColors);
            
            if (printDebugFlag) {
                const AString msg("Section ("
                                  + AString::number(startScalar)
                                  + ", "
                                  + AString::number(endScalar)
                                  + ") normalized ("
                                  + AString::number(startNormalizedScalar)
                                  + ", "
                                  + AString::number(endNormalizedScalar)
                                  + ") X-range ("
                                  + AString::number(blX)
                                  + ", "
                                  + AString::number(brX)
                                  + ") Y-range ("
                                  + AString::number(blY)
                                  + ", "
                                  + AString::number(tlY)
                                  + ")");
                std::cout << qPrintable(msg) << std::endl;
            }
        }
    }
    
    /*
     * Lines need to be drawn OVER any quads (otherwise the quads
     * would obscure the lines
     */
    glPolygonOffset(1.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_LINE);
    for (std::vector<ColorBarLine>::iterator iter = colorBarLines.begin();
         iter != colorBarLines.end();
         iter++) {
        CaretAssert(iter->m_lineCoords.size() == 6);
        BrainOpenGLPrimitiveDrawing::drawLines(iter->m_lineCoords,
                                               iter->m_rgba,
                                               1.0);
    }
    glDisable(GL_POLYGON_OFFSET_LINE);
}

/**
 * Draw the color bar's text
 *
 * @param colorBar
 *     Colorbar whose text is drawn.
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * @param textHeightInPixels
 *     Height of text in pixels.
 * @param offsetFromTopInPixels
 *     Offset of text from the top of the colorbar viewport in pixels
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawColorBarText(const AnnotationColorBar* colorBar,
                                                            const float bottomLeft[3],
                                                            const float bottomRight[3],
                                                            const float topRight[3],
                                                            const float topLeft[3],
                                                            const float textHeightInPixels,
                                                            const float offsetFromTopInPixels)
{
    const float textPercentageHeight = (textHeightInPixels / m_modelSpaceViewport[3]) * 100.0;
    
    AnnotationPercentSizeText annText(AnnotationAttributesDefaultTypeEnum::NORMAL);
    annText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
    annText.setFont(colorBar->getFont());
    annText.setFontPercentViewportSize(textPercentageHeight); //colorBar->getFontPercentViewportSize());
    annText.setTextColor(CaretColorEnum::CUSTOM);
    float rgba[4];
    colorBar->getTextColorRGBA(rgba);
    annText.setCustomTextColor(rgba);
    annText.setRotationAngle(colorBar->getRotationAngle());
    
    float bottomToTopUnitVector[3];
    MathFunctions::createUnitVector(bottomLeft,
                                    topLeft,
                                    bottomToTopUnitVector);
    const float distanceBottomToTop = MathFunctions::distance3D(topLeft,
                                                                bottomLeft);
    const float distanceBottomToTopWithOffset = (distanceBottomToTop
                                                 - offsetFromTopInPixels);
    
    const float xTopLeft  = (bottomLeft[0] + (bottomToTopUnitVector[0] * distanceBottomToTopWithOffset));
    const float yTopLeft  = (bottomLeft[1] + (bottomToTopUnitVector[1] * distanceBottomToTopWithOffset));
    
    float leftToRightVector[3];
    MathFunctions::subtractVectors(topRight,
                                   topLeft,
                                   leftToRightVector);
    
    const float dx = leftToRightVector[0];
    const float dy = leftToRightVector[1];
    
    const float windowZ = (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0;
    const int32_t numText = colorBar->getNumberOfNumericText();
    for (int32_t i = 0; i < numText; i++) {
        const AnnotationColorBarNumericText* numericText = colorBar->getNumericText(i);
        const float scalar = numericText->getScalar();
        float scalarOffset = 0.0;
        annText.setHorizontalAlignment(numericText->getHorizontalAlignment());
        
        const float windowX = xTopLeft + (dx * (scalar + scalarOffset));
        const float windowY = yTopLeft + (dy * (scalar + scalarOffset));
        
        annText.setText(numericText->getNumericText());
        m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(windowX,
                                                                                windowY,
                                                                                windowZ,
                                                                                annText);
    }
}


/**
 * Draw an annotation oval.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param oval
 *    Annotation oval to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawOval(AnnotationFile* annotationFile,
                                                    AnnotationOval* oval,
                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(oval);
    CaretAssert(oval->getType() == AnnotationTypeEnum::OVAL);
    
    float windowXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(oval->getCoordinate(),
                                         oval->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(oval, windowXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    const float majorAxis     = ((oval->getWidth()  / 100.0) * (m_modelSpaceViewport[2] / 2.0));
    const float minorAxis     = ((oval->getHeight() / 100.0) * (m_modelSpaceViewport[3] / 2.0));
    const float rotationAngle = oval->getRotationAngle();
    const float outlineWidth  = oval->getLineWidth();
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(oval);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    uint8_t backgroundRGBA[4];
    oval->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    oval->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0.0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    if (drawAnnotationFlag) {
        glPushMatrix();
        glTranslatef(windowXYZ[0], windowXYZ[1], windowXYZ[2]);
        if (rotationAngle != 0.0) {
            glRotatef(-rotationAngle, 0.0, 0.0, 1.0);
        }
        
        if (m_selectionModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                m_brainOpenGLFixedPipeline->drawEllipseFilled(selectionColorRGBA,
                                                              majorAxis,
                                                              minorAxis);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                const float slightlyThicker = 2.0;
                m_brainOpenGLFixedPipeline->drawEllipseOutline(selectionColorRGBA,
                                                               majorAxis,
                                                               minorAxis,
                                                               outlineWidth + slightlyThicker);
            }
            
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    oval,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    selectionCenterXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                m_brainOpenGLFixedPipeline->drawEllipseFilled(backgroundRGBA,
                                                              majorAxis,
                                                              minorAxis);
            }
            
            if (drawForegroundFlag) {
                m_brainOpenGLFixedPipeline->drawEllipseOutline(foregroundRGBA,
                                                               majorAxis,
                                                               minorAxis,
                                                               outlineWidth);
            }
        }
        glPopMatrix();
        
        if (oval->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimSizingHandles(annotationFile,
                                              oval,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              outlineWidth,
                                              oval->getRotationAngle());
        }
    }
    
    
    
    
    setDepthTestingStatus(savedDepthTestStatus);
}

/**
 * Get coordinate for drawing a line that connects text to brainordinate.
 *
 * @param text
 *    The text annotation.
 * @param surfaceDisplayed
 *    Surface for text
 * @param lineCoordinatesOut
 *    Output with coordinates for drawing line.  Will be EMPTY if line is not drawn.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::getTextLineToBrainordinateLineCoordinates(const AnnotationText* text,
                                                                                     const Surface* surfaceDisplayed,
                                                                                     std::vector<float>& lineCoordinatesOut) const
{
    lineCoordinatesOut.clear();
    
    if (text->isConnectToBrainordinateValid()) {
        bool showLineFlag = false;
        bool showArrowFlag = false;
        
        if (text->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::SURFACE) {
            if (surfaceDisplayed != NULL) {
                switch (text->getConnectToBrainordinate()) {
                    case AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE:
                        break;
                    case AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_ARROW:
                        showArrowFlag = true;
                        break;
                    case AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_LINE:
                        showLineFlag = true;
                        break;
                }
            }
        }
        
        if (showLineFlag
            || showArrowFlag) {
            const AnnotationCoordinate* coord = text->getCoordinate();
            StructureEnum::Enum structure = StructureEnum::INVALID;
            int32_t numNodes  = 0;
            int32_t nodeIndex = 0;
            float offset      = 0.0;
            AnnotationSurfaceOffsetVectorTypeEnum::Enum offsetVector = AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX;
            coord->getSurfaceSpace(structure,
                                   numNodes,
                                   nodeIndex,
                                   offset,
                                   offsetVector);
            
            if (offset > 0.0) {
                AnnotationCoordinate noOffsetCoord = *coord;
                noOffsetCoord.setSurfaceSpace(structure,
                                              numNodes,
                                              nodeIndex,
                                              0.0,
                                              AnnotationSurfaceOffsetVectorTypeEnum::CENTROID_THRU_VERTEX);
                
                float brainordinateXYZ[3];
                if (getAnnotationWindowCoordinate(&noOffsetCoord,
                                                  text->getCoordinateSpace(),
                                                  surfaceDisplayed,
                                                  brainordinateXYZ)) {
                    float windowXYZ[3];
                    if (getAnnotationWindowCoordinate(coord,
                                                      text->getCoordinateSpace(),
                                                      surfaceDisplayed,
                                                      windowXYZ)) {
                        
                        createLineCoordinates(windowXYZ,
                                              brainordinateXYZ,
                                              text->getLineWidth(),
                                              false,
                                              showArrowFlag,
                                              lineCoordinatesOut);
                    }
                }
            }
        }
    }
}

/**
 * Draw an annotation text.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param text
 *    Annotation text to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawText(AnnotationFile* annotationFile,
                                                    AnnotationText* text,
                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(text);
    CaretAssert(text->getType() == AnnotationTypeEnum::TEXT);

    float windowXYZ[3];
    if ( ! getAnnotationWindowCoordinate(text->getCoordinate(),
                                         text->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    
    /*
     * The fonts are sized using either the height 
     * of the OpenGL viewport or the height of the
     * TAB viewport.  During a single surface view,
     * they will be the same but different in a 
     * surface montage view if there are two or more
     * rows.
     */
    AnnotationPercentSizeText* percentSizeText = NULL;
    float savedFontPercentViewportHeight = -1.0;
    const bool modifiedStatus = text->isModified();
    
    const bool allowDifferentHeightModesFlag = false;
    if (allowDifferentHeightModesFlag) {
        switch (m_inputs->m_textHeightMode) {
            case Inputs::TEXT_HEIGHT_USE_OPENGL_VIEWPORT_HEIGHT:
                break;
            case Inputs::TEXT_HEIGHT_USE_TAB_VIEWPORT_HEIGHT:
                if (m_inputs->m_tabViewport[3] != m_modelSpaceViewport[3]) {
                    percentSizeText = dynamic_cast<AnnotationPercentSizeText*>(text);
                    if (percentSizeText != NULL) {
                        savedFontPercentViewportHeight = percentSizeText->getFontPercentViewportSize();
                        if (savedFontPercentViewportHeight > 0.0) {
                            CaretAssert(m_modelSpaceViewport[3]);
                            const float heightScaling = m_inputs->m_tabViewport[3] / m_modelSpaceViewport[3];
                            percentSizeText->setFontPercentViewportSize(savedFontPercentViewportHeight
                                                                        * heightScaling);
                        }
                    }
                }
                break;
        }
    }
    
    m_brainOpenGLFixedPipeline->getTextRenderer()->getBoundsForTextAtViewportCoords(*text,
                                                                                    windowXYZ[0], windowXYZ[1], windowXYZ[2],
                                                                                    //textDrawingViewportHeight,
                                                                                    m_modelSpaceViewport[3],
                                                                                    bottomLeft, bottomRight, topRight, topLeft);
    
    std::vector<float> coords;
    coords.insert(coords.end(), bottomLeft,  bottomLeft + 3);
    coords.insert(coords.end(), bottomRight, bottomRight + 3);
    coords.insert(coords.end(), topRight,    topRight + 3);
    coords.insert(coords.end(), topLeft,     topLeft + 3);
    
    std::vector<float> dummyNormals;

    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0
    };

    const bool depthTestFlag = isDrawnWithDepthTesting(text);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    float backgroundRGBA[4];
    text->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    text->getLineColorRGBA(foregroundRGBA);
    uint8_t textColorRGBA[4];
    text->getTextColorRGBA(textColorRGBA);
    
    const bool drawTextFlag       = (textColorRGBA[3] > 0.0);
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0.0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawTextFlag);
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                     dummyNormals,
                                                     selectionColorRGBA);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    text,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    selectionCenterXYZ));
        }
        else {
            std::vector<float> connectLineCoordinates;

            getTextLineToBrainordinateLineCoordinates(text,
                                                      surfaceDisplayed,
                                                      connectLineCoordinates);
            
            if ( ! connectLineCoordinates.empty()) {
                BrainOpenGLPrimitiveDrawing::drawLines(connectLineCoordinates,
                                                       textColorRGBA,
                                                       text->getLineWidth());
            }
            
            if (drawBackgroundFlag) {
                BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                         dummyNormals,
                                                         backgroundRGBA);
            }
            
            if (drawTextFlag) {
                const bool debugFlag = false;
                if (debugFlag) {
                    if (text->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
                        AString msg("Drawing Text: " + text->getText() + "  DepthTest=" + AString::fromBool(depthTestFlag));
                        const float* xyz = text->getCoordinate()->getXYZ();
                        msg.appendWithNewLine("    Stereotaxic: " + AString::fromNumbers(xyz, 3, ","));
                        msg.appendWithNewLine("    Window: " + AString::fromNumbers(windowXYZ, 3, ","));
                        std::cout << qPrintable(msg) << std::endl;
                    }
                }
                
                if (depthTestFlag) {
                    m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(windowXYZ[0],
                                                                                            windowXYZ[1],
                                                                                            windowXYZ[2],
                                                                                            *text);
                }
                else {
                    if (text->getText().isEmpty()) {
                        /*
                         * Text is empty when user is dragging mouse to create a
                         * text region.  In this case, use the bounds of the 
                         * two-dim shape.
                         */
                        float bl[3];
                        float br[3];
                        float tr[3];
                        float tl[3];
                        getAnnotationTwoDimShapeBounds(text, windowXYZ, bl, br, tr, tl);
                        
                        std::vector<float> boxCoords;
                        boxCoords.insert(boxCoords.end(), bl, bl + 3);
                        boxCoords.insert(boxCoords.end(), br, br + 3);
                        boxCoords.insert(boxCoords.end(), tr, tr + 3);
                        boxCoords.insert(boxCoords.end(), tl, tl + 3);
                        
                        if (boxCoords.size() == 12) {
                            BrainOpenGLPrimitiveDrawing::drawLineLoop(boxCoords,
                                                                      foregroundRGBA,
                                                                      text->getLineWidth());
                        }
                    }
                    else {
                        m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(windowXYZ[0],
                                                                                           windowXYZ[1],
                                                                                           *text);
                    }
                }
                
                setDepthTestingStatus(depthTestFlag);
            }
        }
        
        if (text->isSelectedForEditing(m_inputs->m_windowIndex)) {
            const float outlineWidth = 2.0;
            drawAnnotationTwoDimSizingHandles(annotationFile,
                                              text,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              outlineWidth,
                                              text->getRotationAngle());
        }
    }

    if (percentSizeText != NULL) {
        if (savedFontPercentViewportHeight > 0.0) {
            percentSizeText->setFontPercentViewportSize(savedFontPercentViewportHeight);
            if ( ! modifiedStatus) {
                percentSizeText->clearModified();
            }
        }
    }

    setDepthTestingStatus(savedDepthTestStatus);
}

/**
 * Draw an annotation image.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param image
 *    Annotation image to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawImage(AnnotationFile* annotationFile,
                                                     AnnotationImage* image,
                                                    const Surface* surfaceDisplayed)
{
    const uint8_t* imageRgbaBytes = image->getImageBytesRGBA();
    const int32_t imageWidth      = image->getImageWidth();
    const int32_t imageHeight     = image->getImageHeight();
    
    if ((imageWidth > 0)
        && (imageHeight > 0)
        && (imageRgbaBytes != NULL)) {
        
    }
    else {
        CaretLogWarning("Attempt to draw invalid image annotation.");
    }

    float windowXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(image->getCoordinate(),
                                         image->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         windowXYZ)) {
        return;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(image, windowXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    std::vector<float> coords;
    coords.insert(coords.end(), bottomLeft,  bottomLeft + 3);
    coords.insert(coords.end(), bottomRight, bottomRight + 3);
    coords.insert(coords.end(), topRight,    topRight + 3);
    coords.insert(coords.end(), topLeft,     topLeft + 3);
    
    std::vector<float> dummyNormals;
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(image);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    const bool drawAnnotationFlag = true;
    
    float foregroundRGBA[4];
    image->getLineColorRGBA(foregroundRGBA);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0);

    if (drawAnnotationFlag) {
        if (m_selectionModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                     dummyNormals,
                                                     selectionColorRGBA);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    image,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    selectionCenterXYZ));
        }
        else {
            const bool debugFlag = false;
            if (debugFlag) {
                if (image->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
                    AString msg("Drawing Image: DepthTest=" + AString::fromBool(depthTestFlag));
                    const float* xyz = image->getCoordinate()->getXYZ();
                    msg.appendWithNewLine("    Stereotaxic: " + AString::fromNumbers(xyz, 3, ","));
                    msg.appendWithNewLine("    Window: " + AString::fromNumbers(windowXYZ, 3, ","));
                    std::cout << qPrintable(msg) << std::endl;
                }
            }
            
            drawImageBytesWithTexture(image->getDrawWithOpenGLTextureInfo(),
                                      bottomLeft,
                                      bottomRight,
                                      topRight,
                                      topLeft,
                                      imageRgbaBytes,
                                      imageWidth,
                                      imageHeight);
            
            if (drawForegroundFlag) {
                BrainOpenGLPrimitiveDrawing::drawLineLoop(coords,
                                                          foregroundRGBA,
                                                          image->getLineWidth());
            }
            
            setDepthTestingStatus(depthTestFlag);
        }
        
        if (image->isSelectedForEditing(m_inputs->m_windowIndex)) {
            const float outlineWidth = 2.0;
            drawAnnotationTwoDimSizingHandles(annotationFile,
                                              image,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              outlineWidth,
                                              image->getRotationAngle());
        }
    }

    setDepthTestingStatus(savedDepthTestStatus);
}


/**
 * Draw image annoation using a textured quad.
 *
 * @param textureInfo
 *     Information for OpenGL texture usage.
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * @param imageBytesRGBA
 *     Bytes containing the image data.
 * @param imageWidth
 *     Width of the actual image.
 * @param imageHeight
 *     Height of the image.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawImageBytesWithTexture(DrawnWithOpenGLTextureInfo* textureInfo,
                                                                     const float bottomLeft[3],
                                                                     const float bottomRight[3],
                                                                     const float topRight[3],
                                                                     const float topLeft[3],
                                                                     const uint8_t* imageBytesRGBA,
                                                                     const int32_t imageWidth,
                                                                     const int32_t imageHeight)
{
    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("At beginning of annotation drawImageBytesWithTexture()"));
    
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    static bool useMipMapFlag = true;

    BrainOpenGLTextureManager* textureManager = m_brainOpenGLFixedPipeline->getTextureManager();
    CaretAssert(textureManager);
    
    GLuint textureName = 0;
    bool newTextureNameFlag = false;
    textureManager->getTextureName(textureInfo,
                                   textureName,
                                   newTextureNameFlag);
    
    if (newTextureNameFlag) {
        glBindTexture(GL_TEXTURE_2D, textureName);
        if (useMipMapFlag) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            const int errorCode = gluBuild2DMipmaps(GL_TEXTURE_2D,     // MUST BE GL_TEXTURE_2D
                                                    GL_RGBA,           // number of components
                                                    imageWidth,        // width of image
                                                    imageHeight,       // height of image
                                                    GL_RGBA,           // format of the pixel data
                                                    GL_UNSIGNED_BYTE,  // data type of pixel data
                                                    imageBytesRGBA);    // pointer to image data
            if (errorCode != 0) {
                useMipMapFlag = false;
                
                const GLubyte* errorChars = gluErrorString(errorCode);
                if (errorChars != NULL) {
                    const QString errorText = ("ERROR building mipmaps for annotation image: "
                                               + AString((char*)errorChars));
                    CaretLogSevere(errorText);
                }
            }
        }
        
        if ( ! useMipMapFlag) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            
            glTexImage2D(GL_TEXTURE_2D,     // MUST BE GL_TEXTURE_2D
                         0,                 // level of detail 0=base, n is nth mipmap reduction
                         GL_RGBA,           // number of components
                         imageWidth,        // width of image
                         imageHeight,       // height of image
                         0,                 // border
                         GL_RGBA,           // format of the pixel data
                         GL_UNSIGNED_BYTE,  // data type of pixel data
                         imageBytesRGBA);   // pointer to image data
        }
    }
    
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, textureName);
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3fv(bottomLeft);
    glTexCoord2f(1.0, 0.0);
    glVertex3fv(bottomRight);
    glTexCoord2f(1.0, 1.0);
    glVertex3fv(topRight);
    glTexCoord2f(0.0, 1.0);
    glVertex3fv(topLeft);
    glEnd();
    
    /*
     *
     */
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glDisable(GL_TEXTURE_2D);
    
    glPopClientAttrib();
    
    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("At end of annotation drawImageBytesWithTexture()"));
}

/**
 * Draw an image texture by drawing pixels.
 * DOES NOT WORK IF BOTTOM LEFT CORNER OF IMAGE HAS A NEGATIVE VALUE !!
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawImageBytes(const float windowX,
                                                          const float windowY,
                                                          const float windowZ,
                                                          const uint8_t* imageBytesRGBA,
                                                          const int32_t imageWidth,
                                                          const int32_t imageHeight)
{
    /*
     * Reset orthographic projection to viewport size
     */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    const float drawX = windowX - (imageWidth / 2.0);
    const float drawY = windowY - (imageHeight / 2.0);
    
    /*
     * Saves glPixelStore parameters
     */
    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
    
    //
    // Draw image near far clipping plane
    //
    glRasterPos3f(drawX, drawY, windowZ); //-500.0); // set Z so image behind surface
    
    glDrawPixels(imageWidth, imageHeight,
                 GL_RGBA, GL_UNSIGNED_BYTE,
                 (GLvoid*)imageBytesRGBA);
    
    glPopClientAttrib();
    
    glPopMatrix();
}

/**
 * Create the coordinates for drawing a line with optional arrows at the end points.
 *
 * @param lineHeadXYZ
 *     Start of the line
 * @param lineTailXYZ
 *     End of the line
 * @param lineThickness
 *     Thickness of the line that affects size of the optional arrow heads.
 * @param validStartArrow
 *     Add an arrow at the start of the line
 * @param validEndArrow
 *     Add an arrow at the end of the line
 * @param coordinatesOut
 *     Output containing coordinates for drawing the line
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::createLineCoordinates(const float lineHeadXYZ[3],
                                                                 const float lineTailXYZ[3],
                                                                 const float lineThickness,
                                                                 const bool validStartArrow,
                                                                 const bool validEndArrow,
                                                                 std::vector<float>& coordinatesOut) const
{
    coordinatesOut.clear();
    
    /*
     * Length of arrow's tips is function of line thickness
     */
    const float tipScale = 3.0;
    const float tipLength = lineThickness * tipScale;
    
    /*
     * Point on arrow's line that is between the arrow's left and right arrow tips
     */
    float headToTailVector[3];
    MathFunctions::createUnitVector(lineHeadXYZ, lineTailXYZ, headToTailVector);
    const float startArrowTipsOnLine[3] = {
        lineHeadXYZ[0] + (headToTailVector[0] * tipLength),
        lineHeadXYZ[1] + (headToTailVector[1] * tipLength),
        lineHeadXYZ[2] + (headToTailVector[2] * tipLength)
    };
    const float tailArrowTipsOnLine[3] = {
        lineTailXYZ[0] - (headToTailVector[0] * tipLength),
        lineTailXYZ[1] - (headToTailVector[1] * tipLength),
        lineTailXYZ[2] - (headToTailVector[2] * tipLength)
    };
    
    /*
     * Vector for arrow tip's on left and right
     *
     * Create a perpendicular vector by swapping first two elements
     * and negating the second element.
     */
    float headLeftRightTipOffset[3] = {
        headToTailVector[0],
        headToTailVector[1],
        headToTailVector[2]
    };
    MathFunctions::normalizeVector(headLeftRightTipOffset);
    std::swap(headLeftRightTipOffset[0],
              headLeftRightTipOffset[1]);
    headLeftRightTipOffset[1] *= -1;
    headLeftRightTipOffset[0] *= tipLength;
    headLeftRightTipOffset[1] *= tipLength;
    headLeftRightTipOffset[2] *= tipLength;
    
    /*
     * Tip of arrow's head pointer on the right
     */
    const float headRightTipEnd[3] = {
        startArrowTipsOnLine[0] - headLeftRightTipOffset[0],
        startArrowTipsOnLine[1] - headLeftRightTipOffset[1],
        startArrowTipsOnLine[2] - headLeftRightTipOffset[2]
    };
    
    /*
     * Tip of arrow's head pointer on the left
     */
    const float headLeftTipEnd[3] = {
        startArrowTipsOnLine[0] + headLeftRightTipOffset[0],
        startArrowTipsOnLine[1] + headLeftRightTipOffset[1],
        startArrowTipsOnLine[2] + headLeftRightTipOffset[2]
    };
    
    /*
     * Tip of arrow tail pointer on the right
     */
    const float tailRightTipEnd[3] = {
        tailArrowTipsOnLine[0] + headLeftRightTipOffset[0],
        tailArrowTipsOnLine[1] + headLeftRightTipOffset[1],
        tailArrowTipsOnLine[2] + headLeftRightTipOffset[2]
    };
    
    /*
     * Tip of arrow tail pointer on the right
     */
    const float tailLeftTipEnd[3] = {
        tailArrowTipsOnLine[0] - headLeftRightTipOffset[0],
        tailArrowTipsOnLine[1] - headLeftRightTipOffset[1],
        tailArrowTipsOnLine[2] - headLeftRightTipOffset[2]
    };
    
    coordinatesOut.insert(coordinatesOut.end(), lineHeadXYZ, lineHeadXYZ + 3);
    coordinatesOut.insert(coordinatesOut.end(), lineTailXYZ, lineTailXYZ + 3);
    if (validStartArrow) {
        coordinatesOut.insert(coordinatesOut.end(), lineHeadXYZ,     lineHeadXYZ + 3);
        coordinatesOut.insert(coordinatesOut.end(), headRightTipEnd, headRightTipEnd + 3);
        coordinatesOut.insert(coordinatesOut.end(), lineHeadXYZ,     lineHeadXYZ + 3);
        coordinatesOut.insert(coordinatesOut.end(), headLeftTipEnd,  headLeftTipEnd + 3);
    }
    if (validEndArrow) {
        coordinatesOut.insert(coordinatesOut.end(), lineTailXYZ,     lineTailXYZ + 3);
        coordinatesOut.insert(coordinatesOut.end(), tailRightTipEnd, tailRightTipEnd + 3);
        coordinatesOut.insert(coordinatesOut.end(), lineTailXYZ,     lineTailXYZ + 3);
        coordinatesOut.insert(coordinatesOut.end(), tailLeftTipEnd,  tailLeftTipEnd + 3);
    }
}

/**
 * Draw an annotation line.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param line
 *    Annotation line to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawLine(AnnotationFile* annotationFile,
                                                    AnnotationLine* line,
                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(line);
    CaretAssert(line->getType() == AnnotationTypeEnum::LINE);
    
    float lineHeadXYZ[3];
    float lineTailXYZ[3];
    
    if ( ! getAnnotationWindowCoordinate(line->getStartCoordinate(),
                                         line->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         lineHeadXYZ)) {
        return;
    }
    if ( ! getAnnotationWindowCoordinate(line->getEndCoordinate(),
                                         line->getCoordinateSpace(),
                                         surfaceDisplayed,
                                         lineTailXYZ)) {
        return;
    }
    const float lineWidth = line->getLineWidth();
    const float backgroundLineWidth = lineWidth + 4;
    
    const float selectionCenterXYZ[3] = {
        (lineHeadXYZ[0] + lineTailXYZ[0]) / 2.0,
        (lineHeadXYZ[1] + lineTailXYZ[1]) / 2.0,
        (lineHeadXYZ[2] + lineTailXYZ[2]) / 2.0
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(line);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    std::vector<float> coords;
    createLineCoordinates(lineHeadXYZ,
                          lineTailXYZ,
                          line->getLineWidth(),
                          line->isDisplayStartArrow(),
                          line->isDisplayEndArrow(),
                          coords);
    
    uint8_t foregroundRGBA[4];
    line->getLineColorRGBA(foregroundRGBA);
    
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0);
    
    if (drawForegroundFlag) {
        if (m_selectionModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            BrainOpenGLPrimitiveDrawing::drawLines(coords,
                                                   selectionColorRGBA,
                                                   backgroundLineWidth);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    line,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    selectionCenterXYZ));
        }
        else {
            if (drawForegroundFlag) {
                BrainOpenGLPrimitiveDrawing::drawLines(coords,
                                                       foregroundRGBA,
                                                       lineWidth);
                
            }
        }
        
        if (line->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationOneDimSizingHandles(annotationFile,
                                              line,
                                              lineHeadXYZ,
                                              lineTailXYZ,
                                              lineWidth);
        }
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
}

/**
 * Draw a sizing handle at the given coordinate.
 *
 * @param handleType
 *     Type of sizing handle.
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param xyz
 *     Center of square.
 * @param halfWidthHeight
 *     Half Width/height of square.
 * @param rotationAngle
 *     Rotation angle for the sizing handle.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawSizingHandle(const AnnotationSizingHandleTypeEnum::Enum handleType,
                                                            AnnotationFile* annotationFile,
                                                            Annotation* annotation,
                                                            const float xyz[3],
                                                            const float halfWidthHeight,
                                                            const float rotationAngle)
{
    glPushMatrix();
    
    glTranslatef(xyz[0], xyz[1], xyz[2]);
    if (rotationAngle != 0.0) {
        glRotatef(-rotationAngle, 0.0, 0.0, 1.0);
    }
    
    std::vector<float> coords;
    coords.push_back(-halfWidthHeight);
    coords.push_back(-halfWidthHeight);
    coords.push_back(0.0);
    coords.push_back( halfWidthHeight);
    coords.push_back(-halfWidthHeight);
    coords.push_back(0.0);
    coords.push_back( halfWidthHeight);
    coords.push_back( halfWidthHeight);
    coords.push_back(0.0);
    coords.push_back(-halfWidthHeight);
    coords.push_back( halfWidthHeight);
    coords.push_back(0.0);
    
    std::vector<float> dummyNormals;
    
    bool drawFilledCircleFlag  = false;
    bool drawOutlineCircleFlag = false;
    bool drawSquareFlag        = false;
    
    switch (handleType) {
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM:
            drawSquareFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT:
            drawFilledCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT:
            drawFilledCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT:
            drawSquareFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT:
            drawSquareFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP:
            drawSquareFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT:
            drawFilledCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT:
            drawFilledCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END:
            drawFilledCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            drawFilledCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            drawOutlineCircleFlag = true;
            break;
    }
    
    if (m_selectionModeFlag) {
        uint8_t identificationRGBA[4];
        getIdentificationColor(identificationRGBA);
        BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                 dummyNormals,
                                                 identificationRGBA);
        m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                annotation,
                                                handleType,
                                                xyz));
    }
    else {
        if (drawFilledCircleFlag) {
            m_brainOpenGLFixedPipeline->drawCircleFilled(m_selectionBoxRGBA,
                                                         halfWidthHeight);
        }
        else if (drawSquareFlag) {
            BrainOpenGLPrimitiveDrawing::drawPolygon(coords,
                                                     dummyNormals,
                                                     m_selectionBoxRGBA);
        }
        else if (drawOutlineCircleFlag) {
            const float diameter = halfWidthHeight;
            glPushMatrix();
            glScaled(diameter, diameter, 1.0);
            m_rotationHandleCircle->draw(m_selectionBoxRGBA);
            glPopMatrix();
        }
    }

    glPopMatrix();
}

/**
 * Draw sizing handles around a one-dimensional annotation.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param firstPoint
 *     Top right corner of annotation.
 * @param secondPoint
 *     Top left corner of annotation.
 * @param lineThickness
 *     Thickness of line (when enabled).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationOneDimSizingHandles(AnnotationFile* annotationFile,
                                                                             Annotation* annotation,
                                                                             const float firstPoint[3],
                                                                        const float secondPoint[3],
                                                                        const float lineThickness)
{
    float lengthVector[3];
    MathFunctions::subtractVectors(secondPoint, firstPoint, lengthVector);
    MathFunctions::normalizeVector(lengthVector);
    
    const float dx = secondPoint[0] - firstPoint[0];
    const float dy = secondPoint[1] - firstPoint[1];
    
    const float cornerSquareSize = 3.0 + lineThickness;
    const float directionVector[3] = {
        lengthVector[0] * cornerSquareSize,
        lengthVector[1] * cornerSquareSize,
        0.0
    };
    
    const float firstPointSymbolXYZ[3] = {
        firstPoint[0] - directionVector[0],
        firstPoint[1] - directionVector[1],
        firstPoint[2] - directionVector[2]
    };
    
    const float secondPointSymbolXYZ[3] = {
        secondPoint[0] + directionVector[0],
        secondPoint[1] + directionVector[1],
        secondPoint[2] + directionVector[2]
    };
    
    float rotationAngle = 0.0;
    if ((dy != 0.0) && (dx != 0.0)) {
        const float angleRadians = std::atan2(dx, dy);
        rotationAngle = MathFunctions::toDegrees(angleRadians);
    }
    
    /*
     * Symbol for first coordinate is a little bigger
     */
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START,
                         annotationFile,
                         annotation,
                         firstPointSymbolXYZ,
                         cornerSquareSize + 2.0,
                         rotationAngle);
    }
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END,
                         annotationFile,
                         annotation,
                         secondPointSymbolXYZ,
                         cornerSquareSize,
                         rotationAngle);
    }
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        const float midPointXYZ[3] = {
            (firstPoint[0] + secondPoint[0]) / 2.0,
            (firstPoint[1] + secondPoint[1]) / 2.0,
            (firstPoint[2] + secondPoint[2]) / 2.0
        };
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION,
                         annotationFile,
                         annotation,
                         midPointXYZ,
                         cornerSquareSize,
                         rotationAngle);
    }
}

/**
 * Expand a box by given amounts in X and Y.
 *
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * @param extraSpaceX
 *     Extra space to add in X.
 * @param extraSpaceY
 *     Extra space to add in Y.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::expandBox(float bottomLeft[3],
                                                     float bottomRight[3],
                                                     float topRight[3],
                                                     float topLeft[3],
                                                     const float extraSpaceX,
                                                     const float extraSpaceY)
{
    float widthVector[3];
    MathFunctions::subtractVectors(topRight, topLeft, widthVector);
    MathFunctions::normalizeVector(widthVector);
    
    float heightVector[3];
    MathFunctions::subtractVectors(topLeft, bottomLeft, heightVector);
    MathFunctions::normalizeVector(heightVector);
    
    const float widthSpacingX = extraSpaceX * widthVector[0];
    const float widthSpacingY = extraSpaceY * widthVector[1];
    
    const float heightSpacingX = extraSpaceX * heightVector[0];
    const float heightSpacingY = extraSpaceY * heightVector[1];
    

    topLeft[0] += (-widthSpacingX + heightSpacingX);
    topLeft[1] += (-widthSpacingY + heightSpacingY);
    
    topRight[0] += (widthSpacingX + heightSpacingX);
    topRight[1] += (widthSpacingY + heightSpacingY);

    bottomLeft[0] += (-widthSpacingX - heightSpacingX);
    bottomLeft[1] += (-widthSpacingY - heightSpacingY);

    bottomRight[0] += (widthSpacingX - heightSpacingX);
    bottomRight[1] += (widthSpacingY - heightSpacingY);
}

/**
 * Draw sizing handles around a two-dimensional annotation.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * @param lineThickness
 *     Thickness of line (when enabled).
 * @param rotationAngle
 *     Rotation of the annotation.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationTwoDimSizingHandles(AnnotationFile* annotationFile,
                                                                             Annotation* annotation,
                                                                             const float bottomLeft[3],
                                                                        const float bottomRight[3],
                                                                        const float topRight[3],
                                                                        const float topLeft[3],
                                                                        const float lineThickness,
                                                                        const float rotationAngle)
{
    float heightVector[3];
    MathFunctions::subtractVectors(topLeft, bottomLeft, heightVector);
    MathFunctions::normalizeVector(heightVector);

    const float innerSpacing = 2.0 + (lineThickness / 2.0);
        float handleTopLeft[3];
        float handleTopRight[3];
        float handleBottomRight[3];
        float handleBottomLeft[3];
        for (int32_t i = 0; i < 3; i++) {
            handleTopLeft[i]     = topLeft[i];
            handleTopRight[i]    = topRight[i];
            handleBottomRight[i] = bottomRight[i];
            handleBottomLeft[i]  = bottomLeft[i];
        }
    expandBox(handleBottomLeft, handleBottomRight, handleTopRight, handleTopLeft, innerSpacing, innerSpacing);
    
    if (! m_selectionModeFlag) {
        std::vector<float> coords;
        coords.insert(coords.end(), handleBottomLeft,  handleBottomLeft + 3);
        coords.insert(coords.end(), handleBottomRight, handleBottomRight + 3);
        coords.insert(coords.end(), handleTopRight,    handleTopRight + 3);
        coords.insert(coords.end(), handleTopLeft,     handleTopLeft + 3);
        
        BrainOpenGLPrimitiveDrawing::drawLineLoop(coords,
                                                  m_selectionBoxRGBA,
                                                  2.0);
    }
    
    const float handleLeft[3] = {
        (handleBottomLeft[0] + handleTopLeft[0]) / 2.0,
        (handleBottomLeft[1] + handleTopLeft[1]) / 2.0,
        (handleBottomLeft[2] + handleTopLeft[2]) / 2.0,
    };
    
    const float handleRight[3] = {
        (handleBottomRight[0] + handleTopRight[0]) / 2.0,
        (handleBottomRight[1] + handleTopRight[1]) / 2.0,
        (handleBottomRight[2] + handleTopRight[2]) / 2.0,
    };
    
    const float handleBottom[3] = {
        (handleBottomLeft[0] + handleBottomRight[0]) / 2.0,
        (handleBottomLeft[1] + handleBottomRight[1]) / 2.0,
        (handleBottomLeft[2] + handleBottomRight[2]) / 2.0,
    };
    
    const float handleTop[3] = {
        (handleTopLeft[0] + handleTopRight[0]) / 2.0,
        (handleTopLeft[1] + handleTopRight[1]) / 2.0,
        (handleTopLeft[2] + handleTopRight[2]) / 2.0,
    };
    
    const float sizeHandleSize = 5.0;
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT,
                         annotationFile,
                         annotation,
                         handleBottomLeft,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT,
                         annotationFile,
                         annotation,
                         handleBottomRight,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT,
                         annotationFile,
                         annotation,
                         handleTopRight,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT,
                         annotationFile,
                         annotation,
                         handleTopLeft,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP,
                         annotationFile,
                         annotation,
                         handleTop,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM,
                         annotationFile,
                         annotation,
                         handleBottom,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT,
                         annotationFile,
                         annotation,
                         handleRight,
                         sizeHandleSize,
                         rotationAngle);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT,
                         annotationFile,
                         annotation,
                         handleLeft,
                         sizeHandleSize,
                         rotationAngle);
    }
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        
        float handleOffset[3] = {
            handleTop[0],
            handleTop[1],
            handleTop[2]
        };
        
        if (annotation->getType() == AnnotationTypeEnum::TEXT) {
            const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(annotation);
            CaretAssert(textAnn);
            
            /*
             * The rotation point of a text annotation
             * is adjusted for the horizontal alignment.
             */
            switch (textAnn->getHorizontalAlignment()) {
                case AnnotationTextAlignHorizontalEnum::CENTER:
                    break;
                case AnnotationTextAlignHorizontalEnum::LEFT:
                    handleOffset[0] = handleTopLeft[0];
                    handleOffset[1] = handleTopLeft[1];
                    handleOffset[2] = handleTopLeft[2];
                    break;
                case AnnotationTextAlignHorizontalEnum::RIGHT:
                    handleOffset[0] = handleTopRight[0];
                    handleOffset[1] = handleTopRight[1];
                    handleOffset[2] = handleTopRight[2];
                    break;
            }
        }

        const float rotationOffset = sizeHandleSize * 3.0;
        const float handleRotation[3] = {
            handleOffset[0] + (rotationOffset * heightVector[0]),
            handleOffset[1] + (rotationOffset * heightVector[1]),
            handleOffset[2] + (rotationOffset * heightVector[2])
        };
        
        const float handleRotationLineEnd[3] = {
            handleOffset[0] + (rotationOffset * 0.75 * heightVector[0] ),
            handleOffset[1] + (rotationOffset * 0.75 * heightVector[1]),
            handleOffset[2] + (rotationOffset * 0.75 * heightVector[2])
        };
        
        /*
         * Rotation handle and line connecting rotation handle to selection box
         */
        std::vector<float> coords;
        coords.insert(coords.end(), handleRotationLineEnd, handleRotationLineEnd + 3);
        coords.insert(coords.end(), handleOffset, handleOffset + 3);
        BrainOpenGLPrimitiveDrawing::drawLines(coords,
                                               m_selectionBoxRGBA,
                                               2.0);
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION,
                         annotationFile,
                         annotation,
                         handleRotation,
                         sizeHandleSize,
                         rotationAngle);
    }
}

/**
 * Set the color for drawing the selection box and handles.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::setSelectionBoxColor()
{
    /*
     * Use the foreground color but reduce the intensity and saturation.
     */
    m_selectionBoxRGBA[0] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[0];
    m_selectionBoxRGBA[1] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[1];
    m_selectionBoxRGBA[2] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[2];
    m_selectionBoxRGBA[3] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[3];
    
    QColor color(m_selectionBoxRGBA[0],
                 m_selectionBoxRGBA[1],
                 m_selectionBoxRGBA[2],
                 m_selectionBoxRGBA[3]);
    
    qreal hue = 0.0;
    qreal saturation = 0.0;
    qreal value = 0.0;
    
    color.getHsvF(&hue,
                  &saturation,
                  &value);
    
    saturation *= 0.8;
    value      *= 0.8;
    
    color.setHsvF(hue,
                  saturation,
                  value);
    
    m_selectionBoxRGBA[0] = static_cast<uint8_t>(color.red());
    m_selectionBoxRGBA[1] = static_cast<uint8_t>(color.green());
    m_selectionBoxRGBA[2] = static_cast<uint8_t>(color.blue());
}


/**
 * Is the annotation drawn with depth testing enabled (based upon coordinate space)?
 *
 * @param annotation
 *     Annotation that will be drawn.
 * @return 
 *     True if the annotation is drawn with depth testing, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::isDrawnWithDepthTesting(const Annotation* /*annotation*/)
{
    return true;
    
/*
    bool depthTestFlag = false;
    
    switch (annotation->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            depthTestFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::PIXELS:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            depthTestFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }

    return depthTestFlag;
*/
}

/**
 * Set the depth testing to the given status.
 *
 * @param newDepthTestingStatus
 *     New status for depth testing.
 * @return
 *     Depth testing status PRIOR to applying the new depth testing status.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::setDepthTestingStatus(const bool newDepthTestingStatus)
{
    GLboolean savedStatus = GL_FALSE;
    glGetBooleanv(GL_DEPTH_TEST, &savedStatus);
    
    if (newDepthTestingStatus) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    
    return (savedStatus == GL_TRUE);
}



