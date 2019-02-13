
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __SCENE_XML_STREAM_READER_DECLARE__
#include "SceneXmlStreamReader.h"
#undef __SCENE_XML_STREAM_READER_DECLARE__

#include <QXmlStreamAttributes>
#include <QXmlStreamReader>

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
#include "WuQMacroGroup.h"
#include "WuQMacroGroupXmlStreamReader.h"

using namespace caret;


    
/**
 * \class caret::SceneXmlStreamReader 
 * \brief XML stream reader for Scene
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
SceneXmlStreamReader::SceneXmlStreamReader()
: SceneXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
SceneXmlStreamReader::~SceneXmlStreamReader()
{
}

/**
 * Read a scene from the given XML stream reader.  It assumes that
 * the start element for the scene has already been read and is
 * the current element.  If xmlReader.hasError() is set after this
 * method is called, there was an error reading the scene.
 *
 * @param xmlReader
 *    The XML stream reader
 * @param scene
 *    The scene
 * @param sceneFileName
 *    Name of the scene file
 */
void
SceneXmlStreamReader::readScene(QXmlStreamReader& xmlReader,
                                Scene* scene,
                                const AString& sceneFileName)
{
    CaretAssert(scene);
    if (scene == NULL) {
        return;
    }
    
    m_filename = sceneFileName;
    
    if (xmlReader.name() == ELEMENT_SCENE) {
        /*
         * Set when ending scene element is found
         */
        bool endElementFound(false);
        
        while ( ( ! xmlReader.atEnd())
               && ( ! endElementFound)) {
            xmlReader.readNext();
            
            switch (xmlReader.tokenType()) {
                case QXmlStreamReader::StartElement:
                {
                    if (xmlReader.name() == ELEMENT_SCENE_NAME) {
                        scene->setName(xmlReader.readElementText());
                    }
                    else if (xmlReader.name() == ELEMENT_SCENE_DESCRIPTION) {
                        scene->setDescription(xmlReader.readElementText());
                    }
                    else if (xmlReader.name() == WuQMacroGroupXmlStreamReader::ELEMENT_MACRO_GROUP) {
                        WuQMacroGroupXmlStreamReader macroGroupReader;
                        macroGroupReader.readMacroGroup(xmlReader,
                                                        scene->getMacroGroup());
                    }
                    else if (xmlReader.name() == ELEMENT_OBJECT) {
                        SceneObject* object = readSceneObject(xmlReader);
                        if (object != NULL) {
                            if (object->castToSceneClass()) {
                                scene->addClass(object->castToSceneClass());
                            }
                            else {
                                delete object;
                                xmlReader.raiseError("Child of Scene is not a SceneClass");
                            }
                        }
                    }
                    else {
                        m_unrecognizedElements.insert(xmlReader.name().toString());
                        xmlReader.skipCurrentElement();
                    }
                }
                    break;
                case QXmlStreamReader::EndElement:
                    if (xmlReader.name() == ELEMENT_SCENE) {
                        endElementFound = true;
                    }
                    break;
                default:
                    break;
            }
        }
        
        if ( ! xmlReader.hasError()) {
            /*
             * This will cause update of the macro's name
             * using the scene's name that is actually
             * inside of SceneInfo.
             */
            scene->setName(scene->getName());
        }
    }
    else {
        xmlReader.raiseError("Element should be \""
                             + ELEMENT_SCENE
                             + "\" but is \""
                             + xmlReader.name().toString()
                             + "\" while reading Scene");
    }
}

/**
 * Read any of the scene object sub-classes
 *
 * @param xmlReader
 *    The XML stream reader
 * @return
 *    Pointer to object read or NULL if not valid
 */
SceneObject*
SceneXmlStreamReader::readSceneObject(QXmlStreamReader& xmlReader)
{
    SceneObject* sceneObject(NULL);
    
    if (xmlReader.name() == ELEMENT_OBJECT) {
        sceneObject = readSceneObjectSingle(xmlReader);
    }
    else if (xmlReader.name() == ELEMENT_OBJECT_ARRAY) {
        sceneObject = readSceneObjectArray(xmlReader);
    }
    else if (xmlReader.name() == ELEMENT_OBJECT_MAP) {
        sceneObject = readSceneObjectMap(xmlReader);
    }
    else {
        xmlReader.raiseError("Unexpected element \""
                             + xmlReader.name().toString()
                             + "\" that is none of "
                             + ELEMENT_OBJECT
                             + ", "
                             + ELEMENT_OBJECT_ARRAY
                             + ", "
                             + ELEMENT_OBJECT_MAP);
    }
    return sceneObject;
}

