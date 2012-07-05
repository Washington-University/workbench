
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

#define __SCENE_WRITER_XML_DECLARE__
#include "SceneWriterXml.h"
#undef __SCENE_WRITER_XML_DECLARE__

#include "CaretAssert.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneEnumeratedType.h"
#include "SceneEnumeratedTypeArray.h"
#include "SceneMapIntegerKey.h"
#include "ScenePrimitive.h"
#include "ScenePrimitiveArray.h"
#include "SceneXmlElements.h"
#include "XmlAttributes.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class caret::SceneWriterXml 
 * \brief Writes scenes to an XmlWriter.
 */

/**
 * Constructor.
 */
SceneWriterXml::SceneWriterXml(XmlWriter& xmlWriter)
: SceneWriterInterface(),
  m_xmlWriter(xmlWriter)
{
}

/**
 * Destructor.
 */
SceneWriterXml::~SceneWriterXml()
{
    
}

/**
 * Write the given scene.
 * @param scene
 *    Scene that is written.
 * @throws XmlException
 *    If there is an error writing the scene as XML.
 */
void 
SceneWriterXml::writeScene(const Scene& scene,
                           const int32_t sceneIndex)
{
    /*
     * Type of scene
     */
    const SceneAttributes* sceneAttributes = scene.getAttributes();
    const AString sceneTypeName = SceneTypeEnum::toName(sceneAttributes->getSceneType());
    
    /*
     * Start scene element
     */
    XmlAttributes attributes;
    attributes.addAttribute(SceneXmlElements::SCENE_INDEX_ATTRIBUTE,
                                 sceneIndex);
    attributes.addAttribute(SceneXmlElements::SCENE_TYPE_ATTRIBUTE, 
                            sceneTypeName);
    m_xmlWriter.writeStartElement(SceneXmlElements::SCENE_TAG, 
                                  attributes);
    
    m_xmlWriter.writeElementCData(SceneXmlElements::SCENE_NAME_TAG, 
                                  scene.getName());
    
    /*
     * Write scene classes.
     */
    const int32_t numClasses = scene.getNumberOfClasses();
    for (int32_t i = 0; i < numClasses; i++) {
        writeSceneClass(*scene.getClassAtIndex(i));
    }
    
    /*
     * End scene element
     */
    m_xmlWriter.writeEndElement();
}

/**
 * Write then given class to a scene.
 * @param sceneClass
 *    Class that is written to the scene.
 */
