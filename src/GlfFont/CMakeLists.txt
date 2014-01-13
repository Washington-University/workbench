
#
# Name of Project
#
PROJECT(GlfFont)

#
# Create a library
#
ADD_LIBRARY(GlfFont
glf.h

glf.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/GlfFont
)
