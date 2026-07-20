
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

#define __BRAIN_OPEN_G_L_NEUROGLANCER_ANNOTATION_DRAWING_DECLARE__
#include "BrainOpenGLNeuroglancerAnnotationDrawing.h"
#undef __BRAIN_OPEN_G_L_NEUROGLANCER_ANNOTATION_DRAWING_DECLARE__

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesNeuroglancerAnnotations.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GroupAndNameHierarchyModel.h"
#include "HistologySlice.h"
#include "IdentificationWithColor.h"
#include "NeuroglancerAnnotation.h"
#include "NeuroglancerAnnotationsFile.h"
#include "Plane.h"
#include "SelectionItemNeuroglancerAnnotation.h"
#include "SelectionManager.h"
#include "VolumeMappableInterface.h"

using namespace caret;



/**
 * \class caret::BrainOpenGLNeuroglancerAnnotationDrawing
 * \brief Draws neuroglancer annotations on brain models
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLNeuroglancerAnnotationDrawing::BrainOpenGLNeuroglancerAnnotationDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLNeuroglancerAnnotationDrawing::~BrainOpenGLNeuroglancerAnnotationDrawing()
{
}

/**
 * Draw neuroglancer annotations on surface
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param surface
 *    Surface on which foci are drawn
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawOnSurface(Brain* brain,
                                                        BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                        const Surface* surface)
{
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    VolumeMappableInterface* invalidUnderlayVolume(NULL);
    const Plane invalidPlane;
    const VolumeSliceViewPlaneEnum::Enum invalidSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    const float invalidlSliceThickness(0.0);
    drawAllNeuroAnn(DrawType::SURFACE,
                    brain,
                    fixedPipelineDrawing,
                    surface,
                    invalidHistologySlicesFile,
                    invalidHistologySlice,
                    invalidUnderlayVolume,
                    invalidPlane,
                    invalidSliceViewPlane,
                    invalidlSliceThickness);
    
}

/**
 * Draw neuroglancer annotations on whole brain
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param underlayVolume
 *    Underlay volume for volume drawing
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawOnWholeBrain(Brain* brain,
                                                           BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                           VolumeMappableInterface* underlayVolume)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    const Plane invalidPlane;
    const VolumeSliceViewPlaneEnum::Enum invalidSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    const float invalidlSliceThickness(0.0);
    drawAllNeuroAnn(DrawType::WHOLE_BRAIN,
                    brain,
                    fixedPipelineDrawing,
                    invalidSurface,
                    invalidHistologySlicesFile,
                    invalidHistologySlice,
                    underlayVolume,
                    invalidPlane,
                    invalidSliceViewPlane,
                    invalidlSliceThickness);
    
}

/**
 * Draw neuroglancer annotations on MPR volume slices.
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param histologySlicesFile
 *   The histology slices file
 * @param histologySlice
 *    The histology slice
 * @param plane
 *    Plane of the volume slice
 * @param sliceThickness
 *   Thickness of a slice
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawOnHistology(Brain* brain,
                                                          BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                          HistologySlicesFile* histologySlicesFile,
                                                          const HistologySlice* histologySlice,
                                                          const Plane& plane,
                                                          const float sliceThickness)
{
    CaretAssert(histologySlice);
    const Surface* invalidSurface(NULL);
    VolumeMappableInterface* invalidVolumeFile(NULL);
    const VolumeSliceViewPlaneEnum::Enum invalidSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    drawAllNeuroAnn(DrawType::HISTOLOGY,
                    brain,
                    fixedPipelineDrawing,
                    invalidSurface,
                    histologySlicesFile,
                    histologySlice,
                    invalidVolumeFile,
                    plane,
                    invalidSliceViewPlane,
                    sliceThickness);
}

/**
 * Draw neuroglancer annotations on MPR volume slices.
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param underlayVolume
 *    Underlay volume for volume drawing
 * @param plane
 *    Plane of the volume slice
 * @param sliceViewPlane
 *    Slice plane being viewed
 * @param sliceThickness
 *   Thickness of a slice
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawOnVolumeMpr(Brain* brain,
                                                          BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                          VolumeMappableInterface* underlayVolume,
                                                          const Plane& plane,
                                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                          const float sliceThickness)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllNeuroAnn(DrawType::VOLUME_MPR,
                    brain,
                    fixedPipelineDrawing,
                    invalidSurface,
                    invalidHistologySlicesFile,
                    invalidHistologySlice,
                    underlayVolume,
                    plane,
                    sliceViewPlane,
                    sliceThickness);
}


/**
 * Draw neuroglancer annotations on oblique volume slices.
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param underlayVolume
 *    Underlay volume for volume drawing
 * @param plane
 *    Plane of the volume slice
 * @param sliceViewPlane
 *    Slice plane being viewed
 * @param sliceThickness
 *   Thickness of a slice */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawOnVolumeOblique(Brain* brain,
                                                              BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                              VolumeMappableInterface* underlayVolume,
                                                              const Plane& plane,
                                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                              const float sliceThickness)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllNeuroAnn(DrawType::VOLUME_OBLIQUE,
                    brain,
                    fixedPipelineDrawing,
                    invalidSurface,
                    invalidHistologySlicesFile,
                    invalidHistologySlice,
                    underlayVolume,
                    plane,
                    sliceViewPlane,
                    sliceThickness);
}

