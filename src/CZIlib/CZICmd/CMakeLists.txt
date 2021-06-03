PROJECT(CZICmd)

IF(UNIX)
###find_package(ZLIB)
###find_package(PNG)
ENDIF(UNIX)

add_library(CZICmd STATIC cmdlineoptions.cpp consoleio.cpp execute.cpp getOpt.cpp CZIcmd.h CZIcmd.cpp SaveBitmap.cpp stdafx.cpp streamimpl.cpp cmdlineoptions.h consoleio.h execute.h getOpt.h inc_libCZI.h SaveBitmap.h stdafx.h streamimpl.h targetver.h utils.cpp utils.h platform_defines.h DisplaySettingsHelper.h)

add_definitions(-DUNICODE -D_UNICODE)

include_directories(
   ${CMAKE_SOURCE_DIR}/CZIlib/CZI
   ${CMAKE_SOURCE_DIR}/CZIlib/CZIJxrDecode
)



