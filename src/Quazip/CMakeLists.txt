#
# Name of project
#
PROJECT(Quazip)

#
# QT include files
#
SET(QT_DONT_USE_QTGUI TRUE)

IF(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
ENDIF()
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

SET(CMAKE_AUTOMOC ON)

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

SET(QUAZIP_LIBRARIES Quazip PARENT_SCOPE)
SET(QUAZIP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
