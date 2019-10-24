#
# Minimum required version of CMAKE
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

SET(WB_VERSION "1.4.0")
#
# Set to true for verbose output when debugging this file
#
SET(WB_CMAKE_VERBOSE_OUTPUT_FLAG TRUE)

##########################################################################################
#
# A function to get all user defined variables with a specified prefix
# From https://cmake.org/Wiki/CMake/Tutorials/SettingVariableGroups
#
function (getListOfVarsStartingWith _prefix _varResult)
    get_cmake_property(_vars CACHE_VARIABLES)
    string (REGEX MATCHALL "(^|;)${_prefix}[A-Za-z0-9_]*" _matchedVars "${_vars}")
    set (${_varResult} ${_matchedVars} PARENT_SCOPE)
endfunction()


##########################################################################################
#
# Workbench CACHE variables
# TSC: use "CACHE <type> <docstring>" syntax in SET commands so they can be overridden by cmake options
#
set (WORKBENCH_QT5_DISABLE_DEPRECATED FALSE CACHE BOOL 
     "If TRUE, use of Qt 5 deprecated functionality will cause compiler to fail")
set (WORKBENCH_USE_CMAKE_AUTOMOC FALSE CACHE BOOL 
     "If TRUE, use CMAKE_AUTOMOC command for generating 'moc' files")
set (WORKBENCH_USE_QT5 TRUE CACHE BOOL 
     "If TRUE, require Qt5, else Qt4.<NEWLINE>Specify as argument to cmake: -DWORKBENCH_USE_QT5=TRUE")
set (WORKBENCH_USE_QT5_QOPENGL_WIDGET FALSE CACHE BOOL 
     "With Qt5, QOpenGLWidget instead of deprecated QGLWidget")
set (WORKBENCH_MESA_DIR "" CACHE PATH 
     "Directory containing Mesa's 'include' and 'lib' <NEWLINE>directories.  Mesa is optional and used by <NEWLINE>wb_command's -show-scene operation.")

##########################################################################################
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
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -DNOMINMAX)
            SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -wd4290 -wd4244 -wd4267 -wd4305 -wd4100 -wd4005" )
        IF(CMAKE_CL_64)
        ELSE()
        ENDIF(CMAKE_CL_64)
    ELSE(MSVC)
    ENDIF(MSVC)
ELSE(APPLE)
    MESSAGE(SEND_ERROR "Unrecognized operating system " ${CMAKE_SYSTEM_NAME})
ENDIF(APPLE)

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
        message("gcc version is: " ${GCC_VERSION})
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

##########################################################################################
#
# If GNU compiler, use SIMD-based dot computation, if possible
#
SET(SIMD_RESULT "Search for SIMD disabled for compiler on this system.")
if ((CMAKE_COMPILER_IS_GNUCC OR CLANG_FLAG) AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    #
    # Define flag to avoid trying to compile SIMD stuff (coded for x86_64 only)
    #
    SET(WORKBENCH_USE_SIMD TRUE CACHE BOOL "try to compile with SIMD support")

    #
    # If we should try to use SIMD, check whether cpuinfo compiles
    #
    IF (WORKBENCH_USE_SIMD)
        TRY_COMPILE(CPUINFO_COMPILES ${CMAKE_CURRENT_BINARY_DIR}/cpuinfo_compile_test ${CMAKE_CURRENT_SOURCE_DIR}/kloewe/cpuinfo cpuinfo)
        
        #
        # Add the necessary definition and include directory to enable the 
        # SIMD-based dot product implementations
        #
        IF (CPUINFO_COMPILES)
            ADD_DEFINITIONS(-DCARET_DOTFCN)
            INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/kloewe/dot/src)
            SET(SIMD_RESULT "Enabled")
        ELSE()
            SET(SIMD_RESULT "Failed when compiling with SIMD")
        ENDIF()
    ELSE (WORKBENCH_USE_SIMD)
        SET(SIMD_RESULT "Search for SIMD disabled, WORKBENCH_USE_SIMD=FALSE")
    ENDIF (WORKBENCH_USE_SIMD)
endif ((CMAKE_COMPILER_IS_GNUCC OR CLANG_FLAG) AND CMAKE_SIZEOF_VOID_P EQUAL 8)

