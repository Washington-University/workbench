#
# Name of Project
#
PROJECT(Desktop)

#
# Name of executable
#
set (EXE_NAME workbench)

#
#  QT Libraries
#
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)
SET(QT_USE_QTNETWORK TRUE)
#SET(QT_USE_MNG TRUE)

#
# QT Include directories
#
INCLUDE (${QT_USE_FILE})

#
# Added by JWH to eliminate OpenGL linking errors in Ubuntu Linux
#
IF (UNIX)
    IF (NOT APPLE)
        FIND_PACKAGE(OpenGL)
    ENDIF (NOT APPLE)
ENDIF (UNIX)
#
# Images
#
SET (IMAGE_RCS ./images/images.qrc)
QT4_ADD_RESOURCES(IMAGE_RCS_SRCS ${IMAGE_RCS})

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

#   SET_TARGET_PROPERTIES(
#       ${EXE_NAME}
#       PROPERTIES
#       RESOURCE
#       ${QT_BINARY_DIR}/../src/gui/mac/qt_menu.nib
#   )
ELSE (APPLE)
   ADD_EXECUTABLE(${EXE_NAME}
      desktop.cxx
      ${IMAGE_RCS_SRCS}
   )
ENDIF (APPLE)

#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(${EXE_NAME}
Algorithms
GuiQt
Qwt
Brain
Files
Palette
Cifti
Gifti
Nifti
FilesBase
Xml
Common
${QT_LIBRARIES}
${ZLIB_LIBRARIES}
#${LIBS}
)

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
${CMAKE_SOURCE_DIR}/GuiQt
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
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
    MESSAGE("EXE: " ${CMAKE_BUILD_DIR} " " ${EXE_NAME})
        ADD_CUSTOM_COMMAND(
            TARGET ${EXE_NAME}
            POST_BUILD
            COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_nib.sh ${EXE_NAME}
            ###COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_frameworks.sh ${EXE_NAME}
        )
ENDIF (APPLE)
