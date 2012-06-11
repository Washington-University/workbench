
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

#include "Scene.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneEnumeratedType.h"
#include "ScenePrimitive.h"
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
     * Write primitives.
     */
    const int32_t numberOfPrimitives = sceneClass.getNumberOfPrimitives();
    for (int32_t i = 0; i < numberOfPrimitives; i++) {
        const ScenePrimitive* primitive = sceneClass.getPrimitiveAtIndex(i);
        const AString& dataTypeName = SceneObjectDataTypeEnum::toXmlName(primitive->getDataType());
        
        XmlAttributes attributes;
        attributes.addAttribute(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                                dataTypeName);
        attributes.addAttribute(SceneXmlElements::OBJECT_NAME_ATTRIBUTE, 
                                primitive->getName());
        if (primitive->getDataType() == SceneObjectDataTypeEnum::SCENE_STRING) {
            m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_TAG, 
                                          attributes,
                                          primitive->stringValue());
        }
        else {
            m_xmlWriter.writeElementCharacters(SceneXmlElements::OBJECT_TAG, 
                                               attributes,
                                               primitive->stringValue());
        }
    }
    
    /*
     * Write enumerated types
     */
    const int32_t numberOfEnumeratedTypes = sceneClass.getNumberOfEnumeratedTypes();
    for (int32_t i = 0; i < numberOfEnumeratedTypes; i++) {
        const SceneEnumeratedType* enumeration = sceneClass.getEnumeratedTypeAtIndex(i);
        const AString& dataTypeName = SceneObjectDataTypeEnum::toXmlName(enumeration->getDataType());
        
        XmlAttributes attributes;
        attributes.addAttribute(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                                dataTypeName);
        attributes.addAttribute(SceneXmlElements::OBJECT_NAME_ATTRIBUTE, 
                                enumeration->getName());
        m_xmlWriter.writeElementCData(SceneXmlElements::OBJECT_TAG, 
                                      attributes,
                                      enumeration->stringValue());
    }
    
    /*
     * Write classes
     */
    const int32_t numberOfClasses = sceneClass.getNumberOfClasses();
    for (int32_t i = 0; i < numberOfClasses; i++) {
        const SceneClass* childClass = sceneClass.getClassAtIndex(i);
        writeSceneClass(*childClass);
    }
    
    /*
     * End class element.
     */
    m_xmlWriter.writeEndElement();
}



