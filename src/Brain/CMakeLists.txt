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
BorderDrawingTypeEnum.h
Brain.h
BrainOpenGL.h
BrainOpenGLFixedPipeline.h
BrainOpenGLShape.h
BrainOpenGLShapeCone.h
BrainOpenGLShapeCube.h
BrainOpenGLShapeRing.h
BrainOpenGLShapeSphere.h
BrainOpenGLTextRenderInterface.h
BrainOpenGLViewportContent.h
BrainordinateDataSelection.h
BrainStructure.h
BrainStructureNodeAttributes.h
BrowserTabContent.h
ChartingDataManager.h
CiftiConnectivityMatrixDataFileManager.h
CiftiFiberTrajectoryManager.h
DisplayProperties.h
DisplayPropertiesBorders.h
DisplayPropertiesFiberOrientation.h
DisplayPropertiesFoci.h
DisplayPropertiesLabels.h
DisplayPropertiesSurface.h
DisplayPropertiesVolume.h
EventBrowserTabDelete.h
EventBrowserTabGet.h
EventBrowserTabGetAll.h
EventBrowserTabNew.h
EventCaretMappableDataFilesGet.h
EventDataFileRead.h
EventDataFileReload.h
EventIdentificationHighlightLocation.h
EventNodeDataFilesGet.h
EventModelAdd.h
EventModelDelete.h
EventModelGetAll.h
EventModelSurfaceGet.h
EventOverlayValidate.h
EventOverlayYokingGroupGet.h
EventSpecFileReadDataFiles.h
EventSurfacesGet.h
FeatureColoringTypeEnum.h
FiberOrientationSymbolTypeEnum.h
FociDrawingTypeEnum.h
IdentificationManager.h
IdentificationStringBuilder.h
IdentificationTextGenerator.h
IdentificationWithColor.h
IdentifiedItem.h
IdentifiedItemNode.h
LabelDrawingTypeEnum.h
Model.h
ModelSurface.h
ModelSurfaceMontage.h
ModelSurfaceSelector.h
ModelTypeEnum.h
ModelVolume.h
ModelWholeBrain.h
Overlay.h
OverlaySet.h
OverlaySetArray.h
OverlayYokingGroupEnum.h
ProjectionViewTypeEnum.h
SelectionItemDataTypeEnum.h
SelectionItem.h
SelectionItemBorderSurface.h
SelectionItemFocusSurface.h
SelectionItemFocusVolume.h
SelectionItemSurfaceNode.h
SelectionItemSurfaceNodeIdentificationSymbol.h
SelectionItemSurfaceTriangle.h
SelectionItemVoxel.h
SelectionManager.h
SessionManager.h
Surface.h
SurfaceDrawingTypeEnum.h
SurfaceMontageViewport.h
SurfaceNodeColoring.h
SurfaceSelectionModel.h
ViewingTransformations.h
VolumeSliceSettings.h
VolumeSliceViewModeEnum.h
VolumeSurfaceOutlineColorOrTabModel.h
VolumeSurfaceOutlineModel.h
VolumeSurfaceOutlineSetModel.h
WholeBrainSurfaceSettings.h
WholeBrainVoxelDrawingMode.h
YokingGroupEnum.h

BorderDrawingTypeEnum.cxx
Brain.cxx
BrainOpenGL.cxx
BrainOpenGLFixedPipeline.cxx
BrainOpenGLShape.cxx
BrainOpenGLShapeCone.cxx
BrainOpenGLShapeCube.cxx
BrainOpenGLShapeRing.cxx
BrainOpenGLShapeSphere.cxx
BrainOpenGLViewportContent.cxx
BrainordinateDataSelection.cxx
BrainStructure.cxx
BrainStructureNodeAttributes.cxx
BrowserTabContent.cxx
ChartingDataManager.cxx
CiftiConnectivityMatrixDataFileManager.cxx
CiftiFiberTrajectoryManager.cxx
DisplayProperties.cxx
DisplayPropertiesBorders.cxx
DisplayPropertiesFiberOrientation.cxx
DisplayPropertiesFoci.cxx
DisplayPropertiesLabels.cxx
DisplayPropertiesSurface.cxx
DisplayPropertiesVolume.cxx
EventBrowserTabDelete.cxx
EventBrowserTabGet.cxx
EventBrowserTabGetAll.cxx
EventBrowserTabNew.cxx
EventDataFileRead.cxx
EventDataFileReload.cxx
EventCaretMappableDataFilesGet.cxx
EventIdentificationHighlightLocation.cxx
EventNodeDataFilesGet.cxx
EventModelAdd.cxx
EventModelDelete.cxx
EventModelGetAll.cxx
EventModelSurfaceGet.cxx
EventOverlayValidate.cxx
EventOverlayYokingGroupGet.cxx
EventSpecFileReadDataFiles.cxx
EventSurfacesGet.cxx
FeatureColoringTypeEnum.cxx
FiberOrientationSymbolTypeEnum.cxx
FociDrawingTypeEnum.cxx
IdentificationManager.cxx
IdentificationStringBuilder.cxx
IdentificationTextGenerator.cxx
IdentificationWithColor.cxx
IdentifiedItem.cxx
IdentifiedItemNode.cxx
LabelDrawingTypeEnum.cxx
Model.cxx
ModelSurface.cxx
ModelSurfaceMontage.cxx
ModelSurfaceSelector.cxx
ModelTypeEnum.cxx
ModelVolume.cxx
ModelWholeBrain.cxx
Overlay.cxx
OverlaySet.cxx
OverlaySetArray.cxx
OverlayYokingGroupEnum.cxx
ProjectionViewTypeEnum.cxx
SelectionItemDataTypeEnum.cxx
SelectionItem.cxx
SelectionItemBorderSurface.cxx
SelectionItemFocusSurface.cxx
SelectionItemFocusVolume.cxx
SelectionItemSurfaceNode.cxx
SelectionItemSurfaceNodeIdentificationSymbol.cxx
SelectionItemSurfaceTriangle.cxx
SelectionItemVoxel.cxx
SelectionManager.cxx
SessionManager.cxx
Surface.cxx
SurfaceDrawingTypeEnum.cxx
SurfaceMontageViewport.cxx
SurfaceNodeColoring.cxx
SurfaceSelectionModel.cxx
ViewingTransformations.cxx
VolumeSliceSettings.cxx
VolumeSliceViewModeEnum.cxx
VolumeSurfaceOutlineColorOrTabModel.cxx
VolumeSurfaceOutlineModel.cxx
VolumeSurfaceOutlineSetModel.cxx
WholeBrainSurfaceSettings.cxx
WholeBrainVoxelDrawingMode.cxx
YokingGroupEnum.cxx
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
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
