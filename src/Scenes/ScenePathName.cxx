
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __SCENE_PATH_NAME_DECLARE__
#include "ScenePathName.h"
#undef __SCENE_PATH_NAME_DECLARE__

#include "CaretLogger.h"
#include "FileInformation.h"

using namespace caret;


    
/**
 * \class caret::ScenePathName 
 * \brief For storage of a path name in a scene.
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
    m_value = value;
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
}

/**
 * @return The value as a string data type.
 */
AString 
ScenePathName::stringValue() const
{
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
            FileInformation fileInfo(name);
            if (fileInfo.isRelative()) {
                FileInformation fileInfo(sceneFileInfo.getPathName(),
                                         name);
                name = fileInfo.getFilePath();
            }
        }
        
        const AString message = ("After converting TO absolute path, " 
                                 + value
                                 + " becomes " 
                                 + name);
        CaretLogFine(message);
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
        
        const AString message = ("After converting FROM absolute path, " 
                                 + m_value
                                 + " becomes " 
                                 + name);
        CaretLogFine(message);
    }
    
    return name;
}


