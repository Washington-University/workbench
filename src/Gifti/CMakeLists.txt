
#
# Name of Project
#
PROJECT (Gifti)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create GIFTI Library
#
ADD_LIBRARY(Gifti
GiftiArrayIndexingOrderEnum.h
GiftiDataArray.h
GiftiEncodingEnum.h
GiftiEndianEnum.h
GiftiFile.h
GiftiFileSaxReader.h
GiftiFileWriter.h
GiftiLabelTableSaxReader.h
GiftiMetaDataSaxReader.h

GiftiArrayIndexingOrderEnum.cxx
GiftiDataArray.cxx
GiftiEncodingEnum.cxx
GiftiEndianEnum.cxx
GiftiFile.cxx
GiftiFileSaxReader.cxx
GiftiFileWriter.cxx
GiftiLabelTableSaxReader.cxx
GiftiMetaDataSaxReader.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
