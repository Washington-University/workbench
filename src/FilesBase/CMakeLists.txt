#
# The FilesBase Project
#
project (FilesBase)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Gui_INCLUDE_DIRS})
    include_directories(${Qt6Core5Compat_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Gui_INCLUDE_DIRS})
endif()

#
# Create the NIFTI library
#
ADD_LIBRARY(FilesBase
GiftiLabel.h
GiftiLabelTable.h
LabelViewModeEnum.h
nifti1.h
nifti2.h
NiftiEnums.h
TabDrawingInfo.h
VolumeBase.h
VolumeMappableInterface.h
VolumeSliceViewPlaneEnum.h
VolumeSpace.h
VolumeTextureCoordinateMapper.h
VolumeToImageMappingModeEnum.h

GiftiLabel.cxx
GiftiLabelTable.cxx
LabelViewModeEnum.cxx
NiftiEnums.cxx
TabDrawingInfo.cxx
VolumeBase.cxx
VolumeMappableInterface.cxx
VolumeSliceViewPlaneEnum.cxx
VolumeSpace.cxx
VolumeTextureCoordinateMapper.cxx
VolumeToImageMappingModeEnum.cxx
)

TARGET_LINK_LIBRARIES(FilesBase ${CARET_QT5_LINK})

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Xml
)
