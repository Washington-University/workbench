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
SET(CARET_QT_LINK_MODULES "")
if(Qt6_FOUND)
    include_directories(${Qt6Core5Compat_INCLUDE_DIRS})
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Gui_INCLUDE_DIRS})
    include_directories(${Qt6Xml_INCLUDE_DIRS})
# do we need this?    SET(CARET_QT_LINK_MODULES "Qt6::Core")
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})
    #include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
    SET(CARET_QT_LINK_MODULES "Qt5::Core")
endif()

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
SceneFileBasePathTypeEnum.h
SceneFloat.h
SceneFloatArray.h
SceneInfo.h
SceneInfoXmlStreamBase.h
SceneInfoXmlStreamReader.h
SceneInfoXmlStreamWriter.h
SceneInteger.h
SceneIntegerArray.h
SceneLongInteger.h
SceneLongIntegerArray.h
SceneObject.h
SceneObjectArray.h
SceneObjectContainerTypeEnum.h
SceneObjectDataTypeEnum.h
SceneObjectMapIntegerKey.h
ScenePathName.h
ScenePathNameArray.h
ScenePrimitive.h
ScenePrimitiveArray.h
SceneRestoreWarningCodesEnum.h
SceneString.h
SceneStringArray.h
SceneTypeEnum.h
SceneUnsignedByte.h
SceneUnsignedByteArray.h
SceneWriterInterface.h
SceneXmlStreamBase.h
SceneXmlStreamReader.h
SceneXmlStreamWriter.h
SceneableInterface.h
TileTabsBrowserTabGeometrySceneHelper.h

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
SceneFileBasePathTypeEnum.cxx
SceneFloat.cxx
SceneFloatArray.cxx
SceneInfo.cxx
SceneInfoXmlStreamBase.cxx
SceneInfoXmlStreamReader.cxx
SceneInfoXmlStreamWriter.cxx
SceneInteger.cxx
SceneIntegerArray.cxx
SceneLongInteger.cxx
SceneLongIntegerArray.cxx
SceneObject.cxx
SceneObjectArray.cxx
SceneObjectContainerTypeEnum.cxx
SceneObjectDataTypeEnum.cxx
SceneObjectMapIntegerKey.cxx
ScenePathName.cxx
ScenePathNameArray.cxx
ScenePrimitive.cxx
ScenePrimitiveArray.cxx
SceneRestoreWarningCodesEnum.cxx
SceneString.cxx
SceneStringArray.cxx
SceneTypeEnum.cxx
SceneUnsignedByte.cxx
SceneUnsignedByteArray.cxx
SceneXmlStreamBase.cxx
SceneXmlStreamReader.cxx
SceneXmlStreamWriter.cxx
TileTabsBrowserTabGeometrySceneHelper.cxx
)

TARGET_LINK_LIBRARIES(Scenes ${CARET_QT5_LINK})

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Xml
)