#TSC: we require c++11, so it needs to be in the compile flags - in older cmake, this needs to be done manually
IF (${CMAKE_VERSION} VERSION_LESS "3.1")
    IF (CMAKE_COMPILER_IS_GNUCC)
        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("-std=c++11" GCC_STD11)
        IF (${GCC_STD11})
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        ELSE (${GCC_STD11})
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        ENDIF (${GCC_STD11})
    ENDIF (CMAKE_COMPILER_IS_GNUCC)
ELSE (${CMAKE_VERSION} VERSION_LESS "3.1")
    SET(CMAKE_CXX_STANDARD 11)
    SET(CMAKE_CXX_STANDARD_REQUIRED TRUE)
ENDIF (${CMAKE_VERSION} VERSION_LESS "3.1")

##########################################################################################
#
# OpenSSL
#
SET(OPENSSL_RESULT "Not Found")
FIND_PACKAGE(OpenSSL)
IF(OPENSSL_FOUND)
    INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
    SET(OPENSSL_RESULT "${OPENSSL_VERSION}   ${OPENSSL_LIBRARIES}")
ELSE (OPENSSL_FOUND)
    MESSAGE("OpenSSL NOT FOUND.  May need to define OPENSSL_ROOT_DIR when running CMake.")
    MESSAGE("   cmake -DOPENSSL_ROOT_DIR=<path> \n")
    MESSAGE("See: https://cmake.org/cmake/help/v3.5/module/FindOpenSSL.html")
ENDIF(OPENSSL_FOUND)

##########################################################################################
#
# Must have QT 4.8 or QT 5, recommended 5.6 or later
# https://cmake.org/cmake/help/v3.5/manual/cmake-packages.7.html#manual:cmake-packages(7)
# Modules List: http://doc.qt.io/qt-5/qtmodules.html
#
SET(WB_WEBKIT_COMPONENTS "")
SET(WB_WEBKIT_LIBS "")
SET(HAVE_QT_WEBKIT False)
IF (HAVE_QT_WEBKIT)
    ADD_DEFINITIONS(-DHAVE_WEBKIT)
    SET (WB_WEBKIT_COMPONENTS "WebEngine WebEngineCore WebEngineWidgets")
    SET (WB_WEBKIT_LIBS "Qt5::WebEngine Qt5::WebEngineCore Qt5::WebEngineWidgets")

ENDIF (HAVE_QT_WEBKIT)
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

    FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Concurrent Core Gui Network ${WB_QT_OPENGL_MODULE} PrintSupport Test Widgets Xml ${WB_WEBKIT_COMPONENTS})
    IF (WORKBENCH_USE_QT5_QOPENGL_WIDGET)
        #
        # QGLWidget is deprecated in Qt 5 and is replaced with QOpenGLWidget
        #
        # If WORKBENCH_USE_QT5_QOPENGL_WIDGET is defined, QOpenGLWidget is used
        # for OpenGL.  Otherwise, QGLWidget is used.
        #
        add_definitions(-DWORKBENCH_USE_QT5_QOPENGL_WIDGET)
    ELSE (WORKBENCH_USE_QT5_QOPENGL_WIDGET)
        remove_definitions(-DWORKBENCH_USE_QT5_QOPENGL_WIDGET)
    ENDIF (WORKBENCH_USE_QT5_QOPENGL_WIDGET)

    IF (WORKBENCH_QT5_DISABLE_DEPRECATED)
        # Causes Qt to remove declarations for deprecated functionality
        add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050700) 
        # Enable usage of replacements for Qt deprecated functionality in Workbench source code
        add_definitions(-DWORKBENCH_REPLACE_QT_DEPRECATED)
    ENDIF (WORKBENCH_QT5_DISABLE_DEPRECATED)

    #small hack to get the qt5 compile flag requirements into libraries
    SET(CARET_QT5_LINK "Qt5::Core")

    SET(QT_RESULT "${Qt5_VERSION}")
ELSE ()
    FIND_PACKAGE(Qt4 4.8 REQUIRED)
    IF(QT4_FOUND)
        INCLUDE(${QT_USE_FILE})
        SET(QT_RESULT "${QTVERSION}")
    ELSE(QT4_FOUND)
        MESSAGE(SEND_ERROR "QT4 not found")
    ENDIF(QT4_FOUND)
