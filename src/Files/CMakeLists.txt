#
# Name of project
#
PROJECT (Files)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Files Library
#
ADD_LIBRARY(Files
BinaryFileReader.h
DataFileTypeEnum.h
GiftiTypeFile.h
LabelFile.h
MetricFile.h
Palette.h
PaletteFile.h
PaletteScalarAndColor.h
RgbaFile.h
SpecFile.h
SpecFileDataFileTypeGroup.h
SpecFileDataFile.h
SpecFileSaxReader.h
SurfaceFile.h
SurfaceTypeEnum.h
TextFile.h
VolumeFile.h
VolumeTypeEnum.h

BinaryFileReader.cxx
DataFileTypeEnum.cxx
GiftiTypeFile.cxx
LabelFile.cxx
MetricFile.cxx
Palette.cxx
PaletteFile.cxx
PaletteScalarAndColor.cxx
RgbaFile.cxx
SpecFile.cxx
SpecFileDataFileTypeGroup.cxx
SpecFileDataFile.cxx
SpecFileSaxReader.cxx
SurfaceFile.cxx
SurfaceTypeEnum.cxx
TextFile.cxx
VolumeFile.cxx
VolumeTypeEnum.cxx
)


#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
