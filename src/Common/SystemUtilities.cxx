/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <cstdlib>
#include <iostream>
#include <sstream>

#ifndef _WIN32
#include "execinfo.h"
#else
#include "Windows.h"
#endif

#include "SystemUtilities.h"

using namespace caret;

SystemUtilities::SystemUtilities()
    : CaretObject()
{
}

/**
 * Destructor
 */
SystemUtilities::~SystemUtilities()
{
}

#include <QtCore>
AString 
SystemUtilities::getBackTrace()
{
#ifdef CARET_OS_WINDOWS
    return "";
#else  // CARET_OS_WINDOWS
    std::stringstream str;
    void* callstack[1024];
    int numFrames = backtrace(callstack, 1024);
    char** symbols = backtrace_symbols(callstack, numFrames);
    for (int i = 0; i < numFrames; i++) {
        str << symbols[i] << std::endl;
    }
    return AString::fromStdString(str.str());
#endif // CARET_OS_WINDOWS
}


/**
 * Get the current directory.
 *
 * @return  Path of current directory.
 *
 */
AString
SystemUtilities::getCurrentDirectory()
{
    return AString("");
}

/**
 * Set the current directory.
 * @param path
 *
 */
void
SystemUtilities::setCurrentDirectory(const AString& path)
{
}

/**
 * Get the temporary directory.
 * 
 * @return  Path of temporary directory.
 *
 */
AString
SystemUtilities::getTempDirectory()
{
    return AString("");
}

/**
 * Get the user's home directory.
 * 
 * @return  Path to user's home directory.
 *
 */
AString
SystemUtilities::getUsersHomeDirectory()
{
    return AString("");
}

/**
 * Get the user's name.
 * 
 * @return  Name of user.
 *
 */
AString
SystemUtilities::getUserName()
{
    return AString("");
}

/**
 * Get the date as ISO format  yyyy-mm-dd  (2009-12-09)
 *
 * @return  A string containing the date.
 *
 */
AString
SystemUtilities::getDate()
{
    return AString("");
}

/**
 * Get the time as ISO format hh:mm:ss (11:42:28)
 *
 * @return  A string containing the time.
 *
 */
AString
SystemUtilities::getTime()
{
    return AString("");
}

/**
 * Get the date and time as month, day, year, hour:min:sec AM/PM
 * 
 * @return Data and Time.
 *
 */
AString
SystemUtilities::getDateAndTime()
{
    return AString("");
}

/**
 * Get the name of the operating system.
 * @return  Name of operating system.
 *
 */
AString
SystemUtilities::getOperatingSystemName()
{
    return AString("");
}

/**
 * Is the operating system the Microsoft Windows operating system?
 * 
 * @return  true if the operating system is Microsoft Windows.
 *
 */
bool
SystemUtilities::isWindowsOperatingSystem()
{
    return false;
}

/**
 * Is the operating system the Mac OSX operating system?
 * 
 * @return  true if the operating system is Mac OSX.
 *
 */
bool
SystemUtilities::isMacOperatingSystem()
{
    return false;
}

/**
 * Get the number of processors in the computer.
 * 
 * @return  The number of processors.
 *
 */
int32_t
SystemUtilities::getNumberOfProcessors()
{
    return 1;
}

/**
 * basename function (gets name of file without any path.
 * @param  name  File name that may have path in it.
 * @return File name without any path.
 *
 */
AString
SystemUtilities::basename(const AString& name)
{
    return AString("");
}

/**
 * dirname functions (get path of file without file name.)(
 * @param  path  File name that my contain the path.
 * @return Path of file.
 *
 */
AString
SystemUtilities::dirname(const AString& path)
{
    return AString("");
}

/**
 * Get the extension of a file's name.
 * 
 * @param path The file.
 * @return  The part of the file after the last "." in the file name.
 *
 */
AString
SystemUtilities::getFileExtension(const AString& path)
{
    return AString("");
}

/**
 * Test some of the methods in this class.
 *
 */
