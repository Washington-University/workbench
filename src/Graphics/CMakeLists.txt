#
# Name of Project
#
PROJECT (Graphics)

#
# Need OpenGL
#
FIND_PACKAGE(OpenGL REQUIRED)
IF (OPENGL_FOUND)
   #
   # Need help finding includes on Apple
   #
   IF (APPLE)
      # When searching for the include directory, find the location
      # for the OpenGL framework rather than an individual header file.
      FIND_PATH(OPENGL_INCLUDE_DIR OpenGL.framework
        /System/Library/Frameworks
        /Library/Frameworks
        ~/Library/Frameworks
      )
   ENDIF (APPLE)
   #
   # OpenGL Include Directory
   #
   INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
ELSE (OPENGL_FOUND)
   MESSAGE(SEND_ERROR "OpenGL Libraries were not found")
ENDIF (OPENGL_FOUND)

#
# Create a Library
#
ADD_LIBRARY(Graphics
CaretOpenGLInclude.h
CaretOpenGLWindowsFunctionsKludge.h
EventGraphicsOpenGLCreateBufferObject.h
EventGraphicsOpenGLCreateTextureName.h
EventGraphicsOpenGLDeleteBufferObject.h
EventGraphicsOpenGLDeleteTextureName.h
GraphicsEngineData.h
GraphicsEngineDataOpenGL.h
GraphicsOpenGLBufferObject.h
GraphicsOpenGLTextureName.h
GraphicsPrimitive.h
GraphicsPrimitiveSelectionHelper.h
GraphicsPrimitiveV3f.h
GraphicsPrimitiveV3fC4f.h
GraphicsPrimitiveV3fC4ub.h
GraphicsPrimitiveV3fT3f.h

EventGraphicsOpenGLCreateBufferObject.cxx
EventGraphicsOpenGLCreateTextureName.cxx
EventGraphicsOpenGLDeleteBufferObject.cxx
EventGraphicsOpenGLDeleteTextureName.cxx
GraphicsEngineData.cxx
GraphicsEngineDataOpenGL.cxx
GraphicsOpenGLBufferObject.cxx
GraphicsOpenGLTextureName.cxx
GraphicsPrimitive.cxx
GraphicsPrimitiveSelectionHelper.cxx
GraphicsPrimitiveV3f.cxx
GraphicsPrimitiveV3fC4f.cxx
GraphicsPrimitiveV3fC4ub.cxx
GraphicsPrimitiveV3fT3f.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Graphics
)

TARGET_LINK_LIBRARIES(Graphics ${CARET_QT5_LINK})

