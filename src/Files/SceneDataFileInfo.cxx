
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

#define __SCENE_DATA_FILE_INFO_DECLARE__
#include "SceneDataFileInfo.h"
#undef __SCENE_DATA_FILE_INFO_DECLARE__

#include "CaretAssert.h"
#include "FileInformation.h"
using namespace caret;


    
/**
 * \class caret::SceneDataFileInfo 
 * \brief Information about data files in a scene file
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param absoluteDataFilePathAndName
 *     Absolute path and file name of the data file
 * @param absoluteBasePath
 *     The absolute base path
 * @param absoluteSceneFilePathAndName
 *     Absolute path and name of the scene file
 * @param sceneIndex
 *     Index of scene using this data file.
 */
SceneDataFileInfo::SceneDataFileInfo(const AString& absoluteDataFilePathAndName,
                                     const AString& absoluteBasePath,
                                     const AString& absoluteSceneFilePathAndName,
                                     const std::vector<int32_t>& sceneIndices)
: CaretObject()
{
    FileInformation fileInfo(absoluteDataFilePathAndName);
    m_remoteFlag  = fileInfo.isRemoteFile();
    if ( ! m_remoteFlag) {
        m_missingFlag = ( ! fileInfo.exists());
    }
    
    m_absolutePath = fileInfo.getAbsolutePath();
    
    m_dataFileName = fileInfo.getFileName();
    
    FileInformation sceneFileInfo(absoluteSceneFilePathAndName);
    m_relativePathToSceneFile = SystemUtilities::relativePath(m_absolutePath,
                                                          sceneFileInfo.getAbsolutePath());

    m_relativePathToBasePath = SystemUtilities::relativePath(m_absolutePath,
                                                             absoluteBasePath);
    
    m_sceneIndices.insert(sceneIndices.begin(),
                          sceneIndices.end());
}

/**
 * Destructor.
 */
SceneDataFileInfo::~SceneDataFileInfo()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SceneDataFileInfo::SceneDataFileInfo(const SceneDataFileInfo& obj)
: CaretObject(obj)
{
    this->copyHelperSceneDataFileInfo(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
SceneDataFileInfo&
SceneDataFileInfo::operator=(const SceneDataFileInfo& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperSceneDataFileInfo(obj);
    }
    return *this;    
}

/**
 * Less-than operator.
 *
 * @param rhs
 *     Other instance for comparison
 * @return 
 *     True if this instance is 'less-than' the other instance.
 */
bool
SceneDataFileInfo::operator<(const SceneDataFileInfo& rhs) const {
    if (m_absolutePath == rhs.m_absolutePath) {
        return (m_dataFileName < rhs.m_dataFileName);
    }
    return (m_absolutePath < rhs.m_absolutePath);
}

/**
 * Add a scene index that uses this data file.
 *
 * @param sceneIndex
 *     Additional scene index.
 */
void
SceneDataFileInfo::addSceneIndex(const int32_t sceneIndex) const
{
    m_sceneIndices.insert(sceneIndex);
}

/**
 * @return The scene indices as a string
 */
AString
SceneDataFileInfo::getSceneIndicesAsString() const
{
    std::vector<int32_t> indicesVector(m_sceneIndices.begin(),
                                       m_sceneIndices.end());
    return AString::fromNumbers(indicesVector, ",");
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
SceneDataFileInfo::copyHelperSceneDataFileInfo(const SceneDataFileInfo& obj)
{
    m_absolutePath            = obj.m_absolutePath;
    m_dataFileName            = obj.m_dataFileName;
    m_relativePathToBasePath  = obj.m_relativePathToBasePath;
    m_relativePathToSceneFile = obj.m_relativePathToSceneFile;
    m_remoteFlag              = obj.m_remoteFlag;
    m_missingFlag             = obj.m_missingFlag;
    m_sceneIndices            = obj.m_sceneIndices;
}

/**
 * @return
 */
AString
SceneDataFileInfo::getAbsolutePath() const
{
    return m_absolutePath;
}

/**
 * @return
 */
AString
SceneDataFileInfo::getAbsolutePathAndFileName() const
{
    if (m_absolutePath.isEmpty()) {
        return m_dataFileName;
    }
    const AString s(m_absolutePath
                    + "/"
                    + m_dataFileName);
    return s;
}

/**
 * @return
 */
AString
SceneDataFileInfo::getDataFileName() const
{
    return m_dataFileName;
}

/**
 * @return
 */
AString
SceneDataFileInfo::getRelativePathToBasePath() const
{
    return m_relativePathToBasePath;
}

/**
 * @return
 */
AString
SceneDataFileInfo::getRelativePathToSceneFile() const
{
    return m_relativePathToSceneFile;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SceneDataFileInfo::toString() const
{
    return "SceneDataFileInfo";
}

/**
 * @return True if the file is remote.
 */
bool
SceneDataFileInfo::isRemote() const
{
    return m_remoteFlag;
}

/**
 * @return True if this file is missing.
 * Note: Remote file is never missing.
 */
bool
SceneDataFileInfo::isMissing() const
{
    if (isRemote()) {
        return false;
    }
    
    return m_missingFlag;
}

/**
 * Sort a vector of SceneDataFileInfo objects using the given sort mode
 *
 * @param sceneDataFileInfo
 *     Vector of object that is sorted.
 * @param sortMode
 *     Mode for sorting.
 */
void
SceneDataFileInfo::sort(std::vector<SceneDataFileInfo>& sceneDataFileInfo,
                        const SortMode sortMode)
{
    switch (sortMode) {
        case SortMode::AbsolutePath:
            break;
        case SortMode::RelativeToBasePath:
            break;
        case SortMode::RelativeToSceneFilePath:
            break;
    }
    
    std::sort(sceneDataFileInfo.begin(),
              sceneDataFileInfo.end(),
              [&sortMode] (const SceneDataFileInfo& lhs, const SceneDataFileInfo& rhs) {
                  AString lhsPath;
                  AString rhsPath;
                  
                  switch (sortMode) {
                      case SortMode::AbsolutePath:
                          lhsPath = lhs.m_absolutePath;
                          rhsPath = rhs.m_absolutePath;
                          break;
                      case SortMode::RelativeToBasePath:
                          lhsPath = lhs.m_relativePathToBasePath;
                          rhsPath = rhs.m_relativePathToBasePath;
                          break;
                      case SortMode::RelativeToSceneFilePath:
                          lhsPath = lhs.m_relativePathToSceneFile;
                          rhsPath = rhs.m_relativePathToSceneFile;
                          break;
                  }
                  
                  if (lhsPath == rhsPath) {
                      return (lhs.m_dataFileName < rhs.m_dataFileName);
                  }
                  return (lhsPath < rhsPath);
              });
    
    /*print (sceneDataFileInfo);*/
}

/**
 * Print the vector of file information.
 *
 * @param sceneDataFileInfo
 *     Information that is printed.
 */
void
SceneDataFileInfo::print(const std::vector<SceneDataFileInfo>& sceneDataFileInfo)
{
    for (const auto& sdfi : sceneDataFileInfo) {
        std::cout << sdfi.getAbsolutePathAndFileName() << std::endl;
    }
}

