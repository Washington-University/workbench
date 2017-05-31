
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

if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    include_directories(${Qt5Xml_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()
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

    IF(Qt5_FOUND)
        QT5_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
    IF (QT4_FOUND)
        QT4_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF ()
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
BackgroundAndForegroundColorsModeEnum.h
Base64.h
BoundingBox.h
BrainConstants.h
ByteOrderEnum.h
ByteSwapping.h
CaretAssert.h
CaretAssertion.h
CaretBinaryFile.h
CaretColorEnum.h
CaretCommandLine.h
CaretCompact3DLookup.h
CaretCompactLookup.h
CaretException.h
CaretFunctionName.h
CaretHeap.h
CaretHttpManager.h
CaretLogger.h
CaretMathExpression.h
CaretMutex.h
CaretObject.h
CaretObjectTracksModification.h
CaretOMP.h
CaretPointer.h
CaretPointLocator.h
CaretPreferences.h
CaretTemporaryFile.h
CaretUndoCommand.h
CaretUndoStack.h
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
ElapsedTimer.h
Event.h
EventAlertUser.h
EventBrowserTabIndicesGetAll.h
EventCaretPreferencesGet.h
EventGetViewportSize.h
EventListenerInterface.h
EventManager.h
EventPaletteGetByName.h
EventProgressUpdate.h
EventTypeEnum.h
FastStatistics.h
FileAdapter.h
FileInformation.h
FloatMatrix.h
Histogram.h
HtmlStringBuilder.h
ImageCaptureMethodEnum.h
Logger.h
LogHandler.h
LogHandlerStandardError.h
LogLevelEnum.h
LogManager.h
LogRecord.h
MathFunctionEnum.h
MathFunctions.h
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
ReductionEnum.h
ReductionOperation.h
SpecFileDialogViewFilesTypeEnum.h
SpeciesEnum.h
StereotaxicSpaceEnum.h
StringTableModel.h
StructureEnum.h
SystemUtilities.h
TileTabsConfiguration.h
TracksModificationInterface.h
TriStateSelectionStatusEnum.h
Vector3D.h
VectorOperation.h
VoxelIJK.h
WorkbenchSpecialVersionEnum.h
YokingGroupEnum.h

${MOC_SOURCE_FILES}

${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
ApplicationTypeEnum.cxx
AString.cxx
AStringNaturalComparison.cxx
BackgroundAndForegroundColors.cxx
BackgroundAndForegroundColorsModeEnum.cxx
Base64.cxx
BoundingBox.cxx
BrainConstants.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CaretAssertion.cxx
CaretBinaryFile.cxx
CaretColorEnum.cxx
CaretCommandLine.cxx
CaretException.cxx
CaretHttpManager.cxx
CaretLogger.cxx
CaretMathExpression.cxx
CaretObject.cxx
CaretObjectTracksModification.cxx
CaretPointLocator.cxx
CaretPreferences.cxx
CaretTemporaryFile.cxx
CaretUndoCommand.cxx
CaretUndoStack.cxx
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
ElapsedTimer.cxx
Event.cxx
EventAlertUser.cxx
EventBrowserTabIndicesGetAll.cxx
EventCaretPreferencesGet.cxx
EventGetViewportSize.cxx
EventManager.cxx
EventPaletteGetByName.cxx
EventProgressUpdate.cxx
EventTypeEnum.cxx
FastStatistics.cxx
FileAdapter.cxx
FileInformation.cxx
FloatMatrix.cxx
Histogram.cxx
HtmlStringBuilder.cxx
ImageCaptureMethodEnum.cxx
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
ReductionEnum.cxx
ReductionOperation.cxx
SpecFileDialogViewFilesTypeEnum.cxx
SpeciesEnum.cxx
StereotaxicSpaceEnum.cxx
StringTableModel.cxx
StructureEnum.cxx
SystemUtilities.cxx
TileTabsConfiguration.cxx
TriStateSelectionStatusEnum.cxx
Vector3D.cxx
VectorOperation.cxx
WorkbenchSpecialVersionEnum.cxx
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
            -P "${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in"
        WORKING_DIRECTORY ${GIT_REPOSITORY}/..
        DEPENDS ApplicationInformation.cxx.in ${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in ${GIT_REPOSITORY}/HEAD ${GIT_REPOSITORY}/index
        COMMENT "Setting commit info"
    )
ELSE(EXISTS ${GIT_REPOSITORY})
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
        COMMAND ${CMAKE_COMMAND}
            -DINFILE="${CMAKE_SOURCE_DIR}/Common/ApplicationInformation.cxx.in"
            -DOUTFILE="${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx"
            -P "${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
        DEPENDS ApplicationInformation.cxx.in ${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in
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

