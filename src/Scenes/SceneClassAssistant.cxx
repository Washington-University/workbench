
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

#define __SCENE_CLASS_ASSISTANT_DECLARE__
#include "SceneClassAssistant.h"
#undef __SCENE_CLASS_ASSISTANT_DECLARE__

#include "CaretAssert.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePrimitive.h"
#include "SceneableInterface.h"

using namespace caret;


    
/**
 * \class caret::SceneClassAssistant 
 * \brief Assists a class with restoring and saving its data to a scene.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 *
 * Assists with the restoration and saving of data for a SceneClass.
 * Rather than the scene class making explicit calls to save and restore
 * each of its members, an assistant can be created and setup with the
 * members of the class.  The assistant will then handle restoration
 * and saving of the data.
 */

/**
 * Constructor.
 */
SceneClassAssistant::SceneClassAssistant()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
SceneClassAssistant::~SceneClassAssistant()
{
    for (DataStorageIterator iter = m_dataStorage.begin();
         iter != m_dataStorage.end();
         iter++) {
        delete *iter;
    }
}

/**
 * Add a float member.
 *
 * Note: If restoring and the member is not found, the default
 * value used will be the value at the pointer.
 *
 * @param name
 *    Name of member.
 * @param floatAddress
 *    Address of the member.
 */
void 
SceneClassAssistant::add(const AString& name,
                         float* floatAddress)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(floatAddress);
    FloatData* fd = new FloatData(name,
                                  floatAddress,
                                  *floatAddress);
    m_dataStorage.push_back(fd);
}

/**
 * Add an integer member.
 *
 * Note: If restoring and the member is not found, the default
 * value used will be the value at the pointer.
 *
 * @param name
 *    Name of member.
 * @param integerAddress
 *    Address of the member.
 */
void 
SceneClassAssistant::add(const AString& name,
                         int32_t* integerAddress)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(integerAddress);
    IntegerData* id = new IntegerData(name,
                                  integerAddress,
                                  *integerAddress);
    m_dataStorage.push_back(id);
}

/**
 * Add a long integer member.
 *
 * Note: If restoring and the member is not found, the default
 * value used will be the value at the pointer.
 *
 * @param name
 *    Name of member.
 * @param integerAddress
 *    Address of the member.
 */
void
SceneClassAssistant::add(const AString& name,
                         int64_t* longIntegerAddress)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(longIntegerAddress);
    LongIntegerData* id = new LongIntegerData(name,
                                              longIntegerAddress,
                                              *longIntegerAddress);
    m_dataStorage.push_back(id);
}

/**
 * Add a boolean member.
 *
 * Note: If restoring and the member is not found, the default
 * value used will be the value at the pointer.
 *
 * @param name
 *    Name of member.
 * @param booleanAddress
 *    Address of the member.
 */
void 
SceneClassAssistant::add(const AString& name,
                         bool* booleanAddress)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(booleanAddress);
    BooleanData* bd = new BooleanData(name,
                                      booleanAddress,
                                      *booleanAddress);
    m_dataStorage.push_back(bd);
}

/**
 * Add a string member.
 *
 * Note: If restoring and the member is not found, the default
 * value used will be the value at the pointer.
 *
 * @param name
 *    Name of member.
 * @param stringAddress
 *    Address of the member.
 */
void 
SceneClassAssistant::add(const AString& name,
                         AString* stringAddress)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(stringAddress);
    StringData* sd = new StringData(name,
                                      stringAddress,
                                      *stringAddress);
    m_dataStorage.push_back(sd);
    
}

/**
 * Add a scene class.
 * @param name
 *     Name of class instance.
 * @param className
 *     Name of the class.
 * @param sceneClass
 *     Handle (pointer to the pointer) that points to the class.
 *     If the value of the pointer (*sceneClass) is NULL, then
 *     the scene class is NOT added to the scene.  This method
 *     is best used when a member of a class is a pointer to
 *     a class that implementes the SceneableInterface.
 */
//void 
//SceneClassAssistant::add(const AString& name,
//                         const AString& className,
//                         SceneableInterface** sceneClass)
//{
//    CaretAssert(sceneClass);
//    
//    ClassData* cd = new ClassData(name,
//                                  className,
//                                  sceneClass);
//    m_dataStorage.push_back(cd);
//}

