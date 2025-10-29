
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
#include "CaretLogger.h"
#include "CaretPreferences.h"
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
 * Constructor.
 * @param mode
 * Mode for the content type in the container
 * @param writeIfModifiedType
 * Mode for writing if modified when instance is destroyed
 */
RecentFileItemsContainer::RecentFileItemsContainer(const RecentFileItemsContainerModeEnum::Enum mode,
                                                   const WriteIfModifiedType writeIfModifiedType)
: CaretObjectTracksModification(),
m_mode(mode),
m_writeIfModifiedType(writeIfModifiedType)
{
}

/**
 * Destructor.
 */
RecentFileItemsContainer::~RecentFileItemsContainer()
{
    if (isModified()) {
        bool writeFlag(false);
        switch (m_writeIfModifiedType) {
            case WRITE_NO:
                writeFlag = false;
                break;
            case WRITE_YES:
                writeFlag = true;
                break;
        }
        
        if (writeFlag) {
            switch (m_mode) {
                case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
                    /* Nothing to write */
                    break;
                case RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS:
                    /* Nothing to write */
                    break;
                case RecentFileItemsContainerModeEnum::FAVORITES:
                    /* Nothing to write */
                    break;
                case RecentFileItemsContainerModeEnum::OTHER:
                    /* Nothing to write */
                    break;
                case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
                {
                    CaretAssert(m_caretPreferences);
                    AString errorMessage;
                    if ( ! m_caretPreferences->writeRecentDirectories(this, errorMessage)) {
                        CaretLogSevere("Failed to write recent directories to preferences: "
                                       + errorMessage);
                    }
                }
                    break;
                case RecentFileItemsContainerModeEnum::RECENT_FILES:
                {
                    CaretAssert(m_caretPreferences);
                    AString errorMessage;
                    if ( ! m_caretPreferences->writeRecentSceneAndSpecFiles(this, errorMessage)) {
                        CaretLogSevere("Failed to write recent scene and spec files to preferences: "
                                       + errorMessage);
                    }
                }
                    break;
                case RecentFileItemsContainerModeEnum::RECENT_SCENES:
                {
                    CaretAssert(m_caretPreferences);
                    AString errorMessage;
                    if ( ! m_caretPreferences->writeRecentScenes(this, errorMessage)) {
                        CaretLogSevere("Failed to write recent scenes to preferences: "
                                       + errorMessage);
                    }
                }
                    break;
            }
        }
    }
    
    removeAllItemsIncludingFavorites();
}

/**
 * @return New instance for other usage.  
 */
RecentFileItemsContainer*
RecentFileItemsContainer::newInstance()
{
    RecentFileItemsContainer* container = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::OTHER,
                                                                       WriteIfModifiedType::WRITE_NO);
    return container;
}

/**
 * @return a new insrtance containing example data sets
 */
RecentFileItemsContainer* 
RecentFileItemsContainer::newInstanceExampleDataSets()
{
    RecentFileItemsContainer* containerOut = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS,
                                                                          WriteIfModifiedType::WRITE_NO);
    
    return containerOut;
}

/**
 * @return A new instance containing favorites using items in other containers
 * @param otherContainers
 *    Other containers with items that are shared with this container
 */
RecentFileItemsContainer*
RecentFileItemsContainer::newInstanceFavorites(std::vector<RecentFileItemsContainer*>& otherContainers)
{
    RecentFileItemsContainer* containerOut = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::FAVORITES,
                                                                          WriteIfModifiedType::WRITE_NO);
    containerOut->updateFavorites(otherContainers);
    
    return containerOut;
}

/**
 * @return A new instance containing spec and scene files in a directory
 * @param directoryPath
 *    Directory from which files are read
 */
RecentFileItemsContainer*
RecentFileItemsContainer::newInstanceSceneAndSpecFilesInDirectory(const AString& directoryPath)
{
    RecentFileItemsContainer* container = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES,
                                                                       WriteIfModifiedType::WRITE_NO);
    CaretAssert(container);
    container->addFilesInDirectoryToRecentItems(RecentFileItemTypeEnum::SCENE_FILE,
                                                directoryPath);
    container->addFilesInDirectoryToRecentItems(RecentFileItemTypeEnum::SPEC_FILE,
                                                directoryPath);
    return container;
}

/**
 * @return A new instance containing recent scene and spec files from Preferences
 * @param preferences
 *    The caret preferences
 * @param writeIfModifiedType
 * Mode for writing if modified when instance is destroyed
 */
