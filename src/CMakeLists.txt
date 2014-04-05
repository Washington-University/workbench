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
    ADD_DEFINITIONS(-DCARET_OS_LINUX)
ELSEIF(WIN32)
    ADD_DEFINITIONS(-DCARET_OS_WINDOWS)
    IF(MSVC)
        ADD_DEFINITIONS(-DCARET_OS_WINDOWS_MSVC)
        IF(CMAKE_CL_64)
## SET(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform Toolset" FORCE) 
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -DNOMINMAX)
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -wd4290 -wd4244 -wd4267 -wd4305 -wd4100 -wd4005" )
            ##SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -we4061")
            SET( ZLIB_INCLUDE_DIR "C:\\dev64\\install\\zlib\\include" CACHE STRING "zlib include directory (headers)")
#           SET( ZLIB_LIBRARY "C:\\dev64\\install\\zlib\\lib\\zlib.lib" CACHE STRING "zlib library (binary)")
            SET(ZLIB_LIBRARY optimized "C:\\dev64\\install\\zlib\\lib\\zlib.lib" debug "C:\\dev64\\install\\zlib\\lib\\zlibd.lib" CACHE STRING "zlib library (binary)")
            SET( OPENSSL_ROOT_DIR "c:\\dev64\\install\\openssl" CACHE_STRING "open ssl root directory")

        ELSE()
## SET(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012" CACHE STRING "Platform Toolset" FORCE)         
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -DNOMINMAX)
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -wd4290 -wd4244 -wd4267 -wd4305 -wd4100 -wd4005" )
            ##SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -we4061")
            SET( ZLIB_INCLUDE_DIR "C:\\dev32\\install\\zlib\\include" CACHE STRING "zlib include directory (headers)")
#            SET( ZLIB_LIBRARY "C:\\dev32\\install\\zlib\\lib\\zlib.lib" CACHE STRING "zlib library (binary)") 
            SET(ZLIB_LIBRARY optimized "C:\\dev32\\install\\zlib\\lib\\zlib.lib" debug "C:\\dev32\\install\\zlib\\lib\\zlibd.lib" CACHE STRING "zlib library (binary)")             
            SET( OPENSSL_ROOT_DIR "c:\\dev32\\install\\openssl" CACHE_STRING "open ssl root directory")
        ENDIF(CMAKE_CL_64)
    ELSE(MSVC)
        SET( OPENSSL_ROOT_DIR "c:\\dev32\\install\\openssl" CACHE_STRING "open ssl root directory")
        FIND_PATH(ZLIB_INCLUDE_DIR zlib.h
            C:\\dev32\\install\\zlib\\include
            C:\\zlib_software\\zlib-1.2.5-install\\include
            $ENV{ZLIB_INC_DIR})
        FIND_PATH(ZLIB_LIBRARY libzlib.a
            C:\\zlib_software\\zlib-1.2.5-install\\lib)
        IF (NOT ZLIB_FOUND)
            FIND_PATH(ZLIB_LIBRARY zlib.lib
            C:\\dev32\\install\\zlib\\lib)
        ENDIF(NOT ZLIB_FOUND)
        IF (NOT ZLIB_FOUND)
            FIND_PATH(ZLIB_LIBRARY libz.a
                $ENV{ZLIB_LIB_DIR})
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
# Allow support for C11X compiler
#
SET (WORKBENCH_C11X FALSE)
##SET (WORKBENCH_C11X TRUE)

#
# Set flags for C11 compiler
# Only set for C++ compiler
# C11x options are not recognized by C compiler
#
SET (CLANG_11X_FLAGS "")
SET (GNU_11X_FLAGS "")
SET (INTEL_11X_FLAGS "")
IF (WORKBENCH_C11X)
    ADD_DEFINITIONS("-DWORKBENCH_HAVE_C11X")
    SET (CLANG_11X_FLAGS "-std=c++11 -Wno-error=c++11-narrowing")
    SET (GNU_11X_FLAGS "-std=c++11 -Wno-error=c++11-narrowing")
    SET (INTEL_11X_FLAGS "-std=c++11 -Wno-error=c++11-narrowing")
ENDIF (WORKBENCH_C11X)

