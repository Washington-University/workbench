
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
#    include_directories(${Qt6Network_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
#    include_directories(${Qt5Network_INCLUDE_DIRS})
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
OmeAxis.h
OmeAxisTypeEnum.h
OmeDataSet.h
OmeSpaceUnitEnum.h
OmeTimeUnitEnum.h
OmeVersionEnum.h
OmeZarrReader.h
ZarrDataTypeEnum.h
ZarrImageReader.h

OmeAxis.cxx
OmeAxisTypeEnum.cxx
OmeDataSet.cxx
OmeSpaceUnitEnum.cxx
OmeTimeUnitEnum.cxx
OmeVersionEnum.cxx
OmeZarrReader.cxx
ZarrDataTypeEnum.cxx
ZarrImageReader.cxx
)

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Common
${CMAKE_SOURCE_DIR}/OmeZarr
)

