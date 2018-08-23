
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <QDir>

#define __SCENE_PATH_NAME_DECLARE__
#include "ScenePathName.h"
#undef __SCENE_PATH_NAME_DECLARE__

#include "CaretLogger.h"
#include "DataFile.h"
#include "FileInformation.h"
#include "Scene.h"

using namespace caret;


    
/**
 * \class caret::ScenePathName 
 * \brief For storage of a path name in a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 *
 * Stores a path name (file name) in a scene.
 * When the scene file is written, the a path relative
 * to the scene file's path is stored in the scene file.
 * When the path is read from the scene file, the path
 * is converted to an absolute path in memory.
 * Thus: In memory (RAM) the path is an absolute path
 * and when written to file (DISK), the path is relative
 * to the scene file.
 */

/**
 * Constructor.
 *
 * @param name
 *   Name of object.
 * @param value
 *   Value of object.
 */
ScenePathName::ScenePathName(const AString& name,
                             const AString& value)
: SceneObject(name,
              SceneObjectDataTypeEnum::SCENE_PATH_NAME)
{
    setValue(value);
}

ScenePathName::ScenePathName(const ScenePathName& rhs): SceneObject(rhs.getName(), SceneObjectDataTypeEnum::SCENE_PATH_NAME)
{
    m_value = rhs.m_value;
}

SceneObject* ScenePathName::clone() const
{
    return new ScenePathName(*this);
}

/**
 * Destructor.
 */
ScenePathName::~ScenePathName()
{
    
}

/**
 * Set the value.
 * @param value
 *   New value.
 */
void 
ScenePathName::setValue(const AString& value)
{
    m_value = value;
    
    if (DataFile::isFileOnNetwork(m_value)) {
        Scene::setSceneBeingCreatedHasFilesWithRemotePaths();
    }
}

/**
 * @return The value as a string data type.
 */
AString 
ScenePathName::stringValue() const
{
    m_restoredFlag = true;
    return m_value;
}

/**
 * Set the value.  If the value is not an absolute path, make the 
 * new value an absolute path assuming that the value is relative
 * to the scene file.
 * @param sceneFileName
 *    Name of scene file.
 * @param value
 *    New value.
 */
void 
ScenePathName::setValueToAbsolutePath(const AString& sceneFileName,
                                      const AString& value)
{    
    AString name = value;
    
    if (name.isEmpty() == false) {
        FileInformation sceneFileInfo(sceneFileName);
        if (sceneFileInfo.isAbsolute()) {
            if (DataFile::isFileOnNetwork(name) == false) {
                FileInformation fileInfo(name);
                if (fileInfo.isRelative()) {
                    FileInformation fileInfo(sceneFileInfo.getPathName(),
                                             name);
                    name = fileInfo.getAbsoluteFilePath();
                    name = FileInformation::cleanPath(name);
                }
            }
        }
        
//        const AString message = ("After converting TO absolute path, " 
//                                 + value
//                                 + " becomes " 
//                                 + name);
//        CaretLogFine(message);
    }
    
    m_value = name;
}

/**
 * Using the given scene file name, return a path relative to the scene file.
 * @param sceneFileName
 *    Name of scene file.
 * @return
 *    Value of this item (path) relative to the given scene file.
 */
AString 
ScenePathName::getRelativePathToSceneFile(const AString& sceneFileName) const
{
    AString name = m_value;
    
    if (name.isEmpty() == false) {
        FileInformation fileInfo(name);
        if (fileInfo.isAbsolute()) {
            FileInformation specFileInfo(sceneFileName);
            if (specFileInfo.isAbsolute()) {
                const AString newPath = SystemUtilities::relativePath(fileInfo.getPathName(),
                                                                      specFileInfo.getPathName());
                if (newPath.isEmpty()) {
                    name = fileInfo.getFileName();
                }
                else {
                    name = (newPath
                            + "/"
                            + fileInfo.getFileName());
                }
            }
        }
        
//        const AString message = ("After converting FROM absolute path, " 
//                                 + m_value
//                                 + " becomes " 
//                                 + name);
//        CaretLogFine(message);
    }
    
    return name;
}


