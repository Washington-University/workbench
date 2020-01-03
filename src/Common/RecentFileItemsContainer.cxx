
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __RECENT_FILE_ITEMS_CONTAINER_DECLARE__
#include "RecentFileItemsContainer.h"
#undef __RECENT_FILE_ITEMS_CONTAINER_DECLARE__

#include <algorithm>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "DataFileTypeEnum.h"
#include "RecentFileItem.h"
#include "RecentFileItemsFilter.h"

using namespace caret;


    
/**
 * \class caret::RecentFileItemsContainer 
 * \brief Container for recent file items
 * \ingroup Common
 */

/**
 * Constructor for scene and spec files in a directory
 * @param directoryPath
 * Path of directory
 */
RecentFileItemsContainer::RecentFileItemsContainer(const AString& directoryPath)
: CaretObjectTracksModification()
{
    addFilesInDirectoryToRecentItems(RecentFileTypeEnum::SCENE_FILE,
                                     directoryPath);
    addFilesInDirectoryToRecentItems(RecentFileTypeEnum::SPEC_FILE,
                                     directoryPath);
}

/**
 * Destructor.
 */
RecentFileItemsContainer::~RecentFileItemsContainer()
{
}

/**
 *@return True if this instance has been modified, else false.
 */
bool
RecentFileItemsContainer::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    
    for (const auto& rfi : m_recentFiles) {
        if (rfi->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modified status
 */
void
RecentFileItemsContainer::clearModified()
{
    CaretObjectTracksModification::clearModified();
    
    for (auto& rfi : m_recentFiles) {
        rfi->clearModified();
    }
}

/**
 * Add an item to this container
 * @param recentFile
 * Recent file item that is added will be managed (destroyed at appropriate time) by this container
 */
void
RecentFileItemsContainer::addItem(RecentFileItem* recentFile)
{
    CaretAssert(recentFile);
    
    std::unique_ptr<RecentFileItem> ptr(recentFile);
    m_recentFiles.push_back(std::move(ptr));
}

/**
 * Clear all items in this container
 */
void
RecentFileItemsContainer::clear()
{
    m_recentFiles.clear();
}

/**
 * Add files of the given type and in the given directory to the recent items
 * @param recentFileType
 *  The recent file type type
 * @param directoryPaht
 *  Directory from which to get files
 */
void
RecentFileItemsContainer::addFilesInDirectoryToRecentItems(const RecentFileTypeEnum::Enum recentFileType,
                                                           const AString& directoryPath)
{
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    switch (recentFileType) {
        case RecentFileTypeEnum::DIRECTORY:
            CaretAssert(0);
            return;
            break;
        case RecentFileTypeEnum::SCENE_FILE:
            dataFileType = DataFileTypeEnum::SCENE;
            break;
        case RecentFileTypeEnum::SPEC_FILE:
            dataFileType = DataFileTypeEnum::SPECIFICATION;
            break;
    }
    
    std::vector<AString> fileNames = DataFileTypeEnum::getFilesInDirectory(dataFileType,
                                                                           directoryPath);
    
    for (auto name : fileNames) {
        std::cout << "Adding file: " << name << std::endl;
        RecentFileItem* fileItem = new RecentFileItem(recentFileType,
                                                      name);
        addItem(fileItem);
    }
}

/**
 * Get items in this container using the given item filter
 * @param itemsFilter
 *    Contains information for filtering items
 * @return
 *    Items in container that match filter
 */
std::vector<RecentFileItem*>
RecentFileItemsContainer::getItems(const RecentFileItemsFilter& itemsFilter) const
{
    std::vector<RecentFileItem*> itemsOut;
    
    for (const auto& item : m_recentFiles) {
        itemsOut.push_back(item.get());
    }
    
    return itemsOut;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
RecentFileItemsContainer::toString() const
{
    return "RecentFileItemsContainer";
}

/**
 * Sort recent file items ordered by the given sorting key
 * @param sortingKey
 *    Key on which to sort
 */
void
RecentFileItemsContainer::sort(const RecentFileItemSortingKeyEnum::Enum sortingKey)
{
    switch (sortingKey) {
        case RecentFileItemSortingKeyEnum::DATE_ASCENDING:
            std::sort(m_recentFiles.begin(),
                      m_recentFiles.end(),
                      [](const std::unique_ptr<RecentFileItem>& a, const std::unique_ptr<RecentFileItem>& b) { return a->getLastAccessDateTime() < b->getLastAccessDateTime(); });
            break;
        case RecentFileItemSortingKeyEnum::DATE_DESCENDING:
            std::sort(m_recentFiles.begin(),
                      m_recentFiles.end(),
                      [](const std::unique_ptr<RecentFileItem>& a, const std::unique_ptr<RecentFileItem>& b) { return a->getLastAccessDateTime() > b->getLastAccessDateTime(); });
            break;
        case RecentFileItemSortingKeyEnum::NAME_ASCENDING:
            std::sort(m_recentFiles.begin(),
                      m_recentFiles.end(),
                      [](const std::unique_ptr<RecentFileItem>& a, const std::unique_ptr<RecentFileItem>& b) { return a->getFileName() < b->getFileName(); });
            break;
        case RecentFileItemSortingKeyEnum::NAME_DESCENDING:
            std::sort(m_recentFiles.begin(),
                      m_recentFiles.end(),
                      [](const std::unique_ptr<RecentFileItem>& a, const std::unique_ptr<RecentFileItem>& b) { return a->getFileName() > b->getFileName(); });
            break;
    }
}

/**
 * Read from the given xml string
 * @param xml
 *     String containing XML.
 * @param errorMessageOut
 *     Contains error information
 * @return
 *     True if successful, else false
 */
bool
RecentFileItemsContainer::readFromXML(const AString& xml,
                                      AString& errorMessageOut)
{
    errorMessageOut.clear();
    m_recentFiles.clear();
    
    QXmlStreamReader reader(xml);
    if (reader.atEnd()) {
        /* empty string is OK */
        clearModified();
        return false;
    }
    
    clearModified();
    
    reader.readNextStartElement();
    if (reader.name() == XML_TAG_RECENT_FILE_ITEMS_CONTAINER) {
        readFromXmlVersionOne(reader);
    }
    else {
        reader.raiseError("First element should be \""
                          + XML_TAG_RECENT_FILE_ITEMS_CONTAINER
                          + "\" but is \""
                          + reader.name().toString()
                          + "\"");
    }
    
    if (reader.hasError()) {
        errorMessageOut = reader.errorString();
        return false;
    }
    
    return true;
}

/**
 * Read the XML version one
 * @param reader
 *    The XML reader
 */
void
RecentFileItemsContainer::readFromXmlVersionOne(QXmlStreamReader& reader)
{
    bool endElementFoundFlag(false);
    
    while ( ( ! reader.atEnd())
           && ( ! endElementFoundFlag)) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == XML_TAG_RECENT_FILE_ITEM) {
                readFromXMLVersionOneRecentFileItem(reader);
            }
            else {
                reader.raiseError("Unrecognized element \""
                                  + reader.name().toString()
                                  + "\"");
                reader.skipCurrentElement();
            }
        }
        else if (reader.isEndElement()) {
            if (reader.name() == XML_TAG_RECENT_FILE_ITEMS_CONTAINER) {
                endElementFoundFlag = true;
            }
        }
    }
}

