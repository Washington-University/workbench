#
# Minimum required version of CMAKE
#
CMAKE_MINIMUM_REQUIRED (VERSION 3.5)

MESSAGE("\nCMake Version: ${CMAKE_VERSION}\n")

#
#
#
#message(INFORMATION "\nTo get the correct version of QT, qmake must be in the PATH\n")

#TSC: use "CACHE <type> <docstring>" syntax in SET commands so they can be overridden by cmake options

set (WORKBENCH_USE_QT5 FALSE CACHE BOOL "Enable search for Qt 5.x libraries")
set (WORKBENCH_USE_QT5_QOPENGL_WIDGET FALSE CACHE BOOL "If Qt 5.x is used, use QOpenGLWidget instead of deprecated QGLWidget")

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
            SET( OPENSSL_ROOT_DIR "c:\\dev64\\install\\openssl-1.0.1t" CACHE_STRING "open ssl root directory")

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
# May not need these, use CMAKE_CXX_STANDARD:UNINITIALIZED=11
#
SET (WORKBENCH_C11X FALSE)
##SET (WORKBENCH_C11X TRUE)
IF ("$ENV{WORKBENCH_CONFIGURE_C11X}" STREQUAL "YES")
   SET (WORKBENCH_C11X TRUE)
   MESSAGE("Configuring Workbench build with C11X enabled.")
ENDIF ("$ENV{WORKBENCH_CONFIGURE_C11X}" STREQUAL "YES")


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
    SET (CLANG_11X_FLAGS "-std=c++11 -stdlib=libstdc++ -Wno-error=c++11-narrowing")
    ####SET (CLANG_11X_FLAGS "-std=c++11 -stdlib=libc++ -Wno-error=c++11-narrowing")
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
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_11X_FLAGS}")
    ##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_11X_FLAGS} -W -Wall -Werror=return-type -Werror=switch -Wunused-parameter")
    ##SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_11X_FLAGS} -W -Wall -Werror=return-type -Werror=switch -Wunused-parameter" CACHE STRING "C++ compiler options" FORCE)

    IF (WORKBENCH_C11X)
        ####set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=c++11 -stdlib=libc++")
    ENDIF (WORKBENCH_C11X)
ENDIF (CLANG_FLAG)


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

    if (${GCC_VERSION} VERSION_LESS "4.9")
        # there is no greater than or equal in CMake
    else()
        ADD_DEFINITIONS(-Wno-narrowing -Wno-unused-local-typedefs)
    endif()

#    execute_process(COMMAND uname -n OUTPUT_VARIABLE MACHINE_NAME)
#    message("MACHINE_NAME: ${MACHINE_NAME}")
#    if (${MACHINE_NAME} MATCHES "linuxbuild")
#        message("is linuxbuild")
#        SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-E" ${CMAKE_EXE_LINKER_FLAGS})
#    endif()

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

#
# will set name of XCode project:  
# disabled at this time
#
##PROJECT(BorderOptProject C CXX)

FIND_PACKAGE(OpenSSL)
IF(OPENSSL_FOUND)
    INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
    MESSAGE("OpenSSL version=${OPENSSL_VERSION}")
    MESSAGE("OpenSSL include=${OPENSSL_INCLUDE_DIR}")
    MESSAGE("OpenSSL libraries=${OPENSSL_LIBRARIES}")
ELSE (OPENSSL_FOUND)
    MESSAGE("OpenSSL NOT FOUND")
ENDIF(OPENSSL_FOUND)

#
# Must have QT 4.8 or QT 5.6 or later
# https://cmake.org/cmake/help/v3.5/manual/cmake-packages.7.html#manual:cmake-packages(7)
# Modules List: http://doc.qt.io/qt-5/qtmodules.html
#
IF (WORKBENCH_USE_QT5) 
    message("WORKBENCH_USE_QT5 defined")
ELSE ()
    message("WORKBENCH_USE_QT5 NOT defined")
ENDIF ()

IF (WORKBENCH_USE_QT5) 
    
    #
    # The OpenGL module is deprecated in Qt5 and 
    # is needed only if QGLWidget is used for
    # the OpenGL graphics.
    #
    IF (NOT WORKBENCH_USE_QT5_QOPENGL_WIDGET)
        SET(WB_QT_OPENGL_MODULE "OpenGL")
    ENDIF ()

    #FIND_PACKAGE(Qt5 5.6.0 COMPONENTS Concurrent Core Gui Network OpenGL PrintSupport Test Widgets Xml)
    FIND_PACKAGE(Qt5 5.6.0 COMPONENTS Concurrent Core Gui Network ${WB_QT_OPENGL_MODULE} PrintSupport Test Widgets Xml)
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
            MESSAGE("Configuring to use QOpenGLWidget instead of deprecated QGLWidget")
            add_definitions(-DWORKBENCH_USE_QT5_QOPENGL_WIDGET)
        ELSE ()
            MESSAGE("Configuring to use deprecated QGLWidget instead of new QOpenGLWidget")
        ENDIF ()
    ELSE ()
        MESSAGE(FATAL_ERROR "Qt5 was required but not found")
    ENDIF ()
