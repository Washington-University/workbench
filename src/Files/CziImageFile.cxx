
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

#define __CZI_IMAGE_FILE_DECLARE__
#include "CziImageFile.h"
#undef __CZI_IMAGE_FILE_DECLARE__

#include <cmath>
#include <limits>

#include <QImage>

#include "BackgroundAndForegroundColors.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CziImage.h"
#include "CziImageLoaderMultiResolution.h"
#include "CziUtilities.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "DescriptiveStatistics.h"
#include "EventCaretPreferencesGet.h"
#include "EventBrowserTabClose.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabNewClone.h"
#include "EventManager.h"
#include "EventResetView.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "MathFunctions.h"
#include "Plane.h"
#include "RectangleTransform.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "StringTableModel.h"
#include "VolumeFile.h"

using namespace caret;

static bool cziDebugFlag(false);

/**
 * \class caret::CziImageFile
 * \brief A Zeiss CZI image file
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziImageFile::CziImageFile()
: MediaFile(DataFileTypeEnum::CZI_IMAGE_FILE)
{
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        for (int32_t iOverlay = 0; iOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; iOverlay++) {
            m_tabOverlayInfo[iTab][iOverlay].reset(new TabOverlayInfo());
        }
    }

    resetPrivate();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_pyramidLayerIndexInTabs",
                               m_pyramidLayerIndexInTabs.data(),
                               m_pyramidLayerIndexInTabs.size(),
                               0);

    /* NEED THIS AFTER Tile Tabs have been modified */
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_CLOSE);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW_CLONE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_RESET_VIEW);
}

/**
 * Destructor.
 */
CziImageFile::~CziImageFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Reset before file reading.
 */
void
CziImageFile::resetPrivate()
{
    m_status = Status::CLOSED;
    
    m_errorMessage.clear();
    
    m_stream.reset();
    
    m_allFramesPyramidInfo = CziSceneInfo();
    m_cziScenePyramidInfos.clear();
    
    m_reader.reset();
    m_scalingTileAccessor.reset();
    m_pyramidLayerTileAccessor.reset();
    m_lowestResolutionPyramidLayerIndex = -1;
    m_highestResolutionPyramidLayerIndex = -1;
    m_numberOfPyramidLayers = 0;
    m_pixelSizeMmX = 1.0f;
    m_pixelSizeMmY = 1.0f;
    m_pixelSizeMmZ = 1.0f;
    m_fileMetaData.reset(new GiftiMetaData());
//    m_defaultAllFramesImage.reset();
    m_defaultFrameImages.clear();
    m_fullResolutionLogicalRect = QRectF();
    m_pyramidLayers.clear();
    m_pixelToStereotaxicTransform = NiftiTransform();
    m_stereotaxicToPixelTransform = NiftiTransform();
    m_imagePlane.reset();
    m_imagePlaneInvalid = false;
    
    m_pyramidLayerIndexInTabs.fill(0);
    m_tabCziImagePyramidLevelChanged.fill(false);
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        for (int32_t iOverlay = 0; iOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; iOverlay++) {
            m_tabOverlayInfo[iTab][iOverlay]->resetContent();
            m_imageLoaderMultiResolution[iTab][iOverlay].reset();
        }
    }
}

/**
 * Close the file
 */
void
CziImageFile::closeFile()
{
    switch (m_status) {
        case Status::CLOSED:
            return;
            break;
        case Status::ERRORED:
            return;
            break;
        case Status::OPEN:
            break;
    }
    m_reader->Close();

    /*
     * Desctructors will close the files
     */
    m_scalingTileAccessor.reset();
    m_reader.reset();
    m_stream.reset();
    
    
    /*
     * File is now closed
     */
    m_status = Status::CLOSED;
    
    m_fullResolutionLogicalRect = QRectF();
    
    /*
     * Remove all images
     */
    for (auto& img : m_tabCziImages) {
        img.reset();
    }
//    m_defaultAllFramesImage.reset();
}

/**
 * @return File casted to a CZI image file (avoids use of dynamic_cast that can be slow)
 */
CziImageFile*
CziImageFile::castToCziImageFile()
{
    return this;
}

/**
 * @return File casted to an image file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const CziImageFile*
CziImageFile::castToCziImageFile() const
{
    return this;
}

/**
 * Clear this file's modified status
 */
void
CziImageFile::clearModified()
{
    MediaFile::clearModified();
    m_fileMetaData->clearModified();
}

/**
 * @return True if this file is modified, else falsel
 */
bool
CziImageFile::isModified() const
{
    if (MediaFile::isModified()) {
        return true;
    }
    if (m_fileMetaData->isModified()) {
        return true;
    }
    return false;
}

/**
 * @return true if the file is is empty (image contains no pixels).
 */

