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
Matrix4x4.h
NiftiEnums.h
NiftiException.h
NiftiFile.h
NiftiHeaderIO.h
Nifti1Header.h
Nifti2Header.h
NiftiMatrix.h
nifti1.h
nifti2.h


Matrix4x4.cxx
NiftiEnums.cxx
NiftiException.cxx
NiftiFile.cxx
NiftiHeaderIO.cxx
Nifti1Header.cxx
Nifti2Header.cxx
NiftiMatrix.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
)
