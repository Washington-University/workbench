
#
# Name of Project
#
PROJECT (Cifti)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)
SET(QT_USE_QTNETWORK TRUE)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create GIFTI Library
#
ADD_LIBRARY(Cifti
CiftiFileException.h
CiftiFile.h
CiftiHeader.h
CiftiHeaderIO.h
CiftiInterface.h
CiftiMatrix.h
CiftiVersion.h
CiftiXMLOld.h
CiftiXMLElements.h
CiftiXMLReader.h
CiftiXMLWriter.h
CiftiXnat.h

CiftiXML.h
CiftiMappingType.h
CiftiBrainModelsMap.h
CiftiLabelsMap.h
CiftiParcelsMap.h
CiftiScalarsMap.h
CiftiSeriesMap.h

CiftiFile.cxx
CiftiFileException.cxx
CiftiHeader.cxx
CiftiHeaderIO.cxx
CiftiInterface.cxx
CiftiMatrix.cxx
CiftiVersion.cxx
CiftiXMLOld.cxx
CiftiXMLElements.cxx
CiftiXMLReader.cxx
CiftiXMLWriter.cxx
CiftiXnat.cxx

CiftiXML.cxx
CiftiMappingType.cxx
CiftiBrainModelsMap.cxx
CiftiLabelsMap.cxx
CiftiParcelsMap.cxx
CiftiScalarsMap.cxx
CiftiSeriesMap.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
