
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

#define __SCENE_OBJECT_MAP_STRING_KEY_DECLARE__
#include "SceneObjectMapStringKey.h"
#undef __SCENE_OBJECT_MAP_STRING_KEY_DECLARE__

#include "CaretAssert.h"
#include "SceneBoolean.h"
#include "SceneClass.h"
#include "SceneEnumeratedType.h"
#include "SceneFloat.h"
#include "SceneInteger.h"
#include "SceneLongInteger.h"
#include "ScenePathName.h"
#include "ScenePrimitive.h"
#include "SceneString.h"
#include "SceneUnsignedByte.h"

#include <utility>

using namespace caret;


    
/**
 * \class caret::SceneObjectMapStringKey 
 * \brief Map for saving data to a scene using a string as the key.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 *
 * @param name
 *    Name of map.
 * @param valueDataType
 *    Type of data stored in the map.  Any items added MUST match
 *    the data type passed to the constructor.  Assertions will
 *    check for this condition.
 */
SceneObjectMapStringKey::SceneObjectMapStringKey(const QString& name,
                                       const SceneObjectDataTypeEnum::Enum valueDataType)
: SceneObject(name,
              SceneObjectContainerTypeEnum::MAP_STRING_KEY,
              valueDataType)
{
    
}

/**
 * Destructor.
 */
SceneObjectMapStringKey::~SceneObjectMapStringKey()
{
    for (DATA_MAP_CONST_ITERATOR iter = m_dataMap.begin();
         iter != m_dataMap.end();
         iter++) {
        delete iter->second;
    }
}

/**
 * Cast an instance of SceneObject to a SceneObjectMapStringKey.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneObjectMapStringKey
 */
SceneObjectMapStringKey*
SceneObjectMapStringKey::castToSceneObjectMapStringKey()
{
    return this;
}

/**
 * Cast an instance of SceneObject to a SceneObjectMapStringKey.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneObjectMapStringKey
 */
const SceneObjectMapStringKey*
SceneObjectMapStringKey::castToSceneObjectMapStringKey() const
{
    return this;
}

/**
 * @return True if this map's content is empty, else false.
 */
bool
SceneObjectMapStringKey::isEmpty() const
{
    return m_dataMap.empty();
}

/**
 * @return All descendant SceneClasses (children, grandchildren, etc.) of this instance.
 */
std::vector<SceneObject*>
SceneObjectMapStringKey::getDescendants() const
{
    std::vector<SceneObject*> descendants;
    
    for (DATA_MAP_CONST_ITERATOR iter = m_dataMap.begin();
         iter != m_dataMap.end();
         iter++) {
        SceneObject* sceneObject = iter->second;
        descendants.push_back(sceneObject);
        
        std::vector<SceneObject*> objectDescendants = sceneObject->getDescendants();
        descendants.insert(descendants.end(),
                           objectDescendants.begin(),
                           objectDescendants.end());
    }
    
    return descendants;
}

/**
 * Add the given boolean value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapStringKey::addBoolean(const AString& key,
         const bool value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_BOOLEAN);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneBoolean("b", value)));
}

/**
 * Add the given integer value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapStringKey::addInteger(const AString& key,
         const int32_t value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_INTEGER);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneInteger("i", value)));
}

/**
 * Add the given integer value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void
SceneObjectMapStringKey::addLongInteger(const AString& key,
                                         const int64_t value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_LONG_INTEGER);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneLongInteger("i", value)));
}


/**
 * Add the given float value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void
SceneObjectMapStringKey::addFloat(const AString& key,
         const float value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_FLOAT);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneFloat("f", value)));
}

/**
 * Add the given class value to the map using the given key.
 * This map will take ownership of the class.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapStringKey::addClass(const AString& key,
         SceneClass* value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_CLASS);
    m_dataMap.insert(std::make_pair(key, value));
}

/**
 * Add the given string value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapStringKey::addString(const AString& key,
                        const AString& value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_STRING);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneString("s", value)));
}

/**
 * Add the given path name value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapStringKey::addPathName(const AString& key,
                                     const AString& value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_PATH_NAME);
    m_dataMap.insert(std::make_pair(key,
                                    new ScenePathName("s", value)));
}

/**
 * Add the given unsigned byte value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void
SceneObjectMapStringKey::addUnsignedByte(const AString& key,
                                          const uint8_t value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneUnsignedByte("u", value)));
}

/**
 * Add the given enumerated type value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapStringKey::addEnumeratedType(const AString& key,
                        const AString& value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneEnumeratedType("e", value)));
}

/**
 * Find the SceneObject with the given key.
 * Key MUST be valid.
 *
 * @param key
 *    The key.
 * @return
 *    Object at the given key.
 */
