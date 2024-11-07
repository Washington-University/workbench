#
# Name of project
#
PROJECT (Tests)

#
# Add QT for includes
#
if(Qt6_FOUND)
    include_directories(${Qt6Core_INCLUDE_DIRS})
    include_directories(${Qt6Network_INCLUDE_DIRS})
endif()
if(Qt5_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    include_directories(${Qt5Network_INCLUDE_DIRS})
endif()

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

TARGET_LINK_LIBRARIES(Tests ${CARET_QT5_LINK})

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

if(Qt6_FOUND)
    set(QT6_LINK_LIBS
        Qt6::Concurrent
        Qt6::Core
        Qt6::Core5Compat
        Qt6::Gui
        Qt6::Network
        Qt6::Test
        Qt6::Xml)
endif()

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
Graphics
Cifti
Gifti
Nifti
QxtCore
FilesBase
Charting
Palette
Scenes
Xml
CZICmd
CZI
CZIJxrDecode
${WORKBENCH_OME_ZARR_Z5_MODULE}
Common
${QT5_LINK_LIBS}
${QT6_LINK_LIBS}
${QT_LIBRARIES}
${GLEW_LIBRARIES}
${OSMESA_OFFSCREEN_LIBRARY}
${OSMESA_GL_LIBRARY}
${OSMESA_GLU_LIBRARY}
${WORKBENCH_OME_ZARR_COMPRESSION_LIBRARIES}
${ZLIB_LIBRARIES}
${OPENMP_LIBRARY}
#${LIBS}
)

IF(WIN32)
    TARGET_LINK_LIBRARIES(test_driver
    ${GLEW_LIBRARIES}
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
