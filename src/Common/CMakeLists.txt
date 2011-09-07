
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
CaretConstants.h
CaretException.h
CaretObject.h
CaretWindowEnum.h
DataCompressZLib.h
DataFile.h
DataFileException.h
Event.h
EventListenerInterface.h
EventLoadSurfaceFile.h
EventManager.h
EventUpdateAllGraphics.h
FileInformation.h
MathFunctions.h
MessageManager.h
ProgramParameters.h
ProgramParametersException.h
SystemUtilities.h
TracksModificationInterface.h

AString.cxx
Base64.cxx
BoundingBox.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CaretAssertion.cxx
CaretConstants.cxx
CaretException.cxx
CaretObject.cxx
CaretWindowEnum.cxx
DataCompressZLib.cxx
DataFile.cxx
DataFileException.cxx
Event.cxx
EventLoadSurfaceFile.cxx
EventManager.cxx
EventUpdateAllGraphics.cxx
FileInformation.cxx
MathFunctions.cxx
MessageManager.cxx
ProgramParameters.cxx
ProgramParametersException.cxx
SystemUtilities.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)
