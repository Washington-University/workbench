Adding support for reading ZARR files to Connectome Workbench.

Compiler support for C++20 is required for these libraries.  The top level CMakeLists.txt file in the Workbench source code will set the C++ standard to "20" using the cmake variables.  Ensure that your command to configure the workbench source code DOES NOT request a C++ standard using either the cmake variable CMAKE_CXX_STANDARD or by adding "-std=XX" to the CXX flags.

These cmake scripts can be used to get and build the Z5 and dependent libraries on systems without a package manager (such as those commonly found on Linux).  

If you have a package manager, using the package manager to install Z5 (that also installs its dependencies) should work.  After installing the libraries, set the variable WORKBENCH_USE_Z5 to true when running cmake to configure the workbench source code (-DWORKBENCH_USE_Z5=TRUE).

If you do not have a package manager, these scripts will get and build the source for the Z5 library and its dependencies.  These scripts are written in cmake’s language and are not traditional cmake files.  Each of the scripts executes a series of commands instead of configuring source code.

* Create a new directory OUTSIDE of the workbench source code. (Eg: /Users/me/z5_libraries).
* “cd /Users/me/z5_libraries” so that you are in the new, empty directory.
* Run “cmake -P <path-to-workbench-source>/caret7_source/z5_support_scripts/get_source.cmake_script”.  This command will get the GIT repositories for Z5 and the other libraries and place them in the “source” subdirectory.
* Run “cmake -P <path-to-workbench-source>/caret7_source/z5_support_scripts/build.cmake_script”.  This will build all of the libraries.  Two new subdirectories are created.  The first, “build”, contains the intermediate files used during the build process.  The second, “install”, contains the output header files and libraries.
* Add two new variables when configuring the workbench source code.  Set the variable WORKBENCH_USE_Z5 to true and set WORKBENCH_Z5_PATH to the absolute path of the “install” subdirectory (-DWORKBENCH_USE_Z5=TRUE -DWORKBENCH_Z5_PATH=Users/me/z5_libraries/install).
* If desired, the “build” and “source” directories can be deleted.

