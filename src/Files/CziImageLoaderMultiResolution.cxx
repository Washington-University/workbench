
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

#define __CZI_IMAGE_LOADER_MULTI_RESOLUTION_DECLARE__
#include "CziImageLoaderMultiResolution.h"
#undef __CZI_IMAGE_LOADER_MULTI_RESOLUTION_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImage.h"
#include "CziImageFile.h"
#include "CziUtilities.h"
#include "ElapsedTimer.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsUtilitiesOpenGL.h"

using namespace caret;

static const bool cziDebugFlag(false);
    
/**
 * \class caret::CziImageLoaderMultiResolution
 * \brief Loads image data for all frames in a CZI Image File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziImageLoaderMultiResolution::CziImageLoaderMultiResolution()
: CziImageLoaderBase()
{
    
}

/**
 * Destructor.
 */
CziImageLoaderMultiResolution::~CziImageLoaderMultiResolution()
{
    m_cziImage.reset();
}

/**
 * Initialize this instance with the given CZI Image File
 * @param tabIndex
 *    Index of tab
 * @param overlayIndex
 *    Index of overlay
 * @param cziImageFile
 *    CZI image file whose data is load by this instance
 */
void
CziImageLoaderMultiResolution::initialize(const int32_t tabIndex,
                                          const int32_t overlayIndex,
                                          CziImageFile* cziImageFile)
{
    m_tabIndex = tabIndex;
    m_overlayIndex = overlayIndex;
    m_cziImageFile = cziImageFile;
}

/**
 * @return Pointer to CZI image loaded by this loader (NULL if not available)
 */
CziImage*
CziImageLoaderMultiResolution::getImage()
{
    if (m_cziImage) {
        return m_cziImage.get();
    }
    
    return NULL;
}

/**
 * @return Pointer to CZI image loaded by this loader (NULL if not available)
 */
const CziImage*
CziImageLoaderMultiResolution::getImage() const
{
    CziImageLoaderMultiResolution* nonConstThis(const_cast<CziImageLoaderMultiResolution*>(this));
    CaretAssert(nonConstThis);
    return nonConstThis->getImage();
}

/**
 * Possible load new image data
 * @param cziImage
 *    Currentr CZI image
 * @param frameIndex
 *    Index of frame
 * @param allFramesFlag
 *    True if all frames are selected for display
 * @param resolutionChangeMode
 *    Mode for changing resolutiln (auto/manual)
 * @param coordinateMode
 *    Coordinate mode (pixel or plane)
 * @param channelIndex
 *    Index of channel.  Use Zero for all channels.  This parameter is ignored if there
 *    is only one channel in the file.
 * @param manualPyramidLayerIndex
 *    Index of pyramid layer for manual mode
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
void
CziImageLoaderMultiResolution::updateImage(const CziImage* cziImage,
                                           const int32_t frameIndex,
                                           const bool allFramesFlag,
                                           const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                           const MediaDisplayCoordinateModeEnum::Enum coordinateMode,
                                           const int32_t channelIndex,
                                           const int32_t manualPyramidLayerIndex,
                                           const GraphicsObjectToWindowTransform* transform)
{
    m_frameChangedFlag = false;
    m_reloadImageFlag  = false;
    
    if (cziImage == NULL) {
        m_reloadImageFlag = true;
        m_frameChangedFlag = true;
        if (cziDebugFlag) std::cout << "Reload image due to previous image invalid" << std::endl;
    }
    if ((frameIndex != m_previousFrameIndex)
        || (allFramesFlag != m_previousAllFramesFlag)) {
        m_reloadImageFlag  = true;
        m_frameChangedFlag = true;
        if (cziDebugFlag) std::cout << "Reload image due to frame change" << std::endl;
    }
    if ((resolutionChangeMode != m_previousResolutionChangeMode)
        || (manualPyramidLayerIndex != m_previousManualPyramidLayerIndex)) {
        m_reloadImageFlag = true;
        if (cziDebugFlag) std::cout << "Reload image due to resolution/pyramid change" << std::endl;
    }
    
    if (m_forceImageReloadFlag) {
        m_reloadImageFlag = true;
    }
    
    const CziImageFile::CziSceneInfo& cziSceneInfo = (allFramesFlag
                                                   ? m_cziImageFile->m_allFramesPyramidInfo
                                                   : m_cziImageFile->m_cziScenePyramidInfos[frameIndex]);
    
    int32_t zoomLayerIndex(0);
    switch (resolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            CaretAssert(0);
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
            zoomLayerIndex = getLayerIndexForCurrentZoom(cziSceneInfo,
                                                         transform,
                                                         coordinateMode);
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            zoomLayerIndex = manualPyramidLayerIndex;
            break;
    }
    
    if (m_frameChangedFlag) {
        /*
         * Reset to full view of frame
         */
        zoomLayerIndex = 0;
    }

    if ( ! m_reloadImageFlag) {
        if (zoomLayerIndex != m_previousZoomLayerIndex) {
            m_reloadImageFlag = true;
            if (cziDebugFlag) std::cout << "Reload image due to zoom resolution change index=" << zoomLayerIndex << std::endl;
        }
    }
    m_previousZoomLayerIndex = zoomLayerIndex;

    if (m_previousCoordinateMode != coordinateMode) {
        m_reloadImageFlag = true;
    }
    
    if ( ! m_reloadImageFlag) {
        switch (resolutionChangeMode) {
            case CziImageResolutionChangeModeEnum::INVALID:
                CaretAssert(0);
                break;
            case CziImageResolutionChangeModeEnum::AUTO2:
                /*
                 * If cziImage is invalid, we should not get here as
                 * m_reloadImageFlag should be set
                 */
                CaretAssert(cziImage);
                m_reloadImageFlag = isReloadForPanZoom(cziImage,
                                                       transform,
                                                       coordinateMode);
                if (m_reloadImageFlag) {
                    if (cziDebugFlag) std::cout << "Reload image due to panning/zooming" << std::endl;
                }
                break;
            case CziImageResolutionChangeModeEnum::MANUAL2:
                break;
        }
    }
    
    if (m_reloadImageFlag) {
        CziImage* newImage(loadImageForPyrmaidLayer(cziImage,
                                                    cziSceneInfo,
                                                    transform,
                                                    resolutionChangeMode,
                                                    coordinateMode,
                                                    channelIndex,
                                                    zoomLayerIndex));
        if (newImage != NULL) {
            if (newImage != m_cziImage.get()) {
                m_cziImage.reset(newImage);
            }
        }
        else {
            m_cziImage.reset();
        }
    }
    m_previousFrameIndex              = frameIndex;
    m_previousAllFramesFlag           = allFramesFlag;
    m_previousResolutionChangeMode    = resolutionChangeMode;
    m_previousCoordinateMode          = coordinateMode;
    m_previousManualPyramidLayerIndex = manualPyramidLayerIndex;
    m_reloadImageFlag                 = false;
    m_frameChangedFlag                = false;
    m_forceImageReloadFlag            = false;
}