/**
 * Read any of the 'single' scene object sub-classes.
 * 'Single' is one item (not an array nor a map)
 *
 * @param xmlReader
 *    The XML stream reader
 * @return
 *    Pointer to object read or NULL if not valid
 */
SceneObject*
SceneXmlStreamReader::readSceneObjectSingle(QXmlStreamReader& xmlReader)
{
    if (xmlReader.name() != ELEMENT_OBJECT) {
        xmlReader.raiseError("Current element should be "
                             + ELEMENT_OBJECT
                             + " at beginning of readSceneObjectSingle"
                             + " but is \""
                             + xmlReader.name().toString());
        return NULL;
    }
    
    const QXmlStreamAttributes attributes = xmlReader.attributes();
    const QString typeString    = attributes.value(ATTRIBUTE_OBJECT_TYPE).toString();
    const QString className     = attributes.value(ATTRIBUTE_OBJECT_CLASS).toString();
    const QString name          = attributes.value(ATTRIBUTE_OBJECT_NAME).toString();
    const QString versionString = attributes.value(ATTRIBUTE_OBJECT_VERSION).toString();
    
    SceneObjectDataTypeEnum::Enum dataType = SceneObjectDataTypeEnum::SCENE_INVALID;
    
    AString errorString;
    if (typeString.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_TYPE
                                      + " is missing on "
                                      + ELEMENT_OBJECT);
    }
    else {
        bool typeStringValid(false);
        dataType = SceneObjectDataTypeEnum::fromXmlName(typeString,
                                                        &typeStringValid);
        if ( ! typeStringValid) {
            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_TYPE
                                          + " \""
                                          + typeString
                                          + "\" is invalid on "
                                          + ELEMENT_OBJECT);
        }
    }
    
    if (name.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_NAME
                                      + " is missing on "
                                      + ELEMENT_OBJECT);
    }
    if (dataType == SceneObjectDataTypeEnum::SCENE_CLASS) {
        if (className.isEmpty()) {
            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_CLASS
                                          + " is missing on "
                                          + ELEMENT_OBJECT);
        }
        if (versionString.isEmpty()) {
            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_VERSION
                                          + " is missing on "
                                          + ELEMENT_OBJECT);
        }
    }
    if ( ! errorString.isEmpty()) {
        xmlReader.raiseError(errorString);
        return NULL;
    }
    
    SceneObject* sceneObject(NULL);
    
    SceneClass* sceneClass(NULL);
    switch (dataType) {
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            sceneObject = new SceneBoolean(name,
                                           AString(xmlReader.readElementText()).toBool());
            break;
        case SceneObjectDataTypeEnum::SCENE_CLASS:
        {
            sceneClass = new SceneClass(name,
                                        className,
                                        versionString.toInt());
            sceneObject = sceneClass;
        }
            break;
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            sceneObject = new SceneEnumeratedType(name,
                                                  xmlReader.readElementText());
            break;
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
            sceneObject = new SceneFloat(name,
                                         xmlReader.readElementText().toFloat());
            break;
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
            sceneObject = new SceneInteger(name,
                                           xmlReader.readElementText().toInt());
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            CaretAssert(0);
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
        {
            ScenePathName* pathName = new ScenePathName(name,
                                                        "");
            pathName->setValueToAbsolutePath(m_filename,
                                             xmlReader.readElementText());
            sceneObject = pathName;
        }
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            sceneObject = new SceneString(name,
                                          xmlReader.readElementText());
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
        {
            uint32_t value = xmlReader.readElementText().toUInt();
            if (value > std::numeric_limits<uint8_t>::max()) {
                value = std::numeric_limits<uint8_t>::max();
            }
            const uint8_t byteValue = static_cast<uint8_t>(value);
            sceneObject = new SceneUnsignedByte(name,
                                                byteValue);
        }
            break;
    }
    
    /*
     * Note: The 'primitive' values are read using QXmlStreamReader::readElementText()
     * which reads throught the end element.  Only a 'class' needs to continue until
     * the end element is found
     */
    if (sceneClass != NULL) {
        /*
         * Set when ending scene element is found
         */
        bool endElementFound(false);
        
        while ( ( ! xmlReader.atEnd())
               && ( ! endElementFound)) {
            xmlReader.readNext();
            
            switch (xmlReader.tokenType()) {
                case QXmlStreamReader::StartElement:
                    if (sceneClass != NULL) {
                        SceneObject* child = readSceneObject(xmlReader);
                        if (child != NULL) {
                            sceneClass->addChild(child);
                        }
                    }
                    else {
                        AString msg("Should not find a start element when reading a primitive type (not a class).."
                                    "Element name \""
                                    + xmlReader.name().toString()
                                    + "\" at line "
                                    + AString::number(xmlReader.lineNumber())
                                    + " column "
                                    + AString::number(xmlReader.columnNumber()));
                        CaretAssertMessage(0, msg);
                    }
                    break;
                case QXmlStreamReader::EndElement:
                    if (xmlReader.name() == ELEMENT_OBJECT) {
                        endElementFound = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return sceneObject;
}

/**
 * Read an array.
 *
 * @param xmlReader
 *    The XML stream reader
 * @return
 *    Pointer to array read or NULL if not valid
 */
SceneObjectArray*
SceneXmlStreamReader::readSceneObjectArray(QXmlStreamReader& xmlReader)
{
    if (xmlReader.name() != ELEMENT_OBJECT_ARRAY) {
        xmlReader.raiseError("Current element should be "
                             + ELEMENT_OBJECT_ARRAY
                             + " at beginning of readSceneObjectArray"
                             + " but is \""
                             + xmlReader.name().toString());
        return NULL;
    }
    
    const QXmlStreamAttributes arrayAttributes = xmlReader.attributes();
    const QString typeString    = arrayAttributes.value(ATTRIBUTE_OBJECT_ARRAY_TYPE).toString();
    const QString name          = arrayAttributes.value(ATTRIBUTE_OBJECT_ARRAY_NAME).toString();
    const QString lengthString = arrayAttributes.value(ATTRIBUTE_OBJECT_ARRAY_LENGTH).toString();
    
    SceneObjectDataTypeEnum::Enum dataType = SceneObjectDataTypeEnum::SCENE_INVALID;
    
    AString errorString;
    if (typeString.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_TYPE
                                      + " is missing on "
                                      + ELEMENT_OBJECT_ARRAY);
    }
    else {
        bool typeStringValid(false);
        dataType = SceneObjectDataTypeEnum::fromXmlName(typeString,
                                                     &typeStringValid);
        if ( ! typeStringValid) {
            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_TYPE
                                          + " \""
                                          + typeString
                                          + "\" is invalid on "
                                          + ELEMENT_OBJECT_ARRAY);
        }
    }
    if (name.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_NAME
                                      + " is missing on "
                                      + ELEMENT_OBJECT_ARRAY);
    }
    
    int32_t arrayLength(0);
    if (lengthString.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_LENGTH
                                      + " is missing on "
                                      + ELEMENT_OBJECT_ARRAY);
    }
    else {
        arrayLength = lengthString.toInt();
        if (arrayLength < 0) {
            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_LENGTH
                                          + "="
                                          + lengthString
                                          + " is invalid on "
                                          + ELEMENT_OBJECT_ARRAY);
        }
