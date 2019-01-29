
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

#define __SCENE_DECLARE__
#include "Scene.h"
#undef __SCENE_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneInfo.h"
#include "WuQMacroGroup.h"

using namespace caret;

/**
 * \defgroup Scene  
 */

    
/**
 * \class caret::Scene 
 * \brief Contains a scene which is the saved state of workbench.
 * \ingroup Scene
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
 * the Caret enumerated types, and child classes.  Conversely, the restoreFromScene()
 * method receives a SceneClass object, and uses the SceneClass object's
 * 'get' methods to restore the values of the class' members.
 *
 * When restoring a class, most of the SceneClass' 'get' methods contain
 * a default value parameter.  In some cases a 'get' method may be called
 * for a member that is not in the SceneClass object (perhaps a member was
 * added to the class since the scene was saved).  In these cases, the
 * default value will be returned by the 'get' method.
 *
 * To simplify this process, a SceneClassAssistant can be used for 
 * members of a class that are not dynamically allocated.  An instance
 * of the SceneClassAssistant is created in the class' constructor.
 * Members of the class (actually the addresses of the members) are 
 * added to the SceneClassAssistant in the constructor's code.  When saving the
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
 * a SceneClassAssistant with four members, a boolean, a
 * Caret enumerated type, a float, and a child class.
 *
 * <p>
 * \code
 
SomeClass::SomeClass()
: SceneableInterface()
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_childClass = new ChildClass();
    m_contralateralIdentificationEnabled = false;
    m_identificationSymbolColor = CaretColorEnum::WHITE;
    m_identifcationSymbolSize = 3.0;
    
    m_sceneAssistant->add("m_contralateralIdentificationEnabled",
                          &m_contralateralIdentificationEnabled);
    
    m_sceneAssistant->add("m_identifcationSymbolSize",
                          &m_identifcationSymbolSize);
    
    m_sceneAssistant->add<CaretColorEnum, CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                                &m_identificationSymbolColor);
    m_sceneAssistant->add("m_childClass"
                          "ChildClass",
                          m_childClass);
}

SceneClass*
SomeClass::saveToScene(const SceneAttributes* sceneAttributes,
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

SomeClass::restoreFromScene(const SceneAttributes* sceneAttributes,
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
 *
 * Example of saving and restoring an instance of a class without a 
 * SceneAssistant.  Note that a class may save/restore members using a
 * SceneClassAssistant for some members and explicitly saving/restoring
 * members in both saveToScene() and restoreFromScene().
 * 
 * \code

SceneClass*
SomeClass::saveToScene(const SceneAttributes* sceneAttributes,
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
    
    sceneClass->addBoolean("m_contralateralIdentificationEnabled",
                           m_contralateralIdentificationEnabled);
    sceneClass->addFloat("m_identifcationSymbolSize",
                         m_identifcationSymbolSize);
    sceneClass->addEnumeratedType<CaretColorEnum,CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                  m_identificationSymbolColor);
    sceneClass->addClass(m_childClass->saveToScene(sceneAttributes,
                                                   "m_childClass"));
    
    return sceneClass;
}

SomeClass::restoreFromScene(const SceneAttributes* sceneAttributes,
                            const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_contralateralIdentificationEnabled = sceneClass->getBooleanValue("m_contralateralIdentificationEnabled",
                                                                       false);
    m_identifcationSymbolSize = sceneClass->getFloatValue("m_identifcationSymbolSize",
                                                          2.0);
    m_identificationSymbolColor = sceneClass->getEnumeratedTypeValue<CaretColorEnum,CaretColorEnum::Enum>("m_identificationSymbolColor",
                                                                                     CaretColorEnum::WHITE);
    m_childClass->restoreFromScene(sceneAttributes,
                                   scene->getClass("m_childClass"));
}

 * \endcode
 *
 *
 * Approximate XML produced by above code serializing its members
 * including the child class.
 *
 * \code
 <Object Type="class"
     Class="SomeClass"
     Name="m_someClass"
     Version="1">
        <Object Type="boolean" Name="m_contralateralIdentificationEnabled">false</Object>
        <Object Type="float" Name="m_identifcationSymbolSize">3</Object>
        <Object Type="enumeratedType" Name="m_identificationSymbolColor"><![CDATA[WHITE]]></Object>
        <Object Type="class"
           Class="ChildClass"
           Name="m_childClass"
           Version="1">
               <Object Type="integer" Name="m_surfaceNumberOfNodes">32492</Object>
               <Object Type="integer" Name="m_nodeIndex">3096</Object>
        </Object>
 </Object>
 
 * \endcode
 */


/**
 * Constructor.
 * @param sceneType
 *    Type of scene.
 */
