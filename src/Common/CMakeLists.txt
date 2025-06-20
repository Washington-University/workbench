
#
# Name of Project
#
PROJECT(Common)


#
# QT include files
#
SET(QT_USE_QTXML TRUE)
SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTNETWORK TRUE)

if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Core5Compat_INCLUDE_DIRS})
    include_directories(${Qt6Gui_INCLUDE_DIRS})
    include_directories(${Qt6Network_INCLUDE_DIRS})
    include_directories(${Qt6Xml_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
endif()
#message("QT VERSION: " ${QT_VERSION_MAJOR} ${QT_VERSION_MINOR} ${QT_VERSION_PATCH})

#
# With AUTOMOC, do not need to specify files
# that contain Q_OBJECT macro for Qt to process with 'moc'
# (meta-object compiler).
#
IF(WORKBENCH_USE_CMAKE_AUTOMOC)
    SET(CMAKE_AUTOMOC ON)
ELSE()
    SET(MOC_INPUT_HEADER_FILES
        CaretHttpManager.h
    )

    IF(Qt6_FOUND)
        QT6_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
    IF(Qt5_FOUND)
        QT5_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
ENDIF()

#
# Create a library
#
ADD_LIBRARY(Common
ApplicationInformation.h
ApplicationTypeEnum.h
AString.h
AStringNaturalComparison.h
BackgroundAndForegroundColors.h
Base64.h
BoundingBox.h
BrainConstants.h
ByteOrderEnum.h
ByteSwapping.h
CardinalDirectionEnum.h
CaretAssert.h
CaretAssertion.h
CaretBinaryFile.h
CaretColor.h
CaretColorEnum.h
CaretCommandLine.h
CaretCompact3DLookup.h
CaretCompactLookup.h
CaretException.h
CaretFiveFileTypeEnum.h
CaretFunctionName.h
CaretHeap.h
CaretHierarchy.h
CaretHttpManager.h
CaretLogger.h
CaretMathExpression.h
CaretMutex.h
CaretObject.h
CaretObjectTracksModification.h
CaretOMP.h
CaretPointer.h
CaretPointLocator.h
CaretPreferenceDataValue.h
CaretPreferenceDataValueList.h
CaretPreferenceValueSceneOverrideModeEnum.h
CaretPreferences.h
CaretResult.h
CaretRgb.h
CaretTemporaryFile.h
CaretUndoCommand.h
CaretUndoStack.h
CaretUnitsTypeEnum.h
Cluster.h
ClusterContainer.h
ColorFunctions.h
CubicSpline.h
DataCompressZLib.h
DataFile.h
DataFileContentCopyMoveInterface.h
DataFileContentCopyMoveParameters.h
DataFileContentInformation.h
DataFileException.h
DataFileInterface.h
DataFileTypeEnum.h
DescriptiveStatistics.h
DeveloperFlagsEnum.h
DisplayGroupAndTabItemInterface.h 
DisplayGroupEnum.h
DisplayHighDpiModeEnum.h
ElapsedTimer.h
Event.h
EventAlertUser.h
EventBrowserTabClose.h
EventBrowserTabDelete.h
EventBrowserTabIndicesGetAll.h
EventBrowserTabIndicesGetAllViewed.h
EventBrowserTabNew.h
EventBrowserTabNewClone.h
EventBrowserTabReopenClosed.h
EventBrowserTabSelectInWindow.h
EventBrowserWindowPixelSizeInfoEvent.h
EventCaretPreferencesGet.h
EventGetViewportSize.h
EventListenerInterface.h
EventManager.h
EventPaletteGetByName.h
EventProgressUpdate.h
EventRecentFilesSystemAccessMode.h
EventResetView.h
EventSurfaceNodesGetNearXYZ.h
EventTileTabsGridConfigurationModification.h
EventTypeEnum.h
FastStatistics.h
FileAdapter.h
FileInformation.h
FileOpenFromOpSysTypeEnum.h
FloatMatrix.h
FunctionResult.h
HemisphereEnum.h
Histogram.h
HtmlStringBuilder.h
HtmlTableBuilder.h
IdentificationDisplayModeEnum.h
ImageCaptureMethodEnum.h
InfoItem.h
JsonHelper.h
LinearEquationTransform.h
Logger.h
LogHandler.h
LogHandlerStandardError.h
LogLevelEnum.h
LogManager.h
LogRecord.h
MathFunctionEnum.h
MathFunctions.h
Matrix4x4Interface.h
MatrixFunctions.h
ModelTransform.h
MultiDimArray.h
MultiDimIterator.h
NetworkException.h
NumericFormatModeEnum.h
NumericTextFormatting.h
OctTree.h
OpenGLDrawingMethodEnum.h
PlainTextStringBuilder.h
Plane.h
ProgramParameters.h
ProgramParametersException.h
ProgressObject.h
ProgressReportingInterface.h
QtPluginsPathSetup.h
RecentFileItem.h
RecentFileItemSortingKeyEnum.h
RecentFileItemTypeEnum.h
RecentFileItemsContainer.h
RecentFileItemsContainerModeEnum.h
RecentFileItemsFilter.h
RecentFilesSystemAccessModeEnum.h
RecentSceneInfoContainer.h
ReductionEnum.h
ReductionOperation.h
SpacerTabIndex.h
SpecFileDialogViewFilesTypeEnum.h
SpeciesEnum.h
StereotaxicSpaceEnum.h
StringTableModel.h
StructureEnum.h
SystemUtilities.h
TileTabsBrowserTabGeometry.h
TileTabsLayoutBackgroundTypeEnum.h
TileTabsLayoutBaseConfiguration.h
TileTabsLayoutConfigurationTypeEnum.h
TileTabsLayoutGridConfiguration.h
TileTabsLayoutManualConfiguration.h
TileTabsGridRowColumnContentTypeEnum.h
TileTabsGridRowColumnElement.h
TileTabsGridRowColumnStretchTypeEnum.h
ToolBarWidthModeEnum.h
TracksModificationInterface.h
TriStateSelectionStatusEnum.h
UnitsConversion.h
Vector3D.h
VectorOperation.h
VolumeMontageCoordinateDisplayTypeEnum.h
VolumeSliceViewAllPlanesLayoutEnum.h
VoxelColorUpdate.h
VoxelIJK.h
WorkbenchInstallationAssistant.h
WorkbenchQtMessageHandler.h
WorkbenchSpecialVersionEnum.h
WuQMacro.h
WuQMacroCommand.h
WuQMacroCommandParameter.h
WuQMacroCommandTypeEnum.h
WuQMacroDataValueTypeEnum.h
WuQMacroFile.h
WuQMacroGroup.h
WuQMacroGroupXmlStreamBase.h
WuQMacroGroupXmlStreamReader.h
WuQMacroGroupXmlStreamWriter.h
WuQMacroModeEnum.h
WuQMacroMouseEventInfo.h
WuQMacroMouseEventTypeEnum.h
WuQMacroShortCutKeyEnum.h
WuQMacroStandardItemTypeEnum.h
WuQMacroWidgetTypeEnum.h
YokingGroupEnum.h

${MOC_SOURCE_FILES}

${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
ApplicationTypeEnum.cxx
AString.cxx
AStringNaturalComparison.cxx
BackgroundAndForegroundColors.cxx
Base64.cxx
BoundingBox.cxx
BrainConstants.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CardinalDirectionEnum.cxx
CaretAssertion.cxx
CaretBinaryFile.cxx
CaretColor.cxx
CaretColorEnum.cxx
CaretCommandLine.cxx
CaretException.cxx
CaretFiveFileTypeEnum.cxx
CaretHierarchy.cxx
CaretHttpManager.cxx
CaretLogger.cxx
CaretMathExpression.cxx
CaretObject.cxx
CaretObjectTracksModification.cxx
CaretPointLocator.cxx
CaretPreferenceDataValue.cxx
CaretPreferenceDataValueList.cxx
CaretPreferenceValueSceneOverrideModeEnum.cxx
CaretPreferences.cxx
CaretResult.cxx
CaretRgb.cxx
CaretTemporaryFile.cxx
CaretUndoCommand.cxx
CaretUndoStack.cxx
CaretUnitsTypeEnum.cxx
Cluster.cxx
ClusterContainer.cxx
ColorFunctions.cxx
CubicSpline.cxx
DataCompressZLib.cxx
DataFile.cxx
DataFileContentCopyMoveParameters.cxx
DataFileContentInformation.cxx
DataFileException.cxx
DataFileTypeEnum.cxx
DescriptiveStatistics.cxx
DeveloperFlagsEnum.cxx
DisplayGroupAndTabItemInterface.cxx
DisplayGroupEnum.cxx
DisplayHighDpiModeEnum.cxx
ElapsedTimer.cxx
Event.cxx
EventAlertUser.cxx
EventBrowserTabClose.cxx
EventBrowserTabDelete.cxx
EventBrowserTabIndicesGetAll.cxx
EventBrowserTabIndicesGetAllViewed.cxx
EventBrowserTabNew.cxx
EventBrowserTabNewClone.cxx
EventBrowserTabReopenClosed.cxx
EventBrowserTabSelectInWindow.cxx
EventBrowserWindowPixelSizeInfoEvent.cxx
EventCaretPreferencesGet.cxx
EventGetViewportSize.cxx
EventListenerInterface.cxx
EventManager.cxx
EventPaletteGetByName.cxx
EventProgressUpdate.cxx
EventRecentFilesSystemAccessMode.cxx
EventResetView.cxx
EventSurfaceNodesGetNearXYZ.cxx
EventTileTabsGridConfigurationModification.cxx
EventTypeEnum.cxx
FastStatistics.cxx
FileAdapter.cxx
FileInformation.cxx
FileOpenFromOpSysTypeEnum.cxx
FloatMatrix.cxx
HemisphereEnum.cxx
Histogram.cxx
HtmlStringBuilder.cxx
HtmlTableBuilder.cxx
IdentificationDisplayModeEnum.cxx
ImageCaptureMethodEnum.cxx
InfoItem.cxx
JsonHelper.cxx
LinearEquationTransform.cxx
Logger.cxx
LogHandler.cxx
LogHandlerStandardError.cxx
LogLevelEnum.cxx
LogManager.cxx
LogRecord.cxx
MathFunctionEnum.cxx
MathFunctions.cxx
ModelTransform.cxx
NetworkException.cxx
NumericFormatModeEnum.cxx
NumericTextFormatting.cxx
OpenGLDrawingMethodEnum.cxx
PlainTextStringBuilder.cxx
Plane.cxx
ProgramParameters.cxx
ProgramParametersException.cxx
ProgressObject.cxx
QtPluginsPathSetup.cxx
RecentFileItem.cxx
RecentFileItemSortingKeyEnum.cxx
RecentFileItemTypeEnum.cxx
RecentFileItemsContainer.cxx
RecentFileItemsContainerModeEnum.cxx
RecentFileItemsFilter.cxx
RecentFilesSystemAccessModeEnum.cxx
RecentSceneInfoContainer.cxx
ReductionEnum.cxx
ReductionOperation.cxx
SpacerTabIndex.cxx
SpecFileDialogViewFilesTypeEnum.cxx
SpeciesEnum.cxx
StereotaxicSpaceEnum.cxx
StringTableModel.cxx
StructureEnum.cxx
SystemUtilities.cxx
TileTabsBrowserTabGeometry.cxx
TileTabsLayoutBackgroundTypeEnum.cxx
TileTabsLayoutBaseConfiguration.cxx
TileTabsLayoutConfigurationTypeEnum.cxx
TileTabsLayoutGridConfiguration.cxx
TileTabsLayoutManualConfiguration.cxx
TileTabsGridRowColumnContentTypeEnum.cxx
TileTabsGridRowColumnElement.cxx
TileTabsGridRowColumnStretchTypeEnum.cxx
ToolBarWidthModeEnum.cxx
TriStateSelectionStatusEnum.cxx
UnitsConversion.cxx
Vector3D.cxx
VectorOperation.cxx
VolumeMontageCoordinateDisplayTypeEnum.cxx
VolumeSliceViewAllPlanesLayoutEnum.cxx
VoxelColorUpdate.cxx
WorkbenchInstallationAssistant.cxx
WorkbenchQtMessageHandler.cxx
WorkbenchSpecialVersionEnum.cxx
WuQMacro.cxx
WuQMacroCommand.cxx
WuQMacroCommandTypeEnum.cxx
WuQMacroCommandParameter.cxx
WuQMacroDataValueTypeEnum.cxx
WuQMacroFile.cxx
WuQMacroGroup.cxx
WuQMacroGroupXmlStreamBase.cxx
WuQMacroGroupXmlStreamReader.cxx
WuQMacroGroupXmlStreamWriter.cxx
WuQMacroModeEnum.cxx
WuQMacroMouseEventInfo.cxx
WuQMacroMouseEventTypeEnum.cxx
WuQMacroShortCutKeyEnum.cxx
WuQMacroStandardItemTypeEnum.cxx
WuQMacroWidgetTypeEnum.cxx
YokingGroupEnum.cxx
)

ADD_DEFINITIONS(-DCOMPILER_NAME="${CMAKE_CXX_COMPILER}")
ADD_DEFINITIONS(-DCOMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}")

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)

SET (GIT_REPOSITORY "${CMAKE_SOURCE_DIR}/../.git")
IF (EXISTS ${GIT_REPOSITORY})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
        COMMAND ${CMAKE_COMMAND}
            -DINFILE="${CMAKE_SOURCE_DIR}/Common/ApplicationInformation.cxx.in"
            -DOUTFILE="${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx"
            -DVERSION="${WB_VERSION}"
            -P "${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in"
        WORKING_DIRECTORY ${GIT_REPOSITORY}/..
        DEPENDS ApplicationInformation.cxx.in ${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in ${CMAKE_SOURCE_DIR}/CMakeLists.txt ${GIT_REPOSITORY}/HEAD ${GIT_REPOSITORY}/index
        COMMENT "Setting commit info"
    )
ELSE(EXISTS ${GIT_REPOSITORY})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
        COMMAND ${CMAKE_COMMAND}
            -DINFILE="${CMAKE_SOURCE_DIR}/Common/ApplicationInformation.cxx.in"
            -DOUTFILE="${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx"
            -DVERSION="${WB_VERSION}"
            -P "${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        DEPENDS ApplicationInformation.cxx.in ${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in ${CMAKE_SOURCE_DIR}/CMakeLists.txt
        COMMENT "No repository found, setting commit info to 'unknown'.  Reconfigure to look for the repository again."
    )
ENDIF(EXISTS ${GIT_REPOSITORY})

#
# Conditionally link the dot library to use the SIMD-based dot product implementation
#
IF (WORKBENCH_USE_SIMD AND CPUINFO_COMPILES)
    TARGET_LINK_LIBRARIES(Common dot ${CARET_QT5_LINK})
ELSE (WORKBENCH_USE_SIMD AND CPUINFO_COMPILES)
    TARGET_LINK_LIBRARIES(Common ${CARET_QT5_LINK})
ENDIF (WORKBENCH_USE_SIMD AND CPUINFO_COMPILES)

