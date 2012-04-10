#
# Name of project
#
PROJECT (Brain)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Use OpenGL from QT
#
#SET(QT_USE_QTOPENGL TRUE)

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
BrainOpenGL.h
BrainOpenGLFixedPipeline.h
BrainOpenGLTextRenderInterface.h
BrainOpenGLViewportContent.h
BrainStructure.h
BrainStructureNodeAttributes.h
BrowserTabContent.h
BrowserTabYoking.h
ConnectivityLoaderManager.h
DisplayProperties.h
DisplayPropertiesBorders.h
DisplayPropertiesInformation.h
DisplayPropertiesVolume.h
EventBrowserTabDelete.h
EventBrowserTabGet.h
EventBrowserTabGetAll.h
EventBrowserTabNew.h
EventCaretMappableDataFilesGet.h
EventDataFileRead.h
EventIdentificationHighlightLocation.h
EventIdentificationSymbolRemoval.h
EventNodeDataFilesGet.h
EventModelAdd.h
EventModelDelete.h
EventModelGetAll.h
EventModelSurfaceGet.h
EventModelYokingGroupGetAll.h
EventSpecFileReadDataFiles.h
EventSurfacesGet.h
IdentificationItemDataTypeEnum.h
IdentificationItem.h
IdentificationItemBorderSurface.h
IdentificationItemSurfaceNode.h
IdentificationItemSurfaceNodeIdentificationSymbol.h
IdentificationItemSurfaceTriangle.h
IdentificationItemVoxel.h
IdentificationManager.h
IdentificationStringBuilder.h
IdentificationTextGenerator.h
IdentificationWithColor.h
Model.h
ModelSurface.h
ModelSurfaceMontage.h
ModelSurfaceSelector.h
ModelTypeEnum.h
ModelVolume.h
ModelWholeBrain.h
ModelYokingGroup.h
NodeAndVoxelColoring.h
NodeIdentificationTypeEnum.h
Overlay.h
OverlaySet.h
SessionManager.h
SphereOpenGL.h
Surface.h
SurfaceSelectionModel.h
SurfaceNodeColoring.h
VolumeSliceCoordinateSelection.h
VolumeSliceViewModeEnum.h
VolumeSliceViewPlaneEnum.h
VolumeSurfaceOutlineColorOrTabModel.h
VolumeSurfaceOutlineSelection.h
YokingTypeEnum.h

Brain.cxx
BrainOpenGL.cxx
BrainOpenGLFixedPipeline.cxx
BrainOpenGLViewportContent.cxx
BrainStructure.cxx
BrainStructureNodeAttributes.cxx
BrowserTabContent.cxx
BrowserTabYoking.cxx
ConnectivityLoaderManager.cxx
DisplayProperties.cxx
DisplayPropertiesBorders.cxx
DisplayPropertiesInformation.cxx
DisplayPropertiesVolume.cxx
EventBrowserTabDelete.cxx
EventBrowserTabGet.cxx
EventBrowserTabGetAll.cxx
EventBrowserTabNew.cxx
EventDataFileRead.cxx
EventCaretMappableDataFilesGet.cxx
EventIdentificationHighlightLocation.cxx
EventIdentificationSymbolRemoval.cxx
EventNodeDataFilesGet.cxx
EventModelAdd.cxx
EventModelDelete.cxx
EventModelGetAll.cxx
EventModelSurfaceGet.cxx
EventModelYokingGroupGetAll.cxx
EventSpecFileReadDataFiles.cxx
EventSurfacesGet.cxx
IdentificationItemDataTypeEnum.cxx
IdentificationItem.cxx
IdentificationItemBorderSurface.cxx
IdentificationItemSurfaceNode.cxx
IdentificationItemSurfaceNodeIdentificationSymbol.cxx
IdentificationItemSurfaceTriangle.cxx
IdentificationItemVoxel.cxx
IdentificationManager.cxx
IdentificationStringBuilder.cxx
IdentificationTextGenerator.cxx
IdentificationWithColor.cxx
Model.cxx
ModelSurface.cxx
ModelSurfaceMontage.cxx
ModelSurfaceSelector.cxx
ModelTypeEnum.cxx
ModelVolume.cxx
ModelWholeBrain.cxx
ModelYokingGroup.cxx
NodeAndVoxelColoring.cxx
NodeIdentificationTypeEnum.cxx
Overlay.cxx
OverlaySet.cxx
SessionManager.cxx
SphereOpenGL.cxx
Surface.cxx
SurfaceSelectionModel.cxx
SurfaceNodeColoring.cxx
VolumeSliceCoordinateSelection.cxx
VolumeSliceViewModeEnum.cxx
VolumeSliceViewPlaneEnum.cxx
VolumeSurfaceOutlineColorOrTabModel.cxx
VolumeSurfaceOutlineSelection.cxx
YokingTypeEnum.cxx
) 

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
