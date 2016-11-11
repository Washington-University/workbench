#
# Minimum required version of CMAKE
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

MESSAGE("\nCMake Version: ${CMAKE_VERSION}\n")

#TSC: use "CACHE <type> <docstring>" syntax in SET commands so they can be overridden by cmake options

set (WORKBENCH_QT5_DEPRECATED_FATAL_ERROR FALSE CACHE BOOL "If TRUE, produce fatal compiler error for Qt 5 deprecated functionality")
set (WORKBENCH_USE_QT5 FALSE CACHE BOOL "Enable search for Qt 5.x libraries.  Must be passed as an argument to CMake:  cmake -DWORKBENCH_USE_QT5=TRUE <path-to-source>")
set (WORKBENCH_USE_QT5_QOPENGL_WIDGET FALSE CACHE BOOL "If Qt 5.x is used, use QOpenGLWidget instead of deprecated QGLWidget")
set (WORKBENCH_MESA_DIR "" CACHE PATH "Path to the Mesa root directory that contains the 'include' and 'lib' directories.  Mesa is optional and used by wb_command's -show-scene operation.")
#
# Setting the compiler MUST be done before the PROJECT
# statement or else an infinite loop will occur indicating
# that the compiler has been redefined.
#
 
 

IF(APPLE)
    ADD_DEFINITIONS(-DCARET_OS_MACOSX)
ELSEIF(UNIX)
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        ADD_DEFINITIONS(-DCARET_OS_LINUX)
    ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ELSEIF(WIN32)
    ADD_DEFINITIONS(-DCARET_OS_WINDOWS)
    # Policy CMP0020 is related to Qt
    # https://cmake.org/cmake/help/v3.5/policy/CMP0020.html
    if(POLICY CMP0020) 
        cmake_policy(SET CMP0020 NEW)
    endif()
    IF(MSVC)
        ADD_DEFINITIONS(-DCARET_OS_WINDOWS_MSVC)
        IF(CMAKE_CL_64)
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -DNOMINMAX)
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -wd4290 -wd4244 -wd4267 -wd4305 -wd4100 -wd4005" )
        ELSE()
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -DNOMINMAX)
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -wd4290 -wd4244 -wd4267 -wd4305 -wd4100 -wd4005" )
        ENDIF(CMAKE_CL_64)
    ELSE(MSVC)
    ENDIF(MSVC)
ELSE(APPLE)
    MESSAGE(FATAL_ERROR "Unrecognized operating system " ${CMAKE_SYSTEM_NAME})
ENDIF(APPLE)

#
# Shows compilation command when true
#
SET(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "cause all build commands to be displayed")

#
# Intel compiler
#
IF (${CMAKE_CXX_COMPILER} MATCHES "^.*icpc$")
    ADD_DEFINITIONS("-W -Wall -Werror=return-type -Werror=switch -Wunused-parameter")
ENDIF (${CMAKE_CXX_COMPILER} MATCHES "^.*icpc$")

#
# Clang compiler on Mac
#
UNSET(CLANG_FLAG)
IF (${CMAKE_CXX_COMPILER} MATCHES "^.*clang\\+\\+.*")
    SET(CLANG_FLAG TRUE)
ENDIF (${CMAKE_CXX_COMPILER} MATCHES "^.*clang\\+\\+.*")
IF (${CMAKE_CXX_COMPILER} MATCHES "^.*clang2\\+\\+.*")
    SET(CLANG_FLAG TRUE)
ENDIF (${CMAKE_CXX_COMPILER} MATCHES "^.*clang2\\+\\+.*")
IF (CLANG_FLAG)
    MESSAGE("USING CLANG COMPILER ${CMAKE_CXX_COMPILER}")
    ADD_DEFINITIONS("-W -Wall -Werror=return-type -Werror=switch -Wunused-parameter -Wno-deprecated-declarations")
ENDIF (CLANG_FLAG)


#
# IF GNU compiler, functions without a return type or switch
# statements that do not handle all of the enumerated types
# are treated as an error.  Also, all warnings.
#
IF (NOT MSVC)
    if (CMAKE_COMPILER_IS_GNUCC)
        execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                        OUTPUT_VARIABLE GCC_VERSION)
        string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
        list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
        list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)
        message("gcc major minor version numbers are: " ${GCC_MAJOR},${GCC_MINOR})
        if(${GCC_VERSION} VERSION_LESS "4.2")
            ADD_DEFINITIONS(-W -Wall)
        else()
            ADD_DEFINITIONS(-W -Wall -Werror=return-type -Werror=switch -Wunused-parameter)
        endif()

        if (${GCC_VERSION} VERSION_LESS "4.9")
            # there is no greater than or equal in CMake
        else()
            ADD_DEFINITIONS(-Wno-narrowing -Wno-unused-local-typedefs)
        endif()
    endif()
