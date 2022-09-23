
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

#include <algorithm>
#include <cmath>
#include <limits>

#include <QImage>
#include <QImageWriter>

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
CziImageFile::CziImageFile(const ParentType parentType)
: MediaFile(DataFileTypeEnum::CZI_IMAGE_FILE,
            parentType)
{
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        for (int32_t iOverlay = 0; iOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; iOverlay++) {
            m_tabOverlayInfo[iTab][iOverlay].reset(new TabOverlayInfo(this, iTab, iOverlay));
        }
    }

    resetPrivate();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());

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
    
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    if (prefs != NULL) {
        m_maximumImageDimension = prefs->getCziDimension();
    }
    else {
        m_maximumImageDimension = 2048;
    }

    m_allFramesPyramidInfo = CziSceneInfo();
    m_cziScenePyramidInfos.clear();
    m_scalingTileAccessor.reset();
    m_pyramidLayerTileAccessor.reset();

    if (m_reader) {
        m_reader->Close();
    }
    m_reader.reset();
    
    m_stream.reset();

    m_pixelSizeMmX = 1.0f;
    m_pixelSizeMmY = 1.0f;
    m_pixelSizeMmZ = 1.0f;
    m_fileMetaData.reset(new GiftiMetaData());
    m_fullResolutionLogicalRect = QRectF();
    m_pixelToStereotaxicTransform = NiftiTransform();
    m_stereotaxicToPixelTransform = NiftiTransform();
    m_imagePlane.reset();
    m_imagePlaneInvalid = false;
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        for (int32_t iOverlay = 0; iOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; iOverlay++) {
            CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, iTab);
            CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, iOverlay);
            m_tabOverlayInfo[iTab][iOverlay]->resetContent();
        }
    }
    
    resetMatrices();
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
    
    resetPrivate();
    
    /*
     * File is now closed
     */
    m_status = Status::CLOSED;
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
    bool emptyFlag(true);
    
    switch (m_status) {
        case Status::CLOSED:
            break;
        case Status::ERRORED:
            break;
        case Status::OPEN:
            emptyFlag = false;
            break;
    }
    
    return emptyFlag;
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
        
        for (int32_t jOverlay = 0; jOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; jOverlay++) {
            m_tabOverlayInfo[cloneToTabIndex][jOverlay]->cloneFromOtherTabOverlayInfo(m_tabOverlayInfo[cloneFromTabIndex][jOverlay].get());
        }

        cloneTabEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_RESET_VIEW) {
        EventResetView* resetViewEvent = dynamic_cast<EventResetView*>(event);
        CaretAssert(resetViewEvent);
        resetViewEvent->setEventProcessed();
        removeTabIndex = resetViewEvent->getTabIndex();
    }
    
    if (removeTabIndex >= 0) {
        for (int32_t jOverlay = 0; jOverlay < BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS; jOverlay++) {
            CaretAssertArrayIndex(m_imageLoaderMultiResolution, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, removeTabIndex);
            CaretAssertArrayIndex(m_imageLoaderMultiResolution, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, jOverlay);
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
        
        if ( ! testReadingSmallImage(m_errorMessage)) {
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
                
                m_displaySettings = docInfo->GetDisplaySettings();
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
    const QRectF overallBoundingBoxRect(CziUtilities::intRectToQRect(overallBoundingBox));

    int32_t width(overallBoundingBox.w);
    int32_t height(overallBoundingBox.h);
    
    if ((width <= 0)
        || (height <= 0)) {
        throw DataFileException("Bounding box has invalid width or height (zero)");
    }
        
    m_allFramesPyramidInfo = CziSceneInfo(this,
                                          s_allFramesIndex,
                                          overallBoundingBoxRect,
                                          "All Scenes");

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

        m_allFramesPyramidInfo.addPyramidLayer(pyramidLayer);
        width /= 2;
        height /= 2;
        pyramidLayerNumber++;
    }
    
    const bool fixMinificationFactorFlag(false);
    m_allFramesPyramidInfo.finishSetup(fixMinificationFactorFlag);
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
    libCZI::PyramidStatistics pyramidStatistics = m_reader->GetPyramidStatistics();
    const int32_t numberOfScenes(pyramidStatistics.scenePyramidStatistics.size());
    
    for (int32_t iScene = 0; iScene < numberOfScenes; iScene++) {
        auto subBlockSceneIter = subBlockStatistics.sceneBoundingBoxes.find(iScene);
        if (subBlockSceneIter == subBlockStatistics.sceneBoundingBoxes.end()) {
            throw DataFileException("Unable to fine scene bounding box for scene index="
                                    + AString::number(iScene));
        }

        const AString sceneName("Scene "
                                + AString::number(iScene + 1));
        const libCZI::BoundingBoxes& boundingBoxes = subBlockSceneIter->second;
        m_cziScenePyramidInfos.push_back(CziSceneInfo(this,
                                                      iScene,
                                                      CziUtilities::intRectToQRect(boundingBoxes.boundingBox),
                                                      sceneName));
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

            CaretAssertVectorIndex(m_cziScenePyramidInfos, sceneIndex);
            m_cziScenePyramidInfos[sceneIndex].addPyramidLayer(pyramidLayer);
            
            if (cziDebugFlag) {
                auto& pl = pyramidLayer;
                std::cout << "Scene Index=" << pl.m_sceneIndex << " CZI Pyramid Layer Number: " << (int)pl.m_layerInfo.pyramidLayerNo << " MinFactor: " << (int)pl.m_layerInfo.minificationFactor
                << " width=" << pl.m_pixelWidth << " height=" << pl.m_pixelHeight << std::endl;
            }
        }
    }
    
    for (auto& sceneInfo : m_cziScenePyramidInfos) {
        const bool fixMinificationFactorFlag(true);
        sceneInfo.finishSetup(fixMinificationFactorFlag);
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
 * Want image loaded to be as close to the maximum dimension.
 * Enlarge image if small and set zoom when logical image is too large
 *
 * @param regionOfInterestToRead
 *    Region that is requested for reading
 * @param fullRegionOfInterest
 *    Total region of data that can be read
 * @param maximumPixelWidthOrHeight
 *    Maximum size for either dimension in pixels
 * @param regionToReadOut
 *    Region that should be read
 * @param zoomOut
 *    Zooming to keep image read within maximumPixelWidthOrHeight
 */
void
CziImageFile::zoomToMatchPixelDimension(const QRectF& regionOfInterestToRead,
                                        const QRectF& fullRegionOfInterest,
                                        const float maximumPixelWidthOrHeight,
                                        QRectF& regionToReadOut,
                                        float& zoomOut) const
{
    regionToReadOut = regionOfInterestToRead;
    zoomOut = 1.0;

    /*
     * Negative means ignore and use full size
     */
    if (maximumPixelWidthOrHeight < 0) {
        return;
    }
    
    const float inputWidth(regionOfInterestToRead.width());
    const float inputHeight(regionOfInterestToRead.height());
    
    /*
     * If ROI is exact pixel dimensions do nothing
     */
    if ((inputWidth == maximumPixelWidthOrHeight)
        && (inputHeight == maximumPixelWidthOrHeight)) {
        return;
    }
    

    /*
     * If width and height are less than maximum dimension,
     * increase size of region to fill maximum dimension
     */
    if ((inputWidth < maximumPixelWidthOrHeight)
        && (inputHeight < maximumPixelWidthOrHeight)) {
        float newWidth(0.0);
        float newHeight(0.0);
        
        /*
         * Enlarge ROI to maximum pixel dimension
         */
        if (inputWidth > inputHeight) {
            const float scale = maximumPixelWidthOrHeight / inputWidth;
            newWidth = maximumPixelWidthOrHeight;
            newHeight = inputHeight * scale;
        }
        else {
            const float scale = maximumPixelWidthOrHeight / inputHeight;
            newWidth  = inputWidth * scale;
            newHeight = maximumPixelWidthOrHeight;
        }
        
        if ((newWidth <= 1.0)
            || (newHeight <= 1.0)) {
            return;
        }
        
        /* Enlarge the ROI */
        const float centerX(regionOfInterestToRead.center().x());
        const float centerY(regionOfInterestToRead.center().y());
        const float halfWidth(newWidth / 2.0);
        const float halfHeight(newHeight / 2.0);
        
        const QRectF newRegionToRead(centerX - halfWidth,  /* left */
                                     centerY - halfHeight, /* top */
                                     newWidth,             /* width */
                                     newHeight);           /* height */
        
        /*
         * Limit to valid region
         */
        if (fullRegionOfInterest.intersects(newRegionToRead)) {
            regionToReadOut = fullRegionOfInterest.intersected(newRegionToRead);
        }
    }
    
    float roiWidth(regionToReadOut.width());
    float roiHeight(regionToReadOut.height());
    
    /*
     * If either width or height is greater than maximum dimension
     * use zooming to keep image loaded to no more than maximum dimensions
     */
    if ((roiWidth > maximumPixelWidthOrHeight)
        || (roiHeight > maximumPixelWidthOrHeight)) {
        if (roiWidth > roiHeight) {
            zoomOut =  maximumPixelWidthOrHeight / roiWidth;
        }
        else {
            zoomOut =  maximumPixelWidthOrHeight / roiHeight;
        }
    }
}

/**
 * Read the specified SCALED region from the CZI file into an image of the given width and height.
 * @param imageDataFormat
 *     Format of image data QImage or CZI Bitmap data
 * @param imageName
 *     Name of image that may be used when debugging
 * @param regionOfInterest
 *    Region of interest to read from file.  Origin is in top left.
 * @param frameRegionOfInterest
 *    Region of interest of the frame or all frames
 * @param outputImageWidthHeightMaximum
 *    Maximum width and height of output image
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readFromCziImageFile(const ImageDataFormat imageDataFormat,
                                   const AString& imageName,
                                   const QRectF& regionOfInterestIn,
                                   const QRectF& frameRegionOfInterest,
                                   const int64_t outputImageWidthHeightMaximum,
                                   AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! regionOfInterestIn.isValid()) {
        errorMessageOut = "Region of interest for reading from file is invalid";
        return NULL;
    }
    
    libCZI::CDimCoordinate coordinate;
    coordinate.Set(libCZI::DimensionIndex::C, 0);
    
    const std::array<float, 3> prefBackRGB = getPreferencesImageBackgroundFloatRGB();
    libCZI::ISingleChannelScalingTileAccessor::Options scstaOptions;
    scstaOptions.Clear();
    scstaOptions.backGroundColor.r = prefBackRGB[0];
    scstaOptions.backGroundColor.g = prefBackRGB[1];
    scstaOptions.backGroundColor.b = prefBackRGB[2];
    
    float zoomToRead(1.0);
    QRectF regionOfInterest(regionOfInterestIn);
    
    /*
     * If ROI width/height is greater than output image width/height,
     * use zoom to reduce the dimensions of the image data that is read
     */
    {
        QRectF newRegion(regionOfInterest);
        float newZoom(1.0);
        zoomToMatchPixelDimension(regionOfInterest,
                                  frameRegionOfInterest,
                                  outputImageWidthHeightMaximum,
                                  newRegion,
                                  newZoom);
        if (cziDebugFlag) {
            std::cout << "Region: " << CziUtilities::qRectToString(regionOfInterest) << std::endl;
            std::cout << "   New: " << CziUtilities::qRectToString(newRegion) << std::endl;
            std::cout << "  Zoom: " << newZoom << std::endl;
        }
        
        regionOfInterest = newRegion;
        zoomToRead = newZoom;
    }
    
    /*
     * Read into 24 bit RGB to avoid conversion from other pixel formats
     */
    if (cziDebugFlag) {
        std::cout << "----------------------" << std::endl;
        std::cout << "READING IMAGE with ROI: " << CziUtilities::qRectToString(regionOfInterest) << std::endl;
    }
    const libCZI::PixelType pixelType(libCZI::PixelType::Bgr24);
    const libCZI::IntRect intRectROI = CziUtilities::qRectToIntRect(regionOfInterest);
    CaretAssert(m_scalingTileAccessor);
    
    std::shared_ptr<libCZI::IBitmapData> bitmapDataRead;
    
    /*
     * "Tinting" applys coloring functions in the CZI library similar
     * to coloring functions in the ZEN software.  The resulting image
     * is nearly identical (colorwise) to the image viewing in ZEN.
     *
     * Text copied from <czi documentation>/html/using_naczirlib.html in CZI documentation
     *
     * Creating a multi-channel composite
     * In order to create a colorful picture from a bunch of channels (usually grayscale),
     * we need to apply a color to it - that's refered to as "tinting". Furthermore, we want
     * to apply a gradation curve. All the required parameters for this are refered to as "display settings".
     * In a CZI-file we can find display settings in the metadata.  The following sample is reading
     * the display settings from the metadata; then we get a (scaled) multi-tile composite for
     * each of the channels (more exactly: only for those channels which are marked 'active' in
     * the display settings). Those bitmaps are then fed into a function which will produce the
     * multi-channel-composite (according to the display settings).
     */
    const bool applyTintingFlag(false);
    if (applyTintingFlag) {
        /*
         * Code copied from <czi documentation>/html/using_naczirlib.html in CZI documentation
         * get the display-setting from the document's metadata
         * Replaced 'auto' with actual data types to know what they are
         */

        /* get the tile-composite for all channels (which are marked 'active' in the display-settings) */
        std::vector<std::shared_ptr<libCZI::IBitmapData>> actvChBms;
        int index = 0;  /* index counting only the active channels */
        std::map<int, int> activeChNoToChIdx;   /* we need to keep track which 'active channels" corresponds to which channel index */

        libCZI::CDisplaySettingsHelper::EnumEnabledChannels(m_displaySettings.get(),
                                                            [&](int chIdx)->bool
                                                            {
            libCZI::CDimCoordinate planeCoord{ { libCZI::DimensionIndex::C, chIdx } };
            actvChBms.emplace_back(m_scalingTileAccessor->Get(intRectROI,
                                                              &planeCoord,
                                                              zoomToRead,
                                                              nullptr));
            activeChNoToChIdx[chIdx] = index++;
            return true;
        });
        
        /*
         * initialize the helper with the display-settings and provide the pixeltypes
         * (for each active channel)
         */
        libCZI::CDisplaySettingsHelper dsplHlp;
        dsplHlp.Initialize(m_displaySettings.get(),
                           [&](int chIdx)->libCZI::PixelType { return actvChBms[activeChNoToChIdx[chIdx]]->GetPixelType(); });

        /*
         * pass the tile-composites we just created (and the display-settings for the those active
         * channels) into the multi-channel-composor-function
         */
        bitmapDataRead = libCZI::Compositors::ComposeMultiChannel_Bgr24(dsplHlp.GetActiveChannelsCount(),
                                                                          std::begin(actvChBms),
                                                                          dsplHlp.GetChannelInfosArray());
    }
    else {
        bitmapDataRead = m_scalingTileAccessor->Get(pixelType,
                                                    intRectROI,
                                                    &coordinate,
                                                    zoomToRead,
                                                    &scstaOptions);
    }

    if ( ! bitmapDataRead) {
        errorMessageOut = ("Failed to read data for region "
                           + CziUtilities::intRectToString(intRectROI));
        return NULL;
    }
    
    const bool removeGrayFlag(false);
    if (removeGrayFlag) {
        uint8_t backRGB[3] = { 0, 0, 0 };
        makeWhiteGrayBackgroundColor(bitmapDataRead,
                                     backRGB);
    }
    
    CziImage* cziImageOut(NULL);
    
    switch (imageDataFormat) {
        case ImageDataFormat::CZI_BITMAP:
            cziImageOut = new CziImage(this,
                                       imageName,
                                       bitmapDataRead,
                                       frameRegionOfInterest,
                                       CziUtilities::intRectToQRect(intRectROI));
            break;
        case ImageDataFormat::Q_IMAGE:
        {
            QImage* qImage = createQImageFromBitmapData(QImagePixelFormat::RGBA,
                                                        bitmapDataRead.get(),
                                                        errorMessageOut);
            if (qImage == NULL) {
                return NULL;
            }
            
            cziImageOut = new CziImage(this,
                                       imageName,
                                       qImage,
                                       frameRegionOfInterest,
                                       CziUtilities::intRectToQRect(intRectROI));
        }
            break;
    }
    return cziImageOut;
}

void
CziImageFile::makeWhiteGrayBackgroundColor(std::shared_ptr<libCZI::IBitmapData>& bitmapData,
                                           const unsigned char backgroundColorRGB[3])
{
    libCZI::BitmapLockInfo lockInfo(bitmapData->Lock());
    switch (bitmapData->GetPixelType()) {
        case libCZI::PixelType::Bgr24:
        {
            uint8_t* rgb(static_cast<uint8_t*>(lockInfo.ptrDataRoi));
            const int64_t numRGB(lockInfo.size / 3);
            for (int64_t i = 0; i < numRGB; i++) {
                const int64_t i3(i * 3);
                uint8_t* pixel(&rgb[i3]);
                auto  iterMinP = std::min_element(pixel, pixel + 3);
                auto  iterMaxP = std::max_element(pixel, pixel + 3);
                const uint8_t sameValueTOL(5);
                if ((*iterMaxP - *iterMinP) < sameValueTOL) {
                    pixel[0] = backgroundColorRGB[0];
                    pixel[1] = backgroundColorRGB[1];
                    pixel[2] = backgroundColorRGB[2];
                }
            }
        }
            break;
        default:
            break;
    }
    bitmapData->Unlock();
}

/**
 * Reload the pyramid layer in the given tab.
 * @param tabIndex
 *    Index of the tab.
 * @param overlayIndex
 * Index of overlasy
 */
void
CziImageFile::reloadPyramidLayerInTabOverlay(const int32_t tabIndex,
                                             const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
    
    CziImageLoaderBase* imageLoader(getImageLoaderForTabOverlay(tabIndex,
                                                                overlayIndex));
    imageLoader->forceImageReloading();
}

/**
 * Get the range of available pyramid layers.  If range is invalid, layer indices will be -1.
 * @param frameIndex
 *    Index of frame
 * @param allFramesFlag
 *    If true, image contains all frames (for CZI this is all scenes)
 * @param lowestPyramidLayerIndexOut
 *    Output with lowest resolution layer index (
 * @param highestPyramidLayerIndexOut
 *    Output with highest resolution layer index
 */
void
CziImageFile::getPyramidLayerRangeForFrame(const int32_t frameIndex,
                                           const bool allFramesFlag,
                                           int32_t& lowestPyramidLayerIndexOut,
                                           int32_t& highestPyramidLayerIndexOut) const
{
    lowestPyramidLayerIndexOut  = -1;
    highestPyramidLayerIndexOut = -1;
    
    const CziSceneInfo* cziInfo(NULL);
    if (allFramesFlag) {
        cziInfo = &m_allFramesPyramidInfo;
    }
    else {
        CaretAssertVectorIndex(m_cziScenePyramidInfos, frameIndex);
        cziInfo = &m_cziScenePyramidInfos[frameIndex];
    }
    
    if (cziInfo != NULL) {
        const int32_t numPyramidLayers(cziInfo->m_pyramidLayers.size());
        if (numPyramidLayers > 0) {
            lowestPyramidLayerIndexOut  = 0;
            highestPyramidLayerIndexOut = numPyramidLayers - 1;
        }
    }
}

/**
 * Read a pyramid layer from CZI file
 * @param imageName
 *    Name of image that may be used when debugging
 * @param frameIndex
 *  Index of frame (scene)
 * @param pyramidLayer
 *    Index of pyramid layer to read
 *    Rectangular region to read NOT REALLY USED ???
 * @param rectangleForReadingRect
 *    Rectangular region to read
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readFramePyramidLayerFromCziImageFile(const AString& imageName,
                                                    const int32_t frameIndex,
                                                    const int32_t pyramidLayer,
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
    
    const std::array<float, 3> prefBackRGB = getPreferencesImageBackgroundFloatRGB();
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
    QImage* qImage = createQImageFromBitmapData(QImagePixelFormat::RGBA,
                                                bitmapData.get(),
                                                errorMessageOut);
    if (qImage == NULL) {
        return NULL;
    }
    
    CziImage* cziImageOut = new CziImage(this,
                                         imageName,
                                         qImage,
                                         m_fullResolutionLogicalRect,
                                         CziUtilities::intRectToQRect(rectToReadROI));
    return cziImageOut;
}

/**
 * Create a QImage from the CZI bitmap data
 * @param imagePixelFormat
 *    Pixel format of QImage.
 * @param bitmapData
 *    The CZI bitmap data
 * @param errorMessageOut
 *    Contains error information
 * @return A QImage containing the bitmap data or NULL if not valid
 */
QImage*
CziImageFile::createQImageFromBitmapData(const QImagePixelFormat imagePixelFormat,
                                         libCZI::IBitmapData* bitmapData,
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
    
    if (bitmapData->GetPixelType() != libCZI::PixelType::Bgr24) {
        errorMessageOut = "Only pixel type Bgr24 is supported";
        return NULL;
    }

    /*
     * call to "Lock()" must have corresponding "Unlock()"
     */
    libCZI::BitmapLockInfo bitMapInfo = bitmapData->Lock();
    if (cziDebugFlag) std::cout << "   Stride: " << bitMapInfo.stride << std::endl;
    if (cziDebugFlag) std::cout << "   Size: " << bitMapInfo.size << std::endl;
    
    unsigned char* cziPtr8 = (unsigned char*)bitMapInfo.ptrDataRoi;
    
    /*
     * Documentation for QImage states that setPixel may be very costly
     * and recommends using the scanLine() method to access pixel data.
     */
    QImage::Format qFormat = QImage::Format_ARGB32;
    switch (imagePixelFormat) {
        case QImagePixelFormat::RGB:
            qFormat = QImage::Format_RGB32;
            break;
        case QImagePixelFormat::RGBA:
            qFormat = QImage::Format_ARGB32;
            break;
    }
    QImage* imageOut = new QImage(width,
                                  height,
                                  qFormat);
    
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
            switch (imagePixelFormat) {
                case QImagePixelFormat::RGB:
                    *pixel = rgba.rgb();
                    break;
                case QImagePixelFormat::RGBA:
                    *pixel = rgba.rgba();
                    break;
            }
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
 * @param frameIndex
 *    Indics of the frames
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
CziImageFile::getPixelLogicalIdentificationTextForFrames(const int32_t tabIndex,
                                                  const std::vector<int32_t>& frameIndices,
                                                 const PixelLogicalIndex& pixelLogicalIndex,
                                                 std::vector<AString>& columnOneTextOut,
                                                 std::vector<AString>& columnTwoTextOut,
                                                 std::vector<AString>& toolTipTextOut) const
{
    columnOneTextOut.clear();
    columnTwoTextOut.clear();
    toolTipTextOut.clear();
    
    std::vector<int32_t> validFrameIndices;
    for (int32_t frameIndex : frameIndices) {
        if (isPixelIndexInFrameValid(frameIndex,
                                     pixelLogicalIndex)) {
            validFrameIndices.push_back(frameIndex);
        }
    }
    if (validFrameIndices.empty()) {
        return;
    }
        
    
    std::vector<AString> leftRgbaText;
    std::vector<AString> rightRgbaText;
    for (int32_t frameIndex : validFrameIndices) {
        uint8_t rgba[4];
        const bool rgbaValidFlag = getPixelRGBA(tabIndex,
                                                frameIndex,
                                                pixelLogicalIndex,
                                                rgba);
        if (rgbaValidFlag) {
            leftRgbaText.push_back("Scene "
                                   + AString::number(frameIndex + 1));
            rightRgbaText.push_back("RGBA ("
                                    + (rgbaValidFlag
                                       ? AString::fromNumbers(rgba, 4, ",")
                                       : "Invalid")
                                    + ")");
        }
    }
    CaretAssert(leftRgbaText.size() == rightRgbaText.size());
    const int32_t numRgbaText(static_cast<int32_t>(leftRgbaText.size()));
    
    const PixelIndex pixelIndex(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    const int64_t fullResPixelI(pixelIndex.getI());
    const int64_t fullResPixelJ(pixelIndex.getJ());
    const AString pixelText("Pixel IJ ("
                            + AString::number(fullResPixelI)
                            + ","
                            + AString::number(fullResPixelJ)
                            + ")");
    
    const AString logicalText("Logical IJ ("
                              + AString::number(pixelLogicalIndex.getI(), 'f', 3)
                              + ","
                              + AString::number(pixelLogicalIndex.getJ(), 'f', 3)
                              + ")");
    
    const PixelCoordinate pixelsSize(getPixelSizeInMillimeters());
    const float pixelX(fullResPixelI * pixelsSize.getX());
    const float pixelY(fullResPixelJ * pixelsSize.getY());
    const AString mmText("Pixel XY ("
                         + AString::number(pixelX, 'f', 3)
                         + "mm,"
                         + AString::number(pixelY, 'f', 3)
                         + "mm)");
    
    
    
    columnOneTextOut.push_back("Filename");
    columnTwoTextOut.push_back(getFileNameNoPath());
    
    columnOneTextOut.push_back(pixelText);
    columnTwoTextOut.push_back(logicalText);
    
    columnOneTextOut.push_back(mmText);
    columnTwoTextOut.push_back("");
    
    Vector3D xyz;
    if (pixelIndexToStereotaxicXYZ(pixelLogicalIndex, false, xyz)) {
        columnOneTextOut.push_back("Stereotaxic XYZ");
        columnTwoTextOut.push_back(AString::fromNumbers(xyz, 3, ", "));
    }
    
    if (pixelIndexToStereotaxicXYZ(pixelLogicalIndex, true, xyz)) {
        columnOneTextOut.push_back("Stereotaxic XYZ with NIFTI warping");
        columnTwoTextOut.push_back(AString::fromNumbers(xyz, 3, ", "));
    }

    for (int32_t i = 0; i < numRgbaText; i++) {
        CaretAssertVectorIndex(leftRgbaText, i);
        CaretAssertVectorIndex(rightRgbaText, i);
        toolTipTextOut.push_back(leftRgbaText[i]
                                 + ": "
                                 + rightRgbaText[i]);
        columnOneTextOut.push_back(leftRgbaText[i]);
        columnTwoTextOut.push_back(rightRgbaText[i]);
    }
    toolTipTextOut.push_back(pixelText);
    toolTipTextOut.push_back(logicalText);
    toolTipTextOut.push_back(mmText);
    
    CaretAssert(columnOneTextOut.size() == columnTwoTextOut.size());
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
                                         Vector3D& xyzOut) const
{
    Vector3D debugPixelIndex;
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
                                         Vector3D& xyzOut,
                                         Vector3D& debugPixelIndexOut) const
{
    const PixelIndex pixelIndexOriginAtTop(pixelLogicalIndexToPixelIndex(pixelLogicalIndex));
    debugPixelIndexOut[0] = -1;
    debugPixelIndexOut[1] = -1;
    debugPixelIndexOut[2] = -1;

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
        Vector3D pt {
            static_cast<float>(MathFunctions::round(pixelI * m_pixelToStereotaxicTransform.m_pixelScaleI)),
            static_cast<float>(MathFunctions::round(pixelJ * m_pixelToStereotaxicTransform.m_pixelScaleJ)),
            0.0f
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
        m_pixelToStereotaxicTransform.m_sformMatrix->multiplyPoint3(pt);
        
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
CziImageFile::stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                         const bool includeNonlinearFlag,
                                         PixelLogicalIndex& pixelLogicalIndexOut) const
{
    Vector3D debugPixelIndex;
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
CziImageFile::stereotaxicXyzToPixelIndex(const Vector3D& xyz,
                                         const bool includeNonlinearFlag,
                                         PixelLogicalIndex& pixelLogicalIndex,
                                         const Vector3D& debugPixelIndex) const
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
        Vector3D pt { xyz[0], xyz[1], xyz[2] };
        m_stereotaxicToPixelTransform.m_sformMatrix->multiplyPoint3(pt);
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
                
                /*
                 * Multiply new XYZ to get pixel index
                 */
                m_stereotaxicToPixelTransform.m_sformMatrix->multiplyPoint3(pt);
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
        Vector3D ptRounded {
            static_cast<float>(MathFunctions::round(pt[0])),
            static_cast<float>(MathFunctions::round(pt[1])),
            static_cast<float>(MathFunctions::round(pt[2]))
        };
        
        CaretLogFine("\n"
                     + AString(includeNonlinearFlag ? "With NIFTI warping ": "")
                     + "XYZ to Pixel Index Test: "
                     + AString::fromNumbers(ptRounded)
                     + "\n"
                     + AString(includeNonlinearFlag ? "With NIFTI warping ": "")
                     + "    Correct pixel index: "
                     + AString::fromNumbers(debugPixelIndex));
        
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
            Vector3D xyz;
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
                                                    + AString::fromNumbers(xyz, 3, ",")
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
CziImageFile::findPixelNearestStereotaxicXYZ(const Vector3D& xyz,
                                             const bool includeNonLinearFlag,
                                             float& signedDistanceToPixelMillimetersOut,
                                             PixelLogicalIndex& pixelLogicalIndexOut) const
{
    const Plane* plane(getImagePlane());
    if (plane == NULL) {
        return false;
    }
    
    Vector3D xyzOnPlane;
    plane->projectPointToPlane(xyz, xyzOnPlane);
    
    if (stereotaxicXyzToPixelIndex(xyzOnPlane,
                                   includeNonLinearFlag,
                                   pixelLogicalIndexOut)) {
        if (isPixelIndexValid(pixelLogicalIndexOut)) {
            signedDistanceToPixelMillimetersOut = plane->absoluteDistanceToPlane(xyz);
            return true;
        }
    }
    return false;
}

/**
 * @return The stereotaxic plane for this CZI image calculated from the coordinates at the image's corners
 * Null if not valid.
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
    Vector3D bottomLeftXYZ, topLeftXYZ, topRightXYZ;
    const bool nonLinearFlag(true);
    if (pixelIndexToStereotaxicXYZ(bottomLeftPixel, nonLinearFlag, bottomLeftXYZ)
        && pixelIndexToStereotaxicXYZ(topLeftPixel, nonLinearFlag, topLeftXYZ)
        && pixelIndexToStereotaxicXYZ(topRightPixel, nonLinearFlag, topRightXYZ)) {
        /*
         * Create the plane from XYZ coordinates
         */
        m_imagePlane.reset(new Plane(bottomLeftXYZ,
                                     topLeftXYZ,
                                     topRightXYZ));
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
            
            /*
             * Map full-res pixel index to a NIFTI transform voxel index
             */
            transform.m_pixelScaleI = (static_cast<float>(dims[0]) / getWidth());
            transform.m_pixelScaleJ = (static_cast<float>(dims[1])  / getHeight());
            
            std::vector<std::vector<float>> sform(transform.m_niftiFile->getSform());
            transform.m_sformMatrix.reset(new Matrix4x4(sform));
            
            
            AString pixelStepText;
            if (CaretLogger::getLogger()->isFine()) {
                Vector3D p1 { 0.0, 0.0, 0.0 };
                transform.m_sformMatrix->multiplyPoint3(p1);
                Vector3D p2 { 1.0, 1.0, 1.0 };
                transform.m_sformMatrix->multiplyPoint3(p2);
                Vector3D stepXYZ {
                    p2[0] - p1[0],
                    p2[1] - p1[1],
                    p2[2] - p1[2],
                };
                pixelStepText = ("Pixel Step (0, 0, 0) to (1, 1, 1): "
                                 + AString::fromNumbers(stepXYZ));
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
    
    dataFileInformation.addNameAndValue("Width (pixels)", getWidth());
    dataFileInformation.addNameAndValue("Height (pixels)", getHeight());
    dataFileInformation.addNameAndValue("Logical X", m_fullResolutionLogicalRect.x());
    dataFileInformation.addNameAndValue("Logical Y", m_fullResolutionLogicalRect.y());
    dataFileInformation.addNameAndValue("Logical Width", m_fullResolutionLogicalRect.width());
    dataFileInformation.addNameAndValue("Logical Height", m_fullResolutionLogicalRect.height());

    dataFileInformation.addNameAndValue("Pixel Size X (mm)", m_pixelSizeMmX, 6);
    dataFileInformation.addNameAndValue("Pixel Size Y (mm)", m_pixelSizeMmY, 6);
    dataFileInformation.addNameAndValue("Pixel Size Z (mm)", m_pixelSizeMmZ, 6);
    dataFileInformation.addNameAndValue("Full Logical Rectangle",
                                        CziUtilities::qRectToString(m_fullResolutionLogicalRect));
    dataFileInformation.addNameAndValue("Plane XYZ Rect",
                                        CziUtilities::qRectToString(getPlaneXyzRect()));
    
    addPlaneCoordsToDataFileContentInformation(dataFileInformation);
    

    m_allFramesPyramidInfo.addToDataFileContentInformation(dataFileInformation, "All Scenes");
    
    const int32_t numScenes = getNumberOfScenes();
    dataFileInformation.addNameAndValue("Number of Scenes", numScenes);
    for (int32_t iScene = 0; iScene < numScenes; iScene++) {
        const CziSceneInfo& cziSceneInfo(m_cziScenePyramidInfos[iScene]);
        cziSceneInfo.addToDataFileContentInformation(dataFileInformation,
                                                     ("Scene "
                                                      + AString::number(cziSceneInfo.m_sceneIndex)));
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
 * @return Return a rectangle that defines the bounds of the media data
 */
QRectF
CziImageFile::getLogicalBoundsRect() const
{
    return m_fullResolutionLogicalRect;
}

/**
 * Get the image loader for the given tab and overlay
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of the overlay
 * @return
 *    Image loader for the given tab and index
 */
CziImageLoaderBase*
CziImageFile::getImageLoaderForTabOverlay(const int32_t tabIndex,
                                          const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
    return m_tabOverlayInfo[tabIndex][overlayIndex]->getMultiResolutionImageLoader();
}

/**
 * Get the image loader for the given tab and overlay
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of the overlay
 * @return
 *    Image loader for the given tab and index
 */
const CziImageLoaderBase*
CziImageFile::getImageLoaderForTabOverlay(const int32_t tabIndex,
                                          const int32_t overlayIndex) const
{
    CziImageFile* nonConstThis(const_cast<CziImageFile*>(this));
    CaretAssert(nonConstThis);
    return nonConstThis->getImageLoaderForTabOverlay(tabIndex,
                                                     overlayIndex);
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
    CziImageLoaderBase* imageLoader = getImageLoaderForTabOverlay(tabIndex,
                                                                  overlayIndex);
    CaretAssert(imageLoader);
    CziImage* cziImageOut = imageLoader->getImage();
    return cziImageOut;
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
    CziImageFile* nonConstThis(const_cast<CziImageFile*>(this));
    return nonConstThis->getImageForTabOverlay(tabIndex,
                                               overlayIndex);
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
 * @param coordinateMode
 *    Coordinate mode (pixel or plane)
 * @param manualPyramidLayerIndex
 *    Index of pyramid layer for manual mode
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 */
void
CziImageFile::updateImageForDrawingInTab(const int32_t tabIndex,
                                         const int32_t overlayIndex,
                                         const int32_t frameIndex,
                                         const bool allFramesFlag,
                                         const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode,
                                         const MediaDisplayCoordinateModeEnum::Enum coordinateMode,
                                         const int32_t manualPyramidLayerIndex,
                                         const GraphicsObjectToWindowTransform* transform)
{
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
    m_tabOverlayInfo[tabIndex][overlayIndex]->m_imageResolutionChangeMode = resolutionChangeMode;
    
    CziImage* cziImage(getImageForTabOverlay(tabIndex,
                                             overlayIndex));
    
    switch (resolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            CaretAssert(0);
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            break;
    }
    
    CziImageLoaderBase* imageLoaderBase(getImageLoaderForTabOverlay(tabIndex,
                                                                    overlayIndex));
    CaretAssert(imageLoaderBase);
    
    imageLoaderBase->updateImage(cziImage,
                                 frameIndex,
                                 allFramesFlag,
                                 resolutionChangeMode,
                                 coordinateMode,
                                 manualPyramidLayerIndex,
                                 transform);
}

/**
 * @return The graphics primitive for drawing the image as a texture in media drawing model.  Can be NULL.
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
    if (cziImage == NULL) {
        return NULL;
    }
    
    GraphicsPrimitiveV3fT2f* primitive(cziImage->getGraphicsPrimitiveForMediaDrawing());
    return primitive;
}

/*
 * @return Primitive for drawing media with coordinates
 * @param tabIndex
 *    Index of tab where image is drawn
 * @param overlayIndex
 *    Index of overlay
 */
GraphicsPrimitiveV3fT2f*
CziImageFile::getGraphicsPrimitiveForPlaneXyzDrawing(const int32_t tabIndex,
                                                     const int32_t overlayIndex) const
{
    const CziImage* cziImage(getImageForTabOverlay(tabIndex,
                                                   overlayIndex));
    if (cziImage == NULL) {
        return NULL;
    }
    
    GraphicsPrimitiveV3fT2f* primitive(cziImage->getGraphicsPrimitiveForPlaneXyzDrawing());
    return primitive;
}

/**
 * @return True if the given pixel index is valid for the CZI image file (may be outside of currently loaded sub-image)
 * @param frameIndex
 *    Index of frame
 * @param pixelIndexOriginAtTopLeft
 *    Image of pixel with origin (0, 0) at the top left
 */
bool
CziImageFile::isPixelIndexInFrameValid(const int32_t frameIndex,
                                       const PixelIndex& pixelIndexOriginAtTopLeft) const
{
    return isPixelIndexInFrameValid(frameIndex,
                                    pixelIndexToPixelLogicalIndex(pixelIndexOriginAtTopLeft));
}

/**
 * @return True if the given pixel index is valid
 * @param frameIndex
 *    Index of frame
 * @param pixelLogicalIndex
 *    Pixel logical index
 */
bool
CziImageFile::isPixelIndexInFrameValid(const int32_t frameIndex,
                                       const PixelLogicalIndex& pixelLogicalIndex) const
{
    CaretAssertVectorIndex(m_cziScenePyramidInfos, frameIndex);
    const QRectF frameLogicalRect(m_cziScenePyramidInfos[frameIndex].m_logicalRectangle);
    const float i(pixelLogicalIndex.getI());
    const float j(pixelLogicalIndex.getJ());
    
    if ((i >= frameLogicalRect.left())
        && (i < frameLogicalRect.right())
        && (j >= frameLogicalRect.top())
        && (j < frameLogicalRect.bottom())) {
        return true;
    }
    
    return false;
}

/**
 * @return True if the given pixel index is valid for the CZI image file (may be outside of currently loaded sub-image)
 * @param pixelIndexOriginAtTopLeft
 *    Image of pixel with origin (0, 0) at the top left
 */
bool
CziImageFile::isPixelIndexValid(const PixelIndex& pixelIndexOriginAtTopLeft) const
{
    return isPixelIndexValid(pixelIndexToPixelLogicalIndex(pixelIndexOriginAtTopLeft));
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
 *     True if valid, else false.  If pixel is background color, false is returned.
 */
bool
CziImageFile::getPixelRGBA(const int32_t tabIndex,
                           const int32_t overlayIndex,
                           const PixelLogicalIndex& pixelLogicalIndex,
                           uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = 0;
    pixelRGBAOut[1] = 0;
    pixelRGBAOut[2] = 0;
    pixelRGBAOut[3] = 0;
    
    const libCZI::PixelType pixelType(libCZI::PixelType::Bgr24);
    libCZI::IntRect pixelRect;
    pixelRect.x = pixelLogicalIndex.getI();
    pixelRect.y = pixelLogicalIndex.getJ();
    pixelRect.w = 1;
    pixelRect.h = 1;
    
    libCZI::CDimCoordinate coordinate;
    coordinate.Set(libCZI::DimensionIndex::C, 0);

    const bool readFromFileFlag(true);
    if (readFromFileFlag) {
        auto singleChannelTileAccessor = m_reader->CreateSingleChannelTileAccessor();
        if (singleChannelTileAccessor) {
            const std::array<float, 3> prefBackFloatRGB = getPreferencesImageBackgroundFloatRGB();

            libCZI::ISingleChannelTileAccessor::Options options;
            options.Clear();
            options.backGroundColor.r = prefBackFloatRGB[0];
            options.backGroundColor.g = prefBackFloatRGB[1];
            options.backGroundColor.b = prefBackFloatRGB[2];

            std::shared_ptr<libCZI::IBitmapData> bitmapData = singleChannelTileAccessor->Get(pixelType,
                                                                                             pixelRect,
                                                                                             &coordinate,
                                                                                             &options);
                                                                                             
            if (bitmapData) {
                if ((bitmapData->GetWidth() == 1)
                    && (bitmapData->GetHeight() == 1)
                    && (bitmapData->GetPixelType() == pixelType)) {
                    libCZI::BitmapLockInfo bitMapInfo = bitmapData->Lock();
                    unsigned char* cziPtr8 = (unsigned char*)bitMapInfo.ptrDataRoi;
                    pixelRGBAOut[0] = cziPtr8[2];
                    pixelRGBAOut[1] = cziPtr8[1];
                    pixelRGBAOut[2] = cziPtr8[0];
                    pixelRGBAOut[3] = 255;
                    bitmapData->Unlock();
                    
                    const std::array<uint8_t, 3> prefBackByteRGB = getPreferencesImageBackgroundByteRGB();
                    if ((prefBackByteRGB[0] == pixelRGBAOut[0])
                        && (prefBackByteRGB[1] == pixelRGBAOut[1])
                        && (prefBackByteRGB[2] == pixelRGBAOut[2])) {
                        /*
                         * If pixel color is background, then return invalid status
                         */
                        return false;
                    }
                    
                    return true;
                }
                else {
                    CaretLogSevere("Single Channel Data read is incorrect size width="
                                   + AString::number(bitmapData->GetWidth())
                                   + ", height="
                                   + AString::number(bitmapData->GetHeight()));
                }
            }
            else {
                CaretLogSevere("Single Channel Failed to read RGBA pixel "
                               + pixelLogicalIndex.toString()
                               + " from file "
                               + getFileNameNoPath());
            }
        }
    }
    
    /*
     * If pixel identification above failed, try to access from image data
     */
    const CziImage* cziImage = getImageForTabOverlay(tabIndex,
                                                     overlayIndex);
    CaretAssert(cziImage);
    if (cziImage->getImageDataPixelRGBA(pixelLogicalIndex,
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
    CaretAssert(m_maximumImageDimension >= 2048);
    return m_maximumImageDimension;
}

/**
 * @return The RGB background color for images from preferences BYTE values
 */
std::array<uint8_t, 3>
CziImageFile::getPreferencesImageBackgroundByteRGB() const
{
    std::array<uint8_t, 3> rgb;
    
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    if (prefs != NULL) {
        prefs->getBackgroundAndForegroundColors()->getColorBackgroundMediaView(rgb.data());
    }
    
    return rgb;
}

/**
 * @return The RGB background color for images from preferences FLOAT values
 */
std::array<float, 3>
CziImageFile::getPreferencesImageBackgroundFloatRGB() const
{
    std::array<uint8_t, 3> rgbByte = getPreferencesImageBackgroundByteRGB();
    std::array<float, 3> rgb = BackgroundAndForegroundColors::toFloatRGB(rgbByte.data());

    return rgb;
}

/**
 * Read a small image to verify that data can be read from the file.
 * @param errorMessageOut
 *    Contains error information if there is an error
 * @return True if data read successfully, else false
 */
bool
CziImageFile::testReadingSmallImage(AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const int32_t imageDimensionWidthHeight(512);
    std::unique_ptr<CziImage> cziImage(readFromCziImageFile(CziImageFile::ImageDataFormat::CZI_BITMAP,
                                                            "Test Image File Reading",
                                                            m_fullResolutionLogicalRect,
                                                            m_fullResolutionLogicalRect,
                                                            imageDimensionWidthHeight,
                                                            errorMessageOut));
    if (cziImage) {
        return true;
    }
        
    return false;
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

/**
 * Export a full resolution image to an image file with the maximum width/height
 * @param imageFileName
 *    Name for file
 * @param maximumWidthHeight
 *    Width and height will be no greater than this value (aspect is preserved)
 *     Negative is no limit on size
 * @param addPlaneMatrixTransformsFlag
 *    If transformation matrices are available for pixel to plane and plane to millimeters, include them.
 * @param includeAlphaFlag
 *    Include the alpha component in the pixels
 * @param errorMessageOut
 *    Contains info if writing image fails
 * @return True if successful, else false.
 */
bool
CziImageFile::exportToImageFile(const QString& imageFileName,
                                const int32_t maximumWidthHeight,
                                const bool addPlaneMatrixTransformsFlag,
                                const bool includeAlphaFlag,
                                AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if (maximumWidthHeight == 0) {
        errorMessageOut.appendWithNewLine("Image maximum size is zero.  Must be positive value or use "
                                          "any negative value for no size limit.");
    }
    if (imageFileName.isEmpty()) {
        errorMessageOut.appendWithNewLine("Image file name is invalid.");
    }
    if ( ! errorMessageOut.isEmpty()) {
        return false;
    }
    
    
    std::unique_ptr<CziImage> cziImage(readFromCziImageFile(CziImageFile::ImageDataFormat::Q_IMAGE,
                                                            "cziImageName",
                                                            m_fullResolutionLogicalRect,
                                                            m_fullResolutionLogicalRect,
                                                            maximumWidthHeight,
                                                            errorMessageOut));
    if (cziImage == NULL) {
        errorMessageOut = ("readFromCziImageFile() failed to read image data.");
        return false;
    }
    
    QImage* qImage(const_cast<QImage*>(cziImage->getQImagePointer()));
    if (qImage == NULL) {
        errorMessageOut = ("Failed to get QImage from CziImage");
        return false;
    }

    /*
     * Set format to RGB or RGBA
     */
    const QImage::Format imageFormat(includeAlphaFlag
                                     ? QImage::Format_ARGB32
                                     : QImage::Format_RGB32);
    if (qImage->format() != imageFormat) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
        qImage->convertTo(imageFormat);
#else
        *qImage = qImage->convertToFormat(imageFormat);
#endif
    }
    
    /*
     * Add matrices if available for Plane Coordinate Viewing Mode
     */
    if (addPlaneMatrixTransformsFlag) {
        if (isScaledToPlaneMatrixValid()
            && isPlaneToMillimetersMatrixValid()) {
            qImage->setText(getMetaDataNameScaledToPlaneMatrix(),
                            getScaledToPlaneMatrix().getMatrixInRowMajorOrderString());
            qImage->setText(getMetaDataNamePlaneToMillimetersMatrix(),
                            getPlaneToMillimetersMatrix().getMatrixInRowMajorOrderString());
        }
    }
    
    FileInformation fileInfo(imageFileName);
    AString format = fileInfo.getFileExtension().toUpper();
    if (format == "JPG") {
        format = "JPEG";
    }
    
    QImageWriter writer(imageFileName, format.toLatin1());
    if (writer.supportsOption(QImageIOHandler::Quality)) {
        if (format.compare("png", Qt::CaseInsensitive) == 0) {
            const int quality = 1;
            writer.setQuality(quality);
        }
        else {
            const int quality = 100;
            writer.setQuality(quality);
        }
    }
    
    if (writer.supportsOption(QImageIOHandler::CompressionRatio)) {
        writer.setCompression(1);
    }
    
    if ( ! writer.write(*qImage)) {
        errorMessageOut = writer.errorString();
    }
    
    return errorMessageOut.isEmpty();
}

/**
 * Set the matrices for display drawing.
 * @param scaledToPlaneMatrix
 *    The scaled to plane matrix.
 * @param scaledToPlaneMatrixValidFlag
 *    Validity of the scaled to plane matrix.
 * @param planeToMillimetersMatrix
 *    Matrix for converting from plane coords to millimeter coords
 * @param planeToMillimetersMatrixValidFlag
 *    Validity of the plane to millimeters matrix
 */
void
CziImageFile::setScaledToPlaneMatrix(const Matrix4x4& scaledToPlaneMatrix,
                                     const bool scaledToPlaneMatrixValidFlag,
                                     const Matrix4x4& planeToMillimetersMatrix,
                                     const bool planeToMillimetersMatrixValidFlag)
{
    MediaFile::setScaledToPlaneMatrix(scaledToPlaneMatrix,
                                      scaledToPlaneMatrixValidFlag,
                                      planeToMillimetersMatrix,
                                      planeToMillimetersMatrixValidFlag);
    
    /*
     * For all scenes, set bounds in plane coordinates
     */
    for (CziSceneInfo& cziSceneInfo: m_cziScenePyramidInfos) {
        QRectF logicalRect(cziSceneInfo.m_logicalRectangle);
        const float l(logicalRect.left());
        const float r(logicalRect.right());
        const float b(logicalRect.bottom());
        const float t(logicalRect.top());
        
        Vector3D leftBottom, rightBottom, leftTop, rightTop;
        if (logicalPixelIndexToPlaneXYZ(l, t, leftTop)
            && logicalPixelIndexToPlaneXYZ(r, t, rightTop)
            && logicalPixelIndexToPlaneXYZ(l, b, leftBottom)
            && logicalPixelIndexToPlaneXYZ(r, b, rightBottom)) {
            BoundingBox bb;
            bb.resetForUpdate();
            bb.update(leftTop);
            bb.update(rightTop);
            bb.update(leftBottom);
            bb.update(rightBottom);
            
            QRectF boundsRect(bb.getMinX(),
                              bb.getMinY(),
                              bb.getDifferenceX(),
                              bb.getDifferenceY());
            if (boundsRect.isValid()) {
                cziSceneInfo.m_planeRectangle = boundsRect;
            }
        }
    }
    
    {
        QRectF logicalRect(m_allFramesPyramidInfo.m_logicalRectangle);
        const float l(logicalRect.left());
        const float r(logicalRect.right());
        const float b(logicalRect.bottom());
        const float t(logicalRect.top());
        
        Vector3D leftBottom, rightBottom, leftTop, rightTop;
        if (logicalPixelIndexToPlaneXYZ(l, t, leftTop)
            && logicalPixelIndexToPlaneXYZ(r, t, rightTop)
            && logicalPixelIndexToPlaneXYZ(l, b, leftBottom)
            && logicalPixelIndexToPlaneXYZ(r, b, rightBottom)) {
            BoundingBox bb;
            bb.resetForUpdate();
            bb.update(leftTop);
            bb.update(rightTop);
            bb.update(leftBottom);
            bb.update(rightBottom);
            
            QRectF boundsRect(bb.getMinX(),
                              bb.getMinY(),
                              bb.getDifferenceX(),
                              bb.getDifferenceY());
            if (boundsRect.isValid()) {
                m_allFramesPyramidInfo.m_planeRectangle = boundsRect;
            }
        }
    }
}

/**
 * @return Range of pyramid layer indices
 */
std::array<int32_t, 2>
CziImageFile::CziSceneInfo::getPyramidLayerIndexRange() const
{
    std::array<int32_t, 2> rangeMinMax { m_minimumPyramidLayerIndex, m_maximumPyramidLayerIndex };
    return rangeMinMax;
}


/**
 * Set the pyramid level range
 */
void
CziImageFile::CziSceneInfo::setPyramidLayerIndexRange()
{
    const float fullWidth(m_logicalRectangle.width());
    const float fullHeight(m_logicalRectangle.height());
    
    const int32_t numLayers(getNumberOfPyramidLayers());
    if (numLayers <= 0) {
        return;
    }
    
    for (int32_t i = 0; i < numLayers; i++) {
        CaretAssertVectorIndex(m_pyramidLayers, i);
        if ((m_pyramidLayers[i].m_logicalWidthForImageReading == fullWidth)
            && (m_pyramidLayers[i].m_logicalHeightForImageReading == fullHeight)) {
            m_minimumPyramidLayerIndex = i;
        }
    }
    
    m_maximumPyramidLayerIndex = (numLayers - 1);
}

void
CziImageFile::CziSceneInfo::finishSetup(const bool fixMinFactorFlag)
{
    std::sort(m_pyramidLayers.begin(),
              m_pyramidLayers.end(),
              [=](PyramidLayer a, PyramidLayer b) { return (a.m_layerInfo.pyramidLayerNo > b.m_layerInfo.pyramidLayerNo); } );
    
    
    if (fixMinFactorFlag) {
        const int32_t numPyramidLayers(getNumberOfPyramidLayers());
        if (numPyramidLayers >= 1) {
            const int32_t lastIndex(numPyramidLayers - 1);
            CaretAssertVectorIndex(m_pyramidLayers, lastIndex);
            
            /*
             * For the highest resolution pyramid layer, the CZI library has the
             * minification factor set to zero.  If one tries to load this pyramid
             * layer with minification factor of zero, the CZI library goes into
             * an infinite loop in CSingleChannelPyramidLevelTileAccessor::CalcPyramidLayerNo
             * at " if (f >= minFactorInt)" because 'f' will be zero and 'minFactorInt' is 2.
             *
             * So, replacing the minification factor with 2 seems to prevent the infinite loop.
             */
            if (m_pyramidLayers[lastIndex].m_layerInfo.minificationFactor == 0) {
                m_pyramidLayers[lastIndex].m_layerInfo.minificationFactor = 2;
            }
        }
    }
    
    setLayersZoomFactors();
    
    setPyramidLayerIndexRange();
}

/**
 * Set the zoom factors for layers
 */
void
CziImageFile::CziSceneInfo::setLayersZoomFactors()
{
    const int64_t widthImageLoad(m_logicalRectangle.width());
    const int64_t heightImageLoad(m_logicalRectangle.height());
    
    const int32_t preferredDim(m_parentCziImageFile->getPreferencesImageDimension());
    int32_t zoomFactor(1);
    for (auto& layer : m_pyramidLayers) {
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
 * Add to the data file information.
 * @param dataFileInformation
 *    Item to which information is added.
 */
void
CziImageFile::CziSceneInfo::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation,
                                                            const AString& sceneInfoName) const
{
    dataFileInformation.addNameAndValue(sceneInfoName, "");
    dataFileInformation.addNameAndValue("----Logical Rectangle",
                                        CziUtilities::qRectToString(m_logicalRectangle));
    dataFileInformation.addNameAndValue("----Pyramid Range",
                                        (QString::number(m_minimumPyramidLayerIndex)
                                         + " to "
                                         + QString::number(m_maximumPyramidLayerIndex)
                                         + " (inclusive)"));
    const int32_t numPyramidLayers(getNumberOfPyramidLayers());
    for (int32_t iPyramid = 0; iPyramid < numPyramidLayers; iPyramid++) {
        CaretAssertVectorIndex(m_pyramidLayers, iPyramid);
        const PyramidLayer& pl(m_pyramidLayers[iPyramid]);
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

/**
 * Constructor
 * @param cziImageFile
 *    CZI image file using this tab overlay info
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of overlay
 */
CziImageFile::TabOverlayInfo::TabOverlayInfo(CziImageFile* cziImageFile,
                                             const int32_t tabIndex,
                                             const int32_t overlayIndex)
:
m_cziImageFile(cziImageFile),
m_tabIndex(tabIndex),
m_overlayIndex(overlayIndex)
{
    
}

/**
 * @return The selected image loader
 */
CziImageFile::TabOverlayInfo::~TabOverlayInfo()
{
}

/**
 * @return The selected image loader
 * @param resolutionChangeMode)
 *    The resolution mode for loader selection
 */
CziImageLoaderBase*
CziImageFile::TabOverlayInfo::getMultiResolutionImageLoader()
{
    CziImageLoaderBase* imageLoaderOut(NULL);
    
    switch (m_imageResolutionChangeMode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            CaretAssert(0);
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
        case CziImageResolutionChangeModeEnum::MANUAL2:
            if (m_multiResolutionImageLoader == NULL) {
                m_multiResolutionImageLoader.reset(new CziImageLoaderMultiResolution());
                m_multiResolutionImageLoader->initialize(m_tabIndex,
                                                         m_overlayIndex,
                                                         m_cziImageFile);
            }
            imageLoaderOut = m_multiResolutionImageLoader.get();
            break;
    }
    
    
    CaretAssert(imageLoaderOut);
    return imageLoaderOut;
}

/**
 * @return The selected image loader
 */
const CziImageLoaderBase*
CziImageFile::TabOverlayInfo::getMultiResolutionImageLoader() const
{
    TabOverlayInfo* nonConstThis(const_cast<TabOverlayInfo*>(this));
    CaretAssert(nonConstThis);
    return nonConstThis->getMultiResolutionImageLoader();
}

/**
 * Clone from the given tag overlay info
 * @param otherTabOverlayInfo
 *    Tab overlay info that is cloned
 */
void
CziImageFile::TabOverlayInfo::cloneFromOtherTabOverlayInfo(TabOverlayInfo* otherTabOverlayInfo)
{
    CaretAssert(otherTabOverlayInfo);
    m_imageResolutionChangeMode = otherTabOverlayInfo->m_imageResolutionChangeMode;
}

/**
 * Reset content such as when file is closed
 */
void
CziImageFile::TabOverlayInfo::resetContent()
{
    m_imageResolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;
    m_multiResolutionImageLoader.reset();
}

