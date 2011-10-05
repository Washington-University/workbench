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
PaletteColorMapping.h
PaletteColorMappingSaxReader.h
PaletteEnums.h

PaletteColorMapping.cxx
PaletteColorMappingSaxReader.cxx
PaletteEnums.cxx

)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
)
