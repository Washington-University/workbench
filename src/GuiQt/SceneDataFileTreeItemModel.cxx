
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

#define __SCENE_DATA_FILE_TREE_ITEM_MODEL_DECLARE__
#include "SceneDataFileTreeItemModel.h"
#undef __SCENE_DATA_FILE_TREE_ITEM_MODEL_DECLARE__

#include <QDir>
#include <stack>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "FileInformation.h"
#include "SceneDataFileTreeItem.h"

using namespace caret;

static bool debugFlag(false);
    
/**
 * \class caret::SceneDataFileTreeItemModel 
 * \brief Tree item model for hierarchical list of files in a scene file
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param relativeToPath
 *     When the sortMode is a "relative to" mode, this contains that path that files are relative to.
 * @param sceneDataFileInfo
 *     The scene file info.
 * @param sortMode
 *     The sorting mode.
 */
SceneDataFileTreeItemModel::SceneDataFileTreeItemModel(const AString& /*relativeToPath */,
                                                       const std::vector<SceneDataFileInfo>& sceneDataFileInfo,
                                                       const SceneDataFileInfo::SortMode sortMode)
: QStandardItemModel()
{
    for (const auto dataFileInfo : sceneDataFileInfo) {
        AString pathName;
        AString pathAndFileName;
        bool relativePathFlag = false;
        switch (sortMode) {
            case SceneDataFileInfo::SortMode::AbsolutePath:
                pathName = dataFileInfo.getAbsolutePath();
                pathAndFileName = dataFileInfo.getAbsolutePathAndFileName();
                break;
            case SceneDataFileInfo::SortMode::RelativeToBasePath:
                pathName = dataFileInfo.getRelativePathToBasePath();
                pathAndFileName = (dataFileInfo.getRelativePathToBasePath()
                                   + "/"
                                   + dataFileInfo.getDataFileName());
                relativePathFlag = true;
                break;
            case SceneDataFileInfo::SortMode::RelativeToSceneFilePath:
                pathName = dataFileInfo.getRelativePathToSceneFile();
                pathAndFileName = (dataFileInfo.getRelativePathToSceneFile()
                                   + "/"
                                   + dataFileInfo.getDataFileName());
                relativePathFlag = true;
                break;
        }
        
        
        addFile(pathAndFileName,
                dataFileInfo.getSceneIndicesAsString());
    }
}

/**
 * Destructor.
 */
SceneDataFileTreeItemModel::~SceneDataFileTreeItemModel()
{
}

/**
 * Find or add all components in a directory path.
 * @param absoluteDirName
 *     Full path of directory.
 * @return
 *     Tree item containing directory with the given name.
 */
SceneDataFileTreeItem*
SceneDataFileTreeItemModel::addFindDirectoryPath(const AString& absoluteDirName)
{
    SceneDataFileTreeItem* directoryItemOut = findDirectory(absoluteDirName);
    
    if (directoryItemOut == NULL) {
        AString parentDirName;
        AString dirName;

        const AString httpsPrefix("https://");
        
        AString rootPrefix("/");
        AString nameForSplitting(absoluteDirName);
        if (absoluteDirName.startsWith(httpsPrefix)) {
            nameForSplitting = absoluteDirName.mid(httpsPrefix.length());
            rootPrefix = httpsPrefix;
        }
        
        QStringList components(nameForSplitting.split("/"));
        
        for (int32_t i = 0; i < components.length(); i++) {
            if (i == 0) {
                if (absoluteDirName.startsWith(rootPrefix)) {
                    dirName = rootPrefix;
                    addDirectory(dirName,
                                 parentDirName);
                }
            }
            
            parentDirName = dirName;
            
            if ( ! dirName.endsWith('/')) {
                dirName.append("/");
            }
            dirName.append(components.at(i));
            addDirectory(dirName,
                         parentDirName);
        }
        
        directoryItemOut = findDirectory(absoluteDirName);
        CaretAssert(directoryItemOut);
    }
    
    return directoryItemOut;
}


/**
 * Add a directory.  If directory already in tree, the existing item is returned.
 *
 * @param absoluteDirName
 *     Full path of directory.
 * @param absoluteParentDirName
 *     Full path of parent directory.
 * @return
 *     Tree item containing directory.
 */
