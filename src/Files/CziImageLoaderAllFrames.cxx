
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

#define __CZI_IMAGE_LOADER_ALL_FRAMES_DECLARE__
#include "CziImageLoaderAllFrames.h"
#undef __CZI_IMAGE_LOADER_ALL_FRAMES_DECLARE__

#include <algorithm>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CziImage.h"
#include "CziImageFile.h"
#include "CziUtilities.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsUtilitiesOpenGL.h"

using namespace caret;

static const bool cziDebugFlag(true);
    
/**
 * \class caret::CziImageLoaderAllFrames 
 * \brief Loads image data for all frames in a CZI Image File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziImageLoaderAllFrames::CziImageLoaderAllFrames()
: CziImageLoaderBase()
{
    
}

/**
 * Destructor.
 */
CziImageLoaderAllFrames::~CziImageLoaderAllFrames()
{
}

/**
 * Initialize this instance with the given CZI Image File
 * @param cziImageFile
 *     CZI image file whose data is load by this instance
 */
void
CziImageLoaderAllFrames::initialize(CziImageFile* cziImageFile)
{
    m_cziImageFile = cziImageFile;
    
    m_fullResolutionLogicalRect = cziImageFile->m_fullResolutionLogicalRect;
    
    int32_t width(m_fullResolutionLogicalRect.width());
    int32_t height(m_fullResolutionLogicalRect.height());
    const int32_t dimStart = std::min(width,
                                      height);
    const int32_t minSize(512);
    
    float pixelScale(1.0);
    int32_t dim(dimStart);
    while (dim >= minSize) {
        m_fullResolutionLayers.push_back(FullResolutionLayer(width, height, pixelScale));
        dim /= 2;
        width /= 2;
        height /= 2;
        pixelScale *= 2;
    }
    
//    std::reverse(m_fullResolutionLayers.begin(),
//                 m_fullResolutionLayers.end());
    m_numberOfFullResolutionLayers = static_cast<int32_t>(m_fullResolutionLayers.size());
    
    for (int32_t i = 0; i < m_numberOfFullResolutionLayers; i++) {
        CaretAssertVectorIndex(m_fullResolutionLayers, i);
        const FullResolutionLayer& frl(m_fullResolutionLayers[i]);
        std::cout << "Full layer " << i << ": w=" << frl.m_width << ", h=" << frl.m_height
        << " pixel scale=" << frl.m_pixelScale << std::endl;
    }
    
    const bool testLayerSelectionFlag(false);
    if (testLayerSelectionFlag) {
        std::cout << "--- BEGIN TESTING HEIGHT TO INDEX" << std::endl;
        for (int32_t iHeight = 100; iHeight <= 60000; iHeight += 100) {
            getLayerIndexForHeight(iHeight);
        }
        std::cout << "--- END TESTING HEIGHT TO INDEX" << std::endl;
    }
}

/**
 * Possible load new image data
 * @param cziImageFile
 *    CZI image file whose data is load by this instance
 * @param tabIndex
 *    Index of tab
 * @param overlayIndex
 *    Index of overlay
 * @param frameIndex
 *    Index of frame
 * @param allFramesFlag
 *    True if all frames are selected for display
 * @param transform
 *    Transforms from/to viewport and model coordinates
 * @return non-NULL if new image data was loaded. else NULL and continue
 *    using previously loaded data
 */
