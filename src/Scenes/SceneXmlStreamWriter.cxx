
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

#define __SCENE_XML_STREAM_WRITER_DECLARE__
#include "SceneXmlStreamWriter.h"
#undef __SCENE_XML_STREAM_WRITER_DECLARE__

#include <QXmlStreamWriter>

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

using namespace caret;


    
/**
 * \class caret::SceneXmlStreamWriter 
 * \brief XML stream writer for Scene
 * \ingroup Scenes
 */

/**
 * Constructor.
 */
SceneXmlStreamWriter::SceneXmlStreamWriter()
: SceneXmlStreamBase()
{
    
}

/**
 * Destructor.
 */
SceneXmlStreamWriter::~SceneXmlStreamWriter()
{
}

/**
 * Write the given scene using the given xml stream writer
 *
 * @param xmlWriter
 *     The XML writer
 * @param scene
 *     The scene
 * @param sceneIndex
 *     Index of the scene
 * @param sceneFileName
 *     Name of the scene file
 */
void
SceneXmlStreamWriter::writeXML(QXmlStreamWriter* xmlWriter,
                               const Scene* scene,
                               const int32_t sceneIndex,
                               const AString& sceneFileName)
{
    CaretAssert(xmlWriter);
    CaretAssert(scene);
    CaretAssert(sceneIndex >= 0);
    CaretAssert( ! sceneFileName.isEmpty());
    
    m_xmlWriter     = xmlWriter;
    m_sceneFileName = sceneFileName;
    
    const SceneAttributes* sceneAttributes = scene->getAttributes();
    const AString sceneTypeName = SceneTypeEnum::toName(sceneAttributes->getSceneType());
    
    m_xmlWriter->writeStartElement(ELEMENT_SCENE);
    m_xmlWriter->writeAttribute(ATTRIBUTE_SCENE_INDEX,
                                QString::number(sceneIndex));
    m_xmlWriter->writeAttribute(ATTRIBUTE_SCENE_TYPE,
                                sceneTypeName);

    m_xmlWriter->writeTextElement(ELEMENT_SCENE_NAME,
                                  scene->getName());
    m_xmlWriter->writeTextElement(ELEMENT_SCENE_DESCRIPTION,
                                  scene->getDescription());
    
    const int32_t numClasses = scene->getNumberOfClasses();
    for (int32_t i = 0; i < numClasses; i++) {
        writeSceneClass(scene->getClassAtIndex(i));
    }
    
    m_xmlWriter->writeEndElement();

    m_xmlWriter = NULL;
}

/**
 * Write the given scene class using the xml stream writer
 *
 * @param sceneClass
 *     Scene class to write
 */
void
SceneXmlStreamWriter::writeSceneClass(const SceneClass* sceneClass)
{
    CaretAssert(sceneClass);
    if (sceneClass == NULL) {
        return;
    }

    const AString& objectTypeName = SceneObjectDataTypeEnum::toXmlName(sceneClass->getDataType());

    m_xmlWriter->writeStartElement(ELEMENT_OBJECT);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_TYPE,
                                objectTypeName);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_CLASS,
                                sceneClass->getClassName());
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_NAME,
                                sceneClass->getName());
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_VERSION,
                                AString::number(sceneClass->getVersionNumber()));
    
    const int32_t numberOfObjects = sceneClass->getNumberOfObjects();
    for (int32_t i = 0; i < numberOfObjects; i++) {
        writeSceneObject(sceneClass->getObjectAtIndex(i));
    }
    m_xmlWriter->writeEndElement();
}

/**
 * Write the given scene object using the xml stream writer
 *
 * @param sceneObject
 *     Scene object to write
 */
void
SceneXmlStreamWriter::writeSceneObject(const SceneObject* sceneObject)
{
    CaretAssert(sceneObject);

    switch (sceneObject->getContainerType()) {
        case SceneObjectContainerTypeEnum::ARRAY:
            writeArrayObject(sceneObject->castToSceneObjectArray());
            break;
        case SceneObjectContainerTypeEnum::MAP:
            writeMapObject(sceneObject->castToSceneObjectMapIntegerKey());
            break;
        case SceneObjectContainerTypeEnum::SINGLE:
            writeSingleObject(sceneObject);
            break;
    }
    
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
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            break;
    }
}

/**
 * Write an object array
 *
 * @param objectArray
 *    The object array
 */
