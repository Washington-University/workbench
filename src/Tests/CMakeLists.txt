#
# Name of project
#
PROJECT (Tests)

#
# Add QT for includes
#
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
endif()
IF (QT4_FOUND)
    SET(QT_USE_QTXML TRUE)
    SET(QT_USE_QTNETWORK TRUE)
    INCLUDE(${QT_USE_FILE})
ENDIF ()

#
#The individual tests
#
ADD_LIBRARY(Tests
CiftiFileTest.h
DotTest.h
GeodesicHelperTest.h
HttpTest.h
HeapTest.h
LookupTest.h
MathExpressionTest.h
NiftiTest.h
PointerTest.h
ProgressTest.h
QuatTest.h
StatisticsTest.h
TestInterface.h
TimerTest.h
TopologyHelperOld.h
TopologyHelperTest.h
VolumeFileTest.h
XnatTest.h

CiftiFileTest.cxx
DotTest.cxx
GeodesicHelperTest.cxx
HttpTest.cxx
HeapTest.cxx
LookupTest.cxx
MathExpressionTest.cxx
NiftiTest.cxx
PointerTest.cxx
ProgressTest.cxx
QuatTest.cxx
StatisticsTest.cxx
TestInterface.cxx
TimerTest.cxx
TopologyHelperOld.cxx
TopologyHelperTest.cxx
VolumeFileTest.cxx
XnatTest.cxx
)


#
# Create the test1 executable
#
IF (APPLE)
   ADD_EXECUTABLE(test_driver
      MACOSX_BUNDLE
      test_driver.cxx
      )
      ADD_CUSTOM_COMMAND(
         TARGET test_driver
         POST_BUILD
         COMMAND ${CMAKE_SOURCE_DIR}/CMakeScripts/copy_mac_nib.sh test1
      )
ELSE (APPLE)
   ADD_EXECUTABLE(test_driver
      test_driver.cxx
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

#
# Libraries that are linked
#
TARGET_LINK_LIBRARIES(test_driver
Tests
Operations
Algorithms
OperationsBase
GuiQt
Brain
Files
Annotations
Cifti
Gifti
Nifti
FilesBase
Charting
Palette
Scenes
Xml
Common
${QT5_LINK_LIBS}
${QT_LIBRARIES}
${ZLIB_LIBRARIES}
#${LIBS}
)

IF(WIN32)
    TARGET_LINK_LIBRARIES(test_driver
    opengl32
    glu32
    )
ENDIF(WIN32)

IF (UNIX)
   IF (NOT APPLE) 
      TARGET_LINK_LIBRARIES(test_driver
         gobject-2.0
      )
   ENDIF (NOT APPLE)
ENDIF (UNIX)

#
# At this time, Cocoa needs to be explicitly added for Apple Mac
#
IF (APPLE)
   #SET (QT_MAC_USE_COCOA TRUE)
   TARGET_LINK_LIBRARIES(test_driver
     "-framework Cocoa"
     "-framework OpenGL"
   )
ENDIF (APPLE)

#
# Find Headers
#
INCLUDE_DIRECTORIES(
${CMAKE_SOURCE_DIR}/Tests
${CMAKE_SOURCE_DIR}/Operations
${CMAKE_SOURCE_DIR}/Algorithms
${CMAKE_SOURCE_DIR}/Annotations
${CMAKE_SOURCE_DIR}/OperationsBase
${CMAKE_SOURCE_DIR}/GuiQt
${CMAKE_SOURCE_DIR}/Brain
${CMAKE_SOURCE_DIR}/Charting
${CMAKE_SOURCE_DIR}/Palette
${CMAKE_SOURCE_DIR}/FilesBase
${CMAKE_SOURCE_DIR}/Files
${CMAKE_SOURCE_DIR}/Cifti
${CMAKE_SOURCE_DIR}/Gifti
${CMAKE_SOURCE_DIR}/Nifti
${CMAKE_SOURCE_DIR}/Scenes
${CMAKE_SOURCE_DIR}/Xml
${CMAKE_SOURCE_DIR}/Common
)

ENABLE_TESTING()

ADD_TEST(timer test_driver timer)
ADD_TEST(progress test_driver progress)
ADD_TEST(volumefile test_driver volumefile)
#debian build machines don't have internet access
#ADD_TEST(http test_driver http)
ADD_TEST(heap test_driver heap)
ADD_TEST(pointer test_driver pointer)
ADD_TEST(statistics test_driver statistics)
ADD_TEST(quaternion test_driver quaternion)
ADD_TEST(mathexpression test_driver mathexpression)
ADD_TEST(lookup test_driver lookup)
ADD_TEST(dotsimd test_driver dotsimd)
