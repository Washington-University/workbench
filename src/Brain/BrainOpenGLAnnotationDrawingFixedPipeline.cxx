
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

#include <array>
#include <cmath>
#include <limits>

#include <QColor>

#define __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__
#include "BrainOpenGLAnnotationDrawingFixedPipeline.h"
#undef __BRAIN_OPEN_G_L_ANNOTATION_DRAWING_FIXED_PIPELINE_DECLARE__

#include "AnnotationBox.h"
#include "AnnotationBrowserTab.h"
#include "AnnotationColorBar.h"
#include "AnnotationColorBarSection.h"
#include "AnnotationColorBarNumericText.h"
#include "AnnotationCoordinate.h"
#include "AnnotationFile.h"
#include "AnnotationImage.h"
#include "AnnotationLine.h"
#include "AnnotationManager.h"
#include "AnnotationOval.h"
#include "AnnotationPolyLine.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationPercentSizeText.h"
#include "AnnotationScaleBar.h"
#include "AnnotationText.h"
#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrainOpenGLTextRenderInterface.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnum.h"
#include "CaretLogger.h"
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesAnnotation.h"
#include "DisplayPropertiesAnnotationTextSubstitution.h"
#include "DisplayPropertiesSamples.h"
#include "DrawingViewportContent.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventBrowserTabGet.h"
#include "EventDrawingViewportContentGet.h"
#include "EventManager.h"
#include "EventOpenGLObjectToWindowTransform.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3f.h"
#include "GraphicsPrimitiveV3fC4f.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsPrimitiveV3fN3f.h"
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsShape.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "HistologySlice.h"
#include "IdentificationWithColor.h"
#include "MathFunctions.h"
#include "Matrix4x4.h"
#include "SamplesFile.h"
#include "SelectionManager.h"
#include "SelectionItemAnnotation.h"
#include "Surface.h"
#include "TopologyHelper.h"

using namespace caret;

static const bool debugFlag = false;
    
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
m_volumeSliceThickness(0.0),
m_histologySpacePlaneValid(false),
m_histologySliceThickness(0.0)
{
    m_volumeSpacePlane = Plane();
    
    CaretAssert(brainOpenGLFixedPipeline);
    
    m_dummyAnnotationFile = new AnnotationFile(AnnotationFile::ANNOTATION_FILE_DUMMY_FOR_DRAWING);
    m_dummyAnnotationFile->setFileName("DummyFileForDrawing."
                                       + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::ANNOTATION));

    float unusedLineWidthMaximum(0);
    BrainOpenGL::getMinMaxLineWidth(m_lineWidthMinimum,
                                    unusedLineWidthMaximum);
}

/**
 * Destructor.
 */
BrainOpenGLAnnotationDrawingFixedPipeline::~BrainOpenGLAnnotationDrawingFixedPipeline()
{
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
 * Get the drawing space coordinate for display of the annotation.
 *
 * @param annotation
 *     The annotation.
 * @param coordinate
 *     The annotation coordinate whose window coordinate is computed.
 * @param surfaceDisplayed
 *     Surface that is displayed (may be NULL !)
 * @param xyzOut
 *     Output containing the drawing space coordinate.
 * @return
 *     True if the drawing space coordinate is valid, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::getAnnotationDrawingSpaceCoordinate(const Annotation* annotation,
                                                                               const AnnotationCoordinate* coordinate,
                                                                               const Surface* surfaceDisplayed,
                                                                               float xyzOut[3]) const
{
    CaretAssert(annotation);
    const AnnotationCoordinateSpaceEnum::Enum annotationCoordSpace = annotation->getCoordinateSpace();
    
    float modelXYZ[3]  = { 0.0, 0.0, 0.0 };
    bool modelXYZValid = false;
    
    float drawingSpaceXYZ[3] = { 0.0, 0.0, 0.0 };
    bool drawingSpaceXYZValid = false;
    
    float annotationXYZ[3];
    coordinate->getXYZ(annotationXYZ);
    
    switch (annotationCoordSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            modelXYZ[0] = annotationXYZ[0];
            modelXYZ[1] = annotationXYZ[1];
            modelXYZ[2] = annotationXYZ[2];
            modelXYZValid = true;
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            modelXYZ[0] = annotationXYZ[0];
            modelXYZ[1] = annotationXYZ[1];
            modelXYZ[2] = annotationXYZ[2];
            modelXYZValid = true;
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            modelXYZ[0] = annotationXYZ[0];
            modelXYZ[1] = annotationXYZ[1];
            modelXYZ[2] = annotationXYZ[2];
            modelXYZValid = true;
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            viewportToOpenGLWindowCoordinate(annotationXYZ, drawingSpaceXYZ);
            drawingSpaceXYZValid = true;
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            modelXYZ[0] = annotationXYZ[0];
            modelXYZ[1] = annotationXYZ[1];
            modelXYZ[2] = annotationXYZ[2];
            modelXYZValid = true;
            
            if (m_histologySpacePlaneValid) {
                modelXYZValid = false;
                
                Vector3D planeXYZ;
                Vector3D stereotaxicXYZ;
                float distanceToSlice(0.0);
                if (m_histologySlice->projectStereotaxicXyzToSlice(modelXYZ,
                                                                   stereotaxicXYZ,
                                                                   distanceToSlice,
                                                                   planeXYZ)) {
                    const float distToPlaneAbs = std::fabs(distanceToSlice);
                    const float halfSliceThickness = ((m_histologySliceThickness > 0.0)
                                                      ? (m_histologySliceThickness / 2.0)
                                                      : 1.0);
                    if (distToPlaneAbs < halfSliceThickness) {
                        /*
                         * Histology slices are drawn in 'Plane' coordinates
                         */
                        modelXYZ[0] = planeXYZ[0];
                        modelXYZ[1] = planeXYZ[1];
                        modelXYZ[2] = planeXYZ[2];
                        modelXYZValid = true;
                    }
                }
            }

            if (m_volumeSpacePlane.isValidPlane()) {
                float xyzFloat[3] = {
                    modelXYZ[0],
                    modelXYZ[1],
                    modelXYZ[2]
                };
                const float distToPlaneAbs = std::fabs(m_volumeSpacePlane.signedDistanceToPlane(xyzFloat));
                const float halfSliceThickness = ((m_volumeSliceThickness > 0.0)
                                                  ? (m_volumeSliceThickness / 2.0)
                                                  : 1.0);
                if (distToPlaneAbs < halfSliceThickness) {
                    modelXYZValid = true;
                    
                    float projectedPoint[3];
                    m_volumeSpacePlane.projectPointToPlane(modelXYZ, projectedPoint);
                }
                else {
                    modelXYZValid = false;
                }
            }
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
                
                /*
                 * Always use surface offset vector reported by annotation since 
                 * annotations with multiple coordinates must use the same offset
                 */
                annotationOffsetVector = annotation->getSurfaceOffsetVectorType();
                
                const StructureEnum::Enum surfaceStructure = surfaceDisplayed->getStructure();
                const int32_t surfaceNumberOfNodes = surfaceDisplayed->getNumberOfNodes();
                if ((surfaceStructure == annotationStructure)
                    && (surfaceNumberOfNodes == annotationNumberOfNodes)) {
                    if ((annotationNodeIndex >= 0)
                        && (annotationNodeIndex < surfaceNumberOfNodes)) {
                        float nodeXYZ[3];
                        surfaceDisplayed->getCoordinate(annotationNodeIndex,
                                                        nodeXYZ);
                        modelXYZ[0] = nodeXYZ[0];
                        modelXYZ[1] = nodeXYZ[1];
                        modelXYZ[2] = nodeXYZ[2];
                        
                        
                        
                        float offsetUnitVector[3] = { 0.0, 0.0, 0.0 };
                        
                        /*
                         * For a flat surface, ALWAYS use the normal vector.
                         * Using the centroid will not work as there is no
                         * "z depth" so it will incorrectly offset in the XY-plane.
                         *
                         */
                        if (surfaceDisplayed->getSurfaceType() == SurfaceTypeEnum::FLAT) {
                            annotationOffsetVector = AnnotationSurfaceOffsetVectorTypeEnum::SURFACE_NORMAL;
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
                            }
                                break;
                            case AnnotationSurfaceOffsetVectorTypeEnum::SURFACE_NORMAL:
                            {
                                const float* normalVector = surfaceDisplayed->getNormalVector(annotationNodeIndex);
                                offsetUnitVector[0] = normalVector[0];
                                offsetUnitVector[1] = normalVector[1];
                                offsetUnitVector[2] = normalVector[2];
                            }
                                break;
                            case AnnotationSurfaceOffsetVectorTypeEnum::TANGENT:
                                break;
                        }
                        
                        modelXYZ[0] += (offsetUnitVector[0] * annotationOffsetLength);
                        modelXYZ[1] += (offsetUnitVector[1] * annotationOffsetLength);
                        modelXYZ[2] += (offsetUnitVector[2] * annotationOffsetLength);
                        modelXYZValid = true;
                    }
                }
            }
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            viewportToOpenGLWindowCoordinate(annotationXYZ, drawingSpaceXYZ);
            drawingSpaceXYZValid = true;
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
        {
            drawingSpaceXYZ[0] = annotationXYZ[0];
            drawingSpaceXYZ[1] = annotationXYZ[1];
            drawingSpaceXYZ[2] = annotationXYZ[2];
            drawingSpaceXYZValid = true;

            const bool drawCrossFlag = false;
            if (drawCrossFlag) {
                const float redRGBA[4] = { 1.0f, 0.0, 0.0, 1.0f };
                std::unique_ptr<GraphicsPrimitiveV3f> crossShape = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_LINES,
                                                                                                                                            redRGBA));
                crossShape->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
                crossShape->addVertex(drawingSpaceXYZ[0],
                                      drawingSpaceXYZ[1] - 10);
                crossShape->addVertex(drawingSpaceXYZ[0],
                                      drawingSpaceXYZ[1] + 10);
                crossShape->addVertex(drawingSpaceXYZ[0] - 10,
                                      drawingSpaceXYZ[1]);
                crossShape->addVertex(drawingSpaceXYZ[0] + 10,
                                      drawingSpaceXYZ[1]);
                GraphicsEngineDataOpenGL::draw(crossShape.get());
            }
        }
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            viewportToOpenGLWindowCoordinate(annotationXYZ, drawingSpaceXYZ);
            drawingSpaceXYZValid = true;
            break;
    }
    
    if (modelXYZValid) {
        /*
         * Convert model space coordinates to window coordinates
         * as all annotations are drawn in window coordinates.
         */
        
        if (convertModelToWindowCoordinate(modelXYZ, drawingSpaceXYZ)) {
            drawingSpaceXYZValid = true;
        }
        else {
            CaretLogSevere("Failed to convert model coordinates to drawing space coordinates for annotation drawing.");
        }
    }
    
    if (drawingSpaceXYZValid) {
        xyzOut[0] = drawingSpaceXYZ[0];
        xyzOut[1] = drawingSpaceXYZ[1];
        xyzOut[2] = drawingSpaceXYZ[2];
    }
    
    return drawingSpaceXYZValid;
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
    CaretAssert(m_transformEvent);
    CaretAssert(m_transformEvent->isValid());
    m_transformEvent->transformPoint(modelXYZ, windowXYZOut);
    windowXYZOut[0] -= m_modelSpaceViewport[0];
    windowXYZOut[1] -= m_modelSpaceViewport[1];
    return true;
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
BrainOpenGLAnnotationDrawingFixedPipeline::getAnnotationTwoDimShapeBounds(const AnnotationOneCoordinateShape* annotation2D,
                                                                          const float windowXYZ[3],
                                                                          float bottomLeftOut[3],
                                                                          float bottomRightOut[3],
                                                                          float topRightOut[3],
                                                                          float topLeftOut[3]) const
{
    float viewportWidth  = m_modelSpaceViewport[2];
    float viewportHeight = m_modelSpaceViewport[3];
    
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
                                                                                        m_textDrawingFlags,
                                                                                        windowXYZ[0], windowXYZ[1], windowXYZ[2],
                                                                                        viewportWidth, viewportHeight,
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
    
    BoundingBox bb;
    if (boundsValid) {
        bb.set(bottomLeftOut, bottomRightOut, topRightOut, topLeftOut);
    }
    annotation2D->setDrawnInWindowBounds(m_inputs->m_windowIndex, bb);

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
BrainOpenGLAnnotationDrawingFixedPipeline::drawModelSpaceSamplesOnVolumeSlice(Inputs* inputs,
                                                                              const Plane& plane,
                                                                              const float sliceThickness)
{
    CaretAssert(inputs);
    m_inputs = inputs;
    m_surfaceViewScaling = 1.0f;
    
    const DisplayPropertiesSamples* dps(m_inputs->m_brain->getDisplayPropertiesSamples());
    if ( ! dps->isDisplaySamples()) {
        return;
    }
    
    if (plane.isValidPlane()) {
        m_volumeSpacePlane = plane;
        
        std::vector<AnnotationColorBar*> emptyColorBars;
        std::vector<AnnotationScaleBar*> emptyScaleBars;
        std::vector<Annotation*> emptyNotInFileAnnotations;
        drawAnnotationsInternal(DrawingDataType::SAMPLES,
                                AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                emptyColorBars,
                                emptyScaleBars,
                                emptyNotInFileAnnotations,
                                NULL,
                                sliceThickness);
    }
    
    m_volumeSpacePlane = Plane();
    m_inputs = NULL;
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
    m_surfaceViewScaling = 1.0f;
    
    if (plane.isValidPlane()) {
        m_volumeSpacePlane = plane;
        
        std::vector<AnnotationColorBar*> emptyColorBars;
        std::vector<AnnotationScaleBar*> emptyScaleBars;
        std::vector<Annotation*> emptyNotInFileAnnotations;
        drawAnnotationsInternal(DrawingDataType::ANNOTATIONS,
                                AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                emptyColorBars,
                                emptyScaleBars,
                                emptyNotInFileAnnotations,
                                NULL,
                                sliceThickness);
    }
    
    m_volumeSpacePlane = Plane();
    m_inputs = NULL;
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
BrainOpenGLAnnotationDrawingFixedPipeline::drawModelSpaceAnnotationsOnHistologySlice(Inputs* inputs,
                                                                                     const HistologySlice* histologySlice,
                                                                                     const float sliceThickness)
{
    CaretAssert(inputs);
    m_inputs = inputs;
    m_surfaceViewScaling = 1.0f;
    m_histologySpacePlaneValid = false;
    m_histologySlice = histologySlice;
    if (m_histologySlice != NULL) {
        if (m_histologySlice->getStereotaxicPlane().isValidPlane()) {
            m_histologySpacePlaneValid = true;
            
            std::vector<AnnotationColorBar*> emptyColorBars;
            std::vector<AnnotationScaleBar*> emptyScaleBars;
            std::vector<Annotation*> emptyNotInFileAnnotations;
            drawAnnotationsInternal(DrawingDataType::ANNOTATIONS,
                                    AnnotationCoordinateSpaceEnum::STEREOTAXIC,
                                    emptyColorBars,
                                    emptyScaleBars,
                                    emptyNotInFileAnnotations,
                                    NULL,
                                    sliceThickness);
        }
    }
    
    m_histologySpacePlaneValid = false;
    m_histologySlice = NULL;
    
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
 * @param scalebars
 *     Scalebars that will be drawn.
 * @param notInFileAnnotations
 *     Annotations that are not in a file but need to be drawn.
 * @param surfaceDisplayed
 *     In not NULL, surface no which annotations are drawn.
 * @param surfaceViewScaling
 *     Scaling of the viewed surface.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotations(Inputs* inputs,
                                                           const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                                                           std::vector<AnnotationColorBar*>& colorBars,
                                                           std::vector<AnnotationScaleBar*>& scaleBars,
                                                           std::vector<Annotation*>& notInFileAnnotations,
                                                           const Surface* surfaceDisplayed,
                                                           const float surfaceViewScaling)
{
    CaretAssert(inputs);
    m_inputs = inputs;
    m_surfaceViewScaling = surfaceViewScaling;
    
    m_histologySpacePlaneValid = false;
    m_volumeSpacePlane = Plane();
    
    const float sliceThickness = 0.0;
    
    drawAnnotationsInternal(DrawingDataType::ANNOTATIONS,
                            drawingCoordinateSpace,
                            colorBars,
                            scaleBars,
                            notInFileAnnotations,
                            surfaceDisplayed,
                            sliceThickness);
    
    m_inputs = NULL;
}

/**
 * Draw the annotations in the given coordinate space.
 *
 * @param drawingDataType
 *     Type of data being drawn
 * @param drawingCoordinateSpace
 *     Coordinate space of annotation that are drawn.
 * @param colorbars
 *     Colorbars that will be drawn.
 * @param scalebars
 *     Scale that will be drawn.
 * @param notInFileAnnotations
 *     Annotations that are not in a file but need to be drawn.
 * @param surfaceDisplayed
 *     Surface that is displayed.  May be NULL in some instances.
 * @param surfaceDisplayed
 *     In not NULL, surface no which annotations are drawn.
 * @param sliceThickness
 *     Thickness of volume slice
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationsInternal(const DrawingDataType drawingDataType,
                                                                   const AnnotationCoordinateSpaceEnum::Enum drawingCoordinateSpace,
                                                                   std::vector<AnnotationColorBar*>& colorBars,
                                                                   std::vector<AnnotationScaleBar*>& scaleBars,
                                                                   std::vector<Annotation*>& notInFileAnnotations,
                                                                   const Surface* surfaceDisplayed,
                                                                   const float sliceThickness)
{
    CaretAssert(drawingDataType != DrawingDataType::INVALID);
    
    if (m_inputs->m_brain == NULL) {
        return;
    }
    
    EventOpenGLObjectToWindowTransform::SpaceType spaceType = EventOpenGLObjectToWindowTransform::SpaceType::MODEL;
    if (m_volumeSpacePlane.isValidPlane()) {
        spaceType = EventOpenGLObjectToWindowTransform::SpaceType::VOLUME_SLICE_MODEL;
    }
    m_transformEvent.reset(new EventOpenGLObjectToWindowTransform(spaceType));
    EventManager::get()->sendEvent(m_transformEvent.get()->getPointer());
    CaretAssert(m_transformEvent->isValid());
    
    m_histologySliceThickness = sliceThickness;
    m_volumeSliceThickness    = sliceThickness;
    
    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("At beginning of annotation drawing in space "
                                                           + AnnotationCoordinateSpaceEnum::toName(drawingCoordinateSpace)));
    
    /*
     * Get annotation being drawn by the user for this window
     */
    UserInputModeEnum::Enum userInputMode(UserInputModeEnum::Enum::INVALID);
    switch (drawingDataType) {
        case DrawingDataType::ANNOTATIONS:
            userInputMode = UserInputModeEnum::Enum::ANNOTATIONS;
            break;
        case DrawingDataType::INVALID:
            break;
        case DrawingDataType::SAMPLES:
            userInputMode = UserInputModeEnum::Enum::SAMPLES_EDITING;
            break;
    }
    CaretAssert(userInputMode != UserInputModeEnum::Enum::INVALID);
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(userInputMode,
                                                      m_inputs->m_windowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    m_annotationBeingDrawn = NULL;
    m_annotationBeingDrawnViewportHeight = 0;
    if (annDrawEvent.getAnnotation() != NULL) {
        Annotation* ann(annDrawEvent.getAnnotation());
        CaretAssert(ann);
        if (ann->getCoordinateSpace() == drawingCoordinateSpace) {
            m_annotationBeingDrawn = ann;
            m_annotationBeingDrawnViewportHeight = annDrawEvent.getDrawingViewportHeight();
        }
    }

    bool drawAnnotationsFromFilesFlag = true;
    
    const DisplayPropertiesAnnotationTextSubstitution* dpats = m_inputs->m_brain->getDisplayPropertiesAnnotationTextSubstitution();
    m_textDrawingFlags.setDrawSubstitutedText(dpats->isEnableSubstitutions());
    
    bool haveDisplayGroupFlag = true;
    switch (drawingCoordinateSpace) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            haveDisplayGroupFlag = false;
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            haveDisplayGroupFlag = false;
            switch (m_inputs->m_windowDrawingMode) {
                case Inputs::WINDOW_DRAWING_NO:
                    drawAnnotationsFromFilesFlag = false;
                    break;
                case Inputs::WINDOW_DRAWING_YES:
                    break;
            }
            break;
    }
    
    const DisplayPropertiesAnnotation* dpa = m_inputs->m_brain->getDisplayPropertiesAnnotation();
    switch (drawingDataType) {
        case DrawingDataType::ANNOTATIONS:
            /*
             * User may turn off display of all annotations.
             * Color bars and other annotations not in a file are always
             * drawn so continue processing.
             */
            if ( ! dpa->isDisplayAnnotations()) {
                drawAnnotationsFromFilesFlag = false;
            }
            break;
        case DrawingDataType::INVALID:
            break;
        case DrawingDataType::SAMPLES:
            break;
    }
    
    
    /*
     * Note: When window annotations are being drawn, the
     * tab index is invalid so it must be ignored.
     */
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::DISPLAY_GROUP_A;
    if (haveDisplayGroupFlag) {
        displayGroup = dpa->getDisplayGroupForTab(m_inputs->m_tabIndex);
    }
    
    SelectionItemAnnotation* annotationID = m_inputs->m_brain->getSelectionManager()->getAnnotationIdentification();
    switch (drawingDataType) {
        case DrawingDataType::ANNOTATIONS:
            break;
        case DrawingDataType::INVALID:
            break;
        case DrawingDataType::SAMPLES:
            annotationID = m_inputs->m_brain->getSelectionManager()->getSamplesIdentification();
            break;
    }
    
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
    switch (drawingDataType) {
        case DrawingDataType::ANNOTATIONS:
            if (drawAnnotationsFromFilesFlag) {
                m_inputs->m_brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allAnnotationFiles);
            }
            break;
        case DrawingDataType::INVALID:
            break;
        case DrawingDataType::SAMPLES:
        {
            std::vector<SamplesFile*> allSamplesFiles(m_inputs->m_brain->getAllSamplesFiles());
            allAnnotationFiles.insert(allAnnotationFiles.end(),
                                      allSamplesFiles.begin(),
                                      allSamplesFiles.end());
        }
            break;
    }

    m_brainOpenGLFixedPipeline->checkForOpenGLError(NULL, ("Before draw annotations loop in space: "
                                                           + AnnotationCoordinateSpaceEnum::toName(drawingCoordinateSpace)));
    
    /*
     * Draw annotations from all files.
     * NOTE: iFile == numAnnFiles, the annotation colorbars and scale bars
     * and annotation chart labels are drawn
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
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                    break;
                case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                case AnnotationCoordinateSpaceEnum::WINDOW:
                {
                    /*
                     * Note: Positions are in percentages ranging [0.0, 100.0]
                     */
                    const float yStart = 4.0;
                    float y = yStart;
                    int32_t lastTabIndex = -1;
                    bool firstColorBarFlag = true;
                    std::vector<AnnotationOneCoordinateShape*> colorAndScaleBars;
                    colorAndScaleBars.insert(colorAndScaleBars.end(),
                                             colorBars.begin(), colorBars.end());
                    colorAndScaleBars.insert(colorAndScaleBars.end(),
                                             scaleBars.begin(), scaleBars.end());
                    if ( ! colorAndScaleBars.empty()) {
                        for (auto annBar : colorAndScaleBars) {
                            if (annBar->getCoordinateSpace() == drawingCoordinateSpace) {
                                AnnotationColorBarPositionModeEnum::Enum positionMode = AnnotationColorBarPositionModeEnum::AUTOMATIC;
                                AnnotationColorBar* colorBar = dynamic_cast<AnnotationColorBar*>(annBar);
                                AnnotationScaleBar* scaleBar = annBar->castToScaleBar();
                                float x = 14.0;
                                if (colorBar != NULL) {
                                    positionMode = colorBar->getPositionMode();
                                }
                                else {
                                    if (scaleBar != NULL) {
                                        positionMode = scaleBar->getPositionMode();
                                        
                                        /*
                                         * When drawn, color bars are at pixel X=18.5
                                         * so try to align left side of scale bars
                                         */
                                        x = ((18.5 / m_modelSpaceViewport[2]) * 100.0);
                                    }
                                    else {
                                        CaretAssert(0);
                                    }
                                }
                                switch  (positionMode) {
                                    case AnnotationColorBarPositionModeEnum::AUTOMATIC:
                                    {
                                        /*
                                         * Note: Y is incremented twice.  Once to move colorbar
                                         * so that the colorbars bottom is just above the previous
                                         * colorbar or bottom of screen.  Second time to move the
                                         * Y to the top of this annotation.
                                         */
                                        float halfHeight(0.0);
                                        float yOffset(0.0);
                                        if (scaleBar != NULL) {
                                            /*
                                             * Need to get scale bar height
                                             * Will need something other than half height since
                                             * Y=0 is at bottom of scale bar
                                             * Half Height => yOffset, yHeight
                                             */
                                            float bottomLeft[3], bottomRight[3], topRight[3], topLeft[3];
                                            float dummyXYZ[3] { 0.0, 0.0, 0.0 };
                                            scaleBar->getShapeBounds(m_modelSpaceViewport[2], m_modelSpaceViewport[3],
                                                                     dummyXYZ,
                                                                     bottomLeft, bottomRight, topRight, topLeft);
                                            const float scaleBarHeightPixels(topLeft[1] - bottomLeft[1]);
                                            const float halfHeightPixels(scaleBarHeightPixels / 2.0);
                                            halfHeight = (halfHeightPixels / m_modelSpaceViewport[3]) * 100.0;
                                            
                                            /* scale bar has origin at bottom left*/
                                            yOffset = -halfHeight;
                                        }
                                        else {
                                            /* color bar has origin in center-Y */
                                            halfHeight = annBar->getHeight() / 2.0;
                                        }
                                        if (firstColorBarFlag) {
                                            firstColorBarFlag = false;
                                            y = 4;
                                            if (halfHeight > y) {
                                                y = halfHeight;
                                            }
                                            
                                            if (drawingCoordinateSpace == AnnotationCoordinateSpaceEnum::TAB) {
                                                lastTabIndex = annBar->getTabIndex();
                                            }
                                        }
                                        else {
                                            y += halfHeight;
                                            
                                            if (drawingCoordinateSpace == AnnotationCoordinateSpaceEnum::TAB) {
                                                if (annBar->getTabIndex() != lastTabIndex) {
                                                    /*
                                                     * First color bar in tab is at bottom of tab
                                                     */
                                                    y = yStart;
                                                }
                                                lastTabIndex = annBar->getTabIndex();
                                            }
                                        }
                                        
                                        float xyz[3];
                                        annBar->getCoordinate()->getXYZ(xyz);
                                        xyz[0] = x;
                                        xyz[1] = y + yOffset;
                                        annBar->getCoordinate()->setXYZ(xyz);
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
                        annotationsFromFile.insert(annotationsFromFile.end(),
                                                   scaleBars.begin(),
                                                   scaleBars.end());
                    }
                }
                    break;
            }
            
            annotationsFromFile.insert(annotationsFromFile.end(),
                                       notInFileAnnotations.begin(),
                                       notInFileAnnotations.end());
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
            
            if ( ! drawItFlag) {
                continue;
            }
            
            AnnotationTwoCoordinateShape* oneDimAnn = dynamic_cast<AnnotationTwoCoordinateShape*>(annotation);
            AnnotationOneCoordinateShape* twoDimAnn = dynamic_cast<AnnotationOneCoordinateShape*>(annotation);
            AnnotationMultiCoordinateShape* multiCoordAnn = dynamic_cast<AnnotationMultiCoordinateShape*>(annotation);
            AnnotationMultiPairedCoordinateShape* multiPairedCoordAnn(annotation->castToMultiPairedCoordinateShape());

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
            else if (multiCoordAnn != NULL) {
            }
            else if (multiPairedCoordAnn != NULL) {
            }
            else {
                CaretAssertMessage(0, ("Annotation is not derived from One or Two Dim Annotation classes: "
                                       + annotation->toString()));
                continue;
            }
            
            switch (annotationCoordinateSpace) {
                case AnnotationCoordinateSpaceEnum::CHART:
                    break;
                case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                    if (annotation->getCoordinate(0)->getHistologySpaceKey() != m_inputs->m_histologySpaceKey) {
                        continue;
                    }
                    break;
                case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                {
                    const AString annotationMediaFileName(annotation->getCoordinate(0)->getMediaFileName());
                    if (m_inputs->m_mediaFileNames.find(annotationMediaFileName)
                        == m_inputs->m_mediaFileNames.end()) {
                        if (m_inputs->m_mediaFileNamesNoPath.find(annotationMediaFileName)
                            == m_inputs->m_mediaFileNamesNoPath.end()) {
                            continue;
                        }
                    }
                }
                    break;
                case AnnotationCoordinateSpaceEnum::SPACER:
                {
                    const SpacerTabIndex spacerTabIndex = annotation->getSpacerTabIndex();
                    CaretAssert(spacerTabIndex.isValid());
                    if (m_inputs->m_spacerTabIndex != spacerTabIndex) {
                        continue;
                    }
                }
                    break;
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                {
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
                    break;
                case AnnotationCoordinateSpaceEnum::VIEWPORT:
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                {
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
                    break;
            }

            setSelectionBoxColor(annotation);

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
                    
                    float firstPointToPointOnLineNormalizedDisance(0.0);
                    if (selectionInfo.m_polyLineCoordinateIndex >= 0) {
                        const AnnotationMultiCoordinateShape* multiCoordShape = selectionInfo.m_annotation->castToMultiCoordinateShape();
                        const AnnotationMultiPairedCoordinateShape* multiPairedCoordShape(selectionInfo.m_annotation->castToMultiPairedCoordinateShape());
                        if (multiCoordShape != NULL) {
                            const int32_t numCoords(multiCoordShape->getNumberOfCoordinates());
                            const int32_t indexOne(selectionInfo.m_polyLineCoordinateIndex);
                            const int32_t indexTwo((indexOne >= (numCoords - 1))
                                                   ? 0
                                                   : (indexOne + 1));
                            CaretAssertVectorIndex(selectionInfo.m_coordsInWindowXYZ, indexOne);
                            CaretAssertVectorIndex(selectionInfo.m_coordsInWindowXYZ, indexTwo);
                            
                            GLint viewport[4];
                            glGetIntegerv(GL_VIEWPORT,
                                          viewport);

                            const float mouseXYZ[3] {
                                static_cast<float>(m_brainOpenGLFixedPipeline->mouseX - viewport[0]),
                                static_cast<float>(m_brainOpenGLFixedPipeline->mouseY - viewport[1]),
                                0.0
                            };
                            
                            float pointOnLine[3];
                            MathFunctions::nearestPointOnLine3D(selectionInfo.m_coordsInWindowXYZ[indexOne],
                                                                selectionInfo.m_coordsInWindowXYZ[indexTwo],
                                                                mouseXYZ,
                                                                pointOnLine,
                                                                firstPointToPointOnLineNormalizedDisance);
                        }
                        else if (multiPairedCoordShape != NULL) {
                            const int32_t numCoords(multiPairedCoordShape->getNumberOfCoordinates());
                            const int32_t indexOne(selectionInfo.m_polyLineCoordinateIndex);
                            const int32_t indexTwo((indexOne >= (numCoords - 1))
                                                   ? 0
                                                   : (indexOne + 1));
                            CaretAssertVectorIndex(selectionInfo.m_coordsInWindowXYZ, indexOne);
                            CaretAssertVectorIndex(selectionInfo.m_coordsInWindowXYZ, indexTwo);
                            
                            GLint viewport[4];
                            glGetIntegerv(GL_VIEWPORT,
                                          viewport);
                            
                            const float mouseXYZ[3] {
                                static_cast<float>(m_brainOpenGLFixedPipeline->mouseX - viewport[0]),
                                static_cast<float>(m_brainOpenGLFixedPipeline->mouseY - viewport[1]),
                                0.0
                            };
                            
                            float pointOnLine[3];
                            MathFunctions::nearestPointOnLine3D(selectionInfo.m_coordsInWindowXYZ[indexOne],
                                                                selectionInfo.m_coordsInWindowXYZ[indexTwo],
                                                                mouseXYZ,
                                                                pointOnLine,
                                                                firstPointToPointOnLineNormalizedDisance);
                        }
                    }
                    
                    annotationID->setAnnotation(selectionInfo.m_annotationFile,
                                                selectionInfo.m_annotation,
                                                selectionInfo.m_sizingHandle,
                                                selectionInfo.m_polyLineCoordinateIndex,
                                                firstPointToPointOnLineNormalizedDisance,
                                                selectionInfo.m_coordsInWindowXYZ);
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
        if (m_annotationBeingDrawn != NULL) {
            setSelectionBoxColor(m_annotationBeingDrawn);
            
            if (m_annotationBeingDrawn->getType() == AnnotationTypeEnum::TEXT) {
                const AnnotationText* textAnn = dynamic_cast<const AnnotationText*>(m_annotationBeingDrawn);
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
                               const_cast<Annotation*>(m_annotationBeingDrawn),
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
    GraphicsUtilitiesOpenGL::pushMatrix(); /* Projection stack too small (4) on nvidia systems */
    glLoadIdentity();
    glOrtho(0.0, m_modelSpaceViewport[2],
            0.0, m_modelSpaceViewport[3],
            depthRange[0], depthRange[1]);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
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
    GraphicsUtilitiesOpenGL::popMatrix(); /* Projection stack too small (4) on nvidia systems */
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
    
    bool drawnFlag = false;
    
    if (annotation->isInSurfaceSpaceWithTangentOffset()) {
        AnnotationOneCoordinateShape* oneCoordAnn = annotation->castToOneCoordinateShape();
        AnnotationTwoCoordinateShape* twoCoordAnn = annotation->castToTwoCoordinateShape();
        AnnotationMultiCoordinateShape* multiCoordAnn = annotation->castToMultiCoordinateShape();
        AnnotationMultiPairedCoordinateShape* multiPairedCoordAnn(annotation->castToMultiPairedCoordinateShape());
        if (oneCoordAnn != NULL) {
            drawnFlag = drawOneCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                                                        oneCoordAnn,
                                                                        surfaceDisplayed);
        }
        else if (twoCoordAnn != NULL) {
            drawnFlag = drawTwoCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                                                        twoCoordAnn,
                                                                        surfaceDisplayed);
        }
        else if (multiCoordAnn != NULL) {
            drawnFlag = drawMultiCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                                                          multiCoordAnn,
                                                                          surfaceDisplayed);
        }
        else if (multiPairedCoordAnn != NULL) {
            /* Never drawn on surface */
            drawnFlag = false;
        }
        else {
            CaretAssertMessage(0, "Has new annotation type been added");
        }
        
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
            case AnnotationTypeEnum::IMAGE:
            case AnnotationTypeEnum::OVAL:
            case AnnotationTypeEnum::TEXT:
                drawnFlag = drawOneCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                    dynamic_cast<AnnotationOneCoordinateShape*>(annotation),
                                    surfaceDisplayed);
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                break;
            case AnnotationTypeEnum::LINE:
                drawnFlag = drawTwoCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                     dynamic_cast<AnnotationTwoCoordinateShape*>(annotation),
                                     surfaceDisplayed);
                break;
            case AnnotationTypeEnum::POLYHEDRON:
                CaretAssertMessage(0, "Polyhedrons not drawn in surface space");
                break;
            case AnnotationTypeEnum::POLYGON:
                drawnFlag = drawMultiCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                                                              annotation->castToMultiCoordinateShape(),
                                                                              surfaceDisplayed);
                break;
            case AnnotationTypeEnum::POLYLINE:
                drawnFlag = drawMultiCoordinateAnnotationSurfaceTangentOffset(annotationFile,
                                                                            dynamic_cast<AnnotationMultiCoordinateShape*>(annotation),
                                                                            surfaceDisplayed);
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                break;
        }
    }
    else {
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
                drawnFlag = drawBox(annotationFile,
                                    dynamic_cast<AnnotationBox*>(annotation),
                                    surfaceDisplayed);
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                drawnFlag = drawBrowserTab(annotationFile,
                                           dynamic_cast<AnnotationBrowserTab*>(annotation));
                break;
            case AnnotationTypeEnum::COLOR_BAR:
                drawColorBar(annotationFile,
                             dynamic_cast<AnnotationColorBar*>(annotation));
                break;
            case AnnotationTypeEnum::IMAGE:
                drawnFlag = drawImage(annotationFile,
                                      dynamic_cast<AnnotationImage*>(annotation),
                                      surfaceDisplayed);
                break;
            case AnnotationTypeEnum::LINE:
                drawnFlag = drawLine(annotationFile,
                                     dynamic_cast<AnnotationLine*>(annotation),
                                     surfaceDisplayed);
                break;
            case AnnotationTypeEnum::OVAL:
                drawnFlag = drawOval(annotationFile,
                                     dynamic_cast<AnnotationOval*>(annotation),
                                     surfaceDisplayed);
                break;
            case AnnotationTypeEnum::POLYHEDRON:
                drawnFlag = drawMultiPairedCoordinateShape(annotationFile,
                                                           annotation->castToMultiPairedCoordinateShape(),
                                                           surfaceDisplayed);
                break;
            case AnnotationTypeEnum::POLYGON:
                drawnFlag = drawMultiCoordinateShape(annotationFile,
                                                     annotation->castToMultiCoordinateShape(), surfaceDisplayed);
                break;
            case AnnotationTypeEnum::POLYLINE:
                drawnFlag = drawMultiCoordinateShape(annotationFile,
                                                     annotation->castToMultiCoordinateShape(), surfaceDisplayed);
                break;
            case AnnotationTypeEnum::SCALE_BAR:
                drawScaleBar(annotationFile,
                             annotation->castToScaleBar());
                break;
            case AnnotationTypeEnum::TEXT:
                drawnFlag = drawText(annotationFile,
                                     dynamic_cast<AnnotationText*>(annotation),
                                     surfaceDisplayed);
                break;
        }
    }
    
    if (drawnFlag) {
        annotation->setDrawnInWindowStatus(m_inputs->m_windowIndex);
    }
}

