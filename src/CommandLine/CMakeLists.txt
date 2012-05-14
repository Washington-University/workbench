
#
# Name of project
#
PROJECT (CommandLine)

#cmake_policy(SET CMP0015 OLD) 

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Name of executable
#
set (EXE_NAME wb_command)

#
# Create the executable
# Apple creates a bundle
#
IF (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      MACOSX_BUNDLE
      wb_command.cxx
   )
ELSE (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      wb_command.cxx
   )
ENDIF (APPLE)


#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(${EXE_NAME}
Commands
Operations
Algorithms
OperationsBase
Brain
Files
Palette
Gifti
Cifti
Nifti
FilesBase
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
${CMAKE_SOURCE_DIR}/Commands
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