CziImage*
CziImageLoaderAllFrames::loadNewData(const CziImage* cziImage,
                              const int32_t tabIndex,
                              const int32_t overlayIndex,
                              const int32_t frameIndex,
                              const bool allFramesFlag,
                              const GraphicsObjectToWindowTransform* transform)
{
    /*
     * Compute pixel height of the high-resolution image when drawn in the window.
     * The size of the image, in pixels when drawn, determines when to switch to
     * lower or higher resolution image.
     */
    const std::array<float, 4> viewport(transform->getViewport());
    std::array<float, 3> viewportBottomLeft {
        viewport[0],
        viewport[1] + viewport[3],
        0.0
    };
    std::array<float, 3> logicalBottomLeft;
    transform->inverseTransformPoint(viewportBottomLeft.data(),
                                     logicalBottomLeft.data());
    
    std::array<float, 3> viewportTopLeft {
        viewport[0],
        viewport[1],
        0.0
    };
    std::array<float, 3> logicalTopLeft;
    transform->inverseTransformPoint(viewportTopLeft.data(),
                                     logicalTopLeft.data());

    const float viewportLogicalHeight(logicalTopLeft[1] - logicalBottomLeft[1]);
    
    const int32_t bestLayerIndex(getLayerIndexForHeight(viewportLogicalHeight));
    std::cout << "Viewport Layer Index: " << bestLayerIndex << " for height " << viewportLogicalHeight << std::endl;
    
    int32_t currentLayerIndex(-1);
    if (cziImage != NULL) {
        currentLayerIndex = 0;
        if (cziImage->m_resolutionChangeMode == CziImageResolutionChangeModeEnum::AUTO2) {
            currentLayerIndex = cziImage->m_resolutionChangeModeLevel;
        }
    }
    std::cout << "   Current layer index: " << currentLayerIndex << std::endl;
//    const int32_t currentLayerIndex((cziImage != NULL)
//                                    ? getLayerIndexForHeight(cziImage->m_imageDataLogicalRect.height())
//                                    : -1);
//    std::cout << "Current Layer Index: " << currentLayerIndex << " for height " << cziImage->m_imageDataLogicalRect.height() << std::endl;

    
    CziImage* newCziImage(NULL);
    if (bestLayerIndex > 0) {
        if (bestLayerIndex != currentLayerIndex) {
            newCziImage = loadImageForLayer(cziImage,
                                            tabIndex,
                                            overlayIndex,
                                            transform,
                                            bestLayerIndex);
        }
    }

    //    if (pyramidLayerForScaling != getPyramidLayerIndexForTabOverlay(tabIndex,
//                                                                    overlayIndex)) {
//        setPyramidLayerIndexForTab(tabIndex,
//                                   pyramidLayerForScaling);
//    }
//
//    return pyramidLayerForScaling;
    return newCziImage;
    
}

/**
 * Load a image from the given full resolution layer for the center of the tab region defined by the transform
 * @param oldCziImage
 *    CZI image being replaced
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of the overlay
 * @param transform
 *    Transform from the tab where image is drawn
 * @param fullResolutionLayerIndexIn
 *    Index of the full resoluiton layer
 */
