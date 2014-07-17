

How FtglFont was created.

* Download and uncompress an FTGL library.
* Copy the source only: cp -r <ftgl-x.y>/src ./FtglFont
* cd FtglFont
* Create the CMakeLists.txt file.
** find . -name '*.h' -print
** find . -name '*.cpp' -print
** Add .h and .cpp files to ADD_LIBRARY
** Add all subdirectories to INCLUDE_DIRECTORIES
** Need 'ft2build.h' from FreeType it is in /Developer/SDKs/MacOSX10.7.sdk/usr/X11/include  on mac
** Change the include for "config.h" to "FtglConfig.h" so that it does not conflict with other "config.h" files
** Create the FtglConfig.h file by "cp <ftgl-x.y>/config.h.in   ./FtglConfig.h"
** Add FtglConfig.h to CMakeLists.txt
** Edit FtglConfig.h and define these items:
*** HAVE_INTTYPES_H
*** HAVE_MEMORY_H
*** HAVE_STDINT_H
*** HAVE_STDLIB_H
*** HAVE_STRINGS_H
*** HAVE_STRING_H
*** HAVE_STRNDUP
*** HAVE_SYS_TYPES_H
*** STDC_HEADERS
*** X_DISPLAY_MISSING