RecentFileItemsContainer*
RecentFileItemsContainer::newInstanceRecentSceneAndSpecFiles(CaretPreferences* preferences,
                                                             const WriteIfModifiedType writeIfModifiedType)
{
    RecentFileItemsContainer* container = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::RECENT_FILES,
                                                                       writeIfModifiedType);
    CaretAssert(container);
    container->m_caretPreferences = preferences;
    
    AString errorMessage;
    const bool flag = preferences->readRecentSceneAndSpecFiles(container,
                                                               errorMessage);
    if ( ! flag) {
        CaretLogSevere("Reading recent scene/spec files from preferences: "
                       + errorMessage);
    }
    return container;
}

/**
 * @return A new instance containing recent directories from Preferences
 * @param preferences
 *    The caret preferences
 * @param writeIfModifiedType
 * Mode for writing if modified when instance is destroyed
 */
RecentFileItemsContainer*
RecentFileItemsContainer::newInstanceRecentDirectories(CaretPreferences* preferences,
                                                       const WriteIfModifiedType writeIfModifiedType)
{
    RecentFileItemsContainer* container = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES,
                                                                       writeIfModifiedType);
    CaretAssert(container);
    container->m_caretPreferences = preferences;
    
    AString errorMessage;
    const bool flag = preferences->readRecentDirectories(container,
                                                         errorMessage);
    if ( ! flag) {
        CaretLogSevere("Reading recent directories from preferences: "
                       + errorMessage);
    }
    return container;
}

/**
 * @return A new instance containing recent scenes from Preferences
 * @param preferences
 *    The caret preferences
 */
RecentFileItemsContainer*
RecentFileItemsContainer::newInstanceRecentScenes(CaretPreferences* preferences,
                                                  const WriteIfModifiedType writeIfModifiedType)
{
    RecentFileItemsContainer* container = new RecentFileItemsContainer(RecentFileItemsContainerModeEnum::RECENT_SCENES,
                                                                       writeIfModifiedType);
    CaretAssert(container);
    container->m_caretPreferences = preferences;
    
    AString errorMessage;
    const bool flag = preferences->readRecentScenes(container,
                                                    errorMessage);
    if ( ! flag) {
        CaretLogSevere("Reading recent scenes from preferences: "
                       + errorMessage);
    }
    return container;
}


/**
 * Update a favorites with items from other containers
 * @param otherContainers
 *   Items that are favorites from other containers are shared with this container
 */
