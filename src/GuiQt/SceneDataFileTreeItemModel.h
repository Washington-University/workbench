#ifndef __SCENE_DATA_FILE_TREE_ITEM_MODEL_H__
#define __SCENE_DATA_FILE_TREE_ITEM_MODEL_H__

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


#include <map>
#include <memory>

#include <QStandardItemModel>

#include "SceneDataFileInfo.h"


namespace caret {

    class SceneDataFileTreeItem;
    
    class SceneDataFileTreeItemModel : public QStandardItemModel {
        
        Q_OBJECT

    public:
        SceneDataFileTreeItemModel(const AString& sceneFilePathAndName,
                                   const AString& baseDirectoryPath,
                                   const std::vector<SceneDataFileInfo>& sceneDataFileInfo,
                                   const SceneDataFileInfo::SortMode sortMode);
        
        virtual ~SceneDataFileTreeItemModel();
        
        SceneDataFileTreeItemModel(const SceneDataFileTreeItemModel&) = delete;

        SceneDataFileTreeItemModel& operator=(const SceneDataFileTreeItemModel&) = delete;

        SceneDataFileTreeItem* addFindDirectoryPath(const AString& absoluteDirName);
        
        SceneDataFileTreeItem* addDirectory(const AString& absoluteDirName,
                                            const AString& absoluteParentDirName);
        
        SceneDataFileTreeItem* findDirectory(const AString& absoluteDirName);

        SceneDataFileTreeItem* addFile(const AString& absoluteFilePathAndName,
                                       const AString& sceneIndicesText);
        
        SceneDataFileTreeItem* findFile(const AString& absoluteFilePathAndName);
        
        // ADD_NEW_METHODS_HERE

    private:
        std::map<AString, SceneDataFileTreeItem*> m_directoryToTreeItemMap;
        
        std::map<AString, SceneDataFileTreeItem*> m_fileNameToTreeItemMap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_DATA_FILE_TREE_ITEM_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_DATA_FILE_TREE_ITEM_MODEL_DECLARE__

} // namespace
#endif  //__SCENE_DATA_FILE_TREE_ITEM_MODEL_H__