void
SceneXmlStreamWriter::writeArrayObject(const SceneObjectArray* objectArray)
{
    CaretAssert(objectArray);
    if (objectArray == NULL) {
        return;
    }
    
    switch (objectArray->getDataType()) {
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            writeArrayPrimitiveType(objectArray->castToScenePrimitiveArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_CLASS:
            writeArrayClass(objectArray->castToSceneClassArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            writeArrayEnumeratedType(objectArray->castToSceneEnumeratedTypeArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
            writeArrayPrimitiveType(objectArray->castToScenePrimitiveArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
            writeArrayPrimitiveType(objectArray->castToScenePrimitiveArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            CaretAssert(0);
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            writeArrayPathName(objectArray->castToScenePathNameArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            writeArrayPrimitiveType(objectArray->castToScenePrimitiveArray());
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            writeArrayPrimitiveType(objectArray->castToScenePrimitiveArray());
            break;
    }
}

/**
 * Write an object map
 *
 * @param objectMap
 *     The object map
 */
void
SceneXmlStreamWriter::writeMapObject(const SceneObjectMapIntegerKey* objectMap)
{
    CaretAssert(objectMap);
    if (objectMap == NULL) {
        return;
    }
    
    const SceneObjectDataTypeEnum::Enum dataType = objectMap->getDataType();
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT_MAP);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_MAP_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(dataType));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_MAP_NAME,
                                objectMap->getName());

    const std::map<int32_t, SceneObject*>& sceneMap = objectMap->getMap();
    for (const auto iter : sceneMap) {
        const int32_t key = iter.first;
        const QString keyString(QString::number(key));
        
        m_xmlWriter->writeStartElement(ELEMENT_OBJECT_MAP_VALUE);
        m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_MAP_VALUE_KEY,
                                    keyString);
        
        const SceneObject* valueObject = iter.second;
        switch (dataType) {
            case SceneObjectDataTypeEnum::SCENE_INVALID:
                CaretAssert(0);
                break;
            case SceneObjectDataTypeEnum::SCENE_CLASS:
            {
                const SceneClass* value = valueObject->castToSceneClass();
                CaretAssert(value);
                writeSceneClass(value);
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            {
                const SceneEnumeratedType* value = valueObject->castToSceneEnumeratedType();
                CaretAssert(value);
                m_xmlWriter->writeCharacters(value->stringValue());
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            {
                const ScenePrimitive* value = valueObject->castToScenePrimitive();
                CaretAssert(value);
                m_xmlWriter->writeCharacters(value->stringValue());
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_FLOAT:
            {
                const ScenePrimitive* value = valueObject->castToScenePrimitive();
                CaretAssert(value);
                m_xmlWriter->writeCharacters(value->stringValue());
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_INTEGER:
            {
                const ScenePrimitive* value = valueObject->castToScenePrimitive();
                CaretAssert(value);
                m_xmlWriter->writeCharacters(value->stringValue());
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            {
                const ScenePathName* pathName = valueObject->castToScenePathName();
                CaretAssert(pathName);
                const AString path = pathName->getRelativePathToSceneFile(m_sceneFileName);
                m_xmlWriter->writeCharacters(path);
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_STRING:
            {
                const ScenePrimitive* value = valueObject->castToScenePrimitive();
                CaretAssert(value);
                m_xmlWriter->writeCharacters(value->stringValue());
            }
                break;
            case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            {
                const ScenePrimitive* value = valueObject->castToScenePrimitive();
                CaretAssert(value);
                m_xmlWriter->writeCharacters(value->stringValue());
            }
                break;
        }
        
        m_xmlWriter->writeEndElement();
    }


    m_xmlWriter->writeEndElement();
}

/**
 * Write a single object (not an array nor map)
 *
 * @param sceneObject
 *     The single object
 */
void
SceneXmlStreamWriter::writeSingleObject(const SceneObject* sceneObject)
{
    CaretAssert(sceneObject);
    if (sceneObject == NULL) {
        return;
    }
    
    switch (sceneObject->getDataType()) {
        case SceneObjectDataTypeEnum::SCENE_BOOLEAN:
            writePrimitive(sceneObject->castToScenePrimitive());
            break;
        case SceneObjectDataTypeEnum::SCENE_CLASS:
            writeSceneClass(sceneObject->castToSceneClass());
            break;
        case SceneObjectDataTypeEnum::SCENE_ENUMERATED_TYPE:
            writeEnumeratedType(sceneObject->castToSceneEnumeratedType());
            break;
        case SceneObjectDataTypeEnum::SCENE_FLOAT:
            writePrimitive(sceneObject->castToScenePrimitive());
            break;
        case SceneObjectDataTypeEnum::SCENE_INTEGER:
            writePrimitive(sceneObject->castToScenePrimitive());
            break;
        case SceneObjectDataTypeEnum::SCENE_INVALID:
            CaretAssert(0);
            break;
        case SceneObjectDataTypeEnum::SCENE_PATH_NAME:
            writePathName(sceneObject->castToScenePathName());
            break;
        case SceneObjectDataTypeEnum::SCENE_STRING:
            writePrimitive(sceneObject->castToScenePrimitive());
            break;
        case SceneObjectDataTypeEnum::SCENE_UNSIGNED_BYTE:
            writePrimitive(sceneObject->castToScenePrimitive());
            break;
    }
}

/**
 * Write an enumerated type
 *
 * @param sceneEnumeratedType
 *     The object map
 */
void
SceneXmlStreamWriter::writeEnumeratedType(const SceneEnumeratedType* sceneEnumeratedType)
{
    CaretAssert(sceneEnumeratedType);
    if (sceneEnumeratedType == NULL) {
        return;
    }
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(sceneEnumeratedType->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_NAME,
                                sceneEnumeratedType->getName());
    m_xmlWriter->writeCharacters(sceneEnumeratedType->stringValue());
    m_xmlWriter->writeEndElement();
}

/**
 * Write a path name
 *
 * @param scenePathName
 *     The path name
 */
void
SceneXmlStreamWriter::writePathName(const ScenePathName* scenePathName)
{
    CaretAssert(scenePathName);
    if (scenePathName == NULL) {
        return;
    }
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(scenePathName->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_NAME,
                                scenePathName->getName());
    m_xmlWriter->writeCharacters(scenePathName->getRelativePathToSceneFile(m_sceneFileName));
    m_xmlWriter->writeEndElement();
}

/**
 * Write a primitive
 *
 * @param scenePrimitive
 *     The primitive
 */
void
SceneXmlStreamWriter::writePrimitive(const ScenePrimitive* scenePrimitive)
{
    CaretAssert(scenePrimitive);
    if (scenePrimitive == NULL) {
        return;
    }
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(scenePrimitive->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_NAME,
                                scenePrimitive->getName());
    m_xmlWriter->writeCharacters(scenePrimitive->stringValue());
    m_xmlWriter->writeEndElement();
}

/**
 * Write an enumerated array
 *
 * @param enumeratedArray
 *     The enumerated array
 */
void
SceneXmlStreamWriter::writeArrayEnumeratedType(const SceneEnumeratedTypeArray* enumeratedArray)
{
    CaretAssert(enumeratedArray);
    if (enumeratedArray == NULL) {
        return;
    }
    
    const int32_t numberOfElements = enumeratedArray->getNumberOfArrayElements();
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(enumeratedArray->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_NAME,
                                enumeratedArray->getName());
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_LENGTH,
                                QString::number(numberOfElements));
    
    for (int32_t i = 0; i < numberOfElements; i++) {
        m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY_ELEMENT);
        m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX,
                                    QString::number(i));
        m_xmlWriter->writeCharacters(enumeratedArray->stringValue(i));
        m_xmlWriter->writeEndElement();
    }
    m_xmlWriter->writeEndElement();
}

/**
 * Write a primitive array
 *
 * @param primitiveArray
 *     The primitive array
 */
void
SceneXmlStreamWriter::writeArrayPrimitiveType(const ScenePrimitiveArray* primitiveArray)
{
    CaretAssert(primitiveArray);
    if (primitiveArray == NULL) {
        return;
    }

    const int32_t numberOfElements = primitiveArray->getNumberOfArrayElements();
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(primitiveArray->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_NAME,
                                primitiveArray->getName());
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_LENGTH,
                                QString::number(numberOfElements));

    for (int32_t i = 0; i < numberOfElements; i++) {
        m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY_ELEMENT);
        m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX,
                                    QString::number(i));
        m_xmlWriter->writeCharacters(primitiveArray->stringValue(i));
        m_xmlWriter->writeEndElement();
    }
    m_xmlWriter->writeEndElement();
}

/**
 * Write a class array
 *
 * @param classArray
 *     The class array
 */
void
SceneXmlStreamWriter::writeArrayClass(const SceneClassArray* classArray)
{
    CaretAssert(classArray);
    if (classArray == NULL) {
        return;
    }
    
    const int32_t numberOfElements = classArray->getNumberOfArrayElements();
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(classArray->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_NAME,
                                classArray->getName());
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_LENGTH,
                                QString::number(numberOfElements));
    
    for (int32_t i = 0; i < numberOfElements; i++) {
        m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY_ELEMENT);
        m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX,
                                    QString::number(i));
        writeSceneClass(classArray->getClassAtIndex(i));
        m_xmlWriter->writeEndElement();
    }
    m_xmlWriter->writeEndElement();
}

