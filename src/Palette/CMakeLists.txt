#
# The NIFTI Project
#
project (Palette)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})


#
# Create the NIFTI library
#
ADD_LIBRARY(Palette
Palette.h
PaletteColorMapping.h
PaletteColorMappingSaxReader.h
PaletteEnums.h
PaletteScalarAndColor.h

Palette.cxx
PaletteColorMapping.cxx
PaletteColorMappingSaxReader.cxx
PaletteEnums.cxx
PaletteScalarAndColor.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Xml
)
