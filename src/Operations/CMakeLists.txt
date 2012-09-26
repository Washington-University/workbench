#
# Name of project
#
PROJECT (Operations)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create the helper library
#
ADD_LIBRARY(Operations
OperationAddToSpecFile.h
OperationBackendAverageDenseROI.h
OperationBackendAverageROICorrelation.h
OperationCiftiChangeTimestep.h
OperationCiftiConvert.h
OperationCiftiConvertToScalar.h
OperationCiftiROIAverage.h
OperationCiftiSeparateAll.h
OperationEstimateFiberBinghams.h
OperationException.h
OperationMetricMask.h
OperationMetricPalette.h
OperationSetStructure.h
OperationSurfaceClosestNode.h
OperationSurfaceGeodesicROIs.h
OperationVolumeLabelImport.h
OperationVolumePalette.h
OperationZipSpecFile.h

OperationAddToSpecFile.cxx
OperationBackendAverageDenseROI.cxx
OperationBackendAverageROICorrelation.cxx
OperationCiftiChangeTimestep.cxx
OperationCiftiConvert.cxx
OperationCiftiConvertToScalar.cxx
OperationCiftiROIAverage.cxx
OperationCiftiSeparateAll.cxx
OperationException.cxx
OperationEstimateFiberBinghams.cxx
OperationMetricMask.cxx
OperationMetricPalette.cxx
OperationSetStructure.cxx
OperationSurfaceClosestNode.cxx
OperationSurfaceGeodesicROIs.cxx
OperationVolumeLabelImport.cxx
OperationVolumePalette.cxx
OperationZipSpecFile.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Quazip
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
