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

#include <sstream>

#include "execinfo.h"
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

std::string 
SystemUtilities::getBackTrace()
{
    std::ostringstream str;
    void* callstack[1024];
    int numFrames = backtrace(callstack, 1024);
    char** symbols = backtrace_symbols(callstack, numFrames);
    for (int i = 0; i < numFrames; i++) {
        str << symbols[i] << std::endl;
    }
    return str.str();
}


/**
 * Get the current directory.
 *
 * @return  Path of current directory.
 *
 */
std::string
SystemUtilities::getCurrentDirectory()
{
}

/**
 * Set the current directory.
 * @param path
 *
 */
void
SystemUtilities::setCurrentDirectory(const std::string& path)
{
}

/**
 * Get the temporary directory.
 * 
 * @return  Path of temporary directory.
 *
 */
std::string
SystemUtilities::getTempDirectory()
{
}

/**
 * Get the user's home directory.
 * 
 * @return  Path to user's home directory.
 *
 */
std::string
SystemUtilities::getUsersHomeDirectory()
{
}

/**
 * Get the user's name.
 * 
 * @return  Name of user.
 *
 */
std::string
SystemUtilities::getUserName()
{
}

/**
 * Get the date as ISO format  yyyy-mm-dd  (2009-12-09)
 *
 * @return  A string containing the date.
 *
 */
std::string
SystemUtilities::getDate()
{
}

/**
 * Get the time as ISO format hh:mm:ss (11:42:28)
 *
 * @return  A string containing the time.
 *
 */
std::string
SystemUtilities::getTime()
{
}

/**
 * Get the date and time as month, day, year, hour:min:sec AM/PM
 * 
 * @return Data and Time.
 *
 */
std::string
SystemUtilities::getDateAndTime()
{
}

/**
 * Get the name of the operating system.
 * @return  Name of operating system.
 *
 */
std::string
SystemUtilities::getOperatingSystemName()
{
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
}

/**
 * basename function (gets name of file without any path.
 * @param  name  File name that may have path in it.
 * @return File name without any path.
 *
 */
std::string
SystemUtilities::basename(const std::string& name)
{
}

/**
 * dirname functions (get path of file without file name.)(
 * @param  path  File name that my contain the path.
 * @return Path of file.
 *
 */
std::string
SystemUtilities::dirname(const std::string& path)
{
}

/**
 * Get the extension of a file's name.
 * 
 * @param path The file.
 * @return  The part of the file after the last "." in the file name.
 *
 */
std::string
SystemUtilities::getFileExtension(const std::string& path)
{
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
                   const std::string& otherPath,
                   const std::string& myPath,
                   const std::string& correctResult)
{
}

std::string
SystemUtilities::createUniqueID()
{
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
std::string
SystemUtilities::relativePath(
                   const std::string& otherPathIn,
                   const std::string& myPathIn)
{
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
                   const std::string& directoryPath,
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
std::string
SystemUtilities::createFileAbsolutePath(
                   const std::string& directoryName,
                   const std::string& fileName)
{
}