/**
 * Write a path name array
 *
 * @param pathNameArray
 *     The path name array
 */
void
SceneXmlStreamWriter::writeArrayPathName(const ScenePathNameArray* pathNameArray)
{
    CaretAssert(pathNameArray);
    if (pathNameArray == NULL) {
        return;
    }
    
    const int32_t numberOfElements = pathNameArray->getNumberOfArrayElements();
    
    m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY);
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_TYPE,
                                SceneObjectDataTypeEnum::toXmlName(pathNameArray->getDataType()));
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_NAME,
                                pathNameArray->getName());
    m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_LENGTH,
                                QString::number(numberOfElements));
    
    for (int32_t i = 0; i < numberOfElements; i++) {
        m_xmlWriter->writeStartElement(ELEMENT_OBJECT_ARRAY_ELEMENT);
        m_xmlWriter->writeAttribute(ATTRIBUTE_OBJECT_ARRAY_ELEMENT_INDEX,
                                    QString::number(i));
        const ScenePathName* scenePathName = pathNameArray->getScenePathNameAtIndex(i);
        const QString path = scenePathName->getRelativePathToSceneFile(m_sceneFileName);
        m_xmlWriter->writeCharacters(path);
        m_xmlWriter->writeEndElement();
    }
    m_xmlWriter->writeEndElement();
}
