
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

/*=========================================================================
 *  Code for reading OME-ZARR is adapted/copied from
 *  https://github.com/InsightSoftwareConsortium/ITKIOOMEZarrNGFF/blob/main/src/itkOMEZarrNGFFImageIO.cxx#L328
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#define __OME_ZARR_IMAGE_FILE_DECLARE__
#include "OmeZarrImageFile.h"
#undef __OME_ZARR_IMAGE_FILE_DECLARE__

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
#include "GraphicsPrimitiveV3fT2f.h"
#include "GraphicsUtilitiesOpenGL.h"
#include "ImageFile.h"
#include "MathFunctions.h"
#include "MediaFileChannelInfo.h"
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
#include "OmeAttrsV0p4JsonFile.h"
#include "OmeFileReader.h"
#endif
#include "Plane.h"
#include "RectangleTransform.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "StringTableModel.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class caret::OmeZarrImageFile
 * \brief A Zeiss CZI image file
 * \ingroup Files
 */

/**
 * Constructor.
 */
OmeZarrImageFile::OmeZarrImageFile()
: MediaFile(DataFileTypeEnum::OME_ZARR_IMAGE_FILE)
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
OmeZarrImageFile::~OmeZarrImageFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Reset before file reading.
 */
void
OmeZarrImageFile::resetPrivate()
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

    m_pyramidLevels.clear();
    
    m_pixelSizeMmX = 1.0f;
    m_pixelSizeMmY = 1.0f;
    m_pixelSizeMmZ = 1.0f;
    m_fileMetaData.reset(new GiftiMetaData());
    m_fullResolutionLogicalRect = QRectF();
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
    
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    m_omeFileReader.reset();
#endif
}

/**
 * Close the file
 */
void
OmeZarrImageFile::closeFile()
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
OmeZarrImageFile*
OmeZarrImageFile::castToOmeZarrImageFile()
{
    return this;
}

/**
 * @return File casted to an image file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const OmeZarrImageFile*
OmeZarrImageFile::castToOmeZarrImageFile() const
{
    return this;
}

/**
 * Clear this file's modified status
 */
void
OmeZarrImageFile::clearModified()
{
    MediaFile::clearModified();
    m_fileMetaData->clearModified();
}

/**
 * @return True if this file is modified, else falsel
 */
bool
OmeZarrImageFile::isModified() const
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
OmeZarrImageFile::isEmpty() const
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
OmeZarrImageFile::getFileMetaData()
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
OmeZarrImageFile::receiveEvent(Event* event)
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
OmeZarrImageFile::getFileMetaData() const
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
OmeZarrImageFile::readFile(const AString& filename)
{
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

    resetPrivate();
    
    if (filename.isEmpty()) {
        throw DataFileException("Filename is empty.");
    }
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    /*
     * Initialize the ZARR file for reading data
     */
    OmeFileReader* omeFileReader(new OmeFileReader());
    const FunctionResult result(omeFileReader->initialize(filename));
    if (result.isError()) {
        m_status = Status::ERRORED;
        throw DataFileException(result.getErrorMessage());
    }
    
    m_omeFileReader.reset(omeFileReader);
    
    setFileName(filename);
            
    const OmeAttrsV0p4JsonFile* zattrs(m_omeFileReader->getZAttrs());
    if (zattrs == NULL) {
        throw DataFileException("Failed to get ZAttrs (is NULL) from OmeFileReader");
    }
    const int32_t numLevels(zattrs->getNumberOfDataSets());
    if (numLevels <= 0) {
        throw DataFileException("No images were read from OME ZARR file");
    }
    for (int32_t i = 0; i < numLevels; i++) {
        const OmeDataSet* dataSet(zattrs->getDataSet(i));
        PyramidLevelInfo pli(dataSet->getWidth(),
                             dataSet->getHeight(),
                             dataSet->getNumberOfSlices(),
                             Vector3D(0, 0, 0),
                             Vector3D(dataSet->getWidth(),
                                      dataSet->getHeight(),
                                      dataSet->getNumberOfSlices()));
        m_pyramidLevels.push_back(pli);
    }

    if (m_pyramidLevels.empty()) {
        throw DataFileException("No image pyramids were read from file");
    }

    CaretAssertVectorIndex(m_pyramidLevels, 0);
    m_fullResolutionLogicalRect = QRectF(0, 0,
                                         m_pyramidLevels[0].m_pixelWidth,
                                         m_pyramidLevels[0].m_pixelHeight);
    m_status = Status::OPEN;
#else /* WORKBENCH_HAVE_OME_ZARR_Z5 */
    m_status = Status::ERRORED;
    throw DataFileException("Workbench has been compiled without OME-ZARR Z5");
#endif /* WORKBENCH_HAVE_OME_ZARR_Z5 */
}

/**
 * @return Size of pixel in millimeters for X, Y, and Z dimensions
 */
PixelCoordinate
OmeZarrImageFile::getPixelSizeInMillimeters() const
{
    return PixelCoordinate(m_pixelSizeMmX,
                           m_pixelSizeMmY,
                           m_pixelSizeMmZ);
}