ENDIF ()

##########################################################################################
#
# Try to find Qwt, otherwise use bundle
#
SET(QWT_RESULT "Not Found")
IF (NOT WIN32)
    PKG_CHECK_MODULES(Qwt qwt)
ENDIF (NOT WIN32)
IF (Qwt_FOUND)
    SET(QWT_RESULT "Using Qwt Library ${Qwt_LIBRARIES}")
ELSE (Qwt_FOUND)
    SET(QWT_RESULT "In Workbench Source Code")
ENDIF (Qwt_FOUND)

##########################################################################################
#
# glm Math Library is header filess only containing C++ templates and functions.
# Since there is no code to link, only need the path to the include files
# https://glm.g-truc.net/0.9.9/index.html
#
SET(GLMATH_RESULT "Not Found")
IF (NOT WIN32)
    PKG_CHECK_MODULES(GLMath glm)
ENDIF (NOT WIN32)
IF (GLMath_FOUND)
    SET(GLMATH_RESULT "Using OpenGL Math directory ${GLMath_INCLUDE_DIRS}")
    INCLUDE_DIRECTORIES("${GLMath_INCLUDE_DIRS}")
ELSE (GLMath_FOUND)
    SET(GLMATH_RESULT "In Workbench Source Code")
    INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/GLMath")
ENDIF (GLMath_FOUND)


##########################################################################################
#
# Can Use OpenCL
#
SET(OPENCL_RESULT "Not found (OK, unused at this time)")
FIND_PACKAGE(OpenCL)
IF (OpenCL_FOUND)
    SET(OPENCL_RESULT "Version: ${OpenCL_VERSION_STRING}")
ENDIF (OpenCL_FOUND)

##########################################################################################
#
# Need OpenGL
#
SET(OPENGL_RESULT "")
FIND_PACKAGE(OpenGL REQUIRED)
IF (OPENGL_FOUND)
    #
    # Need help finding includes on Apple
    #
    IF (APPLE)
        # When searching for the include directory, find the location
        # for the OpenGL framework rather than an individual header file.
        FIND_PATH(OPENGL_INCLUDE_DIR OpenGL.framework
                  /System/Library/Frameworks
                  /Library/Frameworks
                  ~/Library/Frameworks)
    ENDIF (APPLE)
    #
    # OpenGL Include Directory
    #
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
    MESSAGE("OpenGL Include Directory: ${OPENGL_INCLUDE_DIR}")
    MESSAGE("OpenGL Libraries: ${OPENGL_LIBRARIES}")
    SET(OPENGL_RESULT ${OPENGL_LIBRARIES})
ELSE (OPENGL_FOUND)
    MESSAGE(SEND_ERROR "OpenGL Libraries were not found")
ENDIF (OPENGL_FOUND)

##########################################################################################
#
# Look for GLEW but only on Windows
#
SET(GLEW_RESULT "No")
IF (WIN32)
    FIND_PACKAGE(GLEW)
ENDIF ()

IF (GLEW_FOUND)
    SET(GLEW_RESULT "${GLEW_INCLUDE_DIRS} ${GLEW_LIBRARIES}")
    MESSAGE("GLEW Include Dirs: ${GLEW_INCLUDE_DIRS}")
    MESSAGE("GLEW Libraries: ${GLEW_LIBRARIES}")
    STRING(FIND ${GLEW_LIBRARIES} "glew32s.lib" GLEW_STATIC_LIB_NAME_INDEX)
    ADD_DEFINITIONS(-DHAVE_GLEW)
    IF (GLEW_STATIC_LIB_NAME_INDEX GREATER -1)
        ADD_DEFINITIONS(-DHAVE_GLEW_STATIC_LIB)
        MESSAGE("GLEW Static Library Found")
    ENDIF()
    INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIRS})
ELSE()
    SET(GLEW_LIBRARIES "")
    SET(GLEW_INCLUDE_DIRS "")
    IF (WIN32)
        MESSAGE(SEND_ERROR "GLEW not found and is required on Windows for finding OpenGL functions.\n"
                           "    Setting CMAKE_PREFIX_PATH to the GLEW installation directory when\n"
                           "    running CMAKE may help find GLEW.\n")
    ENDIF()
