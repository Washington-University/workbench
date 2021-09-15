
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_DECLARE__
#include "BrainOpenGLIdentificationDrawing.h"
#undef __BRAIN_OPEN_G_L_IDENTIFICATION_DRAWING_DECLARE__

#include <cmath>

#include "Brain.h"
#include "BrainOpenGLFixedPipeline.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ClippingPlaneGroup.h"
#include "CziImageFile.h"
#include "GraphicsEngineDataOpenGL.h"
#include "GraphicsPrimitiveV3fC4ub.h"
#include "IdentificationWithColor.h"
#include "ImageFile.h"
#include "IdentifiedItemUniversal.h"
#include "IdentificationManager.h"
#include "Plane.h"
#include "SelectionItemUniversalIdentificationSymbol.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLIdentificationDrawing 
 * \brief Draws identification symbols on images, surfaces, volumes
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param fixedPipelineDrawing
 *    The fixed pipeline drawing
 * @param brain
 *    The brain
 * @param browserTabContent
 *    The content of the browser tab
 * @param drawingMode
 *    The drawing mode
 */
BrainOpenGLIdentificationDrawing::BrainOpenGLIdentificationDrawing(BrainOpenGLFixedPipeline* fixedPipelineDrawing,
                                                                   Brain* brain,
                                                                   BrowserTabContent* browserTabContent,
                                                                   const BrainOpenGLFixedPipeline::Mode drawingMode)
: CaretObject(),
m_fixedPipelineDrawing(fixedPipelineDrawing),
m_brain(brain),
m_browserTabContent(browserTabContent),
m_drawingMode(drawingMode)
{
    CaretAssert(m_brain);
    CaretAssert(m_browserTabContent);
    
    m_clippingPlaneGroup = const_cast<ClippingPlaneGroup*>(m_browserTabContent->getClippingPlaneGroup());
    CaretAssert(m_clippingPlaneGroup);
    m_idManager = m_brain->getIdentificationManager();
    CaretAssert(m_idManager);
    m_selectionManager = m_brain->getSelectionManager();
    CaretAssert(m_selectionManager);
}

/**
 * Destructor.
 */
BrainOpenGLIdentificationDrawing::~BrainOpenGLIdentificationDrawing()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrainOpenGLIdentificationDrawing::toString() const
{
    return "BrainOpenGLIdentificationDrawing";
}

/**
 * Draw identification symbols on media file
 * @param mediaFile
 *    Media file on which symbols are drawn
 * @param plane
 *    Plane of the media
 * @param mediaThickness
 *    Thickness of the media for those that support stereotaxic coordinates
 */
void
BrainOpenGLIdentificationDrawing::drawMediaFileIdentificationSymbols(const MediaFile* mediaFile,
                                                                     const Plane& plane,
                                                                     const float mediaThickness,
                                                                     const float viewportHeight)
{
    CaretAssert(mediaFile);

    const Surface* surface(NULL);
    const VolumeMappableInterface* volume(NULL);
    
    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::MEDIA,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              mediaThickness,
                              viewportHeight);
}

/**
 * Draw identification symbols on media file
 * @param mediaFile
 *    Media file on which symbols are drawn
 */
void
BrainOpenGLIdentificationDrawing::drawSurfaceIdentificationSymbols(const Surface* surface)
{
    CaretAssert(surface);

    const MediaFile* mediaFile(NULL);
    const VolumeMappableInterface* volume(NULL);
    Plane plane;
    const float planeThickness(1.0f);
    const float viewportHeight(1.0f);
    
    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::SURFACE,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              planeThickness,
                              viewportHeight);
}

