#
# Name of project
#
PROJECT(Quazip)

IF(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Core5Compat_INCLUDE_DIRS})
ENDIF()
IF(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
ENDIF()

#
# With AUTOMOC, do not need to specify files
# that contain Q_OBJECT macro for Qt to process with 'moc'
# (meta-object compiler).
#
IF(WORKBENCH_USE_CMAKE_AUTOMOC)
    SET(CMAKE_AUTOMOC ON)
ELSE()
    SET(MOC_INPUT_HEADER_FILES
        quazipfile.h
    )

    IF(Qt5_FOUND)
        QT5_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
    IF(Qt6_FOUND)
        QT6_WRAP_CPP(MOC_SOURCE_FILES ${MOC_INPUT_HEADER_FILES})
    ENDIF()
ENDIF()

#
# Prevents dll linkage errors on windows
#
#IF(WIN32)
#   ADD_DEFINITIONS(-DQUAZIP_STATIC)
#   IF(MSVC)
#      ADD_DEFINITIONS(-DQUAZIP_STATIC)
#      #ADD_DEFINITIONS(-DQUAZIP_BUILD)
#   ENDIF(MSVC)
#ELSE(WIN32)
#    ADD_DEFINITIONS(-DQUAZIP_STATIC)
#ENDIF(WIN32)

#
# Create a library
#
ADD_LIBRARY(Quazip
JlCompress.h
crypt.h
ioapi.h
quaadler32.h
quachecksum32.h
quacrc32.h
quagzipfile.h
quaziodevice.h
quazip.h
quazip_global.h
quazipdir.h
quazipfile.h
quazipfileinfo.h
quazipnewinfo.h
unzip.h
zip.h

${MOC_SOURCE_FILES}

JlCompress.cpp
qioapi.cpp
quaadler32.cpp
quacrc32.cpp
quagzipfile.cpp
quaziodevice.cpp
quazip.cpp
quazipdir.cpp
quazipfile.cpp
quazipfileinfo.cpp
quazipnewinfo.cpp
unzip.c
zip.c
)

TARGET_LINK_LIBRARIES(Quazip ${CARET_QT5_LINK})

SET(QUAZIP_LIBRARIES Quazip PARENT_SCOPE)
SET(QUAZIP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