/**
 * Draw a two-dimensional annotation that is in surface space with a texture offset.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the anntotation was drawn, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawOneCoordinateAnnotationSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                                    AnnotationOneCoordinateShape* annotation,
                                                                                    const Surface* surfaceDisplayed)
{
    bool drawnFlag = false;
    
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    CaretAssert(surfaceDisplayed);
    
    
    const AnnotationCoordinate* coord = annotation->getCoordinate();
    CaretAssert(coord->getSurfaceOffsetVectorType() == AnnotationSurfaceOffsetVectorTypeEnum::TANGENT);
    StructureEnum::Enum structure;
    int32_t surfaceNumberOfNodes(0);
    int32_t vertexIndex(0);
    float offsetLength(0.0f);
    AnnotationSurfaceOffsetVectorTypeEnum::Enum offsetVectorType;
    coord->getSurfaceSpace(structure, surfaceNumberOfNodes, vertexIndex, offsetLength, offsetVectorType);
    
    if (structure != surfaceDisplayed->getStructure()) {
        return false;
    }
    if (surfaceDisplayed->getNumberOfNodes() != surfaceNumberOfNodes) {
        return false;
    }
    float vertexXYZ[3];
    surfaceDisplayed->getCoordinate(vertexIndex,
                                    vertexXYZ);
    float normalXYZ[3];
    getSurfaceNormalVector(surfaceDisplayed, vertexIndex, normalXYZ);
    const BoundingBox* boundingBox = surfaceDisplayed->getBoundingBox();
    const float surfaceExtentZ = ((surfaceDisplayed->getSurfaceType() == SurfaceTypeEnum::FLAT)
                                  ? boundingBox->getDifferenceY()
                                  : boundingBox->getDifferenceZ());
    
    /*
     * Need to restore model space
     * Recall that all other annotation spaces are drawn in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(m_modelSpaceProjectionMatrix);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(m_modelSpaceModelMatrix);
    int32_t savedViewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  savedViewport);
    glViewport(m_modelSpaceViewport[0],
               m_modelSpaceViewport[1],
               m_modelSpaceViewport[2],
               m_modelSpaceViewport[3]);
    
    glPushMatrix();
    
    /*
     * Matrix that rotates annotation to plane of vertex's normal vector
     */
    Matrix4x4 rotationMatrix;
    rotationMatrix.setMatrixToOpenGLRotationFromVector(normalXYZ);
    double rotationArray[16];
    rotationMatrix.getMatrixForOpenGL(rotationArray);
    
    
    /*
     * Translate to the vertex and then translate using offset
     * vector of text.
     */
    const float offsetVectorXYZ[3] {
        normalXYZ[0] * offsetLength,
        normalXYZ[1] * offsetLength,
        normalXYZ[2] * offsetLength
    };
    glTranslatef(vertexXYZ[0], vertexXYZ[1], vertexXYZ[2]);
    glTranslatef(offsetVectorXYZ[0], offsetVectorXYZ[1], offsetVectorXYZ[2]);
    
    /*
     * Rotate into plane of surface normal vector
     */
    glMultMatrixd(rotationArray);
    
    /*
     * Up vector in local coordinates (text drawing plane)
     * Z points out of text
     */
    const float len(25.0f);
    const float localUpXYZ[3] {
        0.0,
        len,
        0.0
    };
    
    /*
     * Inverse matrix goes back to surface coordinate system
     */
    Matrix4x4 inverseMatrix(rotationMatrix);
    inverseMatrix.invert();
    
    /*
     * Create the plane in which text is drawn
     * (plane is not used for drawing text but is used
     * to orient the text).  The plane is constructed
     * from the vertex's normal vector and the the
     * vertex's XYZ-coordinate.
     */
    Plane textDrawingPlane(normalXYZ,
                           vertexXYZ);
    
    /*
     * Project a point with a large Z-coordinate to the plane
     * and use it to creat the 'text up orientation vector'.
     */
    const float zBig[3] { 0.0, 0.0, 10000.0 };
    float textUpOrientationVectorXYZ[3];
    textDrawingPlane.projectPointToPlane(zBig, textUpOrientationVectorXYZ);
    inverseMatrix.multiplyPoint3(textUpOrientationVectorXYZ);
    
    /*
     * Vector is parallel to surface Z-axis
     */
    float vectorSurfaceAxisZ[3] = { 0.0f, 0.0f, 100.0f };
    inverseMatrix.multiplyPoint3(vectorSurfaceAxisZ);
    
    if (debugFlag) {
        /*
         * Red line is normal vector
         * Green line is "text up" vector
         * Blue points to surface Z-vector
         * Yellow is parallel to surface Z-axis
         */
        const float startXYZ[3] {
            0.0,
            0.0,
            0.0
        };
        const float endXYZ[3] {
            0.0,
            0.0,
            len
        };
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3fv(startXYZ);
        glVertex3fv(endXYZ);
        glColor3f(0.0, 1.0, 0.0);
        glVertex3fv(startXYZ);
        glVertex3fv(localUpXYZ);
        glColor3f(0.0, 0.0, 1.0);
        glVertex3fv(startXYZ);
        glVertex3fv(textUpOrientationVectorXYZ);
        glColor3f(1.0, 1.0, 0.0);
        glVertex3fv(startXYZ);
        glVertex3fv(vectorSurfaceAxisZ);
        glEnd();
    }
    
    /*
     * Rotate the text so that the horzontal flow of the text
     * is orthogonal to the 'text up orientation vector'.
     */
    const float orientationUpAngle = MathFunctions::angleInDegreesBetweenVectors(localUpXYZ,
                                                                                 vectorSurfaceAxisZ);
    
    if (debugFlag) {
        std::cout << "Annotation: " <<  annotation->toString() << std::endl;
        std::cout << "   Plane: " << textDrawingPlane.toString() << std::endl;
        std::cout << "   Local Up Vector: " << AString::fromNumbers(localUpXYZ, 3, ", ") << std::endl;
        std::cout << "   Angle: " << orientationUpAngle << std::endl;
    }
    
    /*
     * Do not adjust tangent text on flat surfaces
     */
    if (surfaceDisplayed->getSurfaceType() != SurfaceTypeEnum::FLAT) {
        /*
         * Rotates annotation so that its horizontal axis is aligned with the 
         * 'best matching' cartesian axis.
         */
        const float angle = annotation->getSurfaceSpaceWithTangentOffsetRotation(structure,
                                                                                 normalXYZ);
        glRotated(angle, 0.0, 0.0, -1.0);
    }
    

    /*
     * Note that text is rotated by the text renderer
     */
    if (annotation->getType() != AnnotationTypeEnum::TEXT) {
        glRotated(annotation->getRotationAngle(), 0.0, 0.0, -1.0);
    }
    
    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
            drawnFlag = drawBoxSurfaceTangentOffset(annotationFile,
                                                    dynamic_cast<AnnotationBox*>(annotation),
                                                    surfaceExtentZ,
                                                    vertexXYZ);
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            CaretAssertMessage(0, "Browser Tab is NEVER drawn in surface space");
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            CaretAssertMessage(0, "Color Bar is NEVER drawn in surface space");
            break;
        case AnnotationTypeEnum::IMAGE:
            drawnFlag = drawImageSurfaceTangentOffset(annotationFile,
                                                      dynamic_cast<AnnotationImage*>(annotation),
                                                      surfaceExtentZ,
                                                      vertexXYZ);
            break;
        case AnnotationTypeEnum::LINE:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::OVAL:
            drawnFlag = drawOvalSurfaceTangentOffset(annotationFile,
                                                     dynamic_cast<AnnotationOval*>(annotation),
                                                     surfaceExtentZ,
                                                     vertexXYZ);
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::POLYGON:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::POLYLINE:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            CaretAssertMessage(0, "Scale Bar is NEVER drawn in surface space");
            break;
        case AnnotationTypeEnum::TEXT:
            drawnFlag = drawTextSurfaceTangentOffset(annotationFile,
                                                     dynamic_cast<AnnotationText*>(annotation),
                                                     surfaceExtentZ,
                                                     vertexXYZ,
                                                     normalXYZ);
            break;
    }
    
    glPopMatrix(); /* restore MODELVIEW */
    
    glViewport(savedViewport[0],
               savedViewport[1],
               savedViewport[2],
               savedViewport[3]);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    return drawnFlag;
}

