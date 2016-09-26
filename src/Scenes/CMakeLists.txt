#
# Name of Project
#
PROJECT (Scenes)

#
# SceneObject is derived from CaretObject.  When a scene is created many 
# instances of SceneObject may be created via its subclasses. 
# When compiled with debugging on, CaretObject maintains a call stack
# and new/delete is tracked for all CaretObjects.  As a result, the many 
# instances of SceneObject will cause reading of scenes to be slow and
# memory usage to become excessive.  So, do not derive SceneObject from
# CaretObject unless CARET_SCENE_DEBUG is defined.
#
#ADD_DEFINITIONS(-DCARET_SCENE_DEBUG)

#
# QT include files
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    #include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    SET(QT_DONT_USE_QTGUI TRUE)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

#
# Create a Library
#
ADD_LIBRARY(Scenes
BackgroundAndForegroundColorsSceneHelper.h
DisplayGroupAndTabItemHelper.h
Scene.h
SceneAttributes.h
SceneBoolean.h
SceneBooleanArray.h
SceneClass.h
SceneClassArray.h
SceneClassAssistant.h
SceneEnumeratedTypeArray.h
SceneEnumeratedType.h
SceneFloat.h
SceneFloatArray.h
SceneInfo.h
SceneInfoSaxReader.h
SceneInteger.h
SceneIntegerArray.h
SceneObject.h
SceneObjectArray.h
SceneObjectDataTypeEnum.h
SceneObjectMapIntegerKey.h
ScenePathName.h
ScenePathNameArray.h
ScenePrimitive.h
ScenePrimitiveArray.h
SceneSaxReader.h
SceneString.h
SceneStringArray.h
SceneTypeEnum.h
SceneUnsignedByte.h
SceneUnsignedByteArray.h
SceneWriterInterface.h
SceneWriterXml.h
SceneXmlElements.h
SceneableInterface.h

BackgroundAndForegroundColorsSceneHelper.cxx
DisplayGroupAndTabItemHelper.cxx
Scene.cxx
SceneAttributes.cxx
SceneBoolean.cxx
SceneBooleanArray.cxx
SceneClass.cxx
SceneClassArray.cxx
SceneClassAssistant.cxx
SceneEnumeratedTypeArray.cxx
SceneEnumeratedType.cxx
SceneFloat.cxx
SceneFloatArray.cxx
SceneInfo.cxx
SceneInfoSaxReader.cxx
SceneInteger.cxx
SceneIntegerArray.cxx
SceneObject.cxx
SceneObjectArray.cxx
SceneObjectDataTypeEnum.cxx
SceneObjectMapIntegerKey.cxx
ScenePathName.cxx
ScenePathNameArray.cxx
ScenePrimitive.cxx
ScenePrimitiveArray.cxx
SceneSaxReader.cxx
SceneString.cxx
SceneStringArray.cxx
SceneTypeEnum.cxx
SceneUnsignedByte.cxx
SceneUnsignedByteArray.cxx
SceneWriterXml.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Xml
)

