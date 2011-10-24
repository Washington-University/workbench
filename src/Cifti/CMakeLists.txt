
#
# Name of Project
#
PROJECT (Cifti)

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
ADD_LIBRARY(Cifti
#CiftiByteSwap.h
CiftiFileException.h
CiftiFile.h
CiftiXMLElements.h
CiftiXML.h
CiftiXMLReader.h
CiftiXMLWriter.h

CiftiFile.cxx
CiftiFileException.cxx
CiftiXML.cxx
CiftiXMLElements.cxx
CiftiXMLReader.cxx
CiftiXMLWriter.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Files
)