void
SystemUtilities::unitTest()
{
}

/**
 * Test the relative path method.
 * @param otherPath - determine relative path to otherpath
 * @param myPath - from myPath
 * @param correctResult - The correct result
 * @return  true if test passes.
 *
 */
bool
SystemUtilities::testRelativePath(
                   const AString& otherPath,
                   const AString& myPath,
                   const AString& correctResult)
{
   return false;
}

AString
SystemUtilities::createUniqueID()
{
    return AString("");
}

/**
 * Given the directory "mypath", determine the relative path to "otherpath".
 * Both input paths must be absolute paths, otherwise, otherPathIn is
 * returned.
 *
 * Examples:
 *    otherpath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS"
 *    mypath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM"
 *    result - "..";
 *
 *    otherpath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM/subdir"
 *    mypath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM"
 *    result - "subdir";
 *
 *    otherpath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/LEFT_HEM/subdir"
 *    mypath - "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM"
 *    result - "../LEFT_HEM/subdir";
 *
 * @param otherPathIn - The path for which relative path is sought.
 * @param myPathIn - Get the path from this
 * @return The relative path
 *
 */
AString
SystemUtilities::relativePath(
                   const AString& otherPathIn,
                   const AString& myPathIn)
{
   return AString("");
}

/**
 * Delete all files contained in a directory.  If the directory contains
 * subdirectories they are not deleted.
 * @param directoryPath  directory path.
 * @param deleteDirectoryFlag  If true, the directory is deleted after the
 * files in the directory have been deleted.
 *
 */
void
SystemUtilities::deleteAllFilesInDirectory(
                   const AString& directoryPath,
                   const bool deleteDirectoryFlag)
{
}

/**
 * Combine the directory and file names to create an absolute path.
 * If the file's name is an absolute path, it is simply returned.
 * Otherwise, the directory name followed by a directory separator
 * followed by the filename is returned.  Note that if the directory
 * is not an absolute path, a relative path is returned.
 * 
 * @param directoryName Name of directory.
 * @param fileName      Name of file.
 * @return  "directory/file"
 *
 */
AString
SystemUtilities::createFileAbsolutePath(
                   const AString& directoryName,
                   const AString& fileName)
{
    return AString("");
}
/**
 * Unexpected handler
 */
static void unexpectedHandler()
{
    std::cerr << "WARNING: unhandled exception." << std::endl;
    //if (theMainWindow != NULL) {
        const AString msg("Caret will be terminating due to an unexpected exception.\n"
                          "abort() will be called and a core file may be created.");
    std::cerr << qPrintable(msg) << std::endl;
        //QMessageBox::critical(theMainWindow, "ERROR", msg);
    //}
    
    std::cerr << qPrintable(SystemUtilities::getBackTrace()) << std::endl;
    
    abort();
}

/**
 * New handler
 */
static void newHandler()
{
    std::ostringstream str;
    str << "\n"
    << "OUT OF MEMORY\n"
    << "\n"
    << "This means that Caret is unable to get memory that it needs.\n"
    << "Possible causes:\n"
    << "   (1) Your computer lacks sufficient RAM.\n"
    << "   (2) Swap space is too small (you might increase it).\n"
    << "   (3) Your computer may be using an non-English character \n"
    << "       set.  Try switching to the English character set.\n"
    << "\n";
    std::cerr << str.str().c_str() << std::endl;
    
    std::cerr << qPrintable(SystemUtilities::getBackTrace()) << std::endl;
    
    abort();
    
    //if (theMainWindow != NULL) {
    //    QMessageBox::critical(theMainWindow, "OUT OF MEMORY",
    //                          "Out of memory, Caret terminating");
    //    std::exit(-1);
    //}
}

/**
 * Set handlers for unhandled exceptions or
 * out of memory errors.
 */
void 
SystemUtilities::setHandlersForUnexpected()
{
    std::set_unexpected(unexpectedHandler);
    std::set_new_handler(newHandler);

}
