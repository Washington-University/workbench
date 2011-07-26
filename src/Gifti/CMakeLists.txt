
#
# Name of Project
#
PROJECT (Gifti)

#
# Create GIFTI Library
#
ADD_LIBRARY(Gifti
GiftiArrayIndexingOrder.h
GiftiDataArray.h
GiftiEncoding.h
GiftiEndian.h
GiftiException.h
GiftiFile.h
GiftiFileSaxReader.h
GiftiLabel.h
GiftiLabelTable.h
GiftiMetaData.h

GiftiArrayIndexingOrder.cxx
GiftiDataArray.cxx
GiftiEncoding.cxx
GiftiEndian.cxx
GiftiException.cxx
GiftiFile.cxx
GiftiFileSaxReader.cxx
GiftiLabel.cxx
GiftiLabelTable.cxx
GiftiMetaData.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
