
#
# Name of Project
#
PROJECT(Common)


#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)
INCLUDE(${QT_USE_FILE})
#message("QT VERSION: " ${QT_VERSION_MAJOR} ${QT_VERSION_MINOR} ${QT_VERSION_PATCH})

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
CaretLogger.h
CaretMutex.h
CaretObject.h
DataCompressZLib.h
DataFile.h
DataFileException.h
ElapsedTimer.h
Event.h
EventListenerInterface.h
EventManager.h
EventProgressUpdate.h
EventTypeEnum.h
FileInformation.h
FloatMatrix.h
Logger.h
LogHandler.h
LogHandlerStandardError.h
LogLevelEnum.h
LogManager.h
LogRecord.h
MathFunctions.h
MatrixFunctions.h
MessageManager.h
ProgramParameters.h
ProgramParametersException.h
ProgressObject.h
SpeciesEnum.h
StereotaxicSpaceEnum.h
StructureEnum.h
SystemUtilities.h
TracksModificationInterface.h

AString.cxx
Base64.cxx
BoundingBox.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CaretAssertion.cxx
CaretException.cxx
CaretLogger.cxx
CaretMutex.cxx
CaretObject.cxx
DataCompressZLib.cxx
DataFile.cxx
DataFileException.cxx
ElapsedTimer.cxx
Event.cxx
EventManager.cxx
EventProgressUpdate.cxx
EventTypeEnum.cxx
FileInformation.cxx
FloatMatrix.cxx
Logger.cxx
LogHandler.cxx
LogHandlerStandardError.cxx
LogLevelEnum.cxx
LogManager.cxx
LogRecord.cxx
MathFunctions.cxx
MessageManager.cxx
ProgramParameters.cxx
ProgramParametersException.cxx
ProgressObject.cxx
SpeciesEnum.cxx
StereotaxicSpaceEnum.cxx
StructureEnum.cxx
SystemUtilities.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)
