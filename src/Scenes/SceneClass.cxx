
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

#define __SCENE_CLASS_DECLARE__
#include "SceneClass.h"
#undef __SCENE_CLASS_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "SceneBoolean.h"
#include "SceneBooleanArray.h"
#include "SceneClassArray.h"
#include "SceneEnumeratedType.h"
#include "SceneEnumeratedTypeArray.h"
#include "SceneFloat.h"
#include "SceneFloatArray.h"
#include "SceneInteger.h"
#include "SceneIntegerArray.h"
#include "SceneLongInteger.h"
#include "SceneLongIntegerArray.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePathName.h"
#include "ScenePathNameArray.h"
#include "SceneString.h"
#include "SceneStringArray.h"
#include "SceneUnsignedByteArray.h"

using namespace caret;



/**
 * \class caret::SceneClass 
 * \brief For storage of a class instance in a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 *
 * @param name
 *     Name of the instance saved to this scene class.
 * @param className
 *     Name of the class, NOT instance.
 * @param versionNumber
 *     Version number of the class that is saved to this
 *     scene class.  Since a class may change over time,
 *     this version number can be used to handle cases
 *     of scenes saved prior to changes made to a class.
 */
SceneClass::SceneClass(const AString& name,
                       const AString& className,
                       const int32_t versionNumber)
: SceneObject(name,
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_CLASS),
  m_className(className),
  m_versionNumber(versionNumber)
{
      
}

SceneClass::SceneClass(const SceneClass& rhs)
: SceneObject(rhs.getName(),
              SceneObjectContainerTypeEnum::SINGLE,
              SceneObjectDataTypeEnum::SCENE_CLASS),
  m_className(rhs.m_className),
  m_versionNumber(rhs.m_versionNumber)
{
    for (std::vector<SceneObject*>::const_iterator iter = rhs.m_childObjects.begin(); iter != rhs.m_childObjects.end(); ++iter)
    {
        m_childObjects.push_back((*iter)->clone());
    }
}

SceneObject* SceneClass::clone() const
{
    return new SceneClass(*this);
}

/**
 * Destructor.
 */
SceneClass::~SceneClass()
{
    for (std::vector<SceneObject*>::iterator iter = m_childObjects.begin();
         iter != m_childObjects.end();
         iter++) {
        delete *iter;
    }
    m_childObjects.clear();
}

/**
 * Cast an instance of SceneObject to a SceneClass.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
const SceneClass*
SceneClass::castToSceneClass() const
{
    return this;
}

/**
 * Cast an instance of SceneObject to a SceneClass.
 * Is used to avoid dynamic casting and overridden by the class.
 *
 * @return Valid pointer (non-NULL) this is SceneClass
 */
SceneClass*
SceneClass::castToSceneClass()
{
    return this;
}

/**
 * @return Name of the class (NOT the instance).
 */
AString 
SceneClass::getClassName() const
{
    return m_className;
}


/**
 * @return The version number of this scene class instance.
 */
int32_t 
SceneClass::getVersionNumber() const
{
    return m_versionNumber;
}

/**
 * @return All descendant SceneClasses (children, grandchildren, etc.) of this instance.
 */
std::vector<SceneObject*>
SceneClass::getDescendants() const
{
    std::vector<SceneObject*> descendants;
    
    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
         iter != m_childObjects.end();
         iter++) {
        SceneObject* so = *iter;
        descendants.push_back(so);
        
        std::vector<SceneObject*> childDescendants = so->getDescendants();
        descendants.insert(descendants.end(),
                           childDescendants.begin(),
                           childDescendants.end());
    }
    
    return descendants;
}


/**
 * Add a child to this class.
 * @param sceneObject
 *     New child.
 */
void 
SceneClass::addChild(SceneObject* sceneObject)
{
    CaretAssert(sceneObject);
    m_childObjects.push_back(sceneObject);
}

