
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
 * @param sceneFilePathAndName
 *     Scene file path and name.
 * @param baseDirectoryPath
 *     Path of the base directory.
 * @param sceneDataFileInfo
 *     The scene file info.
 * @param sortMode
 *     The sorting mode.
 */
SceneDataFileTreeItemModel::SceneDataFileTreeItemModel(const AString& sceneFilePathAndName,
                                                       const AString& baseDirectoryPath,
                                                       const std::vector<SceneDataFileInfo>& sceneDataFileInfo,
                                                       const SceneDataFileInfo::SortMode sortMode)
: QStandardItemModel()
{
    if ( ! baseDirectoryPath.isEmpty()) {
        /*
         * Add a root element to the tree containing the base path
         */
        addDirectory(baseDirectoryPath,
                     "");
    }
    
    if ( ! sceneFilePathAndName.isEmpty()) {
        /*
         * Add the scene file and highlight the name to make it stand out from other files
         */
        SceneDataFileTreeItem* sceneFileItem = addFile(sceneFilePathAndName,
                                                       "");
        CaretAssert(sceneFileItem);
        QFont font = sceneFileItem->font();
        font.setBold(true);
        sceneFileItem->setFont(font);
    }
    
    /*
     * Add the data files
     */
    for (const auto dataFileInfo : sceneDataFileInfo) {
        AString pathName;
        AString pathAndFileName;
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
                break;
            case SceneDataFileInfo::SortMode::RelativeToSceneFilePath:
                pathName = dataFileInfo.getRelativePathToSceneFile();
                pathAndFileName = (dataFileInfo.getRelativePathToSceneFile()
                                   + "/"
                                   + dataFileInfo.getDataFileName());
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
        const AString httpsPrefix("https://");
        
        AString rootPrefix("/");
        AString nameForSplitting(absoluteDirName);
        if (absoluteDirName.startsWith(httpsPrefix)) {
            nameForSplitting = absoluteDirName.mid(httpsPrefix.length());
            rootPrefix = httpsPrefix;
        }
        
        QStringList components(nameForSplitting.split("/",
                                                      QString::SkipEmptyParts));
        const int32_t componentCount = components.length();

        std::vector<AString> parentDirectoryHierarchy;
        
        /*
         * Create a vector containing all directory paths
         * in the hierarchy as absolute paths.  This is necessary
         * to avoid duplicating a hierarchy that matches the
         * base path.
         */
        AString dirName;
        AString parentDirName;
        for (int32_t i = 0; i < componentCount; i++) {
            if (i == 0) {
                if (absoluteDirName.startsWith(rootPrefix)) {
                    dirName = rootPrefix;
                    parentDirectoryHierarchy.push_back(dirName);
                }
            }
            
            parentDirName = dirName;
            
            if ( ! dirName.endsWith('/')) {
                dirName.append("/");
            }
            dirName.append(components.at(i));
            parentDirectoryHierarchy.push_back(dirName);
        }
        
        if (debugFlag) {
            std::cout << "Parent directory hierarchy: " << std::endl;
            for (const auto s : parentDirectoryHierarchy) {
                std::cout << "    " << s << std::endl;
            }
        }
        
        /*
         * Start at the deepest directory path and work way up
         * to find the deepest existing path.
         */
        int32_t iStart(0);
        const int32_t numDirs = static_cast<int32_t>(parentDirectoryHierarchy.size());
        for (int32_t iDir = (numDirs - 1); iDir >= 0; iDir--) {
            CaretAssertVectorIndex(parentDirectoryHierarchy, iDir);
            SceneDataFileTreeItem* dirItem = findDirectory(parentDirectoryHierarchy[iDir]);
            if (dirItem != NULL) {
                iStart = iDir + 1;
                break;
            }
        }
        if (iStart < numDirs) {
            /*
             * Create directories that are children of the deepest existing path
             */
            for (int32_t iDir = iStart; iDir < numDirs; iDir++) {
                AString parentDirName;
                if (iDir > 0) {
                    CaretAssertVectorIndex(parentDirectoryHierarchy, iDir - 1);
                    parentDirName = parentDirectoryHierarchy[iDir - 1];
                }
                CaretAssertVectorIndex(parentDirectoryHierarchy, iDir);
                if (debugFlag) {
                    std::cout << "Creating directory " << parentDirectoryHierarchy[iDir]
                    << " with parent " << parentDirName << std::endl;
                }
                addDirectory(parentDirectoryHierarchy[iDir],
                             parentDirName);
            }
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
        /*
         * First look for parent directory, which may be the base directory.
         */
        SceneDataFileTreeItem* directoryItem = findDirectory(fileInfo.getAbsolutePath());
        if (directoryItem == NULL) {
            /*
             * Start looking/adding directory from root down to file's parent
             */
            directoryItem = addFindDirectoryPath(fileInfo.getAbsolutePath());
        }
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