#
# Intel compiler
#
IF (${CMAKE_CXX_COMPILER} MATCHES "^.*icpc$")
    ADD_DEFINITIONS("-W -Wall -Werror=return-type -Werror=switch -Wunused-parameter")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${INTEL_11X_FLAGS}")
ENDIF (${CMAKE_CXX_COMPILER} MATCHES "^.*icpc$")

#
# Clang compiler on Mac
#
IF (${CMAKE_CXX_COMPILER} MATCHES "^.*clang\\+\\+.*")
    ADD_DEFINITIONS("-W -Wall -Werror=return-type -Werror=switch -Wunused-parameter")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_11X_FLAGS}")
    ##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_11X_FLAGS} -W -Wall -Werror=return-type -Werror=switch -Wunused-parameter")
    ##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_11X_FLAGS} -W -Wall -Werror=return-type -Werror=switch -Wunused-parameter" CACHE STRING "C++ compiler options" FORCE)
ENDIF (${CMAKE_CXX_COMPILER} MATCHES "^.*clang\\+\\+.*")


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
    message("gcc major minor version numbers are: " ${GCC_MAJOR},${GCC_MINOR})
    if(${GCC_VERSION} VERSION_LESS "4.2")
        #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall" CACHE STRING "c++ compiler specific options")
        ADD_DEFINITIONS(-W -Wall)
    else()
        #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Werror=return-type -Werror=switch" CACHE STRING "c++ compiler specific options")
        ADD_DEFINITIONS(-W -Wall -Werror=return-type -Werror=switch -Wunused-parameter)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNU_11X_FLAGS}")
    endif()

#    execute_process(COMMAND uname -n OUTPUT_VARIABLE MACHINE_NAME)
#    message("MACHINE_NAME: ${MACHINE_NAME}")
#    if (${MACHINE_NAME} MATCHES "linuxbuild")
#        message("is linuxbuild")
#        SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-E" ${CMAKE_EXE_LINKER_FLAGS})
#    endif()

endif()
ENDIF (NOT MSVC)

FIND_PACKAGE(OpenSSL)
IF(OPENSSL_FOUND)
    INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
ENDIF(OPENSSL_FOUND)

#
# Must have QT 4.8 or later
#
FIND_PACKAGE(Qt4 4.8 REQUIRED)
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
    #SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}" CACHE STRING "c++ compiler specific options")
    # add definitions will add the flag to the linker and resource compilers, which don't understand the openmp option
    SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
    #ADD_DEFINITIONS(${OpenMP_CXX_FLAGS})
    #SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")

    #
    # Try to link static with Intel Compiler
    #
    IF (${CMAKE_CXX_COMPILER} MATCHES "^.*icpc$")
        MESSAGE(WARNING "Intel Compiler Being Used")
        SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -openmp-link=static")
        SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-intel")
        ADD_DEFINITIONS("-static-intel")
    ENDIF()
ELSE (OPENMP_FOUND)
    MESSAGE(WARNING "OpenMP was not found")
ENDIF(OPENMP_FOUND)


MESSAGE("\nC++ flags ${CMAKE_CXX_FLAGS}\n")

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
# Quazip needs this defined here for static linking on windows
#
IF(WIN32)
   IF(MSVC)
      ADD_DEFINITIONS(-DQUAZIP_STATIC)
   ENDIF(MSVC)
ENDIF(WIN32)

#
# Fixes issue with XCode and newer version of CMake.
# It prevents the ZERO_CHECK dependency from running
# (which is very slow) every time a build is performed
# in XCode.
#
IF (APPLE)
    SET (CMAKE_SUPPRESS_REGENERATION TRUE)
ENDIF (APPLE)
#=============================================================================
#
# Test for offscreen mesa (optional library)
# If found, set some variables.  Since, Mesa is only used for 
# command line 
#
SET(OSMESA_FOUND FALSE)
SET(OSMESA_DEFINITION "")
SET(OSMESA_OFFSCREEN_LIBRARY "")
SET(OSMESA_GL_LIBRARY "")
SET(OSMESA_GLU_LIBRARY "")
SET(OSMESA_INCLUDE_DIRECTORY "")

