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
OperationCiftiChangeTimestep.h
OperationCiftiConvert.h
OperationCiftiROIAverage.h
OperationException.h
OperationMetricMask.h
OperationMetricPalette.h
OperationSetStructure.h
OperationVolumeLabelImport.h
OperationVolumePalette.h

OperationAddToSpecFile.cxx
OperationCiftiChangeTimestep.cxx
OperationCiftiConvert.cxx
OperationCiftiROIAverage.cxx
OperationException.cxx
OperationMetricMask.cxx
OperationMetricPalette.cxx
OperationSetStructure.cxx
OperationVolumeLabelImport.cxx
OperationVolumePalette.cxx
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
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
