
#
# Name of Project
#
PROJECT(OmeZarr)


#
# QT include files
#
#SET(QT_DONT_USE_QTGUI TRUE)
#SET(QT_USE_QTNETWORK TRUE)

if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Gui_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
endif()
#message("QT VERSION: " ${QT_VERSION_MAJOR} ${QT_VERSION_MINOR} ${QT_VERSION_PATCH})

#
# With AUTOMOC, do not need to specify files
# that contain Q_OBJECT macro for Qt to process with 'moc'
# (meta-object compiler).
#
IF(WORKBENCH_USE_CMAKE_AUTOMOC)
    SET(CMAKE_AUTOMOC ON)
ELSE()
    SET(MOC_INPUT_HEADER_FILES
    )

    IF(Qt6_FOUND)
        QT6_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
    IF(Qt5_FOUND)
        QT5_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
ENDIF()

#
# Create a library
#
ADD_LIBRARY(OmeZarr
OmeAttrsV0p4JsonFile.h
OmeAxis.h
OmeAxisTypeEnum.h
OmeCoordinateTransformationTypeEnum.h
OmeCoordinateTransformations.h
OmeDataSet.h
OmeDimensionIndices.h
OmeDimensionSizes.h
OmeFileReader.h
OmeImage.h
OmeSpaceUnitEnum.h
OmeTimeUnitEnum.h
OmeVersionEnum.h
ZarrCompressorTypeEnum.h
ZarrDataTypeByteOrderEnum.h
ZarrDataTypeEnum.h
ZarrDimensionSeparatorEnum.h
ZarrDriverTypeEnum.h
ZarrHelper.h
ZarrImageReader.h
ZarrJsonFileBase.h
ZarrRowColumnMajorOrderTypeEnum.h
ZarrV2ArrayJsonFile.h
ZarrV2GroupJsonFile.h

OmeAttrsV0p4JsonFile.cxx
OmeAxis.cxx
OmeAxisTypeEnum.cxx
OmeCoordinateTransformationTypeEnum.cxx
OmeCoordinateTransformations.cxx
OmeDataSet.cxx
OmeDimensionIndices.cxx
OmeDimensionSizes.cxx
OmeFileReader.cxx
OmeImage.cxx
OmeSpaceUnitEnum.cxx
OmeTimeUnitEnum.cxx
OmeVersionEnum.cxx
ZarrCompressorTypeEnum.cxx
ZarrDataTypeByteOrderEnum.cxx
ZarrDataTypeEnum.cxx
ZarrDimensionSeparatorEnum.cxx
ZarrDriverTypeEnum.cxx
ZarrHelper.cxx
ZarrImageReader.cxx
ZarrJsonFileBase.cxx
ZarrRowColumnMajorOrderTypeEnum.cxx
ZarrV2ArrayJsonFile.cxx
ZarrV2GroupJsonFile.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/OmeZarr
)