/**
 * Replace the given child object with the object for insertion
 * @param sceneObjectToReplace
 *    Object that is to be replace (IT IS NOT DELETED !!! )
 * @param newObjectToInsert
 *    Object that is inserted
 * @return True if object replacement is sucessful, else false.
 */
bool
SceneClass::replaceChild(const SceneObject* sceneObjectToReplace,
                         SceneObject* newObjectToInsert)
{
    const int32_t numChildren(static_cast<int32_t>(m_childObjects.size()));
    for (int32_t i = 0; i < numChildren; i++) {
        if (m_childObjects[i] == sceneObjectToReplace) {
            m_childObjects[i] = newObjectToInsert;
            return true;
        }
    }
    return false;
}

/**
 * Add a child boolean value to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void 
SceneClass::addBoolean(const AString& name,
                            const bool value)
{
    addChild(new SceneBoolean(name,
                                                 value));
}

/**
 * Add a child boolean array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
void 
SceneClass::addBooleanArray(const AString& name,
                       const bool values[],
                       const int32_t arrayNumberOfElements)
{
    addChild(new SceneBooleanArray(name,
                                                      values,
                                                      arrayNumberOfElements));
}

//void 
//SceneClass::addBooleanVector(const AString& name,
//                             const std::vector<bool>& values)
//{
//    addChild(new SceneBooleanArray(name,
//                                                   values));
//}


/**
 * Add a child class to the class.  NOTE: The given
 * scene class is not copied and this instance will
 * take ownership of the child class and delete when
 * this instance is destroyed.
 * 
 * @param sceneClass
 *    Class that is added.
 */
void SceneClass::addClass(SceneClass* sceneClass)
{
    if (sceneClass != NULL) {
        addChild(sceneClass);    
    }
}

/**
 * Add a child class array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
//void 
//SceneClass::addClassArray(const AString& name,
//                   SceneClass* values[],
//                   const int32_t arrayNumberOfElements)
//{
//   addChild(new SceneClassArray(name,
//                                values,
//                                arrayNumberOfElements));
//}


/**
 * Add a child enumerated type value to the class.
 * @param name
 *    Name associated with the value.
 * @param value
 *    String representation of the enumerated type's value.
 */
void 
SceneClass::addEnumeratedType(const AString& name,
                              const AString& value)
{
    addChild(new SceneEnumeratedType(name,
                                                             value));
}

/**
 * Add a child enumerated type array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
//void 
//SceneClass::addEnumeratedTypeArray(const AString& name,
//                            const AString values[],
//                            const int32_t arrayNumberOfElements)
//{
//    addChild(new SceneEnumeratedTypeArray(name,
//                                   values,
//                                   arrayNumberOfElements));
//}

/**
 * Add a child enumerated type array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The vector containing the values.
 */
void 
SceneClass::addEnumeratedTypeVector(const AString& name,
                                    const std::vector<AString>& values)
{
    addChild(new SceneEnumeratedTypeArray(name,
                                          values));
}



/**
 * Add a child float value to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void SceneClass::addFloat(const AString& name,
                          const float value)
{
    addChild(new SceneFloat(name,
                                               value));
}

/**
 * Add a child float array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
void 
SceneClass::addFloatArray(const AString& name,
                            const float values[],
                            const int32_t arrayNumberOfElements)
{
    addChild(new SceneFloatArray(name,
                                   values,
                                   arrayNumberOfElements));
}

/**
 * Add a child integer value to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void SceneClass::addInteger(const AString& name,
                            const int32_t value)
{
    addChild(new SceneInteger(name,
                              value));
}

/**
 * Add a child long integer value to the class.
 *
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void SceneClass::addLongInteger(const AString& name,
                            const int64_t value)
{
    addChild(new SceneLongInteger(name,
                                  value));
}

/**
 * Add a child integer array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
void 
SceneClass::addIntegerArray(const AString& name,
                          const int32_t values[],
                          const int32_t arrayNumberOfElements)
{
    addChild(new SceneIntegerArray(name,
                                 values,
                                 arrayNumberOfElements));
}

/**
 * Add a child long integer array values to the class.
 *
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
void
SceneClass::addLongIntegerArray(const AString& name,
                                const int64_t values[],
                                const int32_t arrayNumberOfElements)
{
    addChild(new SceneLongIntegerArray(name,
                                       values,
                                       arrayNumberOfElements));
}

/**
 * Add a child unsigned byte array values to the class.
 *
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
void
SceneClass::addUnsignedByteArray(const AString& name,
                            const uint8_t values[],
                            const int32_t arrayNumberOfElements)
{
    addChild(new SceneUnsignedByteArray(name,
                                   values,
                                   arrayNumberOfElements));
}

/**
 * Add a child string value to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void SceneClass::addString(const AString& name,
                           const AString& value)
{
    addChild(new SceneString(name,
                                                value));    
}

/**
 * Add a child path name value to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void SceneClass::addPathName(const AString& name,
                             const AString& value)
{
    addChild(new ScenePathName(name,
                               value));    
}

/**
 * Add a child string type array values to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param values
 *    The array containing the values.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 */