/**
 * Force reloading of the image
 */
void
CziImageLoaderMultiResolution::forceImageReloading()
{
    m_forceImageReloadFlag = true;
}

/**
 * Get best layer index
 * @param cziSceneInfo
 *    CZI info fro frame
 * @param transform
 *    Transforms from/to viewport and model coordinates
 * @param coordinateMode
 *    Coordinate mode (pixel or plane)
 */
int32_t
CziImageLoaderMultiResolution::getLayerIndexForCurrentZoom(const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                           const GraphicsObjectToWindowTransform* transform,
                                                           const MediaDisplayCoordinateModeEnum::Enum coordinateMode) const
{
    int32_t layerIndex(-1);
    
    /*
     * Compute pixel height of the high-resolution image when drawn in the window.
     * The size of the image, in pixels when drawn, determines when to switch to
     * lower or higher resolution image.
     */
    float imageBottomLeftWindow[3];
    float imageTopLeftWindow[3];
    
    switch (coordinateMode) {
        case MediaDisplayCoordinateModeEnum::PIXEL:
        {
            /*
             * The transform will convert logical coordinates to window coordinates
             */
            const float imageBottomLeftPixel[3] { 0.0, static_cast<float>(cziSceneInfo.m_logicalRectangle.height()), 0.0 };
            const float imageTopLeftPixel[3] { 0.0, 0.0, 0.0 };
            transform->transformPoint(imageBottomLeftPixel, imageBottomLeftWindow);
            transform->transformPoint(imageTopLeftPixel, imageTopLeftWindow);
        }
            break;
        case MediaDisplayCoordinateModeEnum::PLANE:
        {
            /*
             * The transform will convert plane coordinates to window coordinates
             */
            const float imageBottomLeftPixel[3] { 0.0, static_cast<float>(m_cziImageFile->getPlaneXyzRect().height()), 0.0 };
            const float imageTopLeftPixel[3] { 0.0, 0.0, 0.0 };
            transform->transformPoint(imageBottomLeftPixel, imageBottomLeftWindow);
            transform->transformPoint(imageTopLeftPixel, imageTopLeftWindow);
        }
            break;
    }
    const float drawnPixelHeight = imageTopLeftWindow[1] - imageBottomLeftWindow[1];
    if (cziDebugFlag) std::cout << "Drawn pixel height: " << drawnPixelHeight << std::endl;
    
    const std::array<int32_t, 2> pyramidIndexRange(cziSceneInfo.getPyramidLayerIndexRange());
    const int32_t firstIndex(pyramidIndexRange[0]);
    const int32_t lastIndex(pyramidIndexRange[1]);
    
    layerIndex = lastIndex;
    for (int32_t i = firstIndex; i <= lastIndex; i++) {
        CaretAssertVectorIndex(cziSceneInfo.m_pyramidLayers, i);
        if (drawnPixelHeight < cziSceneInfo.m_pyramidLayers[i].m_pixelHeight) {
            layerIndex = i;
            break;
        }
    }

    return layerIndex;
}

/**
 * @return Logical rectangle of viewport
 * @param transform
 *    Transforms from/to viewport and model coordinates
 * @param coordinateMode
 *    Coordinate mode (pixel or plane)
 */
QRectF
CziImageLoaderMultiResolution::getViewportLogicalCoordinates(const GraphicsObjectToWindowTransform* transform,
                                                             const MediaDisplayCoordinateModeEnum::Enum coordinateMode) const
{
    QRectF rect;
    switch (coordinateMode) {
        case MediaDisplayCoordinateModeEnum::PIXEL:
            rect = getViewportLogicalCoordinatesForPixelCoords(transform);
            break;
        case MediaDisplayCoordinateModeEnum::PLANE:
            rect = getViewportLogicalCoordinatesForPlaneCoords(transform);
            break;
    }
    return rect;
}

