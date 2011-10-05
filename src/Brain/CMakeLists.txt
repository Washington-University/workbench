#
# Name of project
#
PROJECT (Brain)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

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
   MESSAGE(FATAL_ERROR "OpenGL Libraries were not found")
ENDIF (OPENGL_FOUND)

#
# Create the brain library
#
ADD_LIBRARY(Brain
Brain.h
BrainConstants.h
BrainOpenGL.h
BrainStructure.h
BrowserTabContent.h
EventBrainStructureGet.h
EventBrowserTabDelete.h
EventBrowserTabGet.h
EventBrowserTabNew.h
EventDataFileRead.h
EventNodeDataFilesGet.h
EventModelDisplayControllerAdd.h
EventModelDisplayControllerDelete.h
EventModelDisplayControllerGetAll.h
EventSpecFileReadDataFiles.h
EventSurfaceColoringInvalidate.h
IdentificationItemDataTypeEnum.h
IdentificationItem.h
IdentificationItemSurfaceNode.h
IdentificationItemSurfaceTriangle.h
IdentificationManager.h
IdentificationWithColor.h
ModelDisplayController.h
ModelDisplayControllerSurface.h
ModelDisplayControllerSurfaceSelector.h
ModelDisplayControllerTypeEnum.h
ModelDisplayControllerVolume.h
ModelDisplayControllerWholeBrain.h
SessionManager.h
Surface.h
SurfaceNodeColoring.h
SurfaceOverlay.h
SurfaceOverlayDataTypeEnum.h
SurfaceOverlaySet.h
YokingTypeEnum.h

Brain.cxx
BrainConstants.cxx
BrainOpenGL.cxx
BrainStructure.cxx
BrowserTabContent.cxx
EventBrainStructureGet.cxx
EventBrowserTabDelete.cxx
EventBrowserTabGet.cxx
EventBrowserTabNew.cxx
EventDataFileRead.cxx
EventNodeDataFilesGet.cxx
EventModelDisplayControllerAdd.cxx
EventModelDisplayControllerDelete.cxx
EventModelDisplayControllerGetAll.cxx
EventSpecFileReadDataFiles.cxx
EventSurfaceColoringInvalidate.cxx
IdentificationItemDataTypeEnum.cxx
IdentificationItem.cxx
IdentificationItemSurfaceNode.cxx
IdentificationItemSurfaceTriangle.cxx
IdentificationManager.cxx
IdentificationWithColor.cxx
ModelDisplayController.cxx
ModelDisplayControllerSurface.cxx
ModelDisplayControllerSurfaceSelector.cxx
ModelDisplayControllerTypeEnum.cxx
ModelDisplayControllerVolume.cxx
ModelDisplayControllerWholeBrain.cxx
SessionManager.cxx
Surface.cxx
SurfaceNodeColoring.cxx
SurfaceOverlay.cxx
SurfaceOverlayDataTypeEnum.cxx
SurfaceOverlaySet.cxx
YokingTypeEnum.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