bool
CziImageFile::isEmpty() const
{
    return (getDefaultAllFramesImage() == NULL);
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
CziImageFile::getFileMetaData()
{
    return m_fileMetaData.get();
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
CziImageFile::receiveEvent(Event* event)
{
    int32_t removeTabIndex(-1);
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_CLOSE) {
        EventBrowserTabClose* closeEvent = dynamic_cast<EventBrowserTabClose*>(event);
        CaretAssert(closeEvent);
        removeTabIndex = closeEvent->getBrowserTabIndex();
        closeEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_DELETE) {
        EventBrowserTabDelete* deleteEvent = dynamic_cast<EventBrowserTabDelete*>(event);
        CaretAssert(deleteEvent);
        removeTabIndex = deleteEvent->getBrowserTabIndex();
        deleteEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_TAB_NEW_CLONE) {
        EventBrowserTabNewClone* cloneTabEvent = dynamic_cast<EventBrowserTabNewClone*>(event);
        CaretAssert(cloneTabEvent);
        
        const int32_t cloneToTabIndex   = cloneTabEvent->getNewBrowserTabIndex();
        const int32_t cloneFromTabIndex = cloneTabEvent->getIndexOfBrowserTabThatWasCloned();
        CaretAssertVectorIndex(m_pyramidLayerIndexInTabs, cloneToTabIndex);
        CaretAssertVectorIndex(m_pyramidLayerIndexInTabs, cloneFromTabIndex);
        m_pyramidLayerIndexInTabs[cloneToTabIndex] = m_pyramidLayerIndexInTabs[cloneFromTabIndex];
        cloneTabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_RESET_VIEW) {
        EventResetView* resetViewEvent = dynamic_cast<EventResetView*>(event);
        CaretAssert(resetViewEvent);
        resetViewEvent->setEventProcessed();
        removeTabIndex = resetViewEvent->getTabIndex();
    }
    
    if (removeTabIndex >= 0) {
        CaretAssertVectorIndex(m_tabCziImages, removeTabIndex);
        m_tabCziImages[removeTabIndex].reset();
        for (int32_t jOverlay = 0; jOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; jOverlay++) {
            m_tabOverlayInfo[removeTabIndex][jOverlay]->resetContent();
        }
    }
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CziImageFile::getFileMetaData() const
{
    return m_fileMetaData.get();
}


/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
CziImageFile::readFile(const AString& filename)
{
    resetPrivate();
    
    setFileName(filename);
    
    switch (m_status) {
        case Status::CLOSED:
            break;
        case Status::ERRORED:
            return;
            break;
        case Status::OPEN:
            return;
            break;
    }
    
    try {
        /*
         * If file does not exist, a std::exception is thrown
         */
        m_stream = libCZI::CreateStreamFromFile(filename.toStdWString().c_str());
        if ( ! m_stream) {
            m_errorMessage = "Creating stream for reading CZI file failed.";
            m_status = Status::ERRORED;
            return;
        }
        
        m_reader = libCZI::CreateCZIReader();
        if ( ! m_reader) {
            m_errorMessage = "Creating reader for reading CZI file failed.";
            m_status = Status::ERRORED;
            return;
        }
        
        /*
         * Content of SubBlockStatistics from file:
         *
         (libCZI::SubBlockStatistics) $0 = {
         subBlockCount = 2116
         minMindex = 0
         maxMindex = 1351
         boundingBox = (x = -61920, y = 5391, w = 59229, h = 45513)
         boundingBoxLayer0Only = (x = -61915, y = 5391, w = 59224, h = 45501)
         dimBounds = {
         validDims = 18
         start = ([0] = 32696, [1] = 0, [2] = 32766, [3] = -469475896, [4] = 0, [5] = -1315783472, [6] = 32696, [7] = 1, [8] = 0)
         size = ([0] = -469475856, [1] = 1, [2] = 1, [3] = 0, [4] = 1, [5] = 0, [6] = -1367473608, [7] = 32696, [8] = -469475888)
         }
         sceneBoundingBoxes = size=1 {
         [0] = {
         first = 0
         second = {
         boundingBox = (x = -61920, y = 5391, w = 59229, h = 45513)
         boundingBoxLayer0 = (x = -61915, y = 5391, w = 59224, h = 45501)
         }
         }
         }
         }
         
         */
        m_reader->Open(m_stream);
        
        /*
         * Statistics (bounding box of image)
         */
        libCZI::SubBlockStatistics subBlockStatistics = m_reader->GetStatistics();
        m_fullResolutionLogicalRect = CziUtilities::intRectToQRect(subBlockStatistics.boundingBox);
        
        
        
        readMetaData();
        
        /*
         * Create pyramid info for all frames
         */
        createAllFramesPyramidInfo(subBlockStatistics);
        
        /*
         * Pyramid Information
         */
        readPyramidInfo(subBlockStatistics);
        
        
        m_pyramidLayerTileAccessor = m_reader->CreateSingleChannelPyramidLayerTileAccessor();
        if ( ! m_pyramidLayerTileAccessor) {
            m_errorMessage = "Creating pyramid layer tile accessor for reading CZI file failed.";
            m_status = Status::ERRORED;
            return;
        }
        
        
        m_scalingTileAccessor = m_reader->CreateSingleChannelScalingTileAccessor();
        if ( ! m_scalingTileAccessor) {
            m_errorMessage = "Creating single channel scaling tile accessor for reading CZI file failed.";
            m_status = Status::ERRORED;
            return;
        }

        CziImage* defImage(NULL);
        const bool readPyramidFlag(false);
        if (readPyramidFlag
            && (m_numberOfPyramidLayers > 2)) {
            /*
             * Maximum resolution for default image is no more than 1/2 maximum texture dimension
             */
            const int32_t resolution(GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension() / 2);
            if (cziDebugFlag) {
                std::cout << "For CZI Image, seeking resolution " << resolution << std::endl;
            }
            
            /*
             * Index of pyramid level that is no more than 1/2 maximum texture dimension
             */
            const int32_t defaultPyramidIndex(getPyramidLayerWithMaximumResolution(resolution));
            if (defaultPyramidIndex >= 0) {
                defImage = readPyramidLayerFromCziImageFile(defaultPyramidIndex,
                                                            m_fullResolutionLogicalRect,
                                                            m_fullResolutionLogicalRect,
                                                            m_errorMessage);
                if (defImage == NULL) {
                    std::cout << "PYRAMID ERROR: " << m_errorMessage << std::endl;
                    m_errorMessage.clear();
                }
                else {
                    /*
                     * Range of pyramid layers for low-res to high-res
                     */
                    m_lowestResolutionPyramidLayerIndex  = defaultPyramidIndex;
                    m_highestResolutionPyramidLayerIndex = m_numberOfPyramidLayers - 1;
                    
                    /*
                     * For the highest resolution pyramid layer, the CZI library has the
                     * minification factor set to zero.  If one tries to load this pyramid
                     * layer with minification factor of zero, the CZI library goes into
                     * an infinite loop in CSingleChannelPyramidLevelTileAccessor::CalcPyramidLayerNo
                     * at " if (f >= minFactorInt)" because 'f' will be zero and 'minFactorInt' is 2.
                     *
                     * So, replacing the minification factor with 2 seems to prevent the infinite loop.
                     */
                    CaretAssertVectorIndex(m_pyramidLayers, m_highestResolutionPyramidLayerIndex);
                    if (m_pyramidLayers[m_highestResolutionPyramidLayerIndex].m_layerInfo.minificationFactor == 0) {
                        m_pyramidLayers[m_highestResolutionPyramidLayerIndex].m_layerInfo.minificationFactor = 2;
                    }
                    CaretAssert(m_lowestResolutionPyramidLayerIndex <= m_highestResolutionPyramidLayerIndex);
                    
                    /*
                     * Set level of zoom that corresponds to each of the pyramid layers
                     * and level of zoom used for switching to next layer in auto mode
                     */
                    float zoomFromLowRes(1.0);
                    for (int32_t i = m_lowestResolutionPyramidLayerIndex;
                         i <= m_highestResolutionPyramidLayerIndex;
                         i++) {
                        CaretAssertVectorIndex(m_pyramidLayers, i);
                        m_pyramidLayers[i].m_zoomLevelFromLowestResolutionImage = zoomFromLowRes;
                        zoomFromLowRes *= (m_pyramidLayers[i+1].m_layerInfo.minificationFactor);
                    }
                    
                    if (cziDebugFlag) {
                        std::cout << "Pyramid Range: " << m_lowestResolutionPyramidLayerIndex
                        << ", " << m_highestResolutionPyramidLayerIndex << std::endl;
                        std::cout << "Default pyramid index: " << defaultPyramidIndex << std::endl;
                    }
                }
            }
            m_pyramidLayerIndexInTabs.fill(defaultPyramidIndex);
        }
        
        if (defImage == NULL) {
            
//            m_defaultAllFramesImage.reset(readDefaultImage());
//
//            defImage = readFromCziImageFile(m_fullResolutionLogicalRect,
//                                            getPreferencesImageDimension(),
//                                            CziImageResolutionChangeModeEnum::INVALID, /* use INVALID for default image */
//                                            0, /* level index (value not used for default image) */
//                                            m_errorMessage);
        }
        if ( m_allFramesPyramidInfo.getDefaultImage() == NULL) {
//        if ( ! m_defaultAllFramesImage) {
            m_status = Status::ERRORED;
            return;
        }
        
        /*
         * File is now open
         */
        m_status = Status::OPEN;
        
        clearModified();
    }
    catch (const std::out_of_range& e) {
        m_errorMessage = ("std::out_of_range " + filename + ": " + QString(e.what()));
        m_status = Status::ERRORED;
        throw DataFileException(filename,
                                ("std::out_of_range exception: "
                                 + QString(e.what())));
    }
    catch (const std::exception& e) {
        m_errorMessage = ("std::exception " + filename + ": " + QString(e.what()));
        m_status = Status::ERRORED;
        throw DataFileException(filename,
                                ("std::exception: "
                                 + QString(e.what())));
    }
}

/**
 * @return The full resolution logical rectangle
 */
QRectF
CziImageFile::getFullResolutionLogicalRect() const
{
    return m_fullResolutionLogicalRect;
}


/**
 * Read the default image.
 * @throw DataFileException if there is an error
 */
CziImage*
CziImageFile::readDefaultImage()
{
    AString errorMessage;
    CziImage* cziImage = readFromCziImageFile(m_fullResolutionLogicalRect,
                                              getPreferencesImageDimension(),
                                              CziImageResolutionChangeModeEnum::INVALID, /* use INVALID for default image */
                                              0, /* level index (value not used for default image) */
                                              errorMessage);
    if (cziImage == NULL) {
        throw DataFileException("Reading default image: "
                                + errorMessage);
    }
    
    return cziImage;
}

/**
 * Read metadata from the file
 */
void
CziImageFile::readMetaData()
{
    std::shared_ptr<libCZI::IMetadataSegment> metadataSegment(m_reader->ReadMetadataSegment());
    if (metadataSegment) {
        std::shared_ptr<libCZI::ICziMetadata> metadata(metadataSegment->CreateMetaFromMetadataSegment());
        if (metadata) {
            std::shared_ptr<libCZI::ICziMultiDimensionDocumentInfo> docInfo(metadata->GetDocumentInfo());
            if (docInfo) {
                const libCZI::GeneralDocumentInfo genDocInfo(docInfo->GetGeneralDocumentInfo());
                addToMetadataIfNotEmpty("Name", QString::fromStdWString(genDocInfo.name));
                addToMetadataIfNotEmpty("Title", QString::fromStdWString(genDocInfo.title));
                addToMetadataIfNotEmpty("Username", QString::fromStdWString(genDocInfo.userName));
                addToMetadataIfNotEmpty("Description", QString::fromStdWString(genDocInfo.description));
                addToMetadataIfNotEmpty("Comment", QString::fromStdWString(genDocInfo.comment));
                addToMetadataIfNotEmpty("Keywords", QString::fromStdWString(genDocInfo.keywords));
                addToMetadataIfNotEmpty("Creation Date", QString::fromStdWString(genDocInfo.creationDateTime));
                
                /*
                 * Scaling is in meters so convert to millimeters
                 */
                const libCZI::ScalingInfo scalingInfo(docInfo->GetScalingInfo());
                m_pixelSizeMmX = scalingInfo.scaleX * 1000.0;
                m_pixelSizeMmY = scalingInfo.scaleY * 1000.0;
                m_pixelSizeMmZ = scalingInfo.scaleZ * 1000.0;
            }
        }
    }
}

/**
 * @return Size of pixel in millimeters for X, Y, and Z dimensions
 */
PixelCoordinate
CziImageFile::getPixelSizeInMillimeters() const
{
    return PixelCoordinate(m_pixelSizeMmX,
                           m_pixelSizeMmY,
                           m_pixelSizeMmZ);
}

/**
 * Create the "all frames" pyramid info
 * @param subBlockStatistics
 *    The sub block statistics
 */
void
CziImageFile::createAllFramesPyramidInfo(const libCZI::SubBlockStatistics& subBlockStatistics)
{
    auto overallBoundingBox = subBlockStatistics.boundingBox;
    if (cziDebugFlag) {
        std::cout << "Overall bounding box: " << CziUtilities::intRectToString(overallBoundingBox) << std::endl;
    }

    int32_t width(overallBoundingBox.w);
    int32_t height(overallBoundingBox.h);
    
    if ((width <= 0)
        || (height <= 0)) {
        throw DataFileException("Bounding box has invalid width or height (zero)");
    }
    
    m_allFramesPyramidInfo = CziSceneInfo(this,
                                          s_allFramesIndex,
                                          CziUtilities::intRectToQRect(overallBoundingBox));

    int32_t pyramidLayerNumber(0);
    const int32_t minDim(400);
    while ((width > minDim)
           && (height > minDim)) {
        libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo pyramidInfo;
        pyramidInfo.minificationFactor = 2;
        pyramidInfo.pyramidLayerNo     = pyramidLayerNumber;
        
        PyramidLayer pyramidLayer(s_allFramesIndex,
                                  pyramidInfo,
                                  width,
                                  height);
        m_pyramidLayers.push_back(pyramidLayer);
        
        m_allFramesPyramidInfo.addPyramidLayer(pyramidLayer);
        width /= 2;
        height /= 2;
        pyramidLayerNumber++;
    }
    
    std::sort(m_allFramesPyramidInfo.m_pyramidLayers.begin(),
              m_allFramesPyramidInfo.m_pyramidLayers.end(),
              [=](PyramidLayer a, PyramidLayer b) { return (a.m_layerInfo.pyramidLayerNo > b.m_layerInfo.pyramidLayerNo); } );
    
    setLayersZoomFactors(m_allFramesPyramidInfo);
}

/**
 * Set the zoom factors for layers
 * @param
 * Scene info that set zoom is set for
 */
void
CziImageFile::setLayersZoomFactors(CziSceneInfo& cziSceneInfo)
{
    const int64_t widthImageLoad(cziSceneInfo.m_logicalRectangle.width());
    const int64_t heightImageLoad(cziSceneInfo.m_logicalRectangle.height());

    const int32_t preferredDim(getPreferencesImageDimension());
    int32_t zoomFactor(1);
    for (auto& layer : cziSceneInfo.m_pyramidLayers) {
        layer.m_zoomLevelFromLowestResolutionImage = zoomFactor;
        layer.m_logicalWidthForImageReading = widthImageLoad / zoomFactor;
        layer.m_logicalHeightForImageReading = heightImageLoad/ zoomFactor;
        if ((layer.m_pixelWidth >= preferredDim)
            || (layer.m_pixelHeight >= preferredDim)) {
            zoomFactor *= 2;
        }
    }
}

/**
 * Read pyramid infor for each scene from the file
 * @param subBlockStatistics
 *    The sub block statistics
 */
void
CziImageFile::readPyramidInfo(const libCZI::SubBlockStatistics& subBlockStatistics)
{
    m_numberOfPyramidLayers = 0;
//    auto overallBoundingBox = subBlockStatistics.boundingBox;
//    if (cziDebugFlag) {
//        std::cout << "Overall bounding box: " << CziUtilities::intRectToString(overallBoundingBox) << std::endl;
//    }
    libCZI::PyramidStatistics pyramidStatistics = m_reader->GetPyramidStatistics();
    const int32_t numberOfScenes(pyramidStatistics.scenePyramidStatistics.size());
    
    for (int32_t iScene = 0; iScene < numberOfScenes; iScene++) {
        auto subBlockSceneIter = subBlockStatistics.sceneBoundingBoxes.find(iScene);
        if (subBlockSceneIter == subBlockStatistics.sceneBoundingBoxes.end()) {
            throw DataFileException("Unable to fine scene bounding box for scene index="
                                    + AString::number(iScene));
        }

        const libCZI::BoundingBoxes& boundingBoxes = subBlockSceneIter->second;
        m_cziScenePyramidInfos.push_back(CziSceneInfo(this,
                                                      iScene,
                                                      CziUtilities::intRectToQRect(boundingBoxes.boundingBox)));
    }
    
    for (auto& sceneIter : pyramidStatistics.scenePyramidStatistics) {
        const int32_t sceneIndex(sceneIter.first);
        if ((sceneIndex < 0)
            || (sceneIndex >= numberOfScenes)) {
            throw DataFileException("Scene Index="
                                    + AString::number(sceneIndex)
                                    + " out of range [0, "
                                    + AString::number(numberOfScenes - 1)
                                    + "]");
        }
        auto subBlockSceneIter = subBlockStatistics.sceneBoundingBoxes.find(sceneIndex);
        if (subBlockSceneIter == subBlockStatistics.sceneBoundingBoxes.end()) {
            throw DataFileException("Unable to fine scene bounding box for scene index="
                                    + AString::number(sceneIndex));
        }
        const libCZI::BoundingBoxes& boundingBoxes = subBlockSceneIter->second;
        int64_t width(boundingBoxes.boundingBox.w);
        int64_t height(boundingBoxes.boundingBox.h);
        if (cziDebugFlag) {
            std::cout << "Scene " << sceneIndex << " Bounding Box: " << CziUtilities::intRectToString(boundingBoxes.boundingBox) << std::endl;
        }
        const std::vector<libCZI::PyramidStatistics::PyramidLayerStatistics>& pyrStat = sceneIter.second;
        for (auto& pls : pyrStat) {
            const libCZI::PyramidStatistics::PyramidLayerInfo& ply = pls.layerInfo;
            const int64_t minFactor(ply.minificationFactor);
            if (minFactor > 0) {
                width /= minFactor;
                height /= minFactor;
            }
            
            libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo pyramidInfo;
            pyramidInfo.minificationFactor = ply.minificationFactor;
            pyramidInfo.pyramidLayerNo     = ply.pyramidLayerNo;
            
            PyramidLayer pyramidLayer(sceneIndex,
                                      pyramidInfo,
                                      width,
                                      height);
            m_pyramidLayers.push_back(pyramidLayer);

            CaretAssertVectorIndex(m_cziScenePyramidInfos, sceneIndex);
            m_cziScenePyramidInfos[sceneIndex].addPyramidLayer(pyramidLayer);
            
            if (cziDebugFlag) {
                auto& pl = pyramidLayer;
                std::cout << "Scene Index=" << pl.m_sceneIndex << " CZI Pyramid Layer Number: " << (int)pl.m_layerInfo.pyramidLayerNo << " MinFactor: " << (int)pl.m_layerInfo.minificationFactor
                << " width=" << pl.m_pixelWidth << " height=" << pl.m_pixelHeight << std::endl;
            }
        }
        
        std::sort(m_pyramidLayers.begin(),
                  m_pyramidLayers.end(),
                  [=](PyramidLayer a, PyramidLayer b) { return (a.m_layerInfo.pyramidLayerNo > b.m_layerInfo.pyramidLayerNo); } );
        
        
    }
    
    for (auto& sceneInfo : m_cziScenePyramidInfos) {
        std::sort(sceneInfo.m_pyramidLayers.begin(),
                  sceneInfo.m_pyramidLayers.end(),
                  [=](PyramidLayer a, PyramidLayer b) { return (a.m_layerInfo.pyramidLayerNo > b.m_layerInfo.pyramidLayerNo); } );
        
        
        const int32_t numPyramidLayers(sceneInfo.getNumberOfPyramidLayers());
        if (numPyramidLayers >= 1) {
            const int32_t lastIndex(numPyramidLayers - 1);
            CaretAssertVectorIndex(sceneInfo.m_pyramidLayers, lastIndex);
            
            /*
             * For the highest resolution pyramid layer, the CZI library has the
             * minification factor set to zero.  If one tries to load this pyramid
             * layer with minification factor of zero, the CZI library goes into
             * an infinite loop in CSingleChannelPyramidLevelTileAccessor::CalcPyramidLayerNo
             * at " if (f >= minFactorInt)" because 'f' will be zero and 'minFactorInt' is 2.
             *
             * So, replacing the minification factor with 2 seems to prevent the infinite loop.
             */
            if (sceneInfo.m_pyramidLayers[lastIndex].m_layerInfo.minificationFactor == 0) {
                sceneInfo.m_pyramidLayers[lastIndex].m_layerInfo.minificationFactor = 2;
            }
        }
        
        setLayersZoomFactors(sceneInfo);

    }
    
    m_numberOfPyramidLayers = static_cast<int32_t>(m_pyramidLayers.size());
    
    if (cziDebugFlag) {
        for (int32_t i = 0; i < m_numberOfPyramidLayers; i++) {
            const auto& pl = m_pyramidLayers[i];
            std::cout << "Index=" << i << " CZI Pyramid Layer Number: " << (int)pl.m_layerInfo.pyramidLayerNo << " MinFactor: " << (int)pl.m_layerInfo.minificationFactor
            << " width=" << pl.m_pixelWidth << " height=" << pl.m_pixelHeight << std::endl;
        }
    }
}


/**
 * Add to metadata if text is not empty
 * @param name
 *    Name of metadata item
 * @param text
 *    Text of metadfata
 */
void
CziImageFile::addToMetadataIfNotEmpty(const AString& name,
                                      const AString& text)
{
    const AString textTrimmed(text.trimmed());
    if (textTrimmed.isEmpty()) {
        return;
    }
    getFileMetaData()->set(name, text);
}


/**
 * Read the specified SCALED region from the CZI file into an image of the given width and height.
 * @param regionOfInterest
 *    Region of interest to read from file.  Origin is in top left.
 * @param outputImageWidthHeightMaximum
 *    Maximum width and height of output image
 * @param resolutionChangeMode
 *    Resolution change mode that created this image
 * @param resolutionChangeModeLevel
 *    Level from resolution change mode that created this image
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readFromCziImageFile(const QRectF& regionOfInterest,
                                   const int64_t outputImageWidthHeightMaximum,
                                   const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                   const int32_t resolutionChangeModeLevel,
                                   AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! regionOfInterest.isValid()) {
        errorMessageOut = "Region of interest for reading from file is invalid";
        return NULL;
    }
    
    libCZI::CDimCoordinate coordinate;
    coordinate.Set(libCZI::DimensionIndex::C, 0);
    
    const std::array<float, 3> prefBackRGB = getPreferencesImageBackgroundRGB();
    libCZI::ISingleChannelScalingTileAccessor::Options scstaOptions; scstaOptions.Clear();
    scstaOptions.backGroundColor.r = prefBackRGB[0];
    scstaOptions.backGroundColor.g = prefBackRGB[1];
    scstaOptions.backGroundColor.b = prefBackRGB[2];
    
    /*
     * Maximum of ROI width/height
     */
    const auto roiMaxWidthHeight(std::max(regionOfInterest.width(),
                                          regionOfInterest.height()));
    
    /*
     * If ROI width/height is greater than output image width/height,
     * use zoom to reduce the dimensions of the image data that is read
     */
    float zoom(1.0);
    if (roiMaxWidthHeight > outputImageWidthHeightMaximum) {
        zoom = static_cast<float>(outputImageWidthHeightMaximum) / static_cast<float>(roiMaxWidthHeight);
    }
    else {
        zoom = static_cast<float>(roiMaxWidthHeight) / static_cast<float>(outputImageWidthHeightMaximum);
    }
    
    /*
     * Read into 24 bit RGB to avoid conversion from other pixel formats
     */
    const libCZI::PixelType pixelType(libCZI::PixelType::Bgr24);
    const libCZI::IntRect intRectROI = CziUtilities::qRectToIntRect(regionOfInterest);
    CaretAssert(m_scalingTileAccessor);
    std::shared_ptr<libCZI::IBitmapData> bitmapData = m_scalingTileAccessor->Get(pixelType,
                                                                                 intRectROI,
                                                                                 &coordinate,
                                                                                 zoom,
                                                                                 &scstaOptions);
    if ( ! bitmapData) {
        errorMessageOut = ("Failed to read data for region "
                           + CziUtilities::intRectToString(intRectROI));
        return NULL;
    }
    
    QImage* qImage = createQImageFromBitmapData(bitmapData.get(),
                                                errorMessageOut);
    if (qImage == NULL) {
        return NULL;
    }
    
    CziImage* cziImageOut = new CziImage(this,
                                         qImage,
                                         m_fullResolutionLogicalRect,
                                         CziUtilities::intRectToQRect(intRectROI),
                                         resolutionChangeMode,
                                         resolutionChangeModeLevel);
    return cziImageOut;
}

/**
 * @return The pyramid layer for a tab.
 * @param tabIndex
 * Index of the tab
 * @param overlayIndex
 * Index of overlasy
 */
int32_t
CziImageFile::getPyramidLayerIndexForTabOverlay(const int32_t tabIndex,
                                                const int32_t overlayIndex) const
{
    return m_tabOverlayInfo[tabIndex][overlayIndex]->m_pyramidLevel;
    CaretAssertStdArrayIndex(m_pyramidLayerIndexInTabs, tabIndex);
    return m_pyramidLayerIndexInTabs[tabIndex];
}

/**
 * Set the pyramid layer for a tab.
 * @param tabIndex
 * Index of the tab
 * @param pyramidLayerIndex
 * New pyramid layer
 */
void
CziImageFile::setPyramidLayerIndexForTab(const int32_t tabIndex,
                                         const int32_t pyramidLayerIndex)
{
    CaretAssertStdArrayIndex(m_pyramidLayerIndexInTabs, tabIndex);
    
    int32_t minLayerIndex(0), maxLayerIndex(0);
    getPyramidLayerRange(minLayerIndex, maxLayerIndex);
    int32_t newPyramidLayerIndex = MathFunctions::limitRange(pyramidLayerIndex,
                                                             minLayerIndex,
                                                             maxLayerIndex);
    
    CaretAssertStdArrayIndex(m_pyramidLayerIndexInTabs, tabIndex);
    if (newPyramidLayerIndex != m_pyramidLayerIndexInTabs[tabIndex]) {
        m_pyramidLayerIndexInTabs[tabIndex] = newPyramidLayerIndex;
        if (newPyramidLayerIndex != m_lowestResolutionPyramidLayerIndex) {
            /*
             * Need image specific to tab
             */
            CaretAssertStdArrayIndex(m_tabCziImagePyramidLevelChanged, tabIndex);
            m_tabCziImagePyramidLevelChanged[tabIndex] = true;
        }
    }
}

/**
 * Reload the pyramid layer in the given tab.
 * @param tabIndex
 *    Index of the tab.
 */
void
CziImageFile::reloadPyramidLayerInTab(const int32_t tabIndex)
{
    if (m_pyramidLayerIndexInTabs[tabIndex] != m_lowestResolutionPyramidLayerIndex) {
        /*
         * Set level changed.  Cannot invalidate current image as it may
         * get used before new image is loaded.
         */
        CaretAssertStdArrayIndex(m_tabCziImagePyramidLevelChanged, tabIndex);
        m_tabCziImagePyramidLevelChanged[tabIndex] = true;
    }
}

/**
 * Get the range of available pyramid layers.  If range is invalid, layer indices will be -1.
 * @param lowestResolutionPyramidLayerIndexOut
 *    Output with lowest resolution layer index (
 * @param highestResolutionPyramidLayerIndexOut
 *    Output with highest resolution layer index
 */
void
CziImageFile::getPyramidLayerRange(int32_t& lowestResolutionPyramidLayerIndexOut,
                                   int32_t& highestResolutionPyramidLayerIndexOut) const
{
    lowestResolutionPyramidLayerIndexOut  = m_lowestResolutionPyramidLayerIndex;
    highestResolutionPyramidLayerIndexOut = m_highestResolutionPyramidLayerIndex;
}

/**
 * Find the pyramid layer that is closest to but less than or equal to the given resolution.
 * Both width and height must be less than or equal to the resolution.
 *
 * @param resolution
 *    Maximum resolution for pyramid layer
 * @return
 *    Index of pyramid layer that is closest but less than or equal to the given resolution.
 */
int32_t
CziImageFile::getPyramidLayerWithMaximumResolution(const int32_t resolution) const
{
    int32_t layerIndex = 0;
    
    for (int32_t i = 0; i < m_numberOfPyramidLayers; i++) {
        CaretAssertVectorIndex(m_pyramidLayers, i);
        const auto& layerInfo = m_pyramidLayers[i];
        if ((layerInfo.m_pixelWidth > resolution)
            || (layerInfo.m_pixelHeight > resolution)) {
            break;
        }
        layerIndex = i;
    }
    
    return layerIndex;
}

/**
 * Read a pyramid layer from CZI file
 * @param frameIndex
 *  Index of frame (scene)
 * @param pyramidLayer
 *    Index of pyramid layer to read
 * @param logicalRectangleRegionRect
 *    Rectangular region to read NOT REALLY USED ???
 * @param rectangleForReadingRect
 *    Rectangular region to read
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readFramePyramidLayerFromCziImageFile(const int32_t frameIndex,
                                                    const int32_t pyramidLayer,
                                                    const QRectF& logicalRectangleRegionRect,
                                                    const QRectF& rectangleForReadingRect,
                                                    AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    CaretAssertVectorIndex(m_cziScenePyramidInfos, frameIndex);
    const CziSceneInfo& cziSceneInfo(m_cziScenePyramidInfos[frameIndex]);
    
    const int32_t numPyramidLayers(cziSceneInfo.getNumberOfPyramidLayers());
    if (numPyramidLayers <= 0) {
        errorMessageOut = ("There are no pyramid layers for accessing data for frame "
                           + AString::number(frameIndex));
        return NULL;
    }
    if ((pyramidLayer < 0)
        || (pyramidLayer >= numPyramidLayers)) {
        errorMessageOut = ("Invalid pyramid level="
                           + AString::number(pyramidLayer)
                           + " range is [0,"
                           + AString::number(numPyramidLayers - 1)
                           + "] for frame "
                           + AString::number(frameIndex));
        return NULL;
    }
    CaretAssertVectorIndex(cziSceneInfo.m_pyramidLayers, pyramidLayer);
    libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo pyramidInfo = cziSceneInfo.m_pyramidLayers[pyramidLayer].m_layerInfo;
    
    libCZI::CDimCoordinate coordinate;
    libCZI::IDimCoordinate* iDimCoord = &coordinate;
    
    const std::array<float, 3> prefBackRGB = getPreferencesImageBackgroundRGB();
    libCZI::ISingleChannelPyramidLayerTileAccessor::Options scstaOptions;
    scstaOptions.Clear();
    scstaOptions.backGroundColor.r = prefBackRGB[0];
    scstaOptions.backGroundColor.g = prefBackRGB[1];
    scstaOptions.backGroundColor.b = prefBackRGB[2];
        
    /*
     * Read into 24 bit RGB to avoid conversion from other pixel formats
     */
    const libCZI::PixelType pixelType(libCZI::PixelType::Bgr24);
    CaretAssert(m_pyramidLayerTileAccessor);
    const libCZI::IntRect rectToReadROI = CziUtilities::qRectToIntRect(rectangleForReadingRect);
    std::shared_ptr<libCZI::IBitmapData> bitmapData;
    try {
        bitmapData = m_pyramidLayerTileAccessor->Get(pixelType,
                                                     rectToReadROI,
                                                     iDimCoord,
                                                     pyramidInfo,
                                                     &scstaOptions);
    }
    catch (std::out_of_range& e) {
        errorMessageOut = ("Out of range exception: "
                           + QString(e.exception::what())
                           + " reading pyramid layer="
                           + QString::number(pyramidInfo.pyramidLayerNo)
                           + " for ROI="
                           + CziUtilities::intRectToString(rectToReadROI));
        return NULL;
    }
    
    if ( ! bitmapData) {
        errorMessageOut = "Failed to read data";
        return NULL;
    }
    
    CaretLogInfo("Request reading of :"
                 + CziUtilities::qRectToString(rectangleForReadingRect)
                 + " and actually read width="
                 + QString::number(bitmapData->GetWidth())
                 + ", height="
                 + QString::number(bitmapData->GetHeight()));
    QImage* qImage = createQImageFromBitmapData(bitmapData.get(),
                                                errorMessageOut);
    if (qImage == NULL) {
        return NULL;
    }
    
    CziImage* cziImageOut = new CziImage(this,
                                         qImage,
                                         m_fullResolutionLogicalRect,
                                         CziUtilities::intRectToQRect(rectToReadROI),
                                         CziImageResolutionChangeModeEnum::INVALID,
                                         pyramidInfo.pyramidLayerNo);
    return cziImageOut;
}