/**
 * @return Logical rectangle of viewport for pixel coords
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
QRectF
CziImageLoaderMultiResolution::getViewportLogicalCoordinatesForPixelCoords(const GraphicsObjectToWindowTransform *transform) const
{
    /*
     * After getting the viewport enlarge it a little bit.
     * When the user pans the image, this will cause new image data
     * to be loaded as the edge of the current image is about to
     * be panned into the viewport.
     *
     * If we do not enlarge the viewport, new image data is not loaded
     * until the edge of the image is moved into the viewport and this
     * results in a small amount of the background becoming visible
     * (until the new image is loaded).
     */
    const std::array<float, 4> viewportArray(transform->getViewport());
    QRectF viewport(viewportArray[0],
                    viewportArray[1],
                    viewportArray[2],
                    viewportArray[3]);
    const float mv(10);
    const QMarginsF margins(mv, mv, mv, mv);
    viewport = viewport.marginsAdded(margins);
    
    /*
     * Window coordinate at Top Left Corner of Viewport
     * 'inverseTransformPoint()' transforms from window coordinates to
     * the logical coordinate (PIXEL) or plane coordinate (PLANE)
     */
    float viewportTopLeftWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x(),
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     viewportTopLeftWindowCoordinate);
    PixelLogicalIndex viewportTopLeft(viewportTopLeftWindowCoordinate);
    
    /*
     * Window coordinate at Bottom Right of Viewport
     * 'inverseTransformPoint()' transforms from window coordinates to
     * the logical coordinate (PIXEL) or plane coordinate (PLANE)
     */
    float viewportBottomRightWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x() + viewport.width(),
                                     viewport.y(),
                                     0.0,
                                     viewportBottomRightWindowCoordinate);
    PixelLogicalIndex viewportBottomRight(viewportBottomRightWindowCoordinate);
    
    /*
     * CZI Logical coordinates of viewport (portion of CZI image that fills the viewport)
     */
    const QRectF viewportFullResLogicalRect(viewportTopLeft.getI(),
                                            viewportTopLeft.getJ(),
                                            viewportBottomRight.getI() - viewportTopLeft.getI(),
                                            viewportBottomRight.getJ() - viewportTopLeft.getJ());
    
    if (cziDebugFlag) {
        const AString validString(viewportFullResLogicalRect.isValid() ? "Valid" : "INVALID");
        std::cout << "Full Res Viewport " << validString << " for " << m_cziImageFile->getFileNameNoPath() << std::endl;
        std::cout << "   " << CziUtilities::qRectToString(viewportFullResLogicalRect) << std::endl;
        std::cout << "       Top Left Plane: " << AString::fromNumbers(viewportTopLeftWindowCoordinate) << std::endl;
        std::cout << "   Bottom Right Plane: " << AString::fromNumbers(viewportBottomRightWindowCoordinate) << std::endl;
        std::cout << "     Logical Top Left: " << viewportTopLeft.toString() << std::endl;
        std::cout << " Logical Bottom Right: " << viewportBottomRight.toString() << std::endl;
    }
    
    m_cziImageFile->planeXyzToLogicalPixelIndex(viewportTopLeftWindowCoordinate,
                                                viewportTopLeft);
    m_cziImageFile->planeXyzToLogicalPixelIndex(viewportBottomRightWindowCoordinate, 
                                                viewportBottomRight);
    return viewportFullResLogicalRect;
}

/**
 * @return Logical rectangle of viewport for plane coordinates.
 *
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
QRectF
CziImageLoaderMultiResolution::getViewportLogicalCoordinatesForPlaneCoords(const GraphicsObjectToWindowTransform* transform) const
{
    
    /*
     * Plane coordinates of viewport in a rectangle
     */
    const QRectF viewportPlaneRect(getViewportPlaneCoordinates(transform));
    const float pl(viewportPlaneRect.left());
    const float pr(viewportPlaneRect.right());
    const float pt(viewportPlaneRect.top());
    const float pb(viewportPlaneRect.bottom());
    const Vector3D planeLeftTop(pl, pt, 0.0);
    const Vector3D planeRightTop(pr, pt, 0.0);
    const Vector3D planeLeftBottom(pl, pb, 0.0);
    const Vector3D planeRightBottom(pr, pb, 0.0);

    PixelLogicalIndex topLeftLogicalPixelIndex;
    m_cziImageFile->planeXyzToLogicalPixelIndex(planeLeftTop, topLeftLogicalPixelIndex);
    
    PixelLogicalIndex topRightLogicalPixelIndex;
    m_cziImageFile->planeXyzToLogicalPixelIndex(planeRightTop, topRightLogicalPixelIndex);
    
    PixelLogicalIndex bottomLeftLogicalPixelIndex;
    m_cziImageFile->planeXyzToLogicalPixelIndex(planeLeftBottom, bottomLeftLogicalPixelIndex);
    
    PixelLogicalIndex bottomRightLogicalPixelIndex;
    m_cziImageFile->planeXyzToLogicalPixelIndex(planeRightBottom, bottomRightLogicalPixelIndex);

    BoundingBox boundingBox;
    boundingBox.resetForUpdate();
    boundingBox.update(topLeftLogicalPixelIndex.getI(), topLeftLogicalPixelIndex.getJ(), 0);
    boundingBox.update(topRightLogicalPixelIndex.getI(), topRightLogicalPixelIndex.getJ(), 0);
    boundingBox.update(bottomLeftLogicalPixelIndex.getI(), bottomLeftLogicalPixelIndex.getJ(), 0);
    boundingBox.update(bottomRightLogicalPixelIndex.getI(), bottomRightLogicalPixelIndex.getJ(), 0);
    
    const QRectF viewportFullResLogicalRect(boundingBox.getMinX(),
                                            boundingBox.getMinY(),
                                            boundingBox.getDifferenceX(),
                                            boundingBox.getDifferenceY());
    
    return viewportFullResLogicalRect;
}

/**
 * @return Logical rectangle of viewport for plane coordinates.
 *
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
QRectF
CziImageLoaderMultiResolution::getViewportLogicalCoordinatesForStereotaxicCoords(const GraphicsObjectToWindowTransform* transform) const
{
    /*
     * Plane coordinates of viewport in a rectangle
     */
    const QRectF viewportPlaneRect(getViewportPlaneCoordinates(transform));
    const float pl(viewportPlaneRect.left());
    const float pr(viewportPlaneRect.right());
    const float pt(viewportPlaneRect.top());
    const float pb(viewportPlaneRect.bottom());
    const Vector3D planeLeftTop(pl, pt, 0.0);
    const Vector3D planeRightTop(pr, pt, 0.0);
    const Vector3D planeLeftBottom(pl, pb, 0.0);
    const Vector3D planeRightBottom(pr, pb, 0.0);
    
    PixelLogicalIndex topLeftLogicalPixelIndex;
    m_cziImageFile->stereotaxicXyzToLogicalPixelIndex(m_cziImageFile->getStereotaxicXyzTopLeft(),
                                               topLeftLogicalPixelIndex);
    
    PixelLogicalIndex topRightLogicalPixelIndex;
    m_cziImageFile->stereotaxicXyzToLogicalPixelIndex(m_cziImageFile->getStereotaxicXyzTopRight(),
                                               topRightLogicalPixelIndex);
    
    PixelLogicalIndex bottomLeftLogicalPixelIndex;
    m_cziImageFile->stereotaxicXyzToLogicalPixelIndex(m_cziImageFile->getStereotaxicXyzBottomLeft(),
                                               bottomLeftLogicalPixelIndex);
    
    PixelLogicalIndex bottomRightLogicalPixelIndex;
    m_cziImageFile->stereotaxicXyzToLogicalPixelIndex(m_cziImageFile->getStereotaxicXyzBottomRight(),
                                               bottomRightLogicalPixelIndex);
    
    BoundingBox boundingBox;
    boundingBox.resetForUpdate();
    boundingBox.update(topLeftLogicalPixelIndex.getI(), topLeftLogicalPixelIndex.getJ(), 0);
    boundingBox.update(topRightLogicalPixelIndex.getI(), topRightLogicalPixelIndex.getJ(), 0);
    boundingBox.update(bottomLeftLogicalPixelIndex.getI(), bottomLeftLogicalPixelIndex.getJ(), 0);
    boundingBox.update(bottomRightLogicalPixelIndex.getI(), bottomRightLogicalPixelIndex.getJ(), 0);
    
    const QRectF viewportFullResLogicalRect(boundingBox.getMinX(),
                                            boundingBox.getMinY(),
                                            boundingBox.getDifferenceX(),
                                            boundingBox.getDifferenceY());
    
    return viewportFullResLogicalRect;
}

