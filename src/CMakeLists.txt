#
# Minimum required version of CMAKE
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

#
#
#
#message(INFORMATION "\nTo get the correct version of QT, qmake must be in the PATH\n")

#TSC: use "CACHE <type> <docstring>" syntax in SET commands so they can be overridden by cmake options

#
# Setting the compiler MUST be done before the PROJECT
# statement or else an infinite loop will occur indicating
# that the compiler has been redefined.
#
IF(APPLE)
    ADD_DEFINITIONS(-DCARET_OS_MACOSX)
ELSEIF(UNIX)
ELSEIF(WIN32)
    ADD_DEFINITIONS(-DCARET_OS_WINDOWS)
    IF(MSVC)
        ADD_DEFINITIONS(-DCARET_OS_WINDOWS_MSVC)
        IF(CMAKE_CL_64)
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -wd"4290" -wd"4244" -wd"4267" -wd"4305" -wd"4100" -wd"4005" -MP -DNOMINMAX)
            SET( ZLIB_INCLUDE_DIR "C:\\dev64\\install\\zlib\\include" CACHE STRING "zlib include directory (headers)")
            SET( ZLIB_LIBRARY "C:\\dev64\\install\\zlib\\lib\\zlib.lib" CACHE STRING "zlib library (binary)")
        ELSE()
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -wd"4290" -wd"4244" -wd"4267" -wd"4305" -wd"4100" -wd"4005" -MP -DNOMINMAX)
            SET( ZLIB_INCLUDE_DIR "C:\\dev32\\install\\zlib\\include" CACHE STRING "zlib include directory (headers)")
            SET( ZLIB_LIBRARY "C:\\dev32\\install\\zlib\\lib\\zlib.lib" CACHE STRING "zlib library (binary)")
        ENDIF(CMAKE_CL_64)
    ELSE(MSVC)
        FIND_PATH(ZLIB_INCLUDE_DIR zlib.h
            C:\\dev32\\install\\zlib\\include
            C:\\zlib_software\\zlib-1.2.5-install\\include)
        FIND_PATH(ZLIB_LIBRARY libzlib.a
            C:\\zlib_software\\zlib-1.2.5-install\\lib)
        IF (NOT ZLIB_FOUND)
            FIND_PATH(ZLIB_LIBRARY zlib.lib
            C:\\dev32\\install\\zlib\\lib)
        ENDIF(NOT ZLIB_FOUND)

      ###  SET( ZLIB_INCLUDE_DIR "C:\\dev32\\install\\zlib\\include" CACHE STRING "zlib include directory (headers)")
      ###  SET( ZLIB_LIBRARY "C:\\dev32\\install\\zlib\\lib\\zlib.lib" CACHE STRING "zlib library (binary)")
    ENDIF(MSVC)
ELSE(APPLE)
    MESSAGE(FATAL_ERROR "Unrecognized operating system " ${CMAKE_SYSTEM_NAME})
ENDIF(APPLE)



#cmake_policy(SET CMP0015 OLD)

#
# Shows compilation command when true
#
SET(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "cause all build commands to be displayed")

#
# IF GNU compiler, functions without a return type or switch
# statements that do not handle all of the enumerated types
# are treated as an error.  Also, all warnings.
#
###IF (CMAKE_COMPILER_IS_GNUCXX)
IF (NOT MSVC)
##    SET(CMAKE_CXX_FLAGS "-W -Wall -Werror=return-type -Werror=switch ${CMAKE_CXX_FLAGS}" CACHE STRING "c++ compiler specific options")
##    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror=return-type -Werror=switch -std=c++0x")
##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
if (CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
    list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
    list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)
    message("gcc major minor version numbers are: "${GCC_MAJOR},${GCC_MINOR})
endif()
if(${GCC_VERSION} VERSION_LESS "4.2")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror=return-type -Werror=switch")
endif()
ENDIF (NOT MSVC)

#
# Must have QT 4.7 or later
#
FIND_PACKAGE(Qt4 4.7.1 REQUIRED)
IF(QT4_FOUND)
ELSE(QT4_FOUND)
    MESSAGE(FATAL_ERROR "QT4 not found")
ENDIF(QT4_FOUND)

#
# QT include files
#
INCLUDE(${QT_USE_FILE})
#
#  Expect OpenMP on Apple and UNIX
#
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
    SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}" CACHE STRING "c++ compiler specific options")
ELSE (OPENMP_FOUND)
    MESSAGE(WARNING "OpenMP was not found")
ENDIF(OPENMP_FOUND)


#
# MUST have ZLIB
#
FIND_PACKAGE(ZLIB)
IF ( ZLIB_FOUND )
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
ELSE (ZLIB_FOUND)
    MESSAGE(FATAL_ERROR "ZLIB was not found")
ENDIF (ZLIB_FOUND)

#
# All subdirectories that will be configured for building
#
ADD_SUBDIRECTORY ( Common )
ADD_SUBDIRECTORY ( Xml )
ADD_SUBDIRECTORY ( Palette )
ADD_SUBDIRECTORY ( Nifti )
ADD_SUBDIRECTORY ( Gifti )
ADD_SUBDIRECTORY ( Cifti )
ADD_SUBDIRECTORY ( Files )
ADD_SUBDIRECTORY ( Algorithms )
ADD_SUBDIRECTORY ( Brain )
ADD_SUBDIRECTORY ( GuiQt )
ADD_SUBDIRECTORY ( CommandOperations )
ADD_SUBDIRECTORY ( CommandLine )
ADD_SUBDIRECTORY ( Desktop )
ADD_SUBDIRECTORY ( Tests )

#
#CTest tests
#
ENABLE_TESTING()

ADD_TEST(timer ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver timer)
ADD_TEST(progress ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver progress)
ADD_TEST(volumefile ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver volumefile)
ADD_TEST(http ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver http)
