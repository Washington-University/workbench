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
ControlPoint3D.h
Matrix4x4.h
NiftiHeader.h
NiftiIO.h

ControlPoint3D.cxx
Matrix4x4.cxx
NiftiHeader.cxx
NiftiIO.cxx
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
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
)