/**
 * Read a pyramid layer from CZI file
 * @param pyramidLayer
 *    Index of pyramid layer to read
 * @param logicalRectangleRegionRect
 *    Rectangular region to read NOT REALLY USED ???
 * @param rectangleForReadingRect
 *    Rectangular region to read
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readPyramidLayerFromCziImageFile(const int32_t pyramidLayer,
                                               const QRectF& logicalRectangleRegionRect,
                                               const QRectF& rectangleForReadingRect,
                                               AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const int32_t numPyramidLayers(m_pyramidLayers.size());
    if (numPyramidLayers <= 0) {
        errorMessageOut = "There are no pyramid layers for accessing data";
        return NULL;
    }
    if ((pyramidLayer < 0)
        || (pyramidLayer >= numPyramidLayers)) {
        errorMessageOut = ("Invalid pyramid level="
                           + AString::number(pyramidLayer)
                           + " range is [0,"
                           + AString::number(numPyramidLayers - 1)
                           + "]");
        return NULL;
    }
    CaretAssertVectorIndex(m_pyramidLayers, pyramidLayer);
    libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo pyramidInfo = m_pyramidLayers[pyramidLayer].m_layerInfo;
    
    libCZI::CDimCoordinate coordinate;
    libCZI::IDimCoordinate* iDimCoord = &coordinate;
    
    const std::array<float, 3> prefBackRGB = getPreferencesImageBackgroundRGB();
    libCZI::ISingleChannelPyramidLayerTileAccessor::Options scstaOptions;
    scstaOptions.Clear();
    scstaOptions.backGroundColor.r = prefBackRGB[0];
    scstaOptions.backGroundColor.g = prefBackRGB[1];
    scstaOptions.backGroundColor.b = prefBackRGB[2];
    
    /*
     * Read into 24 bit RGB to avoid conversion from other pixel formats
     */
    const libCZI::PixelType pixelType(libCZI::PixelType::Bgr24);
    const libCZI::IntRect intRectROI = CziUtilities::qRectToIntRect(logicalRectangleRegionRect);
    CaretAssert(m_pyramidLayerTileAccessor);
    const libCZI::IntRect rectToReadROI = CziUtilities::qRectToIntRect(rectangleForReadingRect);
    std::shared_ptr<libCZI::IBitmapData> bitmapData;
    try {
        bitmapData = m_pyramidLayerTileAccessor->Get(pixelType,
                                                     rectToReadROI,
                                                     iDimCoord,
                                                     pyramidInfo,
                                                     &scstaOptions);
    }
    catch (std::out_of_range& e) {
        errorMessageOut = ("Out of range exception: "
                           + QString(e.exception::what())
                           + " reading pyramid layer="
                           + QString::number(pyramidInfo.pyramidLayerNo)
                           + " for ROI="
                           + CziUtilities::intRectToString(rectToReadROI));
        return NULL;
    }
    
    if ( ! bitmapData) {
        errorMessageOut = "Failed to read data";
        return NULL;
    }
    
    CaretLogInfo("Request reading of :"
                 + CziUtilities::intRectToString(intRectROI)
                 + " and actually read width="
                 + QString::number(bitmapData->GetWidth())
                 + ", height="
                 + QString::number(bitmapData->GetHeight()));
    QImage* qImage = createQImageFromBitmapData(bitmapData.get(),
                                                errorMessageOut);
    if (qImage == NULL) {
        return NULL;
    }
    
    CziImage* cziImageOut = new CziImage(this,
                                         qImage,
                                         m_fullResolutionLogicalRect,
                                         CziUtilities::intRectToQRect(rectToReadROI),
                                         CziImageResolutionChangeModeEnum::AUTO_PYRAMID,
                                         pyramidInfo.pyramidLayerNo);
    return cziImageOut;
}

/**
 * This function is taken from CSingleChannelPyramidLevelTileAccessor.  It computes how many pixels from the
 * highest resolution will fit in the pyramid layer.
 */
/// <summary>    For the specified pyramid layer (and the pyramid type), calculate the size of a pixel on this
///             layer as measured by pixels on layer 0. </summary>
/// <param name="pyramidInfo">    Information describing the pyramid and the requested pyramid-layer. </param>
/// <returns>    The calculated size of pixel (in units of pixels on pyramid layer 0). </returns>
/*static*/int
CziImageFile::CalcSizeOfPixelOnLayer0(const libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo& pyramidInfo)
{
    int f = 1;
    for (int i = 0; i < pyramidInfo.pyramidLayerNo; ++i)
    {
        f *= pyramidInfo.minificationFactor;
    }
    
    return f;
}

/**
 * Convert a pixel size to a logical size for the given layer.
 * Reading images uses "logical width"
 * @param pyramidLayers
 *    Pyramid layers for selection
 * @param pyramidLayerIndex
 *    Index of pyramid layer
 * @param widthInOut
 *    Width (input pixels; output logical)
 * @param heightInOut
 *    Height (input pixels; output logical)
 */
void
CziImageFile::pixelSizeToLogicalSize(const std::vector<PyramidLayer>& pyramidLayers,
                                     const int32_t pyramidLayerIndex,
                                     int32_t& widthInOut,
                                     int32_t& heightInOut) const
{
    CaretAssertVectorIndex(pyramidLayers, pyramidLayerIndex);
    const int32_t pixelScale = CalcSizeOfPixelOnLayer0(pyramidLayers[pyramidLayerIndex].m_layerInfo);
    widthInOut  *= pixelScale;
    heightInOut *= pixelScale;
    widthInOut  = std::min(widthInOut, static_cast<int32_t>(m_fullResolutionLogicalRect.width()));
    heightInOut = std::min(heightInOut, static_cast<int32_t>(m_fullResolutionLogicalRect.height()));
}


/**
 * Create a QImage from the CZI bitmap data
 * @param bitmapData
 *    The CZI bitmap data
 * @param errorMessageOut
 *    Contains error information
 * @return A QImage containing the bitmap data or NULL if not valid
 */
QImage*
CziImageFile::createQImageFromBitmapData(libCZI::IBitmapData* bitmapData,
                                         AString& errorMessageOut)
{
    CaretAssert(bitmapData);
    
    const auto width(bitmapData->GetWidth());
    const auto height(bitmapData->GetHeight());
    if (cziDebugFlag) std::cout << "Image width/height: " << width << ", " << height << std::endl;
    
    if ((width <= 0)
        || (height <= 0)) {
        errorMessageOut = "data has invalid width or height";
        return NULL;
    }
    
    std::vector<unsigned char> imageData(width * height * 4);
    libCZI::BitmapLockInfo bitMapInfo = bitmapData->Lock();
    if (cziDebugFlag) std::cout << "   Stride: " << bitMapInfo.stride << std::endl;
    if (cziDebugFlag) std::cout << "   Size: " << bitMapInfo.size << std::endl;
    
    AString colorName;
    if (bitmapData->GetPixelType() != libCZI::PixelType::Bgr24) {
        errorMessageOut = "Only pixel type Bgr24 is supported";
        return NULL;
    }
    
    if (cziDebugFlag) std::cout << "   Color: " << colorName << std::endl << std::flush;
    
    unsigned char* cziPtr8 = (unsigned char*)bitMapInfo.ptrDataRoi;
    
    /*
     * Documentation for QImage states that setPixel may be very costly
     * and recommends using the scanLine() method to access pixel data.
     */
    QImage* imageOut = new QImage(width,
                                  height,
                                  QImage::Format_ARGB32);
    
    const bool isOriginAtTop(true);
    for (int64_t y = 0; y < height; y++) {
        const int32_t scanLineIndex = (isOriginAtTop
                                       ? y
                                       : height - y - 1);
        QRgb* rgbScanLine = (QRgb*)imageOut->scanLine(scanLineIndex);
        
        int64_t cziDataRowOffset = (y * bitMapInfo.stride);
        
        for (int64_t x = 0; x < width; x++) {
            int64_t pixelOffset(cziDataRowOffset + (x * 3));
            QColor rgba(cziPtr8[pixelOffset + 2],
                        cziPtr8[pixelOffset + 1],
                        cziPtr8[pixelOffset],
                        255);
            
            QRgb* pixel = &rgbScanLine[x];
            *pixel = rgba.rgba();
        }
    }
    
    bitmapData->Unlock();
    
    return imageOut;
}


/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
CziImageFile::writeFile(const AString& filename)
{
    /*
     * Cannot write CZI file
     */
    throw DataFileException(filename,
                            "Writing of CZI images files is not supported");
}

/**
 *  @return True if the file supports writing, else false.
 */
bool
CziImageFile::supportsWriting() const
{
    return false;
}

/**
 * @return width of media file
 */
int32_t
CziImageFile::getWidth() const
{
    return m_fullResolutionLogicalRect.width();
}

/**
 * @return height of media file
 */
int32_t
CziImageFile::getHeight() const
{
    return m_fullResolutionLogicalRect.height();
}

/**
 * @return Number of frames in the file
 */
int32_t
CziImageFile::getNumberOfFrames() const
{
    return getNumberOfScenes();
}

/**
 * @return Number of scenes in the file
 */
int32_t
CziImageFile::getNumberOfScenes() const
{
    return m_cziScenePyramidInfos.size();
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param overlayIndex
 *    Index of the overlay
 * @param pixelLogicalIndex
 *    Logical pixel index
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
CziImageFile::getPixelIdentificationText(const int32_t tabIndex,
                                         const int32_t overlayIndex,
                                         const PixelLogicalIndex& pixelLogicalIndex,
                                         std::vector<AString>& columnOneTextOut,
                                         std::vector<AString>& columnTwoTextOut,
                                         std::vector<AString>& toolTipTextOut) const
{
    columnOneTextOut.clear();
    columnTwoTextOut.clear();
    toolTipTextOut.clear();
    if ( ! isPixelIndexValid(tabIndex,
                             overlayIndex,
                             pixelLogicalIndex)) {
        return;
    }
    
    const CziImage* cziImage = getImageForTabOverlay(tabIndex,
                                                     overlayIndex);
    if (cziImage != NULL) {
        cziImage->getPixelIdentificationText(getFileNameNoPath(),
                                             pixelLogicalIndex,
                                             columnOneTextOut,
                                             columnTwoTextOut,
                                             toolTipTextOut);
        
        std::array<float, 3> xyz;
        if (pixelIndexToStereotaxicXYZ(pixelLogicalIndex, false, xyz)) {
            columnOneTextOut.push_back("Stereotaxic XYZ");
            columnTwoTextOut.push_back(AString::fromNumbers(xyz.data(), 3, ", "));
        }
        
        if (pixelIndexToStereotaxicXYZ(pixelLogicalIndex, true, xyz)) {
            columnOneTextOut.push_back("Stereotaxic XYZ with NIFTI warping");
            columnTwoTextOut.push_back(AString::fromNumbers(xyz.data(), 3, ", "));
        }
    }
}

/**
 * convert a pixel index to a stereotaxic coordinate
 * @param pixelLogicalIndex
 *    Logical pixel index
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param xyzOut
 *    Output with the XYZ coordinate
 *    @param
 * @return
 *    True if conversion successful, else false.
 */
bool
CziImageFile::pixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                         const bool includeNonlinearFlag,
                                         std::array<float, 3>& xyzOut) const
{
    std::array<float, 3> debugPixelIndex;
    return pixelIndexToStereotaxicXYZ(pixelLogicalIndex,
                                      includeNonlinearFlag,
                                      xyzOut,
                                      debugPixelIndex);
}

