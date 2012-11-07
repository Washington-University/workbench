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
OperationCiftiMath.h
OperationCiftiPalette.h
OperationCiftiROIAverage.h
OperationCiftiSeparateAll.h
OperationConvertMatrix4ToWorkbenchSparse.h
OperationEstimateFiberBinghams.h
OperationException.h
OperationFociListCoords.h
OperationLabelMask.h
OperationMetricMask.h
OperationMetricMath.h
OperationMetricPalette.h
OperationProbtrackXDotConvert.h
OperationSetStructure.h
OperationSurfaceClosestVertex.h
OperationSurfaceGeodesicROIs.h
OperationSurfaceInformation.h
OperationVolumeLabelImport.h
OperationVolumeMath.h
OperationVolumePalette.h
OperationZipSpecFile.h

OperationAddToSpecFile.cxx
OperationBackendAverageDenseROI.cxx
OperationBackendAverageROICorrelation.cxx
OperationCiftiChangeTimestep.cxx
OperationCiftiConvert.cxx
OperationCiftiConvertToScalar.cxx
OperationCiftiMath.cxx
OperationCiftiPalette.cxx
OperationCiftiROIAverage.cxx
OperationCiftiSeparateAll.cxx
OperationConvertMatrix4ToWorkbenchSparse.cxx
OperationException.cxx
OperationEstimateFiberBinghams.cxx
OperationFociListCoords.cxx
OperationLabelMask.cxx
OperationMetricMask.cxx
OperationMetricMath.cxx
OperationMetricPalette.cxx
OperationProbtrackXDotConvert.cxx
OperationSetStructure.cxx
OperationSurfaceClosestVertex.cxx
OperationSurfaceGeodesicROIs.cxx
OperationSurfaceInformation.cxx
OperationVolumeLabelImport.cxx
OperationVolumeMath.cxx
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
