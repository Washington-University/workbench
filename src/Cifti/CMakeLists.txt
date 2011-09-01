
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
CiftiByteSwap.h
CiftiFileException.h
CiftiFile.h
CiftiMatrix.h
CiftiXMLElements.h
CiftiXML.h
CiftiXMLReader.h
CiftiXMLWriter.h
nifti1.h
nifti2.h
Nifti2Header.h

CiftiFile.cxx
CiftiFileException.cxx
CiftiMatrix.cxx
CiftiXML.cxx
CiftiXMLElements.cxx
CiftiXMLReader.cxx
CiftiXMLWriter.cxx
Nifti2Header.cxx
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
)