CziImage*
CziImageLoaderAllFrames::loadImageForLayer(const CziImage* oldCziImage,
                                           const int32_t tabIndex,
                                           const int32_t overlayIndex,
                                           const GraphicsObjectToWindowTransform* transform,
                                           const int32_t fullResolutionLayerIndexIn)
{
    if (m_fullResolutionLayers.empty()) {
        CaretLogSevere("Attempting to load full resolution layer="
                       + AString::number(fullResolutionLayerIndexIn)
                       + " but no full resolution layers available.");
    }
    
    int32_t fullResolutionLayerIndex(fullResolutionLayerIndexIn);
    if (fullResolutionLayerIndex < 0) {
        fullResolutionLayerIndex = 0;
        CaretLogSevere("Attempt to load invalid full resolution layer="
                       + AString::number(fullResolutionLayerIndexIn));
    }
    else if (fullResolutionLayerIndex >= static_cast<int32_t>(m_fullResolutionLayers.size())) {
        fullResolutionLayerIndex = m_fullResolutionLayers.size() - 1;
        CaretLogSevere("Attempt to load invalid full resolution layer="
                       + AString::number(fullResolutionLayerIndexIn));
    }
    
    CziImage* cziImageOut(NULL);
    
    std::array<float,4> viewport(transform->getViewport());
    if ((viewport[2] <= 0)
        || (viewport[3] <= 0)) {
        CaretLogSevere("Viewport is invalid: "
                       + AString::fromNumbers(viewport.data(), 4, ", ")
                       + " for full resolution layer "
                       + AString::number(fullResolutionLayerIndex));
        return NULL;
    }
    const float vpCenterXYZ[3] {
        viewport[0] + (viewport[2] / 2.0f),
        viewport[1] + (viewport[3] / 2.0f),
        0.0
    };
    float modelXYZ[3];
    transform->inverseTransformPoint(vpCenterXYZ, modelXYZ);
    PixelIndex imagePixelIndex(modelXYZ[0], modelXYZ[1], 0.0f);
    
    float modelXYZ2[3];
    GraphicsUtilitiesOpenGL::unproject(vpCenterXYZ[0], vpCenterXYZ[1], modelXYZ2);
    std::cout << "Model XYZ: " << AString::fromNumbers(modelXYZ, 3, ", ") << std::endl;
    std::cout << "   Model 2 XYZ: " << AString::fromNumbers(modelXYZ2, 3, ", ") << std::endl;
    
    CaretAssert(oldCziImage);
    PixelIndex fullImagePixelIndex = imagePixelIndex;
    PixelIndex fullResolutionLogicalPixelIndex = imagePixelIndex;
    
    /*
     * Get preferred image size from preferences
     */
    const int32_t preferredImageDimension(m_cziImageFile->getPreferencesImageDimension());
    //    EventCaretPreferencesGet prefsEvent;
    //    EventManager::get()->sendEvent(prefsEvent.getPointer());
    //    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    //    int32_t preferredImageDimension(2048);
    //    if (prefs != NULL) {
    //        preferredImageDimension = prefs->getCziDimension();
    //    }
    //    CaretAssert(preferredImageDimension >= 512);
    
    /*
     * Aspect ratio
     *  "2"   when height is double width
     *  "1"   when width equals height
     *  "0.5" when width is double height
     *
     * So, when aspect is:
     *  "> 1.33"  use a vertical rectangle;
     *  "< 0.75" use a horizontal rectangle;
     *  Otherwise, a square;
     * Note: (1 / 0.75) = 1.33
     *
     * Textures should be powers of 2 for performance reasons.
     *
     */
    const float viewportAspectRatio(viewport[3] / viewport[2]);
    CaretAssert(viewportAspectRatio > 0.0f);
    int32_t loadImageWidth(preferredImageDimension);
    int32_t loadImageHeight(preferredImageDimension);
    if (viewportAspectRatio > 1.333f) {
        loadImageHeight *= 2;
    }
    else if (viewportAspectRatio < 0.75) {
        loadImageWidth *= 2;
    }
    
    /*
     * Limit width/height to maximum texture dimension supported
     * on this computer by OpenGL
     */
    const int32_t maximumTextureDimension(GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension());
    loadImageWidth = std::min(loadImageWidth,
                              maximumTextureDimension);
    loadImageHeight = std::min(loadImageHeight,
                               maximumTextureDimension);
    
    /*
     * Create rectangle in logical coordinates
     */
    const float imageLogicalWidth(m_fullResolutionLayers[fullResolutionLayerIndex].m_width);
    const float imageLogicalHeight(m_fullResolutionLayers[fullResolutionLayerIndex].m_height);
    const int32_t halfLogicalImageWidth(loadImageWidth / 2.0);
    const int32_t halfLogicalImageHeight(loadImageHeight / 2.0);
    QRectF imageRegionRect(fullResolutionLogicalPixelIndex.getI() - halfLogicalImageWidth,
                           fullResolutionLogicalPixelIndex.getJ() - halfLogicalImageHeight,
                           imageLogicalWidth,
                           imageLogicalHeight);
    if (cziDebugFlag) {
        std::cout << "Loading logical width/height: " << imageLogicalWidth << ", " << imageLogicalHeight << " for aspect: " << viewportAspectRatio << std::endl;
    }
    
    /*
     * Limit region for reading to valid area of image
     */
    if (m_fullResolutionLogicalRect.intersects(imageRegionRect)) {
        imageRegionRect = m_fullResolutionLogicalRect.intersected(imageRegionRect);
    }
    else {
        CaretLogSevere("Loading layer="
                       + AString::number(fullResolutionLayerIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(imageRegionRect)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " does not overlap the full resolution rectangle="
                       + CziUtilities::qRectToString(m_fullResolutionLogicalRect));
        return NULL;
    }
    
    /*
     * When reading, the size is a "logical size" not PIXELS.  So, need to convert
     * the pixel size to a logical size
     */
//    const int32_t centerX = imageRegionRect.center().rx();
//    const int32_t centerY = imageRegionRect.center().ry();
//    int32_t roiWidth = imageRegionRect.width();
//    int32_t roiHeight = imageRegionRect.height();
//    pixelSizeToLogicalSize(fullResolutionLayerIndex, roiWidth, roiHeight);
    QRectF adjustedRect(imageRegionRect);
//    adjustedRect.setX(centerX - (roiWidth / 2));
//    adjustedRect.setY(centerY - (roiHeight / 2));
//    adjustedRect.setWidth(roiWidth);
//    adjustedRect.setHeight(roiHeight);
    
    /*
     * May need to move or clip to stay in the logical space
     */
    adjustedRect =  m_cziImageFile->moveAndClipRectangle(adjustedRect);
    if (cziDebugFlag) {
        std::cout << "Adjusted logical width/height: " << adjustedRect.width() << ", " << adjustedRect.height() << std::endl;
        std::cout << "Original ROI: " << CziUtilities::qRectToString(imageRegionRect) << std::endl;
        std::cout << "   Adjusted for w/h: " << CziUtilities::qRectToString(adjustedRect) << std::endl;
        std::cout << "   Old Center: " << imageRegionRect.center().x() << ", " << imageRegionRect.center().y() << std::endl;
        std::cout << "   New Center: " << adjustedRect.center().x() << ", " << adjustedRect.center().y() << std::endl;
    }
    if ( ! adjustedRect.isValid()) {
        return NULL;
    }
    
    /*
     * If the region has not changed, do not need to load data
     */
    if (oldCziImage->m_imageDataLogicalRect == adjustedRect) {
        return NULL;
    }
    
    AString errorMessage;
    cziImageOut = m_cziImageFile->readFromCziImageFile(adjustedRect,
                                                       preferredImageDimension,
                                                       CziImageResolutionChangeModeEnum::AUTO2,
                                                       fullResolutionLayerIndex,
                                                       errorMessage);
//    cziImageOut = readPyramidLayerFromCziImageFile(pyramidLayerIndex, imageRegionRect, adjustedRect, errorMessage);
    if (cziImageOut == NULL) {
        CaretLogSevere("Loading layer="
                       + AString::number(fullResolutionLayerIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(imageRegionRect)
                       + " for file "
                       + m_cziImageFile->getFileNameNoPath()
                       + " error: "
                       + errorMessage);
    }
    
//    CaretAssertStdArrayIndex(m_tabCziImagePyramidLevelChanged, tabIndex);
//    m_tabCziImagePyramidLevelChanged[tabIndex] = false;
    
    if (cziDebugFlag) {
        std::cout << std::endl << std::endl;
    }
    return cziImageOut;
}

/**
 * Convert a pixel size to a logical size for the given layer.
 * Reading images uses "logical width"
 *
 * @param layerIndex
 *    Index of full resolution layer
 * @param widthInOut
 *    Width (input pixels; output logical)
 * @param heightInOut
 *    Height (input pixels; output logical)
 */
void
CziImageLoaderAllFrames::pixelSizeToLogicalSize(const int32_t layerIndex,
                                                int32_t& widthInOut,
                                                int32_t& heightInOut) const
{
    CaretAssertVectorIndex(m_fullResolutionLayers, layerIndex);
    //const int32_t pixelScale = CalcSizeOfPixelOnLayer0(m_pyramidLayers[pyramidLayerIndex].m_layerInfo);
    const float  pixelScale(m_fullResolutionLayers[layerIndex].m_pixelScale);
    widthInOut  *= pixelScale;
    heightInOut *= pixelScale;
    widthInOut  = std::min(widthInOut, static_cast<int32_t>(m_fullResolutionLogicalRect.width()));
    heightInOut = std::min(heightInOut, static_cast<int32_t>(m_fullResolutionLogicalRect.height()));
}

/**
 * @return The layer index appropriate for the given image height
 * @param logicalHeight
 *    The logical height
 */
int32_t
CziImageLoaderAllFrames::getLayerIndexForHeight(const int32_t logicalHeight) const
{
//    int32_t layerIndex(0);
    
//    for (int32_t i = 0;
//         i < m_numberOfFullResolutionLayers;
//         i++) {
//        if (logicalHeight >= m_fullResolutionLayers[i].m_height) {
//            layerIndex = i;
//            break;
//        }
//    }
    int32_t layerIndex(m_numberOfFullResolutionLayers - 1);
    for (int32_t i = (m_numberOfFullResolutionLayers - 1);
         i >= 1;
         i--) {
        if (logicalHeight > m_fullResolutionLayers[i].m_height) {
            layerIndex = i - 1;
//            break;
        }
    }

    return layerIndex;
}