/**
 * convert a pixel index to a stereotaxic coordinate
 * @param pixelIndexOriginAtTop
 *    The pixel index (full resolution) with origin at top left
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param xyzOut
 *    Output with the XYZ coordinate
 *    @param
 *@param debugPixelIndexOut
 *    Pixel index for debugging.
 * @return
 *    True if conversion successful, else false.
 */
bool
CziImageFile::pixelIndexToStereotaxicXYZ(const PixelLogicalIndex& pixelLogicalIndex,
                                         const bool includeNonlinearFlag,
                                         std::array<float, 3>& xyzOut,
                                         std::array<float, 3>& debugPixelIndexOut) const
{
    const PixelIndex pixelIndexOriginAtTop(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    debugPixelIndexOut.fill(-1);
    
    /*
     * Load NIFTI transform file if we have not tried to load in previously
     */
    const AString niftiFileName("mri2hist.nii.gz");
    if ( ! m_pixelToStereotaxicTransform.m_triedToLoadFileFlag) {
        m_pixelToStereotaxicTransform.m_triedToLoadFileFlag = true;
        loadNiftiTransformFile(niftiFileName,
                               m_pixelToStereotaxicTransform);
    }
    
    /*
     * Are transforms valid?
     */
    if (m_pixelToStereotaxicTransform.m_sformMatrix) {
        /*
         * CZI Image has origin at top left so pixels range
         * left-to-right and top-to-bottom.
         */
        const float pixelI(m_pixelToStereotaxicTransform.m_xLeftToRightFlag
                           ? pixelIndexOriginAtTop.getI()
                           : (getWidth() - pixelIndexOriginAtTop.getI() - 1));
        const float pixelJ(m_pixelToStereotaxicTransform.m_yTopToBottomFlag
                           ? pixelIndexOriginAtTop.getJ()
                           : (getHeight() - pixelIndexOriginAtTop.getJ() - 1));
        
        /*
         * Scale pixel index from full resolution to resolution
         * contained in the NIFTI transform file
         */
        std::array<float, 4> pt {
            static_cast<float>(MathFunctions::round(pixelI * m_pixelToStereotaxicTransform.m_pixelScaleI)),
            static_cast<float>(MathFunctions::round(pixelJ * m_pixelToStereotaxicTransform.m_pixelScaleJ)),
            0.0f,
            1.0f
        };
        
        const int64_t niftiI(pt[0]);
        const int64_t niftiJ(pt[1]);
        const int64_t niftiK(pt[2]);
        debugPixelIndexOut[0] = pt[0];
        debugPixelIndexOut[1] = pt[1];
        debugPixelIndexOut[2] = pt[2];
        
        /*
         * Use matrix to convert pixel index to coordinate
         */
        m_pixelToStereotaxicTransform.m_sformMatrix->multiplyPoint4(pt.data());
        
        if (includeNonlinearFlag
            && m_pixelToStereotaxicTransform.m_niftiFile) {
            if (m_pixelToStereotaxicTransform.m_niftiFile->indexValid(niftiI, niftiJ, niftiK)) {
                /*
                 * Use pixel index to obtain non-linearity from NIFTI data
                 */
                const float dx = m_pixelToStereotaxicTransform.m_niftiFile->getValue(niftiI, niftiJ, niftiK, 0);
                const float dy = m_pixelToStereotaxicTransform.m_niftiFile->getValue(niftiI, niftiJ, niftiK, 1);
                const float dz = m_pixelToStereotaxicTransform.m_niftiFile->getValue(niftiI, niftiJ, niftiK, 2);
                pt[0] += dx;
                pt[1] += dy;
                pt[2] += dz;
            }
            else {
                CaretLogFine("("
                             + AString::number(niftiI)
                             + ", "
                             + AString::number(niftiJ)
                             + ", "
                             + AString::number(niftiK)
                             + ") is not a valid pixel index for "
                             + niftiFileName
                             + " associated with "
                             + getFileNameNoPath());
            }
        }
        
        xyzOut[0] = pt[0];
        xyzOut[1] = pt[1];
        xyzOut[2] = pt[2];
        
        return true;
    }
    
    return false;
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param pixelLogicalIndexOut
 *    Output logical pixel index
 * @return
 *    True if successful, else false.
 */
bool
CziImageFile::stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                         const bool includeNonlinearFlag,
                                         PixelLogicalIndex& pixelLogicalIndexOut) const
{
    std::array<float, 3> debugPixelIndex;
    if (stereotaxicXyzToPixelIndex(xyz,
                                   includeNonlinearFlag,
                                   pixelLogicalIndexOut,
                                   debugPixelIndex)) {
        return true;
    }
    
    return false;
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param pixelLogicalIndex
 *    Output with pixel logical index
 * @param debugPixelIndex
 *    Pixel index used for debugging
 * @return
 *    True if successful, else false.
 */
bool
CziImageFile::stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                         const bool includeNonlinearFlag,
                                         PixelLogicalIndex& pixelLogicalIndex,
                                         const std::array<float, 3>& debugPixelIndex) const
{
    pixelLogicalIndex.setIJK(-1, -1, -1);
    
    /*
     * Load NIFTI transform file if we have not tried to load in previously
     */
    const AString niftiFileName("hist2mri.nii.gz");
    if ( ! m_stereotaxicToPixelTransform.m_triedToLoadFileFlag) {
        m_stereotaxicToPixelTransform.m_triedToLoadFileFlag = true;
        loadNiftiTransformFile(niftiFileName,
                               m_stereotaxicToPixelTransform);
        
        if (m_stereotaxicToPixelTransform.m_sformMatrix) {
            const Matrix4x4 originalSformMatrix(*m_stereotaxicToPixelTransform.m_sformMatrix);
            
            /*
             * Column one of matrix (x)
             */
            double columnOne[4];
            originalSformMatrix.getColumn(0, columnOne);
            
            /*
             * Column two of matrix (y)
             */
            double columnTwo[4];
            originalSformMatrix.getColumn(1, columnTwo);
            
            /*
             * Cross-product of columns one and two
             * creates vector orthogonal to XY-plane
             */
            double cp[4] { 0.0f, 0.0f, 0.0f, 1.0 };
            MathFunctions::crossProduct(columnOne, columnTwo, cp);
            MathFunctions::normalizeVector(cp);
            
            /*
             * Setting the column sets all four elements in the column
             * but we want to preserve the element in the bottom row
             */
            cp[3] = originalSformMatrix.getMatrixElement(3, 2); /* Do no overwrite matrix last element in column*/
            
            /*
             * Create a copy of the original sform and replace the
             * third column to make the matrix invertible
             */
            Matrix4x4 modifiedSformMatrix(originalSformMatrix);
            modifiedSformMatrix.setColumn(2, cp);
            
            if (cziDebugFlag) {
                std::cout << "Matrix before Inversion after column three replaced: " << modifiedSformMatrix.toString() << std::endl;
            }
            
            /*
             * Invert the matrix
             */
            Matrix4x4 invertedSformMatrix(modifiedSformMatrix);
            if (invertedSformMatrix.invert()) {
                if (cziDebugFlag) {
                    std::cout << "       after Inversion: " << invertedSformMatrix.toString() << std::endl;
                }
                
                /*
                 * Replace original sform with the inverted sform
                 */
                m_stereotaxicToPixelTransform.m_sformMatrix.reset(new Matrix4x4(invertedSformMatrix));
                
                CaretLogFine("Original sform:\n"
                             + originalSformMatrix.toString()
                             + "\nsform modified so that it can be inverted:\n"
                             + modifiedSformMatrix.toString()
                             + "\nInverted sform:\n"
                             + invertedSformMatrix.toString());
                
                if (cziDebugFlag) {
                    std::cout << "Testing: " << std::endl;
                    float p[3] { 10.0, 20.0, 0.0 };
                    std::cout << "   Pixel: " << AString::fromNumbers(p, 3, ", ") << std::endl;
                    originalSformMatrix.multiplyPoint3(p);
                    std::cout << "   Pixel Multiplied: " << AString::fromNumbers(p, 3, ", ") << std::endl;
                    invertedSformMatrix.multiplyPoint3(p);
                    std::cout << "   Coord Inverse Multiplied (Should Match Pixel): " << AString::fromNumbers(p, 3, ", ") << std::endl;
                }
            }
            else {
                std::cout << "Matrix inversion failed." << std::endl;
                m_stereotaxicToPixelTransform.m_sformMatrix.reset();
                m_stereotaxicToPixelTransform.m_niftiFile.reset();
            }
        }
    }
    
    /*
     * Are transforms valid?
     */
    if (m_stereotaxicToPixelTransform.m_sformMatrix) {
        /*
         * Use matrix to convert coordinate to 'mri space' pixel index
         */
        std::array<float, 4> pt { xyz[0], xyz[1], xyz[2], 1.0 };
        m_stereotaxicToPixelTransform.m_sformMatrix->multiplyPoint4(pt.data());
        pt[0] = MathFunctions::round(pt[0]);
        pt[1] = MathFunctions::round(pt[1]);
        pt[2] = MathFunctions::round(pt[2]);
        
        if (includeNonlinearFlag
            && m_stereotaxicToPixelTransform.m_niftiFile) {
            /*
             * Get warpfield values at the pixel
             */
            const int64_t niftiI(pt[0]);
            const int64_t niftiJ(pt[1]);
            const int64_t niftiK(pt[2]);
            if (m_stereotaxicToPixelTransform.m_niftiFile->indexValid(niftiI, niftiJ, niftiK)) {
                const float dx = m_stereotaxicToPixelTransform.m_niftiFile->getValue(niftiI, niftiJ, niftiK, 0);
                const float dy = m_stereotaxicToPixelTransform.m_niftiFile->getValue(niftiI, niftiJ, niftiK, 1);
                const float dz = m_stereotaxicToPixelTransform.m_niftiFile->getValue(niftiI, niftiJ, niftiK, 2);
                
                /*
                 * Add the warpfield values to the original XYZ values
                 */
                pt[0] = xyz[0] + dx;
                pt[1] = xyz[1] + dy;
                pt[2] = xyz[2] + dz;
                pt[3] = 1;
                
                /*
                 * Multiply new XYZ to get pixel index
                 */
                m_stereotaxicToPixelTransform.m_sformMatrix->multiplyPoint4(pt.data());
            }
            else {
                CaretLogFine("("
                             + AString::number(niftiI)
                             + ", "
                             + AString::number(niftiJ)
                             + ", "
                             + AString::number(niftiK)
                             + ") is not a valid index for "
                             + niftiFileName
                             + " associated with "
                             + getFileNameNoPath());
            }
        }
        
        /*
         * Round a copy before printing.
         * Note: Keep fractional parts for later convertion to full res pixel indices
         */
        std::array<float, 4> ptRounded {
            static_cast<float>(MathFunctions::round(pt[0])),
            static_cast<float>(MathFunctions::round(pt[1])),
            static_cast<float>(MathFunctions::round(pt[2])),
            static_cast<float>(MathFunctions::round(pt[3]))
        };
        
        CaretLogFine("\n"
                     + AString(includeNonlinearFlag ? "With NIFTI warping ": "")
                     + "XYZ to Pixel Index Test: "
                     + AString::fromNumbers(ptRounded.data(), 3, ", ")
                     + "\n"
                     + AString(includeNonlinearFlag ? "With NIFTI warping ": "")
                     + "    Correct pixel index: "
                     + AString::fromNumbers(debugPixelIndex.data(), debugPixelIndex.size(), ", "));
        
        /*
         * Convert to full-resolution pixel index
         */
        CaretAssert((m_stereotaxicToPixelTransform.m_pixelScaleI != 0.0)
                    && (m_stereotaxicToPixelTransform.m_pixelScaleJ != 0.0));
        pt[0] /= m_stereotaxicToPixelTransform.m_pixelScaleI;
        pt[1] /= m_stereotaxicToPixelTransform.m_pixelScaleJ;
        
        /*
         * CZI Image has origin at top left so pixels range
         * left-to-right and top-to-bottom
         */
        pt[0] = (m_stereotaxicToPixelTransform.m_xLeftToRightFlag
                 ? pt[0]
                 : (getWidth() - pt[0] - 1));
        pt[1] = (m_stereotaxicToPixelTransform.m_yTopToBottomFlag
                 ? pt[1]
                 : (getHeight() - pt[1] - 1));
        pt[0] = MathFunctions::round(pt[0]);
        pt[1] = MathFunctions::round(pt[1]);
        pt[2] = MathFunctions::round(pt[2]);
        
        pixelLogicalIndex = pixelIndexToPixelLogicalIndex(PixelIndex(pt[0], pt[1], pt[2]));
        
        return true;
    }
    
    return false;
}

/**
 * Test the pixel transformations by transforming pixel indices to stereotaxic coordinates
 * and back to pixel indices.
 * @param pixelIndexStep
 *    Step by this amount of pixel indices in both horizontal and vertical directions
 * @param nonLinearFlag
 *    Include non-linear portion of transformation
 * @Param verboseFlag
 *    Print each point tested
 * @param resultsMessageOut
 *    Output with text describing the results.
 */
void
CziImageFile::testPixelTransforms(const int32_t pixelIndexStep,
                                  const bool nonLinearFlag,
                                  const bool verboseFlag,
                                  AString& resultsMessageOut,
                                  QImage& imageOut) const
{
    resultsMessageOut.clear();
    imageOut = QImage();
    resultsMessageOut.append("Filename : "
                             + getFileNameNoPath());

    const int32_t numRows(getHeight());
    const int32_t numCols(getWidth());
    if ((numCols <= 0)
        || (numRows <= 0)) {
        resultsMessageOut = "Image has invalid width and/or height";
        return;
    }
    
    int32_t imageNumberOfColumns(0);
    std::vector<TestTransformResult> testResults;
    std::vector<float> diffsIJK;
    diffsIJK.reserve(((numRows / pixelIndexStep) + 1)
                      * ((numCols / pixelIndexStep) + 1));
    for (int32_t iRow = 0; iRow < numRows; iRow += pixelIndexStep) {
        for (int32_t iCol = 0; iCol < numCols; iCol += pixelIndexStep) {
            const PixelLogicalIndex pixelLogicalIndex(iCol, iRow, 0);
            std::array<float, 3> xyz;
            if ( ! pixelIndexToStereotaxicXYZ(pixelLogicalIndex,
                                              nonLinearFlag,
                                              xyz)) {
                resultsMessageOut.appendWithNewLine("Failed to convert pixel to xyz.  Pixel="
                                                    + pixelLogicalIndex.toString());
                continue;
            }
            
            PixelLogicalIndex pixelLogicalIndexTwo;
            if ( ! stereotaxicXyzToPixelIndex(xyz,
                                              nonLinearFlag,
                                              pixelLogicalIndexTwo)) {
                resultsMessageOut.appendWithNewLine("Failed to convert pixel to xyz.  Pixel="
                                                    + pixelLogicalIndexTwo.toString()
                                                    + " and XYZ=("
                                                    + AString::fromNumbers(xyz.data(), 3, ",")
                                                    + " back to pixel index.");
                continue;
                
            }
            const float dI(pixelLogicalIndexTwo.getI() - pixelLogicalIndex.getI());
            const float dJ(pixelLogicalIndexTwo.getJ() - pixelLogicalIndex.getJ());
            const float dK(0);
            const float dIJK(std::sqrt(dI*dI + dJ*dJ + dK*dK));
            diffsIJK.push_back(dIJK);
            
            if (verboseFlag) {
                testResults.emplace_back(pixelLogicalIndex,
                                         pixelLogicalIndexTwo,
                                         xyz,
                                         dI,
                                         dJ,
                                         dIJK);
            }
        }
        
        if (iRow == 0) {
            imageNumberOfColumns = static_cast<int32_t>(testResults.size());
        }
    }
    
    if ( ! testResults.empty()) {
        const int32_t tableNumberOfRows(testResults.size());
        const int32_t tableNumberOfColumns(10);
        const int32_t floatPrecision(3);
        StringTableModel tableModel(tableNumberOfRows + 1, tableNumberOfColumns, floatPrecision);
        int32_t col(0);
        tableModel.setElement(0, col++, "I");
        tableModel.setElement(0, col++, "J");
        tableModel.setElement(0, col++, "X");
        tableModel.setElement(0, col++, "Y");
        tableModel.setElement(0, col++, "Z");
        tableModel.setElement(0, col++, "Xform I");
        tableModel.setElement(0, col++, "Xform J");
        tableModel.setElement(0, col++, "diff I");
        tableModel.setElement(0, col++, "diff J");
        tableModel.setElement(0, col++, "IJ Diff");

        for (int32_t i = 1; i <= tableNumberOfRows; i++) {
            CaretAssertVectorIndex(testResults, i - 1);
            const TestTransformResult& trt = testResults[i - 1];
            int32_t col(0);
            tableModel.setElement(i, col++, trt.m_pixel.getI());
            tableModel.setElement(i, col++, trt.m_pixel.getJ());
            tableModel.setElement(i, col++, trt.m_xyz[0]);
            tableModel.setElement(i, col++, trt.m_xyz[1]);
            tableModel.setElement(i, col++, trt.m_xyz[2]);
            tableModel.setElement(i, col++, trt.m_pixelTwo.getI());
            tableModel.setElement(i, col++, trt.m_pixelTwo.getJ());
            tableModel.setElement(i, col++, trt.m_dI);
            tableModel.setElement(i, col++, trt.m_dJ);
            tableModel.setElement(i, col++, trt.m_dIJK);
            CaretAssert(col == tableNumberOfColumns);
        }
        
        CaretAssert((tableNumberOfRows + 1) == tableModel.getNumberOfRows());
        
        resultsMessageOut.appendWithNewLine(tableModel.getInString());
    }
    
    if ( ! diffsIJK.empty()) {
        DescriptiveStatistics stats;
        stats.update(diffsIJK);
        
        AString statsText;
        statsText.appendWithNewLine("Pixels Tested: "
                                    + AString::number(diffsIJK.size()));
        statsText.appendWithNewLine("Mean:          "
                                    + AString::number(stats.getMean()));
        statsText.appendWithNewLine("Min:           "
                                    + AString::number(stats.getMinimumValue()));
        statsText.appendWithNewLine("Max:           "
                                    + AString::number(stats.getMaximumValue()));
        statsText.appendWithNewLine("Std-Dev:       "
                                    + AString::number(stats.getPopulationStandardDeviation()));

        resultsMessageOut.insert(0, statsText + "\n\n");
        
        const int32_t imageNumberOfRows = (testResults.size() / imageNumberOfColumns);        
        if ((imageNumberOfRows > 0)
            && (imageNumberOfColumns > 0)) {
            const float range(stats.getMaximumValue() - stats.getMinimumValue());
            if (range > 0.0) {
                const float minValue(stats.getMinimumValue());
                QImage image(imageNumberOfColumns,
                             imageNumberOfRows,
                             QImage::Format_ARGB32);
                for (int32_t iRow = 0; iRow < imageNumberOfRows; iRow++) {
                    for (int32_t iCol = 0; iCol < imageNumberOfColumns; iCol++) {
                        const int32_t index = (iRow * imageNumberOfColumns) + iCol;
                        CaretAssertVectorIndex(diffsIJK, index);
                        int value(((diffsIJK[index] - minValue) / range) * 255.0);
                        if (value > 255) {
                            value = 255;
                        }
                        else if (value < 0) {
                            value = 0;
                        }
                        image.setPixelColor(iCol, iRow, QColor::fromRgb(value, value, value));
                    }
                }
                
                imageOut = image.scaledToWidth(600,
                                               Qt::SmoothTransformation);
            }
        }
    }
}

/**
 * Find the Pixel nearest the given XYZ coordinate
 * @param xyz
 *    The coordinate
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param signedDistanceToPixelMillimetersOut
 *    Output with signed distance to the pixel in millimeters
 * @param pixelLogicalIndexOut
 *    Output with logical pixel index
 * @return
 *    True if successful, else false.
 */
bool
CziImageFile::findPixelNearestStereotaxicXYZ(const std::array<float, 3>& xyz,
                                             const bool includeNonLinearFlag,
                                             float& signedDistanceToPixelMillimetersOut,
                                             PixelLogicalIndex& pixelLogicalIndexOut) const
{
    const Plane* plane(getImagePlane());
    if (plane == NULL) {
        return false;
    }
    
    std::array<float, 3> xyzOnPlane;
    plane->projectPointToPlane(xyz.data(), xyzOnPlane.data());
    
    if (stereotaxicXyzToPixelIndex(xyzOnPlane,
                                   includeNonLinearFlag,
                                   pixelLogicalIndexOut)) {
        if (isPixelIndexValid(pixelLogicalIndexOut)) {
            signedDistanceToPixelMillimetersOut = plane->absoluteDistanceToPlane(xyz.data());
            return true;
        }
    }
    return false;
}

/**
 * @return The plane for this CZI image calculated from the coordinates at the image's corners
 */
const Plane*
CziImageFile::getImagePlane() const
{
    /*
     * Has plane already been created?
     */
    if (m_imagePlane) {
        /*
         * Plane was previously computed
         */
        return m_imagePlane.get();
    }
    
    if (m_imagePlaneInvalid) {
        /*
         * Tried to create plane previously but failed
         */
        return NULL;
    }
    
    /*
     * Note: Origin at top left
     */
    const float zero(0.0);
    const PixelLogicalIndex bottomLeftPixel(m_fullResolutionLogicalRect.left(),
                                            m_fullResolutionLogicalRect.bottom(),
                                            zero);
    const PixelLogicalIndex topLeftPixel(m_fullResolutionLogicalRect.left(),
                                         m_fullResolutionLogicalRect.top(),
                                         zero);
    const PixelLogicalIndex topRightPixel(m_fullResolutionLogicalRect.right(),
                                          m_fullResolutionLogicalRect.top(),
                                          zero);
    
    /*
     * Convert pixel indices to XYZ coordinates
     */
    std::array<float, 3> bottomLeftXYZ, topLeftXYZ, topRightXYZ;
    const bool nonLinearFlag(true);
    if (pixelIndexToStereotaxicXYZ(bottomLeftPixel, nonLinearFlag, bottomLeftXYZ)
        && pixelIndexToStereotaxicXYZ(topLeftPixel, nonLinearFlag, topLeftXYZ)
        && pixelIndexToStereotaxicXYZ(topRightPixel, nonLinearFlag, topRightXYZ)) {
        /*
         * Create the plane from XYZ coordinates
         */
        m_imagePlane.reset(new Plane(bottomLeftXYZ.data(),
                                     topLeftXYZ.data(),
                                     topRightXYZ.data()));
        if (m_imagePlane->isValidPlane()) {
            return m_imagePlane.get();
        }
        else {
            /*
             * Plane invalid
             */
            m_imagePlane.reset();
            m_imagePlaneInvalid = true;
            CaretLogSevere(getFileNameNoPath()
                           + "Failed to create plane, computation of plane failed.");
        }
    }
    else {
        CaretLogSevere(getFileNameNoPath()
                       + "Failed to create plane, pixel to coordinate transform failed.");
        m_imagePlaneInvalid = true;
    }
    
    return NULL;
}

/**
 * Load NIFTI transform file used to transform between pixel indices and stereotaxic coordinates
 * @param filename
 *    Name of NIFTI file
 * @param transform
 *    NIFTI transform that is loaded
 */
void
CziImageFile::loadNiftiTransformFile(const AString& filename,
                                     NiftiTransform& transform) const
{
    
    FileInformation niftiTransformFileName(FileInformation(getFileName()).getPathName(),
                                           filename);
    
    if (niftiTransformFileName.exists()) {
        transform.m_niftiFile.reset(new VolumeFile());
        try {
            transform.m_niftiFile->readFile(niftiTransformFileName.getAbsoluteFilePath());
            
            std::vector<int64_t> dims;
            transform.m_niftiFile->getDimensions(dims);
            if (dims.size() < 5) {
                throw DataFileException("Dimensions should be 5 but are "
                                        + AString::number(dims.size()));
            }
            if (dims[3] != 3) {
                throw DataFileException("4th dimension should be 3 but is "
                                        + AString::number(dims[3]));
            }
            if (transform.m_niftiFile->getNumberOfMaps() != 3) {
                throw DataFileException("Number of maps should be 3 but is "
                                        + AString::number(transform.m_niftiFile->getNumberOfMaps()));
            }
            
            VolumeSpace::OrientTypes orientation[3];
            transform.m_niftiFile->getOrientation(orientation);
            
            std::vector<AString> orientationNames(3);
            for (int32_t i = 0; i < 3; i++) {
                AString orientName;
                
                switch (orientation[i]) {
                    case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                        orientName = "Anterior to Posterior";
                        break;
                    case VolumeSpace::INFERIOR_TO_SUPERIOR:
                        orientName = "Inferior to Superior";
                        break;
                    case VolumeSpace::LEFT_TO_RIGHT:
                        orientName = "Left to Right";
                        break;
                    case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                        orientName = "Posterior to Anterior";
                        break;
                    case VolumeSpace::RIGHT_TO_LEFT:
                        orientName = "Right to Left";
                        break;
                    case VolumeSpace::SUPERIOR_TO_INFERIOR:
                        orientName = "Superior to Inferior";
                        break;
                }
                orientationNames[i] = orientName;
            }
            
            AString orientationErrorMessage;
            if (orientation[0] == VolumeSpace::LEFT_TO_RIGHT) {
                transform.m_xLeftToRightFlag = true;
            }
            else if (orientation[0] == VolumeSpace::RIGHT_TO_LEFT) {
                transform.m_xLeftToRightFlag = false;
            }
            else {
                orientationErrorMessage.appendWithNewLine("Orientation for first axis "
                                                          + orientationNames[0]
                                                          + " not supported.  Should be a right/left orientation");
            }
            if (orientation[1] == VolumeSpace::ANTERIOR_TO_POSTERIOR) {
                transform.m_yTopToBottomFlag = true;
            }
            else if (orientation[1] == VolumeSpace::POSTERIOR_TO_ANTERIOR) {
                transform.m_yTopToBottomFlag = false;
            }
            else {
                orientationErrorMessage.appendWithNewLine("Orientation for first axis "
                                                          + orientationNames[0]
                                                          + " not supported.  Should be a anterior/posterior orientation");
            }
            
            if ( ! orientationErrorMessage.isEmpty()) {
                throw DataFileException(orientationErrorMessage);
            }
            
            const int64_t dimI(dims[0]);
            const int64_t dimJ(dims[1]);
            
            int32_t pyramidLayerIndex(-1);
            int64_t maxDiff(9999999999);
            const int32_t numResolutions(m_pyramidLayers.size());
            for (int32_t i = 0; i < numResolutions; i++) {
                const int64_t diffI(std::abs(m_pyramidLayers[i].m_pixelWidth - dimI));
                const int64_t diffJ(std::abs(m_pyramidLayers[i].m_pixelHeight - dimJ));
                const int64_t diff(diffI + diffJ);
                if (diff < maxDiff) {
                    maxDiff = diff;
                    pyramidLayerIndex = i;
                }
            }
            
            if (pyramidLayerIndex >= 0) {
                CaretAssertVectorIndex(m_pyramidLayers, pyramidLayerIndex);
                const PyramidLayer& pyramidLayer = m_pyramidLayers[pyramidLayerIndex];
                transform.m_pixelScaleI = (static_cast<float>(pyramidLayer.m_pixelWidth)
                                           / getWidth());
                transform.m_pixelScaleJ = (static_cast<float>(pyramidLayer.m_pixelHeight)
                                           / getHeight());
                
                std::vector<std::vector<float>> sform(transform.m_niftiFile->getSform());
                transform.m_sformMatrix.reset(new Matrix4x4(sform));
                
                AString pixelStepText;
                if (CaretLogger::getLogger()->isFine()) {
                    std::array<float, 4> p1 { 0.0, 0.0, 0.0, 1.0 };
                    transform.m_sformMatrix->multiplyPoint4(p1.data());
                    std::array<float, 4> p2 { 1.0, 1.0, 1.0, 1.0 };
                    transform.m_sformMatrix->multiplyPoint4(p2.data());
                    std::array<float, 4> stepXYZ {
                        p2[0] - p1[0],
                        p2[1] - p1[1],
                        p2[2] - p1[2],
                        p2[3] - p1[3]
                    };
                    pixelStepText = ("Pixel Step (0, 0, 0) to (1, 1, 1): "
                                     + AString::fromNumbers(stepXYZ.data(), 3, ", "));
                }
                
                CaretLogFine("\n"
                             + transform.m_niftiFile->getFileName()
                             + "\nBest matching pyramid layer index: "
                             + AString::number(pyramidLayerIndex)
                             + "\nCZI Layer: "
                             + AString::number(pyramidLayer.m_layerInfo.pyramidLayerNo)
                             + "\nWidth: "
                             + AString::number(pyramidLayer.m_pixelWidth)
                             + " height: "
                             + AString::number(pyramidLayer.m_pixelHeight)
                             + "\nPixel Scale I/J "
                             + AString::number(transform.m_pixelScaleI)
                             + ", "
                             + AString::number(transform.m_pixelScaleJ)
                             + "\nNIFTI Dimensions: "
                             + AString::fromNumbers(dims, ", ")
                             + "\nNIFTI Orientations: "
                             + AString::join(orientationNames, ", ")
                             + "\n" + pixelStepText
                             + "\nNIFTI sform: "
                             + transform.m_sformMatrix->toString());
            }
            else {
                throw DataFileException("Unable to find pyramid layer that best matches width/height: "
                                        + AString::number(dimI)
                                        + ", "
                                        + AString::number(dimJ));
            }
        }
        catch (const DataFileException& dfe) {
            CaretLogWarning("Failed to read "
                            + niftiTransformFileName.getFileName()
                            + " for "
                            + getFileName()
                            + " ERROR: "
                            + dfe.whatString());
            transform.m_niftiFile.reset();
        }
    }
}

/**
 * Add to the data file information.
 * @param dataFileInformation
 *    Item to which information is added.
 */
void
CziImageFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    MediaFile::addToDataFileContentInformation(dataFileInformation);
    
    const CziImage* defaultCziImage(m_allFramesPyramidInfo.getDefaultImage());
    if (defaultCziImage != NULL) {
        dataFileInformation.addNameAndValue("Default Image", QString(""));
        dataFileInformation.addNameAndValue("----Width (pixels)", defaultCziImage->getWidth());
        dataFileInformation.addNameAndValue("----Height (pixels)", defaultCziImage->getHeight());
        dataFileInformation.addNameAndValue("----Logical X", defaultCziImage->m_imageDataLogicalRect.x());
        dataFileInformation.addNameAndValue("----Logical Y", defaultCziImage->m_imageDataLogicalRect.y());
        dataFileInformation.addNameAndValue("----Logical Width", defaultCziImage->m_imageDataLogicalRect.width());
        dataFileInformation.addNameAndValue("----Logical Height", defaultCziImage->m_imageDataLogicalRect.height());
    }
    
    dataFileInformation.addNameAndValue("Pixel Size X (mm)", m_pixelSizeMmX, 6);
    dataFileInformation.addNameAndValue("Pixel Size Y (mm)", m_pixelSizeMmY, 6);
    dataFileInformation.addNameAndValue("Pixel Size Z (mm)", m_pixelSizeMmZ, 6);
    dataFileInformation.addNameAndValue("Full Logical Rectangle",
                                        CziUtilities::qRectToString(m_fullResolutionLogicalRect));
    
    {
        dataFileInformation.addNameAndValue("All Frames", "");
        dataFileInformation.addNameAndValue("----Logical Rectangle",
                                            CziUtilities::qRectToString(m_allFramesPyramidInfo.m_logicalRectangle));
        const int32_t numPyramidLayers(m_allFramesPyramidInfo.getNumberOfPyramidLayers());
        for (int32_t iPyramid = 0; iPyramid < numPyramidLayers; iPyramid++) {
            CaretAssertVectorIndex(m_allFramesPyramidInfo.m_pyramidLayers, iPyramid);
            const PyramidLayer& pl(m_allFramesPyramidInfo.m_pyramidLayers[iPyramid]);
            dataFileInformation.addNameAndValue(("----Pyramid Index "
                                                 + QString::number(iPyramid)),
                                                ("Pixel  W/H: "
                                                 + QString::number(pl.m_pixelWidth)
                                                 + ", "
                                                 + QString::number(pl.m_pixelHeight)
                                                 + "; Load Logical W/H: "
                                                 + QString::number(pl.m_logicalWidthForImageReading)
                                                 + ", "
                                                 + QString::number(pl.m_logicalHeightForImageReading)
                                                 + "; CZI Layer: "
                                                 + QString::number(pl.m_layerInfo.pyramidLayerNo)
                                                 + "; Min Factor: "
                                                 + QString::number(pl.m_layerInfo.minificationFactor)
                                                 + "; Zoom From Low Res: "
                                                 + QString::number(pl.m_zoomLevelFromLowestResolutionImage, 'f', 2)));
        }
    }
    
    const int32_t numScenes = getNumberOfScenes();
    dataFileInformation.addNameAndValue("Number of Scenes", numScenes);
    for (int32_t iScene = 0; iScene < numScenes; iScene++) {
        const CziSceneInfo& cziSceneInfo(m_cziScenePyramidInfos[iScene]);
        dataFileInformation.addNameAndValue(("Scene "
                                             + AString::number(cziSceneInfo.m_sceneIndex)),
                                            QString(""));
        dataFileInformation.addNameAndValue("----Logical Rectangle",
                                            CziUtilities::qRectToString(cziSceneInfo.m_logicalRectangle));
        const int32_t numPyramidLayers(cziSceneInfo.getNumberOfPyramidLayers());
        for (int32_t iPyramid = 0; iPyramid < numPyramidLayers; iPyramid++) {
            CaretAssertVectorIndex(cziSceneInfo.m_pyramidLayers, iPyramid);
            const PyramidLayer& pl(cziSceneInfo.m_pyramidLayers[iPyramid]);
            dataFileInformation.addNameAndValue(("----Pyramid Index "
                                                 + QString::number(iPyramid)),
                                                ("Pixel  W/H: "
                                                 + QString::number(pl.m_pixelWidth)
                                                 + ", "
                                                 + QString::number(pl.m_pixelHeight)
                                                 + "; Load Logical W/H: "
                                                 + QString::number(pl.m_logicalWidthForImageReading)
                                                 + ", "
                                                 + QString::number(pl.m_logicalHeightForImageReading)
                                                 + "; CZI Layer: "
                                                 + QString::number(pl.m_layerInfo.pyramidLayerNo)
                                                 + "; Min Factor: "
                                                 + QString::number(pl.m_layerInfo.minificationFactor)
                                                 + "; Zoom From Low Res: "
                                                 + QString::number(pl.m_zoomLevelFromLowestResolutionImage, 'f', 2)));
        }
    }
    
    const int32_t numPyramidLayers(m_pyramidLayers.size());
    for (int32_t i = 0; i < numPyramidLayers; i++) {
        const PyramidLayer& pl(m_pyramidLayers[i]);
        if (i == m_lowestResolutionPyramidLayerIndex) {
            dataFileInformation.addNameAndValue("---", QString("--- Lowest Resolution Layer ---"));
        }
        dataFileInformation.addNameAndValue(("Pyramid Index "
                                             + QString::number(i)),
                                            ("Scene Index "
                                             + QString::number(pl.m_sceneIndex)
                                             + " W/H: "
                                             + QString::number(pl.m_pixelWidth)
                                             + " x "
                                             + QString::number(pl.m_pixelHeight)
                                             + "; CZI Layer: "
                                             + QString::number(pl.m_layerInfo.pyramidLayerNo)
                                             + "; Min Factor: "
                                             + QString::number(pl.m_layerInfo.minificationFactor)
                                             + "; Zoom From Low Res: "
                                             + QString::number(pl.m_zoomLevelFromLowestResolutionImage, 'f', 2)));
    }
    
    
    if (m_pixelToStereotaxicTransform.m_niftiFile
        && m_pixelToStereotaxicTransform.m_sformMatrix) {
        dataFileInformation.addNameAndValue("NIFTI Pixel To XYZ Transform File",
                                            m_pixelToStereotaxicTransform.m_niftiFile->getFileNameNoPath());
        dataFileInformation.addNameAndValue("NIFTI Pixel To XYZ SFORM",
                                            m_pixelToStereotaxicTransform.m_sformMatrix->toString());
    }
    
    if (m_stereotaxicToPixelTransform.m_niftiFile
        && m_stereotaxicToPixelTransform.m_sformMatrix) {
        dataFileInformation.addNameAndValue("NIFTI XYZ To Pixel Transform File",
                                            m_stereotaxicToPixelTransform.m_niftiFile->getFileNameNoPath());
        dataFileInformation.addNameAndValue("NIFTI XYZ To Pixel SFORM",
                                            m_stereotaxicToPixelTransform.m_sformMatrix->toString());
    }
}