/**
 * Draw a one-dimensional annotation that is in surface space with a texture offset.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the anntotation was drawn, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawTwoCoordinateAnnotationSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                                    AnnotationTwoCoordinateShape* annotation,
                                                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    CaretAssert(surfaceDisplayed);
    
    bool drawnFlag = false;
    
    
    const BoundingBox* boundingBox = surfaceDisplayed->getBoundingBox();
    const float surfaceExtentZ = boundingBox->getDifferenceZ();
    
    /*
     * Need to restore model space
     * Recall that all other annotation spaces are drawn in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(m_modelSpaceProjectionMatrix);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(m_modelSpaceModelMatrix);
    int32_t savedViewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  savedViewport);
    glViewport(m_modelSpaceViewport[0],
               m_modelSpaceViewport[1],
               m_modelSpaceViewport[2],
               m_modelSpaceViewport[3]);
    
    glPushMatrix();
    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::BROWSER_TAB:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::POLYHEDRON:
        case AnnotationTypeEnum::POLYGON:
        case AnnotationTypeEnum::POLYLINE:
        case AnnotationTypeEnum::SCALE_BAR:
        case AnnotationTypeEnum::TEXT:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::LINE:
            drawnFlag = drawLineSurfaceTangentOffset(annotationFile,
                                                     dynamic_cast<AnnotationLine*>(annotation),
                                                     surfaceDisplayed,
                                                     surfaceExtentZ);
            break;
    }
    
    glPopMatrix(); /* restore MODELVIEW */
    
    glViewport(savedViewport[0],
               savedViewport[1],
               savedViewport[2],
               savedViewport[3]);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    return drawnFlag;
}

/**
 * Draw a one-dimensional annotation that is in surface space with a texture offset.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the anntotation was drawn, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawMultiCoordinateAnnotationSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                                 AnnotationMultiCoordinateShape* annotation,
                                                                                 const Surface* surfaceDisplayed)
{
    CaretAssert(annotationFile);
    CaretAssert(annotation);
    CaretAssert(surfaceDisplayed);
    
    
    bool drawnFlag = false;
    
    
    const BoundingBox* boundingBox = surfaceDisplayed->getBoundingBox();
    const float surfaceExtentZ = boundingBox->getDifferenceZ();
    
    /*
     * Need to restore model space
     * Recall that all other annotation spaces are drawn in window space
     */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixd(m_modelSpaceProjectionMatrix);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixd(m_modelSpaceModelMatrix);
    int32_t savedViewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  savedViewport);
    glViewport(m_modelSpaceViewport[0],
               m_modelSpaceViewport[1],
               m_modelSpaceViewport[2],
               m_modelSpaceViewport[3]);
    
    glPushMatrix();
    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::BROWSER_TAB:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::SCALE_BAR:
        case AnnotationTypeEnum::TEXT:
        case AnnotationTypeEnum::LINE:
        case AnnotationTypeEnum::POLYHEDRON:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::POLYGON:
            drawnFlag = drawMultiCoordinateShapeSurfaceTangentOffset(annotationFile,
                                                                     annotation->castToMultiCoordinateShape(),
                                                                     surfaceDisplayed,
                                                                     surfaceExtentZ);
            break;
        case AnnotationTypeEnum::POLYLINE:
            drawnFlag = drawMultiCoordinateShapeSurfaceTangentOffset(annotationFile,
                                                                     annotation->castToMultiCoordinateShape(),
                                                                     surfaceDisplayed,
                                                                     surfaceExtentZ);
            break;
    }
    
    glPopMatrix(); /* restore MODELVIEW */
    
    glViewport(savedViewport[0],
               savedViewport[1],
               savedViewport[2],
               savedViewport[3]);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    return drawnFlag;
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
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawBox(AnnotationFile* annotationFile,
                                                   AnnotationBox* box,
                                                   const Surface* surfaceDisplayed)
{
    CaretAssert(box);
    CaretAssert(box->getType() == AnnotationTypeEnum::BOX);
    
    float annXYZ[3];
    if ( ! getAnnotationDrawingSpaceCoordinate(box,
                                         box->getCoordinate(),
                                         surfaceDisplayed,
                                         annXYZ)) {
        return false;
    }
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(annXYZ[0], annXYZ[1], annXYZ[2]);
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(box, annXYZ,
                               bottomLeft, bottomRight, topRight, topLeft)) {
        return false;
    }
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0f,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0f,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0f
    };
    
    if (box->getLineWidthPercentage() <= 0.0f) {
        convertObsoleteLineWidthPixelsToPercentageWidth(box);
    }
    
    const bool depthTestFlag = isDrawnWithDepthTesting(box,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);

    uint8_t backgroundRGBA[4];
    box->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    box->getLineColorRGBA(foregroundRGBA);

    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    const int32_t invalidPolyLineCoordinateIndex(-1);
    bool drawnFlag = false;
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                                      bottomRight,
                                                      topRight,
                                                      topLeft,
                                                      selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                const float percentHeight = getLineWidthPercentageInSelectionMode(box);
                GraphicsShape::drawBoxOutlineByteColor(bottomLeft,
                                                       bottomRight,
                                                       topRight,
                                                       topLeft,
                                                       selectionColorRGBA,
                                                       GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                       percentHeight);
            }
            
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    box,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                                      bottomRight,
                                                      topRight,
                                                      topLeft,
                                                      backgroundRGBA);
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                    GraphicsShape::drawBoxOutlineByteColor(bottomLeft,
                                                           bottomRight,
                                                           topRight,
                                                           topLeft,
                                                           foregroundRGBA,
                                                           GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                           box->getLineWidthPercentage());
                drawnFlag = true;
            }
        }
        if (box->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                                   box,
                                                   verticesInWindowXYZ,
                                                   bottomLeft,
                                                   bottomRight,
                                                   topRight,
                                                   topLeft,
                                                   s_sizingHandleLineWidthInPixels,
                                                   box->getRotationAngle());
        }
    }

    setDepthTestingStatus(savedDepthTestStatus);
    
    return drawnFlag;
}

/**
 * Draw an annotation box in surface tangent offset space
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param box
 *    box to draw.
 * @param surfaceExtentZ
 *    Z-extent of the surface.
 * @param vertexXYZ
 *    Coordinate of the vertex.
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawBoxSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                       AnnotationBox* box,
                                                                       const float surfaceExtentZ,
                                                                       const float vertexXYZ[3])
{
    CaretAssert(annotationFile);
    CaretAssert(box);
    CaretAssert(box->getType() == AnnotationTypeEnum::BOX);
    
    const float halfWidth  = ((box->getWidth() / 100.0) * surfaceExtentZ) / 2.0;
    const float halfHeight = ((box->getHeight() / 100.0) * surfaceExtentZ) / 2.0;
    float bottomLeft[3]  { -halfWidth, -halfHeight, 0.0f };
    float bottomRight[3] {  halfWidth, -halfHeight, 0.0f };
    float topRight[3]    {  halfWidth,  halfHeight, 0.0f };
    float topLeft[3]     { -halfWidth,  halfHeight, 0.0f };

    const float selectionCenterXYZ[3] = {
        vertexXYZ[0],
        vertexXYZ[1],
        vertexXYZ[2]
    };
    
    float lineThickness = ((box->getLineWidthPercentage() / 100.0)
                                 * surfaceExtentZ);
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        lineThickness = std::max(lineThickness,
                                 s_selectionLineMinimumPixelWidth);
    }
    
    uint8_t backgroundRGBA[4];
    box->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    box->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    bool drawnFlag = false;
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(vertexXYZ[0], vertexXYZ[1], vertexXYZ[2]);

    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                                      bottomRight,
                                                      topRight,
                                                      topLeft,
                                                      selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                GraphicsShape::drawBoxOutlineByteColor(bottomLeft,
                                                       bottomRight,
                                                       topRight,
                                                       topLeft,
                                                       selectionColorRGBA,
                                                       GraphicsPrimitive::LineWidthType::PIXELS,
                                                       lineThickness);
            }
            
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    box,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                                      bottomRight,
                                                      topRight,
                                                      topLeft,
                                                      backgroundRGBA);
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                glPolygonOffset(-1.0, -1.0);
                glEnable(GL_POLYGON_OFFSET_FILL);
                GraphicsShape::drawOutlineRectangleVerticesInMiddle(bottomLeft,
                                                                    bottomRight,
                                                                    topRight,
                                                                    topLeft,
                                                                    lineThickness,
                                                                    foregroundRGBA);
                drawnFlag = true;
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
        }
        if (box->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              box,
                                                    verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              box->getRotationAngle());
        }
    }
    
    return drawnFlag;
}

/**
 * Draw an annotation browser tab.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param browserTab
 *    Browser tab to draw.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawBrowserTab(AnnotationFile* annotationFile,
                                                          AnnotationBrowserTab* browserTab)
{
    CaretAssert(browserTab);
    CaretAssert(browserTab->getType() == AnnotationTypeEnum::BROWSER_TAB);
    
    float annXYZ[3];
    Surface* nullSurface(NULL);
    if ( ! getAnnotationDrawingSpaceCoordinate(browserTab,
                                               browserTab->getCoordinate(),
                                               nullSurface,
                                               annXYZ)) {
        return false;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(browserTab, annXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return false;
    }
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0f,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0f,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0f
    };
    
    if (browserTab->getLineWidthPercentage() <= 0.0f) {
        convertObsoleteLineWidthPixelsToPercentageWidth(browserTab);
    }
    
    const Surface* surfaceDisplayed(NULL);
    const bool depthTestFlag = isDrawnWithDepthTesting(browserTab,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    uint8_t backgroundRGBA[4];
    browserTab->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    browserTab->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    bool drawnFlag = false;
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(annXYZ[0], annXYZ[1], annXYZ[2]);
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_tileTabsManualLayoutUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            const bool alwaysDrawBackgroundForSelectionFlag(true);
            if (drawBackgroundFlag
                || alwaysDrawBackgroundForSelectionFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                                      bottomRight,
                                                      topRight,
                                                      topLeft,
                                                      selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                const float percentHeight = getLineWidthPercentageInSelectionMode(browserTab);
                GraphicsShape::drawBoxOutlineByteColor(bottomLeft,
                                                       bottomRight,
                                                       topRight,
                                                       topLeft,
                                                       selectionColorRGBA,
                                                       GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                       percentHeight);
            }
            
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    browserTab,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                drawnFlag = true;
            }
        }
        if (browserTab->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              browserTab,
                                                   verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              browserTab->getRotationAngle());
        }
        else if (m_inputs->m_tileTabsManualLayoutUserInputModeFlag) {
            drawTabBounds(bottomLeft,
                          bottomRight,
                          topRight,
                          topLeft,
                          foregroundRGBA);
        }
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
    
    return drawnFlag;
}

/**
 * Draw dashed lines around bounds of tab
 * @param bottomLeft
 * @param bottomRight
 * @param topRight
 * @param topLeft
 * @param foregroundRGBA
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawTabBounds(const float bottomLeft[3],
                                                         const float bottomRight[3],
                                                         const float topRight[3],
                                                         const float topLeft[3],
                                                         uint8_t foregroundRGBA[4])
{
    glPushAttrib(GL_LINE_BIT);
    glLineStipple(1, 0xf000);
    glEnable(GL_LINE_STIPPLE);
    glLineWidth(1);
    glColor4ubv(foregroundRGBA);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(bottomLeft);
    glVertex3fv(bottomRight);
    glVertex3fv(topRight);
    glVertex3fv(topLeft);
    glEnd();
    glPopAttrib();
}

/**
 * Draw an annotation scale bar.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param scaleBar
 *    Scale bar to draw.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawScaleBar(AnnotationFile* annotationFile,
                                                        AnnotationScaleBar* scaleBar)
{
    CaretAssert(scaleBar);
    
    if ( ! scaleBar->isDisplayed()) {
        return;
    }
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,
                  viewport);
    
    std::array<float, 3> startXYZ;
    if ( ! getAnnotationDrawingSpaceCoordinate(scaleBar,
                                               scaleBar->getCoordinate(),
                                               NULL,
                                               startXYZ.data())) {
        return;
    }
    
    uint8_t backgroundRGBA[4];
    scaleBar->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    scaleBar->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    bool drawnFlag = false;
    
    bool fontTooSmallFlag(false);
        
    BrainOpenGLTextRenderInterface::DrawingFlags textDrawingFlags;
    textDrawingFlags.setDrawSubstitutedText(false);
    
    const AnnotationPercentSizeText* annLengthText = scaleBar->getLengthTextAnnotation();
    CaretAssert(annLengthText);
    
    AnnotationScaleBar::DrawingInfo scaleBarDrawingInfo;
    scaleBar->getScaleBarDrawingInfo(viewport[2],
                                     viewport[3],
                                     startXYZ,
                                     m_selectionModeFlag,
                                     scaleBarDrawingInfo);
    
    if ( ! scaleBarDrawingInfo.isValid()) {
        return;
    }
    
    const bool depthTestFlag(false);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(startXYZ[0], startXYZ[1], startXYZ[2]);
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                GraphicsShape::drawBoxFilledByteColor(&scaleBarDrawingInfo.m_backgroundBounds[0],
                                                      &scaleBarDrawingInfo.m_backgroundBounds[3],
                                                      &scaleBarDrawingInfo.m_backgroundBounds[6],
                                                      &scaleBarDrawingInfo.m_backgroundBounds[9],
                                                      selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                GraphicsShape::drawBoxFilledByteColor(&scaleBarDrawingInfo.m_barBounds[0],
                                                       &scaleBarDrawingInfo.m_barBounds[3],
                                                       &scaleBarDrawingInfo.m_barBounds[6],
                                                       &scaleBarDrawingInfo.m_barBounds[9],
                                                      selectionColorRGBA);
                
                if (scaleBar->isShowLengthText()) {
                    /*
                     * Drawing box where text is located so text
                     * can be clicked to cause selection
                     */
                    GraphicsShape::drawBoxFilledByteColor(&scaleBarDrawingInfo.m_textBounds[0],
                                                          &scaleBarDrawingInfo.m_textBounds[3],
                                                          &scaleBarDrawingInfo.m_textBounds[6],
                                                          &scaleBarDrawingInfo.m_textBounds[9],
                                                          selectionColorRGBA);
                }
            }
            
            const float selectionCenterXYZ[3] = {
                 (scaleBarDrawingInfo.m_backgroundBounds[0] + scaleBarDrawingInfo.m_backgroundBounds[4]) / 2.0f,
                 (scaleBarDrawingInfo.m_backgroundBounds[1] + scaleBarDrawingInfo.m_backgroundBounds[10]) / 2.0f,
                 scaleBarDrawingInfo.m_backgroundBounds[2]
            };
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    scaleBar,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                /*
                 * Draws background for bar and text
                 */
                GraphicsShape::drawBoxFilledByteColor(&scaleBarDrawingInfo.m_backgroundBounds[0],
                                                      &scaleBarDrawingInfo.m_backgroundBounds[3],
                                                      &scaleBarDrawingInfo.m_backgroundBounds[6],
                                                      &scaleBarDrawingInfo.m_backgroundBounds[9],
                                                      backgroundRGBA);
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                /*
                 * Draws bar only
                 */
                GraphicsShape::drawBoxFilledByteColor(&scaleBarDrawingInfo.m_barBounds[0],
                                                      &scaleBarDrawingInfo.m_barBounds[3],
                                                      &scaleBarDrawingInfo.m_barBounds[6],
                                                      &scaleBarDrawingInfo.m_barBounds[9],
                                                      foregroundRGBA);
                
                if (scaleBar->isShowTickMarks()) {
                    for (const auto& tickBounds : scaleBarDrawingInfo.m_ticksBounds) {
                        GraphicsShape::drawBoxFilledByteColor(&tickBounds[0],
                                                              &tickBounds[3],
                                                              &tickBounds[6],
                                                              &tickBounds[9],
                                                              foregroundRGBA);
                    }
                }
                
                drawnFlag = true;
            }
            
            if (drawnFlag) {
                if (scaleBar->isShowLengthText()) {
                    m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(scaleBarDrawingInfo.m_textStartXYZ[0],
                                                                                            scaleBarDrawingInfo.m_textStartXYZ[1],
                                                                                            scaleBarDrawingInfo.m_textStartXYZ[2],
                                                                                            *annLengthText,
                                                                                            textDrawingFlags);
                    if (annLengthText->isFontTooSmallWhenLastDrawn()) {
                        fontTooSmallFlag = true;
                    }
                }
            }
        }
        
        if (scaleBar->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              scaleBar,
                                                   verticesInWindowXYZ,
                                              &scaleBarDrawingInfo.m_backgroundBounds[0],
                                              &scaleBarDrawingInfo.m_backgroundBounds[3],
                                              &scaleBarDrawingInfo.m_backgroundBounds[6],
                                              &scaleBarDrawingInfo.m_backgroundBounds[9],
                                              s_sizingHandleLineWidthInPixels,
                                              scaleBar->getRotationAngle());
        }
    }
    
    scaleBar->setFontTooSmallWhenLastDrawn(fontTooSmallFlag);
    setDepthTestingStatus(savedDepthTestStatus);
}


/**
 * Draw an annotation color bar.
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
    
    float annXYZ[3];
    if ( ! getAnnotationDrawingSpaceCoordinate(colorBar,
                                         colorBar->getCoordinate(),
                                         NULL,
                                         annXYZ)) {
        return;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(colorBar, annXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return;
    }
    
    const float viewportHeight = m_modelSpaceViewport[3];
    const float viewportWidth  = m_modelSpaceViewport[2];
    
    /*
     * The user sets the total height of the colorbar and the
     * height of the text.
     *
     * There are three items in the colorbar from top to bottom:
     *    (1) The numeric values
     *    (2) The tick marks
     *    (3) The palettes color sections
     */
    float totalHeightPercent = colorBar->getHeight();
    float textHeightPercent        = colorBar->getFontPercentViewportSize();
    float totalHeightPixels = (viewportHeight
                               * (totalHeightPercent / 100.0f));
    
    /*
     * Text is aligned at the top of the characters
     */
    float textHeightPixels = (viewportHeight
                                    * (textHeightPercent / 100.0f));
    
    if (debugFlag) {
        std::cout << "Color bar heights before corrections (pixels) " << std::endl;
        std::cout << "   Text:        " << textHeightPixels << std::endl;
    }
    
    switch (colorBar->getPositionMode()) {
        case AnnotationColorBarPositionModeEnum::AUTOMATIC:
            {
                const float startingX = (bottomLeft[0] + topLeft[0]) / 2.0f;
                float estimatedWidth = estimateColorBarWidth(colorBar,
                                                             textHeightPixels);
                /*
                 * Maximum width is set so that margins on left and right sides
                 * of the color bar are identical.
                 */
                const float maximumWidth = viewportWidth - (startingX * 2.0f);
                if (estimatedWidth > maximumWidth) {
                    /*
                     * Since the width of the text is a function of the text height,
                     * gradually reduce the height of the text until the color bar
                     * width is less than the maximum allowable width.  Note that 
                     * the font heights are not a continuous function so that a
                     * range of "requested font heights" may correspond to one
                     * font height.
                     */
                    const float tooBigTextHeightPixels = textHeightPixels;
                    const float onePercentPixelHeight = textHeightPixels * 0.01;
                    for (int32_t pctSteps = 0; pctSteps < 100; pctSteps++) {
                        textHeightPixels -= onePercentPixelHeight;
                        estimatedWidth = estimateColorBarWidth(colorBar,
                                                               textHeightPixels);
                        if (estimatedWidth <= maximumWidth) {
                            break;
                        }
                    }
                    if (estimatedWidth > maximumWidth) {
                        estimatedWidth = maximumWidth;
                    }
                    
                    /*
                     * Since the height of the text has been reduced, we must also reduce the
                     * height of the color bar.  Otherwise, the color swatches will become taller
                     * (color swatches fill the region of color bar not used by the text).
                     */
                    const float textHeightPixelChangePercent = ((tooBigTextHeightPixels - textHeightPixels)
                                                                / tooBigTextHeightPixels);
                    if (debugFlag) {
                        std::cout << "Too big text height: " << tooBigTextHeightPixels << std::endl;
                        std::cout << "   Reduced to: " << textHeightPixels << std::endl;
                        std::cout << "Text height percentage change: " << textHeightPixelChangePercent << std::endl;
                    }
                    
                    const float tooBigTextHeightPercent = textHeightPercent;
                    textHeightPercent = (textHeightPixels / viewportHeight) * 100.0;
                    const float textPercentChange = tooBigTextHeightPercent - textHeightPercent;
                    totalHeightPercent -= textPercentChange;
                    totalHeightPixels = (viewportHeight
                                         * (totalHeightPercent / 100.0f));
                }
                
                const float shapeWidth = MathFunctions::distance3D(bottomLeft, bottomRight);
                if (estimatedWidth > shapeWidth) {
                    /*
                     * The corners of the color bar must now be recomputed to account
                     * for the reduced dimensions of the color bar.
                     */
                    float bottomUnitVector[3];
                    MathFunctions::createUnitVector(bottomLeft, bottomRight, bottomUnitVector);
                    float topUnitVector[3];
                    MathFunctions::createUnitVector(topLeft, topRight, topUnitVector);
                    float leftUnitVector[3];
                    MathFunctions::createUnitVector(bottomLeft, topLeft, leftUnitVector);
                    if (debugFlag) {
                        std::cout << "Too short bottom right: " << AString::fromNumbers(bottomRight, 3, ", ") << std::endl;
                    }
                    for (int32_t i = 0; i < 3; i++) {
                        topLeft[i]     = bottomLeft[i] + leftUnitVector[i] * totalHeightPixels;
                        topRight[i]    = topLeft[i]    + topUnitVector[i] * estimatedWidth;
                        bottomRight[i] = bottomLeft[i] + bottomUnitVector[i] * estimatedWidth;
                    }
                    if (debugFlag) {
                        std::cout << "   Adjusted bottom right: " << AString::fromNumbers(bottomRight, 3, ", ") << std::endl;
                    }
                }
            }
            break;
        case AnnotationColorBarPositionModeEnum::MANUAL:
            break;
    }
    
    float ticksMarksHeightPercent  = totalHeightPercent * 0.10;
    float sectionsHeightPercent    = totalHeightPercent - (ticksMarksHeightPercent - textHeightPercent);
    
    if (sectionsHeightPercent <= 0.0f) {
        sectionsHeightPercent    = totalHeightPercent * 0.10f;
        textHeightPercent = totalHeightPercent - sectionsHeightPercent;
    }
    const float tickMarksHeightPixels = (viewportHeight
                                         * (ticksMarksHeightPercent / 100.0f));
    const float textOffsetFromTopPixels = 2;
    const float sectionsHeightPixels = (totalHeightPixels
                                        - (textHeightPixels
                                           + textOffsetFromTopPixels
                                           + tickMarksHeightPixels));
    if (debugFlag) {
        std::cout << "Color bar heights after any adjustments (pixels) " << std::endl;
        std::cout << "   Total:       " << totalHeightPixels << std::endl;
        std::cout << "   Text:        " << textHeightPixels << std::endl;
        std::cout << "   Text Offset: " << textOffsetFromTopPixels << std::endl;
        std::cout << "   Ticks:       " << tickMarksHeightPixels << std::endl;
    }

    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0f,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0f,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0f
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(colorBar,
                                                       NULL);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    float backgroundRGBA[4];
    colorBar->getBackgroundColorRGBA(backgroundRGBA);
    float foregroundRGBA[4];
    colorBar->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0.0f);
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(annXYZ[0], annXYZ[1], annXYZ[2]);
    
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        uint8_t selectionColorRGBA[4];
        getIdentificationColor(selectionColorRGBA);
        GraphicsShape::drawBoxFilledByteColor(bottomLeft, bottomRight, topRight, topLeft,
                                              selectionColorRGBA);
        const int32_t invalidPolyLineCoordinateIndex(-1);
        m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                colorBar,
                                                AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                invalidPolyLineCoordinateIndex,
                                                selectionCenterXYZ,
                                                verticesInWindowXYZ));
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
            MathFunctions::expandBox(bgBottomLeft, bgBottomRight, bgTopRight, bgTopLeft, 2, 0);
            
            std::vector<float> bgCoords;
            bgCoords.insert(bgCoords.end(), bgBottomLeft,  bgBottomLeft + 3);
            bgCoords.insert(bgCoords.end(), bgBottomRight, bgBottomRight + 3);
            bgCoords.insert(bgCoords.end(), bgTopRight,    bgTopRight + 3);
            bgCoords.insert(bgCoords.end(), bgTopLeft,     bgTopLeft + 3);
            
            GraphicsShape::drawBoxFilledFloatColor(bgBottomLeft, bgBottomRight, bgTopRight, bgTopLeft,
                                                   backgroundRGBA);
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
        drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                          colorBar,
                                               verticesInWindowXYZ,
                                          bottomLeft,
                                          bottomRight,
                                          topRight,
                                          topLeft,
                                          s_sizingHandleLineWidthInPixels,
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
    MathFunctions::expandBox(bottomLeft, bottomRight, topRight, topLeft, (-tickThickness / 2.0), 0);
    
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
    
    float rgba[4];
    colorBar->getTextColorRGBA(rgba);
    
    const float z = 0.0;

    std::unique_ptr<GraphicsPrimitiveV3f> linesPrimitive(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES,
                                                                                            rgba));
    /*
     * Horizontal line at top of tick marks
     */
    const bool showHorizontalLineFlag = false;
    if (showHorizontalLineFlag) {
        linesPrimitive->addVertex(xTopLeft, yTopLeft, z);
        linesPrimitive->addVertex(xTopRight, yTopRight, z);
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
        
        linesPrimitive->addVertex(tickTopX, tickTopY, z);
        linesPrimitive->addVertex(tickBottomX, tickBottomY, z);
    }
    
    /*
     * Draw lines and use polygon offset to ensure above color bar
     */
    glPolygonOffset(1.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_LINE);
    linesPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, tickThickness);
    GraphicsEngineDataOpenGL::draw(linesPrimitive.get());
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

    float minScalar = 0.0f;
    float maxScalar = 0.0f;
    colorBar->getScalarMinimumAndMaximumValues(minScalar,
                                               maxScalar);
    const float dScalar = maxScalar - minScalar;
    
    const bool printDebugFlag = false;
    if (printDebugFlag) {
        std::cout << qPrintable(QString("minScalar %1, maxScalar %2, dScalar %3").arg(minScalar).arg(maxScalar).arg(dScalar)) << std::endl;
    }
    
    const float z = 0.0f;

    std::unique_ptr<GraphicsPrimitiveV3fC4f> linesPrimitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES));
    linesPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, 1.0f);
    std::unique_ptr<GraphicsPrimitiveV3fC4f> trianglesPrimitive(GraphicsPrimitive::newPrimitiveV3fC4f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLES));
    
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
            const float* rgba = section->getStartRGBA();
            linesPrimitive->addVertex(blX, blY, z, rgba);
            linesPrimitive->addVertex(tlX, tlY, z, rgba);
            
        }
        else {
            const float brX = xBottomLeft + (dx * endNormalizedScalar);
            const float brY = yBottomLeft + (dy * endNormalizedScalar);
            const float trX = xTopLeft    + (dx * endNormalizedScalar);
            const float trY = yTopLeft    + (dy * endNormalizedScalar);
            
            const float* rgbaLeft  = section->getStartRGBA();
            const float* rgbaRight = section->getEndRGBA();
            
            trianglesPrimitive->addVertex(blX, blY, z, rgbaLeft);
            trianglesPrimitive->addVertex(brX, brY, z, rgbaRight);
            trianglesPrimitive->addVertex(tlX, tlY, z, rgbaLeft);
            trianglesPrimitive->addVertex(tlX, tlY, z, rgbaLeft);
            trianglesPrimitive->addVertex(brX, brY, z, rgbaRight);
            trianglesPrimitive->addVertex(trX, trY, z, rgbaRight);
            
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
    /*
     * Lines need to be drawn OVER any quads (otherwise the quads
     * would obscure the lines
     */
    GraphicsEngineDataOpenGL::draw(trianglesPrimitive.get());
    glPolygonOffset(1.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_LINE);
    GraphicsEngineDataOpenGL::draw(linesPrimitive.get());
    glDisable(GL_POLYGON_OFFSET_LINE);
}

