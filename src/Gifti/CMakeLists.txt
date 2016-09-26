
#
# Name of Project
#
PROJECT (Gifti)

#
# Add QT for includes
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    SET(QT_DONT_USE_QTGUI)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

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