/**
 * @return A pixel index converted from a pixel logical index.
 * @param pixelLogicalIndex
 *    The logical pixel index.
 */
PixelIndex
CziImageFile::pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const
{
    PixelIndex pixelIndex(pixelLogicalIndex.getI() - m_fullResolutionLogicalRect.x(),
                          pixelLogicalIndex.getJ() - m_fullResolutionLogicalRect.y(),
                          pixelLogicalIndex.getK());
    
    return pixelIndex;
}

/**
 * @return A pixel logical index converted from a pixel index.
 * @param pixelIndex
 *    The  pixel index.
 */
PixelLogicalIndex
CziImageFile::pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const
{
    PixelLogicalIndex pixelLogicalIndex(pixelIndex.getI() + m_fullResolutionLogicalRect.x(),
                                        pixelIndex.getJ() + m_fullResolutionLogicalRect.y(),
                                        pixelIndex.getK());
    
    return pixelLogicalIndex;
}

/**
 * @return The default image for all frames
 */
CziImage*
CziImageFile::getDefaultAllFramesImage()
{
   return m_allFramesPyramidInfo.getDefaultImage();
//    return m_defaultAllFramesImage.get();
}

/**
 * @return The default image for all frames
 */
const CziImage*
CziImageFile::getDefaultAllFramesImage() const
{
    return m_allFramesPyramidInfo.getDefaultImage();
//    return m_defaultAllFramesImage.get();
}