/**
 * Add to metadata if text is not empty
 * @param name
 *    Name of metadata item
 * @param text
 *    Text of metadfata
 */
void
OmeZarrImageFile::addToMetadataIfNotEmpty(const AString& name,
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
OmeZarrImageFile::zoomToMatchPixelDimension(const QRectF& regionOfInterestToRead,
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
OmeZarrImageFile::getPyramidLayerRangeForFrame(const int32_t /*frameIndex*/,
                                               const bool /*allFramesFlag*/,
                                               int32_t& lowestPyramidLayerIndexOut,
                                               int32_t& highestPyramidLayerIndexOut) const
{
    lowestPyramidLayerIndexOut  = 0;
    highestPyramidLayerIndexOut = (m_pyramidLevels.size() - 1);
}

/**
 * Reload the pyramid layer in the given tab.
 * @param tabIndex
 *    Index of the tab.
 * @param overlayIndex
 * Index of overlasy
 */
void
OmeZarrImageFile::reloadPyramidLayerInTabOverlay(const int32_t tabIndex,
                                             const int32_t overlayIndex)
{
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
    m_tabOverlayInfo[tabIndex][overlayIndex]->m_graphicsPrimitive.reset();
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
OmeZarrImageFile::writeFile(const AString& filename)
{
    /*
     * Cannot write CZI file
     */
    throw DataFileException(filename,
                            "Writing of OME-ZARR image files is not supported");
}

/**
 *  @return True if the file supports writing, else false.
 */
bool
OmeZarrImageFile::supportsWriting() const
{
    return false;
}

/**
 * @return width of media file
 */
int32_t
OmeZarrImageFile::getWidth() const
{
    return m_fullResolutionLogicalRect.width();
}

/**
 * @return height of media file
 */
int32_t
OmeZarrImageFile::getHeight() const
{
    return m_fullResolutionLogicalRect.height();
}

/**
 * @return Number of frames in the file
 */
int32_t
OmeZarrImageFile::getNumberOfFrames() const
{
    return getNumberOfScenes();
}

/**
 * @return Number of scenes in the file
 */
int32_t
OmeZarrImageFile::getNumberOfScenes() const
{
    if ( ! m_pyramidLevels.empty()) {
        CaretAssertVectorIndex(m_pyramidLevels, 0);
        return m_pyramidLevels[0].m_pixelSlices;
    }
    return 0;
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
OmeZarrImageFile::getPixelLogicalIdentificationTextForFrames(const int32_t tabIndex,
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
    if (logicalPixelIndexToStereotaxicXYZ(pixelLogicalIndex, xyz)) {
        columnOneTextOut.push_back("Stereotaxic XYZ");
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
OmeZarrImageFile::testPixelTransforms(const int32_t pixelIndexStep,
                                  const bool /*nonLinearFlag*/,
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
            if ( ! logicalPixelIndexToStereotaxicXYZ(pixelLogicalIndex,
                                                     xyz)) {
                resultsMessageOut.appendWithNewLine("Failed to convert pixel to xyz.  Pixel="
                                                    + pixelLogicalIndex.toString());
                continue;
            }
            
            PixelLogicalIndex pixelLogicalIndexTwo;
            if ( ! stereotaxicXyzToLogicalPixelIndex(xyz,
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
 * @param signedDistanceToPixelMillimetersOut
 *    Output with signed distance to the pixel in millimeters
 * @param pixelLogicalIndexOut
 *    Output with logical pixel index
 * @return
 *    True if successful, else false.
 */
bool
OmeZarrImageFile::findPixelNearestStereotaxicXYZ(const Vector3D& xyz,
                                             float& signedDistanceToPixelMillimetersOut,
                                             PixelLogicalIndex& pixelLogicalIndexOut) const
{
    const Plane* plane(getImagePlane());
    if (plane == NULL) {
        return false;
    }
    
    Vector3D xyzOnPlane;
    plane->projectPointToPlane(xyz, xyzOnPlane);
    
    if (stereotaxicXyzToLogicalPixelIndex(xyzOnPlane,
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
OmeZarrImageFile::getImagePlane() const
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
    if (logicalPixelIndexToStereotaxicXYZ(bottomLeftPixel, bottomLeftXYZ)
        && logicalPixelIndexToStereotaxicXYZ(topLeftPixel, topLeftXYZ)
        && logicalPixelIndexToStereotaxicXYZ(topRightPixel, topRightXYZ)) {
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
 * Add to the data file information.
 * @param dataFileInformation
 *    Item to which information is added.
 */
void
OmeZarrImageFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
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
    
    getMediaFileChannelInfo()->addToDataFileContentInformation(dataFileInformation);
}

/**
 * @return A pixel index converted from a pixel logical index.
 * @param pixelLogicalIndex
 *    The logical pixel index.
 */
PixelIndex
OmeZarrImageFile::pixelLogicalIndexToPixelIndex(const PixelLogicalIndex& pixelLogicalIndex) const
{
    return MediaFile::pixelLogicalIndexToPixelIndex(pixelLogicalIndex);
}

/**
 * @return A pixel logical index converted from a pixel index.
 * @param pixelIndex
 *    The  pixel index.
 */
PixelLogicalIndex
OmeZarrImageFile::pixelIndexToPixelLogicalIndex(const PixelIndex& pixelIndex) const
{
    return MediaFile::pixelIndexToPixelLogicalIndex(pixelIndex);
}

/**
 * @return Return a rectangle that defines the bounds of the media data
 */
QRectF
OmeZarrImageFile::getLogicalBoundsRect() const
{
    return m_fullResolutionLogicalRect;
}

/* Update CZI image for the given tab for drawing
 * @param tabIndex
 *    Index of the tab
 * @param overlayIndex
 *    Index of overlay
 * @param sliceIndex
 *    Index of the slice
 * @param frameIndex
 *    Index of frame
 * @param pyramidLayer
 *    The pyramid layer
 * @param resolutionChangeMode
 *    The resolution change mode
 */
void
OmeZarrImageFile::updateImageForDrawingInTab(const int32_t tabIndex,
                                             const int32_t overlayIndex,
                                             const int32_t sliceIndex,
                                             const int32_t frameIndex,
                                             const int32_t pyramidLevel,
                                             const CziImageResolutionChangeModeEnum::Enum resolutionChangeMode)
{
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
    if ((m_tabOverlayInfo[tabIndex][overlayIndex]->m_sliceIndex != sliceIndex)
        || (m_tabOverlayInfo[tabIndex][overlayIndex]->m_frameIndex != frameIndex)
        || (m_tabOverlayInfo[tabIndex][overlayIndex]->m_pyramidLevel != pyramidLevel)
        || (m_tabOverlayInfo[tabIndex][overlayIndex]->m_resolutionChangeMode != resolutionChangeMode)) {
        m_tabOverlayInfo[tabIndex][overlayIndex]->m_graphicsPrimitive.reset();
    }
    m_tabOverlayInfo[tabIndex][overlayIndex]->m_sliceIndex = sliceIndex;
    m_tabOverlayInfo[tabIndex][overlayIndex]->m_frameIndex = frameIndex;
    m_tabOverlayInfo[tabIndex][overlayIndex]->m_pyramidLevel = pyramidLevel;
    m_tabOverlayInfo[tabIndex][overlayIndex]->m_resolutionChangeMode = resolutionChangeMode;
}

/**
 * @return The graphics primitive for drawing the image as a texture in media drawing model.  Can be NULL.
 * @param tabIndex
 *    Index of tab where image is drawn
 * @param overlayIndex
 *    Index of the overlay
 */
GraphicsPrimitiveV3fT2f*
OmeZarrImageFile::getGraphicsPrimitiveForMediaDrawing(const int32_t tabIndex,
                                                      const int32_t overlayIndex) const
{
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    const int64_t sliceIndex(m_tabOverlayInfo[tabIndex][overlayIndex]->m_frameIndex);
    
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    CaretAssertArrayIndex(m_tabOverlayInfo, BrainConstants::MAXIMUM_NUMBER_OF_OVERLAYS, overlayIndex);
    TabOverlayInfo* tabOverlayInfo(m_tabOverlayInfo[tabIndex][overlayIndex].get());
    CaretAssert(tabOverlayInfo);
    if (tabOverlayInfo->m_graphicsPrimitive) {
        return tabOverlayInfo->m_graphicsPrimitive.get();
    }
    
    if ( ! m_pyramidLevels.empty()) {
        if (m_omeFileReader) {
            const OmeAttrsV0p4JsonFile* zattrs(m_omeFileReader->getZAttrs());
            if (zattrs == NULL) {
                CaretLogSevere("Failed to get ZAttrs (is NULL) from OmeFileReader");
                return NULL;
            }
            if (zattrs->getNumberOfDataSets() > 0) {
                const int32_t dataSetIndex(tabOverlayInfo->m_pyramidLevel);
                if ((dataSetIndex >= 0)
                    && (dataSetIndex < zattrs->getNumberOfDataSets())) {
                    const OmeDataSet* dataSet(zattrs->getDataSet(dataSetIndex));
                    GraphicsPrimitiveV3fT2f* primitive(createGraphicsPrimitive(dataSet,
                                                                               sliceIndex));
                    tabOverlayInfo->m_graphicsPrimitive.reset(primitive);
                }
                else {
                    CaretLogSevere("Invalid data set index="
                                   + AString::number(dataSetIndex)
                                   + " for "
                                   + getFileName());
                }
            }
        }
    }
    return tabOverlayInfo->m_graphicsPrimitive.get();
#else
    return NULL;
#endif
}

/*
 * @return Primitive for drawing media with coordinates
 * @param tabIndex
 *    Index of tab where image is drawn
 * @param overlayIndex
 *    Index of overlay
 */
GraphicsPrimitiveV3fT2f*
OmeZarrImageFile::getGraphicsPrimitiveForPlaneXyzDrawing(const int32_t tabIndex,
                                                     const int32_t overlayIndex) const
{
    return getGraphicsPrimitiveForMediaDrawing(tabIndex,
                                               overlayIndex);
}

/**
 * @return Modelview scaling for display of image
 */
Vector3D
OmeZarrImageFile::getModelviewScaling(const int32_t tabIndex,
                                      const int32_t overlayIndex) const
{
    Vector3D scaleOut(1.0, 1.0, 1.0);
    
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    const TabOverlayInfo* tabOverlayInfo(m_tabOverlayInfo[tabIndex][overlayIndex].get());
    CaretAssert(tabOverlayInfo);
    const int64_t pyramidLevel(tabOverlayInfo->m_pyramidLevel);
    if ((pyramidLevel >= 0)
        && (pyramidLevel < static_cast<int64_t>(m_pyramidLevels.size()))) {
        if (m_omeFileReader) {
            const OmeAttrsV0p4JsonFile* zattrs(m_omeFileReader->getZAttrs());
            if (zattrs == NULL) {
                CaretLogSevere("Failed to get ZAttrs (is NULL) from OmeFileReader");
                return scaleOut;
            }
            if (zattrs->getNumberOfDataSets() > 0) {
                const int32_t dataSetIndex(tabOverlayInfo->m_pyramidLevel);
                if ((dataSetIndex >= 0)
                    && (dataSetIndex < zattrs->getNumberOfDataSets())) {
                    const OmeDataSet* dataSet(zattrs->getDataSet(dataSetIndex));
                    /*
                     * If pixel coordinates are not square, need to
                     * scale the coordinates.  This results from
                     * scaling in coordinateTransformations from
                     * the .zattrs file.
                     */
                    Vector3D xyzPixel00(dataSet->getPixelCoordinate(0, 0, 0));
                    Vector3D xyzPixel11(dataSet->getPixelCoordinate(1, 1, 0));
                    const float dy(xyzPixel11[1] - xyzPixel00[1]);
                    const float dx(xyzPixel11[0] - xyzPixel00[0]);
                    if ((dx > 0.0) && (dy > 0.0)) {
                        scaleOut[1] = (dy / dx);
                    }
                }
            }
        }
    }
#endif

    return scaleOut;
}

/**
 * Create a graphics primitive with data from the OME data set
 * @param omeDataSet
 *    The OME Data Set
 * @param sliceIndex
 *    Index of the slice for display
 * @return
 *    Pointer to graphics primitive or NULL if failure.
 */
GraphicsPrimitiveV3fT2f*
OmeZarrImageFile::createGraphicsPrimitive(const OmeDataSet* dataSet,
                                          const int64_t sliceIndex) const
{
    GraphicsPrimitiveV3fT2f* primitive(NULL);
    
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    CaretAssert(dataSet);
    if (dataSet->getNumberOfSlices() > 0) {
        const int64_t width(dataSet->getWidth());
        const int64_t height(dataSet->getHeight());
        const int64_t numPixels(width * height * 4);
        if (numPixels > 0) {
            FunctionResultValue<OmeImage*> resultImage(dataSet->readSlice(sliceIndex));
            if (resultImage.isOk()) {
                std::unique_ptr<OmeImage> omeImage(resultImage.getValue());
                CaretAssert(omeImage);
                const OmeDimensionSizes dimSizes(omeImage->getDimensionSizes());
                const int64_t numX(dimSizes.getSizeX());
                const int64_t numY(dimSizes.getSizeY());
                const int64_t numZ(dimSizes.getSizeZ());
                
                /*
                 * If image is too big for OpenGL texture limits, scale image to acceptable size
                 */
                const int32_t maxTextureWidthHeight = GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension();
                if (maxTextureWidthHeight > 0) {
                    const int32_t excessWidth(numX - maxTextureWidthHeight);
                    const int32_t excessHeight(numY - maxTextureWidthHeight);
                    if ((excessWidth > 0)
                        || (excessHeight > 0)) {
                        if (excessWidth > excessHeight) {
                            CaretLogWarning(getFileName()
                                            + " is too big for texture.  Maximum width/height is: "
                                            + AString::number(maxTextureWidthHeight)
                                            + " Image Width: "
                                            + AString::number(numX)
                                            + " Image Height: "
                                            + AString::number(numY));
                        }
                    }
                }
                
                FunctionResultValue<uint8_t*> textureResult(omeImage->getDataForOpenGLTexture());
                if (textureResult.isError()) {
                    CaretLogSevere("Failure to get texture for OpenGL drawing of OmeZarr");
                    return NULL;
                }
                const uint8_t* textureRGBA(textureResult.getValue());
                
                const std::array<float, 4> textureBorderColorRGBA { 0.0, 0.0, 0.0, 0.0 };
                
                GraphicsTextureSettings::PixelFormatType pixelFormat(GraphicsTextureSettings::PixelFormatType::BGRA);
                pixelFormat = GraphicsTextureSettings::PixelFormatType::RGBA;
                
                /*
                 * Filtering for matching image pixel to screen pixel
                 */
                GraphicsTextureMagnificationFilterEnum::Enum magFilter(GraphicsTextureMagnificationFilterEnum::NEAREST);
                GraphicsTextureMinificationFilterEnum::Enum minFilter(GraphicsTextureMinificationFilterEnum::NEAREST);
                const bool smoothFlag(false);
                if (smoothFlag) {
                    /*
                     * This will break opacity on drawing volume slices over
                     * histology slices that results in white edges appearing
                     * when opacity is less than one.
                     */
                    magFilter = GraphicsTextureMagnificationFilterEnum::LINEAR;
                    minFilter = GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR;
                }
                /*
                 * Compress texture if image is large and compression is enabled
                 */
                const bool allowTextureCompressionFlag(false);
                const GraphicsTextureSettings::CompressionType textureCompressionType(allowTextureCompressionFlag
                                                                                      ? GraphicsTextureSettings::CompressionType::ENABLED
                                                                                      : GraphicsTextureSettings::CompressionType::DISABLED);
                GraphicsTextureSettings textureSettings(textureRGBA,
                                                        numX,
                                                        numY,
                                                        numZ,
                                                        GraphicsTextureSettings::DimensionType::FLOAT_STR_2D,
                                                        pixelFormat,
                                                        GraphicsTextureSettings::PixelOrigin::TOP_LEFT,
                                                        GraphicsTextureSettings::WrappingType::CLAMP,
                                                        GraphicsTextureSettings::MipMappingType::ENABLED,
                                                        textureCompressionType,
                                                        magFilter,
                                                        minFilter,
                                                        textureBorderColorRGBA);
                primitive = GraphicsPrimitive::newPrimitiveV3fT2f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                                           textureSettings);
                
                bool doXyzFlag(false);
                if (doXyzFlag) {
                    const float minTextureST(0.0);
                    const float maxTextureST(1.0);
                    
                    Vector3D xyzTopLeft(dataSet->getPixelCoordinate(0, 0, 0));
                    Vector3D xyzTopRight(dataSet->getPixelCoordinate(width - 1, 0, 0));
                    Vector3D xyzBottomLeft(dataSet->getPixelCoordinate(0, height - 1, 0));
                    Vector3D xyzBottomRight(dataSet->getPixelCoordinate(width - 1, height - 1, 0));
                    primitive->addVertex(xyzTopLeft[0],     xyzTopLeft[1],     minTextureST, minTextureST);  /* Top Left */
                    primitive->addVertex(xyzBottomLeft[0],  xyzBottomLeft[1],  minTextureST, maxTextureST);  /* Bottom Left */
                    primitive->addVertex(xyzTopRight[0],    xyzTopRight[1],    maxTextureST, minTextureST);  /* Top Right */
                    primitive->addVertex(xyzBottomRight[0], xyzBottomRight[1], maxTextureST, maxTextureST);  /* Bottom Right */

                }
                else {
                    /*
                     * Create a primitive for PIXEL coordinates
                     *
                     * Coordinates at EDGE of the pixels
                     */
                    const float minX = 0;
                    const float maxX = numX;
                    const float minY = 0;
                    const float maxY = numY;
                    
                    /*
                     * A Triangle Strip (consisting of two triangles) is used
                     * for drawing the image.
                     * The order of the vertices in the triangle strip is
                     * Top Left, Bottom Left, Top Right, Bottom Right.
                     * ORIGIN IS AT TOP LEFT
                     */
                    const float minTextureST(0.0);
                    const float maxTextureST(1.0);
                    primitive->addVertex(minX, minY, minTextureST, minTextureST);  /* Top Left */
                    primitive->addVertex(minX, maxY, minTextureST, maxTextureST);  /* Bottom Left */
                    primitive->addVertex(maxX, minY, maxTextureST, minTextureST);  /* Top Right */
                    primitive->addVertex(maxX, maxY, maxTextureST, maxTextureST);  /* Bottom Right */
                }
            }
            else {
                CaretLogSevere("Reading slices from ZARR file: "
                               + resultImage.getErrorMessage());
            }
        }
    }
#endif

    return primitive;
}


/**
 * Create a graphics primitive with data from the OME image
 * @param omeImage
 *    The OME image
 * @return
 *    Pointer to graphics primitive or NULL if failure.
 */
GraphicsPrimitiveV3fT2f*
OmeZarrImageFile::createGraphicsPrimitive(const OmeImage* omeImage) const
{
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    CaretAssert(omeImage);
    const OmeDimensionSizes dimSizes(omeImage->getDimensionSizes());
    const int64_t numX(dimSizes.getSizeX());
    const int64_t numY(dimSizes.getSizeY());
    const int64_t numZ(dimSizes.getSizeZ());

    /*
     * If image is too big for OpenGL texture limits, scale image to acceptable size
     */
    const int32_t maxTextureWidthHeight = GraphicsUtilitiesOpenGL::getTextureWidthHeightMaximumDimension();
    if (maxTextureWidthHeight > 0) {
        const int32_t excessWidth(numX - maxTextureWidthHeight);
        const int32_t excessHeight(numY - maxTextureWidthHeight);
        if ((excessWidth > 0)
            || (excessHeight > 0)) {
            if (excessWidth > excessHeight) {
                CaretLogWarning(getFileName()
                                + " is too big for texture.  Maximum width/height is: "
                                + AString::number(maxTextureWidthHeight)
                                + " Image Width: "
                                + AString::number(numX)
                                + " Image Height: "
                                + AString::number(numY));
            }
        }
    }
    
    FunctionResultValue<uint8_t*> textureResult(omeImage->getDataForOpenGLTexture());
    if (textureResult.isError()) {
        CaretLogSevere("Failure to get texture for OpenGL drawing of OmeZarr");
        return NULL;
    }
    const uint8_t* textureRGBA(textureResult.getValue());
    
    const std::array<float, 4> textureBorderColorRGBA { 0.0, 0.0, 0.0, 0.0 };
    
    GraphicsTextureSettings::PixelFormatType pixelFormat(GraphicsTextureSettings::PixelFormatType::BGRA);
    pixelFormat = GraphicsTextureSettings::PixelFormatType::RGBA;
    
    /*
     * Filtering for matching image pixel to screen pixel
     */
    GraphicsTextureMagnificationFilterEnum::Enum magFilter(GraphicsTextureMagnificationFilterEnum::NEAREST);
    GraphicsTextureMinificationFilterEnum::Enum minFilter(GraphicsTextureMinificationFilterEnum::NEAREST);
    const bool smoothFlag(false);
    if (smoothFlag) {
        /*
         * This will break opacity on drawing volume slices over
         * histology slices that results in white edges appearing
         * when opacity is less than one.
         */
        magFilter = GraphicsTextureMagnificationFilterEnum::LINEAR;
        minFilter = GraphicsTextureMinificationFilterEnum::LINEAR_MIPMAP_LINEAR;
    }
    /*
     * Compress texture if image is large and compression is enabled
     */
    const bool allowTextureCompressionFlag(false);
    const GraphicsTextureSettings::CompressionType textureCompressionType(allowTextureCompressionFlag
                                                                          ? GraphicsTextureSettings::CompressionType::ENABLED
                                                                          : GraphicsTextureSettings::CompressionType::DISABLED);
    GraphicsTextureSettings textureSettings(textureRGBA,
                                            numX,
                                            numY,
                                            numZ,
                                            GraphicsTextureSettings::DimensionType::FLOAT_STR_2D,
                                            pixelFormat,
                                            GraphicsTextureSettings::PixelOrigin::TOP_LEFT,
                                            GraphicsTextureSettings::WrappingType::CLAMP,
                                            GraphicsTextureSettings::MipMappingType::ENABLED,
                                            textureCompressionType,
                                            magFilter,
                                            minFilter,
                                            textureBorderColorRGBA);
    GraphicsPrimitiveV3fT2f* primitive = GraphicsPrimitive::newPrimitiveV3fT2f(GraphicsPrimitive::PrimitiveType::OPENGL_TRIANGLE_STRIP,
                                                                               textureSettings);
    
    /*
     * Create a primitive for PIXEL coordinates
     *
     * Coordinates at EDGE of the pixels
     */
    const float minX = 0;
    const float maxX = numX;
    const float minY = 0;
    const float maxY = numY;
    
    /*
     * A Triangle Strip (consisting of two triangles) is used
     * for drawing the image.
     * The order of the vertices in the triangle strip is
     * Top Left, Bottom Left, Top Right, Bottom Right.
     * ORIGIN IS AT TOP LEFT
     */
    const float minTextureST(0.0);
    const float maxTextureST(1.0);
    primitive->addVertex(minX, minY, minTextureST, minTextureST);  /* Top Left */
    primitive->addVertex(minX, maxY, minTextureST, maxTextureST);  /* Bottom Left */
    primitive->addVertex(maxX, minY, maxTextureST, minTextureST);  /* Top Right */
    primitive->addVertex(maxX, maxY, maxTextureST, maxTextureST);  /* Bottom Right */
        
    return primitive;
#else
    return NULL;
#endif
}

/**
 * @return True if the given pixel index is valid for the CZI image file (may be outside of currently loaded sub-image)
 * @param frameIndex
 *    Index of frame
 * @param pixelIndexOriginAtTopLeft
 *    Image of pixel with origin (0, 0) at the top left
 */
bool
OmeZarrImageFile::isPixelIndexInFrameValid(const int32_t frameIndex,
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
OmeZarrImageFile::isPixelIndexInFrameValid(const int32_t frameIndex,
                                           const PixelLogicalIndex& pixelLogicalIndex) const
{
    CaretAssertVectorIndex(m_pyramidLevels, frameIndex);
    const QRectF& frameLogicalRect(m_pyramidLevels[frameIndex].m_logicalRectangle);
    const float i(pixelLogicalIndex.getI());
    const float j(pixelLogicalIndex.getJ());
    
    return frameLogicalRect.contains(i, j);
}

/**
 * @return True if the given pixel index is valid for the CZI image file (may be outside of currently loaded sub-image)
 * @param pixelIndexOriginAtTopLeft
 *    Image of pixel with origin (0, 0) at the top left
 */
bool
OmeZarrImageFile::isPixelIndexValid(const PixelIndex& pixelIndexOriginAtTopLeft) const
{
    return isPixelIndexValid(pixelIndexToPixelLogicalIndex(pixelIndexOriginAtTopLeft));
}

/**
 * @return True if the given pixel index is valid
 * @param pixelLogicalIndex
 *    Pixel logical index
 */
bool
OmeZarrImageFile::isPixelIndexValid(const PixelLogicalIndex& pixelLogicalIndex) const
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
OmeZarrImageFile::getPixelRGBA(const int32_t tabIndex,
                           const int32_t overlayIndex,
                           const PixelLogicalIndex& pixelLogicalIndex,
                           uint8_t pixelRGBAOut[4]) const
{
    pixelRGBAOut[0] = 0;
    pixelRGBAOut[1] = 0;
    pixelRGBAOut[2] = 0;
    pixelRGBAOut[3] = 0;
    
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    const TabOverlayInfo* tabOverlayInfo(m_tabOverlayInfo[tabIndex][overlayIndex].get());
    CaretAssert(tabOverlayInfo);
    if (tabOverlayInfo->m_graphicsPrimitive) {
        const int32_t pyramidLevel(tabOverlayInfo->m_pyramidLevel);
        if ((pyramidLevel >= 0)
            && (pyramidLevel < static_cast<int32_t>(m_pyramidLevels.size()))) {
            const int64_t pixelI(pixelLogicalIndex.getI());
            const int64_t pixelJ(pixelLogicalIndex.getJ());
            const OmeDataSet* dataSet(m_omeFileReader->getZAttrs()->getDataSet(pyramidLevel));
            CaretAssert(dataSet);
            FunctionResultValue<std::array<uint8_t, 4>> result(dataSet->readSlicePixel(tabOverlayInfo->m_sliceIndex, pixelI, pixelJ));
            if (result.isOk()) {
                const std::array<uint8_t, 4>& rgba(result.getValue());
                pixelRGBAOut[0] = rgba[0];
                pixelRGBAOut[1] = rgba[1];
                pixelRGBAOut[2] = rgba[2];
                pixelRGBAOut[3] = rgba[3];
                return true;
            }
        }
    }
#endif
    
    return false;
}

/**
 * @return The dimension (width/height) for loading image data from preferences
 */
int32_t
OmeZarrImageFile::getPreferencesImageDimension() const
{
    CaretAssert(m_maximumImageDimension >= 256);
    return m_maximumImageDimension;
}

/**
 * @return The RGB background color for images from preferences BYTE values
 */
std::array<uint8_t, 3>
OmeZarrImageFile::getPreferencesImageBackgroundByteRGB() const
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
OmeZarrImageFile::getPreferencesImageBackgroundFloatRGB() const
{
    std::array<uint8_t, 3> rgbByte = getPreferencesImageBackgroundByteRGB();
    std::array<float, 3> rgb = BackgroundAndForegroundColors::toFloatRGB(rgbByte.data());

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
OmeZarrImageFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
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
OmeZarrImageFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                       const SceneClass* sceneClass)
{
    MediaFile::restoreFileDataFromScene(sceneAttributes,
                                        sceneClass);
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * Export all slices for the given pyramid level to a volume file
 * @param pyramidLevel
 *    The pyramid level
 * @return Result of exporting to volume file
 */
FunctionResultValue<VolumeFile*>
OmeZarrImageFile::exportToVolumeFile(const int32_t pyramidLevel) const
{
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    AString errorMessage;
    const OmeAttrsV0p4JsonFile* zattrs(m_omeFileReader->getZAttrs());
    if (zattrs == NULL) {
        errorMessage = "Failed to get ZAttrs (is NULL) from OmeFileReader";
    }
    else {
        const int32_t numLevels(zattrs->getNumberOfDataSets());
        if (numLevels <= 0) {
            errorMessage = "No images were read from OME ZARR file";
        }
        else {
            if ((pyramidLevel < 0)
                || pyramidLevel >= zattrs->getNumberOfDataSets()) {
                errorMessage = "Pyramid level is invalid";
            }
        }
    }
    if ( ! errorMessage.isEmpty()) {
        return FunctionResultValue<VolumeFile*>(NULL, errorMessage, false);
    }
    
    const OmeDataSet* dataSet(zattrs->getDataSet(pyramidLevel));
    CaretAssert(dataSet);
    
    const int64_t dimI(dataSet->getWidth());
    const int64_t dimJ(dataSet->getHeight());
    const int64_t dimK(dataSet->getNumberOfSlices());
    const int64_t voxelCount(dimI * dimJ * dimK);
    if (voxelCount <= 0) {
        return FunctionResultValue<VolumeFile*>(NULL, "No pixels in OME-ZARR image.", false);
    }
    const int64_t numChannels(dataSet->getNumberOfChannels());
    if ((numChannels != 3)
        && (numChannels != 4)) {
        return FunctionResultValue<VolumeFile*>(NULL, "Number of channels in OME-ZARR file must be 3 or 4.", false);
    }
    
    std::vector<int64_t> dimensions;
    dimensions.push_back(dimI);
    dimensions.push_back(dimJ);
    dimensions.push_back(dimK);
    dimensions.push_back(1); /* time points */
    dimensions.push_back(4); /**/
    const int64_t numComponents(4);  /* RGBA */

    std::vector<float> row1 { 1.0, 0.0, 0.0, 0.0 };
    std::vector<float> row2 { 0.0, 1.0, 0.0, 0.0 };
    std::vector<float> row3 { 0.0, 0.0, 1.0, 0.0 };
    std::vector<std::vector<float>> indexToSpace;
    indexToSpace.push_back(row1);
    indexToSpace.push_back(row2);
    indexToSpace.push_back(row3);

    std::unique_ptr<VolumeFile> volumeFilePtr(new VolumeFile(dimensions,
                                                             indexToSpace,
                                                             numComponents,
                                                             SubvolumeAttributes::RGB));
    
    for (int32_t iSlice = 0; iSlice < dimK; iSlice++) {
        FunctionResultValue<OmeImage*> imageResult(dataSet->readSlice(iSlice));
        if (imageResult.isError()) {
            return FunctionResultValue<VolumeFile*>(NULL,
                                                    ("Reading slice "
                                                     + AString::number(iSlice)
                                                     + ": "
                                                     + imageResult.getErrorMessage()),
                                                    false);
        }
        
        std::unique_ptr<const OmeImage> image(imageResult.getValue());
        CaretAssert(image);
        
        const bool flipJ(true);
        const int64_t mapIndex(0);
        for (int64_t i = 0; i < dimI; i++) {
            for (int64_t j = 0; j < dimJ; j++) {
                for (int64_t ch = 0; ch < 4; ch++) {
                    const int64_t jIndex(flipJ
                                         ? (dimJ - 1 - j)
                                         : j);
                    const float value(static_cast<float>(image->getElement(i, j, 0, 0, ch)) / 255.0);
                    CaretAssert(volumeFilePtr->indexValid(i, jIndex, iSlice, mapIndex, ch));
                    volumeFilePtr->setValue(value, i, jIndex, iSlice, mapIndex, ch);
                }
            }
        }
    }
    
    volumeFilePtr->setFileName(getFileName()
                               + "."
                               + DataFileTypeEnum::toFileExtension(DataFileTypeEnum::VOLUME));
    return FunctionResultValue<VolumeFile*>(volumeFilePtr.release(),
                                            "",
                                            true);
#else
    return FunctionResultValue<VolumeFile*>(NULL,
                                            "Workbench was compiled without Z5 support.",
                                            true);
#endif
}


/**
 * Export a full resolution image to an image file with the maximum width/height
 * @param imageFileName
 *    Name for file
 * @param maximumWidthHeight
 *    Width and height will be no greater than this value (aspect is preserved)
 *     Negative is no limit on size
 * @param errorMessageOut
 *    Contains info if writing image fails
 * @return True if successful, else false.
 */
bool
OmeZarrImageFile::exportToImageFile(const QString& imageFileName,
                                    const int32_t maximumWidthHeight,
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
    
    return errorMessageOut.isEmpty();
}

/**
 * @return String describing content
 */
AString
OmeZarrImageFile::toString() const
{
    AString s(MediaFile::toString());
    
#if defined(WORKBENCH_HAVE_OME_ZARR_Z5)
    if (m_omeFileReader) {
        s.append(m_omeFileReader->toString());
    }
#endif
    
    return s;
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
OmeZarrImageFile::TabOverlayInfo::TabOverlayInfo(OmeZarrImageFile* omeZarrImageFile,
                                             const int32_t tabIndex,
                                             const int32_t overlayIndex)
:
m_omeZarrImageFile(omeZarrImageFile),
m_tabIndex(tabIndex),
m_overlayIndex(overlayIndex),
m_sliceIndex(0),
m_frameIndex(0),
m_pyramidLevel(0),
m_resolutionChangeMode(CziImageResolutionChangeModeEnum::AUTO2)
{
    
}

/**
 * @return The selected image loader
 */
OmeZarrImageFile::TabOverlayInfo::~TabOverlayInfo()
{
}

/**
 * Clone from the given tag overlay info
 * @param otherTabOverlayInfo
 *    Tab overlay info that is cloned
 */
void
OmeZarrImageFile::TabOverlayInfo::cloneFromOtherTabOverlayInfo(TabOverlayInfo* otherTabOverlayInfo)
{
    CaretAssert(otherTabOverlayInfo);
    m_sliceIndex = otherTabOverlayInfo->m_sliceIndex;
    m_frameIndex = otherTabOverlayInfo->m_frameIndex;
    m_pyramidLevel = otherTabOverlayInfo->m_pyramidLevel;
    m_resolutionChangeMode = otherTabOverlayInfo->m_resolutionChangeMode;
    m_graphicsPrimitive.reset();
}

/**
 * Reset content such as when file is closed
 */
void
OmeZarrImageFile::TabOverlayInfo::resetContent()
{
    m_sliceIndex = 0;
    m_frameIndex = 0;
    m_pyramidLevel = 0;
    m_resolutionChangeMode = CziImageResolutionChangeModeEnum::AUTO2;
    m_graphicsPrimitive.reset();
}

