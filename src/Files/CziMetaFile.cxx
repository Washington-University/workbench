
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

#define __CZI_META_FILE_DECLARE__
#include "CziMetaFile.h"
#undef __CZI_META_FILE_DECLARE__

#include "CaretAssert.h"
#include "CziMetaFileXmlStreamReader.h"
#include "DataFileException.h"
#include "EventManager.h"
#include "GiftiMetaData.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::CziMetaFile 
 * \brief File for Workbench CZI meta data
 * \ingroup Files
 */

/**
 * Constructor.
 */
CziMetaFile::CziMetaFile()
: CaretDataFile(DataFileTypeEnum::CZI_META_FILE)
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_metaData.reset(new GiftiMetaData());
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
CziMetaFile::~CziMetaFile()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CziMetaFile::CziMetaFile(const CziMetaFile& obj)
: CaretDataFile(obj)
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_metaData.reset(new GiftiMetaData());

    this->copyHelperCziMetaFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CziMetaFile&
CziMetaFile::operator=(const CziMetaFile& obj)
{
    if (this != &obj) {
        CaretDataFile::operator=(obj);
        this->copyHelperCziMetaFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CziMetaFile::copyHelperCziMetaFile(const CziMetaFile& obj)
{
    *m_metaData = *obj.m_metaData;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
CziMetaFile::receiveEvent(Event* event)
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
CziMetaFile::isEmpty() const
{
    return false;
}

/**
 * Clear the content of the file
 */
void
CziMetaFile::clear()
{
    CaretDataFile::clear();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
CziMetaFile::getStructure() const
{
    return StructureEnum::INVALID;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
CziMetaFile::setStructure(const StructureEnum::Enum /*structure*/)
{
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
CziMetaFile::getFileMetaData()
{
    return m_metaData.get();
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CziMetaFile::getFileMetaData() const
{
    return m_metaData.get();
}

/**
 * Add a slice
 * @param slice
 *    Pointer to the slice.  Will take ownership of this instance.
 */
void
CziMetaFile::addSlice(Slice* slice)
{
    CaretAssert(slice);
    std::unique_ptr<Slice> slicePtr(slice);
    m_slices.push_back(std::move(slicePtr));
}

/**
 * @return Number of slices in the meta file
 */
int32_t
CziMetaFile::getNumberOfSlices() const
{
    return m_slices.size();
}

/**
 * @return Pointer to slice at given index or NULL if index is invalid
 * @param sliceIndex
 *    Index of slice
 */
const CziMetaFile::Slice*
CziMetaFile::getSlice(const int32_t sliceIndex) const
{
    CaretAssertVectorIndex(m_slices, sliceIndex);
    if ((sliceIndex >= 0)
        && (sliceIndex < getNumberOfSlices())) {
        return m_slices[sliceIndex].get();
    }
    return NULL;
}

/**
 * @return Slice with the given slice number
 * @param sliceNumber
 *     Number of the slice
 */
const CziMetaFile::Slice*
CziMetaFile::getSliceNumber(const int32_t sliceNumber) const
{
    for (auto& ptr : m_slices) {
        if (ptr->m_sliceNumber == sliceNumber) {
            return ptr.get();
        }
    }
    return NULL;
}

/**
 * @return True if this file can be written
 */
bool
CziMetaFile::supportsWriting() const
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
CziMetaFile::readFile(const AString& filename)
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
CziMetaFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("CziMetaFile does not support writing");
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
CziMetaFile::saveFileDataToScene(const SceneAttributes* sceneAttributes,
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
CziMetaFile::restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
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
CziMetaFile::toString() const
{
    AString s(CaretDataFile::toString());
    
    for (auto& slice : m_slices) {
        s.appendWithNewLine(slice->toString());
    }
    return s;
}
