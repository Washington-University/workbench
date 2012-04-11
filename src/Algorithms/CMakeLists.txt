#
# Name of project
#
PROJECT (Algorithms)

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
ADD_LIBRARY(Algorithms
AbstractAlgorithm.h
AlgorithmCiftiCorrelation.h
AlgorithmCiftiCreateDenseTimeseries.h
AlgorithmCiftiGradient.h
AlgorithmCiftiReplaceStructure.h
AlgorithmCiftiSeparate.h
AlgorithmCiftiSmoothing.h
AlgorithmCreateSignedDistanceVolume.h
AlgorithmException.h
AlgorithmLabelDilate.h
AlgorithmMetricDilate.h
AlgorithmMetricGradient.h
AlgorithmMetricSmoothing.h
AlgorithmNodesInsideBorder.h
AlgorithmVolumeGradient.h
AlgorithmVolumeParcelResampling.h
AlgorithmVolumeParcelResamplingGeneric.h
AlgorithmVolumeParcelSmoothing.h
AlgorithmVolumeSmoothing.h
AlgorithmVolumeToSurfaceMapping.h

AbstractAlgorithm.cxx
AlgorithmCiftiCorrelation.cxx
AlgorithmCiftiCreateDenseTimeseries.cxx
AlgorithmCiftiGradient.cxx
AlgorithmCiftiReplaceStructure.cxx
AlgorithmCiftiSeparate.cxx
AlgorithmCiftiSmoothing.cxx
AlgorithmCreateSignedDistanceVolume.cxx
AlgorithmException.cxx
AlgorithmLabelDilate.cxx
AlgorithmMetricDilate.cxx
AlgorithmMetricGradient.cxx
AlgorithmMetricSmoothing.cxx
AlgorithmNodesInsideBorder.cxx
AlgorithmVolumeGradient.cxx
AlgorithmVolumeParcelResampling.cxx
AlgorithmVolumeParcelResamplingGeneric.cxx
AlgorithmVolumeParcelSmoothing.cxx
AlgorithmVolumeSmoothing.cxx
AlgorithmVolumeToSurfaceMapping.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
