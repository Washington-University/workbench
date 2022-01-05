
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
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
void
CziImageLoaderMultiResolution::updateImage(const CziImage* cziImage,
                                           const int32_t frameIndex,
                                           const bool allFramesFlag,
                                           const GraphicsObjectToWindowTransform* transform)
{
    bool reloadImageFlag(false);
    if ((frameIndex != m_previousFrameIndex)
        || (allFramesFlag != m_previousAllFramesFlag)) {
        reloadImageFlag = true;
        std::cout << "Reload image due to frame change" << std::endl;
    }
    
    const CziImageFile::CziSceneInfo& cziSceneInfo = (allFramesFlag
                                                   ? m_cziImageFile->m_allFramesPyramidInfo
                                                   : m_cziImageFile->m_cziScenePyramidInfos[frameIndex]);
    
    /*
     * Always update zoom layer index
     */
    const int32_t zoomLayerIndex = getLayerIndexForCurrentZoom(cziSceneInfo,
                                                               frameIndex,
                                                               allFramesFlag,
                                                               transform);
    if (zoomLayerIndex != m_previousZoomLayerIndex) {
        reloadImageFlag = true;
        std::cout << "Reload image due to zoom resolution change index=" << zoomLayerIndex << std::endl;
    }
    m_previousZoomLayerIndex = zoomLayerIndex;

    if ( ! reloadImageFlag) {
        reloadImageFlag = isReloadForPanning(cziImage,
                                             transform);
        if (reloadImageFlag) {
            std::cout << "Reload image due to panning" << std::endl;
        }
    }
    
    if (reloadImageFlag) {
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
    m_previousFrameIndex    = frameIndex;
    m_previousAllFramesFlag = allFramesFlag;
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
    const float imageBottomLeftPixel[3] { 0.0, static_cast<float>(m_cziImageFile->m_fullResolutionLogicalRect.height()), 0.0 };
    const float imageTopLeftPixel[3] { 0.0, 0.0, 0.0 };
    float imageBottomLeftWindow[3];
    float imageTopLeftWindow[3];
    transform->transformPoint(imageBottomLeftPixel, imageBottomLeftWindow);
    transform->transformPoint(imageTopLeftPixel, imageTopLeftWindow);
    const float drawnPixelHeight = imageTopLeftWindow[1] - imageBottomLeftWindow[1];
    //std::cout << "Drawn pixel height: " << drawnPixelHeight << std::endl;
    
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
 * @return Does new image data need to be loaded due to panning?
 * @param transform
 *    Transforms from/to viewport and model coordinates
 */
bool
CziImageLoaderMultiResolution::isReloadForPanning(const CziImage* cziImage,
                                                  const GraphicsObjectToWindowTransform* transform) const
{
    bool reloadFlag(false);
    
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
     * 'transformPixelIndexToSpace' transforms pixel index to CZI 'logical coordinates'
     */
    float viewportTopLeftWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x(),
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     viewportTopLeftWindowCoordinate);
    const PixelLogicalIndex windowLogicalTopLeft(viewportTopLeftWindowCoordinate);
    
    /*
     * Bottom Right Corner of Window
     * 'inverseTransformPoint()' transforms from window coordinates to the ortho's pixel index with
     * origin at bottom left.
     * 'transformPixelIndexToSpace' transforms pixel index to CZI 'logical coordinates'
     */
    float viewportBottomRightWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x() + viewport.width(),
                                     viewport.y(),
                                     0.0,
                                     viewportBottomRightWindowCoordinate);
    const PixelLogicalIndex windowLogicalBottomRight(viewportBottomRightWindowCoordinate);
    /*
     * CZI Logical coordinates of viewport (portion of CZI image that fills the viewport)
     */
    const QRectF viewportFullResLogicalRect(windowLogicalTopLeft.getI(),
                                            windowLogicalTopLeft.getJ(),
                                            windowLogicalBottomRight.getI() - windowLogicalTopLeft.getI(),
                                            windowLogicalBottomRight.getJ() - windowLogicalTopLeft.getJ());
    if (cziDebugFlag) {
        std::cout << "Pixel Logical Index Top Left (origin top left): " << windowLogicalTopLeft.toString() << std::endl;

        std::cout << "Pixel Logical Index Bottom Right (origin top left): " << windowLogicalBottomRight.toString() << std::endl;

        std::cout << "Window Logical Rect: " << CziUtilities::qRectToString(viewportFullResLogicalRect) << std::endl;
        std::cout << "Full Res Logical Rect: " << CziUtilities::qRectToString(m_cziImageFile->m_fullResolutionLogicalRect) << std::endl;
        std::cout << "Image Logical Rect: " << CziUtilities::qRectToString(cziImage->m_imageDataLogicalRect) << std::endl;
        
        const QRectF imageIntersectWindowRect(viewportFullResLogicalRect.intersected(cziImage->m_imageDataLogicalRect));
        const float imageInWindowArea(imageIntersectWindowRect.width() * imageIntersectWindowRect.height());
        const float imageArea(cziImage->m_imageDataLogicalRect.width()* cziImage->m_imageDataLogicalRect.height());
        const float viewedPercentage((imageArea > 0.0f)
                                     ? (imageInWindowArea / imageArea)
                                     : imageArea);
        std::cout << "Image Viewed Percentage: " << viewedPercentage << std::endl;
    }
    
    /*
     * (1) Find intersection of currently loaded image region with the viewport region
     * (2) Find amount of viewport that overlaps the current image region
     */
    const QRectF viewportIntersectImageRect(cziImage->m_imageDataLogicalRect.intersected(viewportFullResLogicalRect));
    const float viewportInImageArea(viewportIntersectImageRect.width() * viewportIntersectImageRect.height());
    const float viewportArea(viewportFullResLogicalRect.width() * viewportFullResLogicalRect.height());
    const float viewportRoiPercentage((viewportArea > 0.0f)
                                      ? (viewportInImageArea / viewportArea)
                                      : viewportArea);
    if (cziDebugFlag) {
        std::cout << "Window Image Percentage: " << viewportRoiPercentage << std::endl;
    }
    
    /*
     * (1) Find intersection of full resolution image region with the viewport region
     * (2) Find amount of viewport that overlaps the full image region
     */
    const QRectF viewportIntersectFullImageRect(m_cziImageFile->m_fullResolutionLogicalRect.intersected(viewportFullResLogicalRect));
    const float viewportInFullResImageArea(viewportIntersectFullImageRect.width() * viewportIntersectFullImageRect.height());
    const float viewportFullResPercentage(viewportArea
                                          ? (viewportInFullResImageArea / viewportArea)
                                          : viewportArea);
    if (cziDebugFlag) {
        std::cout << "Window Full Res Image Percentage: " << viewportFullResPercentage << std::endl;
    }
    
    if (viewportFullResPercentage > 0.0) {
        /*
         * Get ratio of current image ROI and viewport full res image
         * When less that one, the current image has been panned so
         * that there is a gap on a side (or sides) of the viewport
         * that can be filled by loading new image data
         */
        const float ratio(viewportRoiPercentage / viewportFullResPercentage);
        if (cziDebugFlag) {
            std::cout << "Viewed vs Available Percentage: " << ratio << std::endl;
        }
        
        /*
         * If parts of the viewport do not contain image data but
         * there is image data available, reload image data to
         * cover the entire viewport.
         */
        const float reloadThreshold(0.99);
        if (ratio < reloadThreshold) {
            /*
             * Cause reloading of image data which should fill the window
             */
            if (cziDebugFlag) {
                std::cout << "...Reloading Image Data" << std::endl;
            }
            reloadFlag = true;
        }
    }
    if (cziDebugFlag) {
        std::cout << std::endl;
    }
    return reloadFlag;
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
    
    CziImage* cziImageOut(NULL);
    
    std::array<float,4> viewport(transform->getViewport());
    if ((viewport[2] <= 0)
        || (viewport[3] <= 0)) {
        CaretLogSevere("Viewport is invalid: "
                       + AString::fromNumbers(viewport.data(), 4, ", ")
                       + " for pyramid layer "
                       + AString::number(pyramidLayerIndex));
        return NULL;
    }
    const float vpCenterXYZ[3] {
        viewport[0] + (viewport[2] / 2.0f),
        viewport[1] + (viewport[3] / 2.0f),
        0.0
    };
    float modelXYZ[3];
    transform->inverseTransformPoint(vpCenterXYZ, modelXYZ);
    
    float modelXYZ2[3];
    GraphicsUtilitiesOpenGL::unproject(vpCenterXYZ[0], vpCenterXYZ[1], modelXYZ2);
    //    std::cout << "Model XYZ: " << AString::fromNumbers(modelXYZ, 3, "f") << std::endl;
    //    std::cout << "   Model 2 XYZ: " << AString::fromNumbers(modelXYZ2, 3, "f") << std::endl;
    
    CaretAssert(oldCziImage);
    
    const PixelLogicalIndex centerLogicalPixelIndex(modelXYZ[0], modelXYZ[1], 0.0f);
    
    const int32_t maxLogicalWidthHeight(std::max(pyramidLayer.m_logicalWidthForImageReading,
                                                 pyramidLayer.m_logicalHeightForImageReading));
    const int32_t halfLogicalWidthHeight(maxLogicalWidthHeight / 2);
    
    QRectF logicalRegion(centerLogicalPixelIndex.getI() - halfLogicalWidthHeight,
                         centerLogicalPixelIndex.getJ() - halfLogicalWidthHeight,
                         maxLogicalWidthHeight,
                         maxLogicalWidthHeight);
    std::cout << "Load logical region: " << CziUtilities::qRectToString(logicalRegion) << std::endl;

    /*
     * Limit region for reading to valid area of image
     */
    if (cziSceneInfo.m_logicalRectangle.intersects(logicalRegion)) {
        logicalRegion = cziSceneInfo.m_logicalRectangle.intersected(logicalRegion);
    }
    else {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(logicalRegion)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " does not overlap the full resolution rectangle="
                       + CziUtilities::qRectToString(cziSceneInfo.m_logicalRectangle));
        return NULL;
    }

    /*
     * May need to move or clip to stay in the logical space
     */
    QRectF adjustedRect = moveAndClipRectangle(cziSceneInfo.m_logicalRectangle,
                                                logicalRegion);
    std::cout << "Clipped logical region: " << CziUtilities::qRectToString(adjustedRect) << std::endl;

    /*
     * If the region has not changed, do not need to load data
     */
    if (oldCziImage->m_imageDataLogicalRect == adjustedRect) {
        return NULL;
    }
    
    AString errorMessage;
    cziImageOut = m_cziImageFile->readFromCziImageFile(adjustedRect,
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
                       + CziUtilities::qRectToString(adjustedRect)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " error: "
                       + errorMessage);
    }
    else {
        std::cout << "Image Pixels width=" << cziImageOut->getWidth() << ", " << cziImageOut->getHeight() << std::endl;
    }

    return cziImageOut;
}
