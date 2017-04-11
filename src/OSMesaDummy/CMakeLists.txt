
#
# Name of Project
#
PROJECT(OSMesaDummy)

#
# Need Qt for reading from resource file.
#
IF (QT4_FOUND)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

#
# Create a library
#
ADD_LIBRARY(OSMesaDummy
OSMesaDummy.h
OSMesaDummy.c
)

TARGET_LINK_LIBRARIES(OSMesaDummy ${CARET_QT5_LINK})

#
# Include directories
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Graphics
${CMAKE_SOURCE_DIR}/OSMesaDummy
)
