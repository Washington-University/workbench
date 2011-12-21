
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
AString.h
Base64.h
BoundingBox.h
ByteOrderEnum.h
ByteSwapping.h
CaretAssert.h
CaretAssertion.h
CaretException.h
CaretFunctionName.h
CaretHeap.h
CaretHttpManager.h
CaretLogger.h
CaretMutex.h
CaretObject.h
CaretOMP.h
CaretPointer.h
CaretPreferences.h
CubicSpline.h
DataCompressZLib.h
DataFile.h
DataFileException.h
DataFileInterface.h
DescriptiveStatistics.h
ElapsedTimer.h
Event.h
EventInformationTextDisplay.h
EventListenerInterface.h
EventManager.h
EventProgressUpdate.h
EventTypeEnum.h
FileInformation.h
FloatMatrix.h
HtmlStringBuilder.h
Logger.h
LogHandler.h
LogHandlerInformationTextDisplay.h
LogHandlerStandardError.h
LogLevelEnum.h
LogManager.h
LogRecord.h
MathFunctions.h
MatrixFunctions.h
NetworkException.h
OctTree.h
Plane.h
ProgramParameters.h
ProgramParametersException.h
ProgressObject.h
SpeciesEnum.h
StereotaxicSpaceEnum.h
StructureEnum.h
SystemUtilities.h
TracksModificationInterface.h
UserView.h
Vector3D.h

${MOC_SOURCE_FILES}
AString.cxx
Base64.cxx
BoundingBox.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CaretAssertion.cxx
CaretException.cxx
CaretHttpManager.cxx
CaretLogger.cxx
CaretMutex.cxx
CaretObject.cxx
CaretPreferences.cxx
CubicSpline.cxx
DataCompressZLib.cxx
DataFile.cxx
DataFileException.cxx
DescriptiveStatistics.cxx
ElapsedTimer.cxx
Event.cxx
EventInformationTextDisplay.cxx
EventManager.cxx
EventProgressUpdate.cxx
EventTypeEnum.cxx
FileInformation.cxx
FloatMatrix.cxx
HtmlStringBuilder.cxx
Logger.cxx
LogHandler.cxx
LogHandlerInformationTextDisplay.cxx
LogHandlerStandardError.cxx
LogLevelEnum.cxx
LogManager.cxx
LogRecord.cxx
MathFunctions.cxx
NetworkException.cxx
Plane.cxx
ProgramParameters.cxx
ProgramParametersException.cxx
ProgressObject.cxx
SpeciesEnum.cxx
StereotaxicSpaceEnum.cxx
StructureEnum.cxx
SystemUtilities.cxx
UserView.cxx
Vector3D.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)
