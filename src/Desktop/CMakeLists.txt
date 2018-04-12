#
# Name of Project
#
PROJECT(Desktop)

#
# Name of executable
#
set (EXE_NAME wb_view)

#
# QT Include directories
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
    if(Qt5OpenGL_FOUND)
        include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    endif()
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    SET(QT_USE_QTXML TRUE)
    SET(QT_USE_QTOPENGL TRUE)
    SET(QT_USE_QTNETWORK TRUE)
    INCLUDE(${QT_USE_FILE})
ENDIF ()


#
# Added by JWH to eliminate OpenGL linking errors in Ubuntu Linux
#
IF (UNIX)
    IF (NOT APPLE)
        FIND_PACKAGE(OpenGL)
    ENDIF (NOT APPLE)
ENDIF (UNIX)

#
# Resources
#
SET (RESOURCES_QRC_FILE
     ../Resources/General/general_resources.qrc
     ../Resources/Gui/gui_resources.qrc
     ../Resources/Help/help_resources.qrc)
IF (Qt5_FOUND)
    QT5_ADD_RESOURCES(IMAGE_RCS_SRCS ${RESOURCES_QRC_FILE})
ELSE (Qt5_FOUND)
    IF (QT4_FOUND)
        QT4_ADD_RESOURCES(IMAGE_RCS_SRCS ${RESOURCES_QRC_FILE})
    ENDIF()
ENDIF (Qt5_FOUND)

#
# Create the executable
# Apple creates a bundle
#
IF (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      MACOSX_BUNDLE
      desktop.cxx
      ${IMAGE_RCS_SRCS}
   )

    #
    # This is a customized Info.Plist for Mac so that a spec
    # file can be opened using Finder
    #
    SET_TARGET_PROPERTIES(
	${EXE_NAME}
	PROPERTIES
	MACOSX_BUNDLE_INFO_PLIST 
	${CMAKE_SOURCE_DIR}/../mac_plist/MacOSXBundleInfo.plist.in
    )
	
#   SET_TARGET_PROPERTIES(
#       ${EXE_NAME}
#       PROPERTIES
#       RESOURCE
#       ${QT_BINARY_DIR}/../src/gui/mac/qt_menu.nib
#   )
ENDIF (APPLE)

IF (WIN32)
   ADD_EXECUTABLE(${EXE_NAME}
      desktop.cxx
      ${IMAGE_RCS_SRCS}
      ${CMAKE_SOURCE_DIR}/../icons/windows/workbench.rc
   )
ENDIF (WIN32)
IF (NOT APPLE)
    IF (UNIX)
       ADD_EXECUTABLE(${EXE_NAME}
          desktop.cxx
          ${IMAGE_RCS_SRCS}
       )
    ENDIF (UNIX)
ENDIF (NOT APPLE)

if(Qt5OpenGL_FOUND)
    set(QT5_OPENGL_LIB_NAME Qt5::OpenGL)
endif()
if(Qt5_FOUND)
    set(QT5_LINK_LIBS
        Qt5::Concurrent
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        ${QT5_OPENGL_LIB_NAME}
        Qt5::PrintSupport
        Qt5::Test
        Qt5::Widgets
        Qt5::Xml)
endif()

#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(${EXE_NAME}
GuiQt
Commands
Operations
Algorithms
OperationsBase
${Qwt_LIBRARIES}
OSMesaDummy
Brain
Files
Graphics
${FTGL_LIBRARIES}
Annotations
Charting
Palette
Cifti
Gifti
Nifti
FilesBase
Scenes
Xml
Common
${GLEW_LIBRARIES}
${QUAZIP_LIBRARIES}
${FREETYPE_LIBRARIES}
${QT_LIBRARIES}
${QT5_LINK_LIBS}
${ZLIB_LIBRARIES}
#${LIBS}
)

INCLUDE(GNUInstallDirs)

INSTALL(TARGETS ${EXE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

IF(WIN32)
    TARGET_LINK_LIBRARIES(${EXE_NAME}
    opengl32
    glu32
    )
ENDIF(WIN32)

IF (UNIX)
   IF (NOT APPLE) 
      TARGET_LINK_LIBRARIES(${EXE_NAME}
         ${OPENGL_LIBRARIES}
         gobject-2.0
      )
   ENDIF (NOT APPLE)

#   EXECUTE_PROCESS(COMMAND uname -n OUTPUT_VARIABLE MACHINE_NAME)
#   MESSAGE("MACHINE_NAME: ${MACHINE_NAME}")
#   IF (${MACHINE_NAME} MATCHES "linuxbuild")
#       MESSAGE("is linuxbuild")
#       SET_TARGET_PROPERTIES(${EXE_NAME}
#                          PROPERTIES
#                          LINK_FLAGS "-Wl,-E"
#                          LINK_FLAGS_DEBUG "-Wl,-E"
#                          LINK_FLAGS_RELEASE "-Wl,-E")
#   ENDIF()
ENDIF (UNIX)

#
# At this time, Cocoa needs to be explicitly added for Apple Mac
#
IF (APPLE)
   #SET (QT_MAC_USE_COCOA TRUE)
   TARGET_LINK_LIBRARIES(${EXE_NAME}
     "-framework Cocoa"
     "-framework OpenGL"
   )
ENDIF (APPLE)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Desktop
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Commands
${CMAKE_SOURCE_DIR}/GuiQt
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Graphics
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${Qwt_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

#
# Apple needs qt_menu.nib directory copied
# into application's Resources directory 
#
# Apple needs framework
#
IF (APPLE)
        ADD_CUSTOM_COMMAND(
            TARGET ${EXE_NAME}
            POST_BUILD
            COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_nib.sh ${EXE_NAME}
            ###COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_frameworks.sh ${EXE_NAME}
        )
ENDIF (APPLE)

IF (APPLE)
    SET (MACOSX_BUNDLE_INFO_STRING wb_view Copyright 2015 )
    SET (MACOSX_BUNDLE_ICON_FILE workbench.icns )
    ##SET (MACOSX_BUNDLE_GUI_IDENTIFIER wb_view )
    ## Underscore is not valid in MACOSX_BUNDLE_GUI_IDENTIFIER
    SET (MACOSX_BUNDLE_GUI_IDENTIFIER workbench )
    SET (MACOSX_BUNDLE_LONG_VERSION_STRING wb_view)
    SET (MACOSX_BUNDLE_BUNDLE_NAME wb_view)
    SET (MACOSX_BUNDLE_SHORT_VERSION_STRING 1.1.1)
    SET (MACOSX_BUNDLE_BUNDLE_VERSION 1.1.1)
    SET (MACOSX_BUNDLE_COPYRIGHT  2015 )

    ADD_CUSTOM_COMMAND(
        TARGET ${EXE_NAME}
        POST_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_icon.sh ${EXE_NAME} ${CMAKE_SOURCE_DIR}/../icons/mac/wb_view.icns
        COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_icon.sh ${EXE_NAME} ${CMAKE_SOURCE_DIR}/../icons/mac/workbench.icns
        COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_icon.sh ${EXE_NAME} ${CMAKE_SOURCE_DIR}/../icons/mac/spec_file.icns
        COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_icon.sh ${EXE_NAME} ${CMAKE_SOURCE_DIR}/../icons/mac/data_file.icns
    )
ENDIF (APPLE)
