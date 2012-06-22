
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

#define __SCENE_CLASS_ASSISTANT_DECLARE__
#include "SceneClassAssistant.h"
#undef __SCENE_CLASS_ASSISTANT_DECLARE__

#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::SceneClassAssistant 
 * \brief Assists a class with restoring and saving its data to a scene.
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
 * @param name
 *    Name of member.
 * @param floatAddress
 *    Address of the member.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::add(const AString& name,
                         float* floatAddress,
                         const float defaultValue)
{
    FloatData* fd = new FloatData(name,
                                  floatAddress,
                                  defaultValue);
    m_dataStorage.push_back(fd);
}

/**
 * Add an integer member.
 * @param name
 *    Name of member.
 * @param integerAddress
 *    Address of the member.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::add(const AString& name,
                         int32_t* integerAddress,
                         const int32_t defaultValue)
{
    IntegerData* id = new IntegerData(name,
                                  integerAddress,
                                  defaultValue);
    m_dataStorage.push_back(id);
}

/**
 * Add a boolean member.
 * @param name
 *    Name of member.
 * @param booleanAddress
 *    Address of the member.
 * @param defaultValue
 *    Value used if the member is not found when restoring scene.
 */
void 
SceneClassAssistant::add(const AString& name,
                         bool* booleanAddress,
                         const bool defaultValue)
{
    BooleanData* bd = new BooleanData(name,
                                      booleanAddress,
                                      defaultValue);
    m_dataStorage.push_back(bd);
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
SceneClassAssistant::restoreMembers(const SceneAttributes& sceneAttributes,
                                    const SceneClass& sceneClass)
{
    for (DataStorageIterator iter = m_dataStorage.begin();
         iter != m_dataStorage.end();
         iter++) {
        Data* data = *iter;
        data->restore(sceneClass);
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
SceneClassAssistant::saveMembers(const SceneAttributes& sceneAttributes,
                                 SceneClass& sceneClass)
{
    for (DataStorageIterator iter = m_dataStorage.begin();
         iter != m_dataStorage.end();
         iter++) {
        Data* data = *iter;
        data->save(sceneClass);
    }
}

/* ========================================================================= */
/* Data */
SceneClassAssistant::Data::Data(const AString& name) 
: m_name(name) 
{
}

/* ========================================================================= */
/* FloatData */

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
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::FloatData::restore(const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getFloatValue(m_name, 
                                              m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::FloatData::save(SceneClass& sceneClass)
{
    sceneClass.addFloat(m_name, 
                        *m_dataPointer);
}
/* ========================================================================= */
/* IntegerData */

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
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::IntegerData::restore(const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getIntegerValue(m_name, 
                                              m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::IntegerData::save(SceneClass& sceneClass)
{
    sceneClass.addInteger(m_name, 
                        *m_dataPointer);
}

/* ========================================================================= */
/* BooleanData */

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
 * @param sceneClass
 *    Class from  which data is restored.
 */
void 
SceneClassAssistant::BooleanData::restore(const SceneClass& sceneClass)
{
    *m_dataPointer = sceneClass.getBooleanValue(m_name, 
                                                m_defaultValue);
}

/**
 * Save the data to the scene.
 * @param sceneClass
 *    Class to which data is saved.
 */
void 
SceneClassAssistant::BooleanData::save(SceneClass& sceneClass)
{
    sceneClass.addBoolean(m_name, 
                          *m_dataPointer);
}


