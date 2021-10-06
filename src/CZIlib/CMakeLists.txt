PROJECT(CZI_Library)

IF(UNIX)
# linking with 'thread' is necessary if we use std::thread and related under Linux it seems
#   otherwise - the program simply crashes (no build-error)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 -fPIC -D_FILE_OFFSET_BITS=64")
   SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__ANSI__ -fPIC -D_FILE_OFFSET_BITS=64")

   ADD_DEFINITIONS("-w")
ENDIF(UNIX)

ADD_DEFINITIONS(-DWORKBENCH_CZI_MOD)

ADD_SUBDIRECTORY(CZIJxrDecode)
ADD_SUBDIRECTORY(CZI)
ADD_SUBDIRECTORY(CZICmd)
