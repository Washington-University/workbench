#
# Name of Project
#
PROJECT (Scenes)

#
# Use XML from Qt but not GUI.
#
SET(QT_DONT_USE_QTGUI TRUE)

#
# QT include files
#
INCLUDE(${QT_USE_FILE})

#
# Create a Library
#
ADD_LIBRARY(Scenes
Scene.h
SceneAttributes.h
SceneBoolean.h
SceneBooleanArray.h
SceneClass.h
SceneClassAssistant.h
SceneEnumeratedTypeArray.h
SceneEnumeratedType.h
SceneException.h
SceneFloat.h
SceneFloatArray.h
SceneInteger.h
SceneIntegerArray.h
SceneObject.h
SceneObjectArray.h
SceneObjectDataTypeEnum.h
ScenePrimitive.h
ScenePrimitiveArray.h
SceneSaxReader.h
SceneString.h
SceneStringArray.h
SceneTypeEnum.h
SceneWriterInterface.h
SceneWriterXml.h
SceneXmlElements.h
SceneableInterface.h

Scene.cxx
SceneAttributes.cxx
SceneBoolean.cxx
SceneBooleanArray.cxx
SceneClass.cxx
SceneClassAssistant.cxx
SceneEnumeratedTypeArray.cxx
SceneEnumeratedType.cxx
SceneException.cxx
SceneFloat.cxx
SceneFloatArray.cxx
SceneInteger.cxx
SceneIntegerArray.cxx
SceneObject.cxx
SceneObjectArray.cxx
SceneObjectDataTypeEnum.cxx
ScenePrimitive.cxx
ScenePrimitiveArray.cxx
SceneSaxReader.cxx
SceneString.cxx
SceneStringArray.cxx
SceneTypeEnum.cxx
SceneWriterXml.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Xml
)

