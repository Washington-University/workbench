#
# Name of project
#
PROJECT (Tests)

#
# Need XML from Qt
#
SET(QT_DONT_USE_QTGUI)

#
# Add QT for includes
#
INCLUDE (${QT_USE_FILE})

#
# Create the test1 executable
#
IF (APPLE)
   ADD_EXECUTABLE(test1
      MACOSX_BUNDLE
      test1.cxx
      )
      ADD_CUSTOM_COMMAND(
         TARGET test1
         POST_BUILD
         COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_nib.sh test1
      )
ELSE (APPLE)
   ADD_EXECUTABLE(test1
      test1.cxx
   )
ENDIF (APPLE)

#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(test1
GuiQt
Brain
Helpers
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
    TARGET_LINK_LIBRARIES(test1
    opengl32
    glu32
    )
ENDIF(WIN32)

IF (UNIX)
   IF (NOT APPLE) 
      TARGET_LINK_LIBRARIES(test1
         gobject-2.0
      )
   ENDIF (NOT APPLE)
ENDIF (UNIX)

#
# At this time, Cocoa needs to be explicitly added for Apple Mac
#
IF (APPLE)
   #SET (QT_MAC_USE_COCOA TRUE)
   TARGET_LINK_LIBRARIES(test1
     "-framework Cocoa"
     "-framework OpenGL"
   )
ENDIF (APPLE)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/CommandOperations
${CMAKE_SOURCE_DIR}/GuiQt
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Helpers
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)