ENDIF (NOT MSVC)

#
# If GNU compiler, use SIMD-based dot computation, if possible
#
if (CMAKE_COMPILER_IS_GNUCC)

    #
    # Define flag to avoid trying to compile SIMD stuff (coded for x86 only)
    #
    SET(WORKBENCH_USE_SIMD TRUE CACHE BOOL "try to compile with SIMD support")

    #
    # If we should try to use SIMD, check whether cpuinfo compiles
    #
    IF (WORKBENCH_USE_SIMD)
        TRY_COMPILE(CPUINFO_COMPILES ${CMAKE_CURRENT_BINARY_DIR}/cpuinfo_compile_test ${CMAKE_CURRENT_SOURCE_DIR}/kloewe/cpuinfo cpuinfo)
        MESSAGE("CPUINFO_COMPILES: ${CPUINFO_COMPILES}")
    ENDIF (WORKBENCH_USE_SIMD)

    #
    # Add the necessary definition and include directory to enable the SIMD-based dot product implementations
    #
    if (WORKBENCH_USE_SIMD AND CPUINFO_COMPILES)
        ADD_DEFINITIONS(-DCARET_DOTFCN)
        INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/kloewe/dot/src)
    endif()
endif()

SET(OPENSSL_RESULT "Not Found")
FIND_PACKAGE(OpenSSL)
IF(OPENSSL_FOUND)
    INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
    MESSAGE("OpenSSL version=${OPENSSL_VERSION}")
    MESSAGE("OpenSSL include=${OPENSSL_INCLUDE_DIR}")
    MESSAGE("OpenSSL libraries=${OPENSSL_LIBRARIES}")
    SET(OPENSSL_RESULT "${OPENSSL_VERSION}   ${OPENSSL_LIBRARIES}")
ELSE (OPENSSL_FOUND)
    MESSAGE("OpenSSL NOT FOUND.  You may need to define OPENSSL_ROOT_DIR when running CMake.")
    MESSAGE("   cmake -DOPENSSL_ROOT_DIR=<path> \n")
    MESSAGE("See: https://cmake.org/cmake/help/v3.5/module/FindOpenSSL.html")
ENDIF(OPENSSL_FOUND)

