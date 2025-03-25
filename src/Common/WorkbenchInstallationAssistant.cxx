
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __WORKBENCH_INSTALLATION_ASSISTANT_DECLARE__
#include "WorkbenchInstallationAssistant.h"
#undef __WORKBENCH_INSTALLATION_ASSISTANT_DECLARE__

#include <QDir>
#include <QFileInfo>

#include "CaretAssert.h"
#include "CaretException.h"

using namespace caret;


    
/**
 * \class caret::WorkbenchInstallationAssistant 
 * \brief Functions to assist with installation of Workbench
 * \ingroup Common
 */

/**
 * Set the application file path (contents of argv[0]).
 * While we could use QCoreApplication::applicationFilePath(), the documentation
 * for it contains this warning:
 *
 * Warning: On Linux, this function will try to get the path from the /proc file system. If that fails, it assumes
 * that argv[0] contains the absolute file name of the executable. The function also assumes that the
 * current directory has not been changed by the application.
 *
 * Since we may already or may in the future change the current directory, use this function to set
 * the application file path when wb_view starts.
 *
 * @param applicationFilePath
 *   The application file path.
 */
void
WorkbenchInstallationAssistant::setApplicationFilePath(const AString& applicationFilePath)
{
    s_applicationFilePath = applicationFilePath;
}

/**
 * Constructor.
 */
WorkbenchInstallationAssistant::WorkbenchInstallationAssistant()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
WorkbenchInstallationAssistant::~WorkbenchInstallationAssistant()
{
}


/**
 * Find the bin directory using the path of wb_view
 * @return FunctionResult with path or error information.
 */
FunctionResultString
WorkbenchInstallationAssistant::findBinDirectory() const
{
    try {
        if (s_applicationFilePath.isEmpty()) {
            return FunctionResultString("",
                                        ("The application has NOT set the application file path by calling: "
                                         "WorkbenchInstallationAssistant::setApplicationFilePath()"),
                                        false);
        }
        
        verifyExists(s_applicationFilePath, "Application file path");
        
        const QFileInfo fileInfo(s_applicationFilePath);
        const QDir appDirPath(fileInfo.dir());
        verifyExists(appDirPath, "Application directory path");

        
#ifdef CARET_OS_LINUX
        return findBinDirectoryLinux(appDirPath);
#endif // CARET_OS_MACOSX
        
#ifdef CARET_OS_MACOSX
        return findBinDirectoryMacos(appDirPath);
#endif // CARET_OS_MACOSX
        
#ifdef CARET_OS_WINDOWS
        return findBinDirectoryWindows(appDirPath);
#endif // CARET_OS_MACOSX
        
    }
    catch (const CaretException& caretException) {
        return FunctionResultString("",
                                    caretException.whatString(),
                                    false);
    }
    return FunctionResultString("",
                                "Unrecognized operating: system not Linux, nor macOS, nor Windows",
                                false);
}

/**
 * @return True if the given bin directory is in the user's PATH
 */
bool
WorkbenchInstallationAssistant::isBinDirectoryInUsersPath(const AString& binDirectory) const
{
    if (binDirectory.isEmpty()) {
        return false;
    }
    
    const AString usersPath(qEnvironmentVariable("PATH"));
    if (m_debugFlag) std::cout << "PATH: " << usersPath << std::endl;
    if ( ! usersPath.isEmpty()) {
        if (usersPath.contains(binDirectory)) {
            return true;
        }
    }
    
    return false;
}


//FunctionResultString testRunWbCommand() const;


/**
 * Find the bin directory using the path of wb_view for Linux
 * @return FunctionResult with path or error information.
 * @param appDirPath
 *    Directory containing the application (wb_view)
 */
