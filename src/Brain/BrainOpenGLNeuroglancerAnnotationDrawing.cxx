
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
#include "DeveloperFlagsEnum.h"
#include "DisplayPropertiesNeuroglancerAnnotation.h"
#include "ElapsedTimer.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GraphicsShapeOptimizedDrawing.h"
#include "GroupAndNameHierarchyModel.h"
#include "HistologySlice.h"
#include "IdentificationWithColor.h"
#include "NeuroglancerAnnotation.h"
#include "NeuroglancerAnnotationFile.h"
#include "NeuroglancerAnnotationFileImporter.h"
#include "Plane.h"
#include "SelectionItemNeuroglancerAnnotation.h"
#include "SelectionManager.h"
#include "SystemUtilities.h"
#include "VolumeMappableInterface.h"

using namespace caret;



/**
 * \class caret::BrainOpenGLNeuroglancerAnnotationDrawing
 * \brief Draws features on brain models
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
 * Draw features on surface
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
    drawAllAnnotations(DrawType::SURFACE,
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
 * Draw features on whole brain
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
    drawAllAnnotations(DrawType::WHOLE_BRAIN,
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
 * Draw features on MPR volume slices.
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
    drawAllAnnotations(DrawType::HISTOLOGY,
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
 * Draw features on MPR volume slices.
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
    drawAllAnnotations(DrawType::VOLUME_MPR,
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
 * Draw features on oblique volume slices.
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
    drawAllAnnotations(DrawType::VOLUME_OBLIQUE,
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
 * Draw features on orthogonal volume slices.
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
    drawAllAnnotations(DrawType::VOLUME_ORTHOGONAL,
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
 * Draw features
 * @param drawType
 *    Type of model for drawing features
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
BrainOpenGLNeuroglancerAnnotationDrawing::drawAllAnnotations(const DrawType drawType,
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
    fixedPipelineDrawing->checkForOpenGLError(NULL, "At beginning BrainOpenGLNeuroglancerAnnotationDrawing::drawAllFeatures())");
    
    const std::vector<NeuroglancerAnnotationFile*> allNeuroAnnFiles(brain->getAllNeuroglancerAnnotationFiles());
    const int32_t numberOfNeuroAnnFiles(allNeuroAnnFiles.size());
    if (numberOfNeuroAnnFiles <= 0) {
        return;
    }
    
    SelectionItemNeuroglancerAnnotation* selectionItemNeuroglancerAnnotation(brain->getSelectionManager()->getNeuroglancerAnnotationIdentification());
    CaretAssert(selectionItemNeuroglancerAnnotation);
    
    /*
     * Check for a 'selection' type mode
     */
    bool selectFlag = false;
    switch (fixedPipelineDrawing->mode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
        {
            if (selectionItemNeuroglancerAnnotation->isEnabledForSelection()) {
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
    
    const DisplayPropertiesNeuroglancerAnnotation* displayProperties(brain->getDisplayPropertiesNeuroglancerAnnotation());
    const DisplayGroupEnum::Enum displayGroup(displayProperties->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex));
    const float distanceFromVolumeSliceTolerance(displayProperties->getDistanceFromVolumeSliceTolerance());
    if ( ! displayProperties->isDisplayed(displayGroup,
                                               fixedPipelineDrawing->windowTabIndex)) {
        return;
    }
    
    const float symbolScale(displayProperties->getSymbolScale());
    
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
    
    const std::set<uint64_t> uniqueIdsForDebugging(NeuroglancerAnnotationFileImporter::getAnnotionsUniqueIDsForDebugging());

    /*
     * Process each feature file
     */
    for (int32_t iFile = 0; iFile < numberOfNeuroAnnFiles; iFile++) {
        CaretAssertVectorIndex(allNeuroAnnFiles, iFile);
        const NeuroglancerAnnotationFile* neuroAnnFile(allNeuroAnnFiles[iFile]);
        CaretAssert(neuroAnnFile);
        
        const bool nXpZpYOrientationFlag(neuroAnnFile->isLeftNegxPosteriorPoszInferiorPosYOrientation());
        
        const int32_t numAnn(neuroAnnFile->getNumberOfAnnotations());
        for (int32_t iAnn = 0; iAnn < numAnn; iAnn++) {
            const NeuroglancerAnnotation* neuroAnn(neuroAnnFile->getAnnotation(iAnn));
            CaretAssert(neuroAnn);
            if ( ! neuroAnn->isDisplayed()) {
                continue;
            }
            
            if (DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DEVELOPER_FLAG_NEUROGLANCER_ANNOTATIONS)) {
                const uint64_t id(neuroAnn->getUniqueID());
                if (uniqueIdsForDebugging.find(id) == uniqueIdsForDebugging.end()) {
                    continue;
                }
            }
            
            const uint8_t* rgbaAnn(neuroAnn->getRgba());
            uint8_t rgba[4] {
                rgbaAnn[0],
                rgbaAnn[1],
                rgbaAnn[2],
                rgbaAnn[3]
            };
            
            const Vector3D neuroAnnXYZ(neuroAnn->getXYZ());
            Vector3D xyz(neuroAnnXYZ);
            if (nXpZpYOrientationFlag) {
                /*
                 * Orientation: Left is -X;
                 *              Anterior is +Z
                 *              Inferior is +Y
                 */
                xyz[1] = -neuroAnnXYZ[2];
                xyz[2] = -neuroAnnXYZ[1];
            }
            
            bool drawFlag = false;
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
                        const float distanceToHistologySliceTolerance = halfSliceThickness;
                        if (distanceToSlice < distanceToHistologySliceTolerance) {
                            xyz[0] = planeOnSliceXYZ[0];
                            xyz[1] = planeOnSliceXYZ[1];
                            xyz[2] = planeOnSliceXYZ[2];
                            drawFlag = true;
                        }
                    }
                }
                    break;
                case DrawType::SURFACE:
                    drawFlag = true;
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                {
                    const bool testFlag(false);
                    if (testFlag) {
                        CaretAssert(underlayVolume);
                        xyz = plane.projectPointToPlane(xyz);
                        drawFlag = true;
                    }
                    else {
                        if (plane.absoluteDistanceToPlane(xyz) < distanceFromVolumeSliceTolerance) {
                            xyz = plane.projectPointToPlane(xyz);
                            drawFlag = true;
                        }
                    }
                }
                    break;
                case DrawType::WHOLE_BRAIN:
                    if (underlayVolume != NULL) {
                        drawFlag = true;
                    }
                    break;
            }
            
            if (drawFlag) {
                if (selectFlag) {
                    fixedPipelineDrawing->colorIdentification->addItem(rgba,
                                                                       SelectionItemDataTypeEnum::NEUROGLANCER_ANNOTATION,
                                                                       iFile, /* file index */
                                                                       iAnn);  /* ann index*/
                    rgba[3] = 255;
                }
                
                glPushAttrib(GL_LIGHTING_BIT);
                
                /*
                 * Need to draw each symbol independently since each symbol
                 * contains a unique size (diameter)
                 */
                const bool drawWithPointsFlag(true);
                if (drawWithPointsFlag) {
                    glDisable(GL_LIGHTING);
                    glPointSize(symbolScale);
                    glBegin(GL_POINTS);
                    glColor4ubv(rgba);
                    glVertex3fv(xyz);
                    glEnd();
                }
                else {
                    GraphicsShapeOptimizedDrawing::drawOctahedron(xyz,
                                                                  symbolScale,
                                                                  rgba);
                }
                
                glPopAttrib();
            }
        }
    }

    if (selectFlag) {
        int32_t fileIndex(-1);
        int32_t annIndex(-1);
        float depth = -1.0;
        fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::NEUROGLANCER_ANNOTATION,
                                                         fixedPipelineDrawing->mouseX,
                                                         fixedPipelineDrawing->mouseY,
                                                         fileIndex,
                                                         annIndex,
                                                         depth);
        if (fileIndex >= 0) {
            NeuroglancerAnnotationFile* neuroAnnFile(brain->getNeuroglancerAnnotationFile(fileIndex));
            CaretAssert(neuroAnnFile);
            const NeuroglancerAnnotation* neuroAnn(neuroAnnFile->getAnnotation(annIndex));
            CaretAssert(neuroAnn);
            const uint64_t groupID(0);
            const uint64_t uniqueID(neuroAnn->getUniqueID());
            const Vector3D xyz(neuroAnn->getXYZ());
            switch (drawType) {
                case DrawType::HISTOLOGY:
                    if (selectionItemNeuroglancerAnnotation->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemNeuroglancerAnnotation->setBrain(brain);
                        selectionItemNeuroglancerAnnotation->setHistologySelection(histologySlicesFile,
                                                              neuroAnnFile,
                                                              neuroAnn);
                        selectionItemNeuroglancerAnnotation->setScreenDepth(depth);
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemNeuroglancerAnnotation, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(groupID)
                                     + " UniqueID="
                                     + QString::number(uniqueID));
                    }
                    break;
                case DrawType::SURFACE:
                    if (selectionItemNeuroglancerAnnotation->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemNeuroglancerAnnotation->setBrain(brain);
                        selectionItemNeuroglancerAnnotation->setSurfaceSelection(surface,
                                                                                 neuroAnnFile,
                                                                                 neuroAnn);
                        selectionItemNeuroglancerAnnotation->setScreenDepth(depth);
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemNeuroglancerAnnotation, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(groupID)
                                     + " UniqueID="
                                     + QString::number(uniqueID));
                    }
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                    CaretAssert(selectionItemNeuroglancerAnnotation);
                    if (selectionItemNeuroglancerAnnotation->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemNeuroglancerAnnotation->setBrain(brain);
                        selectionItemNeuroglancerAnnotation->setVolumeSelection(underlayVolume,
                                                                                neuroAnnFile,
                                                                                neuroAnn);
                        selectionItemNeuroglancerAnnotation->setScreenDepth(depth);
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemNeuroglancerAnnotation, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(groupID)
                                     + " UniqueID="
                                     + QString::number(uniqueID));
                    }
                    break;
                case DrawType::WHOLE_BRAIN:
                    if (selectionItemNeuroglancerAnnotation->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemNeuroglancerAnnotation->setBrain(brain);
                        selectionItemNeuroglancerAnnotation->setWholeBrainSelection(neuroAnnFile,
                                                                                    neuroAnn);
                        selectionItemNeuroglancerAnnotation->setScreenDepth(depth);
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemNeuroglancerAnnotation, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(groupID)
                                     + " UniqueID="
                                     + QString::number(uniqueID));
                    }
                    break;
            }
        }
    }
    
    fixedPipelineDrawing->checkForOpenGLError(NULL, "At end BrainOpenGLNeuroglancerAnnotationDrawing::drawAllFeatures())");
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

