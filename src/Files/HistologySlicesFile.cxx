
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __HISTOLOGY_SLICES_FILE_DECLARE__
#include "HistologySlicesFile.h"
#undef __HISTOLOGY_SLICES_FILE_DECLARE__

#include <QFile>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "DataFileContentInformation.h"
#include "ElapsedTimer.h"
#include "EventCaretDataFilesGet.h"
#include "EventManager.h"
#include "FileInformation.h"
#include "GiftiMetaData.h"
#include "HistologyCoordinate.h"
#include "HistologySlice.h"
#include "HistologySlicesFile.h"
#include "HistologySliceImage.h"
#include "HistologySlicesFileXmlStreamReader.h"
#include "MediaFile.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::HistologySlicesFile
 * \brief Contains a sequence of histology slices
 * \ingroup Files
 */

/**
 * Constructor.
 */
HistologySlicesFile::HistologySlicesFile()
: CaretDataFile(DataFileTypeEnum::HISTOLOGY_SLICES)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_metaData.reset(new GiftiMetaData());
    resetAfterSlicesChanged();
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
HistologySlicesFile::~HistologySlicesFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
HistologySlicesFile::HistologySlicesFile(const HistologySlicesFile& obj)
: CaretDataFile(obj),
EventListenerInterface()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_metaData.reset(new GiftiMetaData());

    this->copyHelperHistologySlicesFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
