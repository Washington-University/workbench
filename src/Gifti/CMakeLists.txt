
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
GiftiException.h
GiftiFile.h
GiftiFileSaxReader.h
GiftiFileWriter.h
GiftiLabel.h
GiftiLabelTable.h
GiftiLabelTableSaxReader.h
GiftiMetaData.h
GiftiMetaDataSaxReader.h
GiftiMetaDataXmlElements.h
GiftiXmlElements.h

GiftiArrayIndexingOrderEnum.cxx
GiftiDataArray.cxx
GiftiEncodingEnum.cxx
GiftiEndianEnum.cxx
GiftiException.cxx
GiftiFile.cxx
GiftiFileSaxReader.cxx
GiftiFileWriter.cxx
GiftiLabel.cxx
GiftiLabelTable.cxx
GiftiLabelTableSaxReader.cxx
GiftiMetaData.cxx
GiftiMetaDataSaxReader.cxx
GiftiXmlElements.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
