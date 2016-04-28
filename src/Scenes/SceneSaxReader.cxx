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

#include <limits>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFile.h"
#include "Scene.h"
#include "SceneBoolean.h"
#include "SceneBooleanArray.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneEnumeratedType.h"
#include "SceneEnumeratedTypeArray.h"
#include "SceneFloat.h"
#include "SceneFloatArray.h"
#include "SceneInteger.h"
#include "SceneIntegerArray.h"
#include "SceneObjectMapIntegerKey.h"
#include "ScenePathName.h"
#include "ScenePathNameArray.h"
#include "SceneSaxReader.h"
#include "SceneString.h"
#include "SceneStringArray.h"
#include "SceneUnsignedByte.h"
#include "SceneUnsignedByteArray.h"
#include "SceneXmlElements.h"

#include "XmlAttributes.h"
#include "XmlException.h"
#include "XmlUtilities.h"

using namespace caret;

static bool debugFlag = false;

/**
 * constructor.
 * @param sceneFileName
 *   Name of scene file being read.
 * @param scene
 *   Scene that is being read.
 */
SceneSaxReader::SceneSaxReader(const AString& sceneFileName,
                               Scene* scene)
: m_sceneFileName(sceneFileName)
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
                             const XmlAttributes& attributes) 
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
                m_state = STATE_SCENE_NAME;
            }
            else if (qName == SceneXmlElements::SCENE_DESCRIPTION_TAG) {
                m_state = STATE_SCENE_DESCRIPTION;
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
        case STATE_SCENE_NAME:
            break;
        case STATE_SCENE_DESCRIPTION:
            break;
        case STATE_OBJECT:
            if (qName == SceneXmlElements::OBJECT_TAG) {
                m_state = STATE_OBJECT;
                processObjectStartTag(attributes);
            }
            else if (qName == SceneXmlElements::OBJECT_ARRAY_TAG) {
                m_state = STATE_OBJECT_ARRAY;
                processObjectArrayStartTag(attributes);
            }
            else if (qName == SceneXmlElements::OBJECT_MAP_TAG) {
                m_state = STATE_OBJECT_MAP;
                processObjectMapStartTag(attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::OBJECT_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_OBJECT_ARRAY:
            if (qName == SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG) {
                const int32_t elementIndex = attributes.getValueAsInt(SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE,
                                                                      -1);
                if (elementIndex < 0) {
                    AString msg = XmlUtilities::createInvalidAttributeMessage(qName, 
                                                                              SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE, 
                                                                              attributes.getValue(SceneXmlElements::OBJECT_ARRAY_ELEMENT_INDEX_ATTRIBUTE));
                    msg += ("  Must be greater than or equal to zero.");
                    XmlSaxParserException e(msg);
                    CaretLogThrowing(e);
                    throw e;
                }
                m_objectArrayBeingReadElementIndexStack.push(elementIndex);
                m_state = STATE_OBJECT_ARRAY_ELEMENT;
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::OBJECT_ARRAY_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_OBJECT_ARRAY_ELEMENT:
            if (qName == SceneXmlElements::OBJECT_TAG) {
                m_state = STATE_OBJECT;
                processObjectStartTag(attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::OBJECT_ARRAY_ELEMENT_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_OBJECT_MAP:
            if (qName == SceneXmlElements::OBJECT_MAP_VALUE_TAG) {
                const int32_t key = attributes.getValueAsInt(SceneXmlElements::OBJECT_MAP_VALUE_KEY_ATTRIBUTE,
                                                             -1);
                if (key < 0) {
                    AString msg = XmlUtilities::createInvalidAttributeMessage(qName, 
                                                                              SceneXmlElements::OBJECT_MAP_VALUE_KEY_ATTRIBUTE, 
                                                                              attributes.getValue(SceneXmlElements::OBJECT_MAP_VALUE_KEY_ATTRIBUTE));
                    msg += ("  Must be greater than or equal to zero.");
                    XmlSaxParserException e(msg);
                    CaretLogThrowing(e);
                    throw e;
                }
                m_objectMapBeingReadValueKeyStack.push(key);
                m_state = STATE_OBJECT_MAP_VALUE;
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::OBJECT_MAP_VALUE_TAG, 
                                                                                   qName);
                XmlSaxParserException e(msg);
                CaretLogThrowing(e);
                throw e;
            }
            break;
        case STATE_OBJECT_MAP_VALUE:
            if (qName == SceneXmlElements::OBJECT_TAG) {
                m_state = STATE_OBJECT;
                processObjectStartTag(attributes);
            }
            else {
                const AString msg = XmlUtilities::createInvalidChildElementMessage(SceneXmlElements::OBJECT_MAP_VALUE_TAG, 
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
 * @param tag
 *     Tag that was read indicating Object, ObjectArray, etc.
 * @param attributes
 *     Attributes contained in the Object tag.
 */
void 
SceneSaxReader::processObjectStartTag(const XmlAttributes& attributes)
{
    /*
     * Get attributes of the object element
     */
    const AString objectTypeName  = attributes.getValue(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE);
    const AString objectName      = attributes.getValue(SceneXmlElements::OBJECT_NAME_ATTRIBUTE);
    const AString objectClassName = attributes.getValue(SceneXmlElements::OBJECT_CLASS_ATTRIBUTE);
    const int32_t objectVersion   = attributes.getValueAsInt(SceneXmlElements::OBJECT_VERSION_ATTRIBUTE);
    
    /*
     * Get the type of the object
     */
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
    
    SceneObject* sceneObject = NULL;
    switch (objectDataType) {
        case SceneObjectDataTypeEnum::SCENE_CLASS:
            sceneObject = new SceneClass(objectName,
                                         objectClassName,
                                         objectVersion);
            break;
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            sceneObject = new SceneBoolean(objectName,
                                           false);
            break;
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
            sceneObject = new SceneFloat(objectName,
                                         0.0);
            break;
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            sceneObject = new SceneEnumeratedType(objectName,
                                                  "");
            break;
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
            sceneObject = new SceneInteger(objectName,
                                           0);
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            sceneObject = new ScenePathName(objectName,
                                            "");
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            sceneObject = new SceneString(objectName,
                                          "");
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            sceneObject = new SceneUnsignedByte(objectName,
                                                0);
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            CaretAssert(0);  // should never get here, 'validObjectType' above
            break;
    }   
    
    /*
     * Track object being read to ensure proper parenting of children objects
     */
    CaretAssert(sceneObject);
    m_objectBeingReadStack.push(sceneObject);
    if (debugFlag) std::cout << "Pushed Object:" << qPrintable(sceneObject->getName()) << " Type=" << qPrintable(objectTypeName) << std::endl;
}

/**
 * Process an ObjectArray start tag.
 * @param tag
 *     Tag that was read indicating Object, ObjectArray, etc.
 * @param attributes
 *     Attributes contained in the Object tag.
 */
void 
SceneSaxReader::processObjectArrayStartTag(const XmlAttributes& attributes)
{
    /*
     * Get attributes of the object element
     */
    const AString objectTypeName  = attributes.getValue(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE);
    const AString objectName      = attributes.getValue(SceneXmlElements::OBJECT_NAME_ATTRIBUTE);
    const AString objectClassName = attributes.getValue(SceneXmlElements::OBJECT_CLASS_ATTRIBUTE);
    
    const int32_t objectNumberOfElements = attributes.getValueAsInt(SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE,
                                                                    -1);
    if (objectNumberOfElements < 0) {
        AString msg = XmlUtilities::createInvalidAttributeMessage(SceneXmlElements::OBJECT_ARRAY_TAG, 
                                                                  SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE, 
                                                                  attributes.getValue(SceneXmlElements::OBJECT_ARRAY_LENGTH_ATTRIBUTE));
        msg += ("  Must be greater than or equal to zero.");
        XmlSaxParserException e(msg);
        CaretLogThrowing(e);
        throw e;
    }
    
    /*
     * Get the type of the object
     */
    bool validObjectType = false;
    const SceneObjectDataTypeEnum::Enum objectDataType = SceneObjectDataTypeEnum::fromXmlName(objectTypeName,
                                                                                              &validObjectType);
    if (validObjectType == false) {
        const AString msg = XmlUtilities::createInvalidAttributeMessage(SceneXmlElements::OBJECT_ARRAY_TAG, 
                                                                        SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                                                                        objectTypeName);
        XmlSaxParserException e(msg);
        CaretLogThrowing(e);
        throw e;
    }
    
    SceneObject* sceneObject = NULL;
    switch (objectDataType) {
        case SceneObjectDataTypeEnum::SCENE_CLASS:
            sceneObject = new SceneClassArray(objectName,
                                              objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            sceneObject = new SceneBooleanArray(objectName, 
                                                objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
            sceneObject = new SceneFloatArray(objectName, 
                                              objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            sceneObject = new SceneEnumeratedTypeArray(objectName, 
                                                       objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
            sceneObject = new SceneIntegerArray(objectName, 
                                                objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            sceneObject = new ScenePathNameArray(objectName,
                                                 objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            sceneObject = new SceneStringArray(objectName, 
                                               objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            sceneObject = new SceneUnsignedByteArray(objectName,
                                                     objectNumberOfElements);
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            break;
    }   
    
    /*
     * Track object being read to ensure proper parenting of children objects
     */
    CaretAssert(sceneObject);
    m_objectBeingReadStack.push(sceneObject);
    if (debugFlag) std::cout << "Pushed ObjectArray:" << qPrintable(sceneObject->getName()) << " Type=" << qPrintable(objectTypeName) << std::endl;
}

/**
 * Process an ObjectMap start tag.
 * @param tag
 *     Tag that was read indicating Object, ObjectArray, etc.
 * @param attributes
 *     Attributes contained in the Object tag.
 */
void 
SceneSaxReader::processObjectMapStartTag(const XmlAttributes& attributes)
{
    /*
     * Get attributes of the object element
     */
    const AString objectTypeName  = attributes.getValue(SceneXmlElements::OBJECT_TYPE_ATTRIBUTE);
    const AString objectName      = attributes.getValue(SceneXmlElements::OBJECT_NAME_ATTRIBUTE);
    const AString objectClassName = attributes.getValue(SceneXmlElements::OBJECT_CLASS_ATTRIBUTE);
    
    /*
     * Get the type of the object
     */
    bool validObjectType = false;
    const SceneObjectDataTypeEnum::Enum objectDataType = SceneObjectDataTypeEnum::fromXmlName(objectTypeName,
                                                                                              &validObjectType);
    if (validObjectType == false) {
        const AString msg = XmlUtilities::createInvalidAttributeMessage(SceneXmlElements::OBJECT_ARRAY_TAG, 
                                                                        SceneXmlElements::OBJECT_TYPE_ATTRIBUTE, 
                                                                        objectTypeName);
        XmlSaxParserException e(msg);
        CaretLogThrowing(e);
        throw e;
    }
    
    SceneObjectMapIntegerKey* sceneMap = new SceneObjectMapIntegerKey(objectName, 
                                                          objectDataType);
    /*
     * Track object being read to ensure proper parenting of children objects
     */
    CaretAssert(sceneMap);
    m_objectBeingReadStack.push(sceneMap);
    if (debugFlag) std::cout << "Pushed ObjectMap:" << qPrintable(sceneMap->getName()) << " Type=" << qPrintable(objectTypeName) << std::endl;
}

/**
 * end an element.
 */
void 
SceneSaxReader::endElement(const AString& /* namspaceURI */,
                           const AString& /* localName */,
                           const AString& /*qName*/)
{
    const AString stringValue = m_elementText.trimmed();
    
    switch (m_state) {
        case STATE_NONE:
            break;
        case STATE_SCENE:
            break;
        case STATE_SCENE_NAME:
            m_scene->setName(stringValue);
            break;
        case STATE_SCENE_DESCRIPTION:
            m_scene->setDescription(stringValue);
            break;
        case STATE_OBJECT:
        {
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneObject* sceneObject = m_objectBeingReadStack.top();
            m_objectBeingReadStack.pop();
            if (debugFlag) std::cout << "Popped Object:" << qPrintable(sceneObject->getName()) << std::endl;
            
            switch (sceneObject->getDataType()) {
                case SceneObjectDataTypeEnum::SCENE_CLASS:
                {
                    SceneClass* sceneClass = dynamic_cast<SceneClass*>(sceneObject);
                    CaretAssert(sceneClass);
                    if (m_objectBeingReadStack.empty()) {
                        /*
                         * Parent must be the scene
                         */
                        m_scene->addClass(sceneClass);
                    }
                    else {
                        /*
                         * Parent is another class
                         */
                        CaretAssert(m_objectBeingReadStack.empty() == false);
                        SceneClass* parentSceneClass = dynamic_cast<SceneClass*>(m_objectBeingReadStack.top());
                        SceneClassArray* parentSceneClassArray = dynamic_cast<SceneClassArray*>(m_objectBeingReadStack.top());
                        SceneObjectMapIntegerKey* parentSceneObjectMapIntegerKey = dynamic_cast<SceneObjectMapIntegerKey*>(m_objectBeingReadStack.top());
                        if (parentSceneClass != NULL) {
                            parentSceneClass->addClass(sceneClass);
                        }
                        else if (parentSceneClassArray != NULL) {
                            parentSceneClassArray->setClassAtIndex(m_objectArrayBeingReadElementIndexStack.top(), 
                                                                   sceneClass);
                        }
                        else if (parentSceneObjectMapIntegerKey != NULL) {
                            parentSceneObjectMapIntegerKey->addClass(m_objectMapBeingReadValueKeyStack.top(), 
                                                               sceneClass);
                        }
                    }
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                {
                    SceneBoolean* sceneBoolean = dynamic_cast<SceneBoolean*>(sceneObject);
                    CaretAssert(sceneBoolean);
                    const bool value = stringValue.toBool();
                    sceneBoolean->setValue(value);
                    addChildToParentClass(sceneBoolean);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_FLOAT:
                {
                    SceneFloat* sceneFloat = dynamic_cast<SceneFloat*>(sceneObject);
                    CaretAssert(sceneFloat);
                    const float value = stringValue.toFloat();
                    sceneFloat->setValue(value);
                    addChildToParentClass(sceneFloat);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                {
                    SceneEnumeratedType* sceneEnum = dynamic_cast<SceneEnumeratedType*>(sceneObject);
                    CaretAssert(sceneEnum);
                    sceneEnum->setValue(stringValue);
                    addChildToParentClass(sceneEnum);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INTEGER:
                {
                    SceneInteger* sceneInteger = dynamic_cast<SceneInteger*>(sceneObject);
                    CaretAssert(sceneInteger);
                    const int32_t value = stringValue.toInt();
                    sceneInteger->setValue(value);
                    addChildToParentClass(sceneInteger);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                {
                    ScenePathName* scenePathName = dynamic_cast<ScenePathName*>(sceneObject);
                    CaretAssert(scenePathName);
                    scenePathName->setValueToAbsolutePath(m_sceneFileName, 
                                                          stringValue);
                    addChildToParentClass(scenePathName);
                    
                    if (DataFile::isFileOnNetwork(stringValue)) {
                        m_scene->setHasFilesWithRemotePaths(true);
                    }
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_STRING:
                {
                    SceneString* sceneString = dynamic_cast<SceneString*>(sceneObject);
                    CaretAssert(sceneString);
                    sceneString->setValue(stringValue);
                    addChildToParentClass(sceneString);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                {
                    SceneUnsignedByte* sceneUnsignedByte = dynamic_cast<SceneUnsignedByte*>(sceneObject);
                    CaretAssert(sceneUnsignedByte);
                    uint32_t value = stringValue.toUInt();
                    if (value > std::numeric_limits<uint8_t>::max()) {
                        value = std::numeric_limits<uint8_t>::max();
                    }
                    const uint8_t byteValue = static_cast<uint8_t>(value);
                    sceneUnsignedByte->setValue(byteValue);
                    addChildToParentClass(sceneUnsignedByte);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INVALID:
                    break;
            }    
        }
            break;
        case STATE_OBJECT_ARRAY:
        {
            /**
             * Get the array.
             */
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneObject* sceneObject = m_objectBeingReadStack.top();
            m_objectBeingReadStack.pop();
            if (debugFlag) std::cout << "Popped ObjectArray:" << qPrintable(sceneObject->getName()) << std::endl;
            
            SceneObjectArray* sceneArray = dynamic_cast<SceneObjectArray*>(sceneObject);
            CaretAssert(sceneArray);
            
            /*
             * Parent is another class
             */
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneClass* parentSceneClass = dynamic_cast<SceneClass*>(m_objectBeingReadStack.top());
            CaretAssert(parentSceneClass);
            parentSceneClass->addChild(sceneArray);
        }
            break;
        case STATE_OBJECT_ARRAY_ELEMENT:
        {
            /**
             * Get the array.
             */
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneObject* sceneObject = m_objectBeingReadStack.top();
            SceneObjectArray* sceneArray = dynamic_cast<SceneObjectArray*>(sceneObject);
            CaretAssert(sceneArray);
            
            switch (sceneArray->getDataType()) {
                case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                {
                    SceneBooleanArray* booleanArray = dynamic_cast<SceneBooleanArray*>(sceneArray);
                    CaretAssert(booleanArray);
                    booleanArray->setValue(m_objectArrayBeingReadElementIndexStack.top(),
                                           stringValue.toBool());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_CLASS:
                    /*
                     * Nothing to do here, handled in "case STATE_OBJECT" above
                     */
                    break;
                case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                {
                    SceneEnumeratedTypeArray* enumeratedArray = dynamic_cast<SceneEnumeratedTypeArray*>(sceneArray);
                    CaretAssert(enumeratedArray);
                    enumeratedArray->setValue(m_objectArrayBeingReadElementIndexStack.top(),
                                              stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_FLOAT:
                {
                    SceneFloatArray* floatArray = dynamic_cast<SceneFloatArray*>(sceneArray);
                    CaretAssert(floatArray);
                    floatArray->setValue(m_objectArrayBeingReadElementIndexStack.top(),
                                         stringValue.toFloat());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INTEGER:
                {
                    SceneIntegerArray* integerArray = dynamic_cast<SceneIntegerArray*>(sceneArray);
                    CaretAssert(integerArray);
                    integerArray->setValue(m_objectArrayBeingReadElementIndexStack.top(),
                                           stringValue.toInt());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INVALID:
                {
                    CaretAssert(0);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                {
                    ScenePathNameArray* pathNameArray = dynamic_cast<ScenePathNameArray*>(sceneArray);
                    CaretAssert(pathNameArray);
                    pathNameArray->setScenePathNameAtIndex(m_objectArrayBeingReadElementIndexStack.top(),
                                                           m_sceneFileName,
                                                           stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_STRING:
                {
                    SceneStringArray* stringArray = dynamic_cast<SceneStringArray*>(sceneArray);
                    CaretAssert(stringArray);
                    stringArray->setValue(m_objectArrayBeingReadElementIndexStack.top(),
                                          stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                {
                    SceneUnsignedByteArray* unsignedByteArray = dynamic_cast<SceneUnsignedByteArray*>(sceneArray);
                    CaretAssert(unsignedByteArray);
                    uint32_t i = stringValue.toUInt();
                    if (i > std::numeric_limits<uint8_t>::max()) {
                        i = std::numeric_limits<uint8_t>::max();
                    }
                    const uint8_t value = static_cast<uint8_t>(i);
                    unsignedByteArray->setValue(m_objectArrayBeingReadElementIndexStack.top(),
                                                value);
                }
                    break;
            }
            
            m_objectArrayBeingReadElementIndexStack.pop();
            
        }
            break;
        case STATE_OBJECT_MAP:
        {
            /**
             * Get the map.
             */
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneObject* sceneObject = m_objectBeingReadStack.top();
            m_objectBeingReadStack.pop();
            if (debugFlag) std::cout << "Popped ObjectMap:" << qPrintable(sceneObject->getName()) << std::endl;
            
            SceneObjectMapIntegerKey* sceneMap = dynamic_cast<SceneObjectMapIntegerKey*>(sceneObject);
            CaretAssert(sceneMap);
            
            /*
             * Parent is another class
             */
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneClass* parentSceneClass = dynamic_cast<SceneClass*>(m_objectBeingReadStack.top());
            CaretAssert(parentSceneClass);
            parentSceneClass->addChild(sceneMap);
        }
            break;
        case STATE_OBJECT_MAP_VALUE:
        {
            /**
             * Get the map.
             */
            CaretAssert(m_objectBeingReadStack.empty() == false);
            SceneObject* sceneObject = m_objectBeingReadStack.top();
            SceneObjectMapIntegerKey* sceneMap = dynamic_cast<SceneObjectMapIntegerKey*>(sceneObject);
            CaretAssert(sceneMap);
            
            const int32_t key = m_objectMapBeingReadValueKeyStack.top();
            m_objectMapBeingReadValueKeyStack.pop();
            
            switch (sceneMap->getDataType()) {
                case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                {
                    sceneMap->addBoolean(key,
                                         stringValue.toBool());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_CLASS:
                    /*
                     * Nothing to do here, handled in "case STATE_OBJECT" above
                     */
                    break;
                case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                {
                    sceneMap->addEnumeratedType(key,
                                                stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_FLOAT:
                {
                    sceneMap->addFloat(key,
                                       stringValue.toFloat());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INTEGER:
                {
                    sceneMap->addInteger(key,
                                         stringValue.toInt());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_INVALID:
                {
                    CaretAssert(0);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                {
                    ScenePathName spn("spn", "");
                    spn.setValueToAbsolutePath(m_sceneFileName, 
                                               stringValue);
                    sceneMap->addPathName(key,
                                          spn.stringValue());
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_STRING:
                {
                    sceneMap->addString(key,
                                        stringValue);
                }
                    break;
                case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                {
                    uint32_t i = stringValue.toUInt();
                    if (i > std::numeric_limits<uint8_t>::max()) {
                        i = std::numeric_limits<uint8_t>::max();
                    }
                    const uint8_t value = static_cast<uint8_t>(i);
                    sceneMap->addUnsignedByte(key,
                                              value);
                }
                    break;
            }
            
        }
            break;
    }
    
    /*
     * Clear out for new elements
     */
    m_elementText = "";
    
    /*
     * Go to previous state
     */
    if (m_stateStack.empty()) {
        throw XmlSaxParserException("State stack is empty while reading Scene.");
    }
    CaretAssert(m_stateStack.empty() == false);
    m_state = m_stateStack.top();
    m_stateStack.pop();
}

/**
 * Add an object to its parent class.
 * @param sceneObject
 *    New child for parent class.
 */
void 
SceneSaxReader::addChildToParentClass(SceneObject* sceneObject)
{
    CaretAssert(m_objectBeingReadStack.empty() == false);
    SceneClass* parentClass = dynamic_cast<SceneClass*>(m_objectBeingReadStack.top());
    CaretAssert(parentClass);
    parentClass->addChild(sceneObject);
}


/**
 * get characters in an element.
 */
void 
SceneSaxReader::characters(const char* ch)
{
    m_elementText += ch;
}

/**
 * a fatal error occurs.
 */
void 
SceneSaxReader::fatalError(const XmlSaxParserException& e)
{
    throw e;
}

/**
 * A warning occurs
 */
void 
SceneSaxReader::warning(const XmlSaxParserException& e)
{    
    CaretLogWarning("XML Parser Warning: " + e.whatString());
}

// an error occurs
void 
SceneSaxReader::error(const XmlSaxParserException& e)
{   
    throw e;
}

void 
SceneSaxReader::startDocument() 
{    
}

void 
SceneSaxReader::endDocument()
{
}

