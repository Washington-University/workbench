PROJECT(CZI)

IF(NOT WIN32)
    #MESSAGE("GCC ${CMAKE_C_COMPILER_VERSION}")
    
    IF (${CMAKE_C_COMPILER_VERSION} VERSION_LESS "12.0")
            # there is no greater than or equal in CMake
    ELSE()
        # Disable warning "converting the enum constant to boolean"
        ADD_DEFINITIONS("-Wno-int-in-bool-context")

        # Disable warning "Destructor called on non-final <class name> that has virtual functions
        # but non-virtual destructor"
        ADD_DEFINITIONS("-Wno-delete-non-abstract-non-virtual-dtor")

        # Disable warning "Comparison of integers of different signs..."
        ADD_DEFINITIONS("-Wno-sign-compare")

        # Disable warning "moving a temporary object prevents copy elision"
        ADD_DEFINITIONS("-Wno-pessimizing-move")
    ENDIF()
ENDIF()

file(GLOB LIBCZISRCFILES "*.h" "*.cpp" "*.hpp")
file(GLOB_RECURSE LIBCZISRCEIGENFILES "eigen/*")

add_library(CZI STATIC ${LIBCZISRCFILES} ${LIBCZISRCEIGENFILES})

SET_TARGET_PROPERTIES (CZI PROPERTIES DEFINE_SYMBOL  "LIBCZI_EXPORTS" )