/**
 * Read the XML version one for a recent file item
 * @param reader
 *    The XML reader
 */
void
RecentFileItemsContainer::readFromXMLVersionOneRecentFileItem(QXmlStreamReader& reader)
{
    bool endElementFoundFlag(false);

    AString pathAndFileName;
    AString comment;
    AString fileTypeString;
    AString dateAndTimeString;
    AString favoriteString;
    
    while ( ( ! reader.atEnd())
           && ( ! endElementFoundFlag)) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == XML_TAG_RECENT_FILE_ITEM_COMMENT) {
                comment = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_DATE_AND_TIME) {
                dateAndTimeString = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_FILE_TYPE) {
                fileTypeString = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_FAVORITE) {
                favoriteString = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME) {
                pathAndFileName = reader.readElementText().trimmed();
            }
            else {
                reader.raiseError("Unrecognized child element of "
                                  + XML_TAG_RECENT_FILE_ITEM
                                  + " \""
                                  + reader.name().toString()
                                  + "\"");
                reader.skipCurrentElement();
                return;
            }
        }
        else if (reader.isEndElement()) {
            if (reader.name() == XML_TAG_RECENT_FILE_ITEM) {
                endElementFoundFlag = true;
            }
        }
    }
    
    bool validFileTypeFlag(false);
    const RecentFileTypeEnum::Enum fileType = RecentFileTypeEnum::fromName(fileTypeString,
                                                                           &validFileTypeFlag);
    
    if ( ! validFileTypeFlag) {
        reader.raiseError(XML_TAG_RECENT_FILE_ITEM
                          + " has invalid file type \""
                          + fileTypeString
                          + "\"");
        return;
    }
    
    if (pathAndFileName.isEmpty()) {
        reader.raiseError(XML_TAG_RECENT_FILE_ITEM
                          + " has empty path and file name");
        return;
    }
    
    RecentFileItem* item = new RecentFileItem(fileType,
                                              pathAndFileName);
    item->setComment(comment);
    item->setFavorite(favoriteString.toBool());
    item->setLastAccessDateTimeFromString(dateAndTimeString);
    
    addItem(item);
    
    sort(RecentFileItemSortingKeyEnum::NAME_ASCENDING);
}