FunctionResultString
WorkbenchInstallationAssistant::findBinDirectoryLinux(const QDir& appDirPath) const
{
    /*
     * For Linux:
     * App:     workbench/exe_linux64/wb_view
     * Scripts: workbench/bin_linux64/wb_command
     *
     * For RedHat Linux:
     * App:     workbench/exe_rh_linux64/wb_view
     * Scripts: workbench/bin_rh_linux64/wb_command
     */
    
    bool linuxFlag(false);
    bool rhLinuxFlag(false);
    QDir dir(appDirPath);
    if (dir.dirName() == "exe_linux64") {
        linuxFlag = true;
    }
    else if (dir.dirName() == "exe_rh_linux64") {
        rhLinuxFlag = true;
    }
    else {
        throw CaretException("The directory containing the wb_view executable should be either "
                             "exe_linux64 or exe_rh_linux64 but wb_view is in "
                             + appDirPath.absolutePath());
    }
    moveUp(dir);  /* Moves to 'workbench' directory but user may have renamed it */
    
    if (linuxFlag) {
        moveDown(dir, "bin_linux64");
    }
    else if (rhLinuxFlag) {
        moveDown(dir, "bin_rh_linux64");
    }
    else {
        CaretAssert(0);
    }
    
    const AString binDirectoryName(dir.absolutePath());
    CaretAssert( ! binDirectoryName.isEmpty());

    AString errorMessage;
    const FunctionResult scriptsResult(binDirectoryContainsScripts(binDirectoryName));
    if (scriptsResult.isError()) {
        errorMessage = scriptsResult.getErrorMessage();
    }
    return FunctionResultString(binDirectoryName,
                                errorMessage,
                                errorMessage.isEmpty());
}
/**
 * Find the bin directory using the path of wb_view for macOS
 * @return FunctionResult with path or error information.
 * @param appDirPath
 *    Directory containing the application (wb_view)
 */
FunctionResultString
WorkbenchInstallationAssistant::findBinDirectoryMacos(const QDir& appDirPath) const
{
    /*
     * For DMG:
     * App:     wb_view.app/Contents/MacOS/wb_view
     * Scripts: wb_view.app/Contents/usr/bin/wb_command
     *
     * For ZIP:
     * App:     workbench/macosxub_apps/wb_view.app/Contents/MacOS/wb_view
     * Scripts: workbench/bin_macosxub/wb_command
     */
    bool zipDistributionFlag(true);
    
    AString binDirectoryName;
    
    QDir dir(appDirPath);
    moveUp(dir); /* Moves to 'Contents' directory */
    moveUp(dir); /* Moves to 'wb_view.app' directory */
    if (dir.dirName() == "wb_view.app") {
        /*
         * Could be DMG
         */
        try {
            /*
             * moveDown() will throw exception if it fails
             */
            QDir dirDmgTest(dir);
            moveDown(dirDmgTest, "Contents");
            moveDown(dirDmgTest, "usr");
            moveDown(dirDmgTest, "bin");
            binDirectoryName = dirDmgTest.absolutePath();
            zipDistributionFlag = false;
        }
        catch (const CaretException& /*caretException*/) {
            /*
             * Ignore.  If we are here it is not a DMG
             * distribution to continue testing for
             * ZIP distribution.
             */
        }
    }
    else {
        throw CaretException("Moved up two directories from "
                             + appDirPath.absolutePath()
                             + " and should be in directory wb_view.app but are in directory: "
                             + dir.absolutePath());
    }
    
    if (zipDistributionFlag) {
        try {
            moveUp(dir); /* Moves to 'macosxub_apps' directory */
            moveUp(dir); /* Moves to 'workbench' directory but user may have renamed it */
            moveDown(dir, "bin_macosxub");
            binDirectoryName = dir.absolutePath();
        }
        catch (const CaretException& caretException) {
            throw CaretException("Should be in directory 'bin_macosxub' but are in directory "
                                 + dir.absolutePath()
                                 + ".  Started in "
                                 + appDirPath.absolutePath());
        }
    }
    
    CaretAssert( ! binDirectoryName.isEmpty());
    
    AString errorMessage;
    const FunctionResult scriptsResult(binDirectoryContainsScripts(binDirectoryName));
    if (scriptsResult.isError()) {
        errorMessage = scriptsResult.getErrorMessage();
    }
    return FunctionResultString(binDirectoryName,
                                errorMessage,
                                errorMessage.isEmpty());
}
/**
 * Find the bin directory using the path of wb_view for Windows
 * @return FunctionResult with path or error information.
 * @param appDirPath
 *    Directory containing the application (wb_view)
 */