/**
 * Estimate the width of the color bar.
 * 
 * @param colorbar
 *    The color bar.
 * @param textHeightInPixels
 *    Height of the text in pixels.
 * @return
 *    Estimated width of the color bar.
 */
float
BrainOpenGLAnnotationDrawingFixedPipeline::estimateColorBarWidth(const AnnotationColorBar* colorBar,
                                                                 const float textHeightInPixels) const
{
    const int32_t numText = colorBar->getNumberOfNumericText();
    if (numText <= 0) {
        return 0.0f;
    }
    
    const int32_t viewportWidth = m_modelSpaceViewport[2];
    const int32_t viewportHeight = m_modelSpaceViewport[3];
    
    const float textPercentageHeight = (textHeightInPixels / viewportHeight) * 100.0;
    
    AnnotationPercentSizeText annText(AnnotationAttributesDefaultTypeEnum::NORMAL);
    annText.setHorizontalAlignment(AnnotationTextAlignHorizontalEnum::CENTER);
    annText.setVerticalAlignment(AnnotationTextAlignVerticalEnum::TOP);
    annText.setFont(colorBar->getFont());
    annText.setFontPercentViewportSize(textPercentageHeight);
    annText.setTextColor(CaretColorEnum::CUSTOM);
    float rgba[4];
    colorBar->getTextColorRGBA(rgba);
    annText.setCustomTextColor(rgba);
    annText.setRotationAngle(colorBar->getRotationAngle());
    
    float windowX = 0.0f;
    float windowY = 0.0f;
    float windowZ = 0.0f;
    
    std::vector<float> allTextWidths;
    
    if (debugFlag) {
        std::cout << "Estimating with pixel height: " << textHeightInPixels << std::endl;
    }
    
    /*
     * Get width of each numerical text value displayed above the color bar
     */
    for (int32_t i = 0; i < numText; i++) {
        const AnnotationColorBarNumericText* numericText = colorBar->getNumericText(i);
        annText.setText(numericText->getNumericText());
        
            {
                float textBottomLeft[3];
                float textBottomRight[3];
                float textTopRight[3];
                float textTopLeft[3];
                m_brainOpenGLFixedPipeline->getTextRenderer()->getBoundsWithoutMarginForTextAtViewportCoords(annText,
                                                                                                             m_textDrawingFlags,
                                                                                                windowX, windowY, windowZ,
                                                                                                viewportWidth, viewportHeight,
                                                                                                textBottomLeft, textBottomRight, textTopRight, textTopLeft);
                float textWidth = MathFunctions::distance3D(textBottomLeft, textBottomRight);
                allTextWidths.push_back(textWidth);
                if (debugFlag) {
                    std::cout << "Width of \"" << numericText->getNumericText() << "\" is " << textWidth << std::endl;
                }
                
            }
    }

    /*
     * A seaparator may be placed between numerical values
     */
    float separatorWidth = 0.0;
    {
        const AString separatorCharacter("0");
        annText.setText(separatorCharacter);
        float textBottomLeft[3];
        float textBottomRight[3];
        float textTopRight[3];
        float textTopLeft[3];
        m_brainOpenGLFixedPipeline->getTextRenderer()->getBoundsForTextAtViewportCoords(annText,
                                                                                        m_textDrawingFlags,
                                                                                        windowX, windowY, windowZ,
                                                                                        viewportWidth, viewportHeight,
                                                                                        textBottomLeft, textBottomRight, textTopRight, textTopLeft);
        separatorWidth = MathFunctions::distance3D(textBottomLeft, textBottomRight);
        if (debugFlag) {
            std::cout << "Separator \"" << separatorCharacter << "\" width: " << separatorWidth << std::endl;
        }
    }
    
    CaretAssert(numText == static_cast<int32_t>(allTextWidths.size()));
    
    CaretAssertVectorIndex(allTextWidths, 0);
    const float firstWidth = allTextWidths[0];
    
    float estimatedWidth = 0.0f;
    if (numText == 1) {
        estimatedWidth = firstWidth;
    }
    else {
        const int32_t lastIndex = numText - 1;
        CaretAssertVectorIndex(allTextWidths, lastIndex);
        const float lastWidth = allTextWidths[lastIndex];
        
        if (numText == 2) {
            estimatedWidth = firstWidth + separatorWidth + lastWidth;
        }
        else {
            /*
             * Each of the numerical values are equally spaced.  The first value is aligned left,
             * the last value is aligned right, and the remaining values are center aligned.
             * Our strategy is examine the interval (width) between each consecutive pair of
             * numerical values and ensure the interval is large enough so that the two
             * numerical text values do not overlap.  Since all intervals are equally spaced,
             * use the largest interval for all intervals and then set the width of the color bar
             * to the sum of these intervals.
             */
            float largestInterval = 0.0f;
            for (int32_t i = 1; i < numText; i++) {
                CaretAssertVectorIndex(allTextWidths, i);
                const float halfWidth = allTextWidths[i] / 2.0f;
                
                float interval = 0.0f;
                if (i == 1) {
                    interval = firstWidth + halfWidth;
                }
                else if (i == lastIndex) {
                    interval = lastWidth + halfWidth;
                }
                else {
                    CaretAssertVectorIndex(allTextWidths, i - 1);
                    const float previousHalfWidth = allTextWidths[i - 1] / 2.0f;
                    interval = previousHalfWidth + halfWidth;
                }
                
                /* space to separate adjacent text */
                interval += separatorWidth;
                
                if (debugFlag) {
                    std::cout << "Interval " << i << " width: " << interval << std::endl;
                }
                largestInterval = std::max(largestInterval,
                                           interval);
            }
            
            const float numberOfIntervals = numText - 1;
            estimatedWidth = largestInterval * numberOfIntervals;
            if (debugFlag) {
                std::cout << "Estimated Width: " << estimatedWidth
                   << " Viewport Width: " << viewportWidth << std::endl;
            }
        }
    }
    
    return estimatedWidth;
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
 * @return
 *     Estimated width when colorBarTextMode is estimating otherwise zero when drawing.
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
    annText.setFontPercentViewportSize(textPercentageHeight);
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
    
    std::vector<std::pair<float, float>> textWidthInfo;
    
    const float windowZ = (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0;
    const int32_t numText = colorBar->getNumberOfNumericText();
    bool fontTooSmallFlag = false;
    for (int32_t i = 0; i < numText; i++) {
        const AnnotationColorBarNumericText* numericText = colorBar->getNumericText(i);
        const float scalar = numericText->getScalar();
        float scalarOffset = 0.0;
        annText.setHorizontalAlignment(numericText->getHorizontalAlignment());
        
        const float windowX = xTopLeft + (dx * (scalar + scalarOffset));
        const float windowY = yTopLeft + (dy * (scalar + scalarOffset));
        
        annText.setText(numericText->getNumericText());

        BrainOpenGLTextRenderInterface::DrawingFlags flags;
        flags.setDrawSubstitutedText(false);
        m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(windowX,
                                                                                windowY,
                                                                                windowZ,
                                                                                annText,
                                                                                flags);
        
        if (annText.isFontTooSmallWhenLastDrawn()) {
            fontTooSmallFlag = true;
        }
    }

    colorBar->setFontTooSmallWhenLastDrawn(fontTooSmallFlag);
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
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawOval(AnnotationFile* annotationFile,
                                                    AnnotationOval* oval,
                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(oval);
    CaretAssert(oval->getType() == AnnotationTypeEnum::OVAL);
    
    float annXYZ[3];
    
    if ( ! getAnnotationDrawingSpaceCoordinate(oval,
                                         oval->getCoordinate(),
                                         surfaceDisplayed,
                                         annXYZ)) {
        return false;
    }
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(annXYZ[0], annXYZ[1], annXYZ[2]);
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(oval, annXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return false;
    }
    
    const float majorAxis     = ((oval->getWidth()  / 100.0f) * (m_modelSpaceViewport[2] / 2.0f));
    const float minorAxis     = ((oval->getHeight() / 100.0f) * (m_modelSpaceViewport[3] / 2.0f));
    const float rotationAngle = oval->getRotationAngle();
    
    if (oval->getLineWidthPercentage() <= 0.0) {
        convertObsoleteLineWidthPixelsToPercentageWidth(oval);
    }
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0f,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0f,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0f
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(oval,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    uint8_t backgroundRGBA[4];
    oval->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    oval->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    bool drawnFlag = false;
    
    if (drawAnnotationFlag) {
        glPushMatrix();
        glTranslatef(annXYZ[0], annXYZ[1], annXYZ[2]);
        if (rotationAngle != 0.0) {
            glRotatef(-rotationAngle, 0.0f, 0.0f, 1.0f);
        }
        
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                GraphicsShape::drawEllipseFilledByteColor(majorAxis * 2.0f,
                                                          minorAxis * 2.0f,
                                                          selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                const float percentHeight = getLineWidthPercentageInSelectionMode(oval);
                GraphicsShape::drawEllipseOutlineByteColor(majorAxis * 2.0f,
                                                           minorAxis * 2.0f,
                                                           selectionColorRGBA,
                                                           GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                           percentHeight);
            }
            
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    oval,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                GraphicsShape::drawEllipseFilledByteColor(majorAxis * 2.0f,
                                                          minorAxis * 2.0f,
                                                          backgroundRGBA);
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                GraphicsShape::drawEllipseOutlineByteColor(majorAxis * 2.0f,
                                                           minorAxis * 2.0f,
                                                           foregroundRGBA,
                                                           GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                           oval->getLineWidthPercentage());
                drawnFlag = true;
            }
        }
        glPopMatrix();
        
        if (oval->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              oval,
                                                   verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              oval->getRotationAngle());
        }
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
    
    return drawnFlag;
}

/**
 * Draw an annotation oval in surface tangent offset space
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param oval
 *    oval to draw.
 * @param surfaceExtentZ
 *    Z-extent of the surface.
 * @param vertexXYZ
 *    Coordinate of the vertex.
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawOvalSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                        AnnotationOval* oval,
                                                                        const float surfaceExtentZ,
                                                                        const float vertexXYZ[3])
{
    CaretAssert(annotationFile);
    CaretAssert(oval);
    CaretAssert(oval->getType() == AnnotationTypeEnum::OVAL);
    
    const float halfWidth  = ((oval->getWidth() / 100.0) * surfaceExtentZ) / 2.0;
    const float halfHeight = ((oval->getHeight() / 100.0) * surfaceExtentZ) / 2.0;
    float bottomLeft[3]  { -halfWidth, -halfHeight, 0.0f };
    float bottomRight[3] {  halfWidth, -halfHeight, 0.0f };
    float topRight[3]    {  halfWidth,  halfHeight, 0.0f };
    float topLeft[3]     { -halfWidth,  halfHeight, 0.0f };
    
    const float selectionCenterXYZ[3] = {
        vertexXYZ[0],
        vertexXYZ[1],
        vertexXYZ[2]
    };
    
    float lineThickness = ((oval->getLineWidthPercentage() / 100.0)
                           * surfaceExtentZ);
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        lineThickness = std::max(lineThickness,
                                 s_selectionLineMinimumPixelWidth);
    }
    
    uint8_t backgroundRGBA[4];
    oval->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    oval->getLineColorRGBA(foregroundRGBA);
    
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawForegroundFlag);
    
    bool drawnFlag = false;
    
    const float majorAxis     = ((oval->getWidth()  / 100.0f) * surfaceExtentZ);
    const float minorAxis     = ((oval->getHeight() / 100.0f) * surfaceExtentZ);
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(vertexXYZ[0], vertexXYZ[1], vertexXYZ[2]);
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            if (drawBackgroundFlag) {
                /*
                 * When selecting draw only background if it is enabled
                 * since it is opaque and prevents "behind" annotations
                 * from being selected
                 */
                GraphicsShape::drawEllipseFilledByteColor(majorAxis,
                                                          minorAxis,
                                                          selectionColorRGBA);
            }
            else {
                /*
                 * Drawing foreground as line will still allow user to
                 * select annotation that are inside of the box
                 */
                GraphicsShape::drawEllipseOutlineByteColor(majorAxis,
                                                           minorAxis,
                                                           selectionColorRGBA,
                                                           GraphicsPrimitive::LineWidthType::PIXELS,
                                                           lineThickness);
            }
            
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    oval,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (drawBackgroundFlag) {
                GraphicsShape::drawEllipseFilledByteColor(majorAxis,
                                                          minorAxis,
                                                          backgroundRGBA);
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                glPolygonOffset(-1.0, 1.0);
                glEnable(GL_POLYGON_OFFSET_FILL);
                GraphicsShape::drawEllipseOutlineModelSpaceByteColor(majorAxis,
                                                                     minorAxis,
                                                                     foregroundRGBA,
                                                                     lineThickness);
                glDisable(GL_POLYGON_OFFSET_FILL);
                drawnFlag = true;
            }
        }
        if (oval->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              oval,
                                                   verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              oval->getRotationAngle());
        }
    }
    
    return drawnFlag;
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
                                                                                     const float bottomLeft[3],
                                                                                     const float bottomRight[3],
                                                                                     const float topRight[3],
                                                                                     const float topLeft[3],
                                                                                     std::vector<float>& lineCoordinatesOut,
                                                                                     std::vector<float>& arrowCoordinatesOut) const
{
    lineCoordinatesOut.clear();
    arrowCoordinatesOut.clear();
    
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
                if (getAnnotationDrawingSpaceCoordinate(text,
                                                  &noOffsetCoord,
                                                  surfaceDisplayed,
                                                  brainordinateXYZ)) {
                    float annXYZ[3];
                    if (getAnnotationDrawingSpaceCoordinate(text,
                                                      coord,
                                                      surfaceDisplayed,
                                                      annXYZ)) {
                        
                        const bool clipAtBoxFlag = true;
                        if (clipAtBoxFlag) {
                            clipLineAtTextBox(bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              brainordinateXYZ,
                                              annXYZ);
                        }
                        
                        if (text->getLineWidthPercentage() <= 0.0) {
                            convertObsoleteLineWidthPixelsToPercentageWidth(text);
                        }
                        const float lineWidth = getLineWidthFromPercentageHeight(text->getLineWidthPercentage());
                        std::vector<float> unusedArrowCoordinates;
                        createLineCoordinates(annXYZ,
                                              brainordinateXYZ,
                                              lineWidth,
                                              false,
                                              showArrowFlag,
                                              lineCoordinatesOut,
                                              unusedArrowCoordinates,
                                              arrowCoordinatesOut);
                        
                    }
                }
            }
        }
    }
}

/*
 * Clip the line that connects the text to the surface so that the line
 * does not enter a 'box' that encloses the text.
 * 
 * @param bottomLeft
 *     Bottom left corner of annotation.
 * @param bottomRight
 *     Bottom right corner of annotation.
 * @param topRight
 *     Top right corner of annotation.
 * @param topLeft
 *     Top left corner of annotation.
 * param startXYZ
 *    XYZ of where line attaches to surface
 * @param endXYZ
 *    XYZ of where line attaches to text and may be changed to avoid
 *    overlapping the text.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::clipLineAtTextBox(const float bottomLeft[3],
                                                             const float bottomRight[3],
                                                             const float topRight[3],
                                                             const float topLeft[3],
                                                             const float startXYZ[3],
                                                             float endXYZ[3]) const
{
    const float tol = 0.01;

    float clippedXYZ[3] = { 0.0, 0.0, 0.0 };
    float clippedDistance = std::numeric_limits<float>::max();
    bool clippedValid = false;
    
    for (int32_t i = 0; i < 4; i++) {
        float* p1 = NULL;
        float* p2 = NULL;
        switch (i) {
            case 0:
                p1 = const_cast<float*>(bottomLeft);
                p2 = const_cast<float*>(bottomRight);
                break;
            case 1:
                p1 = const_cast<float*>(bottomRight);
                p2 = const_cast<float*>(topRight);
                break;
            case 2:
                p1 = const_cast<float*>(topRight);
                p2 = const_cast<float*>(topLeft);
                break;
            case 3:
                p1 = const_cast<float*>(topLeft);
                p2 = const_cast<float*>(bottomLeft);
                break;
        }
        
        /*
         * perform a 2D intersection test
         */
        float intersection[3] = { 0.0, 0.0, 0.0 };
        const bool yesFlag = MathFunctions::lineIntersection2D(p1, p2,
                                                               startXYZ, endXYZ,
                                                               tol, intersection);
        
        if (yesFlag) {
            /*
             * Intersection is TWO-D so must set 
             * the correct Z-coordinate
             */
            const float averageZ = ((p1[2] + p2[2]) / 2.0f);
            intersection[2] = averageZ;
            const float dist = MathFunctions::distance3D(startXYZ,
                                                         intersection);
            if ( ( ! clippedValid)
                || (dist < clippedDistance)) {
                clippedDistance = dist;
                clippedValid    = true;
                clippedXYZ[0]   = intersection[0];
                clippedXYZ[1]   = intersection[1];
                clippedXYZ[2]   = intersection[2];
            }
        }
    }
    
    if (clippedValid) {
        endXYZ[0] = clippedXYZ[0];
        endXYZ[1] = clippedXYZ[1];
        endXYZ[2] = clippedXYZ[2];
    }
}

/**
 * Get the normal vector for a surface vector.
 *
 * @param surface
 *     The surface.
 * @param vertexIndex
 *     Index of the vertex.
 * @param normalVectorOut
 *     Output containing the normal vector.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::getSurfaceNormalVector(const Surface* surfaceDisplayed,
                                                                  const int32_t vertexIndex,
                                                                  float normalVectorOut[3]) const
{
    const float* normalXYZ = surfaceDisplayed->getNormalVector(vertexIndex);
    normalVectorOut[0] = normalXYZ[0];
    normalVectorOut[1] = normalXYZ[1];
    normalVectorOut[2] = normalXYZ[2];
    
    const bool useAverageFlag = false;
    if ( ! useAverageFlag) {
        return;
    }
    
    CaretPointer<TopologyHelper> th = surfaceDisplayed->getTopologyHelper();
    int32_t numNeighbors(0);
    const int32_t* neighbors = th->getNodeNeighbors(vertexIndex, numNeighbors);
    
    normalVectorOut[0] = 0.0;
    normalVectorOut[1] = 0.0;
    normalVectorOut[2] = 0.0;
    for (int32_t n = 0; n < numNeighbors; n++) {
        const float* normalXYZ = surfaceDisplayed->getNormalVector(neighbors[n]);
        normalVectorOut[0] += normalXYZ[0];
        normalVectorOut[1] += normalXYZ[1];
        normalVectorOut[2] += normalXYZ[2];
    }
    
    if (numNeighbors > 0) {
        normalVectorOut[0] /= numNeighbors;
        normalVectorOut[1] /= numNeighbors;
        normalVectorOut[2] /= numNeighbors;
    }
}

/**
 * @return True If the coordinate/normal vector backfacing (facing away from viewer)?
 *
 * @param xyz
 *     Coordinate.
 * @param normal
 *     Normal vector.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::isBackFacing(const float xyz[3],
                                                        const float normal[3]) const
{
    /*
     * We don't know where the viewer is located in relation to the view of the model.
     * But, we can transform the XYZ coordinate and another XYZ coordinate along the
     * normal vector from model space to window space.  Then, we can compare the transformed
     * Z-coordinates and if the Window Z along the normal vector is greater than Window Z
     * of the coordinate, the normal vector is pointing to the viewer (if NOT then backfacing).
     */
    CaretAssert(m_transformEvent.get());
    CaretAssert(m_transformEvent->isValid());
    float windowXYZ[3];
    const float length(25);
    float offsetXYZ[3] {
        xyz[0] + (normal[0] * length),
        xyz[1] + (normal[1] * length),
        xyz[2] + (normal[2] * length)
    };
    m_transformEvent->transformPoint(xyz, windowXYZ);
    float windowOffsetXYZ[3];
    m_transformEvent->transformPoint(offsetXYZ, windowOffsetXYZ);
    const float diff = windowOffsetXYZ[2] - windowXYZ[2];
    return (diff < 0.0f);
}

