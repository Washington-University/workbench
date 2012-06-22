/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include <sstream>

#include "CaretLogger.h"
#include "Scene.h"
#include "SceneClass.h"
#include "SceneSaxReader.h"
#include "SceneXmlElements.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * constructor.
 */
SceneSaxReader::SceneSaxReader(Scene* scene)
{
    m_state = STATE_NONE;
    m_stateStack.push(m_state);
    m_elementText = "";
    m_scene = scene;
}

/**
 * destructor.
 */
SceneSaxReader::~SceneSaxReader()
{
}


/**
 * start an element.
 */
void 
SceneSaxReader::startElement(const AString& /* namespaceURI */,
                             const AString& /* localName */,
                             const AString& qName,
                             const XmlAttributes& attributes)  throw (XmlSaxParserException)
{
    const STATE previousState = m_state;
    switch (m_state) {
        case STATE_NONE:
            if (qName == SceneXmlElements::SCENE_TAG) {
                m_state = STATE_SCENE;
            }
            else {
                const AString msg = ("While reading Scene XML, expected the XML tag to be "
                                     + SceneXmlElements::SCENE_TAG
                                     + " but found "
                                     + qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_SCENE:
            if (qName == SceneXmlElements::SCENE_NAME_TAG) {
                m_state = STATE_NAME;
            }
            else if (qName == SceneXmlElements::OBJECT_TAG) {
                m_state = STATE_OBJECT;
                processObjectStartTag(attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::SCENE_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_NAME:
            break;
        case STATE_OBJECT:
            if (qName == SceneXmlElements::OBJECT_TAG) {
                processObjectStartTag(attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::OBJECT_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
    }
    
    //
    // Save previous state
    //
    m_stateStack.push(previousState);
    
    m_elementText = "";
}

/**
 * Process an Object start tag.
 * @param attributes
 *     Attributes contained in the Object tag.
 */
void 
SceneSaxReader::processObjectStartTag(const XmlAttributes& attributes) throw (XmlSaxParserException)
{
    const AString objectTypeName  = attributes.getValue(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE);
    const AString objectName      = attributes.getValue(SceneXmlElements::OBJECT_NAME_ATTRIBUTE);
    const AString objectClassName = attributes.getValue(SceneXmlElements::OBJECT_CLASS_ATTRIBUTE);
    const int32_t objectVersion   = attributes.getValueAsInt(SceneXmlElements::OBJECT_VERSION_ATTRIBUTE);
    
    bool validObjectType = false;
    const SceneObjectDataTypeEnum::Enum objectDataType = SceneObjectDataTypeEnum::fromXmlName(objectTypeName,
                                                                                              &validObjectType);
    if (validObjectType == false) {
        const AString msg = XmlUtilities::createInvalidAttributeMessage(SceneXmlElements::OBJECT_TAG, 
                                                                        SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                                                                        objectTypeName);
        XmlSaxParserException e(msg);
        CaretLogThrowing(e);
        throw e;
    }
    
    m_objectBeingReadDataType = objectDataType;
    m_objectBeingReadName     = "";
    
    switch (objectDataType) {
        case SceneObjectDataTypeEnum::SCENE_CLASS:
        {
            SceneClass* sceneClass = new SceneClass(objectName,
                                                    objectClassName,
                                                    objectVersion);
            m_sceneClassStack.push(sceneClass);
        }
            break;
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
        case SceneObjectDataTypeEnum::SCENE_STRING:
            m_objectBeingReadDataType = objectDataType;
            m_objectBeingReadName     = objectName;
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            break;
    }    
}


/**
 * end an element.
 */
void 
SceneSaxReader::endElement(const AString& /* namspaceURI */,
                           const AString& /* localName */,
                           const AString& /*qName*/) throw (XmlSaxParserException)
{
    switch (m_state) {
        case STATE_NONE:
            break;
        case STATE_SCENE:
        {
            if (m_sceneClassStack.empty() == false) {
                SceneClass* sceneClass = m_sceneClassStack.top();
                m_sceneClassStack.pop();
                if (m_sceneClassStack.empty()) {
                    m_scene->addClass(sceneClass);
                    
                    std::cout 
                    << "Adding class named "
                    << qPrintable(sceneClass->getName())
                    << " to parent scene named "
                    << qPrintable(m_scene->getName())
                    << std::endl;
                }
            }
        }
            break;
        case STATE_NAME:
            m_scene->setName(m_elementText);
            break;
        case STATE_OBJECT:
            const AString stringValue = m_elementText;
            std::cout << "Value for object " << qPrintable(m_objectBeingReadName) << "=" << qPrintable(stringValue) << std::endl;
            switch (m_objectBeingReadDataType) {
                case SceneObjectDataTypeEnum::SCENE_CLASS:
                {
                    if (m_sceneClassStack.empty() == false) {
                        SceneClass* sceneClass = m_sceneClassStack.top();
                        m_sceneClassStack.pop();
                        
                        SceneClass* parentSceneClass = m_sceneClassStack.top();
                        parentSceneClass->addClass(sceneClass);
                        
                        std::cout 
                        << "Adding class named "
                        << qPrintable(sceneClass->getName())
                        << " to parent class named "
                        << qPrintable(parentSceneClass->getName())
                        << std::endl;
                    }
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                {
                    const bool value = stringValue.toBool();
                    SceneClass* sc = m_sceneClassStack.top();
                    sc->addBoolean(m_objectBeingReadName, 
                                   value);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_FLOAT:
                {
                    const float value = stringValue.toFloat();
                    SceneClass* sc = m_sceneClassStack.top();
                    sc->addFloat(m_objectBeingReadName, 
                                 value);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                {
                    SceneClass* sc = m_sceneClassStack.top();
                    sc->addEnumeratedType(m_objectBeingReadName, 
                                          stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INTEGER:
                {
                    const int32_t value = stringValue.toInt();
                    SceneClass* sc = m_sceneClassStack.top();
                    sc->addInteger(m_objectBeingReadName, 
                                   value);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_STRING:
                {
                    SceneClass* sc = m_sceneClassStack.top();
                    sc->addString(m_objectBeingReadName, 
                                  stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INVALID:
                    break;
            }    
            
            if (m_objectBeingReadDataType != SceneObjectDataTypeEnum::SCENE_CLASS) {
                m_objectBeingReadDataType = SceneObjectDataTypeEnum::SCENE_INVALID;
                m_objectBeingReadName     = "";
            }
            break;
    }
    
    //
    // Clear out for new elements
    //
    m_elementText = "";
    
    //
    // Go to previous state
    //
    if (m_stateStack.empty()) {
        throw XmlSaxParserException("State stack is empty while reading Scene.");
    }
    m_state = m_stateStack.top();
    m_stateStack.pop();
}


/**
 * get characters in an element.
 */
void 
SceneSaxReader::characters(const char* ch) throw (XmlSaxParserException)
{
    m_elementText += ch;
    std::cout << "Element text is: " << qPrintable(m_elementText) << std::endl;
}

/**
 * a fatal error occurs.
 */
void 
SceneSaxReader::fatalError(const XmlSaxParserException& e) throw (XmlSaxParserException)
{
    throw e;
}

/**
 * A warning occurs
 */
void 
SceneSaxReader::warning(const XmlSaxParserException& e) throw (XmlSaxParserException)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SceneSaxReader::error(const XmlSaxParserException& e) throw (XmlSaxParserException)
{   
    throw e;
}

void 
SceneSaxReader::startDocument()  throw (XmlSaxParserException)
{    
}

void 
SceneSaxReader::endDocument() throw (XmlSaxParserException)
{
}

