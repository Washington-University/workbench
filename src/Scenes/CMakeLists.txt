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
SceneBoolean.h
SceneClass.h
SceneDataTypeEnum.h
SceneEnumeratedType.h
SceneFloat.h
SceneInteger.h
SceneObject.h
ScenePrimitive.h
SceneString.h

SceneBoolean.cxx
SceneClass.cxx
SceneDataTypeEnum.cxx
SceneEnumeratedType.cxx
SceneFloat.cxx
SceneInteger.cxx
SceneObject.cxx
ScenePrimitive.cxx
SceneString.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)