/**
 * Draw an annotation text with surface tangent offset
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param text
 *    Annotation text to draw.
 * @param surfaceExtentZ
 *    Z-extent of the surface.
 * @param vertexXYZ,
 *    Coordinate of the vertex.
 * @param vertexNormalXYZ
 *    Normal vector of vertex.
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawTextSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                        AnnotationText* text,
                                                                        const float surfaceExtentZ,
                                                                        const float vertexXYZ[3],
                                                                        const float vertexNormalXYZ[3])
{
    CaretAssert(annotationFile);
    CaretAssert(text);
    
    /*
     * Annotations with "DISPLAY_GROUP" propery may be turned on/off by user.
     */
    DisplayPropertiesAnnotation* dpa = m_inputs->m_brain->getDisplayPropertiesAnnotation();
    if (text->testProperty(Annotation::Property::DISPLAY_GROUP)) {
        if ( ! dpa->isDisplayTextAnnotations()) {
            return false;
        }
    }
    
    if (isBackFacing(vertexXYZ,
                     vertexNormalXYZ)) {
        return false;
    }
    
    double bottomLeft[3];
    double bottomRight[3];
    double topRight[3];
    double topLeft[3];
    double underlineStart[3];
    double underlineEnd[3];
    m_brainOpenGLFixedPipeline->getTextRenderer()->getBoundsForTextInModelSpace(*text, m_surfaceViewScaling, surfaceExtentZ, m_textDrawingFlags,
                                                                                bottomLeft, bottomRight, topRight, topLeft,
                                                                                underlineStart, underlineEnd);
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(vertexXYZ[0], vertexXYZ[1], vertexXYZ[2]);

    bool textDrawnFlag = false;
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        uint8_t selectionColorRGBA[4] = { 0, 0, 0, 0 };
        getIdentificationColor(selectionColorRGBA);
        GraphicsPrimitiveV3fN3f primitive(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                          selectionColorRGBA);
        const double doubleNormalXYZ[3] {
            vertexNormalXYZ[0],
            vertexNormalXYZ[1],
            vertexNormalXYZ[2]
        };
        primitive.addVertex(topLeft, doubleNormalXYZ);
        primitive.addVertex(bottomLeft, doubleNormalXYZ);
        primitive.addVertex(topRight, doubleNormalXYZ);
        primitive.addVertex(bottomRight, doubleNormalXYZ);
        GraphicsEngineDataOpenGL::draw(&primitive);
        
        const int32_t invalidPolyLineCoordinateIndex(-1);
        m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                text,
                                                AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                invalidPolyLineCoordinateIndex,
                                                vertexXYZ,
                                                verticesInWindowXYZ));
    }
    else {
        glPushMatrix();
        m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextInModelSpace(*text,
                                                                            m_surfaceViewScaling,
                                                                            surfaceExtentZ,
                                                                            vertexNormalXYZ,
                                                                            m_textDrawingFlags);
        glPopMatrix();
        
        textDrawnFlag = true;
    }
    
    if (text->isSelectedForEditing(m_inputs->m_windowIndex)) {
        glPushAttrib(GL_POLYGON_BIT
                     | GL_LIGHTING_BIT);
        
        /*
         * So that text and background do not mix together
         * in the Z-buffer
         */
        glEnable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glEnable(GL_POLYGON_OFFSET_POINT);
        glPolygonOffset(-1.0, 1.0);
        glDisable(GL_LIGHTING);
        
        float floatBottomLeft[3];
        float floatBottomRight[3];
        float floatTopRight[3];
        float floatTopLeft[3];
        for (int32_t i = 0; i < 3; i++) {
            floatBottomLeft[i] = bottomLeft[i];
            floatBottomRight[i] = bottomRight[i];
            floatTopLeft[i] = topLeft[i];
            floatTopRight[i] = topRight[i];
        }
        drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                          text,
                                               verticesInWindowXYZ,
                                          floatBottomLeft,
                                          floatBottomRight,
                                          floatTopRight,
                                          floatTopLeft,
                                          s_sizingHandleLineWidthInPixels,
                                          text->getRotationAngle());
        glPopAttrib();
    }
    
    return textDrawnFlag;
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
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawText(AnnotationFile* annotationFile,
                                                    AnnotationText* text,
                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(text);
    CaretAssert(text->getType() == AnnotationTypeEnum::TEXT);
    
    /*
     * Annotations with "DISPLAY_GROUP" propery may be turned on/off by user.
     */
    DisplayPropertiesAnnotation* dpa = m_inputs->m_brain->getDisplayPropertiesAnnotation();
    if (text->testProperty(Annotation::Property::DISPLAY_GROUP)) {
        if ( ! dpa->isDisplayTextAnnotations()) {
            return false;
        }
    }

    float annXYZ[3];
    if ( ! getAnnotationDrawingSpaceCoordinate(text,
                                         text->getCoordinate(),
                                         surfaceDisplayed,
                                         annXYZ)) {
        return false;
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
    
    m_brainOpenGLFixedPipeline->getTextRenderer()->getBoundsForTextAtViewportCoords(*text,
                                                                                    m_textDrawingFlags,
                                                                                    annXYZ[0], annXYZ[1], annXYZ[2],
                                                                                    m_modelSpaceViewport[2], m_modelSpaceViewport[3],
                                                                                    bottomLeft, bottomRight, topRight, topLeft);

    BoundingBox bb;
    bb.set(bottomLeft, bottomRight, topRight, topLeft);
    text->setDrawnInWindowBounds(m_inputs->m_windowIndex, bb);
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0f,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0f,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0f
    };

    const bool depthTestFlag = isDrawnWithDepthTesting(text,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    float backgroundRGBA[4];
    text->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    text->getLineColorRGBA(foregroundRGBA);
    uint8_t textColorRGBA[4];
    text->getTextColorRGBA(textColorRGBA);
    
    const bool drawTextFlag       = (textColorRGBA[3] > 0);
    const bool drawBackgroundFlag = (backgroundRGBA[3] > 0.0f);
    const bool drawAnnotationFlag = (drawBackgroundFlag || drawTextFlag);
    
    bool drawnFlag = false;
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(annXYZ[0], annXYZ[1], annXYZ[2]);

    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            GraphicsShape::drawBoxFilledByteColor(bottomLeft, bottomRight, topRight, topLeft,
                                                  selectionColorRGBA);
            
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    text,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            std::vector<float> connectLineCoordinates;
            std::vector<float> arrowCoordinates;

            getTextLineToBrainordinateLineCoordinates(text,
                                                      surfaceDisplayed,
                                                      bottomLeft,
                                                      bottomRight,
                                                      topRight,
                                                      topLeft,
                                                      connectLineCoordinates,
                                                      arrowCoordinates);
            
            if ( ! connectLineCoordinates.empty()) {
                if (text->getLineWidthPercentage() <= 0.0) {
                    convertObsoleteLineWidthPixelsToPercentageWidth(text);
                }
                
                GraphicsShape::drawLinesByteColor(connectLineCoordinates,
                                                  textColorRGBA,
                                                  GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                  text->getLineWidthPercentage());
                if ( ! arrowCoordinates.empty()) {
                    GraphicsShape::drawLineStripMiterJoinByteColor(arrowCoordinates,
                                                      textColorRGBA,
                                                      GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                      text->getLineWidthPercentage());
                }
            }
            
            if (drawTextFlag) {
                if (debugFlag) {
                    if (text->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
                        AString msg("Drawing Text: " + text->getText() + "  DepthTest=" + AString::fromBool(depthTestFlag));
                        const float* xyz = text->getCoordinate()->getXYZ();
                        msg.appendWithNewLine("    Stereotaxic: " + AString::fromNumbers(xyz, 3, ","));
                        msg.appendWithNewLine("    Drawing Space: " + AString::fromNumbers(annXYZ, 3, ","));
                        std::cout << qPrintable(msg) << std::endl;
                    }
                }
                
                if (depthTestFlag) {
                    m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(annXYZ[0],
                                                                                            annXYZ[1],
                                                                                            annXYZ[2],
                                                                                            *text,
                                                                                            m_textDrawingFlags);
                    drawnFlag = true;
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
                        getAnnotationTwoDimShapeBounds(text, annXYZ, bl, br, tr, tl);
                        
                        GraphicsShape::drawBoxOutlineByteColor(bl, br, tr, tl,
                                                               foregroundRGBA,
                                                               GraphicsPrimitive::LineWidthType::PIXELS,
                                                               2.0f);
                    }
                    else {
                        m_brainOpenGLFixedPipeline->getTextRenderer()->drawTextAtViewportCoords(annXYZ[0],
                                                                                           annXYZ[1],
                                                                                           *text,
                                                                                                m_textDrawingFlags);
                        drawnFlag = true;
                    }
                }
                
                setDepthTestingStatus(depthTestFlag);
            }
        }
        
        if (text->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              text,
                                                   verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              text->getRotationAngle());
            
            /*
             * Cross can be used to show the text annotation's coordinate
             * to assist with placement of the annotation
             */
            const bool drawCrossFlag = false;
            if (drawCrossFlag) {
                const float redRGBA[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
                std::unique_ptr<GraphicsPrimitiveV3f> crossShape = std::unique_ptr<GraphicsPrimitiveV3f>(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_LINES,
                                                                                                                                            redRGBA));
                crossShape->addVertex(annXYZ[0],
                                      annXYZ[1] - 10);
                crossShape->addVertex(annXYZ[0],
                                      annXYZ[1] + 10);
                crossShape->addVertex(annXYZ[0] - 10,
                                      annXYZ[1]);
                crossShape->addVertex(annXYZ[0] + 10,
                                      annXYZ[1]);
                crossShape->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
                GraphicsEngineDataOpenGL::draw(crossShape.get());
            }
        }
    }

    if (percentSizeText != NULL) {
        if (savedFontPercentViewportHeight > 0.0f) {
            percentSizeText->setFontPercentViewportSize(savedFontPercentViewportHeight);
            if ( ! modifiedStatus) {
                percentSizeText->clearModified();
            }
        }
    }

    setDepthTestingStatus(savedDepthTestStatus);
    
    return drawnFlag;
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
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
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
        /* OK */
    }
    else {
        /*
         * This will occur when the user is dragging the mouse to ceate
         * an image annotation.  Continue processing so that a box
         * outline is drawn.
         */
        CaretLogFine("Image annotation is invalid for drawing.  This will "
                     "occur when user is dragging mouse to create image annotation.  ");
    }

    float annXYZ[3];
    
    if ( ! getAnnotationDrawingSpaceCoordinate(image,
                                         image->getCoordinate(),
                                         surfaceDisplayed,
                                         annXYZ)) {
        return false;
    }
    
    float bottomLeft[3];
    float bottomRight[3];
    float topRight[3];
    float topLeft[3];
    if ( ! getAnnotationTwoDimShapeBounds(image, annXYZ,
                                          bottomLeft, bottomRight, topRight, topLeft)) {
        return false;
    }
    
    const float selectionCenterXYZ[3] = {
        (bottomLeft[0] + bottomRight[0] + topRight[0] + topLeft[0]) / 4.0f,
        (bottomLeft[1] + bottomRight[1] + topRight[1] + topLeft[1]) / 4.0f,
        (bottomLeft[2] + bottomRight[2] + topRight[2] + topLeft[2]) / 4.0f
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(image,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    const bool drawAnnotationFlag = true;
    
    float foregroundRGBA[4];
    image->getLineColorRGBA(foregroundRGBA);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0);

    bool drawnFlag = false;
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(annXYZ[0], annXYZ[1], annXYZ[2]);
    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            GraphicsShape::drawBoxFilledByteColor(bottomLeft, bottomRight, topRight, topLeft,
                                                  selectionColorRGBA);
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    image,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            if (debugFlag) {
                if (image->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::STEREOTAXIC) {
                    AString msg("Drawing Image: DepthTest=" + AString::fromBool(depthTestFlag));
                    const float* xyz = image->getCoordinate()->getXYZ();
                    msg.appendWithNewLine("    Stereotaxic: " + AString::fromNumbers(xyz, 3, ","));
                    msg.appendWithNewLine("    Drawing Space: " + AString::fromNumbers(annXYZ, 3, ","));
                    std::cout << qPrintable(msg) << std::endl;
                }
            }
            
            image->setVertexBounds(bottomLeft,
                                   bottomRight,
                                   topRight,
                                   topLeft);
            GraphicsPrimitiveV3fT2f* primitive = image->getGraphicsPrimitive();
            if (primitive != NULL) {
                if (primitive->isValid()) {
                    GraphicsEngineDataOpenGL::draw(primitive);
                }
                drawnFlag = true;
            }

            if (drawForegroundFlag) {
                if (image->getLineWidthPercentage() <= 0.0f) {
                    convertObsoleteLineWidthPixelsToPercentageWidth(image);
                }
                GraphicsShape::drawBoxOutlineFloatColor(bottomLeft, bottomRight, topRight, topLeft,
                                                        foregroundRGBA,
                                                        GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT, image->getLineWidthPercentage());
            }
            
            setDepthTestingStatus(depthTestFlag);
        }
        
        if (image->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              image,
                                                   verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              image->getRotationAngle());
        }
    }

    setDepthTestingStatus(savedDepthTestStatus);
    
    return drawnFlag;
}

/**
 * Draw an annotation image in surface tangent offset space
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param image
 *    Image to draw.
 * @param surfaceExtentZ
 *    Z-extent of the surface.
 * @param vertexXYZ
 *    Coordinate of the vertex.
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawImageSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                       AnnotationImage* image,
                                                                       const float surfaceExtentZ,
                                                                       const float vertexXYZ[3])
{
    CaretAssert(annotationFile);
    CaretAssert(image);
    CaretAssert(image->getType() == AnnotationTypeEnum::IMAGE);
    
    const float halfWidth  = ((image->getWidth() / 100.0) * surfaceExtentZ) / 2.0;
    const float halfHeight = ((image->getHeight() / 100.0) * surfaceExtentZ) / 2.0;
    float bottomLeft[3]  { -halfWidth, -halfHeight, 0.0f };
    float bottomRight[3] {  halfWidth, -halfHeight, 0.0f };
    float topRight[3]    {  halfWidth,  halfHeight, 0.0f };
    float topLeft[3]     { -halfWidth,  halfHeight, 0.0f };
    
    const float selectionCenterXYZ[3] = {
        vertexXYZ[0],
        vertexXYZ[1],
        vertexXYZ[2]
    };
    
    float lineThickness = ((image->getLineWidthPercentage() / 100.0)
                           * surfaceExtentZ);
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        lineThickness = std::max(lineThickness,
                                 s_selectionLineMinimumPixelWidth);
    }
    
    uint8_t backgroundRGBA[4];
    image->getBackgroundColorRGBA(backgroundRGBA);
    uint8_t foregroundRGBA[4];
    image->getLineColorRGBA(foregroundRGBA);
    
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0);
    const bool drawAnnotationFlag = true;
    
    bool drawnFlag = false;
    
    std::vector<Vector3D> verticesInWindowXYZ;
    verticesInWindowXYZ.emplace_back(vertexXYZ[0], vertexXYZ[1], vertexXYZ[2]);    
    if (drawAnnotationFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            /*
             * When selecting draw only background if it is enabled
             * since it is opaque and prevents "behind" annotations
             * from being selected
             */
            GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                                  bottomRight,
                                                  topRight,
                                                  topLeft,
                                                  selectionColorRGBA);
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    image,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    verticesInWindowXYZ));
        }
        else {
            image->setVertexBounds(bottomLeft,
                                   bottomRight,
                                   topRight,
                                   topLeft);
            GraphicsPrimitiveV3fT2f* primitive = image->getGraphicsPrimitive();
            if (primitive != NULL) {
                if (primitive->isValid()) {
                    GraphicsEngineDataOpenGL::draw(primitive);
                }
                drawnFlag = true;
            }
            
            if (drawForegroundFlag) {
                glPolygonOffset(-1.0, -1.0);
                glEnable(GL_POLYGON_OFFSET_FILL);
                GraphicsShape::drawOutlineRectangleVerticesInMiddle(bottomLeft,
                                                                    bottomRight,
                                                                    topRight,
                                                                    topLeft,
                                                                    lineThickness,
                                                                    foregroundRGBA);
                drawnFlag = true;
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
        }
        if (image->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationTwoDimShapeSizingHandles(annotationFile,
                                              image,
                                                   verticesInWindowXYZ,
                                              bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              s_sizingHandleLineWidthInPixels,
                                              image->getRotationAngle());
        }
    }
    
    return drawnFlag;
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
 * @param lineCoordinatesOut
 *     Output containing coordinates for drawing the line
 * @param startArrowCoordinatesOut
 *     Output containing coordinates for drawing the line's start arrow
 * @param endArrowCoordinatesOut
 *     Output containing coordinates for drawing the line's end arrow
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::createLineCoordinates(const float lineHeadXYZ[3],
                                                                 const float lineTailXYZ[3],
                                                                 const float lineThicknessPixels,
                                                                 const bool validStartArrow,
                                                                 const bool validEndArrow,
                                                                 std::vector<float>& lineCoordinatesOut,
                                                                 std::vector<float>& startArrowCoordinatesOut,
                                                                 std::vector<float>& endArrowCoordinatesOut) const
{
    lineCoordinatesOut.clear();
    startArrowCoordinatesOut.clear();
    endArrowCoordinatesOut.clear();
    
    /*
     * Length of arrow's tips is function of line thickness
     */
    const float lineLength = MathFunctions::distance3D(lineHeadXYZ,
                                                       lineTailXYZ);
    const float thirdLineLength = lineLength / 3.0f;
    
    /*
     * Vector from Tail to Head of line
     */
    float tailToHeadUnitVector[3];
    MathFunctions::createUnitVector(lineTailXYZ,
                                    lineHeadXYZ,
                                    tailToHeadUnitVector);
    
    
    /*
     * Create a perpendicular vector by swapping first two elements
     * and negating the second element.
     */
    const float leftToRightUnitVector[3] = {
        tailToHeadUnitVector[1],
        -tailToHeadUnitVector[0],
        tailToHeadUnitVector[2]
    };
    
    /*
     * Left to right vector moves the arrows tips away from the line
     * in a direction perpendicular to the line
     */
    const float awayFromLineScale = 3.0f;
    const float awayFromLineDistance = lineThicknessPixels * awayFromLineScale;
    const float leftToRightVector[3] = {
        leftToRightUnitVector[0] * awayFromLineDistance,
        leftToRightUnitVector[1] * awayFromLineDistance,
        leftToRightUnitVector[2]
    };
    
    /*
     * Away from tail vector moves the arrows tips away from the tail
     * of the line in a direction along the line
     */
    const float awayFromTipScale = 3.0f;
    const float alongLineDistanceFromTip = std::min((lineThicknessPixels * awayFromTipScale),
                                                 (thirdLineLength));
    
    const float awayFromTailVector[3] = {
        (tailToHeadUnitVector[0] * alongLineDistanceFromTip),
        (tailToHeadUnitVector[1] * alongLineDistanceFromTip),
        (tailToHeadUnitVector[2])
    };
    
    if (validEndArrow) {
        const float tailTipRightXYZ[3] = {
            lineTailXYZ[0] + leftToRightVector[0] + awayFromTailVector[0],
            lineTailXYZ[1] + leftToRightVector[1] + awayFromTailVector[1],
            lineTailXYZ[2] + leftToRightVector[2] + awayFromTailVector[2]
        };
        const float tailTipLeftXYZ[3] = {
            lineTailXYZ[0] - leftToRightVector[0] + awayFromTailVector[0],
            lineTailXYZ[1] - leftToRightVector[1] + awayFromTailVector[1],
            lineTailXYZ[2] + leftToRightVector[2] + awayFromTailVector[2]
        };
        
        endArrowCoordinatesOut.insert(endArrowCoordinatesOut.end(),
                                        tailTipLeftXYZ, tailTipLeftXYZ + 3);
        endArrowCoordinatesOut.insert(endArrowCoordinatesOut.end(),
                                        lineTailXYZ, lineTailXYZ + 3);
        endArrowCoordinatesOut.insert(endArrowCoordinatesOut.end(),
                                        tailTipRightXYZ, tailTipRightXYZ + 3);
    }
    
    if (validStartArrow) {
        const float headTipRightXYZ[3] = {
            lineHeadXYZ[0] + leftToRightVector[0] - awayFromTailVector[0],
            lineHeadXYZ[1] + leftToRightVector[1] - awayFromTailVector[1],
            lineHeadXYZ[2] + leftToRightVector[2] - awayFromTailVector[2]
        };
        const float headTipLeftXYZ[3] = {
            lineHeadXYZ[0] - leftToRightVector[0] - awayFromTailVector[0],
            lineHeadXYZ[1] - leftToRightVector[1] - awayFromTailVector[1],
            lineHeadXYZ[2] + leftToRightVector[2] - awayFromTailVector[2]
        };
        
        startArrowCoordinatesOut.insert(startArrowCoordinatesOut.end(),
                                        headTipLeftXYZ, headTipLeftXYZ + 3);
        startArrowCoordinatesOut.insert(startArrowCoordinatesOut.end(),
                                        lineHeadXYZ, lineHeadXYZ + 3);
        startArrowCoordinatesOut.insert(startArrowCoordinatesOut.end(),
                                        headTipRightXYZ, headTipRightXYZ + 3);
    }
    
    lineCoordinatesOut.insert(lineCoordinatesOut.end(), lineHeadXYZ, lineHeadXYZ + 3);
    lineCoordinatesOut.insert(lineCoordinatesOut.end(), lineTailXYZ, lineTailXYZ + 3);
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
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawLine(AnnotationFile* annotationFile,
                                                    AnnotationLine* line,
                                                    const Surface* surfaceDisplayed)
{
    CaretAssert(line);
    CaretAssert(line->getType() == AnnotationTypeEnum::LINE);
    
    Vector3D lineHeadXYZ;
    Vector3D lineTailXYZ;
    
    if ( ! getAnnotationDrawingSpaceCoordinate(line,
                                         line->getStartCoordinate(),
                                         surfaceDisplayed,
                                         lineHeadXYZ)) {
        return false;
    }
    if ( ! getAnnotationDrawingSpaceCoordinate(line,
                                         line->getEndCoordinate(),
                                         surfaceDisplayed,
                                         lineTailXYZ)) {
        return false;
    }
    
    if (line->getLineWidthPercentage() <= 0.0) {
        convertObsoleteLineWidthPixelsToPercentageWidth(line);
    }
    const float lineWidth = getLineWidthFromPercentageHeight(line->getLineWidthPercentage());
    
    const float selectionCenterXYZ[3] = {
        (lineHeadXYZ[0] + lineTailXYZ[0]) / 2.0f,
        (lineHeadXYZ[1] + lineTailXYZ[1]) / 2.0f,
        (lineHeadXYZ[2] + lineTailXYZ[2]) / 2.0f
    };
    
    const bool depthTestFlag = isDrawnWithDepthTesting(line,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    std::vector<float> lineCoordinates;
    std::vector<float> startArrowCoordinates;
    std::vector<float>  endArrowCoordinates;
    createLineCoordinates(lineHeadXYZ,
                          lineTailXYZ,
                          lineWidth,
                          line->isDisplayStartArrow(),
                          line->isDisplayEndArrow(),
                          lineCoordinates,
                          startArrowCoordinates,
                          endArrowCoordinates);
    
    uint8_t foregroundRGBA[4];
    line->getLineColorRGBA(foregroundRGBA);
    
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0f);
    
    bool drawnFlag = false;
    
    std::vector<Vector3D> windowVertexXYZ;
    windowVertexXYZ.push_back(lineHeadXYZ);
    windowVertexXYZ.push_back(lineTailXYZ);
    windowVertexXYZ[0][0] += m_modelSpaceViewport[0];
    windowVertexXYZ[0][1] += m_modelSpaceViewport[1];
    windowVertexXYZ[1][0] += m_modelSpaceViewport[0];
    windowVertexXYZ[1][1] += m_modelSpaceViewport[1];
    
    if (drawForegroundFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            const float percentHeight = getLineWidthPercentageInSelectionMode(line);
            GraphicsShape::drawLinesByteColor(lineCoordinates,
                                              selectionColorRGBA,
                                              GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                              percentHeight);
            if ( ! startArrowCoordinates.empty()) {
                GraphicsShape::drawLineStripMiterJoinByteColor(startArrowCoordinates,
                                                      selectionColorRGBA,
                                                      GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                      line->getLineWidthPercentage());
            }
            if ( ! endArrowCoordinates.empty()) {
                GraphicsShape::drawLineStripMiterJoinByteColor(endArrowCoordinates,
                                                      selectionColorRGBA,
                                                      GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                      line->getLineWidthPercentage());
            }
            
            const int32_t invalidPolyLineCoordinateIndex(-1);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    line,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    windowVertexXYZ));
        }
        else {
            if (drawForegroundFlag) {
                GraphicsShape::drawLinesByteColor(lineCoordinates,
                                                  foregroundRGBA,
                                                  GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                  line->getLineWidthPercentage());
                if ( ! startArrowCoordinates.empty()) {
                    GraphicsShape::drawLineStripMiterJoinByteColor(startArrowCoordinates,
                                                          foregroundRGBA,
                                                          GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                          line->getLineWidthPercentage());
                }
                if ( ! endArrowCoordinates.empty()) {
                    GraphicsShape::drawLineStripMiterJoinByteColor(endArrowCoordinates,
                                                          foregroundRGBA,
                                                          GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                                          line->getLineWidthPercentage());
                }
                drawnFlag = true;
            }
        }
        
        if (line->isSelectedForEditing(m_inputs->m_windowIndex)) {
            drawAnnotationLineSizingHandles(annotationFile,
                                              line,
                                            windowVertexXYZ,
                                              lineHeadXYZ,
                                              lineTailXYZ,
                                              s_sizingHandleLineWidthInPixels);
        }
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
    
    BoundingBox bb;
    if (drawnFlag) {
        /* Note head and tail intentionally used twice since line only has two points, unlike box or other shapes */
        bb.set(lineHeadXYZ, lineTailXYZ, lineHeadXYZ, lineTailXYZ);
    }
    line->setDrawnInWindowBounds(m_inputs->m_windowIndex, bb);
    
    return drawnFlag;
}

