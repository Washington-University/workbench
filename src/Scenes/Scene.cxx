
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
 *
 * Each class that is to be saved to a scene must implement the
 * SceneableInterface which contains two virtual methods, one for
 * saving to the scene, saveScene(), and one for restoring from
 * the scene, restoreFromScene().  
 *
 * The saveToScene() method returns a SceneClass object.  In the saveToScene()
 * method, a SceneClass object is created and it's 'add' methods are used to 
 * add the values of members to the SceneClass object.  Add methods exist
 * for primitive data types (int, float, string, etc), arrays, 
 * the Caret enumerated types, and other classes.  Conversely, the restoreFromScene()
 * method receives a SceneClass object, and uses the SceneClass object's
 * 'get' methods to restore the values of the class' members.
 *
 * When restoring a class, most of the SceneClass' 'get' method contain
 * a default value parameter.  In some cases a 'get' method may be called
 * for a member that is not in the SceneClass object (perhaps a member was
 * added to the class since the scene was saved).  In these cases, the
 * default value will be returned by the 'get' method.
 *
 * To simplify this process a SceneClassAssistant can be used for 
 * members of a class that are not dynamically allocated.  An instance
 * of the SceneClassAssistant is created in the class' constructor.
 * Members of the class (actually the addresses of the members) are 
 * added to the SceneClassAssistant in the constructor.  When saving the
 * scene (in saveToScene), the SceneClassAssistant's saveMembers() method
 * is called and it will add the class's members to the SceneClass.  
 * In restoreFromScene(), the SceneClassAssistant's restoreMembers will
 * restore the class' members.
 * 
 * Some scene data involves saving paths to data files.  When a filename
 * is "in memory" an absolute path is used.  However, absolute paths
 * become a problem when data is transferred to other computers.  So,
 * for this special case, a ScenePathName object can be used.  The 
 * ScenePathName object contains a pathname that will be an absolute
 * pathname when in memory.  When written to a SceneFile, the pathname
 * will be converted to a relative path, relative to the SceneFile so
 * that the Scene will still function correctly when moved to a different
 * computer.
 *
 * Example of saving and restoring an instance of a class using
 * a SceneClassAssistant with three members, a boolean, a
 * Caret enumerated type, and a float.
 *
 * <p>
 * \code{.cpp}
 
SomeClass::SomeClass()
: SceneableInterface()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_contralateralIdentificationEnabled = false;
    m_identificationSymbolColor = CaretColorEnum::WHITE;
    m_identifcationSymbolSize = 3.0;
    
    m_sceneAssistant->add("m_contralateralIdentificationEnabled",
                          &m_contralateralIdentificationEnabled);
    
    m_sceneAssistant->add("m_identifcationSymbolSize",
                          &m_identifcationSymbolSize);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                                &m_identificationSymbolColor);
}

SceneClass*
SomeClass::SomeClass(const SceneAttributes* sceneAttributes,
                     const AString& instanceName)
{
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SomeClass",
                                            1);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
 
    return sceneClass;
}

SomeClass::SomeClass(const SceneAttributes* sceneAttributes,
                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
 
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
}

 * \endcode
 *
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
    m_hasFilesWithRemotePaths = false;
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

/**
 * @return description of scene
 */
AString
Scene::getDescription() const
{
    return m_sceneDescription;
}

/**
 * Set description of scene
 * @param sceneDescription
 *    New value for description of scene
 */
void
Scene::setDescription(const AString& sceneDescription)
{
    m_sceneDescription = sceneDescription;
}

/**
 * @return true if there are files with remote paths in the scene.
 */
bool
Scene::hasFilesWithRemotePaths() const
{
    return m_hasFilesWithRemotePaths;
}

/**
 * Set status of having files with remote paths.
 *
 * @param hasFilesWithRemotePaths
 *    New status.
 */
void
Scene::setHasFilesWithRemotePaths(const bool hasFilesWithRemotePaths)
{
    m_hasFilesWithRemotePaths = hasFilesWithRemotePaths;
}

/**
 * Set a static value for the scene that is being created.
 */
void
Scene::setSceneBeingCreated(Scene* scene)
{
    s_sceneBeingCreated = scene;
}

/**
 * Set the scene being created to have files with remote paths.
 */
void
Scene::setSceneBeingCreatedHasFilesWithRemotePaths()
{
    if (s_sceneBeingCreated != NULL) {
        s_sceneBeingCreated->m_hasFilesWithRemotePaths = true;
    }
}


