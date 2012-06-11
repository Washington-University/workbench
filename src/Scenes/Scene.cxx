
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

#define __SCENE_DECLARE__
#include "Scene.h"
#undef __SCENE_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::Scene 
 * \brief Contains a scene which is the saved state of workbench.
 *
 * Contains the state of workbench that can be restored.  This is
 * similar to Memento design pattern or Java's Serialization.
 */

/**
 * Constructor.
 * @param sceneType
 *    Type of scene.
 */
Scene::Scene(const SceneTypeEnum::Enum sceneType)
: CaretObject()
{
    m_sceneAttributes = new SceneAttributes(sceneType);
}

/**
 * Destructor.
 */
Scene::~Scene()
{
    delete m_sceneAttributes;

    const int32_t numberOfSceneClasses = this->getNumberOfClasses();
    for (int32_t i = 0; i < numberOfSceneClasses; i++) {
        delete m_sceneClasses[i];
    }
    m_sceneClasses.clear();
}

/**
 * @return Attributes of the scene
 */
const SceneAttributes*
Scene::getAttributes() const
{
    return m_sceneAttributes;
}

/**
 * @return Attributes of the scene
 */
SceneAttributes*
Scene::getAttributes()
{
    return m_sceneAttributes;
}

void 
Scene::addClass(SceneClass* sceneClass)
{
    if (sceneClass != NULL) {
        m_sceneClasses.push_back(sceneClass);
    }
}


/**
 * @return Number of classes contained in the scene
 */
int32_t 
Scene::getNumberOfClasses() const
{
    return m_sceneClasses.size();
}

/**
 * Get the scene class at the given index.
 * @param indx
 *    Index of the scene class.
 * @return Scene class at the given index.
 */
const SceneClass* 
Scene::getClassAtIndex(const int32_t indx) const
{
    CaretAssertVectorIndex(m_sceneClasses, indx);
    return m_sceneClasses[indx];
}

/**
 * Get the scene class with the given name.
 * @param sceneClassName
 *    Name of the scene class.
 * @return Scene class with the given name or NULL if not found.
 */
const SceneClass* 
Scene::getClassWithName(const AString& sceneClassName) const
{
    const int32_t numberOfSceneClasses = this->getNumberOfClasses();
    for (int32_t i = 0; i < numberOfSceneClasses; i++) {
        if (m_sceneClasses[i]->getName() == sceneClassName) {
            return m_sceneClasses[i];
        }
    }
    
    return NULL;
}

/**
 * @return name of scene
 */
AString
Scene::getName() const
{
    return m_sceneName;
}

/**
 * Set name of scene
 * @param sceneName
 *    New value for name of scene
 */
void
Scene::setName(const AString& sceneName)
{
    m_sceneName = sceneName;
}