void 
SceneClass::addStringArray(const AString& name,
                                   const AString values[],
                                   const int32_t arrayNumberOfElements)
{
    addChild(new SceneStringArray(name,
                                  values,
                                  arrayNumberOfElements));
}

/**
 * Find and return the child boolean value with the given name.
 * If no primitive matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the primitive with the given
 *    name is not found.
 * @return
 *    The boolean value.
 */
bool 
SceneClass::getBooleanValue(const AString& name,
                            const bool defaultValue) const
{
    const ScenePrimitive* primitive = getPrimitive(name);
    if (primitive != NULL) {
        return primitive->booleanValue();
    }
    return defaultValue;
}

/**
 * Get the values for the boolean array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t 
SceneClass::getBooleanArrayValue(const AString& name,
                                 bool values[],
                                 const int32_t arrayNumberOfElements,
                                 const bool defaultValue) const
{
    const ScenePrimitiveArray* primitiveArray = getPrimitiveArray(name);
    
    if (primitiveArray != NULL) {
        primitiveArray->booleanValues(values, 
                                      arrayNumberOfElements, 
                                      defaultValue);
        return primitiveArray->getNumberOfArrayElements();
    }

    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    
    return 0;
}

/**
 * Find and return the child enumerated type value with the given name.
 * If no enumerated type value matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the enumerated type value with the given
 *    name is not found.
 * @return
 *    The string value representation of the enumerated type.
 */
AString
SceneClass::getEnumeratedTypeValueAsString(const AString& name,
                                           const AString& defaultValue) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        const SceneEnumeratedType* st = dynamic_cast<const SceneEnumeratedType*>(so);
//        if (st != NULL) {
//            if (st->getName() == name) {
//                return st->stringValue();
//            }
//        }
//    }
    
    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const SceneEnumeratedType* st = dynamic_cast<const SceneEnumeratedType*>(so);
        if (st != NULL) {
            if (st->getName() == name) {
                return st->stringValue();
            }
        }
    }
    
    logMissing("Scene Enumerated Type not found: " + name);
    return defaultValue;
}

/**
 * Get the values for the boolean array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t 
SceneClass::getEnumeratedTypeArrayValue(const AString& name,
                                        AString values[],
                                        const int32_t arrayNumberOfElements,
                                        const AString& defaultValue) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        const SceneEnumeratedTypeArray* enumArray = dynamic_cast<const SceneEnumeratedTypeArray*>(so);
//        if (enumArray != NULL) {
//            if (enumArray->getName() == name) {
//                enumArray->stringValues(values,
//                                        arrayNumberOfElements,
//                                        defaultValue);
//                return enumArray->getNumberOfArrayElements();
//            }
//        }
//    }
    
    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const SceneEnumeratedTypeArray* enumArray = dynamic_cast<const SceneEnumeratedTypeArray*>(so);
        if (enumArray != NULL) {
            enumArray->stringValues(values,
                                    arrayNumberOfElements,
                                    defaultValue);
            return enumArray->getNumberOfArrayElements();
        }
    }
    
    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    logMissing("Scene Enumerated Array Type not found: " + name);

    return 0;
}

/**
 * Find and return the child float value with the given name.
 * If no primitive matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the primitive with the given
 *    name is not found.
 * @return
 *    The float value.
 */
