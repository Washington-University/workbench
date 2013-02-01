
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

#define __SCENE_ATTRIBUTES_DECLARE__
#include "SceneAttributes.h"
#undef __SCENE_ATTRIBUTES_DECLARE__

using namespace caret;


    
/**
 * \class caret::SceneAttributes 
 * \brief Attributes of a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 */
SceneAttributes::SceneAttributes(const SceneTypeEnum::Enum sceneType)
: CaretObject(), m_sceneType(sceneType)
{
    m_restoreWindowBehavior = RESTORE_WINDOW_POSITION_RELATIVE_TO_FIRST_AND_USE_SIZES;
    m_includeSpecFileNameInScene = true;
}

/**
 * Destructor.
 */
SceneAttributes::~SceneAttributes()
{
    
}

/**
 * @return The type of scene.
 */
SceneTypeEnum::Enum 
SceneAttributes::getSceneType() const
{
    return m_sceneType;
}

/**
 * Set the indices of the tabs that are to be saved to the scene.
 * @param tabIndices
 *    Indices of tabs that are saved to the scene.
 */
void 
SceneAttributes::setIndicesOfTabsForSavingToScene(const std::vector<int32_t>& tabIndices)
{
    m_indicesOfTabsForSavingToScene = tabIndices;
}

/**
 * @return Indices of tabs for saving to the scene.
 */
std::vector<int32_t> 
SceneAttributes::getIndicesOfTabsForSavingToScene() const
{
    return m_indicesOfTabsForSavingToScene;
}

/**
 * @return Name of the scene file.
 * May only have a value when restoring.
 */
AString 
SceneAttributes::getSceneFileName() const
{
    return m_sceneFileName;
}

/**
 * Set the name of the scene file containing the scene
 * that is being restored.
 * @param sceneFileName
 *    Name of scene file.
 */
void 
SceneAttributes::setSceneFileName(const AString& sceneFileName)
{
    m_sceneFileName = sceneFileName;
}

/**
 * @return The window restoration behavior.
 */
SceneAttributes::RestoreWindowBehavior 
SceneAttributes::getRestoreWindowBehavior() const
{
    return m_restoreWindowBehavior;
}

/**
 * Set the window restoration behavior.
 * @param rwb
 *   New value for window restoration behavior.
 */
void 
SceneAttributes::setWindowRestoreBehavior(const RestoreWindowBehavior rwb)
{
    m_restoreWindowBehavior = rwb;
}

/**
 * @return true if spec file name is included in scene.
 */
bool 
SceneAttributes::isSpecFileNameIncludedInScene() const
{
    return m_includeSpecFileNameInScene;
}

/**
 * Set spec file name included in scene.
 * @param status
 *    New status of spec file name included in scene.
 */
void 
SceneAttributes::setSpecFileNameIncludedInScene(const bool status)
{
    m_includeSpecFileNameInScene = status;
}

/**
 * Add a new message to the error message.  Each message is
 * separated by a newline.
 *
 * @param message
 *    New message added to the error message.
 */
void 
SceneAttributes::addToErrorMessage(const AString& message) const
{
    if (message.isEmpty()) {
        return;
    }
    
    if (m_errorMessage.isEmpty() == false) {
        m_errorMessage += "\n";
    }
    m_errorMessage += message;
}

/**
 * @return The error message.
 */
AString 
SceneAttributes::getErrorMessage() const
{
    return m_errorMessage;
}


