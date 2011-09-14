#
# Name of project
#
PROJECT (Helpers)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create the brain library
#
ADD_LIBRARY(Helpers
GeodesicHelper.h
TopologyHelper.h

GeodesicHelper.cxx
TopologyHelper.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Helpers
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