HistologySlicesFile&
HistologySlicesFile::operator=(const HistologySlicesFile& obj)
{
    if (this != &obj) {
        CaretDataFile::operator=(obj);
        this->copyHelperHistologySlicesFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
HistologySlicesFile::copyHelperHistologySlicesFile(const HistologySlicesFile& obj)
{
    *m_metaData = *obj.m_metaData;
    for (const auto& slice : m_histologySlices) {
        CaretAssert(slice);
        std::unique_ptr<HistologySlice> sliceCopy(new HistologySlice(*slice.get()));
        m_histologySlices.push_back(std::move(sliceCopy));
    }
    resetAfterSlicesChanged();
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
HistologySlicesFile::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * @return File casted to an histology slices file (avoids use of dynamic_cast that can be slow)
 * Overidden in MediaFile
 */
HistologySlicesFile*
HistologySlicesFile::castToHistologySlicesFile()
{
    return this;
}

/**
 * @return File casted to an histology slices file (avoids use of dynamic_cast that can be slow)
 * Overidden in ImageFile
 */
const HistologySlicesFile*
HistologySlicesFile::castToHistologySlicesFile() const
{
    return this;
}


/**
 * @return True if the file is empty
 */
bool
HistologySlicesFile::isEmpty() const
{
    return false;
}

/**
 * Clear the content of the file
 */
void
HistologySlicesFile::clear()
{
    CaretDataFile::clear();
    m_metaData->clear();
    m_histologySlices.clear();
    resetAfterSlicesChanged();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
HistologySlicesFile::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
HistologySlicesFile::setStructure(const StructureEnum::Enum /*structure*/)
{
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
HistologySlicesFile::getFileMetaData()
{
    return m_metaData.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
HistologySlicesFile::getFileMetaData() const
{
    return m_metaData.get();
}

/**
 * Add a histology slice
 * @param histologySlice
 *    Pointer to the slice.  Will take ownership of this instance.
 */
void
HistologySlicesFile::addHistologySlice(HistologySlice* histologySlice)
{
    CaretAssert(histologySlice);
    m_histologySlices.emplace_back(histologySlice);
    resetAfterSlicesChanged();
}

/**
 * Reset items that are dependent upon the slices
 */
void
HistologySlicesFile::resetAfterSlicesChanged()
{
    m_stereotaxicXyzBoundingBoxValidFlag = false;
    m_planeXyzBoundingBoxValidFlag       = false;
    m_sliceSpacingValid                  = false;
}

/**
 * @return Number of histology slices in the meta file
 */
int32_t
HistologySlicesFile::getNumberOfHistologySlices() const
{
    return m_histologySlices.size();
}

/**
 * @return Pointer to slice at given index or NULL if index is invalid
 * @param sliceIndex
 *    Index of slice
 */
HistologySlice*
HistologySlicesFile::getHistologySliceByIndex(const int32_t sliceIndex)
{
    CaretAssertVectorIndex(m_histologySlices, sliceIndex);
    if ((sliceIndex >= 0)
        && (sliceIndex < static_cast<int32_t>(m_histologySlices.size()))) {
        return m_histologySlices[sliceIndex].get();
    }
    return NULL;
}


/**
 * @return Pointer to slice at given index or NULL if index is invalid (const method)
 * @param sliceIndex
 *    Index of slice
 */
const HistologySlice*
HistologySlicesFile::getHistologySliceByIndex(const int32_t sliceIndex) const
{
    CaretAssertVectorIndex(m_histologySlices, sliceIndex);
    if ((sliceIndex >= 0)
        && (sliceIndex < static_cast<int32_t>(m_histologySlices.size()))) {
        return m_histologySlices[sliceIndex].get();
    }
    return NULL;
}

/**
 * @return Slice with the given slice number
 * @param sliceName
 *     Number of the slice
 */
const HistologySlice*
HistologySlicesFile::getHistologySliceByNumber(const AString& sliceName) const
{
    for (auto& ptr : m_histologySlices) {
        if (ptr->getSliceName() == sliceName) {
            return ptr.get();
        }
    }
    return NULL;
}

/**
 * @return Slice number for slice at the given index
 * @param sliceIndex
 *    Index of the slice
 */
AString
HistologySlicesFile::getSliceNameBySliceIndex(const int32_t sliceIndex) const
{
    CaretAssertVectorIndex(m_histologySlices, sliceIndex);
    if ((sliceIndex >= 0)
        && (sliceIndex < static_cast<int32_t>(m_histologySlices.size()))) {
        CaretAssertVectorIndex(m_histologySlices, sliceIndex);
        return m_histologySlices[sliceIndex]->getSliceName();
    }
    return "";
}

/**
 * @return Slice index for slice with slice number, negative if slice number is invalid
 * @param sliceName
 *    Number of the slice
 */
int32_t
HistologySlicesFile::getSliceIndexFromSliceName(const AString& sliceName) const
{
    const int32_t numSlices(getNumberOfHistologySlices());
    for (int32_t sliceIndex = 0; sliceIndex < numSlices; sliceIndex++) {
        CaretAssertVectorIndex(m_histologySlices, sliceIndex);
        if (m_histologySlices[sliceIndex]->getSliceName() == sliceName) {
            return sliceIndex;
        }
    }
    return -1;
}

/**
 * @return Stereotaxic bounding box for all slices
 */
BoundingBox
HistologySlicesFile::getStereotaxicXyzBoundingBox() const
{
    if ( ! m_stereotaxicXyzBoundingBoxValidFlag) {
        m_stereotaxicXyzBoundingBox.resetForUpdate();
        
        for (auto& slice : m_histologySlices) {
            BoundingBox bb(slice->getStereotaxicXyzBoundingBox());
            m_stereotaxicXyzBoundingBox.unionOperation(bb);
        }
        m_stereotaxicXyzBoundingBoxValidFlag = true;
    }
    
    return m_stereotaxicXyzBoundingBox;
}

/**
 * @return Plane bounding box for all slices
 */
BoundingBox
HistologySlicesFile::getPlaneXyzBoundingBox() const
{
    if ( ! m_planeXyzBoundingBoxValidFlag) {
        m_planeXyzBoundingBox.resetForUpdate();
        
        for (auto& slice : m_histologySlices) {
            BoundingBox bb(slice->getPlaneXyzBoundingBox());
            m_planeXyzBoundingBox.unionOperation(bb);
        }
        m_planeXyzBoundingBoxValidFlag = true;
    }
    
    return m_planeXyzBoundingBox;
}

/**
 * @return The slice nearest the stereotaxic coordinate
 * @param stereotaxicXYZ
 *    The coordinate
 * @param mmDistanceToSlice
 *    Distance in millimeters for the coordinate from the output slice
 * @param nearestOnSliceStereotaxicXYZ
 *    Coordinate on slice nearest coordinate
 */
const HistologySlice*
HistologySlicesFile::getSliceNearestStereotaxicXyz(const Vector3D& stereotaxicXYZ,
                                                   float& mmDistanceToSlice,
                                                   Vector3D& nearestOnSliceStereotaxicXYZ) const
{
    const HistologySlice* nearestSlice(NULL);
    mmDistanceToSlice = std::numeric_limits<float>::max();
    nearestOnSliceStereotaxicXYZ = Vector3D();
    
    const int32_t numSlices(getNumberOfHistologySlices());
    for (int32_t iSlice = 0; iSlice < numSlices; iSlice++) {
        const HistologySlice* slice(getHistologySliceByIndex(iSlice));
        CaretAssert(slice);
        const Plane& plane(slice->getStereotaxicPlane());
        if (plane.isValidPlane()) {
            const float dist(plane.absoluteDistanceToPlane(stereotaxicXYZ));
            if (dist < mmDistanceToSlice) {
                nearestSlice = slice;
                mmDistanceToSlice = dist;
                plane.projectPointToPlane(stereotaxicXYZ,
                                          nearestOnSliceStereotaxicXYZ);
            }
        }
    }
    
    return nearestSlice;
}

/**
 * Get the identification text for the given histology coordinate.
 * @param tabIndex
 *    Index of the tab in which identification took place
 * @param histologyCoordinate
 *    The histology coordinate
 * @param columnOneTextOut
 *    Text for column one that is displayed to user.
 * @param columnTwoTextOut
 *    Text for column two that is displayed to user.
 * @param toolTipTextOut
 *    Text for tooltip
 */
void
HistologySlicesFile::getIdentificationText(const int32_t tabIndex,
                                           const HistologyCoordinate& histologyCoordinate,
                                           std::vector<AString>& columnOneTextOut,
                                           std::vector<AString>& columnTwoTextOut,
                                           std::vector<AString>& toolTipTextOut) const
{
    std::vector<AString> columnOneText, columnTwoText, toolTipText;
    
    columnOneText.push_back("Histology File");
    columnTwoText.push_back(getFileNameNoPath());
    columnOneText.push_back("Slice Index / Number");
    columnTwoText.push_back(AString::number(histologyCoordinate.getSliceIndex())
                            + " / "
                            + getSliceNameBySliceIndex(histologyCoordinate.getSliceIndex()));
    
    AString mmText;
    AString planeText;
    if (histologyCoordinate.isStereotaxicXYZValid()) {
        mmText = ("Stereotaxic XYZ ("
                  + AString::fromNumbers(histologyCoordinate.getStereotaxicXYZ())
                  + ")");
    }
    if (histologyCoordinate.isPlaneXYValid()) {
        planeText = ("Plane XYZ ("
                     + AString::fromNumbers(histologyCoordinate.getPlaneXYZ())
                     + ")");
    }
    if ( (! mmText.isEmpty())
        || ( ! mmText.isEmpty())) {
        columnOneText.push_back(mmText);
        columnTwoText.push_back(planeText);
        toolTipText.push_back(mmText);
        toolTipText.push_back(planeText);
    }
    
    if (histologyCoordinate.isStereotaxicNoNonLinearXYZValid()) {
        columnOneText.push_back("Stereotaxic (linear only) XYZ ("
                                + AString::fromNumbers(histologyCoordinate.getStereotaxicNoNonLinearXYZ())
                                + ")");
        columnTwoText.push_back("");
        
        if (histologyCoordinate.isStereotaxicXYZValid()) {
            const AString histologyFileName(histologyCoordinate.getHistologySlicesFileName());
            if ( ! histologyFileName.isEmpty()) {
                CaretDataFile* caretDataFile(EventCaretDataFilesGet::getCaretDataFileWithName(histologyFileName));
                if (caretDataFile != NULL) {
                    HistologySlicesFile* histologySlicesFile(caretDataFile->castToHistologySlicesFile());
                    if (histologySlicesFile != NULL) {
                        const HistologySlice* slice(histologySlicesFile->getHistologySliceByIndex(histologyCoordinate.getSliceIndex()));
                        if (slice != NULL) {
                            Vector3D planeXYZ;
                            Vector3D planeNoNonLinearXYZ;
                            if (slice->stereotaxicXyzToPlaneXyz(histologyCoordinate.getStereotaxicXYZ(),
                                                                planeNoNonLinearXYZ,
                                                                planeXYZ)) {
                                columnOneText.push_back("Debug Back to Plane XYZ: "
                                                        + AString::fromNumbers(planeXYZ));
                                columnTwoText.push_back("Debug Back to Plane (no non-linear) XYZ: "
                                                        + AString::fromNumbers(planeNoNonLinearXYZ));
                            }
                        }
                    }
                }
            }
        }
    }
        
    HistologySlice* histSlice(findHistologySliceForHistologyCoordinate(histologyCoordinate));
    if (histSlice != NULL) {
        histSlice->getIdentificationText(tabIndex,
                                         histologyCoordinate,
                                         columnOneText,
                                         columnTwoText,
                                         toolTipText);
    }
    
    const AString mediaFileName(histologyCoordinate.getHistologyMediaFileName());
    if ( ! mediaFileName.isEmpty()) {
        columnOneText.push_back("Image "
                                + AString::number(histSlice->getIndexOfMediaFileWithName(mediaFileName) + 1)
                                + " of "
                                + AString::number(histSlice->getNumberOfHistologySliceImages())
                                + " in slice");
        columnTwoText.push_back("");
    }
    
    const int32_t numColOne(columnOneText.size());
    const int32_t numColTwo(columnTwoText.size());
    const int32_t maxNum(std::max(numColOne, numColTwo));
    for (int32_t i = 0; i < maxNum; i++) {
        AString colOne;
        AString colTwo;
        if (i < numColOne) {
            CaretAssertVectorIndex(columnOneText, i);
            colOne = columnOneText[i];
        }
        if (i < numColTwo) {
            CaretAssertVectorIndex(columnTwoText, i);
            colTwo = columnTwoText[i];
        }
        columnOneTextOut.push_back(colOne);
        columnTwoTextOut.push_back(colTwo);
    }
    
    toolTipTextOut = toolTipText;
}

/**
 * Find the histology slice containing the media file with the given name
 * @param mediaFileName
 *    Name of media file
 * @return
 *    Media file with the given name or NULL if not found
 */
HistologySlice*
HistologySlicesFile::findHistologySliceForHistologyCoordinate(const HistologyCoordinate& histologyCoordinate) const
{
    if (histologyCoordinate.isHistologyMediaFileNameValid()) {
        const AString mediaFileName(histologyCoordinate.getHistologyMediaFileName());
        for (auto& hs : m_histologySlices) {
            MediaFile* mf(hs->findMediaFileWithName(mediaFileName));
            if (mf != NULL) {
                return hs.get();
            }
        }
    }

    return NULL;
}


/**
 * Find the media file in the histology slices
 * @param mediaFileName
 *    Name of media file
 * @return
 *    Media file with the given name or NULL if not found
 */
MediaFile*
HistologySlicesFile::findMediaFileWithName(const AString& mediaFileName) const
{
    for (auto& hs : m_histologySlices) {
        MediaFile* mf(hs->findMediaFileWithName(mediaFileName));
        if (mf != NULL) {
            return mf;
        }
    }
    
    return NULL;
}


/**
 * @return True if this file can be written
 */
bool
HistologySlicesFile::supportsWriting() const
{
    return false;
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
HistologySlicesFile::readFile(const AString& filename)
{
    clear();
    
    try {
        HistologySlicesFileXmlStreamReader reader;
        reader.readFile(filename,
                        this);
        resetAfterSlicesChanged();

        if (getNumberOfHistologySlices() <= 0) {
            throw DataFileException("No histology slices were read\n"
                                    + getFileReadWarnings());
        }
        addFileWarningsForMissingChildFiles();
        
        createOverlapMaskingTextures();
        
        clearModified();
    }
    catch (const DataFileException& dfe) {
        clear();
        throw dfe;
    }
}

/**
 * If any child files are missing after reading the CZI meta file, add file warnings to user.
 */
void
HistologySlicesFile::addFileWarningsForMissingChildFiles()
{
    std::vector<AString> permissionChildFileNames;
    std::vector<AString> missingChildFileNames;
    
    const std::vector<AString> childDataFileNames(getChildDataFilePathNames());
    for (auto& filename : childDataFileNames) {
        FileInformation fileInfo(filename);
        if ( ! fileInfo.exists()) {
            missingChildFileNames.push_back(filename);
        }
        else if ( ! fileInfo.isReadable()) {
            permissionChildFileNames.push_back(filename);
        }
    }
    
    AString warningMessage;
    bool haveWarningsFlag(false);
    if ( ! missingChildFileNames.empty()) {
        AString msg("These child files are missing: ");
        for (const auto& name : missingChildFileNames) {
            msg.appendWithNewLine("   " + name);
        }
        haveWarningsFlag = true;
        warningMessage.appendWithNewLine(msg);
    }
    
    if ( ! permissionChildFileNames.empty()) {
        AString msg("These child file permissions do not allow reading: ");
        for (const auto& name : permissionChildFileNames) {
            msg.appendWithNewLine("   " + name);
        }
        haveWarningsFlag = true;
        warningMessage.appendWithNewLine(msg);
    }
    
    if (haveWarningsFlag) {
        warningMessage.appendWithNewLine("This will result in images not displayed and possibly Workbench crashing.");
        addFileReadWarning(warningMessage);
    }
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
HistologySlicesFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("HistologySlicesFile does not support writing");
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
void
HistologySlicesFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                 SceneClass* sceneClass)
{
    CaretDataFile::saveFileDataToScene(sceneAttributes,
                                       sceneClass);

    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
HistologySlicesFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    CaretDataFile::restoreFileDataFromScene(sceneAttributes,
                                            sceneClass);
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    resetAfterSlicesChanged();
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * @return String describing file content
 */
AString
HistologySlicesFile::toString() const
{
    AString s("HistologySlicesFile " + getFileName());
    for (auto& slice : m_histologySlices) {
        s.appendWithNewLine(slice->toString());
    }
    return s;
}

/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
HistologySlicesFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    dataFileInformation.addNameAndValue("Slice Spacing (mm)",
                                        getSliceSpacing());
    
    const uint64_t numChars(30);
    const QString separator(numChars, QChar('-'));
    const int32_t numSlices(getNumberOfHistologySlices());
    for (int32_t iSlice = 0; iSlice < numSlices; iSlice++) {
        dataFileInformation.addNameAndValue(separator, separator);
        HistologySlice* slice(getHistologySliceByIndex(iSlice));
        slice->addToDataFileContentInformation(dataFileInformation);
    }
}

/**
 * @return Names (absolute path) of all child data files of this file.
 * This includes the CZI Image Files, Distance File, and the Non-Linear
 * Transform Files.
 */
std::vector<AString>
HistologySlicesFile::getChildDataFilePathNames() const
{
    std::vector<AString> childDataFilePathNames;
    
    for (const auto& slice : m_histologySlices) {
        const std::vector<AString> names(slice->getChildDataFilePathNames());
        childDataFilePathNames.insert(childDataFilePathNames.end(),
                                      names.begin(), names.end());
    }
    return childDataFilePathNames;
}

/**
 * @return Slice spacing (distance between slices) in millimeters.  Returns 1.0 if distance unknown.
 */
float
HistologySlicesFile::getSliceSpacing() const
{
    if ( ! m_sliceSpacingValid) {
        m_sliceSpacing = 1.0;
        
        const int32_t numSlices(getNumberOfHistologySlices());
        if (numSlices >= 2) {
            const BoundingBox firstSliceBoundingBox(getHistologySliceByIndex(0)->getStereotaxicXyzBoundingBox());
            const BoundingBox lastSliceBoundingBox(getHistologySliceByIndex(numSlices - 1)->getStereotaxicXyzBoundingBox());
            if (firstSliceBoundingBox.isValid()
                && lastSliceBoundingBox.isValid()) {
                Vector3D firstSliceCenterXYZ, lastSliceCenterXYZ;
                firstSliceBoundingBox.getCenter(firstSliceCenterXYZ);
                lastSliceBoundingBox.getCenter(lastSliceCenterXYZ);
                const float distanceFirstToLastSlice((lastSliceCenterXYZ - firstSliceCenterXYZ).length());
                const float sliceSpacing(distanceFirstToLastSlice
                                         / static_cast<float>(numSlices - 1));
                if (sliceSpacing > 0.0) {
                    m_sliceSpacing = sliceSpacing;
                }
            }
            m_sliceSpacingValid = true;
        }
    }
    return m_sliceSpacing;
}

/**
 * @return True if overlap testing is enabled
 */
bool
HistologySlicesFile::isOverlapTestingEnabled()
{
    return s_overlapTestingEnabled;
}

/**
 * Set overlap testing enabled
 * @param enabled
 *    New enabled status
 */
void
HistologySlicesFile::setOverlapTestingEnabled(const bool enabled)
{
    s_overlapTestingEnabled = enabled;
}

/**
 * Create the overlapping masking textures
 */
void
HistologySlicesFile::createOverlapMaskingTextures()
{
    ElapsedTimer timer;
    timer.start();
    
    AString allMessages;
    const int32_t numSlices(getNumberOfHistologySlices());
    for (int32_t iSlice = 0; iSlice < numSlices; iSlice++) {
        HistologySlice* slice(getHistologySliceByIndex(iSlice));
        CaretAssert(slice);
        AString msg;
        if ( ! slice->createOverlapMaskingTextures(msg)) {
            allMessages.appendWithNewLine("Slice number "
                                          + slice->getSliceName()
                                          + " has error creating overlap masks: "
                                          + msg);
        }
    }
    
    if ( ! allMessages.isEmpty()) {
        CaretLogWarning(getFileName() + "\n"
                        + allMessages);
    }
    
    CaretLogInfo(getFileName()
                 + " time to create overlap masking textures "
                 + AString::number(timer.getElapsedTimeSeconds())
                 + " seconds.");
}