ENDIF()

##########################################################################################
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

##########################################################################################
#
# MUST have ZLIB
#
SET(ZLIB_RESULT "Not Found")
FIND_PACKAGE(ZLIB)
IF ( ZLIB_FOUND )
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
    SET(ZLIB_RESULT ${ZLIB_VERSION_STRING})
ELSE (ZLIB_FOUND)
    MESSAGE(SEND_ERROR "ZLIB was not found.  It may be necessary to define "
                  "the variable ZLIB_ROOT when running cmake so that "
                  "the FindZLIB module is able to find ZLIB.\n"
                  "   cmake -DZLIB_ROOT=<path> \n"
                  "See HINTS at "
                  "https://cmake.org/cmake/help/v3.5/module/FindZLIB.html")
ENDIF (ZLIB_FOUND)

##########################################################################################
#
# Try to find QuaZip, otherwise use bundled
#
SET(QUAZIP_RESULT "Not Found")
FIND_PACKAGE(QuaZip  QUIET)
IF (QUAZIP_FOUND)
  SET(QUAZIP_RESULT "Using QuaZip Library ${QUAZIP_LIBRARIES}")
ELSE (QUAZIP_FOUND)
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

##########################################################################################
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
    IF (NOT WIN32)
        PKG_CHECK_MODULES(FTGL ftgl)
    ENDIF (NOT WIN32)
    IF (FTGL_FOUND)
        SET(FTGL_RESULT "Using FTGL Library ${FTGL_LIBRARIES}")
    ELSE (FTGL_FOUND)
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

##########################################################################################
#
# Fixes issue with XCode and newer version of CMake.
# It prevents the ZERO_CHECK dependency from running
# (which is very slow) every time a build is performed
# in XCode.
#
IF (APPLE)
    SET (CMAKE_SUPPRESS_REGENERATION TRUE)
ENDIF (APPLE)

##########################################################################################
#
# Test for offscreen MESA (optional library)
# Mesa is used by wb_command's -show-scene.
# If Mesa is not found, the "-show-scene" command will not be available.
#
SET(OSMESA_FOUND FALSE)
SET(OSMESA_DEFINITION "")
SET(OSMESA_OFFSCREEN_LIBRARY "")
SET(OSMESA_GL_LIBRARY "")
SET(OSMESA_GLU_LIBRARY "")
SET(OSMESA_INCLUDE_DIRECTORY "")

