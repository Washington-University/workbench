
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

#define __SCENE_WRITER_XML_DECLARE__
#include "SceneWriterXml.h"
#undef __SCENE_WRITER_XML_DECLARE__

#include "CaretAssert.h"
#include "FileInformation.h"
#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneEnumeratedType.h"
#include "SceneEnumeratedTypeArray.h"
#include "SceneInfo.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePathName.h"
#include "ScenePathNameArray.h"
#include "ScenePrimitive.h"
#include "ScenePrimitiveArray.h"
#include "SceneXmlElements.h"
#include "XmlAttributes.h"
#include "XmlUtilities.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class caret::SceneWriterXml 
 * \brief Writes scenes to an XmlWriter.
 * \ingroup Scene
 *
 * See the documentation in the class Scene for how to use the Scene system.
 */

/**
 * Constructor.
 */
SceneWriterXml::SceneWriterXml(XmlWriter& xmlWriter,
                               const AString& sceneFileName)
: SceneWriterInterface(),
  m_xmlWriter(xmlWriter),
  m_sceneFileName(FileInformation(sceneFileName).getAbsoluteFilePath())
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
    
    m_xmlWriter.writeElementCData(SceneXmlElements::SCENE_DESCRIPTION_TAG, 
                                  scene.getDescription());
    
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
                            XmlUtilities::encodeXmlSpecialCharacters(sceneClass.getName()));
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
        const SceneObjectMapIntegerKey* sceneMapIntegerKey = dynamic_cast<const SceneObjectMapIntegerKey*>(sceneObject);
        const ScenePathName* scenePathName = dynamic_cast<const ScenePathName*>(sceneObject);
        const ScenePathNameArray* scenePathNameArray = dynamic_cast<const ScenePathNameArray*>(sceneObject);
        
        switch (sceneObject->getDataType()) {
            case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                break;
            case SceneObjectDataTypeEnum::SCENE_CLASS:
                break;
            case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                break;
            case SceneObjectDataTypeEnum::SCENE_FLOAT:
                break;
            case SceneObjectDataTypeEnum::SCENE_INTEGER:
                break;
            case SceneObjectDataTypeEnum::SCENE_LONG_INTEGER:
                break;
            case SceneObjectDataTypeEnum::SCENE_INVALID:
                break;
            case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                break;
            case SceneObjectDataTypeEnum::SCENE_STRING:
                break;
            case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                break;
        }
        
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
        else if (scenePathName != NULL) {
            const AString path = scenePathName->getRelativePathToSceneFile(m_sceneFileName);
            m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_TAG, 
                                          attributes,
                                          path);
        }
        else if (scenePathNameArray != NULL) {
            const int32_t numberOfArrayElements = scenePathNameArray->getNumberOfArrayElements();
            attributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE,
                                    numberOfArrayElements);
            m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_ARRAY_TAG,
                                          attributes);
            
            for (int32_t elementIndex = 0; elementIndex < numberOfArrayElements; elementIndex++) {
                XmlAttributes elementAttributes;
                elementAttributes.addAttribute(SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE,
                                               elementIndex);
                const ScenePathName* spn = scenePathNameArray->getScenePathNameAtIndex(elementIndex);
                const AString path = spn->getRelativePathToSceneFile(m_sceneFileName);
                m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG,
                                              elementAttributes,
                                              path);

//                m_xmlWriter.writeStartElement(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG,
//                                              elementAttributes);
//                m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_TAG,
//                                              attributes,
//                                              path);
//                m_xmlWriter.writeEndElement();
            }
            
            m_xmlWriter.writeEndElement();
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
                    case SceneObjectDataTypeEnum::SCENE_LONG_INTEGER:
                    case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                    {
                        const ScenePrimitive* primitive = dynamic_cast<const ScenePrimitive*>(sceneObject);
                        m_xmlWriter.writeElementCharacters(SceneXmlElements::OBJECT_MAP_VALUE_TAG,
                                                           valueAttributes,
                                                           primitive->stringValue());
                    }
                        break;
                    case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                    {
                        const ScenePathName* pathName = dynamic_cast<const ScenePathName*>(sceneObject);
                        const AString path = pathName->getRelativePathToSceneFile(m_sceneFileName);
                        m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_MAP_VALUE_TAG,
                                                      valueAttributes,
                                                      path);
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

