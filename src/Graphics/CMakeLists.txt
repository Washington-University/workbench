#
# Name of Project
#
PROJECT (Graphics)

#
# Create a Library
#
ADD_LIBRARY(Graphics
CaretOpenGLInclude.h
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

