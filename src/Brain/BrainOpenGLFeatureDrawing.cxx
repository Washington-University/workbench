
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

#define __BRAIN_OPEN_G_L_FEATURE_DRAWING_DECLARE__
#include "BrainOpenGLFeatureDrawing.h"
#undef __BRAIN_OPEN_G_L_FEATURE_DRAWING_DECLARE__

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesFeature.h"
#include "FeatureFile.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GroupAndNameHierarchyModel.h"
#include "HistologySlice.h"
#include "IdentificationWithColor.h"
#include "FeatureItem.h"
#include "FeatureItemGroup.h"
#include "FeatureItemModel.h"
#include "Plane.h"
#include "SelectionItemFeature.h"
#include "SelectionManager.h"
#include "VolumeMappableInterface.h"

using namespace caret;



/**
 * \class caret::BrainOpenGLFeatureDrawing
 * \brief Draws features on brain models
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLFeatureDrawing::BrainOpenGLFeatureDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLFeatureDrawing::~BrainOpenGLFeatureDrawing()
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
BrainOpenGLFeatureDrawing::drawOnSurface(Brain* brain,
                                                        BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                        const Surface* surface)
{
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    VolumeMappableInterface* invalidUnderlayVolume(NULL);
    const Plane invalidPlane;
    const VolumeSliceViewPlaneEnum::Enum invalidSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    const float invalidlSliceThickness(0.0);
    drawAllFeatures(DrawType::SURFACE,
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
BrainOpenGLFeatureDrawing::drawOnWholeBrain(Brain* brain,
                                                           BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                           VolumeMappableInterface* underlayVolume)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    const Plane invalidPlane;
    const VolumeSliceViewPlaneEnum::Enum invalidSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    const float invalidlSliceThickness(0.0);
    drawAllFeatures(DrawType::WHOLE_BRAIN,
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
BrainOpenGLFeatureDrawing::drawOnHistology(Brain* brain,
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
    drawAllFeatures(DrawType::HISTOLOGY,
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
BrainOpenGLFeatureDrawing::drawOnVolumeMpr(Brain* brain,
                                                          BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                          VolumeMappableInterface* underlayVolume,
                                                          const Plane& plane,
                                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                          const float sliceThickness)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllFeatures(DrawType::VOLUME_MPR,
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
BrainOpenGLFeatureDrawing::drawOnVolumeOblique(Brain* brain,
                                                              BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                              VolumeMappableInterface* underlayVolume,
                                                              const Plane& plane,
                                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                              const float sliceThickness)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllFeatures(DrawType::VOLUME_OBLIQUE,
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
BrainOpenGLFeatureDrawing::drawOnVolumeOrthogonal(Brain* brain,
                                                                 BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                                 VolumeMappableInterface* underlayVolume,
                                                                 const Plane& plane,
                                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                                 const float sliceThickness)
{
    const Surface* invalidSurface(NULL);
    HistologySlicesFile* invalidHistologySlicesFile(NULL);
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllFeatures(DrawType::VOLUME_ORTHOGONAL,
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
BrainOpenGLFeatureDrawing::drawAllFeatures(const DrawType drawType,
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
    fixedPipelineDrawing->checkForOpenGLError(NULL, "At beginning BrainOpenGLFeatureDrawing::drawAllFeatures())");
    
    const std::vector<FeatureFile*> allFeatureFiles(brain->getAllFeatureFiles());
    const int32_t numberOfFeatureFiles(allFeatureFiles.size());
    if (numberOfFeatureFiles <= 0) {
        return;
    }
    
    SelectionItemFeature* selectionItemFeature = brain->getSelectionManager()->getFeatureIdentification();
    
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
                    CaretAssert(selectionItemFeature);
                    selectionItem = selectionItemFeature;
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
    
    const DisplayPropertiesFeature* featureDisplayProperties(brain->getDisplayPropertiesFeature());
    const DisplayGroupEnum::Enum displayGroup = featureDisplayProperties->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex);
    const float distanceFromVolumeSliceTolerance(featureDisplayProperties->getDistanceFromVolumeSliceTolerance());
    
    if ( ! featureDisplayProperties->isDisplayed(displayGroup,
                                               fixedPipelineDrawing->windowTabIndex)) {
        return;
    }
    
    const float symbolScale(featureDisplayProperties->getSymbolScale());
    
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
     * Process each feature file
     */
    int64_t numDrawn(0);
    for (int32_t iFile = 0; iFile < numberOfFeatureFiles; iFile++) {
        CaretAssertVectorIndex(allFeatureFiles, iFile);
        const FeatureFile* featureFile(allFeatureFiles[iFile]);
        const FeatureItemModel* featureItemModel(featureFile->getFeatureItemModel());
        const std::vector<const FeatureItemGroup*> allFeatureGroups(featureItemModel->getAllFeatureGroups());
        const int32_t numGroups(allFeatureGroups.size());
        for (int32_t jGroupIndex = 0; jGroupIndex < numGroups; jGroupIndex++) {
            CaretAssertVectorIndex(allFeatureGroups, jGroupIndex);
            const FeatureItemGroup* featureGroup(allFeatureGroups[jGroupIndex]);
            
            const std::vector<const FeatureItem*> allFeatures(featureGroup->getAllFeatures());
            
            for (const FeatureItem* featureItem : allFeatures) {
                CaretAssert(featureItem);
                if ( ! featureItem->isDisplayed()) {
                    continue;
                }
                
                bool supportedTypeFlag(false);
                switch (featureItem->getType()) {
                    case FeatureItemTypeEnum::INVALID:
                        break;
                    case FeatureItemTypeEnum::POINT:
                        supportedTypeFlag = true;
                        break;
                }
                if ( ! supportedTypeFlag) {
                    CaretAssert(0);
                    CaretLogSevere("Feature of type "
                                   + featureItem->getTypeName()
                                   + " not supported for drawing.");
                    continue;
                }
                
                const QColor& color(featureItem->getColor());
                std::array<uint8_t, 4> rgba {
                    static_cast<uint8_t>(color.red()),
                    static_cast<uint8_t>(color.green()),
                    static_cast<uint8_t>(color.blue()),
                    static_cast<uint8_t>(color.alpha())
                };
                
                CaretAssert(featureItem->getNumberOfIJK() > 0);
                const int32_t coordIndex(0);
                Vector3D xyz(featureFile->featureIJKtoXYZ(featureItem, coordIndex));
                
                bool drawFeatureFlag = false;
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
                                drawFeatureFlag = true;
                            }
                        }
                    }
                        break;
                    case DrawType::SURFACE:
                        drawFeatureFlag = true;
                        break;
                    case DrawType::VOLUME_MPR:
                    case DrawType::VOLUME_OBLIQUE:
                    case DrawType::VOLUME_ORTHOGONAL:
                    {
                        const bool testFlag(false);
                        if (testFlag) {
                            CaretAssert(underlayVolume);
                            xyz = plane.projectPointToPlane(xyz);
                            drawFeatureFlag = true;
                        }
                        else {
                            if (plane.absoluteDistanceToPlane(xyz) < distanceFromVolumeSliceTolerance) {
                                xyz = plane.projectPointToPlane(xyz);
                                drawFeatureFlag = true;
                            }
                        }
                    }
                        break;
                    case DrawType::WHOLE_BRAIN:
                        if (underlayVolume != NULL) {
                            drawFeatureFlag = true;
                        }
                        break;
                }
                
                if (drawFeatureFlag) {
                    ++numDrawn;
                    
                    glPushMatrix();
                    if (selectFlag) {
                        fixedPipelineDrawing->colorIdentification->addItem(rgba.data(),
                                                                           SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                                           iFile, /* file index */
                                                                           featureItem->getGroupID(),
                                                                           featureItem->getUniqueID());
                        rgba[3] = 255;
                    }
                    /*
                     * Need to draw each symbol independently since each symbol
                     * contains a unique size (diameter)
                     */
                    const bool drawWithPointsFlag(true);
                    if (drawWithPointsFlag) {
                        glPointSize(symbolScale);
                        glBegin(GL_POINTS);
                        glColor4ubv(rgba.data());
                        glVertex3fv(xyz);
                        glEnd();
                    }
                    else {
                        std::unique_ptr<GraphicsPrimitiveV3fC4ub> idPrimitive;
                        idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::SPHERES));
                        idPrimitive->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS,
                                                       (featureItem->getSymbolSize() * symbolScale));
                        idPrimitive->addVertex(xyz,
                                               rgba.data());
                        GraphicsEngineDataOpenGL::draw(idPrimitive.get());
                    }
                    glPopMatrix();
                }
            }
        }
    }
    
    if (selectFlag) {
        int32_t featureFileIndex = -1;
        int32_t featureGroupID(-1);
        int32_t featureUniqueID= -1;
        float depth = -1.0;
        fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                         fixedPipelineDrawing->mouseX,
                                                         fixedPipelineDrawing->mouseY,
                                                         featureFileIndex,
                                                         featureGroupID,
                                                         featureUniqueID,
                                                         depth);
        if (featureFileIndex >= 0) {
            FeatureFile* featureFile(brain->getFeatureFile(featureFileIndex));
            CaretAssert(featureFile);
            FeatureItemModel* featureItemModel(featureFile->getFeatureItemModel());
            CaretAssert(featureItemModel);
            FeatureItem* featureItem(featureItemModel->getFeatureWithGroupAndUniqueID(featureGroupID,
                                                                                      featureUniqueID));
            CaretAssert(featureItem);
            switch (drawType) {
                case DrawType::HISTOLOGY:
                    if (selectionItemFeature->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemFeature->setBrain(brain);
                        selectionItemFeature->setHistologySelection(histologySlicesFile,
                                                              featureFile,
                                                              featureItem,
                                                                    featureGroupID,
                                                                    featureUniqueID);
                        selectionItemFeature->setScreenDepth(depth);
                        const int32_t coordIndex(0);
                        const Vector3D xyz(featureFile->featureIJKtoXYZ(featureItem,
                                                                            coordIndex));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemFeature, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(featureGroupID)
                                     + " UniqueID="
                                     + QString::number(featureUniqueID));
                    }
                    break;
                case DrawType::SURFACE:
                    if (selectionItemFeature->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemFeature->setBrain(brain);
                        selectionItemFeature->setSurfaceSelection(surface,
                                                            featureFile,
                                                            featureItem,
                                                                  featureGroupID,
                                                                  featureUniqueID);
                        selectionItemFeature->setScreenDepth(depth);
                        const int32_t coordIndex(0);
                        const Vector3D xyz(featureFile->featureIJKtoXYZ(featureItem,
                                                                            coordIndex));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemFeature, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(featureGroupID)
                                     + " UniqueID="
                                     + QString::number(featureUniqueID));
                    }
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                    CaretAssert(selectionItemFeature);
                    if (selectionItemFeature->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemFeature->setBrain(brain);
                        selectionItemFeature->setVolumeSelection(underlayVolume,
                                                           featureFile,
                                                                 featureItem,
                                                                 featureGroupID,
                                                                 featureUniqueID);
                        selectionItemFeature->setScreenDepth(depth);
                        const int32_t coordIndex(0);
                        const Vector3D xyz(featureFile->featureIJKtoXYZ(featureItem,
                                                                            coordIndex));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemFeature, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(featureGroupID)
                                     + " UniqueID="
                                     + QString::number(featureUniqueID));
                    }
                    break;
                case DrawType::WHOLE_BRAIN:
                    if (selectionItemFeature->isOtherScreenDepthCloserToViewer(depth)) {
                        selectionItemFeature->setBrain(brain);
                        selectionItemFeature->setWholeBrainSelection(featureFile,
                                                               featureItem,
                                                                     featureGroupID,
                                                                     featureUniqueID);
                        selectionItemFeature->setScreenDepth(depth);
                        const int32_t coordIndex(0);
                        const Vector3D xyz(featureFile->featureIJKtoXYZ(featureItem,
                                                                            coordIndex));
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectionItemFeature, xyz);
                        CaretLogFine("Selected Histology Feature Identification Symbol GroupID="
                                     + QString::number(featureGroupID)
                                     + " UniqueID="
                                     + QString::number(featureUniqueID));
                    }
                    break;
            }
        }
    }
    
    fixedPipelineDrawing->checkForOpenGLError(NULL, "At end BrainOpenGLFeatureDrawing::drawAllFeatures())");
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
BrainOpenGLFeatureDrawing::drawSquare(const float size)
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

