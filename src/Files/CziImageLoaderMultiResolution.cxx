
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
    return m_cziImage.get();
}

/**
 * @return Pointer to CZI image loaded by this loader (NULL if not available)
 */
const CziImage*
CziImageLoaderMultiResolution::getImage() const
{
    return m_cziImage.get();
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
                                           const int32_t manualPyramidLayerIndex,
                                           const GraphicsObjectToWindowTransform* transform)
{
    m_frameChangedFlag = false;
    m_reloadImageFlag  = false;
    
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
                                                         frameIndex,
                                                         allFramesFlag,
                                                         transform);
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

    if ( ! m_reloadImageFlag) {
        switch (resolutionChangeMode) {
            case CziImageResolutionChangeModeEnum::INVALID:
                CaretAssert(0);
                break;
            case CziImageResolutionChangeModeEnum::AUTO2:
                m_reloadImageFlag = isReloadForPanZoom(cziImage,
                                                       transform);
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
    m_previousManualPyramidLayerIndex = manualPyramidLayerIndex;
    m_reloadImageFlag                 = false;
    m_frameChangedFlag                = false;
}

/**
 * Force reloading of the image
 */
void
CziImageLoaderMultiResolution::forceImageReloading()
{
    m_cziImage.reset();
    m_reloadImageFlag = true;
}

/**
 * Get best layer index
 * @param frameIndex
 *    Index of frame
 * @param allFramesFlag
 *    True if all frames are selected for display
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
int32_t
CziImageLoaderMultiResolution::getLayerIndexForCurrentZoom(const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                           const int32_t frameIndex,
                                                           const bool allFramesFlag,
                                                           const GraphicsObjectToWindowTransform* transform) const
{
    int32_t layerIndex(-1);
    
    /*
     * Compute pixel height of the high-resolution image when drawn in the window.
     * The size of the image, in pixels when drawn, determines when to switch to
     * lower or higher resolution image.
     */
    const float imageBottomLeftPixel[3] { 0.0, static_cast<float>(cziSceneInfo.m_logicalRectangle.height()), 0.0 };
    const float imageTopLeftPixel[3] { 0.0, 0.0, 0.0 };
    float imageBottomLeftWindow[3];
    float imageTopLeftWindow[3];
    transform->transformPoint(imageBottomLeftPixel, imageBottomLeftWindow);
    transform->transformPoint(imageTopLeftPixel, imageTopLeftWindow);
    const float drawnPixelHeight = imageTopLeftWindow[1] - imageBottomLeftWindow[1];
    if (cziDebugFlag) std::cout << "Drawn pixel height: " << drawnPixelHeight << std::endl;
    
    const int32_t numberOfPyramidLayers(cziSceneInfo.getNumberOfPyramidLayers());
    layerIndex = (numberOfPyramidLayers - 1);
    for (int32_t i = 0;
         i < numberOfPyramidLayers;
         i++) {
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
 */
QRectF
CziImageLoaderMultiResolution::getViewportLogicalCoordinates(const GraphicsObjectToWindowTransform* transform) const
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
     * 'inverseTransformPoint()' transforms from window coordinates to the ortho's pixel index with
     * origin at bottom left.
     */
    float viewportTopLeftWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x(),
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     viewportTopLeftWindowCoordinate);
    const PixelLogicalIndex viewportLogicalTopLeft(viewportTopLeftWindowCoordinate);
    
    /*
     * Bottom Right Corner of Window
     * 'inverseTransformPoint()' transforms from window coordinates to the ortho's pixel index with
     * origin at bottom left.
     */
    float viewportBottomRightWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x() + viewport.width(),
                                     viewport.y(),
                                     0.0,
                                     viewportBottomRightWindowCoordinate);
    const PixelLogicalIndex viewportLogicalBottomRight(viewportBottomRightWindowCoordinate);
    /*
     * CZI Logical coordinates of viewport (portion of CZI image that fills the viewport)
     */
    const QRectF viewportFullResLogicalRect(viewportLogicalTopLeft.getI(),
                                            viewportLogicalTopLeft.getJ(),
                                            viewportLogicalBottomRight.getI() - viewportLogicalTopLeft.getI(),
                                            viewportLogicalBottomRight.getJ() - viewportLogicalTopLeft.getJ());

    return viewportFullResLogicalRect;
}