/**
 * Draw neuroglancer annotations on orthogonal volume slices.
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param underlayVolume
 *    Underlay volume for volume drawing
 * @param plane
 *    Plane of the volume slice
 * @param sliceViewPlane
 *    Slice plane being viewed
 * @param sliceThickness
 *   Thickness of a slice
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawOnVolumeOrthogonal(Brain* brain,
                                                                 BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                                 VolumeMappableInterface* underlayVolume,
                                                                 const Plane& plane,
                                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                 const float sliceThickness)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllNeuroAnn(DrawType::VOLUME_ORTHOGONAL,
                    brain,
                    fixedPipelineDrawing,
                    invalidSurface,
                    invalidHistologySlicesFile,
                    invalidHistologySlice,
                    underlayVolume,
                    plane,
                    sliceViewPlane,
                    sliceThickness);
}

/**
 * Draw neuroglancer annotations
 * @param drawType
 *    Type of model for drawing neuroglancer annotations
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param histologySlicesFile
 *    The histology slices file
 * @param histologySlice
 *    Histology slice
 * @param underlayVolume
 *    Underlay volume for volume drawing
 * @param plane
 *    Plane of the volume slice
 * @param sliceViewPlane
 *    Slice plane being viewed
 * @param sliceThickness
 *   Thickness of a slice
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawAllNeuroAnn(const DrawType drawType,
                                                          Brain* brain,
                                                          BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                          const Surface* surface,
                                                          HistologySlicesFile* histologySlicesFile,
                                                          const HistologySlice* histologySlice,
                                                          VolumeMappableInterface* underlayVolume,
                                                          const Plane& plane,
                                                          const VolumeSliceViewPlaneEnum::Enum /*sliceViewPlane*/,
                                                          const float sliceThickness)
{
    fixedPipelineDrawing->checkForOpenGLError(NULL, "At beginning BrainOpenGLNeuroglancerAnnotationDrawing::drawAllNeuroAnn())");
    
    const std::vector<NeuroglancerAnnotationsFile*> allNeuroAnnFiles(brain->getAllNeuroglancerAnnotationFiles());
    const int32_t numberOfNeuroAnnFiles(allNeuroAnnFiles.size());
    if (numberOfNeuroAnnFiles <= 0) {
        return;
    }
    
    SelectionItemNeuroglancerAnnotation* selectNeuroAnn = brain->getSelectionManager()->getNeuroglancerAnnotationIdentification();
    
    /*
     * Check for a 'selection' type mode
     */
    bool selectFlag = false;
    switch (fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
        {
            SelectionItem* selectionItem(NULL);
            switch (drawType) {
                case DrawType::HISTOLOGY:
                case DrawType::SURFACE:
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                case DrawType::WHOLE_BRAIN:
                    CaretAssert(selectNeuroAnn);
                    selectionItem = selectNeuroAnn;
                    break;
            }
            CaretAssert(selectionItem);
            if (selectionItem->isEnabledForSelection()) {
                selectFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            
        }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    const float halfSliceThickness = sliceThickness * 0.5;
    
    const DisplayPropertiesNeuroglancerAnnotations* neuroAnnDisplayProperties(brain->getDisplayPropertiesNeuroglancerAnnotations());
    const DisplayGroupEnum::Enum displayGroup = neuroAnnDisplayProperties->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex);
    
    if ( ! neuroAnnDisplayProperties->isDisplayed(displayGroup,
                                               fixedPipelineDrawing->windowTabIndex)) {
        return;
    }
    
    const float symbolScale(neuroAnnDisplayProperties->getSymbolScale());
    
    switch (drawType) {
        case DrawType::HISTOLOGY:
            break;
        case DrawType::SURFACE:
            break;
        case DrawType::VOLUME_MPR:
            break;
        case DrawType::VOLUME_ORTHOGONAL:
            break;
        case DrawType::VOLUME_OBLIQUE:
            break;
        case DrawType::WHOLE_BRAIN:
            break;
    }
    
    /*
     * Process each neuroglancer annotation file
     */
    for (int32_t iFile = 0; iFile < numberOfNeuroAnnFiles; iFile++) {
        CaretAssertVectorIndex(allNeuroAnnFiles, iFile);
        const NeuroglancerAnnotationsFile* neuroAnnFile(allNeuroAnnFiles[iFile]);
        const int32_t numAnn = neuroAnnFile->getNumberOfAnnotations();
        
        for (int32_t jAnn = 0; jAnn < numAnn; jAnn++) {
            const NeuroglancerAnnotation* neuroAnn(neuroAnnFile->getAnnotation(jAnn));
            
            bool supportedTypeFlag(false);
            switch (neuroAnn->getType()) {
                case NeuroglancerAnnotationTypeEnum::INVALID:
                    break;
                case NeuroglancerAnnotationTypeEnum::AXIS_ALIGNED_BOUNDING_BOX:
                    break;
                case NeuroglancerAnnotationTypeEnum::ELLIPSOID:
                    break;
                case NeuroglancerAnnotationTypeEnum::LINE:
                    break;
                case NeuroglancerAnnotationTypeEnum::POINT:
                    supportedTypeFlag = true;
                    break;
                case NeuroglancerAnnotationTypeEnum::POLYLINE:
                    break;
            }
            if ( ! supportedTypeFlag) {
                CaretAssert(0);
                CaretLogSevere("Neuroglancer Annotation of type "
                               + neuroAnn->getTypeName()
                               + " not supported for drawing.");
                continue;
            }
            
            const CaretColor& color(neuroAnn->getColor());
            std::array<uint8_t, 4> rgba(color.getRGBA());
            
            CaretAssert(neuroAnn->getNumberOfXYZ() > 0);
            Vector3D xyz(neuroAnn->getXYZ(0));
            
            bool drawNeuroAnnFlag = false;
            switch (drawType) {
                case DrawType::HISTOLOGY:
                {
                    CaretAssert(histologySlice);
                    Vector3D xyzOnSlice;
                    /*
                     * Need to convert stereotaxic to 'histology plane' XYZ
                     */
                    Vector3D stereotaxicOnSliceXYZ;
                    Vector3D planeOnSliceXYZ;
                    float distanceToSlice;
                    if (histologySlice->projectStereotaxicXyzToSlice(xyz,
                                                                     stereotaxicOnSliceXYZ,
                                                                     distanceToSlice,
                                                                     planeOnSliceXYZ)) {
                        float distanceToHistologySliceTolerance = halfSliceThickness;
                        if (distanceToSlice < distanceToHistologySliceTolerance) {
                            xyz[0] = planeOnSliceXYZ[0];
                            xyz[1] = planeOnSliceXYZ[1];
                            xyz[2] = planeOnSliceXYZ[2];
                            drawNeuroAnnFlag = true;
                        }
                    }
                }
                    break;
                case DrawType::SURFACE:
                    drawNeuroAnnFlag = true;
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                {
                    const bool testFlag(false);
                    if (testFlag) {
                        CaretAssert(underlayVolume);
                        const Vector3D spaceXYZ(underlayVolume->indexToSpace(VoxelIJK(xyz[0], xyz[1], xyz[2])));
                        const Vector3D xyzOnPlane(plane.projectPointToPlane(spaceXYZ));
                        xyz = xyzOnPlane;
                        drawNeuroAnnFlag = true;
                    }
                    else {
                        const Vector3D spaceXYZ(underlayVolume->indexToSpace(VoxelIJK(xyz[0], xyz[1], xyz[2])));
                        if (plane.absoluteDistanceToPlane(spaceXYZ) < halfSliceThickness) {
                            const Vector3D xyzOnPlane(plane.projectPointToPlane(spaceXYZ));
                            xyz = xyzOnPlane;
                            drawNeuroAnnFlag = true;
                        }
                    }
                }
                    break;
                case DrawType::WHOLE_BRAIN:
                if (underlayVolume != NULL) {
                    const Vector3D spaceXYZ(underlayVolume->indexToSpace(VoxelIJK(xyz[0], xyz[1], xyz[2])));
                    xyz = spaceXYZ;
                    drawNeuroAnnFlag = true;
                }
                    break;
            }
            
            if (drawNeuroAnnFlag) {
                glPushMatrix();
                if (selectFlag) {
                    fixedPipelineDrawing->colorIdentification->addItem(rgba.data(),
                                                                       SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                                       iFile, /* file index */
                                                                       jAnn,  /* Ann index*/
                                                                       0); /* projection index */
                    rgba[3] = 255;
                }
                /*
                 * Need to draw each symbol independently since each symbol
                 * contains a unique size (diameter)
                 */
                std::unique_ptr<GraphicsPrimitiveV3fC4ub> idPrimitive;
                idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::SPHERES));
                idPrimitive->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS,
                                               (neuroAnn->getSize() * symbolScale));
                idPrimitive->addVertex(xyz,
                                       rgba.data());
                GraphicsEngineDataOpenGL::draw(idPrimitive.get());
                glPopMatrix();
            }
        }
    }
    
    if (selectFlag) {
        int32_t neuroAnnFileIndex = -1;
        int32_t neuroAnnIndex = -1;
        int32_t neuroAnnProjectionIndex = -1;
        float depth = -1.0;
        fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                         fixedPipelineDrawing->mouseX,
                                                         fixedPipelineDrawing->mouseY,
                                                         neuroAnnFileIndex,
                                                         neuroAnnIndex,
                                                         neuroAnnProjectionIndex,
                                                         depth);
        if (neuroAnnFileIndex >= 0) {
            switch (drawType) {
                case DrawType::HISTOLOGY:
                    if (selectNeuroAnn->isOtherScreenDepthCloserToViewer(depth)) {
                        NeuroglancerAnnotation* neuroAnn(brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex)->getAnnotation(neuroAnnIndex));
                        CaretAssert(neuroAnn);
                        selectNeuroAnn->setBrain(brain);
                        selectNeuroAnn->setHistologySelection(histologySlicesFile,
                                                              brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex),
                                                              neuroAnn,
                                                              neuroAnnIndex);
                        selectNeuroAnn->setScreenDepth(depth);
                        const Vector3D& xyz(neuroAnn->getXYZ(0));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectNeuroAnn, xyz);
                        CaretLogFine("Selected Histology Neuro Ann Identification Symbol: " + QString::number(neuroAnnIndex));
                    }
                    break;
                case DrawType::SURFACE:
                    if (selectNeuroAnn->isOtherScreenDepthCloserToViewer(depth)) {
                        NeuroglancerAnnotation* neuroAnn(brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex)->getAnnotation(neuroAnnIndex));
                        CaretAssert(neuroAnn);
                        selectNeuroAnn->setBrain(brain);
                        selectNeuroAnn->setSurfaceSelection(surface,
                                                            brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex),
                                                            neuroAnn,
                                                            neuroAnnIndex);
                        selectNeuroAnn->setScreenDepth(depth);
                        const Vector3D& xyz(neuroAnn->getXYZ(0));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectNeuroAnn, xyz);
                        CaretLogFine("Selected Surface Neuro Ann Identification Symbol: " + QString::number(neuroAnnIndex));
                    }
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                    CaretAssert(selectNeuroAnn);
                    if (selectNeuroAnn->isOtherScreenDepthCloserToViewer(depth)) {
                        NeuroglancerAnnotation* neuroAnn(brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex)->getAnnotation(neuroAnnIndex));
                        CaretAssert(neuroAnn);
                        selectNeuroAnn->setBrain(brain);
                        selectNeuroAnn->setVolumeSelection(underlayVolume,
                                                           brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex),
                                                           neuroAnn,
                                                           neuroAnnIndex);
                        selectNeuroAnn->setScreenDepth(depth);
                        const Vector3D& xyz(neuroAnn->getXYZ(0));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectNeuroAnn, xyz);
                        CaretLogFine("Selected Volume Neuro Ann Identification Symbol: " + QString::number(neuroAnnIndex));
                    }
                    break;
                case DrawType::WHOLE_BRAIN:
                    if (selectNeuroAnn->isOtherScreenDepthCloserToViewer(depth)) {
                        NeuroglancerAnnotation* neuroAnn(brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex)->getAnnotation(neuroAnnIndex));
                        CaretAssert(neuroAnn);
                        selectNeuroAnn->setBrain(brain);
                        selectNeuroAnn->setWholeBrainSelection(brain->getNeuroglancerAnnotationsFile(neuroAnnFileIndex),
                                                               neuroAnn,
                                                               neuroAnnIndex);
                        selectNeuroAnn->setScreenDepth(depth);
                        const Vector3D& xyz(neuroAnn->getXYZ(0));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectNeuroAnn, xyz);
                        CaretLogFine("Selected Surface Neuro Ann Identification Symbol: " + QString::number(neuroAnnIndex));
                    }
                    break;
            }
        }
    }
    
    fixedPipelineDrawing->checkForOpenGLError(NULL, "At end BrainOpenGLNeuroglancerAnnotationDrawing::drawAllNeuroAnn())");
}

/**
 * Draw a one millimeter square facing the user.
 * NOTE: This method will alter the current
 * modelviewing matrices so caller may need
 * to enclose the call to this method within
 * glPushMatrix() and glPopMatrix().
 *
 * @param size
 *     Size of square.
 */
void
BrainOpenGLNeuroglancerAnnotationDrawing::drawSquare(const float size)
{
    const float length = size * 0.5;
    
    /*
     * Draw both front and back side since in some instances,
     * such as surface montage, we are viweing from the far
     * side (from back of monitor)
     */
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f( length, -length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-length, -length, 0.0);
    glVertex3f(-length,  length, 0.0);
    glVertex3f( length,  length, 0.0);
    glVertex3f( length, -length, 0.0);
    glEnd();
}

