#ifndef __RECENT_FILE_ITEMS_CONTAINER_H__
#define __RECENT_FILE_ITEMS_CONTAINER_H__

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



#include <memory>
#include <set>
#include <utility>

#include "CaretObjectTracksModification.h"
#include "RecentFileItemSortingKeyEnum.h"
#include "RecentFileItemTypeEnum.h"
#include "RecentFileItemsContainerModeEnum.h"

class QXmlStreamReader;

namespace caret {

    class CaretPreferences;
    class RecentFileItem;
    class RecentFileItemsFilter;
    
    class RecentFileItemsContainer : public CaretObjectTracksModification {
        
    public:
        /*
         * Write if modified when this container is destroyed
         */
        enum WriteIfModifiedType {
            /* No, do not update preferences */
            WRITE_NO,
            /* Yes, update preferences */
            WRITE_YES
        };
        
        static RecentFileItemsContainer* newInstance();

        static RecentFileItemsContainer* newInstanceExampleDataSets();
        
        static RecentFileItemsContainer* newInstanceFavorites(std::vector<RecentFileItemsContainer*>& otherContainers);
        
        static RecentFileItemsContainer* newInstanceSceneAndSpecFilesInDirectory(const AString& directoryPath);
        
        static RecentFileItemsContainer* newInstanceRecentSceneAndSpecFiles(CaretPreferences* preferences,
                                                                            const WriteIfModifiedType writeIfModifiedType);
        
        static RecentFileItemsContainer* newInstanceRecentScenes(CaretPreferences* preferences,
                                                                 const WriteIfModifiedType writeIfModifiedType);
        
        static RecentFileItemsContainer* newInstanceRecentDirectories(CaretPreferences* preferences,
                                                                      const WriteIfModifiedType writeIfModifiedType);
        
        virtual ~RecentFileItemsContainer();
        
        RecentFileItemsContainer(const RecentFileItemsContainer&) = delete;

        RecentFileItemsContainer& operator=(const RecentFileItemsContainer&) = delete;

        void updateFavorites(std::vector<RecentFileItemsContainer*>& otherContainers);
        
        bool isEmpty() const;
        
        void addSceneFileAndSceneNamesToExamplesContainer(const std::vector<std::pair<AString, AString>>& exampleSceneFileAndSceneNames);
        
        std::vector<RecentFileItem*> getAllItems() const;
        
        std::vector<RecentFileItem*> getItems(const RecentFileItemsFilter& itemsFilter) const;
                
        void addItem(RecentFileItem* recentFile);
        
        void removeAllItemsIncludingFavorites();
        
        void removeAllItemsExcludingFavorites();
        
        RecentFileItem* getItemWithPathAndFileName(const AString& pathAndFileName);
        
        RecentFileItemsContainerModeEnum::Enum getMode() const;
        
        bool supportsFavorite() const;
        
        bool supportsForget() const;
        
        bool supportsShare() const;

        virtual bool isModified() const;
        
        virtual void clearModified();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        int32_t removeItemsExceedingMaximumNumber(const int32_t maximumNumberOfItems);

        static void sort(const RecentFileItemSortingKeyEnum::Enum sortingKey,
                         std::vector<RecentFileItem*>& items);
        
        bool readFromXML(const AString& xml,
                         AString& errorMessageOut);
        
        bool writeToXML(AString& xml,
                        AString& errorMessageOut) const;
        
        void testXmlReadingAndWriting();
        
    private:
        RecentFileItemsContainer(const RecentFileItemsContainerModeEnum::Enum mode,
                                 const WriteIfModifiedType writeIfModifiedType);
        
        void addItemPointer(std::shared_ptr<RecentFileItem>& recentFilePointer);
        
        void addFilesInDirectoryToRecentItems(const RecentFileItemTypeEnum::Enum recentFileItemType,
                                              const AString& directoryPath);
        
        void readFromXmlVersionOne(QXmlStreamReader& reader);
        
        void readFromXMLVersionOneRecentFileItem(QXmlStreamReader& reader);
        
        RecentFileItemsContainerModeEnum::Enum m_mode;
        
        CaretPreferences* m_caretPreferences = NULL;
        
        const WriteIfModifiedType m_writeIfModifiedType;

        /**
         * Used with the SET containing RecentFileItems to compare using the path and filename.
         * Without this, just pointers are inserted and that would allow duplicates with same path and filename.
         */
        struct ItemCompare {
            bool operator() (const RecentFileItem* lhs, const RecentFileItem* rhs) const;
        };
        
        struct ItemCompareSharedPtr {
            bool operator() (const std::shared_ptr<RecentFileItem>& lhs, const std::shared_ptr<RecentFileItem>& rhs) const;
        };
        
        static bool CompareItems(const RecentFileItem* lhs, const RecentFileItem* rhs);
        
        /**
         * Set containing RecentFileItem's.  Note use of comparison operator so that
         * comparison is performed on content of the RecentFileItem and NOT the pointers.
         *
         * A Favorites container uses items from other containers so shared pointers are used
         * since an item may in a favorites container and another container.
         *
         * A set is used to avoid duplicate entries.
         */
        std::set<std::shared_ptr<RecentFileItem>, ItemCompareSharedPtr> m_recentFiles;
                
        // ADD_NEW_MEMBERS_HERE

        
        static const AString XML_TAG_RECENT_FILE_ITEMS_CONTAINER;
        static const AString XML_TAG_RECENT_FILE_ITEM;
        static const AString XML_TAG_RECENT_FILE_ITEM_COMMENT;
        static const AString XML_TAG_RECENT_FILE_ITEM_DATE_AND_TIME;
        static const AString XML_TAG_RECENT_FILE_ITEM_FILE_ITEM_TYPE;
        static const AString XML_TAG_RECENT_FILE_ITEM_FAVORITE;
        static const AString XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME;
        static const AString XML_TAG_RECENT_FILE_ITEM_SCENE_NAME;

    };
    
#ifdef __RECENT_FILE_ITEMS_CONTAINER_DECLARE__
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEMS_CONTAINER = "RecentFileItemsContainer";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM = "RecentFileItem";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_COMMENT = "Comment";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_DATE_AND_TIME = "DateTime";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_FILE_ITEM_TYPE = "FileItemType";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_FAVORITE = "Favorite";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_PATH_AND_FILE_NAME = "PathAndFileName";
    const AString RecentFileItemsContainer::XML_TAG_RECENT_FILE_ITEM_SCENE_NAME = "SceneName";
#endif // __RECENT_FILE_ITEMS_CONTAINER_DECLARE__

} // namespace
#endif  //__RECENT_FILE_ITEMS_CONTAINER_H__