/**
 * @return A rectangle containing the plane coordinates of the viewport
 * @param transform
 *    Transform from the tab where image is drawn
 */
QRectF
CziImageLoaderMultiResolution::getViewportPlaneCoordinates(const GraphicsObjectToWindowTransform* transform) const
{
    /*
     * After getting the viewport enlarge it a little bit.
     * When the user pans the image, this will cause new image data
     * to be loaded as the edge of the current image is about to
     * be panned into the viewport.
     *
     * If we do not enlarge the viewport, new image data is not loaded
     * until the edge of the image is moved into the viewport and this
     * results in a small amount of the background becoming visible
     * (until the new image is loaded).
     */
    const std::array<float, 4> viewportArray(transform->getViewport());
    QRectF viewport(viewportArray[0],
                    viewportArray[1],
                    viewportArray[2],
                    viewportArray[3]);
    const float mv(10);
    const QMarginsF margins(mv, mv, mv, mv);
    viewport = viewport.marginsAdded(margins);
    
    /*
     * Window coordinate at Top Left Corner of Viewport
     * 'inverseTransformPoint()' transforms from window coordinates to
     * plane coordinate (PLANE)
     */
    Vector3D planeCoordAtTopLeft;
    transform->inverseTransformPoint(viewport.x(),
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     planeCoordAtTopLeft);
    
    /*
     * Window coordinate at Bottom Right of Viewport
     * 'inverseTransformPoint()' transforms from window coordinates to
     * plane coordinate (PLANE)
     */
    Vector3D planeCoordAtBottomRight;
    transform->inverseTransformPoint(viewport.x() + viewport.width(),
                                     viewport.y(),
                                     0.0,
                                     planeCoordAtBottomRight);
    
    /*
     * X, Y, Width, Height
     */
    const QRectF rect(planeCoordAtTopLeft[0],
                      planeCoordAtTopLeft[1],
                      (planeCoordAtBottomRight[0] - planeCoordAtTopLeft[0]),
                      (planeCoordAtBottomRight[1] - planeCoordAtTopLeft[1]));
    return rect;
}

/**
 * @return A rectangle containing the stereotaxic coordinates of the viewport
 * @param transform
 *    Transform from the tab where image is drawn
 */
QRectF
CziImageLoaderMultiResolution::getViewportStereotaxicCoordinates(const GraphicsObjectToWindowTransform* transform) const
{
    /*
     * After getting the viewport enlarge it a little bit.
     * When the user pans the image, this will cause new image data
     * to be loaded as the edge of the current image is about to
     * be panned into the viewport.
     *
     * If we do not enlarge the viewport, new image data is not loaded
     * until the edge of the image is moved into the viewport and this
     * results in a small amount of the background becoming visible
     * (until the new image is loaded).
     */
    const std::array<float, 4> viewportArray(transform->getViewport());
    QRectF viewport(viewportArray[0],
                    viewportArray[1],
                    viewportArray[2],
                    viewportArray[3]);
    const float mv(10);
    const QMarginsF margins(mv, mv, mv, mv);
    viewport = viewport.marginsAdded(margins);
    
    /*
     * Window coordinate at Bottom Left Corner of Viewport
     * 'inverseTransformPoint()' transforms from window coordinates to
     * sterotaxic coordinate
     */
    Vector3D stereotaxicCoordAtBottomLeft;
    transform->inverseTransformPoint(viewport.x(),
                                     viewport.y(),
                                     0.0,
                                     stereotaxicCoordAtBottomLeft);
    
    /*
     * Window coordinate at Top Right of Viewport
     * 'inverseTransformPoint()' transforms from window coordinates to
     * stereotaxic coordinate (PLANE)
     */
    Vector3D stereotaxicCoordAtTopRight;
    transform->inverseTransformPoint(viewport.x() + viewport.width(),
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     stereotaxicCoordAtTopRight);
    
    /*
     * X, Y, Width, Height
     * Will need to change if viewing axis (coronal) changes
     *
     * CaretAssertToDoWarning();
     */
    const QRectF rect(stereotaxicCoordAtBottomLeft[0],
                      stereotaxicCoordAtBottomLeft[1],
                      (stereotaxicCoordAtTopRight[0] - stereotaxicCoordAtBottomLeft[0]),
                      (stereotaxicCoordAtTopRight[2] - stereotaxicCoordAtBottomLeft[2]));
    return rect;
}

/**
 * @return Does new image data need to be loaded due to panning?
 * @param cziImage
 *    Image currently displayed
 * @param transform
 *    Transforms from/to viewport and model coordinates
 * @param coordinateMode
 *    Coordinate mode (pixel or plane)
 */