void
RecentFileItemsContainer::updateFavorites(std::vector<RecentFileItemsContainer*>& otherContainers)
{
    removeAllItemsIncludingFavorites();
    
    for (auto& container : otherContainers) {
        for (auto& rfi : container->m_recentFiles) {
            if (rfi->isFavorite()) {
                std::shared_ptr<RecentFileItem> rfiCopy(rfi);
                addItemPointer(rfiCopy);
            }
        }
    }
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
 * @return The mode
 */
RecentFileItemsContainerModeEnum::Enum
RecentFileItemsContainer::getMode() const
{
    return m_mode;
}

/**
 * Add the scenes in the given scene file as recent items to this container for example data sets
 * @param exampleSceneFileAndSceneNamesOut
 *    Pairs with a scene file and scene name
 */
void
RecentFileItemsContainer::addSceneFileAndSceneNamesToExamplesContainer(const std::vector<ExampleSceneInfo>& exampleSceneInfo)
{
    CaretAssert(m_mode == RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS);
    
    for (const auto& info : exampleSceneInfo) {
        RecentFileItem* rfi(new RecentFileItem(RecentFileItemTypeEnum::EXAMPLE_SCENE,
                                               info.getFilename(),
                                               info.getName(),
                                               info.getDescription()));
        addItem(rfi);
    }
}

/**
 * Add an item to this container.  If an item with the same name is already in the container, it is removed
 * and the given item is added.
 * @param recentFile
 * Recent file item that is added will be managed (destroyed at appropriate time) by this container
 */
void
RecentFileItemsContainer::addItem(RecentFileItem* recentFile)
{
    CaretAssert(recentFile);

    /*
     * Item might not get added but if this happens
     * shared pointer will delete the item.
     */
    std::shared_ptr<RecentFileItem> sp(recentFile);
    addItemPointer(sp);
}

/**
 * Add an item in a shared pointer to this container.  If an item with the same name is already in the container, it is removed
 * and the given item is added.
 * @param recentFilePointer
 * Shared pointer that will be added to this container.
 */
void
RecentFileItemsContainer::addItemPointer(std::shared_ptr<RecentFileItem>& recentFilePointer)
{
    /*
     * Returned pair contains iterator (first) and a boolean (second).
     * If the boolean is FALSE, that means the new items was NOT inserted
     * since it duplicates an item already in the set.
     */
    auto iter = m_recentFiles.insert(recentFilePointer);
    if (iter.second) {
        /* Item was inserted */
    }
    else {
        /*
         * Item was not inserted, just need to update the date
         */
        (*iter.first)->setLastAccessByWorkbenchDateTime(recentFilePointer->getLastAccessByWorkbenchDateTime());
    }
}

/**
 * Remove all items in this container including favorites
 */
void
RecentFileItemsContainer::removeAllItemsIncludingFavorites()
{
    m_recentFiles.clear();
}

/**
 * Remove all items in this container excluding favorites
 */
void
RecentFileItemsContainer::removeAllItemsExcludingFavorites()
{
    for (auto rf : m_recentFiles) {
        if ( ! rf->isFavorite()) {
            rf->setForget(true);
        }
    }
}

/**
 * Add files of the given type and in the given directory to the recent items
 * @param recentFileItemType
 *  The recent file item type
 * @param directoryPaht
 *  Directory from which to get files
 */
void
RecentFileItemsContainer::addFilesInDirectoryToRecentItems(const RecentFileItemTypeEnum::Enum recentFileItemType,
                                                           const AString& directoryPath)
{
    DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::UNKNOWN;
    switch (recentFileItemType) {
        case RecentFileItemTypeEnum::DIRECTORY:
            CaretAssert(0);
            return;
            break;
        case RecentFileItemTypeEnum::EXAMPLE_SCENE:
            dataFileType = DataFileTypeEnum::UNKNOWN;
            break;
        case RecentFileItemTypeEnum::SCENE_FILE:
            dataFileType = DataFileTypeEnum::SCENE;
            break;
        case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
            dataFileType = DataFileTypeEnum::UNKNOWN;
            break;
        case RecentFileItemTypeEnum::SPEC_FILE:
            dataFileType = DataFileTypeEnum::SPECIFICATION;
            break;
    }
    
    if (dataFileType != DataFileTypeEnum::UNKNOWN) {
        std::vector<AString> fileNames = DataFileTypeEnum::getFilesInDirectory(dataFileType,
                                                                               directoryPath);
        
        for (auto name : fileNames) {
            RecentFileItem* fileItem = new RecentFileItem(recentFileItemType,
                                                          name);
            addItem(fileItem);
        }
    }
}

/**
 * Get the recent file item with the given path and file name.
 * @param pathAndFileName
 * Path and file name to match
 * @return Pointer to matching item or NULL if not found.
 */
RecentFileItem*
RecentFileItemsContainer::getItemWithPathAndFileName(const AString& pathAndFileName)
{
    for (auto& item : m_recentFiles) {
        if (pathAndFileName == item->getPathAndFileName()) {
            return item.get();
        }
    }
    
    return NULL;
}

/**
 * @return True if this container is empty (no recent file items)
 */
bool
RecentFileItemsContainer::isEmpty() const
{
    return m_recentFiles.empty();
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
        CaretAssert(item);
        if (itemsFilter.testItemPassesFilter(item.get())) {
            itemsOut.push_back(item.get());
        }
    }
    
    return itemsOut;
}

/**
 * @return All items in this container
 */
std::vector<RecentFileItem*>
RecentFileItemsContainer::getAllItems() const
{
    std::vector<RecentFileItem*> itemsOut;
    
    for (const auto& item : m_recentFiles) {
        CaretAssert(item);
        itemsOut.push_back(item.get());
    }
    
    return itemsOut;
}

/**
 * @return True if this items supports favorite
 */
bool
RecentFileItemsContainer::supportsFavorite() const
{
    bool supportsFlag(true);
    
    switch (m_mode) {
        case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::FAVORITES:
            break;
        case RecentFileItemsContainerModeEnum::OTHER:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_FILES:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_SCENES:
            break;
    }

    return supportsFlag;
}

/**
 * @return True if this items supports forget
 */
bool
RecentFileItemsContainer::supportsForget() const
{
    bool supportsFlag(true);
    
    switch (m_mode) {
        case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::FAVORITES:
            break;
        case RecentFileItemsContainerModeEnum::OTHER:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_FILES:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_SCENES:
            break;
    }

    return supportsFlag;
}