SceneDataFileTreeItem*
SceneDataFileTreeItemModel::addDirectory(const AString& absoluteDirName,
                                         const AString& absoluteParentDirName)
{
    SceneDataFileTreeItem* directoryItemOut = findDirectory(absoluteDirName);
    
    if (directoryItemOut == NULL) {
        if (absoluteParentDirName.isEmpty()) {
            directoryItemOut = new SceneDataFileTreeItem(absoluteDirName,
                                                              absoluteDirName,
                                                              SceneDataFileTreeItem::getItemTypeDirectory());
            invisibleRootItem()->appendRow(directoryItemOut);
            m_directoryToTreeItemMap.insert(std::make_pair(absoluteDirName,
                                                           directoryItemOut));
            if (debugFlag) {
                std::cout << "Added root directory: " << absoluteDirName << std::endl;
            }
        }
        else {
            SceneDataFileTreeItem* parentDirItem = findDirectory(absoluteParentDirName);
            if (parentDirItem != NULL) {
                FileInformation dirInfo(absoluteDirName);
                const AString name = dirInfo.getFileName();
                directoryItemOut = new SceneDataFileTreeItem(name,
                                                             absoluteDirName,
                                                             SceneDataFileTreeItem::getItemTypeDirectory());
                parentDirItem->appendRow(directoryItemOut);
                
                m_directoryToTreeItemMap.insert(std::make_pair(absoluteDirName,
                                                               directoryItemOut));
                if (debugFlag) {
                    std::cout << "Added directory " << name << " to parent " << absoluteParentDirName << std::endl;
                }
            }
            else {
                CaretLogSevere("Unable to find parent directory named "
                               + absoluteParentDirName
                               + " for "
                               + absoluteDirName);
            }
        }
    }
    
    return directoryItemOut;
}

/**
 * Find a directory with the given directory name.
 *
 * @param absoluteDirName
 *     Full path including directory.
 * @return
 *     Tree item containing directory or NULL if not found.
 */
SceneDataFileTreeItem*
SceneDataFileTreeItemModel::findDirectory(const AString& absoluteDirName)
{
    SceneDataFileTreeItem* directoryItemOut = NULL;
    
    auto iter = m_directoryToTreeItemMap.find(absoluteDirName);
    if (iter != m_directoryToTreeItemMap.end()) {
        directoryItemOut = iter->second;
    }

    return directoryItemOut;
}

/**
 * Add a file.  If file already in tree, the existing item is returned.
 *
 * @param absoluteFilePathAndName
 *     Full path including directory and filename.
 * @param sceneIndicesText
 *     Text containing indices of scenes using file.
 * @return
 *     Tree item containing file.
 */
SceneDataFileTreeItem*
SceneDataFileTreeItemModel::addFile(const AString& absoluteFilePathAndName,
                                    const AString& sceneIndicesText)
{
    SceneDataFileTreeItem* fileItemOut = findFile(absoluteFilePathAndName);
    
    if (fileItemOut == NULL) {
        FileInformation fileInfo(absoluteFilePathAndName);
        SceneDataFileTreeItem* directoryItem = addFindDirectoryPath(fileInfo.getAbsolutePath());
        if (directoryItem != NULL) {
            AString itemText(fileInfo.getFileName());
            if ( ! sceneIndicesText.isEmpty()) {
                itemText.append(" ("
                                + sceneIndicesText
                                + ")");
            }
            fileItemOut = new SceneDataFileTreeItem(itemText,
                                                    absoluteFilePathAndName,
                                                    SceneDataFileTreeItem::getItemTypeFile());
            directoryItem->appendRow(fileItemOut);
            m_fileNameToTreeItemMap.insert(std::make_pair(absoluteFilePathAndName,
                                                          fileItemOut));
        }
        else {
            CaretLogSevere("Failed to find directory named "
                           + fileInfo.getAbsolutePath()
                           + " for inserting file named "
                           + fileInfo.getFileName());
        }
    }
    
    return fileItemOut;
}

/**
 * Find a file with the given filename.
 *
 * @param absoluteFilePathAndName
 *     Full path including directory and filename.
 * @return
 *     Tree item containing file or NULL if not found.
 */
SceneDataFileTreeItem*
SceneDataFileTreeItemModel::findFile(const AString& absoluteFilePathAndName)
{
    SceneDataFileTreeItem* fileItemOut = NULL;
    
    auto iter = m_fileNameToTreeItemMap.find(absoluteFilePathAndName);
    if (iter != m_fileNameToTreeItemMap.end()) {
        fileItemOut = iter->second;
    }
    
    return fileItemOut;
}


