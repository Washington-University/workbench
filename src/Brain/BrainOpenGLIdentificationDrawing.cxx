
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
#include "CaretPreferences.h"
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
#include "SessionManager.h"
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
 * Draw identification symbols on media file for logical coordinates
 * @param mediaFile
 *    Media file on which symbols are drawn
 * @param plane
 *    Plane of the media
 * @param mediaThickness
 *    Thickness of the media for those that support stereotaxic coordinates
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawMediaFileLogicalCoordinateIdentificationSymbols(const MediaFile* mediaFile,
                                                                     const Plane& plane,
                                                                     const float mediaThickness,
                                                                     const float viewingZoom,
                                                                     const float viewportHeight)
{
    CaretAssert(mediaFile);

    if ( ! m_idManager->isShowMediaIdentificationSymbols()) {
        return;
    }
    
    const Surface* surface(NULL);
    const VolumeMappableInterface* volume(NULL);
    float surfaceOrVolumeMaximumDimension(0.0);

    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              mediaThickness,
                              viewingZoom,
                              viewportHeight,
                              surfaceOrVolumeMaximumDimension);
}

/**
 * Draw identification symbols on media file for plane coordinates
 * @param mediaFile
 *    Media file on which symbols are drawn
 * @param plane
 *    Plane of the media
 * @param mediaThickness
 *    Thickness of the media for those that support stereotaxic coordinates
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawMediaFilePlaneCoordinateIdentificationSymbols(const MediaFile* mediaFile,
                                                                     const Plane& plane,
                                                                     const float mediaThickness,
                                                                     const float viewingZoom,
                                                                     const float viewportHeight)
{
    CaretAssert(mediaFile);
    
    if ( ! m_idManager->isShowMediaIdentificationSymbols()) {
        return;
    }
    
    const Surface* surface(NULL);
    const VolumeMappableInterface* volume(NULL);
    float surfaceOrVolumeMaximumDimension(0.0);
    
    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              mediaThickness,
                              viewingZoom,
                              viewportHeight,
                              surfaceOrVolumeMaximumDimension);
}

/**
 * Draw identification symbols on surface file
 * @param mediaFile
 *    Media file on which symbols are drawn
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawSurfaceIdentificationSymbols(const Surface* surface,
                                                                   const float viewingZoom,
                                                                   const float viewportHeight)
{
    CaretAssert(surface);

    if ( ! m_idManager->isShowSurfaceIdentificationSymbols()) {
        return;
    }
    
    const MediaFile* mediaFile(NULL);
    const VolumeMappableInterface* volume(NULL);
    Plane plane;
    const float planeThickness(1.0f);
    
    BoundingBox boundingBox;
    surface->getBounds(boundingBox);
    const float surfaceOrVolumeMaximumDimension(boundingBox.getMaximumDifferenceOfXYZ());

    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::SURFACE,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              planeThickness,
                              viewingZoom,
                              viewportHeight,
                              surfaceOrVolumeMaximumDimension);
}

/**
 * Draw volume identification symbols
 * @param volume
 *    The volume on which symbols are drawn
 * @param mapIndex
 *    Index of map in volume that is being drawn
 * @param plane
 *    Plane of the volume
 * @param sliceThickness
 *    Thickness of the slice
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawVolumeIdentificationSymbols(const VolumeMappableInterface* volume,
                                                                  const int32_t mapIndex,
                                                                  const Plane& plane,
                                                                  const float sliceThickness,
                                                                  const float viewingZoom,
                                                                  const float viewportHeight)
{
    CaretAssert(volume);
    
    if ( ! m_idManager->isShowVolumeIdentificationSymbols()) {
        return;
    }
    
    const Surface* surface(NULL);
    const MediaFile* mediaFile(NULL);
    const float surfaceOrVolumeMaximumDimension(getVolumeMaximumCoordinateDimension(volume,
                                                                                    mapIndex));

    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::VOLUME_SLICES,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              sliceThickness,
                              viewingZoom,
                              viewportHeight,
                              surfaceOrVolumeMaximumDimension);
}

/**
 * Draw volume intensity 2D symbols
 * @param volume
 *    The volume on which symbols are drawn
 * @param mapIndex
 *    Index of map in volume that is being drawn
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawVolumeIntensity2dIdentificationSymbols(const VolumeMappableInterface* volume,
                                                                             const int32_t mapIndex,
                                                                             const float viewingZoom,
                                                                             const float viewportHeight)
{
    CaretAssert(volume);
    
    if ( ! m_idManager->isShowVolumeIdentificationSymbols()) {
        return;
    }
    
    const Surface* surface(NULL);
    const MediaFile* mediaFile(NULL);
    const float surfaceOrVolumeMaximumDimension(getVolumeMaximumCoordinateDimension(volume,
                                                                                    mapIndex));

    Plane plane;
    const float sliceThickness(1.0);
    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              sliceThickness,
                              viewingZoom,
                              viewportHeight,
                              surfaceOrVolumeMaximumDimension);
}

/**
 * Draw volume intensity 3D symbols
 * @param volume
 *    The volume on which symbols are drawn
 * @param mapIndex
 *    Index of map in volume that is being drawn
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 */
void
BrainOpenGLIdentificationDrawing::drawVolumeIntensity3dIdentificationSymbols(const VolumeMappableInterface* volume,
                                                                             const int32_t mapIndex,
                                                                             const float viewingZoom,
                                                                             const float viewportHeight)
{
    CaretAssert(volume);
    
    if ( ! m_idManager->isShowVolumeIdentificationSymbols()) {
        return;
    }
    
    const Surface* surface(NULL);
    const MediaFile* mediaFile(NULL);
    const float surfaceOrVolumeMaximumDimension(getVolumeMaximumCoordinateDimension(volume,
                                                                                    mapIndex));
    
    Plane plane;
    const float sliceThickness(1.0);
    drawIdentificationSymbols(IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D,
                              surface,
                              mediaFile,
                              volume,
                              plane,
                              sliceThickness,
                              viewingZoom,
                              viewportHeight,
                              surfaceOrVolumeMaximumDimension);
}