/**
 * @return The default image for a frame
 * @param frameIndex
 *    Index of the frame
 */
CziImage*
CziImageFile::getDefaultFrameImage(const int32_t frameIndex)
{
    if (static_cast<int32_t>(m_defaultFrameImages.size()) < getNumberOfFrames()) {
        m_defaultFrameImages.resize(getNumberOfFrames());
    }
    
    CaretAssertVectorIndex(m_defaultFrameImages, frameIndex);
    if (m_defaultFrameImages[frameIndex] == NULL) {
        m_defaultFrameImages[frameIndex].reset(readDefaultFrameImage(frameIndex));
    }
    
    return m_defaultFrameImages[frameIndex].get();
}

/**
 * @return The default image for a frame
 * @param frameIndex
 *    Index of the frame
 */
const CziImage*
CziImageFile::getDefaultFrameImage(const int32_t frameIndex) const
{
    CziImageFile* nonConst(const_cast<CziImageFile*>(this));
    return nonConst->getDefaultFrameImage(frameIndex);
}

/**
 * Read the default image for a frame
 * @param frameIndex
 *    Index of the frame
 */
CziImage*
CziImageFile::readDefaultFrameImage(const int32_t frameIndex)
{
    CaretAssertVectorIndex(m_cziScenePyramidInfos, frameIndex);
    const CziSceneInfo& cziSceneInfo(m_cziScenePyramidInfos[frameIndex]);
    
    const int32_t numPyramidLayers(cziSceneInfo.getNumberOfPyramidLayers());
    const int32_t preferredImageDimension(getPreferencesImageDimension());
    
    int32_t pyramidLayerIndex(-1);
    for (int32_t i = 1; i < numPyramidLayers; i++) {
        if ((preferredImageDimension < cziSceneInfo.m_pyramidLayers[i].m_pixelWidth)
            || (preferredImageDimension < cziSceneInfo.m_pyramidLayers[i].m_pixelHeight)) {
            pyramidLayerIndex = i - 1;
            break;
        }
    }
    if (pyramidLayerIndex < 0) {
        pyramidLayerIndex = numPyramidLayers - 1;
    }
    
    AString errorMessage;
    QRectF regionRectangle(m_cziScenePyramidInfos[frameIndex].m_logicalRectangle);
    CziImage* cziImage(readFramePyramidLayerFromCziImageFile(frameIndex,
                                                             pyramidLayerIndex,
                                                             regionRectangle,
                                                             regionRectangle,
                                                             errorMessage));
    if (cziImage == NULL) {
        CaretLogSevere("Error reading default image for frame "
                       + AString::number(frameIndex));
    }
    
    
    return cziImage;
}

/**
 * @return CZI image for the given tab and overlay
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of the overlay
 */
CziImage*
CziImageFile::getImageForTabOverlay(const int32_t tabIndex,
                                    const int32_t overlayIndex)
{
    if (m_imageLoaderMultiResolution[tabIndex][overlayIndex] != NULL) {
        CziImage* cziOut(m_imageLoaderMultiResolution[tabIndex][overlayIndex]->getImage());
        if (cziOut != NULL) {
            std::cout << "Using multi-res CZI level " << cziOut->m_resolutionChangeModeLevel << std::endl;
            return cziOut;
        }
    }
//    CziImage* image(m_tabOverlayInfo[tabIndex][overlayIndex]->m_cziImage.get());
//    if (image != NULL) {
//        return image;
//    }
    
    return getDefaultAllFramesImage();
}

/**
 * @return CZI image for the given tab and overlay
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of the overlay
 */
const CziImage*
CziImageFile::getImageForTabOverlay(const int32_t tabIndex,
                                    const int32_t overlayIndex) const
{
    if (m_imageLoaderMultiResolution[tabIndex][overlayIndex] != NULL) {
        const CziImage* cziOut(m_imageLoaderMultiResolution[tabIndex][overlayIndex]->getImage());
        if (cziOut != NULL) {
            std::cout << "Using multi-res CZI level " << cziOut->m_resolutionChangeModeLevel << std::endl;
            return cziOut;
        }
    }
//    CziImage* image(m_tabOverlayInfo[tabIndex][overlayIndex]->m_cziImage.get());
//    if (image != NULL) {
//        return image;
//    }
    
    return getDefaultAllFramesImage();
}

/**
 * Update CZI image for the given tab for drawing
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of overlay
 * @param frameIndex
 *    Index of frame
 * @param allFramesFlag
 *    If true, image contains all frames (for CZI this is all scenes)
 * @param resolutionChangeMode
 *    Mode for changing resolutiln (auto/manual)
 * @param pyramidLayerIndex
 *    Index of pyramid layer for manual pyramid layer mode
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 */
void
CziImageFile::updateImageForDrawingInTab(const int32_t tabIndex,
                                      const int32_t overlayIndex,
                                      const int32_t frameIndex,
                                      const bool allFramesFlag,
                                      const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                      const int32_t pyramidLayerIndex,
                                      const GraphicsObjectToWindowTransform* transform)
{
//    if (allFramesFlag) {
//        return getDefaultAllFramesImage();
//    }
//    else {
//        return getDefaultFrameImage(frameIndex);
//    }
    
    CziImage* cziImageOut = getImageForTabOverlay(tabIndex,
                                                  overlayIndex);
    
    autoTwoModePanZoomResolutionChange(cziImageOut,
                                       tabIndex,
                                       overlayIndex,
                                       frameIndex,
                                       allFramesFlag,
                                       transform);

//    CaretAssertToDoWarning();
    return;
    
    //CziImage* cziImageOut(getDefaultAllFramesImage());
//    CziImage* cziImageOut = getImageForTabOverlay(tabIndex,
//                                               overlayIndex);
//
    switch (resolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            CaretAssert(0);
            break;
        case CziImageResolutionChangeModeEnum::AUTO_PYRAMID:
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
            autoTwoModePanZoomResolutionChange(cziImageOut,
                                               tabIndex,
                                               overlayIndex,
                                               frameIndex,
                                               allFramesFlag,
                                               transform);
            cziImageOut = getImageForTabOverlay(tabIndex,
                                                overlayIndex);
            //return cziImageOut;
            return;
            break;
        case CziImageResolutionChangeModeEnum::AUTO_OLD:
            break;
        case CziImageResolutionChangeModeEnum::MANUAL:
            break;
    }
    CaretAssertStdArrayIndex(m_pyramidLayerIndexInTabs, tabIndex);

    //const int32_t pyramidLayerIndex = m_pyramidLayerIndexInTabs[tabIndex];
    
    /*
     * Lowest pyramid layer is always the default image
     */
    if (pyramidLayerIndex == m_lowestResolutionPyramidLayerIndex) {
        cziImageOut = getDefaultAllFramesImage();
    }
    
    if (cziImageOut == NULL) {
        /*
         * If pyramid level has not changed and have valid image in tab use it
         */
        CaretAssertStdArrayIndex(m_tabCziImagePyramidLevelChanged, tabIndex);
        CaretAssertStdArrayIndex(m_tabCziImages, tabIndex);
        if (  ! m_tabCziImagePyramidLevelChanged[tabIndex]) {
            if (m_tabCziImages[tabIndex] != NULL) {
                cziImageOut = m_tabCziImages[tabIndex].get();
            }
        }
        
        if (cziImageOut == NULL) {
            /*
             * In some instance, a new image is not loaded and
             * 'loadImageForPyramidLayer' returns the current (old)
             * image.  So, do not want to 'reset' with same value
             * as it will delete the image and likely cause a crash.
             */
            CziImage* oldCziImage = m_tabCziImages[tabIndex].get();
            CziImage* newCziImage = loadImageForPyrmaidLayer(tabIndex,
                                                             overlayIndex,
                                                             transform,
                                                             pyramidLayerIndex);
            if (newCziImage != oldCziImage) {
                m_tabCziImages[tabIndex].reset(newCziImage);
            }
            
            if (m_tabCziImages[tabIndex]) {
                cziImageOut = m_tabCziImages[tabIndex].get();
            }
            
            if (cziImageOut == NULL) {
                /*
                 * Failed so go back to default image
                 */
                m_pyramidLayerIndexInTabs[tabIndex] = m_lowestResolutionPyramidLayerIndex;
                //return getDefaultAllFramesImage();
                return;
            }
        }
    }
    
    if (cziImageOut != NULL) {
        /*
         * For AUTO mode
         */
        switch (resolutionChangeMode) {
            case CziImageResolutionChangeModeEnum::INVALID:
                break;
            case CziImageResolutionChangeModeEnum::AUTO_OLD:
                autoModeZoomOnlyResolutionChange(tabIndex,
                                                 overlayIndex,
                                                 transform);
                break;
            case CziImageResolutionChangeModeEnum::AUTO_PYRAMID:
                autoModePanZoomResolutionChange(cziImageOut,
                                                tabIndex,
                                                overlayIndex,
                                                transform);
                break;
            case CziImageResolutionChangeModeEnum::AUTO2:
                break;
            case CziImageResolutionChangeModeEnum::MANUAL:
                break;
        }
    }
    
   // return cziImageOut;
    return;
}

/**
 * @return The graphics primitive for drawing the image as a texture in media drawing model.
 * @param tabIndex
 *    Index of tab where image is drawn
 * @param overlayIndex
 *    Index of the overlay
 */
GraphicsPrimitiveV3fT2f*
CziImageFile::getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                  const int32_t overlayIndex) const
{
    const CziImage* cziImage(getImageForTabOverlay(tabIndex,
                                                   overlayIndex));
    CaretAssert(cziImage);
    return cziImage->getGraphicsPrimitiveForMediaDrawing();
}

/**
 * Convert a viewport XY-coordinate to a model space coordinate and return in a pixel index.
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @param x
 *    X-coordinate
 * @param y
 *    Y-coordinate
 * @return
 *    Pixel index containing coordinate in model space.
 */
