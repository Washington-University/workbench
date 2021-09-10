
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

#include <limits>

#include <QImage>

#include "BackgroundAndForegroundColors.h"
#include "BoundingBox.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "CziImage.h"
#include "CziUtilities.h"
#include "DataFileContentInformation.h"
#include "DataFileException.h"
#include "EventCaretPreferencesGet.h"
#include "EventBrowserTabClose.h"
#include "EventBrowserTabDelete.h"
#include "EventBrowserTabNewClone.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "GraphicsObjectToWindowTransform.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "MathFunctions.h"
#include "Plane.h"
#include "RectangleTransform.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
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
    m_fileMetaData.reset(new GiftiMetaData());
    m_pyramidLayerIndexInTabs.fill(0);
    m_tabCziImagePyramidLevelChanged.fill(false);
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->addArray("m_pyramidLayerIndexInTabs",
                               m_pyramidLayerIndexInTabs.data(),
                               m_pyramidLayerIndexInTabs.size(),
                               0);
    
    /* NEED THIS AFTER Tile Tabs have been modified */
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_CLOSE);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_DELETE);
    EventManager::get()->addProcessedEventListener(this, EventTypeEnum::EVENT_BROWSER_TAB_NEW_CLONE);
}

/**
 * Destructor.
 */
CziImageFile::~CziImageFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
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
    return (getDefaultImage() == NULL);
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
    
    if (removeTabIndex >= 0) {
        CaretAssertVectorIndex(m_tabCziImages, removeTabIndex);
        m_tabCziImages[removeTabIndex].reset();
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
    
    /*
     * Desctructors will close the files
     */
    m_scalingTileAccessor.reset();
    m_reader->Close();
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
    m_defaultImage.reset();
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
         * Pyramid Information
         */
        readPyramidInfo(subBlockStatistics.boundingBox.w,
                        subBlockStatistics.boundingBox.h);
        
        
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
        if (m_numberOfPyramidLayers > 2) {
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
            m_pyramidLayerIndexInTabs.fill(defaultPyramidIndex);
        }
        if (defImage == NULL) {
            std::cout << "PYRAMID ERROR: " << m_errorMessage << std::endl;
            m_errorMessage.clear();
        }
        
        if (defImage == NULL) {
            defImage = readFromCziImageFile(m_fullResolutionLogicalRect,
                                            4096,
                                            m_errorMessage);
        }
        if (defImage == NULL) {
            m_status = Status::ERRORED;
            return;
        }
        
        m_defaultImage.reset(defImage);
        
        /*
         * File is now open
         */
        m_status = Status::OPEN;
        
        clearModified();
    }
    catch (const std::exception& e) {
        m_errorMessage = ("std::exception " + filename + QString(e.what()));
        m_status = Status::ERRORED;
    }
    catch (const std::out_of_range& e) {
        m_errorMessage = ("std::out_of_range " + filename + QString(e.what()));
        m_status = Status::ERRORED;
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
 * Read pyramid from the file
 * @param imageWidth
 *    Width of image
 * @param imageHeight
 *    Height of image
 */
void
CziImageFile::readPyramidInfo(const int64_t imageWidth,
                              const int64_t imageHeight)
{
    m_numberOfPyramidLayers = 0;
    if ((imageWidth <= 0)
        || (imageHeight <= 0)) {
        return;
    }
    
    int64_t width(imageWidth);
    int64_t height(imageHeight);
    libCZI::PyramidStatistics pyramidStatistics = m_reader->GetPyramidStatistics();
    for (auto& sceneIter : pyramidStatistics.scenePyramidStatistics) {
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
            
            PyramidLayer pyramidLayer(pyramidInfo,
                                      width,
                                      height);
            m_pyramidLayers.push_back(pyramidLayer);
        }
        
        std::sort(m_pyramidLayers.begin(),
                  m_pyramidLayers.end(),
                  [=](PyramidLayer a, PyramidLayer b) { return (a.m_layerInfo.pyramidLayerNo > b.m_layerInfo.pyramidLayerNo); } );
    }
    
    m_numberOfPyramidLayers = static_cast<int32_t>(m_pyramidLayers.size());
    
    if (cziDebugFlag) {
        for (int32_t i = 0; i < m_numberOfPyramidLayers; i++) {
            const auto& pl = m_pyramidLayers[i];
            std::cout << "Index=" << i << " CZI Pyramid Layer Number: " << (int)pl.m_layerInfo.pyramidLayerNo << " MinFactor: " << (int)pl.m_layerInfo.minificationFactor
            << " width=" << pl.m_width << " height=" << pl.m_height << std::endl;
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
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readFromCziImageFile(const QRectF& regionOfInterest,
                                   const int64_t outputImageWidthHeightMaximum,
                                   AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    if ( ! regionOfInterest.isValid()) {
        errorMessageOut = "Region of interest for reading from file is invalid";
        return NULL;
    }
    
    libCZI::CDimCoordinate coordinate;
    coordinate.Set(libCZI::DimensionIndex::C, 0);
    
    libCZI::ISingleChannelScalingTileAccessor::Options scstaOptions; scstaOptions.Clear();
    scstaOptions.backGroundColor.r = 0.0;
    scstaOptions.backGroundColor.g = 0.0;
    scstaOptions.backGroundColor.b = 0.0;
    
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    if (prefs != NULL) {
        uint8_t backgroundColor[3];
        prefs->getBackgroundAndForegroundColors()->getColorBackgroundMediaView(backgroundColor);
        std::array<float, 3> rgb(BackgroundAndForegroundColors::toFloatRGB(backgroundColor));
        scstaOptions.backGroundColor.r = rgb[0];
        scstaOptions.backGroundColor.g = rgb[1];
        scstaOptions.backGroundColor.b = rgb[2];
    }
    
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
    if (outputImageWidthHeightMaximum < roiMaxWidthHeight) {
        zoom = static_cast<float>(outputImageWidthHeightMaximum) / static_cast<float>(roiMaxWidthHeight);
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
        errorMessageOut = "Failed to read data";
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
                                         CziUtilities::intRectToQRect(intRectROI));
    return cziImageOut;
}

/**
 * @return The pyramid layer for a tab.
 * @param tabIndex
 * Index of the tab
 */
int32_t
CziImageFile::getPyramidLayerIndexForTab(const int32_t tabIndex) const
{
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
        if ((layerInfo.m_width > resolution)
            || (layerInfo.m_height > resolution)) {
            break;
        }
        layerIndex = i;
    }
    
    return layerIndex;
}
/**
 * Read a pyramid layer from CZI file
 * @param pyramidLayer
 *    Index of pyramid layer to read
 * @param logicalRectangleRegionRect
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
    coordinate.Set(libCZI::DimensionIndex::C, 0);
    libCZI::IDimCoordinate* iDimCoord = &coordinate;
    
    libCZI::ISingleChannelPyramidLayerTileAccessor::Options scstaOptions;
    scstaOptions.Clear();
    scstaOptions.backGroundColor.r = 0.0;
    scstaOptions.backGroundColor.g = 0.0;
    scstaOptions.backGroundColor.b = 0.0;
    
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    if (prefs != NULL) {
        uint8_t backgroundColor[3];
        prefs->getBackgroundAndForegroundColors()->getColorBackgroundMediaView(backgroundColor);
        std::array<float, 3> rgb(BackgroundAndForegroundColors::toFloatRGB(backgroundColor));
        scstaOptions.backGroundColor.r = rgb[0];
        scstaOptions.backGroundColor.g = rgb[1];
        scstaOptions.backGroundColor.b = rgb[2];
    }
    
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
                                         CziUtilities::intRectToQRect(rectToReadROI));
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
 *
 * @param pyramidLayerIndex
 *    Index of pyramid layer
 * @param widthInOut
 *    Width (input pixels; output logical)
 * @param heightInOut
 *    Height (input pixels; output logical)
 */
void
CziImageFile::pixelSizeToLogicalSize(const int32_t pyramidLayerIndex,
                                     int32_t& widthInOut,
                                     int32_t& heightInOut) const
{
    CaretAssertVectorIndex(m_pyramidLayers, pyramidLayerIndex);
    const int32_t pixelScale = CalcSizeOfPixelOnLayer0(m_pyramidLayers[pyramidLayerIndex].m_layerInfo);
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
    if (m_defaultImage != NULL) {
        return 1;
    }
    return 0;
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param pixelIndexOriginAtTop
 *    Index of the pixel with origin at top
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
CziImageFile::getPixelIdentificationText(const int32_t tabIndex,
                                         const PixelIndex& pixelIndexOriginAtTop,
                                         std::vector<AString>& columnOneTextOut,
                                         std::vector<AString>& columnTwoTextOut,
                                         std::vector<AString>& toolTipTextOut) const
{
    columnOneTextOut.clear();
    columnTwoTextOut.clear();
    toolTipTextOut.clear();
    if ( ! isPixelIndexValid(tabIndex, pixelIndexOriginAtTop)) {
        return;
    }
    
    const CziImage* cziImage = getImageForTab(tabIndex);
    if (cziImage != NULL) {
        cziImage->getPixelIdentificationText(getFileNameNoPath(),
                                             pixelIndexOriginAtTop,
                                             columnOneTextOut,
                                             columnTwoTextOut,
                                             toolTipTextOut);
        
        std::array<float, 3> debugPixelIndex;
        std::array<float, 3> xyz;
        if (pixelIndexToStereotaxicXYZ(pixelIndexOriginAtTop, false, xyz, debugPixelIndex)) {
            columnOneTextOut.push_back("Stereotaxic XYZ");
            columnTwoTextOut.push_back(AString::fromNumbers(xyz.data(), 3, ", "));
            
            if (CaretLogger::getLogger()->isFine()) {
                PixelIndex newPixelIndex;
                if (stereotaxicXyzToPixelIndex(xyz, false, newPixelIndex, debugPixelIndex)) {
                    columnOneTextOut.push_back("XYZ Back to Pixel Test");
                    columnTwoTextOut.push_back(newPixelIndex.toString());
                }
            }
        }
        
        if (pixelIndexToStereotaxicXYZ(pixelIndexOriginAtTop, true, xyz, debugPixelIndex)) {
            columnOneTextOut.push_back("Stereotaxic XYZ with NIFTI warping");
            columnTwoTextOut.push_back(AString::fromNumbers(xyz.data(), 3, ", "));
            
            if (CaretLogger::getLogger()->isFine()) {
                PixelIndex newPixelIndex;
                if (stereotaxicXyzToPixelIndex(xyz, true, newPixelIndex, debugPixelIndex)) {
                    columnOneTextOut.push_back("XYZ Back to Pixel with NIFTI warping Test");
                    columnTwoTextOut.push_back(newPixelIndex.toString());
                }
            }
        }
    }
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
 * @return
 *    True if conversion successful, else false.
 */
bool
CziImageFile::pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndexOriginAtTop,
                                         const bool includeNonlinearFlag,
                                         std::array<float, 3>& xyzOut) const
{
    std::array<float, 3> debugPixelIndex;
    return pixelIndexToStereotaxicXYZ(pixelIndexOriginAtTop,
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
CziImageFile::pixelIndexToStereotaxicXYZ(const PixelIndex& pixelIndexOriginAtTop,
                                         const bool includeNonlinearFlag,
                                         std::array<float, 3>& xyzOut,
                                         std::array<float, 3>& debugPixelIndexOut) const
{
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
 * @param pixelIndexOut
 *    Output with pixel index in full resolution with origin at top left
 * @param debugPixelIndex
 *    Pixel index used for debugging
 * @return
 *    True if successful, else false.
 */
bool
CziImageFile::stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                         const bool includeNonlinearFlag,
                                         PixelIndex& pixelIndexOriginAtTopLeftOut) const
{
    std::array<float, 3> debugPixelIndex;
    return stereotaxicXyzToPixelIndex(xyz,
                                      includeNonlinearFlag,
                                      pixelIndexOriginAtTopLeftOut,
                                      debugPixelIndex);
}

/**
 * Convert a stereotaxic xyz coordinate to a pixel index
 * @param xyz
 *    The coordinate
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param pixelIndexOut
 *    Output with pixel index in full resolution with origin at top left
 * @param debugPixelIndex
 *    Pixel index used for debugging
 * @return
 *    True if successful, else false.
 */
bool
CziImageFile::stereotaxicXyzToPixelIndex(const std::array<float, 3>& xyz,
                                         const bool includeNonlinearFlag,
                                         PixelIndex& pixelIndexOriginAtTopLeftOut,
                                         const std::array<float, 3>& debugPixelIndex) const
{
    pixelIndexOriginAtTopLeftOut.setIJK(-1, -1, -1);
    
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
        
        
        pixelIndexOriginAtTopLeftOut.setIJK(pt[0], pt[1], pt[2]);
        
        return true;
    }
    
    return false;
}

/**
 * Find the Pixel nearest the given XYZ coordinate
 * @param xyz
 *    The coordinate
 * @param includeNonlinearFlag
 *    If true, include the non-linear transform when converting
 * @param signedDistanceToPixelMillimetersOut
 *    Output with signed distance to the pixel in millimeters
 * @param pixelIndexOriginAtTopLeftOut
 *    Output with pixel index in full resolution with origin at top left
 * @return
 *    True if successful, else false.
 */
bool
CziImageFile::findPixelNearestStereotaxicXYZ(const std::array<float, 3>& xyz,
                                             const bool includeNonLinearFlag,
                                             float& signedDistanceToPixelMillimetersOut,
                                             PixelIndex& pixelIndexOriginAtTopLeftOut) const
{
    const Plane* plane(getImagePlane());
    if (plane == NULL) {
        return false;
    }
    
    std::array<float, 3> xyzOnPlane;
    plane->projectPointToPlane(xyz.data(), xyzOnPlane.data());
    
    if (stereotaxicXyzToPixelIndex(xyzOnPlane,
                                   includeNonLinearFlag,
                                   pixelIndexOriginAtTopLeftOut)) {
        if (isPixelIndexFullResolutionValid(pixelIndexOriginAtTopLeftOut)) {
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
    const int64_t zero(0);
    const int64_t pixelWidth(getWidth() - 1);
    const int64_t pixelHeight(getHeight() - 1);
    const PixelIndex bottomLeftPixel(zero, pixelHeight, zero);
    const PixelIndex topLeftPixel(zero, zero, zero);
    const PixelIndex topRightPixel(pixelWidth, zero, zero);
    
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
                const int64_t diffI(std::abs(m_pyramidLayers[i].m_width - dimI));
                const int64_t diffJ(std::abs(m_pyramidLayers[i].m_height - dimJ));
                const int64_t diff(diffI + diffJ);
                if (diff < maxDiff) {
                    maxDiff = diff;
                    pyramidLayerIndex = i;
                }
            }
            
            if (pyramidLayerIndex >= 0) {
                CaretAssertVectorIndex(m_pyramidLayers, pyramidLayerIndex);
                const PyramidLayer& pyramidLayer = m_pyramidLayers[pyramidLayerIndex];
                transform.m_pixelScaleI = (static_cast<float>(pyramidLayer.m_width)
                                           / getWidth());
                transform.m_pixelScaleJ = (static_cast<float>(pyramidLayer.m_height)
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
                             + AString::number(pyramidLayer.m_width)
                             + " height: "
                             + AString::number(pyramidLayer.m_height)
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
    
    if (m_defaultImage != NULL) {
        const QImage* qImage = m_defaultImage->m_image.get();
        if (qImage != NULL) {
            dataFileInformation.addNameAndValue("Pixel Size X (mm)", m_pixelSizeMmX, 6);
            dataFileInformation.addNameAndValue("Pixel Size Y (mm)", m_pixelSizeMmY, 6);
            dataFileInformation.addNameAndValue("Pixel Size Z (mm)", m_pixelSizeMmZ, 6);
            dataFileInformation.addNameAndValue("Width (pixels)", qImage->width());
            dataFileInformation.addNameAndValue("Height (pixels)", qImage->height());
            
            const int32_t numPyramidLayers(m_pyramidLayers.size());
            for (int32_t i = 0; i < numPyramidLayers; i++) {
                const PyramidLayer& pl(m_pyramidLayers[i]);
                if (i == m_lowestResolutionPyramidLayerIndex) {
                    dataFileInformation.addNameAndValue("---", QString("--- Lowest Resolution Layer ---"));
                }
                dataFileInformation.addNameAndValue(("Index "
                                                     + QString::number(i)),
                                                    ("W/H: "
                                                     + QString::number(pl.m_width)
                                                     + " x "
                                                     + QString::number(pl.m_height)
                                                     + "; CZI Layer: "
                                                     + QString::number(pl.m_layerInfo.pyramidLayerNo)
                                                     + "; Min Factor: "
                                                     + QString::number(pl.m_layerInfo.minificationFactor)
                                                     + "; Zoom From Low Res: "
                                                     + QString::number(pl.m_zoomLevelFromLowestResolutionImage, 'f', 2)));
            }
            
            const CziImage* cziImage = getDefaultImage();
            if (cziImage != NULL) {
                dataFileInformation.addNameAndValue("Logical X", cziImage->m_logicalRect.x());
                dataFileInformation.addNameAndValue("Logical Y", cziImage->m_logicalRect.y());
                dataFileInformation.addNameAndValue("Logical Width", cziImage->m_logicalRect.width());
                dataFileInformation.addNameAndValue("Logical Height", cziImage->m_logicalRect.height());
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
    }
}

/**
 * @return The default image.
 */
CziImage*
CziImageFile::getDefaultImage()
{
    return m_defaultImage.get();
}

/**
 * @return The default image.
 */
const CziImage*
CziImageFile::getDefaultImage() const
{
    return m_defaultImage.get();
}

/**
 * @return CZI image for the given tab
 * @param tabIndex
 *    Index of the tab
 */
CziImage*
CziImageFile::getImageForTab(const int32_t tabIndex)
{
    CaretAssertVectorIndex(m_pyramidLayerIndexInTabs, tabIndex);
    if (m_pyramidLayerIndexInTabs[tabIndex] != m_lowestResolutionPyramidLayerIndex) {
        if (m_tabCziImages[tabIndex]) {
            return m_tabCziImages[tabIndex].get();
        }
    }
    
    return getDefaultImage();
}

/**
 * @return CZI image for the given tab
 * @param tabIndex
 *    Index of the tab
 */
const CziImage*
CziImageFile::getImageForTab(const int32_t tabIndex) const
{
    if (tabIndex >= 0) {
        CaretAssertVectorIndex(m_pyramidLayerIndexInTabs, tabIndex);
        if (m_pyramidLayerIndexInTabs[tabIndex] != m_lowestResolutionPyramidLayerIndex) {
            if (m_tabCziImages[tabIndex]) {
                return m_tabCziImages[tabIndex].get();
            }
        }
    }
    
    return getDefaultImage();
}

/**
 * @return CZI image for the given tab for drawing
 * @param tabIndex
 *    Index of the tab
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @param resolutionChangeMode
 *    Mode for changing resolutiln (auto/manual)
 */
const CziImage*
CziImageFile::getImageForDrawingInTab(const int32_t tabIndex,
                                      const GraphicsObjectToWindowTransform* transform,
                                      const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode)
{
    CaretAssertStdArrayIndex(m_pyramidLayerIndexInTabs, tabIndex);
    const int32_t pyramidLayerIndex = m_pyramidLayerIndexInTabs[tabIndex];
    
    /*
     * Lowest pyramid layer is always the default image
     */
    CziImage* cziImageOut(NULL);
    if (pyramidLayerIndex == m_lowestResolutionPyramidLayerIndex) {
        cziImageOut = getDefaultImage();
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
                return getDefaultImage();
            }
        }
    }
    
    if (cziImageOut != NULL) {
        /*
         * For AUTO mode
         */
        switch (resolutionChangeMode) {
            case CziImageResolutionChangeModeEnum::AUTO_OLD:
                autoModeZoomOnlyResolutionChange(tabIndex,
                                                 transform);
                break;
            case CziImageResolutionChangeModeEnum::AUTO:
                autoModePanZoomResolutionChange(cziImageOut,
                                                tabIndex,
                                                transform);
                break;
            case CziImageResolutionChangeModeEnum::MANUAL:
                break;
        }
    }
    
    return cziImageOut;
}

/**
 * Process change in resoluion for auto mode for zooming and panning
 * @param cziImage
 *    Current CZI image
 * @param tabIndex
 *    Index of the tab
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @return
 *    Pyramid layer index selected
 */
int32_t
CziImageFile::autoModePanZoomResolutionChange(const CziImage* cziImage,
                                              const int32_t tabIndex,
                                              const GraphicsObjectToWindowTransform* transform)
{
    const int32_t previousPyramidLayerIndex = getPyramidLayerIndexForTab(tabIndex);
    
    const int32_t pyramidLayerIndex = autoModeZoomOnlyResolutionChange(tabIndex,
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
                                     viewport.y() + viewport.height(),
                                     0.0,
                                     viewportTopLeftWindowCoordinate);
    const PixelIndex pixelIndexTopLeft(viewportTopLeftWindowCoordinate);
    const PixelIndex windowLogicalTopLeft(cziImage->transformPixelIndexToSpace(pixelIndexTopLeft,
                                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
                                                                               CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT));
    
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
    const PixelIndex pixelIndexBottomRight(viewportBottomRightWindowCoordinate);
    const PixelIndex windowLogicalBottomRight(cziImage->transformPixelIndexToSpace(pixelIndexBottomRight,
                                                                                   CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
                                                                                   CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT));
    
    /*
     * CZI Logical coordinates of viewport (portion of CZI image that fills the viewport)
     */
    const QRectF viewportFullResLogicalRect(windowLogicalTopLeft.getI(),
                                            windowLogicalTopLeft.getJ(),
                                            windowLogicalBottomRight.getI() - windowLogicalTopLeft.getI(),
                                            windowLogicalBottomRight.getJ() - windowLogicalTopLeft.getJ());
    if (cziDebugFlag) {
        std::cout << "Pixel Index Top Left (origin bottom left): " << pixelIndexTopLeft.toString() << std::endl;
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
        std::cout << "Image Logical Rect: " << CziUtilities::qRectToString(cziImage->m_logicalRect) << std::endl;
        
        const QRectF imageIntersectWindowRect(viewportFullResLogicalRect.intersected(cziImage->m_logicalRect));
        const float imageInWindowArea(imageIntersectWindowRect.width() * imageIntersectWindowRect.height());
        const float imageArea(cziImage->m_logicalRect.width()* cziImage->m_logicalRect.height());
        const float viewedPercentage((imageArea > 0.0f)
                                     ? (imageInWindowArea / imageArea)
                                     : imageArea);
        std::cout << "Image Viewed Percentage: " << viewedPercentage << std::endl;
    }
    
    /*
     * (1) Find intersection of currently loaded image region with the viewport region
     * (2) Find amount of viewport that overlaps the current image region
     */
    const QRectF viewportIntersectImageRect(cziImage->m_logicalRect.intersected(viewportFullResLogicalRect));
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
 * @param transform
 *    Transform for converts from object to window space (and inverse)
 * @return
 *    Pyramid layer index selected
 */
int32_t
CziImageFile::autoModeZoomOnlyResolutionChange(const int32_t tabIndex,
                                               const GraphicsObjectToWindowTransform* transform)
{
    /*
     * Compute pixel height of the high-resolution image when drawn in the window.
     * The size of the image, in pixels when drawn, determines when to switch to
     * lower or higher resolution image.
     */
    const float imageBottomLeftPixel[3] { 0.0, 0.0, 0.0 };
    const float imageTopLeftPixel[3] { 0.0, static_cast<float>(m_fullResolutionLogicalRect.height()), 0.0 };
    float imageBottomLeftWindow[3];
    float imageTopLeftWindow[3];
    transform->transformPoint(imageBottomLeftPixel, imageBottomLeftWindow);
    transform->transformPoint(imageTopLeftPixel, imageTopLeftWindow);
    const float drawnPixelHeight = imageTopLeftWindow[1] - imageBottomLeftWindow[1];
    
    int32_t pyramidLayerForScaling = m_highestResolutionPyramidLayerIndex;
    for (int32_t i = m_lowestResolutionPyramidLayerIndex;
         i <= m_highestResolutionPyramidLayerIndex;
         i++) {
        if (drawnPixelHeight < m_pyramidLayers[i].m_height) {
            pyramidLayerForScaling = i;
            break;
        }
    }
    if (pyramidLayerForScaling != getPyramidLayerIndexForTab(tabIndex)) {
        setPyramidLayerIndexForTab(tabIndex,
                                   pyramidLayerForScaling);
    }
    
    return pyramidLayerForScaling;
}

/**
 * Load a image from the given pyramid layer for the center of the tab region defined by the transform
 * @param tabIndex
 *    Index of the tab
 * @param transform
 *    Transform from the tab where image is drawn
 * @param pyramidLayerIndex
 *    Index of the pyramid layer
 */
CziImage*
CziImageFile::loadImageForPyrmaidLayer(const int32_t tabIndex,
                                       const GraphicsObjectToWindowTransform* transform,
                                       const int32_t pyramidLayerIndex)
{
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
    
    CziImage* oldCziImage(getImageForTab(tabIndex));
    CaretAssert(oldCziImage);
    PixelIndex fullImagePixelIndex = imagePixelIndex;
    PixelIndex fullResolutionLogicalPixelIndex = oldCziImage->transformPixelIndexToSpace(imagePixelIndex,
                                                                                         CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
                                                                                         CziPixelCoordSpaceEnum::FULL_RESOLUTION_LOGICAL_TOP_LEFT);
    
    /*
     * Get preferred image size from preferences
     */
    EventCaretPreferencesGet prefsEvent;
    EventManager::get()->sendEvent(prefsEvent.getPointer());
    CaretPreferences* prefs = prefsEvent.getCaretPreferences();
    int32_t preferredImageDimension(2048);
    if (prefs != NULL) {
        preferredImageDimension = prefs->getCziDimension();
    }
    CaretAssert(preferredImageDimension >= 512);
    
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
    pixelSizeToLogicalSize(pyramidLayerIndex, roiWidth, roiHeight);
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
    if (oldCziImage->m_logicalRect == adjustedRect) {
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
 * @return True if the given pixel index is valid for the image in the given tab
 * @param tabIndex
 *    Index of the tab.
 * @param pixelIndex
 *     Image of pixel in FULL RES image
 */
bool
CziImageFile::isPixelIndexValid(const int32_t tabIndex,
                                const PixelIndex& pixelIndex) const
{
    const CziImage* cziImage = getImageForTab(tabIndex);
    if (cziImage != NULL) {
        return cziImage->isPixelIndexValid(pixelIndex);
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
 * @param imageOrigin
 *    Location of first pixel in the image data.
 * @param pixelIndex
 *     Image of pixel in FULL RES image with origin bottom left
 * @param pixelRGBAOut
 *     RGBA at Pixel I, J
 * @return
 *     True if valid, else false.
 */
bool
CziImageFile::getImagePixelRGBA(const int32_t tabIndex,
                                const IMAGE_DATA_ORIGIN_LOCATION imageOrigin,
                                const PixelIndex& pixelIndex,
                                uint8_t pixelRGBAOut[4]) const
{
    const CziImage* cziImage = getImageForTab(tabIndex);
    CaretAssert(cziImage);
    
    PixelIndex pixelIndexIJ(pixelIndex);
    switch (imageOrigin) {
        case IMAGE_DATA_ORIGIN_AT_BOTTOM:
        {
            /*
             * Convert bottom origin to top origin
             */
            pixelIndexIJ = cziImage->transformPixelIndexToSpace(pixelIndexIJ,
                                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_BOTTOM_LEFT,
                                                                CziPixelCoordSpaceEnum::FULL_RESOLUTION_PIXEL_TOP_LEFT);
        }
            break;
        case IMAGE_DATA_ORIGIN_AT_TOP:
            break;
    }
    
    return cziImage->getImagePixelRGBA(pixelIndexIJ,
                                       pixelRGBAOut);
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

