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
    SET(CMAKE_OSX_ARCHITECTURES x86_64)
    ADD_DEFINITIONS(-DCARET_MAC_OSX)
ELSEIF(UNIX)
ELSEIF(WINDOWS)
    ADD_DEFINITIONS(-DCARET_WINDOWS)
ELSE(APPLE)
    MESSAGE(FATAL_ERROR "Unrecognized operating system " ${CMAKE_SYSTEM_NAME})
ENDIF(APPLE)

#cmake_policy(SET CMP0015 OLD)

#
# Shows compilation command when true
#
SET(CMAKE_VERBOSE_MAKEFILE TRUE)

#
# Must have QT 4.7 or later
#
FIND_PACKAGE(Qt4 4.7.1 REQUIRED)
IF(QT4_FOUND)
ELSE(QT4_FOUND)
    MESSAGE(FATAL_ERROR "QT4 not found")
ENDIF(QT4_FOUND)


#
#  Expect OpenMP on Apple and UNIX
#
FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
    SET(CMAKE_CXX_FLAGS ${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS})
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
ADD_SUBDIRECTORY ( Files )
ADD_SUBDIRECTORY ( Brain )
ADD_SUBDIRECTORY ( GuiQt )
ADD_SUBDIRECTORY ( Desktop )
ADD_SUBDIRECTORY ( Command )