PixelLogicalIndex
CziImageFile::viewportXyToPixelLogicalIndex(const GraphicsObjectToWindowTransform* transform,
                                            const float x,
                                            const float y)
{
    float modelXYZ[3];
    transform->inverseTransformPoint(x,
                                     y,
                                     0.0,
                                     modelXYZ);
    const PixelLogicalIndex pixelLogicalIndex(modelXYZ);
    return pixelLogicalIndex;
}

/**
 * @return Area of the intersected region of the two rectangles
 * @param rectOne
 *    First rectangle
 * @param rectTwo
 *    Second rectangle
 */
float
CziImageFile::getIntersectedArea(const QRectF& rectOne,
                                 const QRectF& rectTwo) const
{
    float area(0.0);
    
    if (rectOne.isValid()
        && rectTwo.isValid()) {
        const QRectF intersectionRect(rectOne.intersected(rectTwo));
        if (intersectionRect.isValid()) {
            area = (intersectionRect.width()
                    * intersectionRect.height());
        }
    }

    return area;
}

/**
 * Process change in resoluion for auto mode for zooming and panning for Auto TWO mode
 * @param cziImage
 *    Current CZI image.  May be image unique to tab or the default image
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of overlay
 * @param frameIndex
 *    Index of frame selected
 * @param allFramesFlag
 *    True if all frames selected, else false.
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @return
 *    Pyramid layer index selected
 */

void
CziImageFile::autoTwoModePanZoomResolutionChange(const CziImage* cziImage,
                                        const int32_t tabIndex,
                                        const int32_t overlayIndex,
                                        const int32_t frameIndex,
                                        const bool allFramesFlag,
                                        const GraphicsObjectToWindowTransform* transform)
{
    CaretAssert(cziImage);

    if ( ! m_imageLoaderMultiResolution[tabIndex][overlayIndex]) {
        m_imageLoaderMultiResolution[tabIndex][overlayIndex].reset(new CziImageLoaderMultiResolution());
        m_imageLoaderMultiResolution[tabIndex][overlayIndex]->initialize(tabIndex,
                                                                         overlayIndex,
                                                                         this);
    }
    CziImage* newCziImage(NULL);
    m_imageLoaderMultiResolution[tabIndex][overlayIndex]->updateImage(cziImage,
                                                                      frameIndex,
                                                                      allFramesFlag,
                                                                      transform);
    

//    need to display newly loaded images NOT Default image
//    if (newCziImage != NULL) {
//        m_tabOverlayInfo[tabIndex][overlayIndex]->m_cziImage.reset(newCziImage);
//    }
    return;

//    /*
//     * CZI uses "Logical Coordinates" that have the origin at the top left
//     * corner of the image.  The origin is typically non-zero with both
//     * the x- and y-coordinates being negative.  Postive-X goes to the right
//     * and positive-Y is down.
//     *
//     * The "Available Rectangle" is the coordinates of the image that is
//     * available and can be displayed.
//     *
//     * A CZI file may contain multiple "Scenes".  A CZI "Scene"
//     * is a separate image that is stored in the file.  In Workbench
//     * these separate images are selected as "Frames" in a Layer.
//     */
//    QRectF availableLogicalRect(m_fullResolutionLogicalRect);
//    if ( ! allFramesFlag) {
//        if ((frameIndex >= 0)
//            && (frameIndex < getNumberOfFrames())) {
//            CaretAssertVectorIndex(m_cziSceneInfos, frameIndex);
//            availableLogicalRect = m_cziSceneInfos[frameIndex].m_logicalRectangle;
//        }
//    }
//
////    CziImage* cziImage(m_tabOverlayInfo[tabIndex][overlayIndex]->m_cziImage.get());
////    if (cziImage == NULL) {
////        cziImage = getDefaultImage();
////    }
//
//    /*
//     * After getting the viewport enlarge it a little bit.
//     * When the user pans the image, this will cause new image data
//     * to be loaded as the edge of the current image is about to
//     * be panned into the viewport.
//     *
//     * If we do not enlarge the viewport, new image data is not loaded
//     * until the edge of the image is moved into the viewport and this
//     * results in a small amount of the background becoming visible
//     * (until the new image is loaded).
//     */
//    const std::array<float, 4> viewportArray(transform->getViewport());
//    const QRectF viewport(viewportArray[0],
//                    viewportArray[1],
//                    viewportArray[2],
//                    viewportArray[3]);
//    const float mv(10);
//    const QMarginsF margins(mv, mv, mv, mv);
//    const QRectF viewportWithMargins = viewport.marginsAdded(margins);
//
//    /*
//     * Logical oordinate at Top Left and Bottom Right of Viewport
//     */
//    const PixelIndex viewportLogicalTopLeft(viewportXyToPixelLogicalIndex(transform,
//                                                                        viewport.x(),
//                                                                        viewport.y() + viewport.height()));
//    const PixelIndex viewportLogicalBottomRight(viewportXyToPixelLogicalIndex(transform,
//                                                                            viewport.x() + viewport.width(),
//                                                                            viewport.y()));
//
////    const PixelIndex pixelIndexBottomRight(viewportBottomRightWindowCoordinate);
////    const PixelIndex windowLogicalBottomRight(cziImage->transformPixelIndexToSpace(pixelIndexBottomRight,
////                                                                                   CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT,
////                                                                                   CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT));
//    /*
//     * CZI Logical coordinates of viewport (portion of CZI image that fills the viewport)
//     */
//    const QRectF viewportLogicalRect(viewportLogicalTopLeft.getI(),
//                                     viewportLogicalTopLeft.getJ(),
//                                     viewportLogicalBottomRight.getI() - viewportLogicalTopLeft.getI(),
//                                     viewportLogicalBottomRight.getJ() - viewportLogicalTopLeft.getJ());
//    bool cziDebugFlag(true);
//    if (cziDebugFlag) {
//        std::cout << "Viewport Logical Top Left: " << viewportLogicalTopLeft.toString() << std::endl;
//////        std::cout << "Pixel Index Top Left (origin top left): " << pixelIndexTopLeft.toString() << std::endl;
////        std::cout << "Pixel Index Top Left (origin top left): "
////        << cziImage->transformPixelIndexToSpace(pixelIndexTopLeft,
////                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
////                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT).toString() << std::endl;
//        std::cout << "Viewport Logical Bottom Right: " << viewportLogicalBottomRight.toString() << std::endl;
//////        std::cout << "Pixel Index Bottom Right (origin top left): " << pixelIndexBottomRight.toString() << std::endl;
////        std::cout << "Pixel Index Bottom Right (origin top left): "
////        << cziImage->transformPixelIndexToSpace(pixelIndexBottomRight,
////                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
////                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT).toString() << std::endl;
//        std::cout << "Viewport Logical Rect: " << CziUtilities::qRectToString(viewportLogicalRect) << std::endl;
//        std::cout << "Available Logical Rect: " << CziUtilities::qRectToString(availableLogicalRect) << std::endl;
//        std::cout << "Image Logical Rect: " << CziUtilities::qRectToString(cziImage->m_imageDataLogicalRect) << std::endl;
//
//        const QRectF imageIntersectWindowRect(viewportLogicalRect.intersected(cziImage->m_imageDataLogicalRect));
//        const float imageInWindowArea(imageIntersectWindowRect.width() * imageIntersectWindowRect.height());
//        const float imageArea(cziImage->m_imageDataLogicalRect.width()* cziImage->m_imageDataLogicalRect.height());
//        const float viewedPercentage((imageArea > 0.0f)
//                                     ? (imageInWindowArea / imageArea)
//                                     : imageArea);
//        std::cout << "Image Viewed Percentage: " << viewedPercentage << std::endl;
//    }
//
//
//    /*
//     * Find the area of the of current image that intersects the viewport
//     */
//    const float imageIntersectsViewportArea(getIntersectedArea(cziImage->m_imageDataLogicalRect,
//                                                               viewportLogicalRect));
//
//    /*
//     * Find the area of the available image that intersects the viewport
//     */
//    const float availableImageIntersectsViewportArea(getIntersectedArea(availableLogicalRect,
//                                                                        viewportLogicalRect));
//
//    if (cziDebugFlag) {
//        std::cout << "Area of image that intersects viewport: " << imageIntersectsViewportArea << std::endl;
//        std::cout << "Area of availble image that intersects viewport: " << availableImageIntersectsViewportArea << std::endl;
//    }
//
//    bool reloadFlag(false);
//
//    /*
//     * If the available image area overlaps more of the viewport than the
//     * current image, need to reload as more of the image can be seen.
//     */
//    if (availableImageIntersectsViewportArea > imageIntersectsViewportArea) {
//        std::cout << "RELOAD: available image greater than current image" << std::endl;
//        reloadFlag = true;
//    }
//
//    if ( ! reloadFlag) {
//        /*
//         * Height of viewport in logical coordinates
//         */
//        const double viewportLogicalHeight(viewportLogicalRect.height());
//
//        /*
//         * Height of viewport in pixels
//         */
//        const double viewportPixelHeight(viewport.height());
//
//        std::cout << "Viewport LOGICAL height: " << viewportLogicalHeight << std::endl;
//        std::cout << "Viewport PIXEL height:   " << viewportPixelHeight << std::endl;
//
//        if ((viewportLogicalHeight > 0.0)
//            && (viewportPixelHeight > 0.0)) {
//            const float logicalToPixelRatio(viewportLogicalHeight
//                                            / viewportPixelHeight);
//
//            /*
//             * If ratio is:
//             * (1) Greater than one; logical element is smaller than pixel
//             * (2) Less than one; logical element is bigger than pixel
//             * (3) Equal to 1; logical element is same size as pixel
//             *
//             * When logical element is bigger than pixel, we should load
//             * higher resolution data as user has zoomed in.
//             */
//            const float threshold(2.0);
//            if (logicalToPixelRatio < threshold) {
//                std::cout << "RELOAD: logical element is larger than pixel" << std::endl;
//                reloadFlag = true;
//            }
//        }
//    }
//
//    if (reloadFlag) {
//        CziImage* newImage(NULL);
//
//        bool doneFlag = false;
//        if (allFramesFlag) {
//            if (viewportLogicalRect.contains(m_fullResolutionLogicalRect)) {
//                /*
//                 * Entire full image is within window, can use default image
//                 */
//                doneFlag = true;
//            }
//        }
//
//        if ( ! doneFlag) {
//            /*
//             * Find portion of available image region that intersects the viewport
//             */
//            const QRectF imageIntersectViewportLogicalRect(availableLogicalRect.intersected(viewportLogicalRect));
//
////            const QRectF imageIntersectFullImageLogicalRect(imageIntersectViewportLogicalRect.intersected(m_fullResolutionLogicalRect));
////            if (imageIntersectFullImageLogicalRect == m_fullResolutionLogicalRect) {
////
////            }
//            QRectF imageRegion(imageIntersectViewportLogicalRect);
//
//            if (cziImage->m_imageDataLogicalRect == imageRegion) {
//                /*
//                 * Do not need to reload since image region has not changed
//                 */
//                doneFlag = true;
//            }
//
//            if ( ! doneFlag) {
//                std::cout << "LOADING IMAGE: " << CziUtilities::qRectToString(imageRegion) << std::endl;
//                AString errorMessage;
//                newImage = readFromCziImageFile(imageRegion,
//                                                        getPreferencesImageDimension(),
//                                                        errorMessage);
//                if (newImage == NULL) {
//                    CaretLogSevere(errorMessage);
//                }
//            }
//        }
//
//        m_tabOverlayInfo[tabIndex][overlayIndex]->m_cziImage.reset(newImage);
//        if (newImage != NULL) {
//            m_tabOverlayInfo[tabIndex][overlayIndex]->m_logicalRect = newImage->m_imageDataLogicalRect;
//        }
//        else {
//            m_tabOverlayInfo[tabIndex][overlayIndex]->m_logicalRect = QRectF();
//        }
//    }
//
//
//
//
//    /*
//     * (1) Find intersection of currently loaded image region with the viewport region
//     * (2) Find amount of viewport that overlaps the current image region
//     */
//    const QRectF viewportIntersectImageRect(cziImage->m_imageDataLogicalRect.intersected(viewportLogicalRect));
//    const float viewportInImageArea(viewportIntersectImageRect.width() * viewportIntersectImageRect.height());
//    const float viewportArea(viewportLogicalRect.width() * viewportLogicalRect.height());
//    const float viewportRoiPercentage((viewportArea > 0.0f)
//                                      ? (viewportInImageArea / viewportArea)
//                                      : viewportArea);
//    if (cziDebugFlag) {
//        std::cout << "Window Image Percentage: " << viewportRoiPercentage << std::endl;
//    }
//
//    /*
//     * (1) Find intersection of full resolution image region with the viewport region
//     * (2) Find amount of viewport that overlaps the full image region
//     */
//    const QRectF viewportIntersectFullImageRect(availableLogicalRect.intersected(viewportLogicalRect));
//    const float viewportInFullResImageArea(viewportIntersectFullImageRect.width() * viewportIntersectFullImageRect.height());
//    const float viewportFullResPercentage(viewportArea
//                                          ? (viewportInFullResImageArea / viewportArea)
//                                          : viewportArea);
//    if (cziDebugFlag) {
//        std::cout << "Window Full Res Image Percentage: " << viewportFullResPercentage << std::endl;
//    }
//
//    if (viewportFullResPercentage > 0.0) {
//        /*
//         * Get ratio of current image ROI and viewport full res image
//         * When less that one, the current image has been panned so
//         * that there is a gap on a side (or sides) of the viewport
//         * that can be filled by loading new image data
//         */
//        const float ratio(viewportRoiPercentage / viewportFullResPercentage);
//        if (cziDebugFlag) {
//            std::cout << "Viewed vs Available Percentage: " << ratio << std::endl;
//        }
//
//        /*
//         * If parts of the viewport do not contain image data but
//         * there is image data available, reload image data to
//         * cover the entire viewport.
//         */
//        const float reloadThreshold(0.99);
//        if (ratio < reloadThreshold) {
//            /*
//             * Cause reloading of image data which should fill the window
//             */
//            if (cziDebugFlag) {
//                std::cout << "...Reloading Image Data" << std::endl;
//            }
//            std::cout << "Need to load image for auto TWO mode" << std::endl;
//            //reloadPyramidLayerInTab(tabIndex);
//        }
//    }
//    if (cziDebugFlag) {
//        std::cout << std::endl;
//    }
}

/**
 * Process change in resoluion for auto mode for zooming and panning
 * @param cziImage
 *    Current CZI image
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of overlay
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @return
 *    Pyramid layer index selected
 */