/**
 * Add a scene class.
 * @param name
 *     Name of class instance.
 * @param className
 *     Name of the class.
 * @param sceneClass
 *     Pointer to the class.  This method is best used when
 *     a member of the class is not a pointer and implements
 *     the SceneableInterface.
 */
void 
SceneClassAssistant::add(const AString& name,
                         const AString& className,
                         SceneableInterface* sceneClass)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(sceneClass);
    
    ClassData* cd = new ClassData(name,
                                  className,
                                  sceneClass);
    m_dataStorage.push_back(cd);
}

/**
 * Add a boolean array member.
 * @param name
 *    Name of member.
 * @param booleanArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::addArray(const AString& name,
                              bool* booleanArray,
                              const int32_t numberOfElements,
                              const bool defaultValue)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(booleanArray);
    CaretAssert(numberOfElements >= 0);
    
    BooleanArrayData* bad = new BooleanArrayData(name,
                                                 booleanArray,
                                                 numberOfElements,
                                                 defaultValue);
    m_dataStorage.push_back(bad);
}

/**
 * Add a float array member.
 * @param name
 *    Name of member.
 * @param floatArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::addArray(const AString& name,
                              float* floatArray,
                              const int32_t numberOfElements,
                              const float defaultValue)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(floatArray);
    CaretAssert(numberOfElements >= 0);
    
    FloatArrayData* fad = new FloatArrayData(name,
                                                 floatArray,
                                                 numberOfElements,
                                                 defaultValue);
    m_dataStorage.push_back(fad);
}

/**
 * Add an integer array member.
 * @param name
 *    Name of member.
 * @param integerArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::addArray(const AString& name,
                              int32_t* integerArray,
                              const int32_t numberOfElements,
                              const int32_t defaultValue)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(integerArray);
    CaretAssert(numberOfElements >= 0);
    
    IntegerArrayData* iad = new IntegerArrayData(name,
                                                 integerArray,
                                                 numberOfElements,
                                                 defaultValue);
    m_dataStorage.push_back(iad);
}

/**
 * Add a long integer array member.
 * @param name
 *    Name of member.
 * @param longIntegerArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void
SceneClassAssistant::addArray(const AString& name,
                              int64_t* longIntegerArray,
                              const int32_t numberOfElements,
                              const int64_t defaultValue)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(longIntegerArray);
    CaretAssert(numberOfElements >= 0);
    
    LongIntegerArrayData* iad = new LongIntegerArrayData(name,
                                                         longIntegerArray,
                                                         numberOfElements,
                                                         defaultValue);
    m_dataStorage.push_back(iad);
}


/**
 * Add an unsigned byte array member.
 * @param name
 *    Name of member.
 * @param byteArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void
SceneClassAssistant::addArray(const AString& name,
                              uint8_t* byteArray,
                              const int32_t numberOfElements,
                              const uint8_t defaultValue)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(byteArray);
    CaretAssert(numberOfElements >= 0);
    
    UnsignedByteArrayData* iad = new UnsignedByteArrayData(name,
                                                 byteArray,
                                                 numberOfElements,
                                                 defaultValue);
    m_dataStorage.push_back(iad);
}


/**
 * Add a string array member.
 * @param name
 *    Name of member.
 * @param stringArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::addArray(const AString& name,
                              AString* stringArray,
                              const int32_t numberOfElements,
                              const AString& defaultValue)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(stringArray);
    CaretAssert(numberOfElements >= 0);
    
    StringArrayData* sad = new StringArrayData(name,
                                                 stringArray,
                                                 numberOfElements,
                                                 defaultValue);
    m_dataStorage.push_back(sad);
}

/**
 * Add a sceneable interface (classes) array member.
 * @param name
 *    Name of member.
 * @param sceneableInterfaceArray
 *    The array (pointer to first element)
 * @param numberOfArrayElements
 *    Number of elements in the array.
 */
//void 
//SceneClassAssistant::addArray(const AString& name,
//              SceneableInterface* sceneableInterfaceArray[],
//              const int32_t numberOfElements)
//{
//    ClassArrayData* cad = new ClassArrayData(name,
//                                             sceneableInterfaceArray,
//                                             numberOfElements);
//    m_dataStorage.push_back(cad);
//}