bool
CziImageLoaderMultiResolution::isReloadForPanZoom(const CziImage* cziImage,
                                                  const GraphicsObjectToWindowTransform* transform,
                                                  const MediaDisplayCoordinateModeEnum::Enum coordinateMode) const
{
    CaretAssert(cziImage);
    CaretAssert(transform);
    
    /*
     * Logical rectangle of viewport
     */
    const QRectF viewportLogicalRect(getViewportLogicalCoordinates(transform,
                                                                   coordinateMode));
    if ( ! viewportLogicalRect.isValid()) {
        std::cout << "Viewport logical rectangle invalid; Image data does not intersect viewport" << std::endl;
        return false;
    }
    
    /*
     * If no part of full image bounds overlaps viewport, then exit
     */
    if ( ! viewportLogicalRect.intersects(cziImage->getFullResolutionLogicalRect())) {
        if (cziDebugFlag) std::cout << "Pan/Zoom Test Reject: Image data does not intersect viewport" << std::endl;
        return false;
    }
    
    /*
     * If all of the image data is loaded, then cannot load anymore data
     */
    if (cziImage->isEntireImageLoaded()) {
        if (cziDebugFlag) std::cout << "Pan/Zoom Test Reject: All possible image data is loaded" << std::endl;
        return false;
    }
    
    /*
     * If current image contains viewport, do not need to load data
     */
    if (cziImage->getImageDataLogicalRect().contains(viewportLogicalRect)) {
        if (cziDebugFlag) std::cout << "Pan/Zoom test Reject: Loaded image data overlaps viewport" << std::endl;
        return false;
    }
    
    /*
     * Region of full image that overlaps viewport
     */
    const float fullImageIntersectionArea(CziUtilities::intersectionArea(cziImage->getFullResolutionLogicalRect(),
                                                                         viewportLogicalRect));

    /*
     * Region of image loaded that overlaps viewport
     */
    const float loadedImageIntersetionArea(CziUtilities::intersectionArea(cziImage->getImageDataLogicalRect(),
                                                                          viewportLogicalRect));
    
    /*
     * No additional image data can be loaded that is within the viewport ?
     */
    if (loadedImageIntersetionArea >= fullImageIntersectionArea) {
        if (cziDebugFlag) std::cout << "Pan/Zoom Test Reject: No additional image data is available that overlaps viewport" << std::endl;
        return false;
    }
    
    /*
     * If we are here, additional image data can be loaded that is within the viewport
     */
    if (cziDebugFlag) std::cout << "Pan/Zoom Test Accept: Need to load image data" << std::endl;
    
    return true;
}

/**
 * Load a image from the given pyramid layer for the center of the tab region defined by the transform
 * @param oldCziImage
 *    Current CZI image
 * @param cziSceneInfo
 *    CZI scene info (pyramid layers) for image selection
 * @param transform
 *    Transform from the tab where image is drawn
 * @param resolutionChangeMode
 *       The resolution change mode
 * @param coordinateMode
 *    Coordinate mode (pixel or plane)
 * @param pyramidLayerIndexIn
 *    Index of the pyramid layer
 */
CziImage*
CziImageLoaderMultiResolution::loadImageForPyrmaidLayer(const CziImage* oldCziImage,
                                                        const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                        const GraphicsObjectToWindowTransform* transform,
                                                        const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                        const MediaDisplayCoordinateModeEnum::Enum coordinateMode,
                                                        const int32_t channelIndex,
                                                        const int32_t pyramidLayerIndexIn)
{
    CziImage* cziImageOut(NULL);
    switch (coordinateMode) {
        case MediaDisplayCoordinateModeEnum::PIXEL:
            cziImageOut = loadImageForPyrmaidLayerForPixelCoords(oldCziImage,
                                                                 cziSceneInfo,
                                                                 transform,
                                                                 resolutionChangeMode,
                                                                 channelIndex,
                                                                 pyramidLayerIndexIn);
            break;
        case MediaDisplayCoordinateModeEnum::PLANE:
            cziImageOut = loadImageForPyrmaidLayerForPlaneCoords(oldCziImage,
                                                                 cziSceneInfo,
                                                                 transform,
                                                                 resolutionChangeMode,
                                                                 channelIndex,
                                                                 pyramidLayerIndexIn);
            break;
    }
    return cziImageOut;
}


/**
 * Load a image from the given pyramid layer for the center of the tab region defined by the transform for PIXEL coords
 * @param oldCziImage
 *    Current CZI image
 * @param cziSceneInfo
 *    CZI scene info (pyramid layers) for image selection
 * @param transform
 *    Transform from the tab where image is drawn
 * @param resolutionChangeMode
 *       The resolution change mode
 * @param channelIndex
 *    Index of channel.  Use Zero for all channels.  This parameter is ignored if there
 *    is only one channel in the file.
 * @param pyramidLayerIndexIn
 *    Index of the pyramid layer
 */
