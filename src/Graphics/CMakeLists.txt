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
EventGraphicsOpenGLCreateBufferObject.h
EventGraphicsOpenGLDeleteBufferObject.h
GraphicsEngine.h
GraphicsEngineData.h
GraphicsEngineDataOpenGL.h
GraphicsEngineOpenGL.h
GraphicsFactory.h
GraphicsOpenGLBufferObject.h
GraphicsPrimitive.h
GraphicsPrimitiveV3f.h
GraphicsPrimitiveV3fC4f.h
GraphicsPrimitiveV3fC4ub.h

EventGraphicsOpenGLCreateBufferObject.cxx
EventGraphicsOpenGLDeleteBufferObject.cxx
GraphicsEngine.cxx
GraphicsEngineData.cxx
GraphicsEngineDataOpenGL.cxx
GraphicsEngineOpenGL.cxx
GraphicsFactory.cxx
GraphicsOpenGLBufferObject.cxx
GraphicsPrimitive.cxx
GraphicsPrimitiveV3f.cxx
GraphicsPrimitiveV3fC4f.cxx
GraphicsPrimitiveV3fC4ub.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Graphics
)

TARGET_LINK_LIBRARIES(Graphics ${CARET_QT5_LINK})

