
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

#define __SCENE_OBJECT_MAP_INTEGER_KEY_DECLARE__
#include "SceneObjectMapIntegerKey.h"
#undef __SCENE_OBJECT_MAP_INTEGER_KEY_DECLARE__

#include "CaretAssert.h"
#include "SceneBoolean.h"
#include "SceneClass.h"
#include "SceneEnumeratedType.h"
#include "SceneFloat.h"
#include "SceneInteger.h"
#include "ScenePrimitive.h"
#include "SceneString.h"

using namespace caret;


    
/**
 * \class caret::SceneObjectMapIntegerKey 
 * \brief Map for saving data to a scene using integers as the key.
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
SceneObjectMapIntegerKey::SceneObjectMapIntegerKey(const QString& name,
                                       const SceneObjectDataTypeEnum::Enum valueDataType)
: SceneObject(name, 
              valueDataType)
{
    
}

/**
 * Destructor.
 */
SceneObjectMapIntegerKey::~SceneObjectMapIntegerKey()
{
    for (DATA_MAP_CONST_ITERATOR iter = m_dataMap.begin();
         iter != m_dataMap.end();
         iter++) {
        delete iter->second;
    }
}

/**
 * Add the given boolean value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapIntegerKey::addBoolean(const int32_t key,
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
SceneObjectMapIntegerKey::addInteger(const int32_t key,
         const int32_t value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_INTEGER);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneInteger("i", value)));
}


/**
 * Add the given float value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void
SceneObjectMapIntegerKey::addFloat(const int32_t key,
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
SceneObjectMapIntegerKey::addClass(const int32_t key,
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
SceneObjectMapIntegerKey::addString(const int32_t key,
                        const AString& value)
{
    CaretAssert(getDataType() == SceneObjectDataTypeEnum::SCENE_STRING);
    m_dataMap.insert(std::make_pair(key,
                                    new SceneString("s", value)));
}

/**
 * Add the given enumerated type value to the map using the given key.
 * @param key
 *    The key.
 * @param value
 *    The value.
 */
void 
SceneObjectMapIntegerKey::addEnumeratedType(const int32_t key,
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
SceneObjectMapIntegerKey::getObject(const int32_t key) const
{    
    const DATA_MAP_CONST_ITERATOR iter = m_dataMap.find(key);
    CaretAssert(iter != m_dataMap.end());
    const SceneObject* object = iter->second;
    CaretAssert(object);
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
SceneObjectMapIntegerKey::booleanValue(const int32_t key) const
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
SceneObjectMapIntegerKey::floatValue(const int32_t key) const
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
SceneObjectMapIntegerKey::integerValue(const int32_t key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->integerValue();
}

/** 
 * Get the value as a string. 
 *
 * @param key
 *    key of element.
 * @return The value with the given key.
 */
AString 
SceneObjectMapIntegerKey::stringValue(const int32_t key) const
{
    const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(getObject(key));
    CaretAssert(primitive);
    return primitive->stringValue();
}

/** 
 * Get the value as a class. 
 *
 * @param key
 *    key of element.
 * @return The class with the given key.
 */
const SceneClass* 
SceneObjectMapIntegerKey::classValue(const int32_t key) const
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
SceneObjectMapIntegerKey::enumeratedTypeValue(const int32_t key) const
{
    const SceneEnumeratedType* enumType = dynamic_cast<const SceneEnumeratedType*>(getObject(key));
    CaretAssert(enumType);
    return enumType->stringValue();
}

/**
 * @return A vector containg all of the keys in the map.
 */
std::vector<int32_t> 
SceneObjectMapIntegerKey::getKeys() const
{
    std::vector<int32_t> theKeys;
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
const std::map<int32_t, SceneObject*>&
SceneObjectMapIntegerKey::getMap() const
{
    return m_dataMap;
}




