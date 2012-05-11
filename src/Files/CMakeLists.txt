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
Border.h
BorderException.h
BorderFile.h
BorderFileSaxReader.h
CaretDataFile.h
CaretMappableDataFile.h
CaretVolumeExtension.h
ClassAndNameHierarchyModel.h
ConnectivityLoaderFile.h
DataFileTypeEnum.h
DisplayGroupEnum.h
EventSurfaceColoringInvalidate.h
FociFile.h
FociFileSaxReader.h
Focus.h
GeodesicHelper.h
GiftiTypeFile.h
ImageFile.h
LabelFile.h
MetricFile.h
MetricSmoothingObject.h
PaletteFile.h
RgbaFile.h
SpecFile.h
SpecFileDataFileTypeGroup.h
SpecFileDataFile.h
SpecFileSaxReader.h
StudyMetaDataLink.h
StudyMetaDataLinkSet.h
StudyMetaDataLinkSetSaxReader.h
SurfaceFile.h
SurfaceProjectedItem.h
SurfaceProjectedItemSaxReader.h
SurfaceProjection.h
SurfaceProjectionBarycentric.h
SurfaceProjectionVanEssen.h
SurfaceTypeEnum.h
TimeLine.h
TextFile.h
TopologyHelper.h
VolumeFile.h

BinaryFileReader.cxx
Border.cxx
BorderException.cxx
BorderFile.cxx
BorderFileSaxReader.cxx
DataFileTypeEnum.cxx
CaretDataFile.cxx
CaretMappableDataFile.cxx
CaretVolumeExtension.cxx
ClassAndNameHierarchyModel.cxx
ConnectivityLoaderFile.cxx
DisplayGroupEnum.cxx
EventSurfaceColoringInvalidate.cxx
FociFile.cxx
FociFileSaxReader.cxx
Focus.cxx
GeodesicHelper.cxx
GiftiTypeFile.cxx
ImageFile.cxx
LabelFile.cxx
MetricFile.cxx
MetricSmoothingObject.cxx
PaletteFile.cxx
RgbaFile.cxx
SpecFile.cxx
SpecFileDataFileTypeGroup.cxx
SpecFileDataFile.cxx
SpecFileSaxReader.cxx
StudyMetaDataLink.cxx
StudyMetaDataLinkSet.cxx
StudyMetaDataLinkSetSaxReader.cxx
SurfaceFile.cxx
SurfaceProjectedItem.cxx
SurfaceProjectedItemSaxReader.cxx
SurfaceProjection.cxx
SurfaceProjectionBarycentric.cxx
SurfaceProjectionVanEssen.cxx
SurfaceTypeEnum.cxx
TextFile.cxx
TimeLine.cxx
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
