
#
# Name of project
#
PROJECT (CommandLine)

#cmake_policy(SET CMP0015 OLD) 

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)
SET(QT_USE_QTXML TRUE)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create the executable
#
ADD_EXECUTABLE(caret_command
caret_command.cxx
)


#LINK_DIRECTORIES(
#${CMAKE_BIN_DIR}/Brain
#${CMAKE_BIN_DIR}/Files
#${CMAKE_BIN_DIR}/Gifti
#${CMAKE_BIN_DIR}/Nifti
#${CMAKE_BIN_DIR}/Xml
#${CMAKE_BIN_DIR}/Common
#)

#set(LIBS ${LIBS} 
#/Users/john/caret7_development/xcode/caret7_source/brain/libBrain.a
#/Users/john/caret7_development/xcode/caret7_source/files/libfiles.a
#/Users/john/caret7_development/xcode/caret7_source/gifti/libGifti.a
#/Users/john/caret7_development/xcode/caret7_source/nifti/libNifti.a
#/Users/john/caret7_development/xcode/caret7_source/xml/libxml.a
#/Users/john/caret7_development/xcode/caret7_source/common/libCommon.a
#"-framework Cocoa")
#ELSE()
#link_directories(
#../brain
#../files
#../gifti
#../nifti
#../xml
#../common)

#set(LIBS ${LIBS} 
#Brain
#files
#Gifti
#Nifti
#xml
#Common)
#ENDIF()

#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(caret_command 
Brain
Files
Gifti
Nifti
Xml
Common
${QT_LIBRARIES}
${ZLIB_LIBRARIES}
${LIBS})

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/CommandLine
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
