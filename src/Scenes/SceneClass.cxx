
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

#define __SCENE_CLASS_DECLARE__
#include "SceneClass.h"
#undef __SCENE_CLASS_DECLARE__

#include "CaretAssert.h"
#include "SceneBoolean.h"
#include "SceneEnumeratedType.h"
#include "SceneFloat.h"
#include "SceneInteger.h"
#include "SceneString.h"
#include "XmlWriter.h"

using namespace caret;



/**
 * \class caret::SceneClass 
 * \brief For storage of a class instance in a scene.
 *
 * When an instance of a class is saved to a scene,
 * non-transient data is added to an instance of a
 * scene class.
 *
 * There are several types of scenes: (1) Full in which
 * all data is saved, and (2) Generic in which only 
 * non-file parameters are saved.
 * 
 * With full scenes, data files will be loaded and
 * the scene data is appled.
 *
 * With generic scenes, no data is loaded and the
 * scene is applied to any currently loaded data.
 */

/**
 * Constructor.
 *
 * @param name
 *     Name of the instance saved to this scene class.
 * @param versionNumber
 *     Version number of the class that is saved to this
 *     scene class.  Since a class may change over time,
 *     this version number can be used to handle cases
 *     of scenes saved prior to changes made to a class.
 */
SceneClass::SceneClass(const AString& name,
                       const int32_t versionNumber)
: SceneObject(name,
              SceneObjectDataTypeEnum::SCENE_CLASS),
  m_versionNumber(versionNumber)
{
    
}

/**
 * Destructor.
 */
SceneClass::~SceneClass()
{
    for (std::vector<SceneClass*>::iterator iter = m_childClasses.begin();
         iter != m_childClasses.end();
         iter++) {
        delete *iter;
    }
    m_childClasses.clear();
    
    for (std::vector<ScenePrimitive*>::iterator iter = m_childPrimitives.begin();
         iter != m_childPrimitives.end();
         iter++) {
        delete *iter;
    }
    m_childPrimitives.clear();

    for (std::vector<SceneEnumeratedType*>::iterator iter = m_childEnumeratedTypes.begin();
         iter != m_childEnumeratedTypes.end();
         iter++) {
        delete *iter;
    }
    m_childEnumeratedTypes.clear();
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
 * Add a child boolean value to the class.
 * 
 * @param name
 *    Name associated with value.
 * @param value
 *    The value.
 */
void SceneClass::addBoolean(const AString& name,
                            const bool value)
{
    m_childPrimitives.push_back(new SceneBoolean(name,
                                                 value));
}

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
    m_childClasses.push_back(sceneClass);    
}
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
    m_childEnumeratedTypes.push_back(new SceneEnumeratedType(name,
                                                             value));
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
    m_childPrimitives.push_back(new SceneFloat(name,
                                               value));
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
    m_childPrimitives.push_back(new SceneInteger(name,
                                                 value));
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
    m_childPrimitives.push_back(new SceneString(name,
                                                value));    
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
 * Find and return the child enumerated type value with the given name.
 * If no enumerated type value matches the name, the given default
 * value is returned.
 * @param name
 *    Name of the value.
 * @param defaultValue
 *    Value returned if the enumerated type value with the given
 *    name is not found.
 * @return
 *    The string value.
 */
AString 
SceneClass::getEnumeratedTypeValue(const AString& name,
                                   const AString& defaultValue) const
{
    for (std::vector<SceneEnumeratedType*>::const_iterator iter = m_childEnumeratedTypes.begin();
         iter != m_childEnumeratedTypes.end();
         iter++) {
        const SceneEnumeratedType* enumType = *iter;
        if (enumType->getName() == name) {
            return enumType->getEnumeratedValueAsString();
        }
    }
    
    return defaultValue;
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
    for (std::vector<ScenePrimitive*>::const_iterator iter = m_childPrimitives.begin();
         iter != m_childPrimitives.end();
         iter++) {
        const ScenePrimitive* primitive = *iter;
        if (primitive->getName() == name) {
            return primitive;
        }
    }
    
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
    for (std::vector<SceneClass*>::const_iterator iter = m_childClasses.begin();
         iter != m_childClasses.end();
         iter++) {
        const SceneClass* childClass = *iter;
        if (childClass->getName() == name) {
            return childClass;
        }
    }
    
    return NULL;
}

/**
 * Write the Scene Class to XML.
 * 
 * @param xmlWriter
 *    Writer that generates XML.
 * @throws
 *    XmlException if there is an error.
 */
void 
SceneClass::writeAsXML(XmlWriter& xmlWriter) const throw (XmlException)
{
    XmlAttributes atts;
    atts.addAttribute(XML_ATTRIBUTE_NAME, this->getName());
    
    xmlWriter.writeStartElement(SceneObjectDataTypeEnum::toName(this->getDataType()),
                                atts);
    
    for (std::vector<SceneEnumeratedType*>::const_iterator iter = m_childEnumeratedTypes.begin();
         iter != m_childEnumeratedTypes.end();
         iter++) {
        SceneObject* so = *iter;
        so->writeAsXML(xmlWriter);
    }
    
    for (std::vector<ScenePrimitive*>::const_iterator iter = m_childPrimitives.begin();
         iter != m_childPrimitives.end();
         iter++) {
        SceneObject* so = *iter;
        so->writeAsXML(xmlWriter);
    }
    
    for (std::vector<SceneClass*>::const_iterator iter = m_childClasses.begin();
         iter != m_childClasses.end();
         iter++) {
        SceneObject* so = *iter;
        so->writeAsXML(xmlWriter);
    }
    
    xmlWriter.writeEndElement();
}


