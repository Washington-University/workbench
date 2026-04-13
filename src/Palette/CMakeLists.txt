#
# The NIFTI Project
#
project (Palette)

#
# Add QT for includes
#
if(Qt6_FOUND)
    SET(QT_DONT_USE_QTGUI)
    include_directories(${Qt6Core_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    SET(QT_DONT_USE_QTGUI)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()

#
# Create the NIFTI library
#
ADD_LIBRARY(Palette
EventPaletteOperation.h
Palette.h
PaletteColorBarValuesModeEnum.h
PaletteColorMapping.h
PaletteColorMappingSaxReader.h
PaletteColorMappingXmlElements.h
PaletteEnums.h
PaletteHistogramRangeModeEnum.h
PaletteInvertModeEnum.h
PaletteModifiedStatusEnum.h
PaletteNew.h
PaletteNewGroup.h
PaletteNormalizationModeEnum.h
PaletteScalarAndColor.h
PaletteThresholdOutlineDrawingModeEnum.h
PaletteThresholdRangeModeEnum.h

EventPaletteOperation.cxx
Palette.cxx
PaletteColorBarValuesModeEnum.cxx
PaletteColorMapping.cxx
PaletteColorMappingSaxReader.cxx
PaletteEnums.cxx
PaletteHistogramRangeModeEnum.cxx
PaletteInvertModeEnum.cxx
PaletteNew.cxx
PaletteNewGroup.cxx
PaletteModifiedStatusEnum.cxx
PaletteNormalizationModeEnum.cxx
PaletteScalarAndColor.cxx
PaletteThresholdOutlineDrawingModeEnum.cxx
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