float 
SceneClass::getFloatValue(const AString& name,
                          const float defaultValue) const
{
    const ScenePrimitive* primitive = getPrimitive(name);
    if (primitive != NULL) {
        return primitive->floatValue();
    }
    return defaultValue;
}

/**
 * Get the values for the float array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t 
SceneClass::getFloatArrayValue(const AString& name,
                               float values[],
                               const int32_t arrayNumberOfElements,
                               const float defaultValue) const
{
    const ScenePrimitiveArray* primitiveArray = getPrimitiveArray(name);
    
    if (primitiveArray != NULL) {
        primitiveArray->floatValues(values, 
                                      arrayNumberOfElements, 
                                      defaultValue);
        return primitiveArray->getNumberOfArrayElements();
    }

    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    return 0;
}

/**
 * Find and return the child integer value with the given name.
 * If no primitive matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the primitive with the given
 *    name is not found.
 * @return
 *    The integer value.
 */
int32_t 
SceneClass::getIntegerValue(const AString& name,
                            const int32_t defaultValue) const
{
    const ScenePrimitive* primitive = getPrimitive(name);
    if (primitive != NULL) {
        return primitive->integerValue();
    }
    return defaultValue;
}

/**
 * Find and return the child long integer value with the given name.
 * If no primitive matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the primitive with the given
 *    name is not found.
 * @return
 *    The integer value.
 */
int64_t
SceneClass::getLongIntegerValue(const AString& name,
                                const int64_t defaultValue) const
{
    const ScenePrimitive* primitive = getPrimitive(name);
    if (primitive != NULL) {
        return primitive->longIntegerValue();
    }
    return defaultValue;
}

/**
 * Get the values for the integer array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t 
SceneClass::getIntegerArrayValue(const AString& name,
                               int32_t values[],
                               const int32_t arrayNumberOfElements,
                               const int32_t defaultValue) const
{
    const ScenePrimitiveArray* primitiveArray = getPrimitiveArray(name);
    
    if (primitiveArray != NULL) {
        primitiveArray->integerValues(values, 
                                    arrayNumberOfElements, 
                                    defaultValue);
        return primitiveArray->getNumberOfArrayElements();
    }

    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    return 0;
}

/**
 * Get the values for the long integer array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t
SceneClass::getLongIntegerArrayValue(const AString& name,
                                     int64_t values[],
                                     const int32_t arrayNumberOfElements,
                                     const int64_t defaultValue) const
{
    const ScenePrimitiveArray* primitiveArray = getPrimitiveArray(name);
    
    if (primitiveArray != NULL) {
        primitiveArray->longIntegerValues(values,
                                          arrayNumberOfElements,
                                          defaultValue);
        return primitiveArray->getNumberOfArrayElements();
    }
    
    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    return 0;
}

/**
 * Get the values for the unsigned byte array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t
SceneClass::getUnsignedByteArrayValue(const AString& name,
                                 uint8_t values[],
                                 const int32_t arrayNumberOfElements,
                                 const uint8_t defaultValue) const
{
    const ScenePrimitiveArray* primitiveArray = getPrimitiveArray(name);
    
    if (primitiveArray != NULL) {
        primitiveArray->unsignedByteValues(values,
                                      arrayNumberOfElements,
                                      defaultValue);
        return primitiveArray->getNumberOfArrayElements();
    }
    
    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    return 0;
}

/**
 * Find and return the child string value with the given name.
 * If no primitive matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the primitive with the given
 *    name is not found.
 * @return
 *    The string value.
 */
AString 
SceneClass::getStringValue(const AString& name,
                           const AString& defaultValue) const
{
    const ScenePrimitive* primitive = getPrimitive(name);
    if (primitive != NULL) {
        return primitive->stringValue();
    }
    return defaultValue;
}