void 
SceneWriterXml::writeSceneClass(const SceneClass& sceneClass)
{
    /*
     * Start class element.
     */
    const AString& dataTypeName = SceneObjectDataTypeEnum::toXmlName(sceneClass.getDataType());
    
    XmlAttributes attributes;
    attributes.addAttribute(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                            dataTypeName);
    attributes.addAttribute(SceneXmlElements::OBJECT_CLASS_ATTRIBUTE, 
                            sceneClass.getClassName());
    attributes.addAttribute(SceneXmlElements::OBJECT_NAME_ATTRIBUTE, 
                            sceneClass.getName());
    attributes.addAttribute(SceneXmlElements::OBJECT_VERSION_ATTRIBUTE, 
                            sceneClass.getVersionNumber());
    m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_TAG,
                                  attributes);
    
    /*
     * Write objects.
     */
    const int32_t numberOfObjects = sceneClass.getNumberOfObjects();
    for (int32_t i = 0; i < numberOfObjects; i++) {
        const SceneObject* sceneObject = sceneClass.getObjectAtIndex(i);
        const AString& dataTypeName = SceneObjectDataTypeEnum::toXmlName(sceneObject->getDataType());
        
        XmlAttributes attributes;
        attributes.addAttribute(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                                dataTypeName);
        attributes.addAttribute(SceneXmlElements::OBJECT_NAME_ATTRIBUTE, 
                                sceneObject->getName());
        const SceneEnumeratedType* sceneEnumeratedType = dynamic_cast<const SceneEnumeratedType*>(sceneObject);
        const SceneEnumeratedTypeArray* sceneEnumeratedTypeArray = dynamic_cast<const SceneEnumeratedTypeArray*>(sceneObject);
        const ScenePrimitive* scenePrimitive= dynamic_cast<const ScenePrimitive*>(sceneObject);
        const ScenePrimitiveArray* scenePrimitiveArray = dynamic_cast<const ScenePrimitiveArray*>(sceneObject);
        const SceneClass* sceneClass = dynamic_cast<const SceneClass*>(sceneObject);
        const SceneClassArray* sceneClassArray = dynamic_cast<const SceneClassArray*>(sceneObject);
        const SceneMapIntegerKey* sceneMapIntegerKey = dynamic_cast<const SceneMapIntegerKey*>(sceneObject);
        
        if (scenePrimitive != NULL) {
            if (scenePrimitive->getDataType() == SceneObjectDataTypeEnum::SCENE_STRING) {
                m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_TAG, 
                                              attributes,
                                              scenePrimitive->stringValue());
            }
            else {
                m_xmlWriter.writeElementCharacters(SceneXmlElements::OBJECT_TAG, 
                                                   attributes,
                                                   scenePrimitive->stringValue());
            }
        }
        else if (scenePrimitiveArray != NULL) {
            const int32_t numberOfArrayElements = scenePrimitiveArray->getNumberOfArrayElements();
            attributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE, 
                                    numberOfArrayElements);
            m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_ARRAY_TAG,
                                          attributes);
            
            for (int32_t elementIndex = 0; elementIndex < numberOfArrayElements; elementIndex++) {
                XmlAttributes elementAttributes;
                elementAttributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE,
                                               elementIndex);
                if (scenePrimitiveArray->getDataType() == SceneObjectDataTypeEnum::SCENE_STRING) {
                    m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG, 
                                                  elementAttributes,
                                                  scenePrimitiveArray->stringValue(elementIndex));
                }
                else {
                    m_xmlWriter.writeElementCharacters(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG, 
                                                       elementAttributes,
                                                       scenePrimitiveArray->stringValue(elementIndex));
                }
            }
            
            m_xmlWriter.writeEndElement();
        }
        else if (sceneClass != NULL) {
            writeSceneClass(*sceneClass);
        }
        else if (sceneClassArray != NULL) {
            const int32_t numberOfArrayElements = sceneClassArray->getNumberOfArrayElements();
            attributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE, 
                                    numberOfArrayElements);
            m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_ARRAY_TAG,
                                          attributes);
            
            for (int32_t elementIndex = 0; elementIndex < numberOfArrayElements; elementIndex++) {
                XmlAttributes elementAttributes;
                elementAttributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE,
                                               elementIndex);
                m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG,
                                              elementAttributes);
                writeSceneClass(*sceneClassArray->getClassAtIndex(elementIndex));
                m_xmlWriter.writeEndElement();
            }
            
            m_xmlWriter.writeEndElement();
        }
        else if (sceneEnumeratedType != NULL) {
            m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_TAG, 
                                          attributes,
                                          sceneEnumeratedType->stringValue());
        }
        else if (sceneEnumeratedTypeArray != NULL) {
            const int32_t numberOfArrayElements = sceneEnumeratedTypeArray->getNumberOfArrayElements();
            attributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE, 
                                    numberOfArrayElements);
            m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_ARRAY_TAG,
                                          attributes);
            
            for (int32_t elementIndex = 0; elementIndex < numberOfArrayElements; elementIndex++) {
                XmlAttributes elementAttributes;
                elementAttributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE,
                                               elementIndex);
                m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG, 
                                                  elementAttributes,
                                                  sceneEnumeratedTypeArray->stringValue(elementIndex));
            }
            
            m_xmlWriter.writeEndElement();
        }
        else if (sceneMapIntegerKey != NULL) {
            m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_MAP_TAG,
                                          attributes);
            
            const std::map<int32_t, SceneObject*>& sceneMap = sceneMapIntegerKey->getMap();
            for (std::map<int32_t, SceneObject*>::const_iterator iter = sceneMap.begin();
                 iter != sceneMap.end();
                 iter++) {
                const int32_t key = iter->first;
                
                XmlAttributes valueAttributes;
                valueAttributes.addAttribute(SceneXmlElements::OBJECT_MAP_VALUE_KEY_ATTRIBUTE, 
                                             key);
                
                const SceneObject* sceneObject = iter->second;
                switch (sceneMapIntegerKey->getDataType()) {
                    case SceneObjectDataTypeEnum::SCENE_CLASS:
                    {
                        const SceneClass* sceneClass = dynamic_cast<const SceneClass*>(sceneObject);
                        m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_MAP_VALUE_TAG,
                                                      valueAttributes);
                        writeSceneClass(*sceneClass);
                        m_xmlWriter.writeEndElement();
                    }
                        break;
                    case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                    {
                        const SceneEnumeratedType* sceneEnumType = dynamic_cast<const SceneEnumeratedType*>(sceneObject);
                        m_xmlWriter.writeElementCharacters(SceneXmlElements::OBJECT_MAP_VALUE_TAG,
                                                           valueAttributes,
                                                           sceneEnumType->stringValue());
                    }
                        break;
                    case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                    case SceneObjectDataTypeEnum::SCENE_FLOAT:
                    case SceneObjectDataTypeEnum::SCENE_INTEGER:
                    {
                        const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(sceneObject);
                        m_xmlWriter.writeElementCharacters(SceneXmlElements::OBJECT_MAP_VALUE_TAG,
                                                           valueAttributes,
                                                           primitive->stringValue());
                    }
                        break;
                    case SceneObjectDataTypeEnum::SCENE_STRING:
                    {
                        const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(sceneObject);
                        m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_MAP_VALUE_TAG,
                                                      valueAttributes,
                                                      primitive->stringValue());
                    }
                        break;
                    case SceneObjectDataTypeEnum::SCENE_INVALID:
                        CaretAssert(0); 
                        break;
                }   
            }
            
            m_xmlWriter.writeEndElement();
        }
        else {
            CaretAssertMessage(0, 
                               ("Unknown scene object type="
                                + dataTypeName));
        }
    }
    
    /*
     * End class element.
     */
    m_xmlWriter.writeEndElement();
}



