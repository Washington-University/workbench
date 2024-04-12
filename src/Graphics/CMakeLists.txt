#
# Name of Project
#
PROJECT (Graphics)

#
# Add QT for includes
#
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Gui_INCLUDE_DIRS})
endif()
if (Qt5_FOUND)
    include_directories(${Qt5Gui_INCLUDE_DIRS})
endif()

#
# Create a Library
#
ADD_LIBRARY(Graphics
CaretOpenGLInclude.h
EventGraphicsOpenGLCreateBufferObject.h
EventGraphicsOpenGLCreateTextureName.h
EventGraphicsOpenGLDeleteBufferObject.h
EventGraphicsOpenGLDeleteTextureName.h
EventOpenGLObjectToWindowTransform.h
GraphicsEngineData.h
GraphicsEngineDataOpenGL.h
GraphicsFramesPerSecond.h
GraphicsLineMeanDeviationSettings.h
GraphicsObjectToWindowTransform.h
GraphicsOpenGLBufferObject.h
GraphicsOpenGLError.h
GraphicsOpenGLPolylineTriangles.h
GraphicsOpenGLTextureName.h
GraphicsPolygonTessellator.h
GraphicsPrimitive.h
GraphicsPrimitiveSelectionHelper.h
GraphicsPrimitiveV3f.h
GraphicsPrimitiveV3fC4f.h
GraphicsPrimitiveV3fC4ub.h
GraphicsPrimitiveV3fN3f.h
GraphicsPrimitiveV3fN3fC4f.h
GraphicsPrimitiveV3fN3fC4ub.h
GraphicsPrimitiveV3fT2f.h
GraphicsPrimitiveV3fT3f.h
GraphicsRegionSelectionBox.h
GraphicsShape.h
GraphicsTextureMagnificationFilterEnum.h
GraphicsTextureMinificationFilterEnum.h
GraphicsTextureRectangle.h
GraphicsTextureSettings.h
GraphicsUtilitiesOpenGL.h
GraphicsViewport.h

EventGraphicsOpenGLCreateBufferObject.cxx
EventGraphicsOpenGLCreateTextureName.cxx
EventGraphicsOpenGLDeleteBufferObject.cxx
EventGraphicsOpenGLDeleteTextureName.cxx
EventOpenGLObjectToWindowTransform.cxx
GraphicsEngineData.cxx
GraphicsEngineDataOpenGL.cxx
GraphicsFramesPerSecond.cxx
GraphicsLineMeanDeviationSettings.cxx
GraphicsObjectToWindowTransform.cxx
GraphicsOpenGLBufferObject.cxx
GraphicsOpenGLError.cxx
GraphicsOpenGLPolylineTriangles.cxx
GraphicsOpenGLTextureName.cxx
GraphicsPolygonTessellator.cxx
GraphicsPrimitive.cxx
GraphicsPrimitiveSelectionHelper.cxx
GraphicsPrimitiveV3f.cxx
GraphicsPrimitiveV3fC4f.cxx
GraphicsPrimitiveV3fC4ub.cxx
GraphicsPrimitiveV3fN3f.cxx
GraphicsPrimitiveV3fN3fC4f.cxx
GraphicsPrimitiveV3fN3fC4ub.cxx
GraphicsPrimitiveV3fT2f.cxx
GraphicsPrimitiveV3fT3f.cxx
GraphicsRegionSelectionBox.cxx
GraphicsShape.cxx
GraphicsTextureMagnificationFilterEnum.cxx
GraphicsTextureMinificationFilterEnum.cxx
GraphicsTextureRectangle.cxx
GraphicsTextureSettings.cxx
GraphicsUtilitiesOpenGL.cxx
GraphicsViewport.cxx
)

#
# Find headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Graphics
${CMAKE_SOURCE_DIR}/Nifti
)

TARGET_LINK_LIBRARIES(Graphics ${CARET_QT5_LINK})