/**
 * Draw an annotation line that is in surface space with tangent offset.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param line
 *    Annotation line to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawLineSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                        AnnotationLine* line,
                                                                        const Surface* surfaceDisplayed,
                                                                        const float surfaceExtentZ)
{
    CaretAssert(line);
    CaretAssert(line->getType() == AnnotationTypeEnum::LINE);
    
    StructureEnum::Enum structureOne(StructureEnum::INVALID);
    StructureEnum::Enum structureTwo(StructureEnum::INVALID);
    int32_t numberOfVerticesOne(0);
    int32_t numberOfVerticesTwo(0);
    int32_t vertexIndexOne(-1);
    int32_t vertexIndexTwo(-1);
    float offsetLengthOne(0.0f);
    float offsetLengthTwo(0.0f);
    AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorOne(AnnotationSurfaceOffsetVectorTypeEnum::TANGENT);
    AnnotationSurfaceOffsetVectorTypeEnum::Enum surfaceOffsetVectorTwo(AnnotationSurfaceOffsetVectorTypeEnum::TANGENT);
    line->getStartCoordinate()->getSurfaceSpace(structureOne, numberOfVerticesOne, vertexIndexOne,
                                                offsetLengthOne, surfaceOffsetVectorOne);
    line->getEndCoordinate()->getSurfaceSpace(structureTwo, numberOfVerticesTwo, vertexIndexTwo,
                                                offsetLengthTwo, surfaceOffsetVectorTwo);

    if ((surfaceDisplayed->getStructure() != structureOne)
        || (surfaceDisplayed->getStructure() != structureTwo)
        || (surfaceDisplayed->getNumberOfNodes() != numberOfVerticesOne)
        || (surfaceDisplayed->getNumberOfNodes() != numberOfVerticesTwo)
        || (vertexIndexOne < 0)
        || (vertexIndexTwo < 0)) {
        return false;
    }
    float lineHeadXYZ[3];
    float lineTailXYZ[3];
    surfaceDisplayed->getCoordinate(vertexIndexOne, lineHeadXYZ);
    surfaceDisplayed->getCoordinate(vertexIndexTwo, lineTailXYZ);
    
    float offsetVectorOne[3];
    float offsetVectorTwo[3];
    surfaceDisplayed->getNormalVector(vertexIndexOne,
                                      offsetVectorOne);
    surfaceDisplayed->getNormalVector(vertexIndexTwo,
                                      offsetVectorTwo);
    for (int32_t i = 0; i < 3; i++) {
        lineHeadXYZ[i] += (offsetVectorOne[i] * offsetLengthOne);
        lineTailXYZ[i] += (offsetVectorTwo[i] * offsetLengthTwo);
    }
    
    const float selectionCenterXYZ[3] = {
        (lineHeadXYZ[0] + lineTailXYZ[0]) / 2.0f,
        (lineHeadXYZ[1] + lineTailXYZ[1]) / 2.0f,
        (lineHeadXYZ[2] + lineTailXYZ[2]) / 2.0f
    };
    
    if (line->getLineWidthPercentage() <= 0.0) {
        convertObsoleteLineWidthPixelsToPercentageWidth(line);
    }
    float lineThickness = ((line->getLineWidthPercentage() / 100.0)
                           * surfaceExtentZ);
    lineThickness *= m_surfaceViewScaling;
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        lineThickness = std::max(lineThickness,
                                 s_selectionLineMinimumPixelWidth);
    }
    lineThickness = GraphicsUtilitiesOpenGL::convertMillimetersToPixels(lineThickness);

    int32_t invalidPolyLineCoordinateIndex(-1);
    bool drawnFlag = false;
    
    std::vector<float> lineCoordinates;
    std::vector<float> startArrowCoordinates;
    std::vector<float> endArrowCoordinates;
    
    createLineCoordinates(lineHeadXYZ,
                          lineTailXYZ,
                          lineThickness,
                          line->isDisplayStartArrow(),
                          line->isDisplayEndArrow(),
                          lineCoordinates,
                          startArrowCoordinates,
                          endArrowCoordinates);
    uint8_t foregroundRGBA[4];
    line->getLineColorRGBA(foregroundRGBA);
    
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0f);
    
    std::vector<Vector3D> windowVertexXYZ;
    for (int32_t i = 0; i < 2; i++) {
        Vector3D xyz;
        if ( ! getAnnotationDrawingSpaceCoordinate(line,
                                                   line->getCoordinate(i),
                                                   surfaceDisplayed,
                                                   xyz)) {
            return false;
        }
        windowVertexXYZ.push_back(xyz);
    }
    
    if (drawForegroundFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            
            
            GraphicsShape::drawLinesByteColor(lineCoordinates,
                                              selectionColorRGBA,
                                              GraphicsPrimitive::LineWidthType::PIXELS,
                                              lineThickness);
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    line,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    invalidPolyLineCoordinateIndex,
                                                    selectionCenterXYZ,
                                                    windowVertexXYZ));
        }
        else {
            if (drawForegroundFlag) {
                GraphicsShape::drawLinesByteColor(lineCoordinates,
                                                  foregroundRGBA,
                                                  GraphicsPrimitive::LineWidthType::PIXELS,
                                                  lineThickness);
                drawnFlag = true;
            }
        }
        
        if (line->isSelectedForEditing(m_inputs->m_windowIndex)) {
            const float minPixelSize = 30.0;
            const float minSizeMM = (GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(minPixelSize)
                                     * m_surfaceViewScaling);
            const float handleThickness = std::max(minSizeMM,
                                                   lineThickness * 2.0f);
            drawAnnotationLineSizingHandles(annotationFile,
                                              line,
                                            windowVertexXYZ,
                                              lineHeadXYZ,
                                              lineTailXYZ,
                                              handleThickness);
        }
    }
    
    return drawnFlag;
}

/**
 * Draw an annotation paired multi-coordinate
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param multiPairedCoordShape
 *    Annotation to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawMultiPairedCoordinateShape(AnnotationFile* annotationFile,
                                                                          AnnotationMultiPairedCoordinateShape* multiPairedCoordShape,
                                                                          const Surface* surfaceDisplayed)
{
    AnnotationPolyhedron* polyhedron(NULL);
    CaretAssert(multiPairedCoordShape);
    switch (multiPairedCoordShape->getType()) {
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::BROWSER_TAB:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::LINE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::SCALE_BAR:
        case AnnotationTypeEnum::TEXT:
        case AnnotationTypeEnum::POLYGON:
        case AnnotationTypeEnum::POLYLINE:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            CaretAssert(multiPairedCoordShape->castToPolyhedron());
            polyhedron = multiPairedCoordShape->castToPolyhedron();
            break;
    }
    
    if (polyhedron == NULL) {
        CaretAssertMessage(0, "Has new multi-paired-coord annotation type been added?");
        return false;
    }
    
    if ( ! multiPairedCoordShape->isDrawingNewAnnotation()) {
        if ( ! MathFunctions::isEvenNumber(multiPairedCoordShape->getNumberOfCoordinates())) {
            if (polyhedron != NULL) {
                CaretLogSevere("Polyhedron shape must have an even number of coordinates");
            }
            else {
                CaretLogSevere("Multi paired coordinate shape must have an even number of coordinates");
            }
            return false;
        }
    }

    uint8_t foregroundRGBA[4];
    multiPairedCoordShape->getLineColorRGBA(foregroundRGBA);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0f);
    
    float absAngle(-10000.0);
    if (m_volumeSpacePlane.isValidPlane()) {
        const Plane annPlane(polyhedron->getPlane());
        if (annPlane.isValidPlane()) {
            absAngle = std::fabs(Plane::angleDegreesOfPlaneNormalVectors(m_volumeSpacePlane,
                                                                         annPlane));
        }
        else {
            CaretLogSevere("Invalid plane, cannot drawn: "
                           + multiPairedCoordShape->toString());
            return false;
        }
    }
    else {
        /*
         * Note: Plane is invalid when a new annotation is being drawn
         */
        if (multiPairedCoordShape->isDrawingNewAnnotation()) {
            /*
             * Special case for annotation being drawn
             */
            absAngle = 0.0;
        }
        else {
            return false;
        }
    }

    if ((absAngle > 5.0)
        && (absAngle < 175.0)) {
        if ( ! polyhedron->isDrawingNewAnnotation()) {
            drawPolyhedronEdgesOnPlane(annotationFile,
                                       polyhedron,
                                       m_volumeSpacePlane,
                                       foregroundRGBA);
        }
        return false;
    }

    std::vector<Vector3D> windowVertexXYZ;
    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive;
    primitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN,
                                                           foregroundRGBA));
    CaretAssert(primitive);
    
    /*
     * Note: A multi-paired coordinate annotations contains 'two sets' of coordinates
     * that are stored as a single set of coordinates.  The total number of coordinates
     * is always even.  The first half of the coordinates draw the one face of a
     * polyhedron and the second half of the coordinates draw the the other face of
     * the polyhedron.  In addition, lines are drawn from the first set of coordinates
     * to the second set.
     */
    bool drawEditableSizingHandlesFlag(false);
    bool drawNonEditableSizingHandlesFlag(false);
    bool drawCreatingNewAnnotationSizingHandlesFlag(false);
    int32_t drawStartingCoordinateIndex(-1);
    int32_t drawCoordinateCount(-1);
    
    if (multiPairedCoordShape->isDrawingNewAnnotation()
        && (multiPairedCoordShape->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::WINDOW)) {
        const int32_t numCoords(multiPairedCoordShape->getNumberOfCoordinates());
        for (int32_t i = 0; i < numCoords; i++) {
            Vector3D xyz;
            if (getAnnotationDrawingSpaceCoordinate(multiPairedCoordShape,
                                                    multiPairedCoordShape->getCoordinate(i),
                                                    surfaceDisplayed,
                                                    xyz)) {
                windowVertexXYZ.push_back(xyz);
                primitive->addVertex(xyz);
            }
        }
        drawCreatingNewAnnotationSizingHandlesFlag = true;
    }
    else {
        if (multiPairedCoordShape->isDrawingNewAnnotation()) {
            drawCreatingNewAnnotationSizingHandlesFlag = true;
        }
        for (int32_t iDraw = 0; iDraw < 3; iDraw++) {
            const int32_t numTotalCoords(multiPairedCoordShape->getNumberOfCoordinates());
            const int32_t numHalfCoords(numTotalCoords / 2);
            
            std::vector<Vector3D> xyzToDraw;
            switch (iDraw) {
                case 0:
                {
                    /*
                     * 'Near' polygon in first half of coordinates
                     */
                    for (int32_t i = 0; i < numHalfCoords; i++) {
                        Vector3D xyz;
                        if (getAnnotationDrawingSpaceCoordinate(multiPairedCoordShape,
                                                                multiPairedCoordShape->getCoordinate(i),
                                                                surfaceDisplayed,
                                                                xyz)) {
                            xyzToDraw.push_back(xyz);
                        }
                    }
                }
                    break;
                case 1:
                {
                    /*
                     * 'Far' polygon in second half of coordinates
                     */
                    for (int32_t i = numHalfCoords; i < numTotalCoords; i++) {
                        Vector3D xyz;
                        if (getAnnotationDrawingSpaceCoordinate(multiPairedCoordShape,
                                                                multiPairedCoordShape->getCoordinate(i),
                                                                surfaceDisplayed,
                                                                xyz)) {
                            xyzToDraw.push_back(xyz);
                        }
                    }
                }
                    break;
                case 2:
                    /*
                     * Points in between near and far
                     */
                    if (m_volumeSpacePlane.isValidPlane()) {
                        for (int32_t i = 0; i < numHalfCoords; i++) {
                            Vector3D xyzOne, xyzTwo;
                            multiPairedCoordShape->getCoordinate(i)->getXYZ(xyzOne);
                            multiPairedCoordShape->getCoordinate(i + numHalfCoords)->getXYZ(xyzTwo);
                            
                            Vector3D xyz;
                            if (m_volumeSpacePlane.lineSegmentIntersectPlane(xyzOne,
                                                                             xyzTwo,
                                                                             xyz)) {
                                AnnotationCoordinate ac(*multiPairedCoordShape->getCoordinate(0));
                                ac.setXYZ(xyz);
                                
                                if (getAnnotationDrawingSpaceCoordinate(multiPairedCoordShape,
                                                                        &ac,
                                                                        surfaceDisplayed,
                                                                        xyz)) {
                                    xyzToDraw.push_back(xyz);
                                }
                            }
                        }
                    }
                    break;
            }
            
            if (static_cast<int32_t>(xyzToDraw.size()) == numHalfCoords) {
                switch (iDraw) {
                    case 0:
                        /*
                         * DRAWING: First half of coordinates, 'near' face
                         *
                         * To prevent problems, number of vertices in primitive
                         * must be all coordinates so that selection works but
                         * we only draw FIRST half of coordinates.
                         * Use start/count for primitive drawing.
                         */
                        drawEditableSizingHandlesFlag = true;
                        drawStartingCoordinateIndex = 0;
                        drawCoordinateCount = numHalfCoords;
                        for (int32_t i = 0; i < numHalfCoords; i++) {
                            xyzToDraw.push_back(xyzToDraw[i]);
                        }
                        CaretAssert(static_cast<int32_t>(xyzToDraw.size()) == numTotalCoords);
                        break;
                    case 1:
                        /*
                         * DRAWING: First half of coordinates, 'far' face
                         *
                         * To prevent problems, number of vertices in primitive
                         * must be all coordinates so that selection works but
                         * we only draw SECOND half of coordinates.
                         * Use start/count for primitive drawing.
                         */
                        drawEditableSizingHandlesFlag  = true;
                        drawStartingCoordinateIndex = numHalfCoords;
                        drawCoordinateCount = numHalfCoords;
                        for (int32_t i = 0; i < numHalfCoords; i++) {
                            xyzToDraw.push_back(xyzToDraw[i]);
                        }
                        CaretAssert(static_cast<int32_t>(xyzToDraw.size()) == numTotalCoords);
                        break;
                    case 2:
                        /*
                         * DRAWING: section in between 'near' and 'far'
                         *
                         * To prevent problems, number of vertices in primitive
                         * must be all coordinates so that selection works but
                         * we only draw SECOND half of coordinates.
                         * Use start/count for primitive drawing.
                         *
                         * NO sizing handles since coordinates cannot be dragged by user
                         */
                        drawNonEditableSizingHandlesFlag = true;
                        drawStartingCoordinateIndex = 0;
                        drawCoordinateCount = numHalfCoords;
                        for (int32_t i = 0; i < numHalfCoords; i++) {
                            xyzToDraw.push_back(xyzToDraw[i]);
                        }
                        CaretAssert(static_cast<int32_t>(xyzToDraw.size()) == numTotalCoords);
                        break;
                }
                
                for (auto& xyz : xyzToDraw) {
                    windowVertexXYZ.push_back(xyz);
                    primitive->addVertex(xyz);
                }
                
                /*
                 * Exit loop
                 */
                iDraw = 4;
            }
        }
    }
    
    bool drawLinesFlag(true);
    if ((drawStartingCoordinateIndex >= 0)
        && (drawCoordinateCount > 0)) {
        if (drawCoordinateCount < 2) {
            drawLinesFlag = false;
        }
        primitive->setDrawArrayIndicesSubset(drawStartingCoordinateIndex,
                                             drawCoordinateCount);
    }

    if ( ! multiPairedCoordShape->isDrawingNewAnnotation()) {
        if (primitive->getNumberOfVertices() < 2) {
            return false;
        }
    }
    
    const bool useNewViewportHeightInfoFlag(true);
    if (useNewViewportHeightInfoFlag) {
        if (primitive->getNumberOfVertices() > 0) {
            Vector3D windowXYZ;
            primitive->getVertexFloatXYZ(0, windowXYZ);
            setPrimitiveLineWidthInPixels(multiPairedCoordShape,
                                          windowXYZ,
                                          primitive.get());
        }
    }
    else {
        if (multiPairedCoordShape->getLineWidthPercentage() <= 0.0) {
            convertObsoleteLineWidthPixelsToPercentageWidth(multiPairedCoordShape);
        }
        
        const float lineWidthMultiplier(getLineWidthMultiplierForAnnotationBeingDrawn(multiPairedCoordShape));
        primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                (multiPairedCoordShape->getLineWidthPercentage()
                                 * lineWidthMultiplier));
    }
    
    BoundingBox boundingBox;
    primitive->getVertexBounds(boundingBox);
    float selectionCenterXYZ[3];
    boundingBox.getCenter(selectionCenterXYZ);
    
    const bool depthTestFlag = isDrawnWithDepthTesting(multiPairedCoordShape,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    bool drawnFlag = false;
    
    if (drawForegroundFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            
            const int32_t numberOfVertices = primitive->getNumberOfVertices();
            int32_t iStart(0);
            int32_t iEnd(numberOfVertices);
            if ((drawStartingCoordinateIndex >= 0)
                && (drawCoordinateCount > 0)) {
                iStart = drawStartingCoordinateIndex;
                iEnd   = drawStartingCoordinateIndex + drawCoordinateCount;
            }

            for (int32_t i = iStart; i < iEnd; i++) {
                int32_t nextCoordIndex(i + 1);
                if (i == (iEnd - 1)) {
                    nextCoordIndex = iStart;
                }
                
                uint8_t selectionColorRGBA[4];
                getIdentificationColor(selectionColorRGBA);
                std::unique_ptr<GraphicsPrimitiveV3f> idPrim(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES,
                                                                                                selectionColorRGBA));
                GraphicsPrimitive::LineWidthType lineWidthType = GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT;
                float lineWidth(0.0);
                primitive->getLineWidth(lineWidthType, lineWidth);
                lineWidth += 3.0; /* thicker to help with ID and reduce flashing of cursor */
                idPrim->setLineWidth(lineWidthType, lineWidth);
                CaretAssertVectorIndex(windowVertexXYZ, i);
                idPrim->addVertex(windowVertexXYZ[i]);
                
                CaretAssertVectorIndex(windowVertexXYZ, nextCoordIndex);
                idPrim->addVertex(windowVertexXYZ[nextCoordIndex]);

                m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                        multiPairedCoordShape,
                                                        AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                        i,
                                                        selectionCenterXYZ,
                                                        windowVertexXYZ));
                GraphicsEngineDataOpenGL::draw(idPrim.get());
                drawnFlag = true;
            }
        }
        else {
            if (drawLinesFlag) {
                GraphicsEngineDataOpenGL::draw(primitive.get());
            }
            drawnFlag = true;
        }
                
        if (drawEditableSizingHandlesFlag
            || drawNonEditableSizingHandlesFlag) {
            if (multiPairedCoordShape->isSelectedForEditing(m_inputs->m_windowIndex)) {
                const float sizeHandleWidthInPixels(computePolySizeHandleDiameter(primitive.get()));
                AnnotationSizingHandleTypeEnum::Enum sizeHandleType(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE);
                if (drawEditableSizingHandlesFlag) {
                    sizeHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE;
                }
                else if (drawNonEditableSizingHandlesFlag) {
                    sizeHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE;
                }
                else {
                    CaretAssert(0);
                }
                drawAnnotationMultiPairedCoordShapeSizingHandles(sizeHandleType,
                                                                 annotationFile,
                                                                 multiPairedCoordShape,
                                                                 windowVertexXYZ,
                                                                 primitive.get(),
                                                                 sizeHandleWidthInPixels);
            }
        }
        if (drawCreatingNewAnnotationSizingHandlesFlag) {
            const float sizeHandleWidthInPixels(computePolySizeHandleDiameter(primitive.get()));
            AnnotationSizingHandleTypeEnum::Enum sizeHandleType(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE);
            sizeHandleType = AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE;
            drawAnnotationMultiPairedCoordShapeSizingHandles(sizeHandleType,
                                                             annotationFile,
                                                             multiPairedCoordShape,
                                                             windowVertexXYZ,
                                                             primitive.get(),
                                                             sizeHandleWidthInPixels);
        }
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
    
    multiPairedCoordShape->setDrawnInWindowBounds(m_inputs->m_windowIndex,
                                                  boundingBox);
    
    return drawnFlag;
}

