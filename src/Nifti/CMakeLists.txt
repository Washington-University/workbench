#
# The NIFTI Project
#
project (Nifti)

#
# Need XML from Qt
#

#
# Add QT for includes
#
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()

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

TARGET_LINK_LIBRARIES(Nifti ${CARET_QT5_LINK})

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
