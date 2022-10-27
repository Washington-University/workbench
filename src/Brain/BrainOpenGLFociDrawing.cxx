
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

#define __BRAIN_OPEN_G_L_FOCI_DRAWING_DECLARE__
#include "BrainOpenGLFociDrawing.h"
#undef __BRAIN_OPEN_G_L_FOCI_DRAWING_DECLARE__

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DisplayPropertiesFoci.h"
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "GroupAndNameHierarchyModel.h"
#include "HistologySlice.h"
#include "IdentificationWithColor.h"
#include "Plane.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionManager.h"
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLFociDrawing 
 * \brief Draws foci on brain models
 * \ingroup Brain
 */

/**
 * Constructor.
 */
BrainOpenGLFociDrawing::BrainOpenGLFociDrawing()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
BrainOpenGLFociDrawing::~BrainOpenGLFociDrawing()
{
}

/**
 * Draw foci on MPR volume slices.
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param histologySlice
 *    The histology slice
 * @param plane
 *    Plane of the volume slice
 * @param sliceThickness
 *   Thickness of a slice
 */
void
BrainOpenGLFociDrawing::drawHistologyFoci(Brain* brain,
                                          BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                          const HistologySlice* histologySlice,
                                          const Plane& plane,
                                          const float sliceThickness)
{
    CaretAssert(histologySlice);
    VolumeMappableInterface* invalidVolumeFile(NULL);
    const VolumeSliceViewPlaneEnum::Enum invalidSliceViewPlane(VolumeSliceViewPlaneEnum::ALL);
    drawAllFoci(DrawType::HISTOLOGY,
                brain,
                fixedPipelineDrawing,
                histologySlice,
                invalidVolumeFile,
                plane,
                invalidSliceViewPlane,
                sliceThickness);
}