#
# Must have QT 4.8 or QT 5.6 or later
# https://cmake.org/cmake/help/v3.5/manual/cmake-packages.7.html#manual:cmake-packages(7)
# Modules List: http://doc.qt.io/qt-5/qtmodules.html
#
SET(QT_RESULT "Not Found")
SET(CARET_QT5_LINK "")
IF (WORKBENCH_USE_QT5) 
    #
    # The OpenGL module is deprecated in Qt5 and 
    # is needed only if QGLWidget is used for
    # the OpenGL graphics.
    #
    IF (NOT WORKBENCH_USE_QT5_QOPENGL_WIDGET)
        SET(WB_QT_OPENGL_MODULE "OpenGL")
    ENDIF ()

    FIND_PACKAGE(Qt5 COMPONENTS Concurrent Core Gui Network ${WB_QT_OPENGL_MODULE} PrintSupport Test Widgets Xml)
    IF (Qt5_FOUND)
        MESSAGE("Found Qt 5")
        IF (NOT Qt5Concurrent_FOUND)
            STRING(APPEND moduleError "Qt5 Concurrentmodule not found\n")
        ENDIF()
        IF (NOT Qt5Core_FOUND)
            STRING(APPEND moduleError "Qt5 Core module not found\n")
        ENDIF()
        IF (NOT Qt5Gui_FOUND)
            STRING(APPEND moduleError "Qt5 Gui module not found\n")
        ENDIF()
        IF (NOT Qt5Network_FOUND)
            STRING(APPEND moduleError "Qt5 Network module not found\n")
        ENDIF()
        IF (NOT WORKBENCH_USE_QT5_QOPENGL_WIDGET)
            IF (NOT Qt5OpenGL_FOUND)
                STRING(APPEND moduleError "Qt5 OpenGL module not found\n")
            ENDIF()
        ENDIF ()
        IF (NOT Qt5PrintSupport_FOUND)
            STRING(APPEND moduleError "Qt5 Print Support module not found\n")
        ENDIF()
        IF (NOT Qt5Test_FOUND)
            STRING(APPEND moduleError "Qt5 Test module not found\n")
        ENDIF()
        IF (NOT Qt5Widgets_FOUND)
            STRING(APPEND moduleError "Qt5 Widgets module not found\n")
        ENDIF()
        IF (NOT Qt5Xml_FOUND)
            STRING(APPEND moduleError "Qt5 XML module not found\n")
        ENDIF()
        IF (moduleError)
            MESSAGE(FATAL_ERROR "Missing Modules\n" ${moduleError})
        ENDIF()

        IF (WORKBENCH_USE_QT5_QOPENGL_WIDGET)
            #
            # QGLWidget is deprecated in Qt 5 and is replaced with QOpenGLWidget
            #
            # If WORKBENCH_USE_QT5_QOPENGL_WIDGET is defined, QOpenGLWidget is used
            # for OpenGL.  Otherwise, QGLWidget is used.
            #
            add_definitions(-DWORKBENCH_USE_QT5_QOPENGL_WIDGET)
        ELSE ()
            remove_definitions(-DWORKBENCH_USE_QT5_QOPENGL_WIDGET)
        ENDIF ()

        IF (WORKBENCH_QT5_DEPRECATED_FATAL_ERROR)
            # Causes Qt to remove declarations for deprecated functionality
            add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050700) 
            # Enable usage of replacements for Qt deprecated functionality in Workbench source code
            add_definitions(-DWORKBENCH_REPLACE_QT_DEPRECATED)
        ENDIF ()

        #small hack to get the qt5 compile flag requirements into libraries
        SET(CARET_QT5_LINK "Qt5::Core")

        SET(QT_RESULT "${Qt5_VERSION}")
    ELSE ()
        MESSAGE(FATAL_ERROR "Qt5 was required but not found")
    ENDIF ()
ELSE ()
    FIND_PACKAGE(Qt4 4.8 REQUIRED)
    IF(QT4_FOUND)
        INCLUDE(${QT_USE_FILE})
        MESSAGE("QT_USE_FILE: ${QT_USE_FILE}")
        SET(QT_RESULT "${QTVERSION}")
    ELSE(QT4_FOUND)
        MESSAGE(FATAL_ERROR "QT4 not found")
    ENDIF(QT4_FOUND)
ENDIF ()

#
# Try to find Qwt, utherwise use bundle
#
SET(QWT_RESULT "Not Found")
IF (NOT WIN32)
    PKG_CHECK_MODULES(Qwt qwt)
ENDIF (NOT WIN32)
IF (Qwt_FOUND)
    MESSAGE("Qwt library found")
    MESSAGE("   INCLUDES ${Qwt_INCLUDE_DIRS}")
    MESSAGE("   LIBS ${Qwt_LIBRARIES}")
    SET(QWT_RESULT "Using Qwt Library ${Qwt_LIBRARIES}")
ELSE (Qwt_FOUND)
    MESSAGE("Qwt library not found, using bundled")
    SET(QWT_RESULT "In Workbench Source Code")
ENDIF (Qwt_FOUND)

#
# The Find OpenMP package may not work on all systems and the user may
# furnish the paths to the OpenMP files by using environment variables.
#
# The environment variables are:
#    OPENMP_COMPILE_OPTION=-fopenmp
#    OPENMP_HEADER_DIR=/usr/local/clang-openmp-opt/llvm/build/Release/include
#    OPENMP_LIB_DIR=/usr/local/clang-openmp-opt/llvm/build/Release/lib
#
UNSET(OPENMP_FOUND)
IF (EXISTS $ENV{OPENMP_HEADER_DIR})
    MESSAGE("OpenMP Header File:  $ENV{OPENMP_HEADER_DIR}")
    IF (EXISTS $ENV{OPENMP_LIB_DIR})
        MESSAGE("OpenMP Library File: $ENV{OPENMP_LIB_DIR}")
        SET (STUFF $ENV{OPENMP_COMPILE_OPTION})
        IF (DEFINED STUFF)
            MESSAGE("OpenMP Compiler Option: $ENV{OPENMP_COMPILE_OPTION}")
            SET(OpenMP_CXX_FLAGS "-I$ENV{OPENMP_HEADER_DIR} $ENV{OPENMP_COMPILE_OPTION}")
            SET(OpenMP_C_FLAGS "-I$ENV{OPENMP_HEADER_DIR} $ENV{OPENMP_COMPILE_OPTION}")
            SET(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -L$ENV{OPENMP_LIB_DIR})
            SET(OPENMP_FOUND TRUE)
        ENDIF (DEFINED STUFF)
    ENDIF (EXISTS $ENV{OPENMP_LIB_DIR})
