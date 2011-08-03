
#
# Name of Project
#
PROJECT (Gifti)

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
GiftiLabel.h
GiftiLabelTable.h
GiftiLabelTableSaxReader.h
GiftiMetaData.h
GiftiMetaDataSaxReader.h
GiftiXmlElements.h

GiftiArrayIndexingOrderEnum.cxx
GiftiDataArray.cxx
GiftiEncodingEnum.cxx
GiftiEndianEnum.cxx
GiftiException.cxx
GiftiFile.cxx
GiftiFileSaxReader.cxx
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
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