ELSE ()
    FIND_PACKAGE(Qt4 4.8 REQUIRED)
    IF(QT4_FOUND)
        INCLUDE(${QT_USE_FILE})
    ELSE(QT4_FOUND)
        MESSAGE(FATAL_ERROR "QT4 not found")
    ENDIF(QT4_FOUND)
    MESSAGE("QT_USE_FILE: ${QT_USE_FILE}")
ENDIF ()

#
# Try to find Qwt, utherwise use bundle
#
IF (NOT WIN32)
    PKG_CHECK_MODULES(Qwt qwt)
ENDIF (NOT WIN32)
IF (Qwt_FOUND)
    MESSAGE("Qwt library found")
    MESSAGE("   INCLUDES ${Qwt_INCLUDE_DIRS}")
    MESSAGE("   LIBS ${Qwt_LIBRARIES}")
ELSE (Qwt_FOUND)
    MESSAGE("Qwt library not found, using bundled")
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
FIND_PACKAGE(ZLIB)
IF ( ZLIB_FOUND )
    INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
ELSE (ZLIB_FOUND)
    MESSAGE(FATAL_ERROR "ZLIB was not found")
ENDIF (ZLIB_FOUND)

#
# Try to find QuaZip, otherwise use bundled
#
FIND_PACKAGE(QuaZip)
IF (QUAZIP_FOUND)
  MESSAGE("QuaZip library found")
  MESSAGE("   INCLUDES ${QUAZIP_INCLUDE_DIRS}")
  MESSAGE("   LIBS ${QUAZIP_LIBRARIES}")
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
ENDIF (QUAZIP_FOUND)

#
# Find FreeType
#
SET(FTGL_FONT_MODULE_FOR_LINKING "")

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
    ELSE (FTGL_FOUND)
        MESSAGE("FTGL library not found, using bundled")
    ENDIF (FTGL_FOUND)

    ADD_DEFINITIONS(-DHAVE_FREETYPE)
ELSE (FREETYPE_FOUND)
    SET (FREETYPE_LIBRARY "")
    SET (FREETYPE_LIBRARIES "")
    MESSAGE("FreeType library NOT found")
    MESSAGE("    The environment variable FREETYPE_DIR can be set to the")
    MESSAGE("    directory containing FreeType include and lib.")
    MESSAGE("")
    MESSAGE("    ")
    MESSAGE("    On Windows (and possibly other systems) it may be necessary to set")
    MESSAGE("        DFREETYPE_INCLUDE_DIR_freetype2")
    MESSAGE("            <your-path>/FreeType-X.Y.Z/include/freetype2")
    MESSAGE("        DFREETYPE_INCLUDE_DIR_ft2build")
    MESSAGE("            <your-path>/FreeType-X.Y.Z/include")
    MESSAGE("        DFREETYPE_LIBRARY")
    MESSAGE("            <your-path>/FreeType-X.Y.Z/lib/freetype.lib")
    MESSAGE("    ")
    MESSAGE("    These variable can be set when running cmake.  For example:")
    MESSAGE("        cmake -DFREETYPE_INCLUDE_DIR_freetype2=<your-path>/FreeType-X.Y.Z/include/freetype2")
    MESSAGE("    ")
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

MESSAGE("OSMESA_DIR: $ENV{OSMESA_DIR}")
IF (EXISTS $ENV{OSMESA_DIR})
    IF (EXISTS $ENV{OSMESA_DIR}/include/GL/osmesa.h)
        MESSAGE("Have Mesa Include Directory")
        FIND_LIBRARY(OSMESA_LIBRARY_FOUND
                     NAMES OSMesa
                     HINTS $ENV{OSMESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GL_LIBRARY_FOUND
                     NAMES GL
                     HINTS $ENV{OSMESA_DIR}/lib)
        FIND_LIBRARY(OSMESA_GLU_LIBRARY_FOUND
                     NAMES GLU
                     HINTS $ENV{OSMESA_DIR}/lib)
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