/**
 * Find and return the child path name value with the given name.
 * If no path name matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the path name with the given
 *    name is not found.
 * @return
 *    The string value.
 */
AString 
SceneClass::getPathNameValue(const AString& name,
                             const AString& defaultValue) const
{
    const ScenePathName* pathName = getPathName(name);
    if (pathName != NULL) {
        return pathName->stringValue();
    }
    return defaultValue;
}

/**
 * Get the values for the string array.  If no array is
 * found with the given name, all values are set to the
 * default value.
 * @param name
 *    Name of the value.
 * @param values
 *    Output array into which values are loaded.
 * @param arrayNumberOfElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used for missing elements.
 * @return Number of elements actually read form scene class.
 */
int32_t 
SceneClass::getStringArrayValue(const AString& name,
                                AString values[],
                                const int32_t arrayNumberOfElements,
                                const AString& defaultValue) const
{
    const ScenePrimitiveArray* primitiveArray = getPrimitiveArray(name);
    
    if (primitiveArray != NULL) {
        primitiveArray->stringValues(values, 
                                      arrayNumberOfElements, 
                                      defaultValue);
        return primitiveArray->getNumberOfArrayElements();
    }

    for (int32_t i = 0; i < arrayNumberOfElements; i++) {
        values[i] = defaultValue;
    }
    return 0;
}

/**
 * Find and return the scene's child primitive with the given name.
 *
 * @param name
 *     Name of the child primitive.
 * @return
 *     Pointer to the primitive with the given name or NULL if
 *     no primitive exists with the given name.
 */
const ScenePrimitive* 
SceneClass::getPrimitive(const AString& name) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        const ScenePrimitive* sp = dynamic_cast<const ScenePrimitive*>(so);
//        if (sp != NULL) {
//            if (sp->getName() == name) {
//                return sp;
//            }
//        }
//    }

    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const ScenePrimitive* sp = dynamic_cast<const ScenePrimitive*>(so);
        if (sp != NULL) {
            return sp;
        }
    }
    
    logMissing("Scene Primitive Type not found: " + name);
    
    return NULL;
}

/**
 * Find and return the scene's child path name with the given name.
 *
 * @param name
 *     Name of the child path name.
 * @return
 *     Pointer to the path name with the given name or NULL if
 *     no path name exists with the given name.
 */
const ScenePathName* 
SceneClass::getPathName(const AString& name) const
{
    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const ScenePathName* sp = dynamic_cast<const ScenePathName*>(so);
        if (sp != NULL) {
            return sp;
        }
    }
    
    logMissing("Scene Path Name not found: " + name);
    
    return NULL;
}

/**
 * Find and return the scene's child primitive array with the given name.
 *
 * @param name
 *     Name of the child primitive.
 * @return
 *     Pointer to the primitive with the given name or NULL if
 *     no primitive exists with the given name.
 */
const ScenePrimitiveArray* 
SceneClass::getPrimitiveArray(const AString& name) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        const ScenePrimitiveArray* spa = dynamic_cast<const ScenePrimitiveArray*>(so);
//        if (spa != NULL) {
//            if (spa->getName() == name) {
//                return spa;
//            }
//        }
//    }
    
    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const ScenePrimitiveArray* spa = dynamic_cast<const ScenePrimitiveArray*>(so);
        if (spa != NULL) {
            return spa;
        }
    }
    
    logMissing("Scene Primitive Array not found: " + name);
    return NULL;
}

/**
 * Find and return the scene's child primitive array with the given name.
 *
 * @param name
 *     Name of the child primitive.
 * @return
 *     Pointer to the primitive with the given name or NULL if
 *     no primitive exists with the given name.
 */
