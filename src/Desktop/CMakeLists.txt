#
# Name of Project
#
PROJECT(Desktop)

#cmake_policy(SET CMP0015 OLD) 

#
#  QT Libraries
#
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTOPENGL TRUE)
#SET(QT_USE_MNG TRUE)

#
# QT Include directories
#
INCLUDE (${QT_USE_FILE})

#
# Create the executable
# Apple creates a bundle
#
IF (APPLE)
   ADD_EXECUTABLE(desktop
      MACOSX_BUNDLE
      desktop.cxx
   )
ELSE (APPLE)
   ADD_EXECUTABLE(desktop
      desktop.cxx
   )
ENDIF (APPLE)

#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(desktop
GuiQt
Brain
Files
Gifti
Nifti
Xml
Common
${QT_LIBRARIES}
${ZLIB_LIBRARIES}
#${LIBS}
)

IF(WIN32)
    TARGET_LINK_LIBRARIES(desktop
    opengl32
    glu32
    )
ENDIF(WIN32)

IF (UNIX)
   IF (NOT APPLE) 
      TARGET_LINK_LIBRARIES(desktop
         gobject-2.0
      )
   ENDIF (NOT APPLE)
ENDIF (UNIX)

#
# At this time, Cocoa needs to be explicitly added for Apple Mac
#
IF (APPLE)
   #SET (QT_MAC_USE_COCOA TRUE)
   TARGET_LINK_LIBRARIES(desktop
     "-framework Cocoa"
     "-framework OpenGL"
   )
ENDIF (APPLE)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Desktop
${CMAKE_SOURCE_DIR}/GuiQt
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

IF (APPLE)
   #
   # If the path to the Resources directory does not exist
   # it needs to be created for the ".app".
   #
   SET(RESOURCE_DIR ${CMAKE_BINARY_DIR}/Desktop/desktop.app/Contents/Resources)
   #MESSAGE("RESOURCE DIR: " ${RESOURCE_DIR})
   #MESSAGE(WARNING "CURRENT DIR: " ${Desktop_BINARY_DIR})

   IF (NOT EXISTS ${RESOURCE_DIR})
      file(MAKE_DIRECTORY ${RESOURCE_DIR})
   ENDIF (NOT EXISTS ${RESOURCE_DIR})

   #
   # Copy the qt_menu.nib directory into the app's Resource directory
   #
   ADD_CUSTOM_COMMAND(
      TARGET desktop
      POST_BUILD
      COMMAND cp -r ${QT_BINARY_DIR}/../src/gui/mac/qt_menu.nib ${RESOURCE_DIR}
   )
ENDIF (APPLE)
