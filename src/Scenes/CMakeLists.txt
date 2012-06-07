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
SceneEnumeratedType.h
SceneFloat.h
SceneInteger.h
SceneObject.h
SceneObjectDataTypeEnum.h
ScenePrimitive.h
SceneString.h
SceneTypeEnum.h
SceneableInterface.h

Scene.cxx
SceneAttributes.cxx
SceneBoolean.cxx
SceneClass.cxx
SceneEnumeratedType.cxx
SceneFloat.cxx
SceneInteger.cxx
SceneObject.cxx
SceneObjectDataTypeEnum.cxx
ScenePrimitive.cxx
SceneString.cxx
SceneTypeEnum.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)