int32_t
CziImageFile::autoModePanZoomResolutionChange(const CziImage* cziImage,
                                              const int32_t tabIndex,
                                              const int32_t overlayIndex,
                                              const GraphicsObjectToWindowTransform* transform)
{
    const int32_t previousPyramidLayerIndex = getPyramidLayerIndexForTabOverlay(tabIndex,
                                                                                overlayIndex);
    
    const int32_t pyramidLayerIndex = autoModeZoomOnlyResolutionChange(tabIndex,
                                                                       overlayIndex,
                                                                       transform);
    
    if (pyramidLayerIndex <= m_lowestResolutionPyramidLayerIndex) {
        /*
         * Lowest resolution shows entire image
         */
        return pyramidLayerIndex;
    }
    
    if (pyramidLayerIndex != previousPyramidLayerIndex) {
        /*
         * Resolution has changed which will cause loading of new image data
         * and the new data may fill the window.
         */
        return pyramidLayerIndex;
    }
    
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
                                     viewport.y(),
                                     0.0,
                                     viewportTopLeftWindowCoordinate);
    const PixelIndex pixelIndexTopLeft(viewportTopLeftWindowCoordinate);
    const PixelIndex windowLogicalTopLeft(cziImage->transformPixelIndexToSpace(pixelIndexTopLeft,
                                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT,
                                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT));
    
    /*
     * Bottom Right Corner of Window
     * 'inverseTransformPoint()' transforms from window coordinates to the ortho's pixel index with
     * origin at bottom left.
     * 'transformPixelIndexToSpace' transforms pixel index to CZI 'logical coordinates'
     */
    float viewportBottomRightWindowCoordinate[3];
    transform->inverseTransformPoint(viewport.x() + viewport.width(),
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     viewportBottomRightWindowCoordinate);
    const PixelIndex pixelIndexBottomRight(viewportBottomRightWindowCoordinate);
    const PixelIndex windowLogicalBottomRight(cziImage->transformPixelIndexToSpace(pixelIndexBottomRight,
                                                                                   CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT,
                                                                                   CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT));
    /*
     * CZI Logical coordinates of viewport (portion of CZI image that fills the viewport)
     */
    const QRectF viewportFullResLogicalRect(windowLogicalTopLeft.getI(),
                                            windowLogicalTopLeft.getJ(),
                                            windowLogicalBottomRight.getI() - windowLogicalTopLeft.getI(),
                                            windowLogicalBottomRight.getJ() - windowLogicalTopLeft.getJ());
    if (cziDebugFlag) {
        std::cout << "Pixel Index Top Left (origin top left): " << pixelIndexTopLeft.toString() << std::endl;
        std::cout << "Pixel Index Top Left (origin top left): "
        << cziImage->transformPixelIndexToSpace(pixelIndexTopLeft,
                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT).toString() << std::endl;
        std::cout << "Pixel Index Bottom Right (origin bottom left): " << pixelIndexBottomRight.toString() << std::endl;
        std::cout << "Pixel Index Bottom Right (origin top left): "
        << cziImage->transformPixelIndexToSpace(pixelIndexBottomRight,
                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT).toString() << std::endl;
        std::cout << "Window Logical Rect: " << CziUtilities::qRectToString(viewportFullResLogicalRect) << std::endl;
        std::cout << "Full Res Logical Rect: " << CziUtilities::qRectToString(m_fullResolutionLogicalRect) << std::endl;
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
    const QRectF viewportIntersectFullImageRect(m_fullResolutionLogicalRect.intersected(viewportFullResLogicalRect));
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
            reloadPyramidLayerInTab(tabIndex);
        }
    }
    if (cziDebugFlag) {
        std::cout << std::endl;
    }
    
    return pyramidLayerIndex;
}

/**
 * Process change in resoluion for auto mode for zooming only
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of overlay
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @return
 *    Pyramid layer index selected
 */
int32_t
CziImageFile::autoModeZoomOnlyResolutionChange(const int32_t tabIndex,
                                               const int32_t overlayIndex,
                                               const GraphicsObjectToWindowTransform* transform)
{
    /*
     * Compute pixel height of the high-resolution image when drawn in the window.
     * The size of the image, in pixels when drawn, determines when to switch to
     * lower or higher resolution image.
     */
    const float imageBottomLeftPixel[3] { 0.0, static_cast<float>(m_fullResolutionLogicalRect.height()), 0.0 };
    const float imageTopLeftPixel[3] { 0.0, 0.0, 0.0 };
    float imageBottomLeftWindow[3];
    float imageTopLeftWindow[3];
    transform->transformPoint(imageBottomLeftPixel, imageBottomLeftWindow);
    transform->transformPoint(imageTopLeftPixel, imageTopLeftWindow);
    const float drawnPixelHeight = imageTopLeftWindow[1] - imageBottomLeftWindow[1];
    
    int32_t pyramidLayerForScaling = m_highestResolutionPyramidLayerIndex;
    for (int32_t i = m_lowestResolutionPyramidLayerIndex;
         i <= m_highestResolutionPyramidLayerIndex;
         i++) {
        if (drawnPixelHeight < m_pyramidLayers[i].m_pixelHeight) {
            pyramidLayerForScaling = i;
            break;
        }
    }
    if (pyramidLayerForScaling != getPyramidLayerIndexForTabOverlay(tabIndex,
                                                                    overlayIndex)) {
        setPyramidLayerIndexForTab(tabIndex,
                                   pyramidLayerForScaling);
    }
    
    return pyramidLayerForScaling;
}

/**
 * Load a image from the given pyramid layer for the center of the tab region defined by the transform
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of the overlay
 * @param transform
 *    Transform from the tab where image is drawn
 * @param pyramidLayerIndexIn
 *    Index of the pyramid layer
 */
CziImage*
CziImageFile::loadImageForPyrmaidLayer(const int32_t tabIndex,
                                       const int32_t overlayIndex,
                                       const GraphicsObjectToWindowTransform* transform,
                                       const int32_t pyramidLayerIndexIn)
{
    if (m_pyramidLayers.empty()) {
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
    else if (pyramidLayerIndex >= static_cast<int32_t>(m_pyramidLayers.size())) {
        pyramidLayerIndex = m_pyramidLayers.size() - 1;
        CaretLogSevere("Attempt to load invalid pyramid layer="
                       + AString::number(pyramidLayerIndexIn));
    }
    
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
    PixelIndex imagePixelIndex(modelXYZ[0], modelXYZ[1], 0.0f);
    
    float modelXYZ2[3];
    GraphicsUtilitiesOpenGL::unproject(vpCenterXYZ[0], vpCenterXYZ[1], modelXYZ2);
//    std::cout << "Model XYZ: " << AString::fromNumbers(modelXYZ, 3, "f") << std::endl;
//    std::cout << "   Model 2 XYZ: " << AString::fromNumbers(modelXYZ2, 3, "f") << std::endl;

    CziImage* oldCziImage(getImageForTabOverlay(tabIndex,
                                                overlayIndex));
    CaretAssert(oldCziImage);
    PixelIndex fullImagePixelIndex = imagePixelIndex;
    PixelIndex fullResolutionLogicalPixelIndex = imagePixelIndex;
    
    /*
     * Get preferred image size from preferences
     */
    const int32_t preferredImageDimension(getPreferencesImageDimension());
    
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
    const int32_t halfLoadImageWidth(loadImageWidth / 2);
    const int32_t halfLoadImageHeight(loadImageHeight / 2);
    QRectF imageRegionRect(fullResolutionLogicalPixelIndex.getI() - halfLoadImageWidth,
                           fullResolutionLogicalPixelIndex.getJ() - halfLoadImageHeight,
                           loadImageWidth,
                           loadImageHeight);
    if (cziDebugFlag) {
        std::cout << "Loading width/height: " << loadImageWidth << ", " << loadImageHeight << " for aspect: " << viewportAspectRatio << std::endl;
    }
    
    /*
     * Limit region for reading to valid area of image
     */
    if (m_fullResolutionLogicalRect.intersects(imageRegionRect)) {
        imageRegionRect = m_fullResolutionLogicalRect.intersected(imageRegionRect);
    }
    else {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(imageRegionRect)
                       + " for file "
                       + getFileNameNoPath()
                       + " does not overlap the full resolution rectangle="
                       + CziUtilities::qRectToString(m_fullResolutionLogicalRect));
        return NULL;
    }
    
    /*
     * When reading, the size is a "logical size" not PIXELS.  So, need to convert
     * the pixel size to a logical size
     */
    const int32_t centerX = imageRegionRect.center().rx();
    const int32_t centerY = imageRegionRect.center().ry();
    int32_t roiWidth = imageRegionRect.width();
    int32_t roiHeight = imageRegionRect.height();
    pixelSizeToLogicalSize(m_pyramidLayers,
                           pyramidLayerIndex,
                           roiWidth,
                           roiHeight);
    QRectF adjustedRect(imageRegionRect);
    adjustedRect.setX(centerX - (roiWidth / 2));
    adjustedRect.setY(centerY - (roiHeight / 2));
    adjustedRect.setWidth(roiWidth);
    adjustedRect.setHeight(roiHeight);
    
    /*
     * May need to move or clip to stay in the logical space
     */
    adjustedRect = moveAndClipRectangle(adjustedRect);
    if (cziDebugFlag) {
        std::cout << "Adjusted width/height: " << roiWidth << ", " << roiHeight << std::endl;
        std::cout << "Original ROI: " << CziUtilities::qRectToString(imageRegionRect) << std::endl;
        std::cout << "   Adjusted for w/h: " << CziUtilities::qRectToString(adjustedRect) << std::endl;
        std::cout << "   Old Center: " << centerX << ", " << centerY << std::endl;
        std::cout << "   New Center: " << adjustedRect.center().x() << ", " << adjustedRect.center().y() << std::endl;
    }
    if ( ! adjustedRect.isValid()) {
        return NULL;
    }
    
    /*
     * If the region has not changed, do not need to load data
     */
    if (oldCziImage->m_imageDataLogicalRect == adjustedRect) {
        return oldCziImage;
    }
    
    AString errorMessage;
    cziImageOut = readPyramidLayerFromCziImageFile(pyramidLayerIndex, imageRegionRect, adjustedRect, errorMessage);
    if (cziImageOut == NULL) {
        CaretLogSevere("Loading Pyramid level="
                       + AString::number(pyramidLayerIndex)
                       + " for rectangle="
                       + CziUtilities::qRectToString(imageRegionRect)
                       + " for file "
                       + getFileNameNoPath()
                       + " error: "
                       + errorMessage);
    }
    
    CaretAssertStdArrayIndex(m_tabCziImagePyramidLevelChanged, tabIndex);
    m_tabCziImagePyramidLevelChanged[tabIndex] = false;
    
    return cziImageOut;
}

/**
 * Move and/or clip the rectangle so all of it (or as much as possible) is within
 * the full resolution logical space.
 */
QRectF
CziImageFile::moveAndClipRectangle(const QRectF& rectangleIn)
{
    return rectangleIn;
    
    QRectF rectangle(rectangleIn);
    
    const bool moveToKeepFullWidthHeightFlag(false);
    if (moveToKeepFullWidthHeightFlag) {
        float dx(0.0);
        if (rectangle.left() < m_fullResolutionLogicalRect.left()) {
            dx = (m_fullResolutionLogicalRect.left() - rectangle.left());
        }
        else if (rectangle.right() > m_fullResolutionLogicalRect.right()) {
            dx = -(rectangle.right() - m_fullResolutionLogicalRect.right());
        }
        
        /*
         * With QRect, the origin is at the top left corner.  Thus,
         * bottom is always GREATER than top.
         */
        float dy(0.0);
        if (rectangle.bottom() > m_fullResolutionLogicalRect.bottom()) {
            dy = (rectangle.bottom() - m_fullResolutionLogicalRect.bottom());
        }
        else if (rectangle.top() < m_fullResolutionLogicalRect.top()) {
            dy = -(m_fullResolutionLogicalRect.top() - rectangle.top());
        }
        
        if ((dx != 0.0)
            || (dy != 0.0)) {
            std::cout << "Translate by " << dx << ", " << dy << std::endl;
            std::cout << "       Full: " << CziUtilities::qRectToLrbtString(m_fullResolutionLogicalRect) << std::endl;
            std::cout << "   Rect was: " << CziUtilities::qRectToLrbtString(rectangle) << std::endl;
            rectangle.translate(dx, dy);
            std::cout << "        now: " << CziUtilities::qRectToLrbtString(rectangle) << std::endl;
        }
    }
    
    if (m_fullResolutionLogicalRect.intersects(rectangle)) {
        rectangle = m_fullResolutionLogicalRect.intersected(rectangle);
    }
    else {
        /*
         * No intersection with full resolution image, return invalid rectangle
         */
        CaretLogWarning("No intersection with full resolution image");
        return QRect();
    }
    
    return rectangle;
}


/**
 * @return True if the given pixel index is valid for the CZI image file (may be outside of currently loaded sub-image)
 * @param tabIndex
 *    Index of the tab.
 *@param overlayIndex
 *    Index of overlay
 * @param pixelIndexOriginAtTopLeft
 *    Image of pixel with origin (0, 0) at the top left
 */
bool
CziImageFile::isPixelIndexValid(const int32_t tabIndex,
                                const int32_t overlayIndex,
                                const PixelIndex& pixelIndexOriginAtTopLeft) const
{
    return isPixelIndexValid(tabIndex, overlayIndex, pixelIndexToPixelLogicalIndex(pixelIndexOriginAtTopLeft));
}

/**
 * @return True if the given pixel index is valid for the image in the given tab
 * @param tabIndex
 *    Index of the tab.
 *@param overlayIndex
 *    Index of overlay
 * @param pixelLogicalIndex
 *    Pixel logical index
 */
bool
CziImageFile::isPixelIndexValid(const int32_t /*tabIndex*/,
                                const int32_t /*overlayIndex*/,
                                const PixelLogicalIndex& pixelLogicalIndex) const
{
    const float i(pixelLogicalIndex.getI());
    const float j(pixelLogicalIndex.getJ());
    
    if ((i >= m_fullResolutionLogicalRect.left())
        && (i < m_fullResolutionLogicalRect.right())
        && (j >= m_fullResolutionLogicalRect.top())
        && (j < m_fullResolutionLogicalRect.bottom())) {
        return true;
    }

    return false;
}

/**
 * @return True if the given pixel index is valid
 * @param pixelLogicalIndex
 *    Pixel logical index
 */
bool
CziImageFile::isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const
{
    const float i(pixelLogicalIndex.getI());
    const float j(pixelLogicalIndex.getJ());
    
    if ((i >= m_fullResolutionLogicalRect.left())
        && (i < m_fullResolutionLogicalRect.right())
        && (j >= m_fullResolutionLogicalRect.top())
        && (j < m_fullResolutionLogicalRect.bottom())) {
        return true;
    }
    
    return false;
}

/**
 * @return True if the given pixel index is valid for the image in the given tab
 * @param pixelIndex
 *     Image of pixel in FULL RES image
 */
bool
CziImageFile::isPixelIndexFullResolutionValid(const PixelIndex& pixelIndex) const
{
    if ((pixelIndex.getI() >= 0)
        && (pixelIndex.getI() < getWidth())
        && (pixelIndex.getJ() >= 0)
        && (pixelIndex.getJ() < getHeight())) {
        return true;
    }
    return false;
}

/**
 * Get the pixel RGBA at the given pixel I and J.
 *
 * @param tabIndex
 *    Index of the tab.
 * @param overlayIndex
 *    Index of overlay
 * @param pixelLogicalIndex
 *     Logical pixel index
 * @param pixelRGBAOut
 *     RGBA at Pixel I, J
 * @return
 *     True if valid, else false.
 */
bool
CziImageFile::getPixelRGBA(const int32_t tabIndex,
                           const int32_t overlayIndex,
                           const PixelLogicalIndex& pixelLogicalIndex,
                           uint8_t pixelRGBAOut[4]) const
{
    const CziImage* cziImage = getImageForTabOverlay(tabIndex,
                                                     overlayIndex);
    CaretAssert(cziImage);
    if (cziImage->getPixelRGBA(pixelLogicalIndex,
                               pixelRGBAOut)) {
        return true;
    }
    
    return false;
}

/**
 * @return The dimension (width/height) for loading image data from preferences
 */
int32_t
CziImageFile::getPreferencesImageDimension() const
{
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    int32_t preferredImageDimension(2048);
    if (prefs != NULL) {
        preferredImageDimension = prefs->getCziDimension();
    }
    CaretAssert(preferredImageDimension >= 512);
    return preferredImageDimension;
}

/**
 * @return The RGB background color for images from preferences
 */
std::array<float, 3>
CziImageFile::getPreferencesImageBackgroundRGB() const
{
    std::array<float, 3> rgb;
    
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    if (prefs != NULL) {
        uint8_t backgroundColor[3];
        prefs->getBackgroundAndForegroundColors()->getColorBackgroundMediaView(backgroundColor);
        rgb = BackgroundAndForegroundColors::toFloatRGB(backgroundColor);
    }
    
    return rgb;
}


/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
CziImageFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                  SceneClass* sceneClass)
{
    MediaFile::saveFileDataToScene(sceneAttributes,
                                   sceneClass);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
CziImageFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                       const SceneClass* sceneClass)
{
    MediaFile::restoreFileDataFromScene(sceneAttributes,
                                        sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}







CziImage*
CziImageFile::CziSceneInfo::getDefaultImage() {
    if ( ! m_defaultImage) {
        if ( ! m_defaultImageErrorFlag) {
            if (m_parentCziImageFile != NULL) {
                try {
                    if (m_sceneIndex >= 0) {
                        m_defaultImage.reset(m_parentCziImageFile->readDefaultFrameImage(m_sceneIndex));
                    }
                    else {
                        m_defaultImage.reset(m_parentCziImageFile->readDefaultImage());
                    }
                }
                catch (const DataFileException& dfe) {
                    m_defaultImageErrorFlag = true;
                    throw dfe;
                }
            }
        }
    }
    return m_defaultImage.get();
}