ENDIF (EXISTS $ENV{OPENMP_HEADER_DIR})
    
#
# IF OpenMP not found through environment variables,
# Use CMAKE's Find OpenMP module
#
IF (NOT OPENMP_FOUND)
    FIND_PACKAGE(OpenMP)
ENDIF (NOT OPENMP_FOUND)

#
# If OpenMP is found, may need to set compiler and linker flags
#
SET(OPENMP_RESULT "Not Found")
IF (OPENMP_FOUND)
    # add definitions will add the flag to the linker and resource compilers, which don't understand the openmp option
    SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

    #
    # Try to link static with Intel Compiler
    #
    IF (${CMAKE_CXX_COMPILER} MATCHES "^.*icpc$")
        MESSAGE(WARNING "Intel Compiler Being Used")
        SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -openmp-link=static")
        SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-intel")
        ADD_DEFINITIONS("-static-intel")
    ENDIF()
    MESSAGE("OpenMP was Found")
    MESSAGE("    OpenMP_CXX_FLAGS: ${OpenMP_CXX_FLAGS}")
    SET(OPENMP_RESULT "Yes")
ELSE (OPENMP_FOUND)
    MESSAGE(WARNING "OpenMP was not found")

    IF (CLANG_FLAG)
        #
        # The clang compiler does not support OpenMP so it produces many warnings
        # with "Unknown pragma ignored".  So, tell clang to ignore unknown pragmas
        # so the message is not printed.
        #
        ADD_DEFINITIONS("-Wno-unknown-pragmas")
    ENDIF (CLANG_FLAG)
ENDIF(OPENMP_FOUND)

MESSAGE("\nC++ flags ${CMAKE_CXX_FLAGS}\n")

#
# MUST have ZLIB
#
SET(ZLIB_RESULT "Not Found")
FIND_PACKAGE(ZLIB)
IF ( ZLIB_FOUND )
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
    SET(ZLIB_RESULT ${ZLIB_VERSION_STRING})
ELSE (ZLIB_FOUND)
    MESSAGE(FATAL_ERROR "ZLIB was not found.  It may be necessary to define "
                  "the variable ZLIB_ROOT when running cmake so that "
                  "the FindZLIB module is able to find ZLIB.\n"
                  "   cmake -DZLIB_ROOT=<path> \n"
                  "See HINTS at "
                  "https://cmake.org/cmake/help/v3.5/module/FindZLIB.html")
ENDIF (ZLIB_FOUND)

#
# Try to find QuaZip, otherwise use bundled
#
SET(QUAZIP_RESULT "Not Found")
FIND_PACKAGE(QuaZip  QUIET)
IF (QUAZIP_FOUND)
  MESSAGE("QuaZip library found")
  MESSAGE("   INCLUDES ${QUAZIP_INCLUDE_DIRS}")
  MESSAGE("   LIBS ${QUAZIP_LIBRARIES}")
  SET(QUAZIP_RESULT "Using QuaZip Library ${QUAZIP_LIBRARIES}")
ELSE (QUAZIP_FOUND)
    MESSAGE("QuaZip library not found, using bundled")
    #
    # Quazip needs this defined here for static linking on windows
    #
    IF(WIN32)
        IF(MSVC)
            ADD_DEFINITIONS(-DQUAZIP_STATIC)
        ENDIF(MSVC)
    ENDIF(WIN32)
    SET(QUAZIP_RESULT "In Workbench Source Code")
ENDIF (QUAZIP_FOUND)

#
# Find FreeType
#
SET(FREETYPE_RESULT "Not Found")
SET(FTGL_RESULT "Must have FreeType to use FTGL")
SET(FTGL_FONT_MODULE_FOR_LINKING "")
IF (DEFINED WORKBENCH_FREETYPE_DIR)
   # FindFreeType looks for environment variable FREETYPE_DIR as a hint
   SET(ENV{FREETYPE_DIR} ${WORKBENCH_FREETYPE_DIR})