FunctionResultString
WorkbenchInstallationAssistant::findBinDirectoryWindows(const QDir& appDirPath) const
{
    /*
     * Scripts are in same directory as executables on Windows
     */
    const AString binDirectoryName(appDirPath.absolutePath());
    AString errorMessage;
    const FunctionResult scriptsResult(binDirectoryContainsScripts(binDirectoryName));
    if (scriptsResult.isError()) {
        errorMessage = scriptsResult.getErrorMessage();
    }
    return FunctionResultString(binDirectoryName,
                                errorMessage,
                                errorMessage.isEmpty());
}

/**
 * Move down into the given subdirectory
 * @param dir
 *    The directory that is modified
 * @param subDirectoryName
 *    Name of subdirectory to move into
 * @throws
 *    CaretException if moving up fails
 */
void
WorkbenchInstallationAssistant::moveDown(QDir& dir,
                                         const AString& subDirectoryName) const
{
    /*
     * Fails if directory about does not exist
     */
    const AString oldPath(dir.absolutePath());
    if ( ! dir.cd(subDirectoryName)) {
        throw CaretException("Failed to cd() from "
                             + dir.absolutePath()
                             + " into subdirectory "
                             + subDirectoryName);
    }
    
    if (m_debugFlag) {
        std::cout << "Moved down from " << oldPath << std::endl;
        std::cout << "   to " << dir.absolutePath().toStdString() << std::endl;
    }
}

/**
 * Move up one directory level with the given directory
 * @param dir
 *    The directory that is modified
 * @throws
 *    CaretException if moving up fails
 */
void
WorkbenchInstallationAssistant::moveUp(QDir& dir) const
{
    /*
     * Fails if directory about does not exist
     */
    const AString oldPath(dir.absolutePath());
    if ( ! dir.cdUp()) {
        throw CaretException("Failed to cdUp() from "
                             + dir.absolutePath());
    }
    
    if (m_debugFlag) {
        std::cout << "Moved up from " << oldPath << std::endl;
        std::cout << "   to " << dir.absolutePath().toStdString() << std::endl;
    }
}


/**
 * Verify the given path name exists.
 * @param pathName
 *    The path name
 * @param description
 *    Description of path included in the throw error message
 * @throws
 *    CaretException if the path does not exist
 */
void
WorkbenchInstallationAssistant::verifyExists(const AString& pathName,
                                             const AString& description) const
{
    if ( ! QFileInfo::exists(pathName)) {
        throw CaretException(description
                             + " \""
                             + pathName
                             + "\" does not exist");
    }

}

/**
 * Verify the given directory exists.
 * @param dir
 *    The directrory
 * @param description
 *    Description of directory included in the throw error message
 * @throws
 *    CaretException if the directory does not exist
 */
void
WorkbenchInstallationAssistant::verifyExists(const QDir& dir,
                                             const AString& description) const
{
    if ( ! dir.exists()) {
        throw CaretException(description
                             + " \""
                             + dir.absolutePath()
                             + "\" does not exist");
    }
}

/**
 * Verify that the given directory contains wb_view and wb_command
 * @param binDirectroryName
 *    Name of the bin directory
 * @return
 *   True if the directory contains both wb_view and wb_command
 */
FunctionResult
WorkbenchInstallationAssistant::binDirectoryContainsScripts(const AString& binDirectoryName) const
{
    CaretAssert( ! binDirectoryName.isEmpty());
    
    QDir dir(binDirectoryName);
    CaretAssert(dir.exists());
    
    if (dir.exists("wb_view")
        && dir.exists("wb_command")) {
        return FunctionResult::ok();
    }
    return FunctionResult::error(binDirectoryName
                                 + " is missing one or both of wb_view and wb_command");
}



