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
AlgorithmCreateSignedDistanceVolume.h
AlgorithmException.h
AlgorithmMetricDilate.h
AlgorithmMetricGradient.h
AlgorithmMetricSmoothing.h
AlgorithmVolumeToSurfaceMapping.h

AbstractAlgorithm.cxx
AlgorithmCreateSignedDistanceVolume.cxx
AlgorithmException.cxx
AlgorithmMetricDilate.cxx
AlgorithmMetricGradient.cxx
AlgorithmMetricSmoothing.cxx
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
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
