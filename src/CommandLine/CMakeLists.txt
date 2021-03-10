
#
# Name of project
#
PROJECT (CommandLine)

#cmake_policy(SET CMP0015 OLD) 


#
# Add QT for includes
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
#    include_directories(${Qt5Network_INCLUDE_DIRS})
endif()

#
# Name of executable
#
set (EXE_NAME wb_command)

#
# Resources
#
SET (RESOURCES_QRC_FILE ../Resources/General/general_resources.qrc)
IF (Qt5_FOUND)
    QT5_ADD_RESOURCES(IMAGE_RCS_SRCS ${RESOURCES_QRC_FILE})
ELSE (Qt5_FOUND)
ENDIF (Qt5_FOUND)

#
# Create the executable
# Apple creates a bundle
#
IF (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      MACOSX_BUNDLE
      wb_command.cxx
      ${IMAGE_RCS_SRCS}
   )
ELSE (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      wb_command.cxx
      ${IMAGE_RCS_SRCS}
   )
ENDIF (APPLE)


if(Qt5_FOUND)
    set(QT5_LINK_LIBS
        Qt5::Concurrent
        Qt5::Core
        Qt5::Gui
        Qt5::Network
#        Qt5::OpenGL
#        Qt5::PrintSupport
        Qt5::Test
#        Qt5::Widgets
        Qt5::Xml)
endif()

SET (MESA_OR_OPENGL_LIBRARIES "")
IF (OSMESA_FOUND)
    SET(MESA_OR_OPENGL_LIBRARIES
        ${OSMESA_OFFSCREEN_LIBRARY}
        ${OSMESA_GL_LIBRARY}
        ${OSMESA_GLU_LIBRARY})
ELSE()
    IF (OPENGL_FOUND)
        SET(MESA_OR_OPENGL_LIBRARIES ${OPENGL_LIBRARIES})
    ELSE()
        MESSAGE(SEND_ERROR "Must have either OpenGL or Mesa3D for command line")
    ENDIF()
ENDIF()

#
# Libraries that are linked
# Note: GLEW_LIBRARIES and OSMESA library variables will be empty and
# have no effect if not available
#
TARGET_LINK_LIBRARIES(${EXE_NAME}
Commands
Operations
Algorithms
OperationsBase
Brain
Files
Graphics
${FTGL_LIBRARIES}
Annotations
Palette
Gifti
Cifti
Nifti
Charting
QxtCore
FilesBase
Scenes
Xml
Common
${QUAZIP_LIBRARIES}
${FREETYPE_LIBRARIES}
${QT_LIBRARIES}
${QT5_LINK_LIBS}
${GLEW_LIBRARIES}
${MESA_OR_OPENGL_LIBRARIES}
${ZLIB_LIBRARIES}
${LIBS})

INCLUDE(GNUInstallDirs)

INSTALL(TARGETS ${EXE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(PROGRAMS wb_shortcuts DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES bashcomplete_wb_command DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bash-completion/completions RENAME wb_command)
INSTALL(FILES bashcomplete_wb_shortcuts DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bash-completion/completions RENAME wb_shortcuts)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/CommandLine
${CMAKE_SOURCE_DIR}/Commands
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
${QUAZIP_INCLUDE_DIRS}
)

ENABLE_TESTING()

ADD_TEST(command_help ${EXE_NAME} -all-commands-help)
