
#
# Name of Project
#
PROJECT(Common)


#
# QT include files
#
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
AString.h
Base64.h
BoundingBox.h
ByteOrderEnum.h
ByteSwapping.h
CaretAssert.h
CaretAssertion.h
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
ColorManager.h
CubicSpline.h
DataCompressZLib.h
DataFile.h
DataFileException.h
DataFileInterface.h
DescriptiveStatistics.h
ElapsedTimer.h
Event.h
EventListenerInterface.h
EventManager.h
EventProgressUpdate.h
EventTypeEnum.h
FastStatistics.h
FileAdapter.h
FileInformation.h
FloatMatrix.h
Histogram.h
HtmlStringBuilder.h
Logger.h
LogHandler.h
LogHandlerInformationTextDisplay.h
LogHandlerStandardError.h
LogLevelEnum.h
LogManager.h
LogRecord.h
MathFunctionEnum.h
MathFunctions.h
MatrixFunctions.h
ModelTransform.h
NameIndexSort.h
NetworkException.h
OctTree.h
Plane.h
ProgramParameters.h
ProgramParametersException.h
ProgressObject.h
ProgressReportingInterface.h
ReductionEnum.h
ReductionOperation.h
SpeciesEnum.h
StereotaxicSpaceEnum.h
StructureEnum.h
SystemUtilities.h
TracksModificationInterface.h
Vector3D.h

${MOC_SOURCE_FILES}
ApplicationInformation.cxx
AString.cxx
Base64.cxx
BoundingBox.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CaretAssertion.cxx
CaretColorEnum.cxx
CaretCommandLine.cxx
CaretException.cxx
CaretHttpManager.cxx
CaretLogger.cxx
CaretMathExpression.cxx
CaretMutex.cxx
CaretObject.cxx
CaretObjectTracksModification.cxx
CaretPointLocator.cxx
CaretPreferences.cxx
ColorManager.cxx
CaretTemporaryFile.cxx
CubicSpline.cxx
DataCompressZLib.cxx
DataFile.cxx
DataFileException.cxx
DescriptiveStatistics.cxx
ElapsedTimer.cxx
Event.cxx
EventManager.cxx
EventProgressUpdate.cxx
EventTypeEnum.cxx
FastStatistics.cxx
FileAdapter.cxx
FileInformation.cxx
FloatMatrix.cxx
Histogram.cxx
HtmlStringBuilder.cxx
Logger.cxx
LogHandler.cxx
LogHandlerInformationTextDisplay.cxx
LogHandlerStandardError.cxx
LogLevelEnum.cxx
LogManager.cxx
LogRecord.cxx
MathFunctionEnum.cxx
MathFunctions.cxx
ModelTransform.cxx
NameIndexSort.cxx
NetworkException.cxx
Plane.cxx
ProgramParameters.cxx
ProgramParametersException.cxx
ProgressObject.cxx
ReductionEnum.cxx
ReductionOperation.cxx
SpeciesEnum.cxx
StereotaxicSpaceEnum.cxx
StructureEnum.cxx
SystemUtilities.cxx
Vector3D.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)