ENDIF ()
FIND_PACKAGE(Freetype)
IF (FREETYPE_FOUND)
    MESSAGE("FreeType library found")
    MESSAGE("   INCLUDES ${FREETYPE_INCLUDE_DIRS}")
    MESSAGE("   INCLUDES_FT2_BUILD ${FREETYPE_INCLUDE_DIR_ft2build}")
    MESSAGE("   INCLUDES_FT2 ${FREETYPE_INCLUDE_DIR_freetype2}")
    MESSAGE("   LIBS ${FREETYPE_LIBRARIES}")

    IF (NOT WIN32)
        PKG_CHECK_MODULES(FTGL ftgl)
    ENDIF (NOT WIN32)
    IF (FTGL_FOUND)
        MESSAGE("FTGL library found")
        MESSAGE("   INCLUDES ${FTGL_INCLUDE_DIRS}")
        MESSAGE("   LIBS ${FTGL_LIBRARIES}")
        SET(FTGL_RESULT "Using FTGL Library ${FTGL_LIBRARIES}")
    ELSE (FTGL_FOUND)
        MESSAGE("FTGL library not found, using bundled")
        SET(FTGL_RESULT "In Workbench Source Code")
    ENDIF (FTGL_FOUND)

    ADD_DEFINITIONS(-DHAVE_FREETYPE)
    SET(FREETYPE_RESULT "${FREETYPE_VERSION_STRING} ${FREETYPE_LIBRARIES}")
ELSE (FREETYPE_FOUND)
    SET (FREETYPE_LIBRARY "")
    SET (FREETYPE_LIBRARIES "")
    MESSAGE("FreeType library NOT found")
    MESSAGE("    Define the PATH to FreeType when running CMake")
    MESSAGE("       -DWORKBENCH_FREETYPE_DIR=<PATH>")
    MESSAGE("    Where <PATH> is the directory containing FreeType's")
    MESSAGE("    'include' and 'lib' directories.")
ENDIF (FREETYPE_FOUND)

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