const ScenePathNameArray*
SceneClass::getPathNameArray(const AString& name) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        if (so->getName() == name) {
//            const ScenePathNameArray* spa = dynamic_cast<const ScenePathNameArray*>(so);
//            if (spa != NULL) {
//                return spa;
//            }
//        }
//    }
    
    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const ScenePathNameArray* spa = dynamic_cast<const ScenePathNameArray*>(so);
        if (spa != NULL) {
            return spa;
        }
    }
    
    logMissing("Scene Path Name Array not found: " + name);
    return NULL;
}


/**
 * Find and return the scene's child class with the given name.
 *
 * @param name
 *     Name of the child class.
 * @return
 *     Pointer to the class with the given name or NULL if
 *     no child class exists with the given name.
 */
const SceneClass* 
SceneClass::getClass(const AString& name) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
//        if (sc != NULL) {
//            if (sc->getName() == name) {
//                return sc;
//            }
//        }
//    }
    
    const SceneObject* so = getObjectWithName(name);
    if (so != NULL) {
        const SceneClass* sc = dynamic_cast<const SceneClass*>(so);
        if (sc != NULL) {
            return sc;
        }
    }
    
    logMissing("Scene Class not found: " + name);
    
    return NULL;
}

/**
 * Find and return the scene's child class with the given name.
 *
 * @param name
 *     Name of the child class.
 * @return
 *     Pointer to the class with the given name or NULL if
 *     no child class exists with the given name.
 */
SceneClass* 
SceneClass::getClass(const AString& name)
{
//    for (std::vector<SceneObject*>::iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        SceneObject* so = *iter;
//        SceneClass* sc = dynamic_cast<SceneClass*>(so);
//        if (sc != NULL) {
//            if (sc->getName() == name) {
//                return sc;
//            }
//        }
//    }
    
    const SceneObject* soConst = getObjectWithName(name);
    if (soConst != NULL) {
        SceneObject* so = const_cast<SceneObject*>(soConst);
        SceneClass* sc = dynamic_cast<SceneClass*>(so);
        if (sc != NULL) {
            return sc;
        }
    }
    
    logMissing("Scene Class not found: " + name);
    
    return NULL;
}

/**
 * Find and return the scene's child map with integer key with the given name.
 *
 * @param name
 *     Name of the child class.
 * @return
 *     Pointer to the map with integer key with the given name or NULL if
 *     no child map with integer key exists with the given name.
 */
const SceneObjectMapIntegerKey* 
SceneClass::getMapIntegerKey(const AString& name) const
{
    
    const SceneObject* sceneObject = getObjectWithName(name);
    if (sceneObject != NULL) {
        const SceneObjectMapIntegerKey* smik = dynamic_cast<const SceneObjectMapIntegerKey*>(sceneObject);
        if (smik == NULL) {
            logMissing("SceneObjectMapIntegerKey not found: " + name);
        }
        return smik;
    }
    
    logMissing("SceneObjectMapIntegerKey not found: " + name);
    return NULL;
}


/**
 * Find and return the scene's child class array with the given name.
 *
 * @param name
 *     Name of the child class array.
 * @return
 *     Pointer to the class array with the given name or NULL if
 *     no child class array exists with the given name.
 */
SceneClassArray* 
SceneClass::getClassArray(const AString& name)
{
//    for (std::vector<SceneObject*>::iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        SceneObject* so = *iter;
//        SceneClassArray* sca = dynamic_cast<SceneClassArray*>(so);
//        if (sca != NULL) {
//            if (sca->getName() == name) {
//                return sca;
//            }
//        }
//    }

    const SceneObject* soConst = getObjectWithName(name);
    if (soConst != NULL) {
        SceneObject* so = const_cast<SceneObject*>(soConst);
        SceneClassArray* sca = dynamic_cast<SceneClassArray*>(so);
        if (sca != NULL) {
            return sca;
        }
    }
    
    logMissing("Scene Class not found: " + name);
    
    return NULL;
}

/**
 * Find and return the scene's child class array with the given name.
 *
 * @param name
 *     Name of the child class array.
 * @return
 *     Pointer to the class array with the given name or NULL if
 *     no child class array exists with the given name.
 */
