
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
#include "EventManager.h"
#include "GiftiMetaData.h"
#include "GraphicsPrimitiveV3fT3f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "RectangleTransform.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "VolumeSpace.h"

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
    

    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}

/**
 * Destructor.
 */
CziImageFile::~CziImageFile()
{
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
            defImage = readPyramidLevelFromCziImageFile(m_numberOfPyramidLayers - 2,
                                                        m_errorMessage);
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
        m_errorMessage = (filename + QString(e.what()));
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
            
            if (cziDebugFlag) {
                std::cout << "Layer number: " << (int)ply.pyramidLayerNo << " MinFactor: " << (int)ply.minificationFactor
                << " Sub-Blocks: " << pls.count
                << " width=" << width << " height=" << height << std::endl;                
            }
            
            libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo pyramidInfo;
            pyramidInfo.minificationFactor = ply.minificationFactor;
            pyramidInfo.pyramidLayerNo     = ply.pyramidLayerNo;
            
            PyramidLayer pyramidLayer(pyramidInfo,
                                      width,
                                      height);
            m_pyramidLayers.push_back(pyramidLayer);
        }
    }
    
    m_numberOfPyramidLayers = static_cast<int32_t>(m_pyramidLayers.size());
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
 * Read the specified region from the CZI file into an image of the given width and height.
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
 * Read a pyramid level from CZI file
 * @param errorMessageOut
 *    Contains information about any errors
 * @return
 *    Pointer to CziImage or NULL if there is an error.
 */
