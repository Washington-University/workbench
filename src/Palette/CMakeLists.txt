#
# The NIFTI Project
#
project (Palette)

#
# Add QT for includes
#
if(Qt5_FOUND)
    SET(QT_DONT_USE_QTGUI)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()
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
PaletteHistogramRangeModeEnum.h
PaletteInvertModeEnum.h
PaletteModifiedStatusEnum.h
PaletteNormalizationModeEnum.h
PaletteScalarAndColor.h
PaletteThresholdRangeModeEnum.h

Palette.cxx
PaletteColorBarValuesModeEnum.cxx
PaletteColorMapping.cxx
PaletteColorMappingSaxReader.cxx
PaletteEnums.cxx
PaletteHistogramRangeModeEnum.cxx
PaletteInvertModeEnum.cxx
PaletteModifiedStatusEnum.cxx
PaletteNormalizationModeEnum.cxx
PaletteScalarAndColor.cxx
PaletteThresholdRangeModeEnum.cxx
)

TARGET_LINK_LIBRARIES(Palette ${CARET_QT5_LINK})

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
