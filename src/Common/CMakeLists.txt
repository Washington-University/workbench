
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

INCLUDE(${QT_USE_FILE})
#message("QT VERSION: " ${QT_VERSION_MAJOR} ${QT_VERSION_MINOR} ${QT_VERSION_PATCH})

SET(MOC_INPUT_HEADER_FILES
CaretHttpManager.h
)

QT4_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})

#
# Create a library
#
ADD_LIBRARY(Common
ApplicationInformation.h
ApplicationTypeEnum.h
AString.h
AStringNaturalComparison.h
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
DataFileContentInformation.h
DataFileContentCopyMoveInterface.h
DataFileException.h
DataFileInterface.h
DataFileTypeEnum.h
DescriptiveStatistics.h
DeveloperFlagsEnum.h
DisplayGroupAndTabItemInterface.h 
DisplayGroupEnum.h
DrawnWithOpenGLTextureInfo.h
DrawnWithOpenGLTextureInterface.h
ElapsedTimer.h
Event.h
EventAlertUser.h
EventBrowserTabIndicesGetAll.h
EventGetViewportSize.h
EventListenerInterface.h
EventManager.h
EventPaletteGetByName.h
EventProgressUpdate.h
EventOpenGLTexture.h
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
YokingGroupEnum.h

${MOC_SOURCE_FILES}
${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
ApplicationTypeEnum.cxx
AString.cxx
AStringNaturalComparison.cxx
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
DataFileContentInformation.cxx
DataFileException.cxx
DataFileTypeEnum.cxx
DescriptiveStatistics.cxx
DeveloperFlagsEnum.cxx
DisplayGroupEnum.cxx
DrawnWithOpenGLTextureInfo.cxx
ElapsedTimer.cxx
Event.cxx
EventAlertUser.cxx
EventBrowserTabIndicesGetAll.cxx
EventGetViewportSize.cxx
EventManager.cxx
EventPaletteGetByName.cxx
EventProgressUpdate.cxx
EventOpenGLTexture.cxx
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
ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx
    COMMAND ${CMAKE_COMMAND}
        -DINFILE="${CMAKE_SOURCE_DIR}/Common/ApplicationInformation.cxx.in"
        -DOUTFILE="${CMAKE_BINARY_DIR}/Common/ApplicationInformation.cxx"
        -P "${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..
    DEPENDS ApplicationInformation.cxx.in ${CMAKE_SOURCE_DIR}/CMakeScripts/git_commit_info.cmake.in ${CMAKE_SOURCE_DIR}/../.git/HEAD ${CMAKE_SOURCE_DIR}/../.git/index
    COMMENT "Setting commit info"
)
