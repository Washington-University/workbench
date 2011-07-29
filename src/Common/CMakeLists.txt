
#
# Name of Project
#
PROJECT(Common)

#
# Create a library
#
ADD_LIBRARY(Common 
Base64.h
BoundingBox.h
ByteOrder.h
ByteSwapping.h
CaretException.h
CaretObject.h
CaretWindow.h
DataCompressZLib.h
DataFile.h
DataFileException.h
MathFunctions.h
StringUtilities.h
SystemUtilities.h
TracksModificationInterface.h

Base64.cxx
BoundingBox.cxx
ByteOrder.cxx
ByteSwapping.cxx
CaretException.cxx
CaretObject.cxx
CaretWindow.cxx
DataCompressZLib.cxx
DataFile.cxx
DataFileException.cxx
MathFunctions.cxx
StringUtilities.cxx
SystemUtilities.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
)
