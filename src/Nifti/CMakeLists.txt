#
# The NIFTI Project
#
project (Nifti)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})


#
# Create the NIFTI library
#
ADD_LIBRARY(Nifti
Layout.h
Matrix4x4.h
NiftiAbstractHeader.h
NiftiAbstractVolumeExtension.h
NiftiFile.h
NiftiHeader.h
NiftiHeaderIO.h
NiftiIO.h
Nifti1Header.h
Nifti2Header.h
NiftiMatrix.h

Layout.cxx
Matrix4x4.cxx
NiftiFile.cxx
NiftiHeader.cxx
NiftiHeaderIO.cxx
NiftiIO.cxx
Nifti1Header.cxx
Nifti2Header.cxx
NiftiMatrix.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Xml
)
