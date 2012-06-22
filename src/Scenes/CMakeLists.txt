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
SceneClass.h
SceneClassAssistant.h
SceneEnumeratedType.h
SceneException.h
SceneFloat.h
SceneInteger.h
SceneObject.h
SceneObjectDataTypeEnum.h
ScenePrimitive.h
SceneSaxReader.h
SceneString.h
SceneTypeEnum.h
SceneWriterInterface.h
SceneWriterXml.h
SceneXmlElements.h
SceneableInterface.h

Scene.cxx
SceneAttributes.cxx
SceneBoolean.cxx
SceneClass.cxx
SceneClassAssistant.cxx
SceneEnumeratedType.cxx
SceneException.cxx
SceneFloat.cxx
SceneInteger.cxx
SceneObject.cxx
SceneObjectDataTypeEnum.cxx
ScenePrimitive.cxx
SceneSaxReader.cxx
SceneString.cxx
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

