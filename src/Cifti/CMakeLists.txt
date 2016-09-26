
#
# Name of Project
#
PROJECT (Cifti)

#
# Add QT for includes
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    SET(QT_DONT_USE_QTGUI)
    SET(QT_USE_QTNETWORK TRUE)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

#
# Create GIFTI Library
#
ADD_LIBRARY(Cifti
CiftiInterface.h
CiftiXMLOld.h
CiftiXMLElements.h
CiftiXMLReader.h
CiftiXMLWriter.h

CiftiFile.h
CiftiXML.h
CiftiMappingType.h
CiftiBrainModelsMap.h
CiftiLabelsMap.h
CiftiParcelsMap.h
CiftiScalarsMap.h
CiftiSeriesMap.h
CiftiVersion.h

CiftiInterface.cxx
CiftiXMLOld.cxx
CiftiXMLElements.cxx
CiftiXMLReader.cxx
CiftiXMLWriter.cxx

CiftiFile.cxx
CiftiXML.cxx
CiftiMappingType.cxx
CiftiBrainModelsMap.cxx
CiftiLabelsMap.cxx
CiftiParcelsMap.cxx
CiftiScalarsMap.cxx
CiftiSeriesMap.cxx
CiftiVersion.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