//        else if (arrayLength == 0) {
//            return NULL;
//        }
    }
    
    if ( ! errorString.isEmpty()) {
        xmlReader.raiseError(errorString);
        return NULL;
    }
    
    SceneObjectArray* sceneArray(NULL);
    SceneClassArray* classArray(NULL);
    SceneEnumeratedTypeArray* enumeratedTypeArray(NULL);
    ScenePathNameArray* pathNameArray(NULL);
    SceneBooleanArray* booleanArray(NULL);
    SceneFloatArray* floatArray(NULL);
    SceneIntegerArray* integerArray(NULL);
    SceneStringArray* stringArray(NULL);
    SceneUnsignedByteArray* unsignedByteArray(NULL);
    switch (dataType) {
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            booleanArray = new SceneBooleanArray(name,
                                                 arrayLength);
            sceneArray = booleanArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_CLASS:
            classArray = new SceneClassArray(name,
                                             arrayLength);
            sceneArray = classArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            enumeratedTypeArray = new SceneEnumeratedTypeArray(name,
                                                               arrayLength);
            sceneArray = enumeratedTypeArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
            floatArray = new SceneFloatArray(name,
                                             arrayLength);
            sceneArray = floatArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
            integerArray = new SceneIntegerArray(name,
                                                 arrayLength);
            sceneArray = integerArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            CaretAssert(0);
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            pathNameArray = new ScenePathNameArray(name,
                                                   arrayLength);
            sceneArray = pathNameArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            stringArray = new SceneStringArray(name,
                                               arrayLength);
            sceneArray = stringArray;
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            unsignedByteArray = new SceneUnsignedByteArray(name,
                                                           arrayLength);
            sceneArray = unsignedByteArray;
            break;
    }
    CaretAssert(sceneArray);

    /*
     * Set when ending scene element is found
     */
    bool endElementFound(false);
    
    int32_t sceneArrayElementIndex = -1;
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == ELEMENT_OBJECT_ARRAY_ELEMENT) {
                    const QXmlStreamAttributes elementAttributes = xmlReader.attributes();
                    
                    int32_t elementIndex(-1);
                    const QString elementIndexString = elementAttributes.value(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX).toString();
                    if (elementIndexString.isEmpty()) {
                        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX
                                                      + " is missing on "
                                                      + ELEMENT_OBJECT_ARRAY_ELEMENT);
                    }
                    else {
                        elementIndex = elementIndexString.toInt();
                        if (elementIndex < 0) {
                            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX
                                                          + "="
                                                          + elementIndexString
                                                          + " is invalid on "
                                                          + ELEMENT_OBJECT_ARRAY_ELEMENT);
                        }
                    }
                    if ( ! errorString.isEmpty()) {
                        xmlReader.raiseError(errorString);
                        if (sceneArray != NULL) {
                            delete sceneArray;
                        }
                        return NULL;
                    }

                    switch (dataType) {
                        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                            CaretAssert(booleanArray);
                            booleanArray->setValue(elementIndex,
                                                   AString(xmlReader.readElementText()).toBool());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_CLASS:
                            /*
                             * Child class is handled when start element is found
                             */
                            sceneArrayElementIndex = elementIndex;
                            break;
                        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                            CaretAssert(enumeratedTypeArray);
                            enumeratedTypeArray->setValue(elementIndex,
                                                                  xmlReader.readElementText());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_FLOAT:
                            CaretAssert(floatArray);
                            floatArray->setValue(elementIndex,
                                                         xmlReader.readElementText().toFloat());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_INTEGER:
                            CaretAssert(integerArray);
                            integerArray->setValue(elementIndex,
                                                           xmlReader.readElementText().toInt());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_INVALID:
                            CaretAssert(0);
                            break;
                        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                            CaretAssert(pathNameArray);
                            pathNameArray->setScenePathNameAtIndex(elementIndex,
                                                                   m_filename,
                                                                   xmlReader.readElementText());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_STRING:
                            CaretAssert(stringArray);
                            stringArray->setValue(elementIndex,
                                                  xmlReader.readElementText());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                        {
                            CaretAssert(unsignedByteArray);
                            uint32_t value = xmlReader.readElementText().toUInt();
                            if (value > std::numeric_limits<uint8_t>::max()) {
                                value = std::numeric_limits<uint8_t>::max();
                            }
                            const uint8_t byteValue = static_cast<uint8_t>(value);
                            unsignedByteArray->setValue(elementIndex,
                                                        byteValue);
                        }
                            break;
                    }
                }
                else if (sceneArrayElementIndex >= 0) {
                    /*
                     * Must be child of a scene class
                     */
                    CaretAssert(classArray);
                    SceneObject* elementObject = readSceneObject(xmlReader);
                    if (elementObject != NULL) {
                        SceneClass* elementClass = elementObject->castToSceneClass();
                        CaretAssert(elementClass);
                        classArray->setClassAtIndex(sceneArrayElementIndex, elementClass);
                    }
                }
                else {
                    
                }
                
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_OBJECT_ARRAY) {
                    endElementFound = true;
                }
                else if (xmlReader.name() == ELEMENT_OBJECT_ARRAY_ELEMENT) {
                    sceneArrayElementIndex = -1;
                }
                break;
            default:
                break;
        }
    }

    return sceneArray;
}