SET(OSMESA_RESULT "Not found")
MESSAGE("OSMESA_DIR: ${WORKBENCH_MESA_DIR}")
IF (EXISTS ${WORKBENCH_MESA_DIR})
    IF (EXISTS ${WORKBENCH_MESA_DIR}/include/GL/osmesa.h)
        MESSAGE("Have Mesa Include Directory")
        FIND_LIBRARY(OSMESA_LIBRARY_FOUND
                     NAMES OSMesa
                     HINTS ${WORKBENCH_MESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GL_LIBRARY_FOUND
                     NAMES GL
                     HINTS ${WORKBENCH_MESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GLU_LIBRARY_FOUND
                     NAMES GLU
                     HINTS ${WORKBENCH_MESA_DIR}/lib)
        MESSAGE("OSMesa lib: " ${OSMESA_LIBRARY})
        IF (EXISTS ${OSMESA_LIBRARY_FOUND} AND EXISTS ${OSMESA_GL_LIBRARY_FOUND} AND EXISTS ${OSMESA_GLU_LIBRARY_FOUND})
            SET(OSMESA_DEFINITION -DHAVE_OSMESA)
            SET(OSMESA_OFFSCREEN_LIBRARY ${OSMESA_LIBRARY_FOUND})
            SET(OSMESA_GL_LIBRARY ${OSMESA_GL_LIBRARY_FOUND})
            SET(OSMESA_GLU_LIBRARY ${OSMESA_GLU_LIBRARY_FOUND})
            SET(OSMESA_INCLUDE_DIRECTORY ${WORKBENCH_MESA_DIR}/include)
            SET(OSMESA_FOUND TRUE)

            MESSAGE("Offscreen Mesa Library was found")
            MESSAGE("   Definition: ${OSMESA_DEFINITION}")
            MESSAGE("   Include:    ${OSMESA_INCLUDE_DIRECTORY}")
            MESSAGE("   Libraries:  ${OSMESA_OFFSCREEN_LIBRARY}")
            MESSAGE("   Libraries:  ${OSMESA_GL_LIBRARY}")
            MESSAGE("   Libraries:  ${OSMESA_GLU_LIBRARY}")
            
            SET(OSMESA_RESULT ${OSMESA_OFFSCREEN_LIBRARY})
        ENDIF (EXISTS ${OSMESA_LIBRARY_FOUND} AND EXISTS ${OSMESA_GL_LIBRARY_FOUND} AND EXISTS ${OSMESA_GLU_LIBRARY_FOUND})
    ENDIF (EXISTS ${WORKBENCH_MESA_DIR}/include/GL/osmesa.h)
ENDIF (EXISTS ${WORKBENCH_MESA_DIR})

#=============================================================================
MESSAGE("")
MESSAGE("Compiler: ${CMAKE_CXX_COMPILER}")
MESSAGE("Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
MESSAGE("")
#=============================================================================


#
# All subdirectories that will be configured for building
#
IF (NOT QUAZIP_FOUND)
  ADD_SUBDIRECTORY ( Quazip )
ENDIF (NOT QUAZIP_FOUND)
ADD_SUBDIRECTORY ( Common )
ADD_SUBDIRECTORY ( Xml )
ADD_SUBDIRECTORY ( Scenes )
ADD_SUBDIRECTORY ( OSMesaDummy )
IF (FREETYPE_FOUND AND NOT FTGL_FOUND)
    ADD_SUBDIRECTORY ( FtglFont )
ENDIF (FREETYPE_FOUND AND NOT FTGL_FOUND)
ADD_SUBDIRECTORY ( Annotations )
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
IF (NOT Qwt_FOUND)
    ADD_SUBDIRECTORY ( Qwt )
ENDIF (NOT Qwt_FOUND)
ADD_SUBDIRECTORY ( GuiQt )
ADD_SUBDIRECTORY ( Commands )
ADD_SUBDIRECTORY ( Desktop )
ADD_SUBDIRECTORY ( CommandLine )
ADD_SUBDIRECTORY ( Tests )
if (WORKBENCH_USE_SIMD AND CPUINFO_COMPILES)
    ADD_SUBDIRECTORY ( kloewe/cpuinfo )
    ADD_SUBDIRECTORY ( kloewe/dot )
ENDIF()

#
#CTest tests
#
ENABLE_TESTING()

#
# This is for help with finding variables created 
# by "Find" type commands
# To use, set DEBUG_VARIABLE_PREFIX to the prefix
# of variables that are sought.  For example,
# set to "Q" to find all variables beginning
# with "Q".  Or, set to "OSMESA" to find print
# all OSMESA variables.
SET(DEBUG_VARIABLE_PREFIX "")
IF(NOT (DEBUG_VARIABLE_PREFIX STREQUAL ""))
    get_cmake_property( debugAllVars VARIABLES )      
    foreach( debugVar in ${debugAllVars} ) 
        string(FIND ${debugVar} ${DEBUG_VARIABLE_PREFIX} debugOffset)
        if(${debugOffset} EQUAL 0)
            message(${MSG_TYPE} ${MSG_INDENT} "${debugVar}=${${debugVar}}" )
        endif()
    endforeach()
ENDIF()

#
# Print Summary
#
SET(MSG_TYPE "")
SET(MSG_INDENT "    ")
MESSAGE(${MSG_TYPE} "************* SUMMARY FROM RUNNING CMake ********************")
FIND_PROGRAM(QMAKE_PROGRAM "qmake" NAMES "qmake.exe")
MESSAGE(${MSG_TYPE} "TOOLS:")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "CMake Version:        ${CMAKE_VERSION}")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "CXX Compiler          ${CMAKE_CXX_COMPILER_ID}")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "CXX Compiler Flags:   ${CMAKE_CXX_FLAGS}")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "CXX Compiler Path:    ${CMAKE_CXX_COMPILER}")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "QMake Program:        ${QMAKE_PROGRAM}")
MESSAGE(${MSG_TYPE} "WORKBENCH VARIABLES:")
get_cmake_property( allVars VARIABLES )      
foreach( var in ${allVars} ) 
    string(FIND ${var} "WORKBENCH" workbenchOffset)
    if(${workbenchOffset} EQUAL 0)
        message(${MSG_TYPE} ${MSG_INDENT} "${var}=${${var}}" )
    endif()
endforeach()

MESSAGE(${MSG_TYPE} "PACKAGES:")
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "FTGL:     " ${FTGL_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "FreeType: " ${FREETYPE_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "OpenMP:   " ${OPENMP_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "OpenSSL:  " ${OPENSSL_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "OSMESA:   " ${OSMESA_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "Qt:       " ${QT_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "QuaZip:   " ${QUAZIP_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "Qwt:      " ${QWT_RESULT})
MESSAGE(${MSG_TYPE} ${MSG_INDENT} "ZLIB:     " ${ZLIB_RESULT})

MESSAGE(${MSG_TYPE} "************* END SUMMARY FROM RUNNING CMake *****************")
