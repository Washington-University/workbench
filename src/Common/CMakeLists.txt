
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
ByteOrderEnum.h
ByteSwapping.h
CaretException.h
CaretObject.h
CaretWindowEnum.h
DataCompressZLib.h
DataFile.h
DataFileException.h
MathFunctions.h
StringUtilities.h
SystemUtilities.h
TracksModificationInterface.h

Base64.cxx
BoundingBox.cxx
ByteOrderEnum.cxx
ByteSwapping.cxx
CaretException.cxx
CaretObject.cxx
CaretWindowEnum.cxx
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