CziImage*
CziImageLoaderMultiResolution::loadImageForPyrmaidLayerForPixelCoords(const CziImage* oldCziImage,
                                                                      const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                                      const GraphicsObjectToWindowTransform* transform,
                                                                      const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                                      const int32_t channelIndex,
                                                                      const int32_t pyramidLayerIndexIn)
{
    const auto& allPyramidLayers(cziSceneInfo.m_pyramidLayers);
    if (allPyramidLayers.empty()) {
        CaretLogSevere("Attempting to load pyramid layer="
                       + AString::number(pyramidLayerIndexIn)
                       + " but no pyramid layers available.");
    }
    
    int32_t pyramidLayerIndex(pyramidLayerIndexIn);
    if (pyramidLayerIndex < 0) {
        pyramidLayerIndex = 0;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    else if (pyramidLayerIndex >= static_cast<int32_t>(allPyramidLayers.size())) {
        pyramidLayerIndex = allPyramidLayers.size() - 1;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    
    
    
    /*
     * Intersection of viewport and image data
     */
    const QRectF viewportRect(getViewportLogicalCoordinates(transform,
                                                            MediaDisplayCoordinateModeEnum::PIXEL));
    CaretAssert(viewportRect.isValid());
    CaretAssert(cziSceneInfo.m_logicalRectangle.isValid());
    CaretAssert(viewportRect.intersects(cziSceneInfo.m_logicalRectangle)
                == cziSceneInfo.m_logicalRectangle.intersects(viewportRect));
    QRectF rectToLoad;
    if (viewportRect.intersects(cziSceneInfo.m_logicalRectangle)) {
        rectToLoad = viewportRect.intersected(cziSceneInfo.m_logicalRectangle);
    }
    else if (viewportRect.contains(cziSceneInfo.m_logicalRectangle)) {
        rectToLoad = cziSceneInfo.m_logicalRectangle;
    }
    else if (cziSceneInfo.m_logicalRectangle.contains(viewportRect)) {
        rectToLoad = viewportRect;
    }
    else {
        rectToLoad = cziSceneInfo.m_logicalRectangle;
    }
    
    CaretAssert(rectToLoad.isValid());
    
    CaretAssertVectorIndex(allPyramidLayers, pyramidLayerIndex);
    const auto& selectedPyramidLayer(allPyramidLayers[pyramidLayerIndex]);
    if ((selectedPyramidLayer.m_logicalWidthForImageReading == cziSceneInfo.m_logicalRectangle.width())
        && (selectedPyramidLayer.m_logicalHeightForImageReading == cziSceneInfo.m_logicalRectangle.height())) {
        rectToLoad = cziSceneInfo.m_logicalRectangle;
        if (cziDebugFlag) std::cout << "Load full resolution" << std::endl;
    }
    else {
        const float widthToLoad(selectedPyramidLayer.m_logicalWidthForImageReading);
        const float heightToLoad(selectedPyramidLayer.m_logicalHeightForImageReading);
        
        const QPointF centerXY(rectToLoad.center());
        const float rectX(centerXY.x() - (widthToLoad / 2.0));
        const float rectY(centerXY.y() - (heightToLoad / 2.0));
        if (cziDebugFlag) std::cout << "Rect to load before pyramid size: " << CziUtilities::qRectToString(rectToLoad) << std::endl;
        rectToLoad.setRect(rectX, rectY, widthToLoad, heightToLoad);
        if (cziDebugFlag) std::cout << "             after pyramid size: " << CziUtilities::qRectToString(rectToLoad) << std::endl;
        rectToLoad = rectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
        if (cziDebugFlag) std::cout << "             after clip: " << CziUtilities::qRectToString(rectToLoad) << std::endl;
    }
    
    const bool expandFlag(false);
    if (expandFlag) {
        /*
         * Expand the region for loading so that it is bigger than the viewport
         * and it will prevent reloading when the image is panned by a small amount.
         */
        const float percentageToExpend(20.0);
        rectToLoad = CziUtilities::expandByPercentage(rectToLoad,
                                                      percentageToExpend);
        rectToLoad = rectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
    }
    
    bool forceReloadFlag(m_forceImageReloadFlag);
    switch (resolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            if (pyramidLayerIndex != m_previousManualPyramidLayerIndex) {
                forceReloadFlag = true;
            }
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
            break;
    }
    
    if (forceReloadFlag) {
        /* nothing */
    }
    else if (oldCziImage != NULL) {
        /*
         * If the region has not changed, do not need to load data
         */
        if (oldCziImage->getImageDataLogicalRect() == rectToLoad) {
            /*
             * Continue using image
             */
            return const_cast<CziImage*>(oldCziImage);
        }
    }
    
    ElapsedTimer timer;
    timer.start();
    
    const AString cziName(cziSceneInfo.getName()
                          + " PyramidLayer="
                          + AString::number(pyramidLayerIndex));
    if (cziDebugFlag) std::cout << "Loading pyramid index=" << pyramidLayerIndex << ", rect=" << CziUtilities::qRectToString(rectToLoad) << std::endl;
    AString errorMessage;
    CziImage* cziImageOut = m_cziImageFile->readFromCziImageFile(s_imageDataFormatForReading,
                                                                 cziName,
                                                                 channelIndex,
                                                                 rectToLoad,
                                                                 cziSceneInfo.m_logicalRectangle,
                                                                 m_cziImageFile->getPreferencesImageDimension(),
                                                                 errorMessage);
    
    if (cziDebugFlag) std::cout << "Time to load CZI Image: (ms): " << timer.getElapsedTimeMilliseconds() << std::endl;
    
    if (cziImageOut == NULL) {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for frame(scene) index="
                       + AString::number(cziSceneInfo.m_sceneIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(rectToLoad)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " error: "
                       + errorMessage);
    }
    else {
        if (cziDebugFlag) std::cout << "Image Pixels width=" << cziImageOut->getWidth() << ", " << cziImageOut->getHeight() << std::endl;
    }
    
    return cziImageOut;
}

/**
 * Load a image from the given pyramid layer for the center of the tab region defined by the transform
 * @param oldCziImage
 *    Current CZI image
 * @param cziSceneInfo
 *    CZI scene info (pyramid layers) for image selection
 * @param transform
 *    Transform from the tab where image is drawn
 * @param resolutionChangeMode
 *       The resolution change mode
 * @param channelIndex
 *    Index of channel.  Use Zero for all channels.  This parameter is ignored if there
 *    is only one channel in the file.
 * @param pyramidLayerIndexIn
 *    Index of the pyramid layer
 */
CziImage*
CziImageLoaderMultiResolution::loadImageForPyrmaidLayerForPlaneCoords(const CziImage* oldCziImage,
                                                                      const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                                      const GraphicsObjectToWindowTransform* transform,
                                                                      const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                                      const int32_t channelIndex,
                                                                      const int32_t pyramidLayerIndexIn)
{
    const auto& allPyramidLayers(cziSceneInfo.m_pyramidLayers);
    if (allPyramidLayers.empty()) {
        CaretLogSevere("Attempting to load pyramid layer="
                       + AString::number(pyramidLayerIndexIn)
                       + " but no pyramid layers available.");
    }
    
    int32_t pyramidLayerIndex(pyramidLayerIndexIn);
    if (pyramidLayerIndex < 0) {
        pyramidLayerIndex = 0;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    else if (pyramidLayerIndex >= static_cast<int32_t>(allPyramidLayers.size())) {
        pyramidLayerIndex = allPyramidLayers.size() - 1;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    
    
    /*
     * Get bounds of viewport in plane coordinates
     */
    const QRectF viewportPlaneCoordsRect(getViewportPlaneCoordinates(transform));
    CaretAssert(viewportPlaneCoordsRect.isValid());
    CaretAssert(cziSceneInfo.m_planeRectangle.isValid());
    CaretAssert(viewportPlaneCoordsRect.intersects(cziSceneInfo.m_planeRectangle)
                == cziSceneInfo.m_planeRectangle.intersects(viewportPlaneCoordsRect));

    QRectF planeRectToLoad;
    if (viewportPlaneCoordsRect.intersects(cziSceneInfo.m_planeRectangle)) {
        planeRectToLoad = viewportPlaneCoordsRect.intersected(cziSceneInfo.m_planeRectangle);
    }
    else if (viewportPlaneCoordsRect.contains(cziSceneInfo.m_planeRectangle)) {
        planeRectToLoad = cziSceneInfo.m_planeRectangle;
    }
    else if (cziSceneInfo.m_planeRectangle.contains(viewportPlaneCoordsRect)) {
        planeRectToLoad = viewportPlaneCoordsRect;
    }
    else {
        planeRectToLoad = cziSceneInfo.m_planeRectangle;
    }

    CaretAssert(planeRectToLoad.isValid());
    
    /*
     * Convert rectangle from plane to logical
     */
    QRectF logicalRectToLoad = m_cziImageFile->planeRectToLogicalRect(planeRectToLoad);
    
    CaretAssertVectorIndex(allPyramidLayers, pyramidLayerIndex);
    const auto& selectedPyramidLayer(allPyramidLayers[pyramidLayerIndex]);
    if ((selectedPyramidLayer.m_logicalWidthForImageReading == cziSceneInfo.m_logicalRectangle.width())
        && (selectedPyramidLayer.m_logicalHeightForImageReading == cziSceneInfo.m_logicalRectangle.height())) {
        logicalRectToLoad = cziSceneInfo.m_logicalRectangle;
        if (cziDebugFlag) std::cout << "Load full resolution" << std::endl;
    }
    else {
        const float widthToLoad(selectedPyramidLayer.m_logicalWidthForImageReading);
        const float heightToLoad(selectedPyramidLayer.m_logicalHeightForImageReading);
        
        const QPointF centerXY(logicalRectToLoad.center());
        const float rectX(centerXY.x() - (widthToLoad / 2.0));
        const float rectY(centerXY.y() - (heightToLoad / 2.0));
        if (cziDebugFlag) std::cout << "Rect to load before pyramid size: " << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
        logicalRectToLoad.setRect(rectX, rectY, widthToLoad, heightToLoad);
        if (cziDebugFlag) std::cout << "             after pyramid size: " << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
        logicalRectToLoad = logicalRectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
        if (cziDebugFlag) std::cout << "             after clip: " << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
    }
    
    const bool expandFlag(false);
    if (expandFlag) {
        /*
         * Expand the region for loading so that it is bigger than the viewport
         * and it will prevent reloading when the image is panned by a small amount.
         */
        const float percentageToExpend(20.0);
        logicalRectToLoad = CziUtilities::expandByPercentage(logicalRectToLoad,
                                                             percentageToExpend);
        logicalRectToLoad = logicalRectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
    }
    
    bool forceReloadFlag(m_forceImageReloadFlag);
    switch (resolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            if (pyramidLayerIndex != m_previousManualPyramidLayerIndex) {
                forceReloadFlag = true;
            }
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
            break;
    }
    
    if (forceReloadFlag) {
        /* nothing */
    }
    else if (oldCziImage != NULL) {
        /*
         * If the region has not changed, do not need to load data
         */
        if (oldCziImage->getImageDataLogicalRect() == logicalRectToLoad) {
            /*
             * Continue using image
             */
            return const_cast<CziImage*>(oldCziImage);
        }
    }
    
    ElapsedTimer timer;
    timer.start();
    
    
    const AString cziName(cziSceneInfo.getName()
                          + " PyramidLayer="
                          + AString::number(pyramidLayerIndex));
    if (cziDebugFlag) std::cout << "Loading pyramid index=" << pyramidLayerIndex << ", rect=" << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
    AString errorMessage;
    CziImage* cziImageOut = m_cziImageFile->readFromCziImageFile(s_imageDataFormatForReading,
                                                                 cziName,
                                                                 channelIndex,
                                                                 logicalRectToLoad,
                                                                 cziSceneInfo.m_logicalRectangle,
                                                                 m_cziImageFile->getPreferencesImageDimension(),
                                                                 errorMessage);
    
    if (cziDebugFlag) std::cout << "Time to load CZI Image: (ms): " << timer.getElapsedTimeMilliseconds() << std::endl;
    
    if (cziImageOut == NULL) {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for frame(scene) index="
                       + AString::number(cziSceneInfo.m_sceneIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(logicalRectToLoad)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " error: "
                       + errorMessage);
    }
    else {
        if (cziDebugFlag) std::cout << "Image Pixels width=" << cziImageOut->getWidth() << ", " << cziImageOut->getHeight() << std::endl;
    }
    
    return cziImageOut;
}

/**
 * Load a image from the given pyramid layer for the center of the tab region defined by the transform
 * @param oldCziImage
 *    Current CZI image
 * @param cziSceneInfo
 *    CZI scene info (pyramid layers) for image selection
 * @param transform
 *    Transform from the tab where image is drawn
 * @param resolutionChangeMode
 *       The resolution change mode
 * @param channelIndex
 *    Index of channel.  Use Zero for all channels.  This parameter is ignored if there
 *    is only one channel in the file.
 * @param pyramidLayerIndexIn
 *    Index of the pyramid layer
 */
CziImage*
CziImageLoaderMultiResolution::loadImageForPyrmaidLayerForStereotaxicCoords(const CziImage* oldCziImage,
                                                                            const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                                            const GraphicsObjectToWindowTransform* transform,
                                                                            const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                                                            const int32_t channelIndex,
                                                                            const int32_t pyramidLayerIndexIn)
{
    const auto& allPyramidLayers(cziSceneInfo.m_pyramidLayers);
    if (allPyramidLayers.empty()) {
        CaretLogSevere("Attempting to load pyramid layer="
                       + AString::number(pyramidLayerIndexIn)
                       + " but no pyramid layers available.");
    }
    
    int32_t pyramidLayerIndex(pyramidLayerIndexIn);
    if (pyramidLayerIndex < 0) {
        pyramidLayerIndex = 0;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    else if (pyramidLayerIndex >= static_cast<int32_t>(allPyramidLayers.size())) {
        pyramidLayerIndex = allPyramidLayers.size() - 1;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    
    
    /*
     * Get bounds of viewport in stereotaxic coordinates
     */
    const QRectF viewportStereotaxicCoordsRect(getViewportStereotaxicCoordinates(transform));
    CaretAssert(viewportStereotaxicCoordsRect.isValid());
    CaretAssert(cziSceneInfo.m_stereotaxicRectangle.isValid());
    CaretAssert(viewportStereotaxicCoordsRect.intersects(cziSceneInfo.m_stereotaxicRectangle)
                == cziSceneInfo.m_stereotaxicRectangle.intersects(viewportStereotaxicCoordsRect));
    
    QRectF stereotaxicRectToLoad;
    if (viewportStereotaxicCoordsRect.intersects(cziSceneInfo.m_stereotaxicRectangle)) {
        stereotaxicRectToLoad = viewportStereotaxicCoordsRect.intersected(cziSceneInfo.m_stereotaxicRectangle);
    }
    else if (viewportStereotaxicCoordsRect.contains(cziSceneInfo.m_stereotaxicRectangle)) {
        stereotaxicRectToLoad = cziSceneInfo.m_stereotaxicRectangle;
    }
    else if (cziSceneInfo.m_stereotaxicRectangle.contains(viewportStereotaxicCoordsRect)) {
        stereotaxicRectToLoad = viewportStereotaxicCoordsRect;
    }
    else {
        stereotaxicRectToLoad = cziSceneInfo.m_stereotaxicRectangle;
    }
    
    CaretAssert(stereotaxicRectToLoad.isValid());
    
    /*
     * Convert rectangle from stereotaxic to logical
     */
    QRectF logicalRectToLoad = m_cziImageFile->stereotaxicRectToLogicalRect(stereotaxicRectToLoad);
    
    CaretAssertVectorIndex(allPyramidLayers, pyramidLayerIndex);
    const auto& selectedPyramidLayer(allPyramidLayers[pyramidLayerIndex]);
    if ((selectedPyramidLayer.m_logicalWidthForImageReading == cziSceneInfo.m_logicalRectangle.width())
        && (selectedPyramidLayer.m_logicalHeightForImageReading == cziSceneInfo.m_logicalRectangle.height())) {
        logicalRectToLoad = cziSceneInfo.m_logicalRectangle;
        if (cziDebugFlag) std::cout << "Load full resolution" << std::endl;
    }
    else {
        const float widthToLoad(selectedPyramidLayer.m_logicalWidthForImageReading);
        const float heightToLoad(selectedPyramidLayer.m_logicalHeightForImageReading);
        
        const QPointF centerXY(logicalRectToLoad.center());
        const float rectX(centerXY.x() - (widthToLoad / 2.0));
        const float rectY(centerXY.y() - (heightToLoad / 2.0));
        if (cziDebugFlag) std::cout << "Rect to load before pyramid size: " << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
        logicalRectToLoad.setRect(rectX, rectY, widthToLoad, heightToLoad);
        if (cziDebugFlag) std::cout << "             after pyramid size: " << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
        logicalRectToLoad = logicalRectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
        if (cziDebugFlag) std::cout << "             after clip: " << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
    }
    
    const bool expandFlag(false);
    if (expandFlag) {
        /*
         * Expand the region for loading so that it is bigger than the viewport
         * and it will prevent reloading when the image is panned by a small amount.
         */
        const float percentageToExpend(20.0);
        logicalRectToLoad = CziUtilities::expandByPercentage(logicalRectToLoad,
                                                             percentageToExpend);
        logicalRectToLoad = logicalRectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
    }
    
    bool forceReloadFlag(m_forceImageReloadFlag);
    switch (resolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            if (pyramidLayerIndex != m_previousManualPyramidLayerIndex) {
                forceReloadFlag = true;
            }
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
            break;
    }
    
    if (forceReloadFlag) {
        /* nothing */
    }
    else if (oldCziImage != NULL) {
        /*
         * If the region has not changed, do not need to load data
         */
        if (oldCziImage->getImageDataLogicalRect() == logicalRectToLoad) {
            /*
             * Continue using image
             */
            return const_cast<CziImage*>(oldCziImage);
        }
    }
    
    ElapsedTimer timer;
    timer.start();
    
    
    const AString cziName(cziSceneInfo.getName()
                          + " PyramidLayer="
                          + AString::number(pyramidLayerIndex));
    if (cziDebugFlag) std::cout << "Loading pyramid index=" << pyramidLayerIndex << ", rect=" << CziUtilities::qRectToString(logicalRectToLoad) << std::endl;
    AString errorMessage;
    CziImage* cziImageOut = m_cziImageFile->readFromCziImageFile(s_imageDataFormatForReading,
                                                                 cziName,
                                                                 channelIndex,
                                                                 logicalRectToLoad,
                                                                 cziSceneInfo.m_logicalRectangle,
                                                                 m_cziImageFile->getPreferencesImageDimension(),
                                                                 errorMessage);
    
    if (cziDebugFlag) std::cout << "Time to load CZI Image: (ms): " << timer.getElapsedTimeMilliseconds() << std::endl;
    
    if (cziImageOut == NULL) {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for frame(scene) index="
                       + AString::number(cziSceneInfo.m_sceneIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(logicalRectToLoad)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " error: "
                       + errorMessage);
    }
    else {
        if (cziDebugFlag) std::cout << "Image Pixels width=" << cziImageOut->getWidth() << ", " << cziImageOut->getHeight() << std::endl;
    }
    
    return cziImageOut;
}