/**
 * @return Does new image data need to be loaded due to panning?
 * @param cziImage
 *    Image currently displayed
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
bool
CziImageLoaderMultiResolution::isReloadForPanZoom(const CziImage* cziImage,
                                                  const GraphicsObjectToWindowTransform* transform) const
{
    CaretAssert(cziImage);
    CaretAssert(transform);
    
    /*
     * Logical rectangle of viewport
     */
    const QRectF viewportLogicalRect(getViewportLogicalCoordinates(transform));
    
    /*
     * If no part of full image bounds overlaps viewport, then exit
     */
    if ( ! viewportLogicalRect.intersects(cziImage->m_fullResolutionLogicalRect)) {
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
    if (cziImage->m_imageDataLogicalRect.contains(viewportLogicalRect)) {
        if (cziDebugFlag) std::cout << "Pan/Zoom test Reject: Loaded image data overlaps viewport" << std::endl;
        return false;
    }
    
    /*
     * Region of full image that overlaps viewport
     */
    const float fullImageIntersectionArea(CziUtilities::intersectionArea(cziImage->m_fullResolutionLogicalRect,
                                                                         viewportLogicalRect));

    /*
     * Region of image loaded that overlaps viewport
     */
    const float loadedImageIntersetionArea(CziUtilities::intersectionArea(cziImage->m_imageDataLogicalRect,
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
 * @param pyramidLayerIndexIn
 *    Index of the pyramid layer
 */
CziImage*
CziImageLoaderMultiResolution::loadImageForPyrmaidLayer(const CziImage* oldCziImage,
                                                        const CziImageFile::CziSceneInfo& cziSceneInfo,
                                                        const GraphicsObjectToWindowTransform* transform,
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
    
    CaretAssertVectorIndex(allPyramidLayers, pyramidLayerIndex);
    const auto& pyramidLayer = allPyramidLayers[pyramidLayerIndex];
    
    
    
    /*
     * Intersection of viewport and image data
     */
    const QRectF viewportRect(getViewportLogicalCoordinates(transform));
    if ( ! viewportRect.intersects(cziSceneInfo.m_logicalRectangle)) {
        std::cout << "Image does not overlap viewport " << std::endl;
        return NULL;
    }
    QRectF rectToLoad(viewportRect.intersected(cziSceneInfo.m_logicalRectangle));
    CaretAssert(rectToLoad.isValid());

    /*
     * Expand the region for loading so that it is bigger than the viewport
     * and it will prevent reloading when the image is panned by a small amount.
     */
    const float percentageToExpend(20.0);
    rectToLoad = CziUtilities::expandByPercentage(rectToLoad,
                                                  percentageToExpend);
    rectToLoad = rectToLoad.intersected(cziSceneInfo.m_logicalRectangle);
 
    /*
     * If the region has not changed, do not need to load data
     */
    if (oldCziImage->m_imageDataLogicalRect == rectToLoad) {
        /*
         * Continue using image
         */
        return const_cast<CziImage*>(oldCziImage);
    }
    
    AString errorMessage;
    CziImage* cziImageOut = m_cziImageFile->readFromCziImageFile(rectToLoad,
                                                                 &cziSceneInfo,
                                                                 pyramidLayerIndex,
                                                                 cziSceneInfo.m_logicalRectangle,
                                                                 m_cziImageFile->getPreferencesImageDimension(),
                                                                 CziImageResolutionChangeModeEnum::AUTO2,
                                                                 pyramidLayerIndex,
                                                                 errorMessage);

    if (cziImageOut == NULL) {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for frame(scene) index="
                       + cziSceneInfo.m_sceneIndex
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