/**
 * Draw foci on MPR volume slices.
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
BrainOpenGLFociDrawing::drawVolumeMprFoci(Brain* brain,
                                          BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                          VolumeMappableInterface* underlayVolume,
                                          const Plane& plane,
                                          const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                          const float sliceThickness)
{
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllFoci(DrawType::VOLUME_MPR,
                brain,
                fixedPipelineDrawing,
                invalidHistologySlice,
                underlayVolume,
                plane,
                sliceViewPlane,
                sliceThickness);
}


/**
 * Draw foci on oblique volume slices.
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
BrainOpenGLFociDrawing::drawVolumeObliqueFoci(Brain* brain,
                                              BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                              VolumeMappableInterface* underlayVolume,
                                              const Plane& plane,
                                              const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                              const float sliceThickness)
{
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllFoci(DrawType::VOLUME_OBLIQUE,
                brain,
                fixedPipelineDrawing,
                invalidHistologySlice,
                underlayVolume,
                plane,
                sliceViewPlane,
                sliceThickness);
}

/**
 * Draw foci on orthogonal volume slices.
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
BrainOpenGLFociDrawing::drawVolumeOrthogonalFoci(Brain* brain,
                                                 BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                 VolumeMappableInterface* underlayVolume,
                                                 const Plane& plane,
                                                 const VolumeSliceViewPlaneEnum::Enum sliceViewPlane,
                                                 const float sliceThickness)
{
    const HistologySlice* invalidHistologySlice(NULL);
    drawAllFoci(DrawType::VOLUME_ORTHOGONAL,
                brain,
                fixedPipelineDrawing,
                invalidHistologySlice,
                underlayVolume,
                plane,
                sliceViewPlane,
                sliceThickness);
}

/**
 * Draw foci
 * @param drawType
 *    Type of model for drawing foci
 * @param brain
 *    The brain
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
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
BrainOpenGLFociDrawing::drawAllFoci(const DrawType drawType,
                                    Brain* brain,
                                    BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                    const HistologySlice* histologySlice,
                                    VolumeMappableInterface* underlayVolume,
                                    const Plane& plane,
                                    const VolumeSliceViewPlaneEnum::Enum /*sliceViewPlane*/,
                                    const float sliceThickness)
{
    SelectionItemFocusVolume* selectVolumeFocus = brain->getSelectionManager()->getVolumeFocusIdentification();
    
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
                    selectionItem = selectVolumeFocus;  /* NEED TO REPLACE */
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                    CaretAssert(selectVolumeFocus);
                    selectionItem = selectVolumeFocus;
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
    
    const DisplayPropertiesFoci* fociDisplayProperties = brain->getDisplayPropertiesFoci();
    const DisplayGroupEnum::Enum displayGroup = fociDisplayProperties->getDisplayGroupForTab(fixedPipelineDrawing->windowTabIndex);
    
    const CaretColorEnum::Enum caretColor = fociDisplayProperties->getStandardColorType(displayGroup,
                                                                                        fixedPipelineDrawing->windowTabIndex);
    float caretColorRGBA[4];
    CaretColorEnum::toRGBAFloat(caretColor, caretColorRGBA);
    
    if (fociDisplayProperties->isDisplayed(displayGroup,
                                           fixedPipelineDrawing->windowTabIndex) == false) {
        return;
    }
    float focusDiameter(1.0);
    IdentificationSymbolSizeTypeEnum::Enum fociSizeType(fociDisplayProperties->getFociSymbolSizeType(displayGroup,
                                                                                            fixedPipelineDrawing->windowTabIndex));
    bool useMmFlag(false);
    bool usePctFlag(false);
    switch (fociSizeType) {
        case IdentificationSymbolSizeTypeEnum::MILLIMETERS:
            useMmFlag = true;
            break;
        case IdentificationSymbolSizeTypeEnum::PERCENTAGE:
            usePctFlag = true;
            break;
    }
    if (usePctFlag) {
        if (underlayVolume != NULL) {
            focusDiameter = fociDisplayProperties->getFociSizePercentage(displayGroup,
                                                                         fixedPipelineDrawing->windowTabIndex);
            BoundingBox boundingBox;
            underlayVolume->getVoxelSpaceBoundingBox(boundingBox);
            const float maxDimension = boundingBox.getMaximumDifferenceOfXYZ();
            focusDiameter = maxDimension * (focusDiameter / 100.0);
        }
        else {
            useMmFlag = true;
        }
    }
    if (useMmFlag) {
        focusDiameter = fociDisplayProperties->getFociSizeMillimeters(displayGroup,
                                                                      fixedPipelineDrawing->windowTabIndex);
    }

    const FeatureColoringTypeEnum::Enum fociColoringType = fociDisplayProperties->getColoringType(displayGroup,
                                                                                                  fixedPipelineDrawing->windowTabIndex);
    
    bool drawAsSpheresFlag = false;
    switch (fociDisplayProperties->getDrawingType(displayGroup,
                                                  fixedPipelineDrawing->windowTabIndex)) {
        case FociDrawingTypeEnum::DRAW_AS_SPHERES:
            drawAsSpheresFlag = true;
            break;
        case FociDrawingTypeEnum::DRAW_AS_SQUARES:
            break;
    }
    
    switch (drawType) {
        case DrawType::HISTOLOGY:
            /*
             * Histology slice is drawn in "histology plane coordinates"
             */
            CaretAssert(histologySlice);
            focusDiameter *= histologySlice->getMillimetersToPlaneFactor();
            
            /*
             * Spheres do not work on histology
             */
            break;
        case DrawType::VOLUME_MPR:
            break;
        case DrawType::VOLUME_ORTHOGONAL:
            break;
        case DrawType::VOLUME_OBLIQUE:
            break;
    }

    /*
     * Process each foci file
     */
    const int32_t numberOfFociFiles = brain->getNumberOfFociFiles();
    for (int32_t iFile = 0; iFile < numberOfFociFiles; iFile++) {
        FociFile* fociFile = brain->getFociFile(iFile);
        
        const GroupAndNameHierarchyModel* classAndNameSelection = fociFile->getGroupAndNameHierarchyModel();
        if (classAndNameSelection->isSelected(displayGroup,
                                              fixedPipelineDrawing->windowTabIndex) == false) {
            continue;
        }
        
        const GiftiLabelTable* classColorTable = fociFile->getClassColorTable();
        const GiftiLabelTable* nameColorTable = fociFile->getNameColorTable();
        
        const int32_t numFoci = fociFile->getNumberOfFoci();
        
        for (int32_t j = 0; j < numFoci; j++) {
            Focus* focus = fociFile->getFocus(j);
            
            const GroupAndNameHierarchyItem* groupNameItem = focus->getGroupNameSelectionItem();
            if (groupNameItem != NULL) {
                if (groupNameItem->isSelected(displayGroup,
                                              fixedPipelineDrawing->windowTabIndex) == false) {
                    continue;
                }
            }
            
            float rgbaFloat[4] = { 0.0, 0.0, 0.0, 1.0 };
            switch (fociColoringType) {
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_CLASS:
                    if (focus->isClassRgbaValid() == false) {
                        const GiftiLabel* colorLabel = classColorTable->getLabelBestMatching(focus->getClassName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgbaFloat);
                            focus->setClassRgba(rgbaFloat);
                        }
                        else {
                            focus->setClassRgba(rgbaFloat);
                        }
                    }
                    focus->getClassRgba(rgbaFloat);
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_STANDARD_COLOR:
                    rgbaFloat[0] = caretColorRGBA[0];
                    rgbaFloat[1] = caretColorRGBA[1];
                    rgbaFloat[2] = caretColorRGBA[2];
                    rgbaFloat[3] = caretColorRGBA[3];
                    break;
                case FeatureColoringTypeEnum::FEATURE_COLORING_TYPE_NAME:
                    if (focus->isNameRgbaValid() == false) {
                        const GiftiLabel* colorLabel = nameColorTable->getLabelBestMatching(focus->getName());
                        if (colorLabel != NULL) {
                            colorLabel->getColor(rgbaFloat);
                            focus->setNameRgba(rgbaFloat);
                        }
                        else {
                            focus->setNameRgba(rgbaFloat);
                        }
                    }
                    focus->getNameRgba(rgbaFloat);
                    break;
            }
            
            /*
             * Some label tables may have alpha at zero, so correct it
             */
            rgbaFloat[3] = 1.0;
            
            const int32_t numProjections = focus->getNumberOfProjections();
            for (int32_t k = 0; k < numProjections; k++) {
                const SurfaceProjectedItem* spi = focus->getProjection(k);
                if (spi->isVolumeXYZValid()) {
                    float xyz[3];
                    spi->getVolumeXYZ(xyz);
                    
                    bool drawFocusFlag = false;
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
                                distanceToHistologySliceTolerance = 500.0;
                                if (distanceToSlice < distanceToHistologySliceTolerance) {
                                    xyz[0] = planeOnSliceXYZ[0];
                                    xyz[1] = planeOnSliceXYZ[1];
                                    xyz[2] = planeOnSliceXYZ[2];
                                    drawFocusFlag = true;
                                }
                            }
                        }
                            break;
                        case DrawType::VOLUME_MPR:
                        case DrawType::VOLUME_OBLIQUE:
                        case DrawType::VOLUME_ORTHOGONAL:
                            if (plane.absoluteDistanceToPlane(xyz) < halfSliceThickness) {
                                float xyzOnPlane[3];
                                plane.projectPointToPlane(xyz, xyzOnPlane);
                                xyz[0] = xyzOnPlane[0];
                                xyz[1] = xyzOnPlane[1];
                                xyz[2] = xyzOnPlane[2];
                                drawFocusFlag = true;
                            }
                            break;
                    }
                    
                    if (drawFocusFlag) {
                        uint8_t rgbaByte[4] = {
                            static_cast<uint8_t>(rgbaFloat[0] * 255.0),
                            static_cast<uint8_t>(rgbaFloat[1] * 255.0),
                            static_cast<uint8_t>(rgbaFloat[2] * 255.0),
                            static_cast<uint8_t>(rgbaFloat[3] * 255.0),
                        };
                        
                        glPushMatrix();
                        if (selectFlag) {
                            fixedPipelineDrawing->colorIdentification->addItem(rgbaByte,
                                                                               SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                                               iFile, /* file index */
                                                                               j,     /* focus index */
                                                                               k);    /* projection index */
                            rgbaByte[3] = 255;
                        }
                        /*
                         * Need to draw each symbol independently since each symbol
                         * contains a unique size (diameter)
                         */
                        std::unique_ptr<GraphicsPrimitiveV3fC4ub> idPrimitive;
                        if (drawAsSpheresFlag) {
                            idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::SPHERES));
                            idPrimitive->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS,
                                                           focusDiameter);
                            
                        }
                        else {
                            idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::OPENGL_POINTS));
                            idPrimitive->setPointDiameter(GraphicsPrimitive::PointSizeType::MILLIMETERS,
                                                          focusDiameter);
                        }
                        idPrimitive->addVertex(xyz,
                                               rgbaByte);
                        GraphicsEngineDataOpenGL::draw(idPrimitive.get());
                        glPopMatrix();
                    }
                }
            }
        }
    }
    
    if (selectFlag) {
        int32_t fociFileIndex = -1;
        int32_t focusIndex = -1;
        int32_t focusProjectionIndex = -1;
        float depth = -1.0;
        fixedPipelineDrawing->getIndexFromColorSelection(SelectionItemDataTypeEnum::FOCUS_VOLUME,
                                                           fixedPipelineDrawing->mouseX,
                                                           fixedPipelineDrawing->mouseY,
                                                           fociFileIndex,
                                                           focusIndex,
                                                           focusProjectionIndex,
                                                           depth);
        if (fociFileIndex >= 0) {
            switch (drawType) {
                case DrawType::HISTOLOGY:
                    break;
                case DrawType::VOLUME_MPR:
                case DrawType::VOLUME_OBLIQUE:
                case DrawType::VOLUME_ORTHOGONAL:
                    CaretAssert(selectVolumeFocus);
                    if (selectVolumeFocus->isOtherScreenDepthCloserToViewer(depth)) {
                        Focus* focus = brain->getFociFile(fociFileIndex)->getFocus(focusIndex);
                        selectVolumeFocus->setBrain(brain);
                        selectVolumeFocus->setFocus(focus);
                        selectVolumeFocus->setFociFile(brain->getFociFile(fociFileIndex));
                        selectVolumeFocus->setFocusIndex(focusIndex);
                        selectVolumeFocus->setFocusProjectionIndex(focusProjectionIndex);
                        selectVolumeFocus->setVolumeFile(underlayVolume);
                        selectVolumeFocus->setScreenDepth(depth);
                        float xyz[3];
                        const SurfaceProjectedItem* spi = focus->getProjection(focusProjectionIndex);
                        spi->getVolumeXYZ(xyz);
                        fixedPipelineDrawing->setSelectedItemScreenXYZ(selectVolumeFocus, xyz);
                        CaretLogFine("Selected Volume Focus Identification Symbol: " + QString::number(focusIndex));
                    }
                    break;
            }
         }
    }
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
BrainOpenGLFociDrawing::drawSquare(const float size)
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
