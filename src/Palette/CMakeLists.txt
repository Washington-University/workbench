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
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()


#
# Create the NIFTI library
#
ADD_LIBRARY(Palette
Palette.h
PaletteColorBarValuesModeEnum.h
PaletteColorMapping.h
PaletteColorMappingSaxReader.h
PaletteColorMappingXmlElements.h
PaletteEnums.h
PaletteNormalizationModeEnum.h
PaletteScalarAndColor.h
PaletteThresholdRangeModeEnum.h

Palette.cxx
PaletteColorBarValuesModeEnum.cxx
PaletteColorMapping.cxx
PaletteColorMappingSaxReader.cxx
PaletteEnums.cxx
PaletteNormalizationModeEnum.cxx
PaletteScalarAndColor.cxx
PaletteThresholdRangeModeEnum.cxx
)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
)
