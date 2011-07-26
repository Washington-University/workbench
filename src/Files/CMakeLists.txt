#
# Name of project
#
PROJECT (Files)

#
# Files Library
#
ADD_LIBRARY(Files
GiftiTypeFile.h
SurfaceFile.h

GiftiTypeFile.cxx
SurfaceFile.cxx
)


#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
