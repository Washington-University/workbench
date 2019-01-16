
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

#define __SCENE_OBJECT_DECLARE__
#include "SceneObject.h"
#undef __SCENE_OBJECT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::SceneObject 
 * \brief Abstract class for any item saved to a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of the item.
 * @param containerType
 *    Type of container of object (array, map, single).
 * @param dataType
 *    Data type of the primitive.
 */
SceneObject::SceneObject(const QString& name,
                         const SceneObjectContainerTypeEnum::Enum containerType,
                         const SceneObjectDataTypeEnum::Enum dataType)
#ifdef CARET_SCENE_DEBUG
: CaretObject(), m_name(name), m_containerType(containerType), m_dataType(dataType), m_restoredFlag(false)
#else   // CARET_SCENE_DEBUG
: m_name(name), m_containerType(containerType), m_dataType(dataType), m_restoredFlag(false)
#endif  // CARET_SCENE_DEBUG
{
    CaretAssert(name.isEmpty() == false);
}

/**
 * Destructor.
 */
SceneObject::~SceneObject()
{
    
}

/**
 * @return Name of the item
 */
QString
SceneObject::getName() const
{
    return m_name;
}

/**
 * @return Type of container of object (array, map, single)
 */
SceneObjectContainerTypeEnum::Enum
SceneObject::getContainerType() const
{
    return m_containerType;
}

/**
 * @return Data type of the object.
 */
SceneObjectDataTypeEnum::Enum 
SceneObject::getDataType() const
{
    return m_dataType;
}

/**
 * @return Has this instance been restored?
 */
bool
SceneObject::isRestored() const
{
    return m_restoredFlag;
}

/**
 * Set this instance restored.
 *
 * @param restoredFlag
 *     New restored status
 */
void
SceneObject::setRestored(const bool restoredFlag) const
{
    m_restoredFlag = restoredFlag;
}

/**
 * Set this instance and all descendants restored.
 *
 * @param restoredFlag
 *     New restored status
 */
void
SceneObject::setDescendantsRestored(const bool restoredFlag) const
{
    setRestored(restoredFlag);
    std::vector<SceneObject*> descendants = getDescendants();
    setRestoredStatus(descendants, restoredFlag);
}

/**
 * Set the restored status for all scene objects.
 *
 * @param sceneObject
 *     Scene objects whose restored status is set.
 * @param restoredStatus
 *     New status applied to all scene object.
 */
void
SceneObject::setRestoredStatus(std::vector<SceneObject*> sceneObjects,
                               const bool restoredStatus)
{
    for (std::vector<SceneObject*>::iterator iter = sceneObjects.begin();
         iter != sceneObjects.end();
         iter++) {
        (*iter)->m_restoredFlag = restoredStatus;
    }
}

/**
 * If scene objects failed to restore, log at the warning level.
 *
 * @param sceneName
 *     Name of the scene.
 * @param sceneObjects
 *     Scene objects that are verified restored.
 */
void
SceneObject::logObjectsFailedRestore(const AString& sceneName,
                                     const std::vector<SceneObject*> sceneObjects)
{
    int64_t failCount = 0;
    
    AString msg;
    
    for (std::vector<SceneObject*>::const_iterator iter = sceneObjects.begin();
         iter != sceneObjects.end();
         iter++) {
        if ( ! (*iter)->m_restoredFlag) {
            failCount++;
            
            if (failCount < 25) {
                msg.appendWithNewLine("    "
                                      + (*iter)->toString());
            }
        }
    }
    
    if (failCount > 0) {
        CaretLogWarning(sceneName
                        + "\n"
                        "Failed to restore "
                        + AString::number(failCount)
                        + " scene objects.\n"
                        + msg);
    }
}


/**
 * Get all descendants of this instance.  This method should be
 * overridden by sub-classes that have children.
 *
 * @return All descendant SceneClasses (children, grandchildren, etc.) of this instance.
 */
std::vector<SceneObject*>
SceneObject::getDescendants() const
{
    std::vector<SceneObject*> descendants;
    return descendants;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SceneObject::toString() const
{
    QString objectInfo("Scene Object Type="
                       + SceneObjectDataTypeEnum::toName(m_dataType)
                       + " name="
                       + m_name);
    return objectInfo;
}

/**
 * Cast an instance of SceneObject to a SceneClass.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
const SceneClass*
SceneObject::castToSceneClass() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObject to a SceneEnumeratedType.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneEnumeratedType
 */
const SceneEnumeratedType*
SceneObject::castToSceneEnumeratedType() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObject to a SceneObjectArray.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneObjectArray
 */
const SceneObjectArray*
SceneObject::castToSceneObjectArray() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObject to a SceneObjectMapIntegerKey.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneObjectMapIntegerKey
 */
const SceneObjectMapIntegerKey*
SceneObject::castToSceneObjectMapIntegerKey() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObject to a ScenePathName.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is ScenePathName
 */
const ScenePathName*
SceneObject::castToScenePathName() const
{
    return NULL;
}

/**
 * Cast an instance of SceneObject to a ScenePrimitive.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is ScenePrimitive
 */
const ScenePrimitive*
SceneObject::castToScenePrimitive() const
{
    return NULL;
}