CziImage*
CziImageFile::readPyramidLevelFromCziImageFile(const int32_t pyramidLevel,
                                               AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    const int32_t numPyramidLayers(m_pyramidLayers.size());
    if (numPyramidLayers <= 0) {
        errorMessageOut = "There are no pyramid layers for accessing data";
        return NULL;
    }
    if ((pyramidLevel < 0)
        || (pyramidLevel >= numPyramidLayers)) {
        errorMessageOut = ("Invalid pyramid level="
                           + AString::number(pyramidLevel)
                           + " range is [0,"
                           + AString::number(numPyramidLayers - 1)
                           + "]");
        return NULL;
    }
    CaretAssertVectorIndex(m_pyramidLayers, pyramidLevel);
    libCZI::ISingleChannelPyramidLayerTileAccessor::PyramidLayerInfo pyramidInfo = m_pyramidLayers[pyramidLevel].m_layerInfo;
    
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
    const libCZI::IntRect intRectROI = CziUtilities::qRectToIntRect(m_fullResolutionLogicalRect);
    CaretAssert(m_pyramidLayerTileAccessor);
    std::shared_ptr<libCZI::IBitmapData> bitmapData = m_pyramidLayerTileAccessor->Get(pixelType,
                                                                                      intRectROI,
                                                                                      iDimCoord,
                                                                                      pyramidInfo,
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
 * @return width of media file in the given tab index
 * @param tabIndex
 *    Index of tab
 */
int32_t
CziImageFile::getWidth(const int32_t tabIndex) const
{
    const CziImage* cziImage = getImageForTab(tabIndex);
    if (cziImage != NULL) {
        return cziImage->getWidth();
    }
    return 0;
}

/**
 * @return height of media file in the given tab index
 * @param tabIndex
 *    Index of tab
 */
int32_t
CziImageFile::getHeight(const int32_t tabIndex) const
{
    const CziImage* cziImage = getImageForTab(tabIndex);
    if (cziImage != NULL) {
        return cziImage->getHeight();
    }
    return 0;
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
 * @return the default view transform
 * @param tabIndex
 *    Index of the tab
 */
DefaultViewTransform
CziImageFile::getDefaultViewTransform(const int32_t tabIndex) const
{
    if ( ! m_defaultViewTransformValidFlag) {
        const CziImage* cziImage(getImageForTab(tabIndex));
        CaretAssert(cziImage);
        GraphicsPrimitiveV3fT3f* primitive = cziImage->getGraphicsPrimitiveForMediaDrawing();
        if (primitive) {
            if (primitive->isValid()) {
                BoundingBox boundingBox;
                primitive->getVertexBounds(boundingBox);
                if (boundingBox.isValid2D()) {
                    const float imageHalfHeight(boundingBox.getDifferenceY() / 2.0);
                    if (imageHalfHeight > 0.0) {
                        /*
                         * Default scaling "fits" the image into the media drawing's orthographic viewport
                         */
                        float tx(0.0);
                        float ty(0.0);
                        float defaultScaling = MediaFile::getMediaDrawingOrthographicHalfHeight() / imageHalfHeight;
                        if (defaultScaling != 0.0) {
                            /*
                             * Need to alter translation with default scaling since viewport is a fixed
                             * size and has no relation to the image size
                             */
                            tx = -boundingBox.getCenterX() * defaultScaling;
                            ty = -boundingBox.getCenterY() * defaultScaling;
                        }

                        m_defaultViewTransform.setScaling(defaultScaling);
                        m_defaultViewTransform.setTranslation(tx, ty);

                        m_defaultViewTransformValidFlag = true;

                        if (cziDebugFlag) std::cout << "Default view transform: " << m_defaultViewTransform.toString() << std::endl;
                    }
                }
            }
        }
    }
    
    return m_defaultViewTransform;
}

/**
 * Get the identification text for the pixel at the given pixel index with origin at bottom left.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param pixelIndex
 *    Index of the pixel.
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
*/
void
CziImageFile::getPixelIdentificationText(const int32_t tabIndex,
                                         const PixelIndex& pixelIndex,
                                         std::vector<AString>& columnOneTextOut,
                                         std::vector<AString>& columnTwoTextOut,
                                         std::vector<AString>& toolTipTextOut) const
{
    columnOneTextOut.clear();
    columnTwoTextOut.clear();
    toolTipTextOut.clear();
    if ( ! isPixelIndexValid(tabIndex, pixelIndex)) {
        return;
    }
    
    const CziImage* cziImage = getImageForTab(tabIndex);
    if (cziImage != NULL) {
        cziImage->getPixelIdentificationText(getFileNameNoPath(),
                                             pixelIndex,
                                             columnOneTextOut,
                                             columnTwoTextOut,
                                             toolTipTextOut);
    }
}

/**
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
                dataFileInformation.addNameAndValue(("Pyramid Layer "
                                                     + QString::number(i)),
                                                    (QString::number(pl.m_width)
                                                     + " x "
                                                     + QString::number(pl.m_height)));
            }
            
            const CziImage* cziImage = getDefaultImage();
            if (cziImage != NULL) {
                dataFileInformation.addNameAndValue("Logical X", cziImage->m_logicalRect.x());
                dataFileInformation.addNameAndValue("Logical Y", cziImage->m_logicalRect.y());
                dataFileInformation.addNameAndValue("Logical Width", cziImage->m_logicalRect.width());
                dataFileInformation.addNameAndValue("Logical Height", cziImage->m_logicalRect.height());
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
    return getDefaultImage();
}

/**
 * @return True if the given pixel index is valid for the image in the given tabf
 * @param tabIndex
 *    Index of the tab.
 * @param pixelIndex
 *     Image of pixel
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
 * Get the pixel RGBA at the given pixel I and J.
 *
 * @param tabIndex
 *    Index of the tab.
 * @param imageOrigin
 *    Location of first pixel in the image data.
 * @param pixelIndex
 *     Image of pixel
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
            break;
        case IMAGE_DATA_ORIGIN_AT_TOP:
        {
            /*
             * Convert to bottom origin
             */
            pixelIndexIJ = cziImage->transformPixelIndexToSpace(pixelIndex,
                                                              CziPixelCoordSpaceEnum::PIXEL_TOP_LEFT,
                                                              CziPixelCoordSpaceEnum::PIXEL_BOTTOM_LEFT);
        }
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
CziImageFile::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
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
CziImageFile::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_defaultViewTransform.reset();
    m_defaultViewTransformValidFlag = false;
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}
