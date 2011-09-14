#
# Minimum required version of CMAKE
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

#
#
#
#message(INFORMATION "\nTo get the correct version of QT, qmake must be in the PATH\n")

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
        IF(CMAKE_CL_64)
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -wd"4290" -wd"4244" -wd"4267" -wd"4305" -wd"4100" -wd"4005" -MP -DNOMINMAX)
            SET( ZLIB_INCLUDE_DIR "C:\\dev64\\install\\zlib\\include")
            SET( ZLIB_LIBRARY "C:\\dev64\\install\\zlib\\lib\\zlib.lib")
        ELSE()
            ADD_DEFINITIONS(-D_USE_MATH_DEFINES -wd"4290" -wd"4244" -wd"4267" -wd"4305" -wd"4100" -wd"4005" -MP -DNOMINMAX)
            SET( ZLIB_INCLUDE_DIR "C:\\dev32\\install\\zlib\\include")
            SET( ZLIB_LIBRARY "C:\\dev32\\install\\zlib\\lib\\zlib.lib")
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

      ###  SET( ZLIB_INCLUDE_DIR "C:\\dev32\\install\\zlib\\include")
      ###  SET( ZLIB_LIBRARY "C:\\dev32\\install\\zlib\\lib\\zlib.lib")
    ENDIF(MSVC)
ELSE(APPLE)
    MESSAGE(FATAL_ERROR "Unrecognized operating system " ${CMAKE_SYSTEM_NAME})
ENDIF(APPLE)



#cmake_policy(SET CMP0015 OLD)

#
# Shows compilation command when true
#
SET(CMAKE_VERBOSE_MAKEFILE TRUE)

#
# IF GNU compiler, functions without a return type or switch
# statements that do not handle all of the enumerated types
# are treated as an error.
#
IF (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "-Werror=return-type -Werror=switch ${CMAKE_CXX_FLAGS}")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

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
    SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
ELSE (OPENMP_FOUND)
    MESSAGE(FATAL_ERROR "OpenMP was not found")
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
ADD_SUBDIRECTORY ( Nifti )
ADD_SUBDIRECTORY ( Gifti )
ADD_SUBDIRECTORY ( Cifti )
ADD_SUBDIRECTORY ( Files )
ADD_SUBDIRECTORY ( Helpers )
ADD_SUBDIRECTORY ( Brain )
ADD_SUBDIRECTORY ( CommandOperations )
ADD_SUBDIRECTORY ( GuiQt )
ADD_SUBDIRECTORY ( Desktop )
ADD_SUBDIRECTORY ( CommandLine )


