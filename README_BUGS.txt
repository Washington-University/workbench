
**********************************************************************************


CMake Error in Tests/CMakeLists.txt:
  No known features for CXX compiler

  "Clang"

  version 4.0.0.

---------------------------------------------------------------------------------

CMake does not recognize the compiler and it fails to find the compiler's 
features.  We have seen this problem with Clang 4.0.0 and Qt 5.7.0 on 
Mac OS X.  It is a documented Qt bug.

Qt Bug 5466: https://bugreports.qt.io/browse/QTBUG-54666

The solution with Qt 5.7.0 is:
   (1) Edit <QTDIR>/lib/cmake/Qt5Core/Qt5CoreConfigExtras.cmake
   (2) Comment out the line "set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_FEATURES cxx_decltype)"  by placing a "#" at the beginning of the line.

**********************************************************************************