/**
 * @return The maximum coordinate dimensions for the given volume file and map index
 * @param volume
 *    The volume
 * @param mapIndex
 *    Index of the map
 */
float
BrainOpenGLIdentificationDrawing::getVolumeMaximumCoordinateDimension(const VolumeMappableInterface* volume,
                                                             const int32_t mapIndex) const
{
    CaretAssert(volume);
    
    BoundingBox boundingBox;
    volume->getNonZeroVoxelCoordinateBoundingBox(mapIndex,
                                                 boundingBox);
    const float maxDim(boundingBox.getMaximumDifferenceOfXYZ());
    
    return maxDim;
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
 * @param viewingZoom
 *    Zooming (scaling) for current view
 * @param viewportHeight
 *    Height of viewport
 * @param surfaceOrVolumeMaximumDimension)
 *    Maximum dimension of surface or volume being drawn
 */
void
BrainOpenGLIdentificationDrawing::drawIdentificationSymbols(const IdentifiedItemUniversalTypeEnum::Enum drawingOnType,
                                                            const Surface* surface,
                                                            const MediaFile* mediaFile,
                                                            const VolumeMappableInterface* volume,
                                                            const Plane& plane,
                                                            const float planeThickness,
                                                            const float viewingZoom,
                                                            const float viewportHeight,
                                                            const float surfaceOrVolumeMaximumDimension)
{
    /*
     * Maximum distance for non-media ID shown on media
     */
    const CaretPreferences* prefs(SessionManager::get()->getCaretPreferences());
    const float maxDistanceMM(prefs->getIdentificationStereotaxicDistance());

    float mediaHeight(0.0f);
    StructureEnum::Enum surfaceStructure(StructureEnum::INVALID);
    int32_t surfaceNumberOfVertices(0);
    
    bool drawingOnSurfaceFlag(false);
    bool drawingOnMediaLogicalCoordFlag(false);
    bool drawingOnMediaPlaneCoordFlag(false);
    bool drawingOnVolumeIntensity2dFlag(false);
    bool drawingOnVolumeIntensity3dFlag(false);
    bool drawingOnVolumeSlicesFlag(false);
    
    switch (drawingOnType) {
        case IdentifiedItemUniversalTypeEnum::INVALID:
            CaretAssert(0);
            return;
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
            CaretAssert(mediaFile);
            if (mediaFile == NULL) {
                return;
            }
            drawingOnMediaLogicalCoordFlag = true;
            mediaHeight = mediaFile->getHeight();
            break;
        case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
        {
            CaretAssert(mediaFile);
            if (mediaFile == NULL) {
                return;
            }
            drawingOnMediaPlaneCoordFlag = true;
            const BoundingBox boundingBox(mediaFile->getPlaneXyzBoundingBox());
            mediaHeight = boundingBox.getDifferenceY();
            if (mediaHeight < 0.0) {
                return;
            }
        }
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
        }
            break;
        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
            CaretAssert(0);
            return;
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
            CaretAssert(volume);
            if (volume == NULL) {
                return;
            }
            drawingOnVolumeIntensity2dFlag = true;
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
            CaretAssert(volume);
            if (volume == NULL) {
                return;
            }
            drawingOnVolumeIntensity3dFlag = true;
            break;
        case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
        {
            CaretAssert(volume);
            if (volume == NULL) {
                return;
            }
            drawingOnVolumeSlicesFlag = true;
        }
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
        bool contralateralFlag(false);
        Vector3D xyz(0.0f, 0.0f, 0.0f);
        switch (item->getType()) {
            case IdentifiedItemUniversalTypeEnum::INVALID:
                break;
            case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
                    /*
                     * Drawing a media symbol on media ?
                     */
                    if (drawingOnMediaLogicalCoordFlag) {
                        CaretAssert(mediaFile);
                        if (mediaFile->getFileNameNoPath() == item->getDataFileName()) {
                            const PixelLogicalIndex pixelLogicalIndex(item->getPixelLogicalIndex());
                            xyz[0] = pixelLogicalIndex.getI();
                            xyz[1] = pixelLogicalIndex.getJ();
                            xyz[2] = 0.0;
                            drawFlag = true;
                        }
                        else if (item->isStereotaxicXYZValid()) {
                            /*
                             * Symbol is from a different image
                             */
                            xyz = item->getStereotaxicXYZ();
                            const bool nonLinearFlag(true);
                            PixelLogicalIndex pixelLogicalIndex;
                            
                            /*
                             * Need to see if xyz is close to media file within some tolerance
                             */
                            float distanceToPixelMM(1.0);
                            if (mediaFile->findPixelNearestStereotaxicXYZ(xyz,
                                                                          nonLinearFlag,
                                                                          distanceToPixelMM,
                                                                          pixelLogicalIndex)) {
                                if (mediaFile->isPixelIndexValid(pixelLogicalIndex)) {
                                    if (distanceToPixelMM < maxDistanceMM) {
                                        xyz[0] = pixelLogicalIndex.getI();
                                        xyz[1] = (mediaHeight - pixelLogicalIndex.getJ() - 1);
                                        xyz[2] = 0.0;
                                        drawFlag = true;
                                    }
                                }
                            }
                        }
                        else if (mediaFile != NULL) {
                            const PixelLogicalIndex pixelLogicalIndex(item->getPixelLogicalIndex());
                            xyz[0] = pixelLogicalIndex.getI();
                            xyz[1] = pixelLogicalIndex.getJ();
                            xyz[2] = 0.0;
                            
                            if ((xyz[0] < mediaFile->getWidth())
                                && (xyz[1] < mediaFile->getHeight())) {
                                drawFlag = true;
                            }
                        }
                    }
                    else {
                        /*
                         * Drawing media symbol on non-media (surface or volume)
                         */
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                break;
            case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
                /*
                 * Drawing a media symbol on media ?
                 */
                if (drawingOnMediaPlaneCoordFlag) {
                    CaretAssert(mediaFile);
                    if (mediaFile->getFileNameNoPath() == item->getDataFileName()) {
                        const Vector3D pixelPlaneXYZ(item->getPixelPlaneCoordinate());
                        xyz[0] = pixelPlaneXYZ[0];
                        xyz[1] = pixelPlaneXYZ[1];
                        xyz[2] = 0.0;
                        drawFlag = true;
                    }
                    else if (item->isStereotaxicXYZValid()) {
                        drawFlag = false;
                        Vector3D planeXYZ;
                        if (mediaFile->stereotaxicXyzToPlaneXyz(item->getStereotaxicXYZ(),
                                                                planeXYZ)) {
                            const QRectF rect = mediaFile->getPlaneXyzRect();
                            if (rect.contains(planeXYZ[0],
                                              planeXYZ[1])) {
                                xyz = planeXYZ;
                                drawFlag = true;
                            }
                        }
                    }
                    else if (mediaFile != NULL) {
                        const Vector3D pixelPlaneXYZ(item->getPixelPlaneCoordinate());
                        xyz[0] = pixelPlaneXYZ[0];
                        xyz[1] = pixelPlaneXYZ[1];
                        xyz[2] = 0.0;
                        
                        if (mediaFile->getPlaneXyzRect().contains(xyz[0],
                                                                  xyz[1])) {
                            drawFlag = true;
                        }
                    }
                }
                else {
                    /*
                     * Drawing media symbol on non-media (surface or volume)
                     */
                    const Vector3D pixelPlaneXYZ(item->getPixelPlaneCoordinate());
                    xyz[0] = pixelPlaneXYZ[0];
                    xyz[1] = pixelPlaneXYZ[1];
                    xyz[2] = pixelPlaneXYZ[2];
                    
                    if (mediaFile->planeXyzToStereotaxicXyz(pixelPlaneXYZ,
                                                            xyz)) {
                        drawFlag = true;
                    }
                }
                break;
            case IdentifiedItemUniversalTypeEnum::SURFACE:
                    if (drawingOnSurfaceFlag) {
                        if ((item->getStructure() == surfaceStructure)
                            && (item->getSurfaceNumberOfVertices() == surfaceNumberOfVertices)) {
                            drawFlag = true;
                        }
                        else if (m_idManager->isContralateralIdentificationEnabled()) {
                            if ((item->getContralateralStructure() == surfaceStructure)
                                && (item->getSurfaceNumberOfVertices() == surfaceNumberOfVertices)) {
                                contralateralFlag = true;
                                drawFlag = true;
                            }
                        }
                        if (drawFlag) {
                            surface->getCoordinate(item->getSurfaceVertexIndex(),
                                                   xyz);
                        }
                    }
                    else {
                        /*
                         * Drawing surface symbol on image or volume
                         */
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                break;
            case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
                CaretAssertMessage(0, "IDs not created on Intensity 2D");
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
                CaretAssertMessage(0, "IDs not created on Intensity 3D");
                break;
            case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
                    if (drawingOnVolumeSlicesFlag) {
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                    else {
                        if (item->isStereotaxicXYZValid()) {
                            xyz = item->getStereotaxicXYZ();
                            drawFlag = true;
                        }
                    }
                break;
        }
        
        if (drawFlag) {
            if (drawingOnSurfaceFlag) {
                if (m_clippingPlaneGroup->isSurfaceSelected()) {
                    if ( ! m_fixedPipelineDrawing->isCoordinateInsideClippingPlanesForStructure(surfaceStructure,
                                                                                                xyz)) {
                        drawFlag = false;
                    }
                }
                
                if (drawFlag) {
                    if (item->getType() != IdentifiedItemUniversalTypeEnum::SURFACE) {
                        CaretAssert(surface);
                        switch (surface->getStructure()) {
                            case StructureEnum::CORTEX_LEFT:
                                /* On right side of medial wall ? */
                                if (xyz[0] >= 0.0) {
                                    drawFlag = false;
                                }
                                break;
                            case StructureEnum::CORTEX_RIGHT:
                                /* On left side of medial wall ? */
                                if (xyz[0] <= 0.0) {
                                    drawFlag = false;
                                }
                                break;
                            default:
                                break;
                        }
                        
                        if (drawFlag) {
                            const Surface* anatSurface = m_brain->getPrimaryAnatomicalSurfaceForStructure(surface->getStructure());
                            if (anatSurface != NULL) {
                                const int32_t nearestVertexIndex = anatSurface->closestNode(xyz,
                                                                                            maxDistanceMM);
                                if (nearestVertexIndex >= 0) {
                                    /*
                                     * Move symbol to nearest vertex
                                     */
                                    CaretAssert(surface);
                                    surface->getCoordinate(nearestVertexIndex,
                                                           xyz);
                                }
                                else {
                                    /*
                                     * Too far from surface
                                     */
                                    drawFlag = false;
                                }
                            }
                        }
                    }
                }
            }
            
            if (drawingOnVolumeIntensity2dFlag
                || drawingOnVolumeIntensity3dFlag) {
                /* just use XYZ */
            }
            if (drawingOnVolumeSlicesFlag) {
                drawFlag = false;
                if (plane.isValidPlane()) {
                    /*
                     * Is symbol near plane of slice?
                     */
                    const float dist = std::fabs(plane.signedDistanceToPlane(xyz));
                    if (dist <= halfSliceThickness) {
                        Vector3D xyzProjected;
                        plane.projectPointToPlane(xyz, xyzProjected);
                        xyz = xyzProjected;
                        drawFlag = true;
                    }
                }
            }
            
            if (drawingOnMediaLogicalCoordFlag) {
                /*
                 * Are we drawing a non-media symbol on media?
                 */
                if (item->getType() != IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE) {
                    drawFlag = false;
                    if (mediaFile != NULL) {
                        /*
                         * Need to see if xyz is close to media file within some tolerance
                         */
                        float distanceToPixelMM(1.0);
                        const bool nonLinearFlag(true);
                        PixelLogicalIndex pixelLogicalIndex;
                        if (mediaFile->findPixelNearestStereotaxicXYZ(xyz,
                                                                      nonLinearFlag,
                                                                      distanceToPixelMM,
                                                                      pixelLogicalIndex)) {
                            if (distanceToPixelMM < maxDistanceMM) {
                                xyz[0] = pixelLogicalIndex.getI();
                                xyz[1] = pixelLogicalIndex.getJ();
                                xyz[2] = 0.0;
                                drawFlag = true;
                            }
                        }
                    }
                }
            }
            if (drawingOnMediaPlaneCoordFlag) {
                /*
                 * Are we drawing a non-media symbol on media?
                 */
                if (item->getType() != IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE) {
                    drawFlag = false;
                    if (mediaFile != NULL) {
                        /*
                         * Need to see if xyz is close to media file within some tolerance
                         */
                        Vector3D planeXYZ;
                        if (mediaFile->stereotaxicXyzToPlaneXyz(xyz, planeXYZ)) {
                            if (mediaFile->getPlaneXyzRect().contains(planeXYZ[0],
                                                                      planeXYZ[1])) {
                                xyz = planeXYZ;
                                drawFlag = true;
                            }
                        }
                    }
                }
            }
        }
        
        if (drawFlag) {
            float height(0.0f);
            if (drawingOnMediaLogicalCoordFlag) {
                height = mediaFile->getHeight();
                //height = viewportHeight;
                if (viewingZoom > 0.0) {
                    /*
                     * Prevents symbols from becoming way too large when zoomed in
                     */
                    height /= viewingZoom;
                }
            }
            else if (drawingOnMediaPlaneCoordFlag) {
                CaretAssert(mediaFile);
                if (mediaFile == NULL) {
                    return;
                }
                
                height = viewportHeight / viewingZoom;
            }
            else if (drawingOnSurfaceFlag) {
                height = surfaceOrVolumeMaximumDimension;
            }
            else if (drawingOnVolumeIntensity2dFlag
                     || drawingOnVolumeIntensity3dFlag) {
                height = surfaceOrVolumeMaximumDimension;
            }
            else if (drawingOnVolumeSlicesFlag) {
                height = surfaceOrVolumeMaximumDimension;
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
                if (drawingOnMediaLogicalCoordFlag
                    || drawingOnMediaPlaneCoordFlag) {
                    m_fixedPipelineDrawing->colorIdentification->addItem(symbolRGBA.data(),
                                                                         SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                                         selectionItemIndex);
                }
                else if (drawingOnSurfaceFlag) {
                    m_fixedPipelineDrawing->colorIdentification->addItem(symbolRGBA.data(),
                                                                         SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                                         selectionItemIndex);
                }
                else if (drawingOnVolumeIntensity2dFlag
                         || drawingOnVolumeIntensity3dFlag) {
                    m_fixedPipelineDrawing->colorIdentification->addItem(symbolRGBA.data(),
                                                                         SelectionItemDataTypeEnum::UNIVERSAL_IDENTIFICATION_SYMBOL,
                                                                         selectionItemIndex);
                }
                else if (drawingOnVolumeSlicesFlag) {
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
            const bool pointSymbolFlag(drawingOnMediaLogicalCoordFlag
                                       || drawingOnMediaPlaneCoordFlag);
            if (pointSymbolFlag) {
                idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::OPENGL_POINTS));
                idPrimitive->setPointDiameter(GraphicsPrimitive::PointSizeType::MILLIMETERS, symbolDiameter);
            }
            else {
                idPrimitive.reset(GraphicsPrimitive::newPrimitiveV3fC4ub(GraphicsPrimitive::PrimitiveType::SPHERES));
                idPrimitive->setSphereDiameter(GraphicsPrimitive::SphereSizeType::MILLIMETERS, symbolDiameter);
                
            }
            idPrimitive->addVertex(xyz,
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
                        case IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE:
                            break;
                        case IdentifiedItemUniversalTypeEnum::MEDIA_PLANE_COORDINATE:
                            break;
                        case IdentifiedItemUniversalTypeEnum::SURFACE:
                            break;
                        case IdentifiedItemUniversalTypeEnum::TEXT_NO_SYMBOL:
                            CaretAssert(0);
                            break;
                        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_2D:
                            break;
                        case IdentifiedItemUniversalTypeEnum::VOLUME_INTENSITY_3D:
                            break;
                        case IdentifiedItemUniversalTypeEnum::VOLUME_SLICES:
                            break;
                    }
                    universalSymbolSelection->setIdentifiedItemUniqueIdentifier(selectedItem->getUniqueIdentifier());
                    universalSymbolSelection->setScreenDepth(depth);
                    universalSymbolSelection->setBrain(m_brain);
                    const Vector3D xyz = selectedItem->getStereotaxicXYZ();
                    m_fixedPipelineDrawing->setSelectedItemScreenXYZ(universalSymbolSelection, xyz);
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