/**
 * Draw volume identification symbols
 * @param volume
 *    The volume on which symbols are drawn
 * @param plane
 *    Plane of the volume
 * @param sliceThickness
 *    Thickness of the slice
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawVolumeIdentificationSymbols(const VolumeMappableInterface* volume,
                                                                  const Plane& plane,
                                                                  const float sliceThickness,
                                                                  const float viewportHeight)
{
    CaretAssert(volume);
    
    const Surface* surface(NULL);
    const MediaFile* mediaFile(NULL);
    
    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::VOLUME,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              sliceThickness,
                              viewportHeight);
}

/**
 * Draw volume identification symbols
 * @param drawingOnType
 *    Type of model on which symbol is drawn
 * @param surface
 *    surface will be non-NULL when drawing identification symbols on a surface.
 * @param mediaFile
 *    mediaFile will be non-NULL when drawing identification symbols on a media file
 * @param volume
 *    volume will be non-NULL when drawing identification symbols on a a volume
 * @param plane
 *    Plane of the volume
 * @param planeThickness
 *    Thickness of the plane for media and volume
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawIdentificationSymbols(const IdentifiedItemUniversalTypeEnum::Enum drawingOnType,
                                                            const Surface* surface,
                                                            const MediaFile* mediaFile,
                                                            const VolumeMappableInterface* volume,
                                                            const Plane& plane,
                                                            const float planeThickness,
                                                            const float viewportHeight)
{
    /*
     * Maximum distance for non-media ID shown on media
     */
    const float mediaMaxDistanceMM(3.0);

    float mediaHeight(0.0f);
    StructureEnum::Enum surfaceStructure(StructureEnum::INVALID);
    int32_t surfaceNumberOfVertices(0);
    float surfaceMaxDimension(-1.0);
    
    bool drawingOnSurfaceFlag(false);
    bool drawingOnMediaFlag(false);
    bool drawingOnVolumeFlag(false);
    
    switch (drawingOnType) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            CaretAssert(0);
            return;
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA:
            CaretAssert(mediaFile);
            if (mediaFile == NULL) {
                return;
            }
            drawingOnMediaFlag = true;
            mediaHeight = mediaFile->getHeight();
            break;
        case IdentifiedItemUniversalTypeEnum::SURFACE:
        {
            CaretAssert(surface);
            if (surface == NULL) {
                return;
            }
            drawingOnSurfaceFlag = true;
            surfaceNumberOfVertices = surface->getNumberOfNodes();
            surfaceStructure = surface->getStructure();
            BoundingBox boundingBox;
            surface->getBounds(boundingBox);
        }
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            CaretAssert(0);
            return;
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME:
            CaretAssert(volume);
            if (volume == NULL) {
                return;
            }
            drawingOnVolumeFlag = true;
            break;
    }
    
    const float halfSliceThickness(planeThickness > 0.0
                                   ? (planeThickness * 0.55) /* ensure symbol falls within a slice*/
                                   : 1.0);

    SelectionItemUniversalIdentificationSymbol* universalSymbolSelection(m_selectionManager->getUniversalIdentificationSymbol());

    /*
     * Check for a 'selection' type mode
     */
    bool selectFlag = false;
    switch (m_drawingMode) {
        case BrainOpenGLFixedPipeline::MODE_DRAWING:
            //EventManager::get()->sendEvent(colorsFromChartsEvent.getPointer());
            break;
        case BrainOpenGLFixedPipeline::MODE_IDENTIFICATION:
            if (universalSymbolSelection->isEnabledForSelection()) {
                selectFlag = true;
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else {
                return;
            }
            break;
        case BrainOpenGLFixedPipeline::MODE_PROJECTION:
            return;
            break;
    }
    
    
    std::vector<const IdentifiedItemUniversal*> allItems(getIdentifiedItems());
    int32_t selectionItemIndex(-1);
    for (const auto& item : allItems) {
        ++selectionItemIndex;
        bool drawFlag(false);
        bool surfaceFlag(false);
        bool contralateralFlag(false);
        std::array<float, 3> xyz { 0.0f, 0.0f, 0.0f };
        switch (item->getType()) {
            case IdentifiedItemUniversalTypeEnum::INVALID:
                break;
            case IdentifiedItemUniversalTypeEnum::MEDIA:
                if (m_idManager->isShowMediaIdentificationSymbols()) {
                    bool xyzFlag(false);
                    /*
                     * Drawing a media symbol on media ?
                     */
                    if (drawingOnMediaFlag) {
                        CaretAssert(mediaFile);
                        if (mediaFile->getFileNameNoPath() == item->getDataFileName()) {
                            const PixelIndex pixelIndex(item->getPixelIndex());
                            xyz[0] = pixelIndex.getI();
                            xyz[1] = (mediaHeight - pixelIndex.getJ() - 1);  /* Convert to origin at bottom */
                            xyz[2] = 0.0;
                            drawFlag = true;
                        }
                        else if (item->isStereotaxicXYZValid()) {
                            /*
                             * Symbol is from a different image
                             */
                            xyz = item->getStereotaxicXYZ();
                            const bool nonLinearFlag(true);
                            PixelIndex pixelIndex;
                            
                            /*
                             * Need to see if xyz is close to media file within some tolerance
                             */
                            float distanceToPixelMM(1.0);
                            if (mediaFile->findPixelNearestStereotaxicXYZ(xyz, nonLinearFlag, distanceToPixelMM, pixelIndex)) {
                                if (pixelIndex.isValid()) {
                                    if (distanceToPixelMM < mediaMaxDistanceMM) {
                                        xyz[0] = pixelIndex.getI();
                                        xyz[1] = (mediaHeight - pixelIndex.getJ() - 1);
                                        xyz[2] = 0.0;
                                        drawFlag = true;
                                    }
                                }
                            }
                        }
                    }
                    else if (m_idManager->isShowOtherTypeIdentificationSymbols()) {
                        /*
                         * Drawing media symbol on non-media (surface or volume)
                         */
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            xyzFlag = true;
                            drawFlag = true;
                        }
                    }
                }
                break;
            case IdentifiedItemUniversalTypeEnum::SURFACE:
                if (m_idManager->isShowSurfaceIdentificationSymbols()) {
                    if (drawingOnSurfaceFlag) {
                        if ((item->getStructure() == surfaceStructure)
                            && (item->getSurfaceNumberOfVertices() == surfaceNumberOfVertices)) {
                            drawFlag = true;
                            surfaceFlag = true;
                        }
                        else if (m_idManager->isContralateralIdentificationEnabled()) {
                            if ((item->getContralateralStructure() == surfaceStructure)
                                && (item->getSurfaceNumberOfVertices() == surfaceNumberOfVertices)) {
                                contralateralFlag = true;
                                drawFlag = true;
                                surfaceFlag = true;
                            }
                        }
                        surface->getCoordinate(item->getSurfaceVertexIndex(),
                                               xyz.data());
                    }
                    else if (m_idManager->isShowOtherTypeIdentificationSymbols()) {
                        /*
                         * Drawing surface symbol on image or volume
                         */
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                }
                break;
            case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME:
                if (m_idManager->isShowVolumeIdentificationSymbols()) {
                    if (drawingOnVolumeFlag) {
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                    else if (m_idManager->isShowOtherTypeIdentificationSymbols()) {
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                }
                break;
        }
        
        if (drawFlag) {
            if (drawingOnSurfaceFlag) {
                if (m_clippingPlaneGroup->isSurfaceSelected()) {
                    if ( ! m_fixedPipelineDrawing->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                                xyz.data())) {
                        drawFlag = false;
                    }
                }
                
                if (drawFlag) {
                    if (item->getType() != IdentifiedItemUniversalTypeEnum::SURFACE) {
                        CaretAssert(surface);
                        switch (surface->getStructure()) {
                            case StructureEnum::CORTEX_LEFT:
                                /* On right side of medial wall */
                                if (xyz[0] > 5.0) {
                                    drawFlag = false;
                                }
                                break;
                            case StructureEnum::CORTEX_RIGHT:
                                /* On left side of medial wall */
                                if (xyz[0] < -5.0) {
                                    drawFlag = false;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
            
            if (drawingOnVolumeFlag) {
                drawFlag = false;
                if (plane.isValidPlane()) {
                    /*
                     * Is symbol near plane of slice?
                     */
                    const float dist = std::fabs(plane.signedDistanceToPlane(xyz.data()));
                    if (dist <= halfSliceThickness) {
                        std::array<float, 3> xyzProjected;
                        plane.projectPointToPlane(xyz.data(), xyzProjected.data());
                        xyz = xyzProjected;
                        drawFlag = true;
                    }
                }
            }
            
            if (drawingOnMediaFlag) {
                /*
                 * Are we drawing a non-media symbol on media?
                 */
                if (item->getType() != IdentifiedItemUniversalTypeEnum::MEDIA) {
                    drawFlag = false;
                    if (mediaFile != NULL) {
                        /*
                         * Need to see if xyz is close to media file within some tolerance
                         */
                        float distanceToPixelMM(1.0);
                        const bool nonLinearFlag(true);
                        PixelIndex pixelIndex;
                        if (mediaFile->findPixelNearestStereotaxicXYZ(xyz, nonLinearFlag, distanceToPixelMM, pixelIndex)) {
                            if (pixelIndex.isValid()) {
                                if (distanceToPixelMM < mediaMaxDistanceMM) {
                                    xyz[0] = pixelIndex.getI();
                                    xyz[1] = (mediaHeight - pixelIndex.getJ() - 1);
                                    xyz[2] = 0.0;
                                    drawFlag = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        if (drawFlag) {
            float height(0.0f);
            if (drawingOnMediaFlag) {
                height = mediaFile->getHeight();
            }
            else if (drawingOnSurfaceFlag) {
                height = surfaceMaxDimension;
            }
            else if (drawingOnVolumeFlag) {
                height = viewportHeight;
            }
            float symbolDiameter(1.0f);
            std::array<uint8_t, 4> symbolRGBA;
            m_idManager->getIdentifiedItemColorAndSize(item,
                                                       drawingOnType,
                                                       height,
                                                       contralateralFlag,
                                                       symbolRGBA,
                                                       symbolDiameter);
            
            if (selectFlag) {
                if (drawingOnMediaFlag) {
                    m_fixedPipelineDrawing->colorIdentification->addItem(symbolRGBA.data(),
                                                                         SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                                         selectionItemIndex);
                }
                else if (drawingOnSurfaceFlag) {
                    m_fixedPipelineDrawing->colorIdentification->addItem(symbolRGBA.data(),
                                                                         SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                                         selectionItemIndex);
                }
                else if (drawingOnVolumeFlag) {
                    m_fixedPipelineDrawing->colorIdentification->addItem(symbolRGBA.data(),
                                                                         SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                                         selectionItemIndex);
                }
                else {
                    CaretAssertMessage(0, "Drawing on unrecognized model type");
                    selectFlag = false;
                }
                
                /*
                 * Draw symbol a bit larger so that it is easier to select
                 */
                symbolDiameter *= 1.5;
            }
            
            /*
             * Need to draw each symbol independently since each symbol
             * contains a unique size (diameter)
             */
            std::unique_ptr<GraphicsPrimitiveV3fC4ub> idPrimitive;
            const bool pointSymbolFlag(drawingOnMediaFlag);
            if (pointSymbolFlag) {
                idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::OPENGL_POINTS));
                idPrimitive->setPointDiameter(GraphicsPrimitive::PointSizeType::MILLIMETERS, symbolDiameter);
            }
            else {
                idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::SPHERES));
                idPrimitive->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS, symbolDiameter);
                
            }
            idPrimitive->addVertex(xyz.data(),
                                   symbolRGBA.data());
            GraphicsEngineDataOpenGL::draw(idPrimitive.get());
        }
    }
    
    if (selectFlag) {
        SelectionItemDataTypeEnum::Enum selectionItemType = SelectionItemDataTypeEnum::INVALID;
        int idIndex = -1;
        float depth = -1.0;
        m_fixedPipelineDrawing->getIndexFromColorSelection(m_fixedPipelineDrawing->mouseX,
                                                           m_fixedPipelineDrawing->mouseY,
                                                           selectionItemType,
                                                           idIndex,
                                                           depth);
                
        if (universalSymbolSelection->isEnabledForSelection()) {
            if ((idIndex >= 0)
                && (selectionItemType == SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL)) {
                if (universalSymbolSelection->isOtherScreenDepthCloserToViewer(depth)) {
                    CaretAssertVectorIndex(allItems, idIndex);
                    const auto& selectedItem = allItems[idIndex];
                    switch (selectedItem->getType()) {
                        case IdentifiedItemUniversalTypeEnum::INVALID:
                            CaretAssert(0);
                            break;
                        case IdentifiedItemUniversalTypeEnum::MEDIA:
                            break;
                        case IdentifiedItemUniversalTypeEnum::SURFACE:
                            break;
                        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
                            CaretAssert(0);
                            break;
                        case IdentifiedItemUniversalTypeEnum::VOLUME:
                            break;
                    }
                    universalSymbolSelection->setIdentifiedItemUniqueIdentifier(selectedItem->getUniqueIdentifier());
                    universalSymbolSelection->setScreenDepth(depth);
                    universalSymbolSelection->setBrain(m_brain);
                    const std::array<float, 3> xyz = selectedItem->getStereotaxicXYZ();
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(universalSymbolSelection, xyz.data());
                    CaretLogFine("Selected Universal Identification Symbol: " + QString::number(idIndex));
                }
            }
        }
    }
}

/**
 * Get all of the idenfiied items
 */
std::vector<const IdentifiedItemUniversal*>
BrainOpenGLIdentificationDrawing::getIdentifiedItems()
{
    std::vector<const IdentifiedItemUniversal*> items(m_idManager->getIdentifiedItems());
    
    return items;
}