/**
 * @return The line width multiplier for the annotation being drawn.  Returns
 * one if the given annotation IS NOT the annotation currently being drawn by the user
 * @param annotation
 *   Annotation for testing to see if it is the annotation being drawn by the user.
 *
 * Note that the viewport for the annotation being drawn is always the window viewport.
 * Also note that lines are a percentage of the viewport height.
 * But if drawing in slice montage, the viewport is always "Number of Rows" too large.
 * So this is sort of a kludge to adjust the line width to sort of fix this problem.
 */
float
BrainOpenGLAnnotationDrawingFixedPipeline::getLineWidthMultiplierForAnnotationBeingDrawn(const Annotation* annotation) const
{
    float lineWidthMultiplier(1.0);
    
    if (annotation != NULL) {
        if (annotation == m_annotationBeingDrawn) {
            GraphicsViewport vp(GraphicsViewport::newInstanceCurrentViewport());
            const float vpHeight(vp.getHeightF());
            if ((vpHeight > 0.0)
                && (m_annotationBeingDrawnViewportHeight > 0.0)) {
                lineWidthMultiplier = m_annotationBeingDrawnViewportHeight / vpHeight;
            }
        }
    }

    return lineWidthMultiplier;
}

/**
 * Compute the diameter for a poly-type annotation size handle
 * @param primitive
 *    Primitive that is drawn and contains width of the line for the poly-type shape
 * @return
 *    Diameter for drawing the size handle.
 */
float
BrainOpenGLAnnotationDrawingFixedPipeline::computePolySizeHandleDiameter(const GraphicsPrimitive* primitive) const
{
    GraphicsPrimitive::LineWidthType lineWidthType = GraphicsPrimitive::LineWidthType::PIXELS;
    float lineWidth(0.0);
    primitive->getLineWidth(lineWidthType, lineWidth);
    float diameterInPixels = lineWidth;
    switch (lineWidthType) {
        case GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT:
        {
            float width(1.0);
            if (lineWidth >= 2) {
                width = lineWidth;
            }
            else {
                /*
                 * When line gets very narrow, make the symbol
                 * larger than the line width
                 *
                 * LineWidth:  SymbolWidth
                 *    0             1
                 *    1             1.5
                 *    2             2
                 */
                width = (0.5 * lineWidth) + 1.0;
            }
            width *= 1.5;
            diameterInPixels = GraphicsUtilitiesOpenGL::convertPercentageOfViewportHeightToPixels(width);
        }
            break;
        case GraphicsPrimitive::LineWidthType::PIXELS:
            diameterInPixels = (lineWidth * 3);
            break;
    }
    return diameterInPixels;
}
    
/**
 * Draw an annotation paired multi-coordinate
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param polyhedron
 *    Polyhedron annotation to draw.
 * @param plane
 *    Plane on which to draw polyhedron.
 * @param foregroundRGBA
 *    The foreground color
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawPolyhedronEdgesOnPlane(AnnotationFile* /*annotationFile*/,
                                                                      AnnotationPolyhedron* polyhedron,
                                                                      const Plane& plane,
                                                                      const uint8_t foregroundRGBA[4])
{
    if (m_selectionModeFlag) {
        /*
         * No identification
         */
        return false;
    }
    
    std::vector<AnnotationPolyhedron::Edge> edges;
    std::vector<AnnotationPolyhedron::Triangle> triangles;
    polyhedron->getEdgesAndTriangles(edges,
                                     triangles);
    
    if (edges.empty()
        && triangles.empty()) {
        return false;
    }
    
    if (foregroundRGBA[3] == 0) {
        return false;
    }
    
    const float lineWidthPct(polyhedron->getLineWidthPercentage());
    
    std::unique_ptr<GraphicsPrimitiveV3f> edgesPrimitive;
    edgesPrimitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::OPENGL_POINTS,
                                                       foregroundRGBA));
    CaretAssert(edgesPrimitive);
    edgesPrimitive->setPointDiameter(GraphicsPrimitive::PointSizeType::PERCENTAGE_VIEWPORT_HEIGHT,
                                lineWidthPct);
    
    std::unique_ptr<GraphicsPrimitiveV3f> trianglesPrimitive;
    trianglesPrimitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES,
                                                            foregroundRGBA));
    CaretAssert(trianglesPrimitive);
    trianglesPrimitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                                     lineWidthPct);
    
    for (const auto& e : edges) {
        Vector3D intersectionXYZ;
        if (plane.lineSegmentIntersectPlane(e.m_v1,
                                            e.m_v2,
                                            intersectionXYZ)) {
            
            AnnotationCoordinate ac(*polyhedron->getCoordinate(0));
            ac.setXYZ(intersectionXYZ);
            
            const Surface* invalidSurface(NULL);
            Vector3D xyz(intersectionXYZ);
            if (getAnnotationDrawingSpaceCoordinate(polyhedron,
                                                    &ac,
                                                    invalidSurface,
                                                    xyz)) {
                edgesPrimitive->addVertex(xyz);
            }
        }
    }
    
    for (const auto& t : triangles) {
        Vector3D intersectionOneXYZ;
        Vector3D intersectionTwoXYZ;
        if (plane.triangleIntersectPlane(t.m_v1,
                                         t.m_v2,
                                         t.m_v3,
                                         intersectionOneXYZ,
                                         intersectionTwoXYZ)) {
            AnnotationCoordinate acOne(*polyhedron->getCoordinate(0));
            acOne.setXYZ(intersectionOneXYZ);
            AnnotationCoordinate acTwo(*polyhedron->getCoordinate(0));
            acTwo.setXYZ(intersectionTwoXYZ);

            const Surface* invalidSurface(NULL);
            Vector3D xyzOne(intersectionOneXYZ);
            Vector3D xyzTwo(intersectionTwoXYZ);
            if (getAnnotationDrawingSpaceCoordinate(polyhedron,
                                                    &acOne,
                                                    invalidSurface,
                                                    xyzOne)
                && getAnnotationDrawingSpaceCoordinate(polyhedron,
                                                       &acTwo,
                                                       invalidSurface,
                                                       xyzTwo)) {
                trianglesPrimitive->addVertex(xyzOne);
                trianglesPrimitive->addVertex(xyzTwo);
            }
        }
    }

    
    if (edgesPrimitive->getNumberOfVertices() >= 2) {
        const Surface* invalidSurface(NULL);
        const bool depthTestFlag = isDrawnWithDepthTesting(polyhedron,
                                                           invalidSurface);
        const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
        GraphicsEngineDataOpenGL::draw(edgesPrimitive.get());
        setDepthTestingStatus(savedDepthTestStatus);
    }
    
    if (trianglesPrimitive->getNumberOfVertices() >= 3) {
        const Surface* invalidSurface(NULL);
        const bool depthTestFlag = isDrawnWithDepthTesting(polyhedron,
                                                           invalidSurface);
        const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
        GraphicsEngineDataOpenGL::draw(trianglesPrimitive.get());
        setDepthTestingStatus(savedDepthTestStatus);
    }
    
    return true;
}

/**
 * Draw an annotation multi-coordinate
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param multiCoordShape
 *    Annotation to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawMultiCoordinateShape(AnnotationFile* annotationFile,
                                                                    AnnotationMultiCoordinateShape* multiCoordShape,
                                                                    const Surface* surfaceDisplayed)
{
    AnnotationPolygon* polygon(NULL);
    AnnotationPolyLine* polyline(NULL);
    CaretAssert(multiCoordShape);
    switch (multiCoordShape->getType()) {
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::BROWSER_TAB:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::LINE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::POLYHEDRON:
        case AnnotationTypeEnum::SCALE_BAR:
        case AnnotationTypeEnum::TEXT:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::POLYGON:
            polygon = multiCoordShape->castToPolygon();
            CaretAssert(polygon);
            break;
        case AnnotationTypeEnum::POLYLINE:
            polyline = multiCoordShape->castToPolyline();
            CaretAssert(polyline);
    }
    
    uint8_t foregroundRGBA[4];
    multiCoordShape->getLineColorRGBA(foregroundRGBA);
    const bool drawForegroundFlag = (foregroundRGBA[3] > 0.0f);
    
    std::vector<Vector3D> windowVertexXYZ;
    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive;
    if (polygon != NULL) {
        primitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN,
                                                           foregroundRGBA));
    }
    else if (polyline != NULL) {
        primitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN,
                                                           foregroundRGBA));
    }
    else {
        CaretAssertMessage(0, "Has new multi-coord annotation type been added?");
        return false;
    }
    CaretAssert(primitive);
    
    const int32_t numCoords = multiCoordShape->getNumberOfCoordinates();
    for (int32_t i = 0; i < numCoords; i++) {
        Vector3D xyz;
        if ( ! getAnnotationDrawingSpaceCoordinate(multiCoordShape,
                                                   multiCoordShape->getCoordinate(i),
                                                   surfaceDisplayed,
                                                   xyz)) {
            return false;
        }
        windowVertexXYZ.push_back(xyz);
        primitive->addVertex(xyz);
    }
        
    if (multiCoordShape->getLineWidthPercentage() <= 0.0) {
        convertObsoleteLineWidthPixelsToPercentageWidth(multiCoordShape);
    }
    const float lineWidthMultiplier(getLineWidthMultiplierForAnnotationBeingDrawn(multiCoordShape));
    primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT,
                            (multiCoordShape->getLineWidthPercentage()
                             * lineWidthMultiplier));
    
    BoundingBox boundingBox;
    primitive->getVertexBounds(boundingBox);
    float selectionCenterXYZ[3];
    boundingBox.getCenter(selectionCenterXYZ);
    
    const bool depthTestFlag = isDrawnWithDepthTesting(multiCoordShape,
                                                       surfaceDisplayed);
    const bool savedDepthTestStatus = setDepthTestingStatus(depthTestFlag);
    
    
    bool drawnFlag = false;
    
    if (drawForegroundFlag) {
        if (m_selectionModeFlag
            && m_inputs->m_annotationUserInputModeFlag) {
            for (int32_t i = 0; i < numCoords; i++) {
                int32_t nextCoordIndex(i + 1);
                if (i == (numCoords - 1)) {
                    nextCoordIndex = 0;
                    if (polygon == NULL) {
                        break;
                    }
                }
                
                uint8_t selectionColorRGBA[4];
                getIdentificationColor(selectionColorRGBA);
                std::unique_ptr<GraphicsPrimitiveV3f> idPrim(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES,
                                                                                             selectionColorRGBA));
                GraphicsPrimitive::LineWidthType lineWidthType = GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT;
                float lineWidth(0.0);
                primitive->getLineWidth(lineWidthType, lineWidth);
                lineWidth += 3.0; /* thicker to help with ID and reduce flashing of cursor */
                idPrim->setLineWidth(lineWidthType, lineWidth);
                CaretAssertVectorIndex(windowVertexXYZ, i);
                idPrim->addVertex(windowVertexXYZ[i]);
                
                CaretAssertVectorIndex(windowVertexXYZ, nextCoordIndex);
                idPrim->addVertex(windowVertexXYZ[nextCoordIndex]);
                
                m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                        multiCoordShape,
                                                        AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                        i,
                                                        selectionCenterXYZ,
                                                        windowVertexXYZ));
                GraphicsEngineDataOpenGL::draw(idPrim.get());
                drawnFlag = true;
            }
        }
        else {
            GraphicsEngineDataOpenGL::draw(primitive.get());
            drawnFlag = true;
        }
        
        if (multiCoordShape->isSelectedForEditing(m_inputs->m_windowIndex)
            || multiCoordShape->isDrawingNewAnnotation()) {
            const float sizeHandleWidthInPixels = computePolySizeHandleDiameter(primitive.get());
            drawAnnotationMultiCoordShapeSizingHandles(annotationFile,
                                                       multiCoordShape,
                                                       windowVertexXYZ,
                                                       primitive.get(),
                                                       sizeHandleWidthInPixels);
        }
    }
    
    setDepthTestingStatus(savedDepthTestStatus);
    
    multiCoordShape->setDrawnInWindowBounds(m_inputs->m_windowIndex,
                                            boundingBox);
    
    return drawnFlag;
}


/**
 * Draw an annotation poly line that is in surface space with tangent offset.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param multiCoordShape
 *    Multi-coord shape to draw.
 * @param surfaceDisplayed
 *    Surface that is displayed (may be NULL).
 * @return
 *    True if the annotation was drawn while NOT selecting annotations.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::drawMultiCoordinateShapeSurfaceTangentOffset(AnnotationFile* annotationFile,
                                                                                        AnnotationMultiCoordinateShape* multiCoordShape,
                                                                                        const Surface* surfaceDisplayed,
                                                                                        const float surfaceExtentZ)
{
    AnnotationPolygon* polygon(NULL);
    AnnotationPolyLine* polyline(NULL);
    CaretAssert(multiCoordShape);
    switch (multiCoordShape->getType()) {
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::BROWSER_TAB:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::LINE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::POLYHEDRON:
        case AnnotationTypeEnum::SCALE_BAR:
        case AnnotationTypeEnum::TEXT:
            CaretAssert(0);
            break;
        case AnnotationTypeEnum::POLYGON:
            polygon = multiCoordShape->castToPolygon();
            CaretAssert(polygon);
            break;
        case AnnotationTypeEnum::POLYLINE:
            polyline = multiCoordShape->castToPolyline();
            CaretAssert(polyline);
    }
    CaretAssert(surfaceDisplayed);
    
    const int32_t numCoords = multiCoordShape->getNumberOfCoordinates();
    if (numCoords < 2) {
        return false;
    }
    
    uint8_t foregroundRGBA[4];
    multiCoordShape->getLineColorRGBA(foregroundRGBA);
    if (foregroundRGBA[3] <= 0) {
        return false;
    }
    
    std::vector<Vector3D> windowVertexXYZ;
    
    const StructureEnum::Enum surfaceStructure(surfaceDisplayed->getStructure());
    const int32_t surfaceVertexCount(surfaceDisplayed->getNumberOfNodes());
    
    std::unique_ptr<GraphicsPrimitiveV3f> primitive;
    if (polygon != NULL) {
        primitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_LOOP_BEVEL_JOIN,
                                                           foregroundRGBA));
    }
    else if (polyline != NULL) {
        primitive.reset(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINE_STRIP_BEVEL_JOIN,
                                                           foregroundRGBA));
    }
    else {
        CaretAssertMessage(0, "Has new multi-coord annotation type been added?");
        return false;
    }
    CaretAssert(primitive);
    
    for (int32_t i = 0; i < numCoords; i++) {
        const AnnotationCoordinate* ac = multiCoordShape->getCoordinate(i);
        StructureEnum::Enum structure(StructureEnum::INVALID);
        int32_t vertexCount(-1);
        int32_t vertexIndex(-1);
        float offsetLength(0.0);
        AnnotationSurfaceOffsetVectorTypeEnum::Enum offsetVector(AnnotationSurfaceOffsetVectorTypeEnum::TANGENT);
        ac->getSurfaceSpace(structure, vertexCount, vertexIndex,
                            offsetLength, offsetVector);
        if (structure != surfaceStructure) {
            CaretLogWarning("Polyline vertex does not map to displayed surface");
            return false;
        }
        else if (vertexCount != surfaceVertexCount) {
            CaretLogWarning("Polyline vertex maps to surface with different vertex count");
            return false;
        }
        
        Vector3D xyz;
        surfaceDisplayed->getCoordinate(vertexIndex, xyz);
        
        float normalVector[3];
        surfaceDisplayed->getNormalVector(vertexIndex, normalVector);
        
        for (int32_t n = 0; n < 3; n++) {
            xyz[n] += (normalVector[n] * offsetLength);
        }
        
        primitive->addVertex(xyz);
        windowVertexXYZ.push_back(xyz);
    }
    
    if (multiCoordShape->getLineWidthPercentage() <= 0.0) {
        convertObsoleteLineWidthPixelsToPercentageWidth(multiCoordShape);
    }
    float lineThickness = ((multiCoordShape->getLineWidthPercentage() / 100.0)
                           * surfaceExtentZ);
    lineThickness *= m_surfaceViewScaling;
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        lineThickness = std::max(lineThickness,
                                 s_selectionLineMinimumPixelWidth);
    }
    lineThickness = GraphicsUtilitiesOpenGL::convertMillimetersToPixels(lineThickness);
    primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS,
                            lineThickness);
    
    if (m_selectionModeFlag
        && m_inputs->m_annotationUserInputModeFlag) {
        BoundingBox bb;
        primitive->getVertexBounds(bb);
        float selectionCenterXYZ[3];
        bb.getCenter(selectionCenterXYZ);
        for (int32_t i = 0; i < numCoords; i++) {
            int32_t nextCoordIndex(i + 1);
            if (i == (numCoords - 1)) {
                nextCoordIndex = 0;
                if (polygon == NULL) {
                    break;
                }
            }
            
            uint8_t selectionColorRGBA[4];
            getIdentificationColor(selectionColorRGBA);
            std::unique_ptr<GraphicsPrimitiveV3f> idPrim(GraphicsPrimitive::newPrimitiveV3f(GraphicsPrimitive::PrimitiveType::POLYGONAL_LINES,
                                                                                            selectionColorRGBA));
            GraphicsPrimitive::LineWidthType lineWidthType = GraphicsPrimitive::LineWidthType::PERCENTAGE_VIEWPORT_HEIGHT;
            float lineWidth(0.0);
            primitive->getLineWidth(lineWidthType, lineWidth);
            lineWidth += 3.0; /* thicker to help with ID and reduce flashing of cursor */
            idPrim->setLineWidth(lineWidthType, lineWidth);
            CaretAssertVectorIndex(windowVertexXYZ, i);
            idPrim->addVertex(windowVertexXYZ[i]);
            
            CaretAssertVectorIndex(windowVertexXYZ, nextCoordIndex);
            idPrim->addVertex(windowVertexXYZ[nextCoordIndex]);
            
            m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                    multiCoordShape,
                                                    AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE,
                                                    i,
                                                    selectionCenterXYZ,
                                                    windowVertexXYZ));
            GraphicsEngineDataOpenGL::draw(idPrim.get());
        }
    }
    else {
        GraphicsEngineDataOpenGL::draw(primitive.get());
    }
    
    if (multiCoordShape->isSelectedForEditing(m_inputs->m_windowIndex)) {
        const float minPixelSize = 30.0;
        const float minSizeMM = (GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(minPixelSize)
                                 * m_surfaceViewScaling);
        const float handleThickness = std::max(minSizeMM,
                                               lineThickness * 2.0f);
        drawAnnotationMultiCoordShapeSizingHandles(annotationFile,
                                            multiCoordShape,
                                            windowVertexXYZ,
                                            primitive.get(),
                                            handleThickness);
    }
    
    return true;
}

/**
 * When lines are very thin, they can be difficult to select.  This method will
 * return the line thickness percentage, adjusted so that is thick enough that
 * the user will be able to select the annotation.
 *
 * @param
 *     Annotation drawn as line
 * @return
 *     Percentage thickness for drawing that may be increased to ensure that
       the annotation is selectable.
 */