const SceneClassArray* 
SceneClass::getClassArray(const AString& name) const
{
//    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
//         iter != m_childObjects.end();
//         iter++) {
//        const SceneObject* so = *iter;
//        const SceneClassArray* sca = dynamic_cast<const SceneClassArray*>(so);
//        if (sca != NULL) {
//            if (sca->getName() == name) {
//                return sca;
//            }
//        }
//    }
    
    const SceneObject* soConst = getObjectWithName(name);
    if (soConst != NULL) {
        SceneObject* so = const_cast<SceneObject*>(soConst);
        const SceneClassArray* sca = dynamic_cast<const SceneClassArray*>(so);
        if (sca != NULL) {
            return sca;
        }
    }
    
    logMissing("Scene Class Array not found: " + name);
    
    return NULL;
}

/**
 * @return Number of objects in the class.
 */
int32_t 
SceneClass::getNumberOfObjects() const
{
    return m_childObjects.size();
}

/**
 * @return Object at the given index.
 * @param indx
 *    Index of the object.
 */
const SceneObject* 
SceneClass::getObjectAtIndex(const int32_t indx) const
{
    CaretAssertVectorIndex(m_childObjects, indx);
    m_childObjects[indx]->m_restoredFlag = true;
    return m_childObjects[indx];
}

/**
 * @return Object with the given name.  Returns NULL
 * if an object with the given name is not found.
 * @param name
 *    Name of object.
 */
const SceneObject* 
SceneClass::getObjectWithName(const AString& name) const
{
    SceneObject* sceneObject = NULL;
    bool wasLoggedFlag = false;
    
    for (std::vector<SceneObject*>::const_iterator iter = m_childObjects.begin();
         iter != m_childObjects.end();
         iter++) {
        SceneObject* so = *iter;
        if (so->getName() == name) {
            so->m_restoredFlag = true;
            
            if (s_debugLoggingEnabledFlag) {
                if (sceneObject != NULL) {
                    if ( ! wasLoggedFlag) {
                        CaretLogWarning("Scene class named \""
                                        + getName()
                                        + "\" contains more than one child named \""
                                        + name
                                        + "\".  If replacing the scene does not eliminate this message "
                                        "there is an error in the source code");
                        wasLoggedFlag = true;
                    }
                }
            }
            else {
                return so;
            }
            
            sceneObject = so;
        }
    }
    
    return sceneObject;
}

/**
 * Set debug logging enabled.
 * If enabled, message are logged at the warning level
 * for duplicated objects and missing objects.
 * 
 * @param status
 *     New status.
 */
void
SceneClass::setDebugLoggingEnabled(const bool status)
{
    s_debugLoggingEnabledFlag = status;
}

/**
 * Log a missing object message to the Caret Logger.
 * This is done through a method so that the level
 * can easily be changed.
 * @param missionInfo
 *    Information about missing object.
 */
void 
SceneClass::logMissing(const AString& missingInfo) const
{
    if (missingInfo.isEmpty()) {
        return;
    }
    
    if (s_debugLoggingEnabledFlag) {
        CaretLogFiner(missingInfo);
    }
}


/**
 * Find and return the scene's child class with the given name that uses a file name,
 * possibly just the filename or a relative path and filename.
 *
 * @param filenameFullPath
 *     Full path of file
 * @param filenameNoPath
 *     Filename no path
 * @param sceneFileName
 *     Name of scene file
 * @return
 *     Pointer to the class with the given name or NULL if
 *     no child class exists with the given name.
 */
const SceneClass*
SceneClass::getClassNamedWithFileName(const AString& filenameFullPath,
                                      const AString& filenameNoPath,
                                      const AString& sceneFileName) const
{
    ScenePathName spn(filenameFullPath,
                      filenameFullPath);
    const AString relativePathName(spn.getRelativePathToSceneFile(sceneFileName));
    const SceneClass* sc(NULL);
    if ( ! relativePathName.isEmpty()) {
        sc = getClass(relativePathName);
    }
    if (sc == NULL) {
        sc = getClass(filenameNoPath);
    }
    return sc;
}