/**
 * Add a tab-indexed boolean array.  The array must
 * contain BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS elements.
 * Depending upon how he scene is saved, all array elements or
 * just those for specific tabs will be saved to the scene.
 * @param name
 *    Name of member.
 * @param booleanArray
 *    The array (pointer to first element.
 */
void 
SceneClassAssistant::addTabIndexedBooleanArray(const AString& name,
                                             bool* booleanArray)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(booleanArray);
    
    BooleanTabIndexArrayMapData* bad = new BooleanTabIndexArrayMapData(name,
                                                                       booleanArray);
    m_dataStorage.push_back(bad);
}

/**
 * Add a tab-indexed integer array.  The array must
 * contain BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS elements.
 * Depending upon how he scene is saved, all array elements or
 * just those for specific tabs will be saved to the scene.
 * @param name
 *    Name of member.
 * @param integerArray
 *    The array (pointer to first element.
 */
void 
SceneClassAssistant::addTabIndexedIntegerArray(const AString& name,
                                             int32_t* integerArray)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(integerArray);
    
    IntegerTabIndexArrayMapData* bad = new IntegerTabIndexArrayMapData(name,
                                                                       integerArray);
    m_dataStorage.push_back(bad);
}

/**
 * Add a tab-indexed float array.  The array must
 * contain BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS elements.
 * Depending upon how he scene is saved, all array elements or
 * just those for specific tabs will be saved to the scene.
 * @param name
 *    Name of member.
 * @param floatArray
 *    The array (pointer to first element.
 */
void 
SceneClassAssistant::addTabIndexedFloatArray(const AString& name,
                                           float* floatArray)
{
    CaretAssert(name.isEmpty() == false);
    CaretAssert(floatArray);
    
    FloatTabIndexArrayMapData* bad = new FloatTabIndexArrayMapData(name,
                                                                       floatArray);
    m_dataStorage.push_back(bad);
}


/**
 * Restore the members of a class.
 * 
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously 
 *     saved and should be restored.
 */
void 
SceneClassAssistant::restoreMembers(const SceneAttributes* sceneAttributes,
                                    const SceneClass* sceneClass)
{
    for (DataStorageIterator iter = m_dataStorage.begin();
         iter != m_dataStorage.end();
         iter++) {
        Data* data = *iter;
        data->restore(*sceneAttributes,
                      *sceneClass);
    }
}

/**
 * Save the members of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */

