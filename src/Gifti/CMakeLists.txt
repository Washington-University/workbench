
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
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
endif()

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

TARGET_LINK_LIBRARIES(Gifti ${CARET_QT5_LINK})

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
