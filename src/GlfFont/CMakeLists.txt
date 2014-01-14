
#
# Name of Project
#
PROJECT(GlfFont)

#
# Need Qt for reading from resource file.
#
INCLUDE(${QT_USE_FILE})

#
# Create a library
#
ADD_LIBRARY(GlfFont
GlfQtFontReader.h
glf.h

GlfQtFontReader.cxx
glf.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/GlfFont
)