/**
 * @return True if this items supports share
 */
bool
RecentFileItemsContainer::supportsShare() const
{
    bool supportsFlag(true);
    
    switch (m_mode) {
        case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
            break;
        case RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::FAVORITES:
            break;
        case RecentFileItemsContainerModeEnum::OTHER:
            supportsFlag = false;
            break;
        case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_FILES:
            break;
        case RecentFileItemsContainerModeEnum::RECENT_SCENES:
            break;
    }

    return supportsFlag;
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
RecentFileItemsContainer::sort(const RecentFileItemSortingKeyEnum::Enum sortingKey,
                               std::vector<RecentFileItem*>& items)
{
    switch (sortingKey) {
        case RecentFileItemSortingKeyEnum::DATE_NEWEST:
            std::sort(items.begin(),
                      items.end(),                                      /* > is newer in Qt Documentation */
                      [](const RecentFileItem* a, RecentFileItem* b) { return a->getLastAccessByWorkbenchDateTime() > b->getLastAccessByWorkbenchDateTime(); });
            break;
        case RecentFileItemSortingKeyEnum::DATE_OLDEST:
            std::sort(items.begin(),
                      items.end(),                                      /* < is earlier in Qt Documentation */
                      [](const RecentFileItem* a, RecentFileItem* b) { return a->getLastAccessByWorkbenchDateTime() < b->getLastAccessByWorkbenchDateTime(); });
            break;
        case RecentFileItemSortingKeyEnum::MODIFIED_NEWEST:
            std::sort(items.begin(),
                      items.end(),                                      /* > is newer in Qt Documentation */
                      [](const RecentFileItem* a, RecentFileItem* b) { return a->getLastModifiedDateTime() > b->getLastModifiedDateTime(); });
            break;
        case RecentFileItemSortingKeyEnum::MODIFIED_OLDEST:
            std::sort(items.begin(),
                      items.end(),                                      /* < is earlier in Qt Documentation */
                      [](const RecentFileItem* a, RecentFileItem* b) { return a->getLastModifiedDateTime() < b->getLastModifiedDateTime(); });
            break;
        case RecentFileItemSortingKeyEnum::NAME_ASCENDING:
            std::sort(items.begin(),
                      items.end(),
                      [](const RecentFileItem* a, RecentFileItem* b) { return a->getFileName() < b->getFileName(); });
            break;
        case RecentFileItemSortingKeyEnum::NAME_DESCENDING:
            std::sort(items.begin(),
                      items.end(),
                      [](const RecentFileItem* a, RecentFileItem* b) { return a->getFileName() > b->getFileName(); });
            break;
    }
}

/**
 * Reduce the number of items in the container so that it does not exceed the given maximum number of items.
 * Note: Favorites are NOT removed
 * @param maximumNumberOfItems
 *    Maximum number of items in the container after this method completes.  However, since favorites are
 *    never removed, this maximum may be exceeded if it is less than the number of favorites in the container.
 * @return Number of items that were removed from the container
 */
int32_t
RecentFileItemsContainer::removeItemsExceedingMaximumNumber(const int32_t maximumNumberOfItems)
{
    std::vector<RecentFileItem*> allItems = getAllItems();
    const int32_t numItems = static_cast<int32_t>(allItems.size());
    if (numItems <= maximumNumberOfItems) {
        return 0;
    }
    
    /*
     * Keep newest items so sort by date last accessed by wb_view
     */
    sort(RecentFileItemSortingKeyEnum::DATE_NEWEST,
         allItems);
    
    static const bool testFlag(false);
    if (testFlag) {
        std::cout << "Container: " << RecentFileItemsContainerModeEnum::toName(m_mode) << std::endl;
        std::cout << "Before Sorting-----------------" << std::endl;
        std::cout << toString() << std::endl;
    }
    
    /*
     * Keep all favorites
     */
    std::vector<RecentFileItem*> itemsToKeep;
    int32_t numItemsToKeep(maximumNumberOfItems);
    for (int32_t i = 0; i < numItems; i++) {
        CaretAssertVectorIndex(allItems, i);
        if (allItems[i]->isFavorite()) {
            --numItemsToKeep;
            itemsToKeep.push_back(new RecentFileItem(*allItems[i]));
        }
    }
    
    if (testFlag) {
        std::cout << std::endl << "After Sorting-----------------" << std::endl;
        std::cout << toString() << std::endl << std::endl;
    }
    
    /*
     * Keep a limited number of non-favorites
     */
    if (numItemsToKeep > 0) {
        for (int32_t i = 0; i < numItems; i++) {
            CaretAssertVectorIndex(allItems, i);
            if ( ! allItems[i]->isFavorite()) {
                itemsToKeep.push_back(new RecentFileItem(*allItems[i]));
                --numItemsToKeep;
            }
            if (numItemsToKeep <= 0) {
                break;
            }
        }
    }

    /*
     * Remove all existing items and add back items
     */
    removeAllItemsIncludingFavorites();
    for (auto ptr : itemsToKeep) {
        addItem(ptr);
    }
    
    const int32_t numItemsRemoved = (numItems - m_recentFiles.size());
    return numItemsRemoved;
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
    removeAllItemsIncludingFavorites();
    
    QXmlStreamReader reader(xml);
    if (reader.atEnd()) {
        /* empty string is OK */
        clearModified();
        return true;
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
    AString sceneName;
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
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_FILE_ITEM_TYPE) {
                fileTypeString = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_FAVORITE) {
                favoriteString = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME) {
                pathAndFileName = reader.readElementText().trimmed();
            }
            else if (reader.name() == XML_TAG_RECENT_FILE_ITEM_SCENE_NAME) {
                sceneName = reader.readElementText().trimmed();
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
    const RecentFileItemTypeEnum::Enum fileType = RecentFileItemTypeEnum::fromName(fileTypeString,
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
                                              pathAndFileName,
                                              sceneName);
    item->setComment(comment);
    item->setFavorite(favoriteString.toBool());
    item->setLastAccessByWorkbenchDateTimeFromString(dateAndTimeString);
    
    addItem(item);
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
                                    rfi->getLastAccessByWorkbenchDateTimeAsString());
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_FAVORITE,
                                    AString::fromBool(rfi->isFavorite()));
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_FILE_ITEM_TYPE,
                                    RecentFileItemTypeEnum::toName(rfi->getFileItemType()));
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME,
                                    rfi->getPathAndFileName());
            writer.writeTextElement(XML_TAG_RECENT_FILE_ITEM_SCENE_NAME,
                                    rfi->getSceneName());
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