MESSAGE("OSMESA_DIR: $ENV{OSMESA_DIR}")
IF (EXISTS $ENV{OSMESA_DIR})
    IF (EXISTS $ENV{OSMESA_DIR}/include/GL/osmesa.h)
        MESSAGE("Have Mesa Include Directory")
        FIND_LIBRARY(OSMESA_LIBRARY_FOUND
                     NAMES libOSMesa.a libOSMesa.so OSMesa.lib OSMesa.dll
                     PATHS $ENV{OSMESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GL_LIBRARY_FOUND
                     NAMES libGL.a libGL.so
                     PATHS $ENV{OSMESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GLU_LIBRARY_FOUND
                     NAMES libGLU.a libGLU.so
                     PATHS $ENV{OSMESA_DIR}/lib)
        MESSAGE("OSMesa lib: " ${OSMESA_LIBRARY})
        IF (EXISTS ${OSMESA_LIBRARY_FOUND} AND EXISTS ${OSMESA_GL_LIBRARY_FOUND} AND EXISTS ${OSMESA_GLU_LIBRARY_FOUND})
            SET(OSMESA_DEFINITION -DHAVE_OSMESA)
            SET(OSMESA_OFFSCREEN_LIBRARY ${OSMESA_LIBRARY_FOUND})
            SET(OSMESA_GL_LIBRARY ${OSMESA_GL_LIBRARY_FOUND})
            SET(OSMESA_GLU_LIBRARY ${OSMESA_GLU_LIBRARY_FOUND})
            SET(OSMESA_INCLUDE_DIRECTORY $ENV{OSMESA_DIR}/include)
            SET(OSMESA_FOUND TRUE)

            MESSAGE("Offscreen Mesa Library was found")
            MESSAGE("   Definition: ${OSMESA_DEFINITION}")
            MESSAGE("   Include:    ${OSMESA_INCLUDE_DIRECTORY}")
            MESSAGE("   Libraries:  ${OSMESA_OFFSCREEN_LIBRARY}")
            MESSAGE("   Libraries:  ${OSMESA_GL_LIBRARY}")
            MESSAGE("   Libraries:  ${OSMESA_GLU_LIBRARY}")
        ENDIF (EXISTS ${OSMESA_LIBRARY_FOUND} AND EXISTS ${OSMESA_GL_LIBRARY_FOUND} AND EXISTS ${OSMESA_GLU_LIBRARY_FOUND})
    ENDIF (EXISTS $ENV{OSMESA_DIR}/include/GL/osmesa.h)
ENDIF (EXISTS $ENV{OSMESA_DIR})

#=============================================================================


#
# All subdirectories that will be configured for building
#
ADD_SUBDIRECTORY ( Quazip )
ADD_SUBDIRECTORY ( Common )
ADD_SUBDIRECTORY ( Xml )
ADD_SUBDIRECTORY ( Scenes )
ADD_SUBDIRECTORY ( GlfFont )
ADD_SUBDIRECTORY ( Charting )
ADD_SUBDIRECTORY ( Palette )
ADD_SUBDIRECTORY ( FilesBase )
ADD_SUBDIRECTORY ( Nifti )
ADD_SUBDIRECTORY ( Gifti )
ADD_SUBDIRECTORY ( Cifti )
ADD_SUBDIRECTORY ( Files )
ADD_SUBDIRECTORY ( OperationsBase )
ADD_SUBDIRECTORY ( Algorithms )
ADD_SUBDIRECTORY ( Operations )
ADD_SUBDIRECTORY ( Brain )
ADD_SUBDIRECTORY ( Qwt )
ADD_SUBDIRECTORY ( GuiQt )
ADD_SUBDIRECTORY ( Commands )
ADD_SUBDIRECTORY ( Desktop )
ADD_SUBDIRECTORY ( CommandLine )
ADD_SUBDIRECTORY ( Tests )

#
#CTest tests
#
ENABLE_TESTING()

ADD_TEST(timer ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver timer)
ADD_TEST(progress ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver progress)
ADD_TEST(volumefile ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver volumefile)
#debian build machines don't have internet access
#ADD_TEST(http ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver http)
ADD_TEST(heap ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver heap)
ADD_TEST(pointer ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver pointer)
ADD_TEST(statistics ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver statistics)
ADD_TEST(quaternion ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver quaternion)
ADD_TEST(mathexpression ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver mathexpression)
ADD_TEST(lookup ${CMAKE_CURRENT_BINARY_DIR}/Tests/test_driver lookup)
