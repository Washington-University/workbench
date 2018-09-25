#ifndef __SCENE_DATA_FILE_TREE_ITEM_H__
#define __SCENE_DATA_FILE_TREE_ITEM_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include <QStandardItem>

#include "AString.h"

namespace caret {

    class SceneDataFileTreeItem : public QStandardItem {
        
    public:
        /**
         * @return ItemType for a directory
         */
        static int32_t getItemTypeDirectory() { return (QStandardItem::UserType + 1); }
        
        /**
         * @return ItemType for a data file
         */
        static int32_t getItemTypeFile()  { return (QStandardItem::UserType + 2); }
        
        SceneDataFileTreeItem(const AString& text,
                              const AString& absolutePathName,
                              const int32_t itemType);
        
        virtual ~SceneDataFileTreeItem();
        
        SceneDataFileTreeItem(const SceneDataFileTreeItem&);

        SceneDataFileTreeItem& operator=(const SceneDataFileTreeItem&);
        
        AString getAbsolutePathName() const;
        
        virtual int type() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperSceneDataFileTreeItem(const SceneDataFileTreeItem& obj);
        
        AString m_absolutePathName;
        
        int32_t m_itemType;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_DATA_FILE_TREE_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_DATA_FILE_TREE_ITEM_DECLARE__

} // namespace
#endif  //__SCENE_DATA_FILE_TREE_ITEM_H__