void 
SceneClassAssistant::saveMembers(const SceneAttributes* sceneAttributes,
                                 SceneClass* sceneClass)
{
    for (DataStorageIterator iter = m_dataStorage.begin();
         iter != m_dataStorage.end();
         iter++) {
        Data* data = *iter;
        data->save(*sceneAttributes,
                   *sceneClass);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::Data 
 * \brief Base class for data added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::Data::Data(const AString& name) 
: m_name(name) 
{
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::FloatData 
 * \brief Float data added to a scene class.
 */

/**
 * Constructor.
 * @param name
 *    Name of data.
 * @param dataPointer
 *    Pointer to data.
 * @param defaultValue
 *    Default value used when restoring and data with name not found.
 */
SceneClassAssistant::FloatData::FloatData(const AString& name,
                                          float* dataPointer,
                                          const float defaultValue) 
: Data(name), 
m_dataPointer(dataPointer),
m_defaultValue(defaultValue) 
{
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes of the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::FloatData::restore(const SceneAttributes& /*sceneAttributes*/,
                                        const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getFloatValue(m_name, 
                                              m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::FloatData::save(const SceneAttributes& /*sceneAttributes*/,
                                     SceneClass& sceneClass)
{
    sceneClass.addFloat(m_name, 
                        *m_dataPointer);
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::IntegerData 
 * \brief Integer data added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of data.
 * @param dataPointer
 *    Pointer to data.
 * @param defaultValue
 *    Default value used when restoring and data with name not found.
 */
SceneClassAssistant::IntegerData::IntegerData(const AString& name,
                                          int32_t* dataPointer,
                                          const int32_t defaultValue) 
: Data(name), 
m_dataPointer(dataPointer),
m_defaultValue(defaultValue) 
{
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::IntegerData::restore(const SceneAttributes& /*sceneAttributes*/,
                                          const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getIntegerValue(m_name, 
                                              m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::IntegerData::save(const SceneAttributes& /*sceneAttributes*/,
                                       SceneClass& sceneClass)
{
    sceneClass.addInteger(m_name, 
                        *m_dataPointer);
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::IntegerData
 * \brief Integer data added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of data.
 * @param dataPointer
 *    Pointer to data.
 * @param defaultValue
 *    Default value used when restoring and data with name not found.
 */
SceneClassAssistant::LongIntegerData::LongIntegerData(const AString& name,
                                                      int64_t* dataPointer,
                                                      const int64_t defaultValue)
: Data(name),
m_dataPointer(dataPointer),
m_defaultValue(defaultValue)
{
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void
SceneClassAssistant::LongIntegerData::restore(const SceneAttributes& /*sceneAttributes*/,
                                              const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getLongIntegerValue(m_name,
                                                    m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void
SceneClassAssistant::LongIntegerData::save(const SceneAttributes& /*sceneAttributes*/,
                                           SceneClass& sceneClass)
{
    sceneClass.addLongInteger(m_name,
                              *m_dataPointer);
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::BooleanData 
 * \brief Boolean data added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of data.
 * @param dataPointer
 *    Pointer to data.
 * @param defaultValue
 *    Default value used when restoring and data with name not found.
 */
SceneClassAssistant::BooleanData::BooleanData(const AString& name,
                                              bool* dataPointer,
                                              const bool defaultValue) 
: Data(name), 
m_dataPointer(dataPointer),
m_defaultValue(defaultValue) 
{
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::BooleanData::restore(const SceneAttributes& /*sceneAttributes*/,
                                          const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getBooleanValue(m_name, 
                                                m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::BooleanData::save(const SceneAttributes& /*sceneAttributes*/,
                                       SceneClass& sceneClass)
{
    sceneClass.addBoolean(m_name, 
                          *m_dataPointer);
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::StringData 
 * \brief String data added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of data.
 * @param dataPointer
 *    Pointer to data.
 * @param defaultValue
 *    Default value used when restoring and data with name not found.
 */
SceneClassAssistant::StringData::StringData(const AString& name,
                                              AString* dataPointer,
                                              const AString& defaultValue) 
: Data(name), 
m_dataPointer(dataPointer),
m_defaultValue(defaultValue) 
{
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::StringData::restore(const SceneAttributes& /*sceneAttributes*/,
                                          const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getStringValue(m_name, 
                                                m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::StringData::save(const SceneAttributes& /*sceneAttributes*/,
                                       SceneClass& sceneClass)
{
    sceneClass.addString(m_name, 
                          *m_dataPointer);
}


/* ========================================================================= */

/**
 * \class caret::SceneClassAssistant::TabIndexArrayMapData
 * \brief Base class for arrays that are index by a tab index.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::TabIndexArrayMapData::TabIndexArrayMapData(const AString& name)
  : Data(name)
{
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::BooleanTabIndexArrayMapData
 * \brief Boolean array that is indexed by a tab index.
 */

/**
 * Constructor.
 * @param name
 *    Name of boolean array.
 * @param booleanArray
 *    Address fo the array that is indexed by tab and contains
 *    BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS elemnts.
 */
SceneClassAssistant::BooleanTabIndexArrayMapData::BooleanTabIndexArrayMapData(const AString& name,
                                                                              bool* booleanArray)
: TabIndexArrayMapData(name),
m_booleanArray(booleanArray)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::BooleanTabIndexArrayMapData::restore(const SceneAttributes& /*sceneAttributes*/,
                                                          const SceneClass& sceneClass)
{
    const SceneObjectMapIntegerKey* sceneMap = sceneClass.getMapIntegerKey(m_name);
    if (sceneMap != NULL) {
        const std::map<int32_t, SceneObject*>& dataMap = sceneMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
             iter != dataMap.end();
             iter++) {
            const int32_t tabIndex = iter->first;
            const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
            m_booleanArray[tabIndex] = primitive->booleanValue();
        }
//        const std::vector<int32_t> keys = sceneMap->getKeys();
//        const int32_t numKeys = static_cast<int32_t>(keys.size());
//        for (int32_t i = 0; i < numKeys; i++) {
//            const int32_t tabIndex = keys[i];
//            m_booleanArray[tabIndex] = sceneMap->booleanValue(tabIndex);
//        }
    }
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class into  which data is stored.
 */
void 
SceneClassAssistant::BooleanTabIndexArrayMapData::save(const SceneAttributes& sceneAttributes,
                                                       SceneClass& sceneClass)
{
    const std::vector<int32_t> tabIndices = sceneAttributes.getIndicesOfTabsForSavingToScene();
    const int32_t numTabIndices = static_cast<int32_t>(tabIndices.size());
                                                       
    SceneObjectMapIntegerKey* sceneMap = new SceneObjectMapIntegerKey(m_name,
                                                          SceneObjectDataTypeEnum::SCENE_BOOLEAN);
    
    for (int32_t i = 0; i < numTabIndices; i++) {
        const int32_t tabIndex = tabIndices[i];
        sceneMap->addBoolean(tabIndex, m_booleanArray[tabIndex]);
    }
    
    sceneClass.addChild(sceneMap);
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::IntegerTabIndexArrayMapData
 * \brief Integer array that is indexed by a tab index.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of integer array.
 * @param integerArray
 *    Address of the array that is indexed by tab and contains
 *    BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS elemnts.
 */
SceneClassAssistant::IntegerTabIndexArrayMapData::IntegerTabIndexArrayMapData(const AString& name,
                                                                              int32_t* integerArray)
: TabIndexArrayMapData(name),
m_integerArray(integerArray)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::IntegerTabIndexArrayMapData::restore(const SceneAttributes& /*sceneAttributes*/,
                                                          const SceneClass& sceneClass)
{
    const SceneObjectMapIntegerKey* sceneMap = sceneClass.getMapIntegerKey(m_name);
    if (sceneMap != NULL) {
        const std::map<int32_t, SceneObject*>& dataMap = sceneMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
             iter != dataMap.end();
             iter++) {
            const int32_t tabIndex = iter->first;
            const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
            m_integerArray[tabIndex] = primitive->integerValue();
        }
    }
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class into  which data is stored.
 */
void 
SceneClassAssistant::IntegerTabIndexArrayMapData::save(const SceneAttributes& sceneAttributes,
                                                       SceneClass& sceneClass)
{
    const std::vector<int32_t> tabIndices = sceneAttributes.getIndicesOfTabsForSavingToScene();
    const int32_t numTabIndices = static_cast<int32_t>(tabIndices.size());
    
    SceneObjectMapIntegerKey* sceneMap = new SceneObjectMapIntegerKey(m_name,
                                                          SceneObjectDataTypeEnum::SCENE_INTEGER);
    
    for (int32_t i = 0; i < numTabIndices; i++) {
        const int32_t tabIndex = tabIndices[i];
        sceneMap->addInteger(tabIndex, m_integerArray[tabIndex]);
    }
    
    sceneClass.addChild(sceneMap);
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::FloatTabIndexArrayMapData
 * \brief Float array that is indexed by a tab index.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of float array.
 * @param floatArray
 *    Address of the array that is indexed by tab and contains
 *    BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS elemnts.
 */
SceneClassAssistant::FloatTabIndexArrayMapData::FloatTabIndexArrayMapData(const AString& name,
                                                                              float* floatArray)
: TabIndexArrayMapData(name),
m_floatArray(floatArray)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::FloatTabIndexArrayMapData::restore(const SceneAttributes& /*sceneAttributes*/,
                                                          const SceneClass& sceneClass)
{
    const SceneObjectMapIntegerKey* sceneMap = sceneClass.getMapIntegerKey(m_name);
    if (sceneMap != NULL) {
        const std::map<int32_t, SceneObject*>& dataMap = sceneMap->getMap();
        for (std::map<int32_t, SceneObject*>::const_iterator iter = dataMap.begin();
             iter != dataMap.end();
             iter++) {
            const int32_t tabIndex = iter->first;
            const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(iter->second);
            m_floatArray[tabIndex] = primitive->floatValue();
        }
    }
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class into  which data is stored.
 */
void 
SceneClassAssistant::FloatTabIndexArrayMapData::save(const SceneAttributes& sceneAttributes,
                                                       SceneClass& sceneClass)
{
    const std::vector<int32_t> tabIndices = sceneAttributes.getIndicesOfTabsForSavingToScene();
    const int32_t numTabIndices = static_cast<int32_t>(tabIndices.size());
    
    SceneObjectMapIntegerKey* sceneMap = new SceneObjectMapIntegerKey(m_name,
                                                          SceneObjectDataTypeEnum::SCENE_FLOAT);
    
    for (int32_t i = 0; i < numTabIndices; i++) {
        const int32_t tabIndex = tabIndices[i];
        sceneMap->addFloat(tabIndex, m_floatArray[tabIndex]);
    }
    
    sceneClass.addChild(sceneMap);
}




/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::BooleanVectorData 
 * \brief Boolean vector added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::ArrayData::ArrayData(const AString& name,
          const int32_t numberOfArrayElements)
: Data(name),
 m_numberOfArrayElements(numberOfArrayElements)
{
    
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::BooleanArrayData 
 * \brief Boolean array added to a scene class.
 */
SceneClassAssistant::BooleanArrayData::BooleanArrayData(const AString& name,
                 bool* booleanArray,
                 const int32_t numberOfArrayElements,
                 const bool defaultValue)
: ArrayData(name,
            numberOfArrayElements),
 m_booleanArray(booleanArray),
 m_defaultValue(defaultValue)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::BooleanArrayData::restore(const SceneAttributes& /*sceneAttributes*/,
                                               const SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.getBooleanArrayValue(m_name, 
                                        m_booleanArray, 
                                        m_numberOfArrayElements, 
                                        m_defaultValue);
    }
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void SceneClassAssistant::BooleanArrayData::save(const SceneAttributes& /*sceneAttributes*/,
                                                 SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.addBooleanArray(m_name, 
                                   m_booleanArray, 
                                   m_numberOfArrayElements);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::IntegerArrayData 
 * \brief Boolean array added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::IntegerArrayData::IntegerArrayData(const AString& name,
                                                        int32_t* integerArray,
                                                        const int32_t numberOfArrayElements,
                                                        const int32_t defaultValue)
: ArrayData(name,
            numberOfArrayElements),
m_integerArray(integerArray),
m_defaultValue(defaultValue)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::IntegerArrayData::restore(const SceneAttributes& /*sceneAttributes*/,
                                               const SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.getIntegerArrayValue(m_name, 
                                        m_integerArray, 
                                        m_numberOfArrayElements, 
                                        m_defaultValue);
    }
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void SceneClassAssistant::IntegerArrayData::save(const SceneAttributes& /*sceneAttributes*/,
                                                 SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.addIntegerArray(m_name, 
                                   m_integerArray, 
                                   m_numberOfArrayElements);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::IntegerArrayData
 * \brief Long integer array added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::LongIntegerArrayData::LongIntegerArrayData(const AString& name,
                                                                int64_t* integerArray,
                                                                const int32_t numberOfArrayElements,
                                                                const int64_t defaultValue)
: ArrayData(name,
            numberOfArrayElements),
m_longIntegerArray(integerArray),
m_defaultValue(defaultValue)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void
SceneClassAssistant::LongIntegerArrayData::restore(const SceneAttributes& /*sceneAttributes*/,
                                                   const SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.getLongIntegerArrayValue(m_name,
                                            m_longIntegerArray,
                                            m_numberOfArrayElements,
                                            m_defaultValue);
    }
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void SceneClassAssistant::LongIntegerArrayData::save(const SceneAttributes& /*sceneAttributes*/,
                                                     SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.addLongIntegerArray(m_name,
                                       m_longIntegerArray,
                                       m_numberOfArrayElements);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::IntegerArrayData
 * \brief Boolean array added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::UnsignedByteArrayData::UnsignedByteArrayData(const AString& name,
                                                        uint8_t* integerArray,
                                                        const int32_t numberOfArrayElements,
                                                        const uint8_t defaultValue)
: ArrayData(name,
            numberOfArrayElements),
m_integerArray(integerArray),
m_defaultValue(defaultValue)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void
SceneClassAssistant::UnsignedByteArrayData::restore(const SceneAttributes& /*sceneAttributes*/,
                                               const SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.getUnsignedByteArrayValue(m_name,
                                        m_integerArray,
                                        m_numberOfArrayElements,
                                        m_defaultValue);
    }
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void SceneClassAssistant::UnsignedByteArrayData::save(const SceneAttributes& /*sceneAttributes*/,
                                                 SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.addUnsignedByteArray(m_name,
                                   m_integerArray,
                                   m_numberOfArrayElements);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::FloatArrayData 
 * \brief Boolean array added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::FloatArrayData::FloatArrayData(const AString& name,
                                                        float* floatArray,
                                                        const int32_t numberOfArrayElements,
                                                        const float defaultValue)
: ArrayData(name,
            numberOfArrayElements),
m_floatArray(floatArray),
m_defaultValue(defaultValue)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::FloatArrayData::restore(const SceneAttributes& /*sceneAttributes*/,
                                               const SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.getFloatArrayValue(m_name, 
                                        m_floatArray, 
                                        m_numberOfArrayElements, 
                                        m_defaultValue);
    }
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void SceneClassAssistant::FloatArrayData::save(const SceneAttributes& /*sceneAttributes*/,
                                                 SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.addFloatArray(m_name, 
                                   m_floatArray, 
                                   m_numberOfArrayElements);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::StringArrayData 
 * \brief Boolean array added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
SceneClassAssistant::StringArrayData::StringArrayData(const AString& name,
                                                    AString* stringArray,
                                                    const int32_t numberOfArrayElements,
                                                    const AString defaultValue)
: ArrayData(name,
            numberOfArrayElements),
m_stringArray(stringArray),
m_defaultValue(defaultValue)
{
    
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::StringArrayData::restore(const SceneAttributes& /*sceneAttributes*/,
                                             const SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.getStringArrayValue(m_name, 
                                      m_stringArray, 
                                      m_numberOfArrayElements, 
                                      m_defaultValue);
    }
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
void SceneClassAssistant::StringArrayData::save(const SceneAttributes& /*sceneAttributes*/,
                                               SceneClass& sceneClass)
{
    if (m_numberOfArrayElements > 0) {
        sceneClass.addStringArray(m_name, 
                                 m_stringArray, 
                                 m_numberOfArrayElements);
    }
}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::ClassArrayData 
 * \brief Scene class added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
//SceneClassAssistant::ClassArrayData::ClassArrayData(const AString& name,
//                                                    SceneableInterface* sceneInterfaces[],
//                                                    const int32_t numberOfArrayElements)
//: ArrayData(name,
//            numberOfArrayElements),
//m_sceneInterfaces(sceneInterfaces)
//{
//    
//}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
//void 
//SceneClassAssistant::ClassArrayData::restore(const SceneAttributes& sceneAttributes,
//                                              const SceneClass& sceneClass)
//{
//    const SceneClassArray* classArray = sceneClass.getClassArray(m_name);
//    if (classArray != NULL) {
//        const int32_t numElem = std::min(m_numberOfArrayElements,
//                                         classArray->getNumberOfArrayElements());
//        for (int32_t i = 0; i < numElem; i++) {
//            m_sceneInterfaces[i]->restoreFromScene(&sceneAttributes, 
//                                                   classArray->getClassAtIndex(i));
//                                                    
//        }
//    }
//}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
//void SceneClassAssistant::ClassArrayData::save(const SceneAttributes& sceneAttributes,
//                                                SceneClass& sceneClass)
//{
//    std::vector<SceneClass*> elementVector;
//    
//    for (int32_t i = 0; i < m_numberOfArrayElements; i++) {
//        const AString elemName = (m_name
//                                  + "["
//                                  + AString::number(i)
//                                  + "]");
//        SceneClass* arrayElemClass = m_sceneInterfaces[i]->saveToScene(&sceneAttributes, 
//                                                                       elemName);
//        elementVector.push_back(arrayElemClass);
//    }
//    SceneClassArray* classArray = new SceneClassArray(m_name,
//                                                      elementVector);
//    sceneClass.addChild(classArray);
//}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::BooleanVectorData 
 * \brief Boolean vector added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */
/**
 * Constructor.
 * @param name
 *    Name of instance.
 * @param booleanVectorPointer
 *    Pointer to vector containing boolean data
 * @param defaultValue
 *    Default value used when restoring and data with name not found.
 */
//SceneClassAssistant::BooleanVectorData::BooleanVectorData(const AString& name,
//                                                          std::vector<bool>& booleanVectorReference,
//                                                          const bool defaultValue)
//: Data(name),
//  m_booleanVectorReference(booleanVectorReference),
//  m_defaultValue(defaultValue)
//{
//    
//}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class from  which data is restored.
 */
//void 
//SceneClassAssistant::BooleanVectorData::restore(const SceneAttributes& /*sceneAttributes*/,
//                                                const SceneClass& sceneClass)
//{
//    const int numElements = static_cast<int32_t>(m_booleanVectorReference.size());
//    if (numElements > 0) {
//        bool* boolArray = new bool[numElements];
//        
//        sceneClass.getBooleanArrayValue(m_name, 
//                                        boolArray, 
//                                        numElements, 
//                                        m_defaultValue);
//        
//        for (int32_t i = 0; i < numElements; i++) {
//            m_booleanVectorReference[i] = boolArray[i];
//        }
//        delete[] boolArray;
//    }
//}
//
//void 
//SceneClassAssistant::BooleanVectorData::save(const SceneAttributes& /*sceneAttributes*/,
//                                             SceneClass& sceneClass)
//{
//    /*
//     * std::vector<bool> is special case of vector. Cannot use &boolVector[0] for pointer address
//     */
//    const int numElements = static_cast<int32_t>(m_booleanVectorReference.size());
//    if (numElements > 0) {
//        bool* boolArray = new bool[numElements];
//        for (int32_t i = 0; i < numElements; i++) {
//            boolArray[i] = m_booleanVectorReference[i];
//        }
//        sceneClass.addBooleanArray(m_name, 
//                                   boolArray, 
//                                   numElements);
//        delete[] boolArray;
//    }
//}

/* ========================================================================= */
/**
 * \class caret::SceneClassAssistant::ClassData 
 * \brief Class added to a scene class.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 * @param name
 *    Name of instance.
 * @param className
 *    Name of class
 * @param sceneClassHandle
 *    Handle (Pointer to pointer) of the class instance.
 */
SceneClassAssistant::ClassData::ClassData(const AString& name,
                                            const AString& className,
                                            SceneableInterface** sceneClassHandle) 
: Data(name), 
  m_className(className),
  m_sceneClassHandle(sceneClassHandle),
  m_sceneClassPointer(NULL)
{
}

/**
 * Constructor.
 * @param name
 *    Name of instance.
 * @param className
 *    Name of class
 * @param sceneClassPointer
 *    Handle (Pointer to pointer) of the class instance.
 */
SceneClassAssistant::ClassData::ClassData(const AString& name,
                                          const AString& className,
                                          SceneableInterface* sceneClassPointer) 
: Data(name), 
m_className(className),
m_sceneClassHandle(NULL),
m_sceneClassPointer(sceneClassPointer)
{
}

/**
 * Restore the data from the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Parent scene class from which data is restored.
 */
void 
SceneClassAssistant::ClassData::restore(const SceneAttributes& sceneAttributes,
                                        const SceneClass& sceneClass)
{
    const SceneClass* mySceneClass = sceneClass.getClass(m_name);
    
    if (mySceneClass != NULL) {
        SceneableInterface* myClassInstance = ((m_sceneClassPointer != NULL) 
                                               ? m_sceneClassPointer 
                                               : *m_sceneClassHandle);
        if (myClassInstance != NULL) {
            myClassInstance->restoreFromScene(&sceneAttributes, 
                                              mySceneClass);
        }
    }
}

/**
 * Save the data to the scene.
 * @param sceneAttributes
 *    Attributes for the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::ClassData::save(const SceneAttributes& sceneAttributes,
                                     SceneClass& sceneClass)
{
    SceneableInterface* myClassInstance = ((m_sceneClassPointer != NULL) 
                                           ? m_sceneClassPointer 
                                           : *m_sceneClassHandle);
    if (myClassInstance != NULL) {
        sceneClass.addClass(myClassInstance->saveToScene(&sceneAttributes, m_name));
    }
}
