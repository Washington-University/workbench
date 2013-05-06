
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
CiftiXMLElements.h
CiftiXML.h
CiftiXMLReader.h
CiftiXMLWriter.h
CiftiXnat.h

CiftiFile.cxx
CiftiFileException.cxx
CiftiHeader.cxx
CiftiHeaderIO.cxx
CiftiInterface.cxx
CiftiMatrix.cxx
CiftiXML.cxx
CiftiXMLElements.cxx
CiftiXMLReader.cxx
CiftiXMLWriter.cxx
CiftiXnat.cxx
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
