
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

#include "CaretAssert.h"
#include "CziMetaFileXmlStreamReader.h"
#include "DataFileException.h"
#include "EventManager.h"
#include "GiftiMetaData.h"
#include "HistologySlice.h"
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
: CaretDataFile(obj)
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
    CaretAssertMessage(0, "Copying not supported");
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
//    std::unique_ptr<HistologySlice> slicePtr(histologySlice);
//    m_histologySlices.push_back(std::move(slicePtr));
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
 * @param sliceNumber
 *     Number of the slice
 */
const HistologySlice*
HistologySlicesFile::getHistologySliceByNumber(const int32_t sliceNumber) const
{
    for (auto& ptr : m_histologySlices) {
        if (ptr->getSliceNumber() == sliceNumber) {
            return ptr.get();
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
        CziMetaFileXmlStreamReader reader;
        reader.readFile(filename,
                        this);
        
        //std::cout << "CZI FILE INFO: " << filename << std:;endl;
        //std::cout << toString() << std::endl;
    }
    catch (const DataFileException& dfe) {
        clear();
        throw dfe;
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