/**
 * Comparison operation used by the SET containing the RecentFileItems.
 * @param lhs
 *  First item for comparison.
 * @param lhs
 *  Second item for comparison.
 * @return
 *     True if "lhs" is less than "rhs"
 */
bool
RecentFileItemsContainer::ItemCompare::operator() (const RecentFileItem* lhs, const RecentFileItem* rhs) const
{
    return RecentFileItemsContainer::CompareItems(lhs, rhs);
}
/**
 * Comparison operation used by the SET containing the RecentFileItems.
 * @param lhs
 *  First item for comparison.
 * @param lhs
 *  Second item for comparison.
 * @return
 *     True if "lhs" is less than "rhs"
 */
bool
RecentFileItemsContainer::ItemCompareSharedPtr::operator() (const std::shared_ptr<RecentFileItem>& lhs,
                                                            const std::shared_ptr<RecentFileItem>& rhs) const
{
    return RecentFileItemsContainer::CompareItems(lhs.get(), rhs.get());
}

/**
 * Comparison operation used by the SET containing the RecentFileItems.
 * @param lhs
 *  First item for comparison.
 * @param lhs
 *  Second item for comparison.
 * @return
 *     True if "lhs" is less than "rhs"
 */
bool
RecentFileItemsContainer::CompareItems(const RecentFileItem* lhs, const RecentFileItem* rhs)
{
    CaretAssert(lhs);
    CaretAssert(rhs);
    
    bool compareSceneNameFlag(false);
    switch (lhs->getFileItemType()) {
        case RecentFileItemTypeEnum::DIRECTORY:
            break;
        case RecentFileItemTypeEnum::EXAMPLE_SCENE:
            compareSceneNameFlag = true;
            break;
        case RecentFileItemTypeEnum::SCENE_FILE:
            break;
        case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
            compareSceneNameFlag = true;
            break;
        case RecentFileItemTypeEnum::SPEC_FILE:
            break;
    }
    
    if (compareSceneNameFlag) {
        if (lhs->getPathAndFileName() == rhs->getPathAndFileName()) {
            return (lhs->getSceneName() < rhs->getSceneName());
        }
    }
    return (lhs->getPathAndFileName() < rhs->getPathAndFileName());
}