SET(OSMESA_RESULT "Not found")
IF (EXISTS ${WORKBENCH_MESA_DIR})
    IF (EXISTS ${WORKBENCH_MESA_DIR}/include/GL/osmesa.h)
        FIND_LIBRARY(OSMESA_LIBRARY_FOUND
                     NAMES OSMesa OSMESA32
                     HINTS ${WORKBENCH_MESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GL_LIBRARY_FOUND
                     NAMES GL OPENGL32
                     HINTS ${WORKBENCH_MESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GLU_LIBRARY_FOUND
                     NAMES GLU GLU32
                     HINTS ${WORKBENCH_MESA_DIR}/lib)
        IF (EXISTS ${OSMESA_LIBRARY_FOUND} AND EXISTS ${OSMESA_GL_LIBRARY_FOUND} AND EXISTS ${OSMESA_GLU_LIBRARY_FOUND})
            SET(OSMESA_DEFINITION -DHAVE_OSMESA)
            SET(OSMESA_OFFSCREEN_LIBRARY ${OSMESA_LIBRARY_FOUND})
            SET(OSMESA_GL_LIBRARY ${OSMESA_GL_LIBRARY_FOUND})
            SET(OSMESA_GLU_LIBRARY ${OSMESA_GLU_LIBRARY_FOUND})
            SET(OSMESA_INCLUDE_DIRECTORY ${WORKBENCH_MESA_DIR}/include)
            SET(OSMESA_FOUND TRUE)

            SET(OSMESA_RESULT ${OSMESA_OFFSCREEN_LIBRARY})
        ENDIF (EXISTS ${OSMESA_LIBRARY_FOUND} AND EXISTS ${OSMESA_GL_LIBRARY_FOUND} AND EXISTS ${OSMESA_GLU_LIBRARY_FOUND})
    ENDIF (EXISTS ${WORKBENCH_MESA_DIR}/include/GL/osmesa.h)
ENDIF (EXISTS ${WORKBENCH_MESA_DIR})

##########################################################################################
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
ADD_SUBDIRECTORY ( QxtCore )
ADD_SUBDIRECTORY ( FilesBase )
ADD_SUBDIRECTORY ( Nifti )
ADD_SUBDIRECTORY ( Gifti )
ADD_SUBDIRECTORY ( Cifti )
ADD_SUBDIRECTORY ( Files )
ADD_SUBDIRECTORY ( OperationsBase )
ADD_SUBDIRECTORY ( Algorithms )
ADD_SUBDIRECTORY ( Operations )
ADD_SUBDIRECTORY ( Brain )
ADD_SUBDIRECTORY ( Graphics )
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

##########################################################################################
#
# CTest tests
#
ENABLE_TESTING()

##########################################################################################
#
# DEBUGGING ONLY
#
# Some find modules do not always document all of their variables.
# If you know the prefix for the find module's variables,
# this code can be used to print the variables by setting
# DEBUG_VARIABLE_PREFIX to the prefix.
# For example, set to "Q" to find all variables beginning from Qt
# or to "OSMESA" to print all OSMESA variables.
#
SET(DEBUG_VARIABLE_PREFIX "")
IF(NOT ("${DEBUG_VARIABLE_PREFIX}" STREQUAL ""))
    getListOfVarsStartingWith(${DEBUG_VARIABLE_PREFIX} allVars)
    foreach( debugVar IN LISTS allVars ) 
            message(${MSG_TYPE} ${MSG_INDENT} "${debugVar}=${${debugVar}}" )
    endforeach()
ENDIF()

##########################################################################################
#
# Print Summary
#
IF(WB_CMAKE_VERBOSE_OUTPUT_FLAG)
    SET(MSG_MODE "")  #Optional 
    SET(MSG_INDENT "    ")
    MESSAGE(${MSG_MODE} "************* SUMMARY FROM RUNNING CMake ********************")
    FIND_PROGRAM(QMAKE_PROGRAM "qmake" NAMES "qmake.exe")

    MESSAGE(${MSG_MODE} "TOOLS:")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "CMake Build Type:     ${CMAKE_BUILD_TYPE}")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "CMake Version:        ${CMAKE_VERSION}")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "CXX Compiler          ${CMAKE_CXX_COMPILER_ID}")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "CXX Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "CXX Compiler Flags:   ${CMAKE_CXX_FLAGS}")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "CXX Compiler Path:    ${CMAKE_CXX_COMPILER}")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "QMake Program:        ${QMAKE_PROGRAM}")

    MESSAGE(${MSG_MODE} "WORKBENCH VARIABLES:")
    getListOfVarsStartingWith("WORKBENCH" allVars)
    foreach( var IN LISTS allVars ) 
            get_property(docString CACHE ${var} PROPERTY HELPSTRING)
            message(${MSG_MODE} ${MSG_INDENT} "${var}=${${var}}" )
            
            STRING(REPLACE "<NEWLINE>" "\n${MSG_INDENT}${MSG_INDENT}" docPrint ${docString})
            message(${MSG_MODE} ${MSG_INDENT} ${MSG_INDENT} ${docPrint})
    endforeach()

    MESSAGE(${MSG_MODE} "PACKAGES:")
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "FTGL:     " ${FTGL_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "FreeType: " ${FREETYPE_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "GLEW:     " ${GLEW_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "GLMath:   " ${GLMATH_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "OpenCL:   " ${OPENCL_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "OpenGL:   " ${OPENGL_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "OpenMP:   " ${OPENMP_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "OpenSSL:  " ${OPENSSL_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "OSMESA:   " ${OSMESA_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "Qt:       " ${QT_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "QuaZip:   " ${QUAZIP_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "Qwt:      " ${QWT_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "SIMD:     " ${SIMD_RESULT})
    MESSAGE(${MSG_MODE} ${MSG_INDENT} "ZLIB:     " ${ZLIB_RESULT})

    MESSAGE(${MSG_MODE} "************* END SUMMARY FROM RUNNING CMake *****************")
ENDIF(WB_CMAKE_VERBOSE_OUTPUT_FLAG)