const SceneObject* 
SceneObjectMapStringKey::getObject(const AString& key) const
{    
    const DATA_MAP_CONST_ITERATOR iter = m_dataMap.find(key);
    CaretAssert(iter != m_dataMap.end());
    const SceneObject* object = iter->second;
    CaretAssert(object);
    object->m_restoredFlag = true;
    return object;
}

/** 
 * Get the value as a boolean. 
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
bool 
SceneObjectMapStringKey::booleanValue(const AString& key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->booleanValue();
}

/** 
 * Get the value as a float. 
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
float 
SceneObjectMapStringKey::floatValue(const AString& key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->floatValue();
}

/** 
 * Get the value as a integer. 
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
int32_t 
SceneObjectMapStringKey::integerValue(const AString& key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->integerValue();
}

/**
 * Get the value as a long integer.
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
int64_t
SceneObjectMapStringKey::longIntegerValue(const AString& key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->longIntegerValue();
}
/**
 * Get the value as a string. 
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
AString 
SceneObjectMapStringKey::stringValue(const AString& key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->stringValue();
}

/** 
 * Get the path name as a string. 
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
AString 
SceneObjectMapStringKey::pathNameValue(const AString& key) const
{
    const ScenePathName* pathName = dynamic_cast<const ScenePathName*>(getObject(key));
    CaretAssert(pathName);
    return pathName->stringValue();
}

/** 
 * Get the value as a class. 
 *
 * @param key
 *    key of element.
 * @return The class with the given key.
 */
const SceneClass* 
SceneObjectMapStringKey::classValue(const AString& key) const
{
    const SceneClass* sceneClass = dynamic_cast<const SceneClass*>(getObject(key));
    CaretAssert(sceneClass);
    return sceneClass;
}

/** 
 * Get the value as a enumerated type stirng. 
 *
 * @param key
 *    key of element.
 * @return The enumerated type value with the given key.
 */
AString 
SceneObjectMapStringKey::enumeratedTypeValue(const AString& key) const
{
    const SceneEnumeratedType* enumType = dynamic_cast<const SceneEnumeratedType*>(getObject(key));
    CaretAssert(enumType);
    return enumType->stringValue();
}

/**
 * @return A vector containg all of the keys in the map.
 */
std::vector<AString>
SceneObjectMapStringKey::getKeys() const
{
    std::vector<AString> theKeys;
    theKeys.reserve(m_dataMap.size());
    
    for (DATA_MAP_CONST_ITERATOR iter = m_dataMap.begin();
         iter != m_dataMap.end();
         iter++) {
        theKeys.push_back(iter->first);
    }
    
    return theKeys;
}

/**
 * @return An iterator for all values in
 * the map.
 */
const std::map<AString, SceneObject*>&
SceneObjectMapStringKey::getMap() const
{
    return m_dataMap;
}

SceneObject* SceneObjectMapStringKey::clone() const
{
    SceneObjectMapStringKey* ret = new SceneObjectMapStringKey(getName(), getDataType());
    for (DATA_MAP_CONST_ITERATOR iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter)
    {
        ret->m_dataMap.insert(std::make_pair(iter->first, iter->second->clone()));
    }
    return ret;
}