float
BrainOpenGLAnnotationDrawingFixedPipeline::getLineWidthPercentageInSelectionMode(const Annotation* annotation) const
{
    CaretAssert(annotation);
    const float minPercentHeight = GraphicsUtilitiesOpenGL::convertPixelsToPercentageOfViewportHeight(s_selectionLineMinimumPixelWidth);
    float percentHeight = annotation->getLineWidthPercentage();
    if (percentHeight < minPercentHeight) {
        percentHeight = minPercentHeight;
    }
    
    return percentHeight;
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
 * @param verticesWindowXYZ
 *    Window coordinates of annotation's vertices when drawing annotation
 * @param xyz
 *     Center of square.
 * @param halfWidthHeight
 *     Half Width/height of square.
 * @param rotationAngle
 *     Rotation angle for the sizing handle.
 * @param polyLineCoordinateIndex
 *     Index of poly line coordinate
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawSizingHandle(const AnnotationSizingHandleTypeEnum::Enum handleType,
                                                            AnnotationFile* annotationFile,
                                                            Annotation* annotation,
                                                            const std::vector<Vector3D>& verticesWindowXYZ,
                                                            const float xyz[3],
                                                            const float halfWidthHeight,
                                                            const float rotationAngle,
                                                            const int32_t polyLineCoordinateIndex)
{
    glPushMatrix();
    
    glTranslatef(xyz[0], xyz[1], xyz[2]);
    if (rotationAngle != 0.0) {
        glRotatef(-rotationAngle, 0.0, 0.0, 1.0);
    }
    
    const float bottomLeft[3]  = { -halfWidthHeight, -halfWidthHeight, 0.0f };
    const float bottomRight[3] = { halfWidthHeight,  -halfWidthHeight, 0.0f };
    const float topRight[3]    = { halfWidthHeight,   halfWidthHeight, 0.0f };
    const float topLeft[3]     = { -halfWidthHeight,  halfWidthHeight, 0.0f };

    bool drawTwoToneFilledCircleFlag(false);
    bool drawFilledCircleFlag  = false;
    bool drawOutlineCircleFlag = false;
    bool drawSquareFlag        = false;
    bool drawSphereFlag        = false;
    
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
            if (annotation->isInSurfaceSpaceWithTangentOffset()) {
                drawSphereFlag = true;
            }
            else {
                drawFilledCircleFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START:
            if (annotation->isInSurfaceSpaceWithTangentOffset()) {
                drawSphereFlag = true;
            }
            else {
                drawFilledCircleFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NONE:
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION:
            drawOutlineCircleFlag = true;
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE:
            if (annotation->isInSurfaceSpaceWithTangentOffset()) {
                drawSphereFlag = true;
            }
            else {
                drawTwoToneFilledCircleFlag = true;
            }
            break;
        case AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_NOT_EDITABLE_POLY_LINE_COORDINATE:
            drawOutlineCircleFlag = true;
            break;
    }
    
    if (annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
        drawFilledCircleFlag  = false;
        drawOutlineCircleFlag = false;
        drawSphereFlag        = false;
        drawSquareFlag        = true;
    }
    
    bool selectionFlag(false);
    if (m_selectionModeFlag) {
        switch (annotation->getType()) {
            case AnnotationTypeEnum::BOX:
            case AnnotationTypeEnum::COLOR_BAR:
            case AnnotationTypeEnum::IMAGE:
            case AnnotationTypeEnum::LINE:
            case AnnotationTypeEnum::OVAL:
            case AnnotationTypeEnum::POLYHEDRON:
            case AnnotationTypeEnum::POLYGON:
            case AnnotationTypeEnum::POLYLINE:
            case AnnotationTypeEnum::SCALE_BAR:
            case AnnotationTypeEnum::TEXT:
                selectionFlag = m_inputs->m_annotationUserInputModeFlag;
                break;
            case AnnotationTypeEnum::BROWSER_TAB:
                selectionFlag = m_inputs->m_tileTabsManualLayoutUserInputModeFlag;
                break;
        }
    }
    
    uint8_t symbolRGBA[4] {
        m_foregroundRGBA[0],
        m_foregroundRGBA[1],
        m_foregroundRGBA[2],
        m_foregroundRGBA[3]
    };

    if (selectionFlag) {
        getIdentificationColor(symbolRGBA);
        m_selectionInfo.push_back(SelectionInfo(annotationFile,
                                                annotation,
                                                handleType,
                                                polyLineCoordinateIndex,
                                                xyz,
                                                verticesWindowXYZ));
        if (drawOutlineCircleFlag) {
            drawFilledCircleFlag = true;
        }
    }
    
    if (drawTwoToneFilledCircleFlag) {
        /*
         * Circle is in foreground color surrounded by background color
         */
        GraphicsShape::drawCircleFilled(NULL,
                                        m_backgroundRGBA,
                                        (halfWidthHeight * 2 + 5));
        GraphicsShape::drawCircleFilled(NULL,
                                        symbolRGBA,
                                        halfWidthHeight * 2);
    }
    else if (drawFilledCircleFlag) {
        GraphicsShape::drawCircleFilled(NULL,
                                        symbolRGBA,
                                        halfWidthHeight * 2);
    }
    else if (drawSquareFlag) {
        GraphicsShape::drawBoxFilledByteColor(bottomLeft,
                                              bottomRight,
                                              topRight,
                                              topLeft,
                                              symbolRGBA);
    }
    else if (drawOutlineCircleFlag) {
        const float diameter = halfWidthHeight;
        glPushMatrix();
        glScaled(diameter, diameter, 1.0f);
        GraphicsShape::drawRing(NULL,
                                symbolRGBA,
                                0.7f,
                                1.0f);
        glPopMatrix();
    }
    else if (drawSphereFlag) {
        float zeros[3] { 0.0f, 0.0f, 0.0f };
        GraphicsShape::drawSphereByteColor(zeros,
                                           symbolRGBA,
                                           halfWidthHeight);
    }

    glPopMatrix();
}

/**
 * Draw sizing handles around a two-coordinate annotation.
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
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationLineSizingHandles(AnnotationFile* annotationFile,
                                                                             Annotation* annotation,
                                                                           const std::vector<Vector3D>& verticesWindowXYZ,
                                                                             const float firstPoint[3],
                                                                        const float secondPoint[3],
                                                                        const float lineThickness)
{
    if ( ! m_inputs->m_annotationUserInputModeFlag) {
        return;
    }
    
    CaretAssert(annotation);
    float lengthVector[3];
    MathFunctions::subtractVectors(secondPoint, firstPoint, lengthVector);
    MathFunctions::normalizeVector(lengthVector);
    
    const float dx = secondPoint[0] - firstPoint[0];
    const float dy = secondPoint[1] - firstPoint[1];
    
    const bool tangentSurfaceOffsetFlag = annotation->isInSurfaceSpaceWithTangentOffset();

    float cornerSquareSize = 3.0 + lineThickness;
    if (tangentSurfaceOffsetFlag) {
        cornerSquareSize = lineThickness;
        cornerSquareSize = GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(cornerSquareSize);
    }
    
    float directionVector[3] { 0.0f, 0.0f, 0.0f };
    if ( ! tangentSurfaceOffsetFlag) {
        directionVector[0] = lengthVector[0] * cornerSquareSize;
        directionVector[1] = lengthVector[1] * cornerSquareSize;
        directionVector[2] = 0.0;
    }
    
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
    
    const int32_t invalidPolyLineCoordinateIndex(-1);
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START)) {
        /*
         * Symbol for first coordinate is a little bigger
         */
        float startSquareSize = cornerSquareSize + 2.0;
        if (tangentSurfaceOffsetFlag) {
            startSquareSize = cornerSquareSize;
        }
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_START,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         firstPointSymbolXYZ,
                         startSquareSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_LINE_END,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         secondPointSymbolXYZ,
                         cornerSquareSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        const float midPointXYZ[3] = {
            (firstPoint[0] + secondPoint[0]) / 2.0f,
            (firstPoint[1] + secondPoint[1]) / 2.0f,
            (firstPoint[2] + secondPoint[2]) / 2.0f
        };
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         midPointXYZ,
                         cornerSquareSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
}

/**
 * Draw sizing handles around a two-dimensional annotation.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param annotation
 *    Annotation to draw.
 * @param verticesWindowXYZ
 *    Window coordinates of annotation's vertices when drawing annotation
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
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationTwoDimShapeSizingHandles(AnnotationFile* annotationFile,
                                                                                  Annotation* annotation,
                                                                                  const std::vector<Vector3D>& verticesWindowXYZ,
                                                                                  const float bottomLeft[3],
                                                                                  const float bottomRight[3],
                                                                                  const float topRight[3],
                                                                                  const float topLeft[3],
                                                                                  const float lineThickness,
                                                                                  const float rotationAngle)
{
    
    CaretAssert(annotation);
    
    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
        case AnnotationTypeEnum::COLOR_BAR:
        case AnnotationTypeEnum::IMAGE:
        case AnnotationTypeEnum::LINE:
        case AnnotationTypeEnum::OVAL:
        case AnnotationTypeEnum::POLYHEDRON:
        case AnnotationTypeEnum::POLYGON:
        case AnnotationTypeEnum::POLYLINE:
        case AnnotationTypeEnum::SCALE_BAR:
        case AnnotationTypeEnum::TEXT:
            if ( ! m_inputs->m_annotationUserInputModeFlag) {
                return;
            }
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            if ( ! m_inputs->m_tileTabsManualLayoutUserInputModeFlag) {
                return;
            }
            break;
    }
    
    AnnotationText* textAnn = dynamic_cast<AnnotationText*>(annotation);
    const bool modelSpaceTangentTextFlag = annotation->isInSurfaceSpaceWithTangentOffset();
    
    float heightVector[3];
    MathFunctions::subtractVectors(topLeft, bottomLeft, heightVector);
    MathFunctions::normalizeVector(heightVector);
    
    float innerSpacing = 2.0f + (lineThickness / 2.0f);
    if (modelSpaceTangentTextFlag) {
        innerSpacing = GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(innerSpacing);
    }
    
    /*
     * Must shrink box around browser tab or else the box is outside of the
     * browser tab and not seen
     */
    if (annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
        innerSpacing = -innerSpacing;
    }
    
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
    
    MathFunctions::expandBox(handleBottomLeft, handleBottomRight, handleTopRight, handleTopLeft, innerSpacing, innerSpacing);
    
    if (! m_selectionModeFlag) {
        GraphicsShape::drawBoxOutlineByteColor(handleBottomLeft, handleBottomRight, handleTopRight, handleTopLeft,
                                               m_foregroundRGBA, GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
    }
    
    float sizeHandleSize = 5.0;
    if (modelSpaceTangentTextFlag) {
        sizeHandleSize = GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(sizeHandleSize);
    }

    float handleOffset(0.0);
    if (annotation->getType() == AnnotationTypeEnum::BROWSER_TAB) {
        if (m_selectionModeFlag) {
            sizeHandleSize *= 4.0;
        }
        else {
            sizeHandleSize *= 2.0;
            MathFunctions::expandBox(handleBottomLeft, handleBottomRight, handleTopRight, handleTopLeft, -sizeHandleSize, -sizeHandleSize);
        }
    }
    const float handleLeft[3] = {
        (handleBottomLeft[0] + handleTopLeft[0]) / 2.0f + handleOffset,
        (handleBottomLeft[1] + handleTopLeft[1]) / 2.0f,
        (handleBottomLeft[2] + handleTopLeft[2]) / 2.0f,
    };
    
    const float handleRight[3] = {
        (handleBottomRight[0] + handleTopRight[0]) / 2.0f - handleOffset,
        (handleBottomRight[1] + handleTopRight[1]) / 2.0f,
        (handleBottomRight[2] + handleTopRight[2]) / 2.0f,
    };
    
    const float handleBottom[3] = {
        (handleBottomLeft[0] + handleBottomRight[0]) / 2.0f,
        (handleBottomLeft[1] + handleBottomRight[1]) / 2.0f + handleOffset,
        (handleBottomLeft[2] + handleBottomRight[2]) / 2.0f,
    };
    
    const float handleTop[3] = {
        (handleTopLeft[0] + handleTopRight[0]) / 2.0f,
        (handleTopLeft[1] + handleTopRight[1]) / 2.0f - handleOffset,
        (handleTopLeft[2] + handleTopRight[2]) / 2.0f,
    };
    
    const int32_t invalidPolyLineCoordinateIndex(-1);
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_LEFT,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleBottomLeft,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM_RIGHT,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleBottomRight,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_RIGHT,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleTopRight,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP_LEFT,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleTopLeft,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_TOP,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleTop,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_BOTTOM,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleBottom,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_RIGHT,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleRight,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT)) {
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_BOX_LEFT,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleLeft,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
    
    if (annotation->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION)) {
        float handleOffset[3] = {
            handleTop[0],
            handleTop[1],
            handleTop[2]
        };
        
        if (textAnn != NULL) {
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
            handleOffset[0] + (rotationOffset * 0.75f * heightVector[0] ),
            handleOffset[1] + (rotationOffset * 0.75f * heightVector[1]),
            handleOffset[2] + (rotationOffset * 0.75f * heightVector[2])
        };
        
        /*
         * Rotation handle and line connecting rotation handle to selection box
         */
        std::vector<float> coords;
        coords.insert(coords.end(), handleRotationLineEnd, handleRotationLineEnd + 3);
        coords.insert(coords.end(), handleOffset, handleOffset + 3);
        GraphicsShape::drawLinesByteColor(coords, m_foregroundRGBA,
                                          GraphicsPrimitive::LineWidthType::PIXELS, 2.0f);
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_ROTATION,
                         annotationFile,
                         annotation,
                         verticesWindowXYZ,
                         handleRotation,
                         sizeHandleSize,
                         rotationAngle,
                         invalidPolyLineCoordinateIndex);
    }
}

/**
 * Draw sizing handles around a multi-coord annotation.
 *
 * @param annotationFile
 *    File containing the annotation.
 * @param multiCoordShape
 *    Multi-coord nnotation to draw.
 * @param primitive
 *     Primitive that draws the annotation
 * @param lineThickness
 *     Thickness of lines (when enabled).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationMultiCoordShapeSizingHandles(AnnotationFile* annotationFile,
                                                                                      AnnotationMultiCoordinateShape* multiCoordShape,
                                                                                      const std::vector<Vector3D>& verticesWindowXYZ,
                                                                                      const GraphicsPrimitive* primitive,
                                                                                      const float lineThickness)
{
    if ( ! multiCoordShape->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE)) {
        return;
    }
    
    const bool tangentSurfaceOffsetFlag = multiCoordShape->isInSurfaceSpaceWithTangentOffset();
    
    float cornerSquareSize = lineThickness;
    if (tangentSurfaceOffsetFlag) {
        cornerSquareSize = lineThickness;
        cornerSquareSize = GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(cornerSquareSize) / 2.0f;
    }
    
    const float rotationAngle(0.0);
    
    const int32_t numberOfVertices = primitive->getNumberOfVertices();
    for (int32_t i = 0; i < numberOfVertices; i++) {
        float xyz[3];
        primitive->getVertexFloatXYZ(i, xyz);

        const float halfSymbolSize(cornerSquareSize / 2.0);
        drawSizingHandle(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE,
                         annotationFile,
                         multiCoordShape,
                         verticesWindowXYZ,
                         xyz,
                         halfSymbolSize,
                         rotationAngle,
                         i);
    }
}

/**
 * Draw sizing handles around a multi-paired coord annotation.
 *
 * @param sizingHandleType
 *    Type of sizing handle to draw
 * @param annotationFile
 *    File containing the annotation.
 * @param multiPairedCoordShape
 *    Multi-paired-coord nnotation to draw.
 * @param primitive
 *     Primitive that draws the annotation
 * @param lineThickness
 *     Thickness of lines (when enabled).
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::drawAnnotationMultiPairedCoordShapeSizingHandles(const AnnotationSizingHandleTypeEnum::Enum sizingHandleType,
                                                                                            AnnotationFile* annotationFile,
                                                                                            AnnotationMultiPairedCoordinateShape* multiPairedCoordShape,
                                                                                            const std::vector<Vector3D>& verticesWindowXYZ,
                                                                                            const GraphicsPrimitive* primitive,
                                                                                            const float lineThickness)
{
    if ( ! multiPairedCoordShape->isSizeHandleValid(AnnotationSizingHandleTypeEnum::ANNOTATION_SIZING_HANDLE_EDITABLE_POLY_LINE_COORDINATE)) {
        return;
    }
    
    const bool tangentSurfaceOffsetFlag = multiPairedCoordShape->isInSurfaceSpaceWithTangentOffset();
    
    float cornerSquareSize = lineThickness;
    if (tangentSurfaceOffsetFlag) {
        cornerSquareSize = lineThickness;
        cornerSquareSize = GraphicsUtilitiesOpenGL::convertPixelsToMillimeters(cornerSquareSize) / 2.0f;
    }
    
    const float rotationAngle(0.0);
    
    int32_t primStart(-1), primCount(-1);
    primitive->getDrawArrayIndicesSubset(primStart, primCount);
    
    const int32_t numberOfVertices = primitive->getNumberOfVertices();
    int32_t iStart(0);
    int32_t iEnd(numberOfVertices);
    if ((primStart >= 0)
        && (primCount > 0)) {
        iStart = primStart;
        iEnd   = primStart + primCount;
    }
    
    for (int32_t i = iStart; i < iEnd; i++) {
        float xyz[3];
        primitive->getVertexFloatXYZ(i, xyz);

        const float halfSymbolSize(cornerSquareSize / 2.0);
        drawSizingHandle(sizingHandleType,
                         annotationFile,
                         multiPairedCoordShape,
                         verticesWindowXYZ,
                         xyz,
                         halfSymbolSize,
                         rotationAngle,
                         i);
    }
}

/**
 * Set the color for drawing the selection box and handles.
 *
 * @param annotation
 *     Annotation this is being drawn.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::setSelectionBoxColor(const Annotation* annotation)
{
    CaretAssert(annotation);
    
    /*
     * Use the foreground color but reduce the intensity and saturation.
     */
    m_foregroundRGBA[0] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[0];
    m_foregroundRGBA[1] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[1];
    m_foregroundRGBA[2] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[2];
    m_foregroundRGBA[3] = m_brainOpenGLFixedPipeline->m_foregroundColorByte[3];
    
    m_backgroundRGBA[0] = m_brainOpenGLFixedPipeline->m_backgroundColorByte[0];
    m_backgroundRGBA[1] = m_brainOpenGLFixedPipeline->m_backgroundColorByte[1];
    m_backgroundRGBA[2] = m_brainOpenGLFixedPipeline->m_backgroundColorByte[2];
    m_backgroundRGBA[3] = m_brainOpenGLFixedPipeline->m_backgroundColorByte[3];

    switch (annotation->getType()) {
        case AnnotationTypeEnum::BOX:
            break;
        case AnnotationTypeEnum::BROWSER_TAB:
            /*
             * Foreground color is loaded into browser tab by BrainOpenGLFixedPipeline
             */
            annotation->getLineColorRGBA(m_foregroundRGBA);
            break;
        case AnnotationTypeEnum::COLOR_BAR:
            break;
        case AnnotationTypeEnum::IMAGE:
            break;
        case AnnotationTypeEnum::LINE:
            break;
        case AnnotationTypeEnum::OVAL:
            break;
        case AnnotationTypeEnum::POLYHEDRON:
            break;
        case AnnotationTypeEnum::POLYGON:
            break;
        case AnnotationTypeEnum::POLYLINE:
            break;
        case AnnotationTypeEnum::SCALE_BAR:
            break;
        case AnnotationTypeEnum::TEXT:
            break;
    }
    
    QColor color(m_foregroundRGBA[0],
                 m_foregroundRGBA[1],
                 m_foregroundRGBA[2],
                 m_foregroundRGBA[3]);
    
#if QT_VERSION >= 0x060000
    float hue = 0.0;
    float saturation = 0.0;
    float value = 0.0;
#else
    qreal hue = 0.0;
    qreal saturation = 0.0;
    qreal value = 0.0;
#endif
    
    const bool reduceBrightnessFlag(false);
    if (reduceBrightnessFlag) {
        color.getHsvF(&hue,
                      &saturation,
                      &value);
        
        saturation *= 0.8;
        value      *= 0.8;
        
        color.setHsvF(hue,
                      saturation,
                      value);
    }
    
    m_foregroundRGBA[0] = static_cast<uint8_t>(color.red());
    m_foregroundRGBA[1] = static_cast<uint8_t>(color.green());
    m_foregroundRGBA[2] = static_cast<uint8_t>(color.blue());
}


/**
 * Is the annotation drawn with depth testing enabled (based upon coordinate space)?
 *
 * @param annotation
 *     Annotation that will be drawn.
 * @param surface
 *     Surface that is being drawn.
 * @return 
 *     True if the annotation is drawn with depth testing, else false.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::isDrawnWithDepthTesting(const Annotation* annotation,
                                                                   const Surface* surface)
{
    bool depthTestFlag = true;
    bool testFlatSurfaceFlag = false;
    
    switch (annotation->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::HISTOLOGY:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            depthTestFlag = false;
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            testFlatSurfaceFlag = true;
            
            /*
             * In stereotaxic space annotations on histology slices, logic elsewhere
             * will only draw annotation if it is close to the slice's plane.
             */
            if (m_histologySpacePlaneValid) {
                depthTestFlag = false;
            }
            if (m_volumeSpacePlane.isValidPlane()) {
                depthTestFlag = false;
            }
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            testFlatSurfaceFlag = true;
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            break;
    }

    if (testFlatSurfaceFlag) {
        if (surface != NULL) {
            if (surface->getSurfaceType() == SurfaceTypeEnum::FLAT) {
                depthTestFlag = false;
            }
        }
    }
    
    return depthTestFlag;
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
    
    if (newDepthTestingStatus) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    
    return (savedStatus == GL_TRUE);
}

/**
 * Convert a percentage height to a line width in pixels
 *
 * @param percentageHeight
 *     Percentage of viewport height.
 * @return
 *     Line width in pixels clamped to valid range
 */
float
BrainOpenGLAnnotationDrawingFixedPipeline::getLineWidthFromPercentageHeight(const float percentageHeight) const
{
    float widthPixels = (percentageHeight / 100.0f) * m_modelSpaceViewport[3];
    
    if (widthPixels < m_lineWidthMinimum) {
        widthPixels = m_lineWidthMinimum;
    }
    
    return widthPixels;
}

/**
 * Set the line width, in pixels, for drawing the primitive.  If the annotation uses "percentage of viewport" height,
 * the appropriate viewport height is found so that the line thickness is correct in pixels.
 * @param annotation
 *    The annotation
 * @param windowXY
 *    XY in window
 * @param primtive
 *    The primitive
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::setPrimitiveLineWidthInPixels(const Annotation* annotation,
                                                                         const Vector3D& windowXY,
                                                                         GraphicsPrimitive* primitive) const
{
    CaretAssert(annotation);
    CaretAssert(primitive);

    if (annotation->getLineWidthPercentage() < 0.0) {
        convertObsoleteLineWidthPixelsToPercentageWidth(annotation);
    }
    const float lineWidthPercentage(annotation->getLineWidthPercentage());
    if (lineWidthPercentage > 0.0) {
        bool modelSpaceFlag(false);
        bool tabSpaceFlag(false);
        bool windowSpaceFlag(false);
        switch (annotation->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                modelSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::HISTOLOGY:
                modelSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                modelSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                tabSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                modelSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
                modelSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
                tabSpaceFlag = true;
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                CaretAssert(0);
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
                windowSpaceFlag = true;
                break;
        }
        
        float viewportHeight(0.0);
        if (modelSpaceFlag) {
            /*
             * Current viewport is in use for model space annotations
             */
            GraphicsViewport vp(GraphicsViewport::newInstanceCurrentViewport());
            viewportHeight = vp.getHeightF();
        }
        else if (tabSpaceFlag) {
            auto vpEvent(EventDrawingViewportContentGet::newInstanceGetContentType(m_brainOpenGLFixedPipeline->m_windowIndex,
                                                                                   windowXY,
                                                                                   DrawingViewportContentTypeEnum::TAB_AFTER_ASPECT_LOCK));
            EventManager::get()->sendEvent(vpEvent->getPointer());
            const DrawingViewportContent* vpContent(vpEvent->getDrawingViewportContent());
            if (vpContent) {
                viewportHeight = vpContent->getGraphicsViewport().getHeight();
            }
        }
        else if (windowSpaceFlag) {
            auto vpEvent(EventDrawingViewportContentGet::newInstanceGetContentType(m_brainOpenGLFixedPipeline->m_windowIndex,
                                                                                   windowXY,
                                                                                   DrawingViewportContentTypeEnum::WINDOW_AFTER_ASPECT_LOCK));
            EventManager::get()->sendEvent(vpEvent->getPointer());
            const DrawingViewportContent* vpContent(vpEvent->getDrawingViewportContent());
            if (vpContent) {
                viewportHeight = vpContent->getGraphicsViewport().getHeight();
            }
        }
        
        if (viewportHeight > 0) {
            const float lineWidthPixels(viewportHeight
                                        * (lineWidthPercentage / 100.0));
            primitive->setLineWidth(GraphicsPrimitive::LineWidthType::PIXELS,
                                    lineWidthPixels);
        }
        else {
            CaretLogSevere("Unable to get viewport height for annotation "
                           + annotation->toString()
                           + " at window XY: "
                           + windowXY.toString());
            CaretAssert(0);
        }
    }
    else {
        CaretLogSevere("Invalid line width percentage="
                       + AString::number(lineWidthPercentage)
                       + " for annotation: "
                       + annotation->toString());
    }
}

/**
 * Convert the annotation's obsolete line width that was in pixels to a percentage of viewport height.
 * Prior to late July, 2017, a line was specified in pixels.
 * 
 * First, the annotation's percentage width is examined.  If it is valid (greater than zero), then
 * no conversion is needed.  Otherwise, use the viewport height to convert the pixel width to a 
 * percentage and set the annotation's percentage line width.
 * 
 * @param annotation
 *     The annotation.
 */
void
BrainOpenGLAnnotationDrawingFixedPipeline::convertObsoleteLineWidthPixelsToPercentageWidth(const Annotation* annotation) const
{
    annotation->convertObsoleteLineWidthPixelsToPercentageWidth(m_modelSpaceViewport[3]);
}



/**
 * Validate the selected item.
 */
bool
BrainOpenGLAnnotationDrawingFixedPipeline::SelectionInfo::validate()
{
    bool validFlag(false);
    if (m_annotation != NULL) {
        if (m_annotation->getNumberOfCoordinates() != static_cast<int32_t>(m_coordsInWindowXYZ.size())) {
            CaretLogSevere("Selection failed for annotation "
                           + m_annotation->toString()
                           + " with "
                           + AString::number(m_annotation->getNumberOfCoordinates())
                           + " coordinates but only "
                           + AString::number(m_coordsInWindowXYZ.size())
                           + " coordinates have valid window positions.");
        }
        else {
            validFlag = false;
        }
    }
    
    return validFlag;
}
