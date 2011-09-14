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
BinaryFile.h
DataFileTypeEnum.h
GiftiTypeFile.h
Palette.h
PaletteFile.h
PaletteScalarAndColor.h
SurfaceFile.h
SurfaceTypeEnum.h
TextFile.h

BinaryFile.cxx
DataFileTypeEnum.cxx
GiftiTypeFile.cxx
Palette.cxx
PaletteFile.cxx
PaletteScalarAndColor.cxx
SurfaceFile.cxx
SurfaceTypeEnum.cxx
TextFile.cxx
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