Scene::Scene(const SceneTypeEnum::Enum sceneType)
:CaretObjectTracksModification()
{
    m_sceneAttributes = new SceneAttributes(sceneType,
                                            this);
    m_hasFilesWithRemotePaths = false;
    m_sceneInfo = new SceneInfo();

    static int counter = 1;
    const AString macroGroupName("SceneFile_"
                                 + AString::number(counter));
    m_macroGroup.reset(new WuQMacroGroup(macroGroupName));
    m_macroGroup->clearModified();
}

Scene::Scene(const Scene& rhs)
:CaretObjectTracksModification()
{
    m_sceneAttributes = new SceneAttributes(*(rhs.m_sceneAttributes));
    m_hasFilesWithRemotePaths = rhs.m_hasFilesWithRemotePaths;
    m_sceneInfo = new SceneInfo(*(rhs.m_sceneInfo));
    for (std::vector<SceneClass*>::const_iterator iter = rhs.m_sceneClasses.begin(); iter != rhs.m_sceneClasses.end(); ++iter)
    {
        m_sceneClasses.push_back(new SceneClass(**iter));
    }
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
    
    delete m_sceneInfo;
}

/**
 * @return All descendant SceneClasses (children, grandchildren, etc.) of this instance.
 */
std::vector<SceneObject*>
Scene::getDescendants() const
{
    std::vector<SceneObject*> descendants;
    
    const int32_t numberOfSceneClasses = this->getNumberOfClasses();
    for (int32_t i = 0; i < numberOfSceneClasses; i++) {
        SceneObject* object = m_sceneClasses[i];
        descendants.push_back(object);
        
        std::vector<SceneObject*> objectDescendants = object->getDescendants();
        descendants.insert(descendants.end(),
                           objectDescendants.begin(),
                           objectDescendants.end());
    }
    
    return descendants;
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
        setModified();
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
    m_sceneClasses[indx]->setRestored(true);
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
            m_sceneClasses[i]->setRestored(true);
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
    return m_sceneInfo->getName();
}

/**
 * Set name of scene
 * @param sceneName
 *    New value for name of scene
 */
void
Scene::setName(const AString& sceneName)
{
    m_sceneInfo->setName(sceneName);
    m_macroGroup->setName("Scene: "
                          + sceneName);
}

/**
 * @return The BALSA Scene ID.
 */
AString
Scene::getBalsaSceneID() const
{
    return m_sceneInfo->getBalsaSceneID();
}

/**
 * Set the BALSA Scene ID.
 *
 * @param balsaSceneID
 *     The BALSA scene ID.
 */
void
Scene::setBalsaSceneID(const AString& balsaSceneID)
{
    m_sceneInfo->setBalsaSceneID(balsaSceneID);
}


/**
 * @return description of scene
 */
AString
Scene::getDescription() const
{
    return m_sceneInfo->getDescription();
}

/**
 * Set description of scene
 * @param sceneDescription
 *    New value for description of scene
 */
void
Scene::setDescription(const AString& sceneDescription)
{
    m_sceneInfo->setDescription(sceneDescription);
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

/**
 * @return The scene info (name, description, etc.) for this scene.
 */
SceneInfo*
Scene::getSceneInfo()
{
    return m_sceneInfo;
}

/**
 * @return The scene info (name, description, etc.) for this scene.
 * Const method.
 */
const SceneInfo*
Scene::getSceneInfo() const
{
    return m_sceneInfo;
}

/**
 * Set the scene info.  Ownership of the pointer to SceneInfo is taken
 * by this Scene object and the Scene object will delete the pointer
 * at the appropriate time (replaced or Scene is deleted).
 */
void
Scene::setSceneInfo(SceneInfo* sceneInfo)
{
    CaretAssert(sceneInfo);
    
    if (m_sceneInfo != NULL) {
        delete m_sceneInfo;
    }
    m_sceneInfo = sceneInfo;
    setModified();
}

/**
 * @return True if this scene is modified.
 */
bool
Scene::isModified() const
{
    if (CaretObjectTracksModification::isModified()) {
        return true;
    }
    if (m_sceneInfo->isModified()) {
        return true;
    }
    
    if (m_macroGroup->isModified()) {
        return true;
    }

    return false;
}

/**
 * Clear the modified status of this scene.
 */
void
Scene::clearModified()
{
    CaretObjectTracksModification::clearModified();
    m_sceneInfo->clearModified();
    m_macroGroup->clearModified();
}

/**
 * @return The macro group
 */
WuQMacroGroup*
Scene::getMacroGroup()
{
    return m_macroGroup.get();
}

/**
 * @return The macro group (const method)
 */
const WuQMacroGroup*
Scene::getMacroGroup() const
{
    return m_macroGroup.get();
}