/**
 * Read a map.
 *
 * @param xmlReader
 *    The XML stream reader
 * @return
 *    Pointer to map read or NULL if not valid
 */
SceneObjectMapIntegerKey*
SceneXmlStreamReader::readSceneObjectMap(QXmlStreamReader& xmlReader)
{
    if (xmlReader.name() != ELEMENT_OBJECT_MAP) {
        xmlReader.raiseError("Current element should be "
                             + ELEMENT_OBJECT_MAP
                             + " at beginning of readSceneObjectMap"
                             + " but is \""
                             + xmlReader.name().toString());
        return NULL;
    }
    
    const QXmlStreamAttributes mapAttributes = xmlReader.attributes();
    const QString typeString    = mapAttributes.value(ATTRIBUTE_OBJECT_MAP_TYPE).toString();
    const QString name          = mapAttributes.value(ATTRIBUTE_OBJECT_MAP_NAME).toString();
    
    SceneObjectDataTypeEnum::Enum dataType = SceneObjectDataTypeEnum::SCENE_INVALID;
    
    AString errorString;
    if (typeString.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_MAP_TYPE
                                      + " is missing on "
                                      + ELEMENT_OBJECT_MAP);
    }
    else {
        bool typeStringValid(false);
        dataType = SceneObjectDataTypeEnum::fromXmlName(typeString,
                                                        &typeStringValid);
        if ( ! typeStringValid) {
            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_MAP_TYPE
                                          + " \""
                                          + typeString
                                          + "\" is invalid on "
                                          + ELEMENT_OBJECT_MAP);
        }
    }
    if (name.isEmpty()) {
        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_MAP_NAME
                                      + " is missing on "
                                      + ELEMENT_OBJECT_MAP);
    }
    
    if ( ! errorString.isEmpty()) {
        xmlReader.raiseError(errorString);
        return NULL;
    }
    
    SceneObjectMapIntegerKey* sceneMap = new SceneObjectMapIntegerKey(name,
                                                                      dataType);
    
    /*
     * Set when ending scene element is found
     */
    bool endElementFound(false);
    
    int32_t sceneKeyIndex = -1;
    while ( ( ! xmlReader.atEnd())
           && ( ! endElementFound)) {
        xmlReader.readNext();
        
        switch (xmlReader.tokenType()) {
            case QXmlStreamReader::StartElement:
                if (xmlReader.name() == ELEMENT_OBJECT_MAP_VALUE) {
                    const QXmlStreamAttributes valueAttributes = xmlReader.attributes();
                    int32_t keyIndex(-1);
                    const QString keyString = valueAttributes.value(ATTRIBUTE_OBJECT_MAP_VALUE_KEY).toString();
                    if (keyString.isEmpty()) {
                        errorString.appendWithNewLine(ATTRIBUTE_OBJECT_MAP_VALUE_KEY
                                                      + " is missing on "
                                                      + ELEMENT_OBJECT_MAP_VALUE);
                    }
                    else {
                        keyIndex = keyString.toInt();
                        if (keyIndex < 0) {
                            errorString.appendWithNewLine(ATTRIBUTE_OBJECT_MAP_VALUE_KEY
                                                          + "="
                                                          + keyIndex
                                                          + " is invalid on "
                                                          + ELEMENT_OBJECT_MAP_VALUE);
                        }
                    }
                    if ( ! errorString.isEmpty()) {
                        xmlReader.raiseError();
                        if (sceneMap != NULL) {
                            delete sceneMap;
                        }
                        return NULL;
                    }
                    
                    switch (dataType) {
                        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
                            sceneMap->addBoolean(keyIndex,
                                                   AString(xmlReader.readElementText()).toBool());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_CLASS:
                            /*
                             * Child class is handled when start element is found
                             */
                            sceneKeyIndex = keyIndex;
                            break;
                        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
                            sceneMap->addEnumeratedType(keyIndex,
                                                          xmlReader.readElementText());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_FLOAT:
                            sceneMap->addFloat(keyIndex,
                                                 xmlReader.readElementText().toFloat());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_INTEGER:
                            sceneMap->addInteger(keyIndex,
                                                 xmlReader.readElementText().toInt());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_INVALID:
                            CaretAssert(0);
                            break;
                        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
                        {
                            ScenePathName spn("spn", "");
                            spn.setValueToAbsolutePath(m_filename,
                                                       xmlReader.readElementText());
                            sceneMap->addPathName(keyIndex,
                                                  spn.toString());
                        }
                            break;
                        case SceneObjectDataTypeEnum::SCENE_STRING:
                            sceneMap->addString(keyIndex,
                                                xmlReader.readElementText());
                            break;
                        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
                        {
                            uint32_t value = xmlReader.readElementText().toUInt();
                            if (value > std::numeric_limits<uint8_t>::max()) {
                                value = std::numeric_limits<uint8_t>::max();
                            }
                            const uint8_t byteValue = static_cast<uint8_t>(value);
                            sceneMap->addUnsignedByte(keyIndex,
                                                      byteValue);
                        }
                            break;
                    }
                }
                else if (sceneKeyIndex >= 0) {
                    /*
                     * Must be child of a scene class
                     */
                    SceneObject* elementObject = readSceneObject(xmlReader);
                    if (elementObject != NULL) {
                        SceneClass* elementClass = elementObject->castToSceneClass();
                        CaretAssert(elementClass);
                        sceneMap->addClass(sceneKeyIndex,
                                           elementClass);
                    }
                }
                else {
                    
                }
                
                break;
            case QXmlStreamReader::EndElement:
                if (xmlReader.name() == ELEMENT_OBJECT_MAP) {
                    endElementFound = true;
                }
                else if (xmlReader.name() == ELEMENT_OBJECT_MAP_VALUE) {
                    sceneKeyIndex = -1;
                }
                break;
            default:
                break;
        }
    }
    
    return sceneMap;
}
