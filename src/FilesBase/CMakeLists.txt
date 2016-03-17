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
INCLUDE (${QT_USE_FILE})


#
# Create the NIFTI library
#
ADD_LIBRARY(FilesBase
GiftiException.h
GiftiLabel.h
GiftiLabelTable.h
GiftiMetaData.h
GiftiMetaDataXmlElements.h
GiftiXmlElements.h
nifti1.h
nifti2.h
NiftiEnums.h
VolumeBase.h
VolumeMappableInterface.h
VolumeSliceViewPlaneEnum.h
VolumeSpace.h

GiftiException.cxx
GiftiLabel.cxx
GiftiLabelTable.cxx
GiftiMetaData.cxx
GiftiXmlElements.cxx
NiftiEnums.cxx
VolumeBase.cxx
VolumeMappableInterface.cxx
VolumeSliceViewPlaneEnum.cxx
VolumeSpace.cxx
)

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
