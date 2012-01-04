#
# Name of project
#
PROJECT (Files)

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
# Files Library
#
ADD_LIBRARY(Files
BinaryFileReader.h
CaretDataFile.h
CaretMappableDataFile.h
ConnectivityLoaderFile.h
DataFileTypeEnum.h
GeodesicHelper.h
GiftiTypeFile.h
ImageFile.h
LabelFile.h
MetricFile.h
PaletteFile.h
RgbaFile.h
SpecFile.h
SpecFileDataFileTypeGroup.h
SpecFileDataFile.h
SpecFileSaxReader.h
SurfaceFile.h
SurfaceProjectionTypeEnum.h
SurfaceTypeEnum.h
TextFile.h
TopologyHelper.h
VolumeFile.h

BinaryFileReader.cxx
DataFileTypeEnum.cxx
CaretDataFile.cxx
CaretMappableDataFile.cxx
ConnectivityLoaderFile.cxx
GeodesicHelper.cxx
GiftiTypeFile.cxx
ImageFile.cxx
LabelFile.cxx
MetricFile.cxx
PaletteFile.cxx
RgbaFile.cxx
SpecFile.cxx
SpecFileDataFileTypeGroup.cxx
SpecFileDataFile.cxx
SpecFileSaxReader.cxx
SurfaceFile.cxx
SurfaceProjectionTypeEnum.cxx
SurfaceTypeEnum.cxx
TextFile.cxx
TopologyHelper.cxx
VolumeFile.cxx
)


#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
