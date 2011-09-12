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
EventBrowserTabDelete.h
EventBrowserTabNew.h
EventModelDisplayControllerAdd.h
EventModelDisplayControllerDelete.h
EventModelDisplayControllerGetAll.h
ModelDisplayController.h
ModelDisplayControllerSurface.h
ModelDisplayControllerVolume.h
ModelDisplayControllerWholeBrain.h
SessionManager.h
Surface.h
SurfaceOverlay.h
SurfaceOverlayDataTypeEnum.h
SurfaceOverlaySet.h
YokingTypeEnum.h

Brain.cxx
BrainConstants.cxx
BrainOpenGL.cxx
BrainStructure.cxx
BrowserTabContent.cxx
EventBrowserTabDelete.cxx
EventBrowserTabNew.cxx
EventModelDisplayControllerAdd.cxx
EventModelDisplayControllerDelete.cxx
EventModelDisplayControllerGetAll.cxx
ModelDisplayController.cxx
ModelDisplayControllerSurface.cxx
ModelDisplayControllerVolume.cxx
ModelDisplayControllerWholeBrain.cxx
SessionManager.cxx
Surface.cxx
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
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