/**
 * Write to the given xml string
 * @param xml
 *     XML written to this string.
 * @param errorMessageOut
 *     Contains error information
 * @return
 *     True if successful, else false
 */
bool
RecentFileItemsContainer::writeToXML(AString& xml,
                                     AString& errorMessageOut) const
{
    xml.clear();
    errorMessageOut.clear();
    
    QXmlStreamWriter writer(&xml);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(XML_TAG_RECENT_FILE_ITEMS_CONTAINER);
    
    for (const auto& rfi : m_recentFiles) {
        if ( ! rfi->isForget()) {
            writer.writeStartElement(XML_TAG_RECENT_FILE_ITEM);
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_COMMENT,
                                    rfi->getComment());
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_DATE_AND_TIME,
                                    rfi->getLastAccessDateTimeAsString());
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_FAVORITE,
                                    AString::fromBool(rfi->isFavorite()));
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_FILE_TYPE,
                                    RecentFileTypeEnum::toName(rfi->getFileType()));
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME,
                                    rfi->getPathAndFileName());
            writer.writeEndElement();
        }
    }
    
    writer.writeEndElement();
    writer.writeEndDocument();
    
    return true;
}

/**
 * Test XML reading/writing by writing and reading it
 */
void
RecentFileItemsContainer::testXmlReadingAndWriting()
{
    sort(RecentFileItemSortingKeyEnum::NAME_ASCENDING);
    
    const int32_t validCount = static_cast<int32_t>(m_recentFiles.size());
    AString xmlOne;
    AString errorMessage;
    bool successFlag = writeToXML(xmlOne, errorMessage);
    if ( ! successFlag) {
        std::cout << "First write to XML failed " << errorMessage << std::endl;
        return;
    }
    std::cout << "XML First write: " << std::endl << xmlOne << std::endl << std::endl;

    
    successFlag = readFromXML(xmlOne, errorMessage);
    if ( ! successFlag) {
        std::cout << "Read from XML failed " << errorMessage << std::endl;
        return;
    }
    
    AString xmlTwo;
    successFlag = writeToXML(xmlTwo, errorMessage);
    if ( ! successFlag) {
        std::cout << "Second write to XML failed " << errorMessage << std::endl;
        return;
    }
    
    std::cout << "XML second write: " << std::endl << xmlTwo << std::endl << std::endl;
    
    if (xmlOne != xmlTwo) {
        std::cout << "XML strings to not match: " << std::endl;
        std::cout << "FIRST: " << std::endl << xmlOne << std::endl;
        std::cout << "Two: " << std::endl << xmlTwo << std::endl;
        return;
    }
    
    if (validCount != static_cast<int32_t>(m_recentFiles.size())) {
        std::cout << "Number of elements changed from " << validCount << " to " << m_recentFiles.size()
        << " during testing" << std::endl;
        return;
    }
    
    std::cout << "XML Read/Write Testing Successful" << std::endl;
}


