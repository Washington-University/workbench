/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <cstdlib>
#include <iostream>
#include <sstream>
#ifdef CARET_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <QDateTime>
#include <QDir>
#include <QHostInfo>
#include <QThread>
#include <QUuid>

#include "CaretOMP.h"

#ifndef _WIN32
#include "execinfo.h"
#else
#include "Windows.h"
#endif

#include "CaretCommandLine.h"
#include "CaretLogger.h"
#include "SystemUtilities.h"

using namespace caret;

AString commandLine;//used to store the command line for output by unexpected handler

/**
 * Constructor.
 */
SystemUtilities::SystemUtilities()
{
}

/**
 * Destructor
 */
SystemUtilities::~SystemUtilities()
{
}

#include <QtCore>

/**
 * Get the backtrace in a string with each frame
 * separated by a newline character.
 *
 * @return
 *   String containing the backtrace.
 */
AString 
SystemUtilities::getBackTrace()
{
/*
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
*/    
    std::vector<AString> backTrace;
    SystemUtilities::getBackTrace(backTrace);
    std::stringstream str;
    for (std::vector<AString>::const_iterator iter = backTrace.begin();
         iter != backTrace.end();
         iter++) {
        str << *iter << std::endl;
    }
    return AString::fromStdString(str.str());
}

/**
 * Get the backtrace with the frames in a vector of strings.
 *
 * @param backTraceOut
 *    Vector of string containg the call stack.
 */
void 
SystemUtilities::getBackTrace(std::vector<AString>& backTraceOut)
{
    backTraceOut.clear();
    
#ifdef CARET_OS_WINDOWS
#else  // CARET_OS_WINDOWS
    void* callstack[1024];
    int numFrames = backtrace(callstack, 1024);
    char** symbols = backtrace_symbols(callstack, numFrames);
    for (int i = 0; i < numFrames; i++) {
        backTraceOut.push_back(symbols[i]);
    }
    free(symbols);
#endif // CARET_OS_WINDOWS
}

void SystemUtilities::getBackTrace(SystemBacktrace& backTraceOut)
{
#ifdef CARET_OS_WINDOWS
#else  // CARET_OS_WINDOWS
    backTraceOut.m_numFrames = backtrace(backTraceOut.m_callstack, 1024);
#endif // CARET_OS_WINDOWS
}

SystemBacktrace::SystemBacktrace()
{
#ifndef CARET_OS_WINDOWS
    m_numFrames = 0;
#endif
}

/**
 * @return String containing the backtrace symbols.
 */
AString SystemBacktrace::toSymbolString() const
{
    std::stringstream str;
#ifdef CARET_OS_WINDOWS
#else  // CARET_OS_WINDOWS
    char** symbols = backtrace_symbols(m_callstack, m_numFrames);
    for (int i = 0; i < m_numFrames; ++i)
    {
        str << symbols[i] << std::endl;
    }
    free(symbols);
#endif // CARET_OS_WINDOWS
    return AString::fromStdString(str.str());
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
    return QDir::tempPath();
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
#ifdef CARET_OS_WINDOWS
    const QString name(getenv("USERNAME"));
#else // CARET_OS_WINDOWS
    QString name(getlogin());
    if (name.isEmpty()) {
        name = getenv("USERNAME");
    }
#endif // CARET_OS_WINDOWS
    return name;
}

/**
 * @return The four digit year.
 */
AString 
SystemUtilities::getYear()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    AString s = dateTime.toString("yyyy");
    return s;
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
    QDateTime dateTime = QDateTime::currentDateTime();
    AString s = dateTime.toString("yyyy:MM:dd");
    return s;
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
    QDateTime dateTime = QDateTime::currentDateTime();
    AString s = dateTime.toString("hh:mm:ss");
    return s;
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
    AString s = (SystemUtilities::getDate()
                 + "T"
                 + SystemUtilities::getTime());
    return s;
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
#ifdef CARET_OS_WINDOWS
    return true;
#endif
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
#ifdef CARET_OS_MACOSX
    return true;
#endif
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
#ifdef CARET_OMP
    return omp_get_num_procs();
#endif
    return 1;
}

/**
 * Unit testing of assertions.
 * 
 * @param stream
 *    Stream to which messages are written.
 * @param isVerbose
 *    Print detailed messages.
 */
void 
SystemUtilities::unitTest(std::ostream& stream,
                         const bool /*isVerbose*/)
{
#ifdef NDEBUG
    stream << "Unit testing of CaretAssertion will not take place since software is not compiled with debug on." << std::endl;
    return;
#endif
    
    stream << "SystemUtilities::unitTest is starting" << std::endl;
    
    /*
     * Redirect std::err to the string stream.
     */
    std::ostringstream str;
    std::streambuf* cerrSave = std::cerr.rdbuf();
    std::cerr.rdbuf(str.rdbuf());
    
    testRelativePath("/surface02/john/caret_data/HUMAN.COLIN.ATLAS",
                     "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM",
                     "..");
    
    testRelativePath("/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM",
                     "/surface02/john/caret_data/HUMAN.COLIN.ATLAS",
                     "RIGHT_HEM");
    
    testRelativePath("/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM/subdir",
                     "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM",
                     "subdir");
    
    testRelativePath("/surface02/john/caret_data/HUMAN.COLIN.ATLAS/LEFT_HEM/subdir",
                     "/surface02/john/caret_data/HUMAN.COLIN.ATLAS/RIGHT_HEM",
                     "../LEFT_HEM/subdir");
    
    testRelativePath("root:/var/etc",
                     "remove:/usr/local",
                     "root:/var/etc");
    
//    testRelativePath("/Volumes/DS4600/caret7_gui_design/data/HCP_demo/Glasser_PilotIII.L.very_inflated.20k_fs_LR.surf.gii",
//                     "/Volumes/DS4600/caret7_gui_design/data/HCP_demo/border.spec",
//                     "border.spec");
    
    /*
     * Restore std::err
     */
    std::cerr.rdbuf(cerrSave);
    
    stream << "SystemUtilities::unitTest has ended" << std::endl << std::endl;;
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
    bool correctFlag = false;
    
    AString result = relativePath(otherPath, myPath);
    if (result == correctResult) {
        correctFlag = true;
    }
    else {
        std::cerr << "SystemUtilities.relativePath() failed:" << std::endl;
        std::cerr << "   otherPath: " + otherPath << std::endl;
        std::cerr << "   myPath:    " + myPath << std::endl;
        std::cerr << "   result:    " + result << std::endl;
        std::cerr << "   correct:   " + correctResult << std::endl;
    }
    
    return correctFlag;
}

/**
 * @return A Universally Unique Identifier (UUID).
 */
AString
SystemUtilities::createUniqueID()
{
    const AString uuid = QUuid::createUuid().toString();
    return uuid;
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
    AString result = otherPathIn;
    
    //
    // Check for either path being empty
    //
    if (otherPathIn.isEmpty() || myPathIn.isEmpty()) {
        return result;
    }
    
#ifdef CARET_OS_WINDOWS
    //
    // Both paths must be absolute paths
    //
    if (otherPathIn.indexOf(":") < 0) {
        return result;
    }
    if (myPathIn.indexOf(":") < 0) {
        return result;
    }
#else
    //
    // Both paths must be absolute paths
    //
    if ((otherPathIn[0] != '/') || (myPathIn[0] != '/')) {
        return result;
    }
#endif
    
    QStringList otherPath = QDir::cleanPath(otherPathIn).split(QRegExp("[/\\\\]"), 
                                                               QString::SkipEmptyParts);
    QStringList myPath = QDir::cleanPath(myPathIn).split(QRegExp("[/\\\\]"), 
                                                         QString::SkipEmptyParts);    
    
    const unsigned int minLength = std::min(myPath.size(), otherPath.size());
    
    int sameCount = 0;
    for (unsigned int i = 0; i < minLength; i++) {
        if (myPath[i] == otherPath[i]) {
            //cout << "Match: |" << myPath[i] << "|" << std::endl;
            sameCount++;
        }
        else {
            break;
        }
    }
    //cout << "same count: " << sameCount << std::endl;
    
    //
    // Is root of both paths different
    //
    if (sameCount == 0) {
        result = otherPathIn;
    }

    /*
     * Always use a forward slash for the separator, even on Windows,
     * so that paths are valid on Unix (Qt does not handle backslashes
     * properly on Unix)
     */
    const AString separatorCharacter("/");
    
    //
    // Is other path a subdirectory of mypath
    //
    if (sameCount == myPath.size()) {
        result = "";
        for (int j = sameCount; j < otherPath.size(); j++) {
            result.append(otherPath[j]);
            if (j < (otherPath.size() - 1)) {
                result.append(separatorCharacter);
            }
        }
    }
    
    //
    // otherpath is above this one
    //
    result = "";
    for (int j = sameCount; j < myPath.size(); j++) {
        result.append("..");
        if (j < (myPath.size() - 1)) {
            result.append(separatorCharacter);
        }
    }
    for (int k = sameCount; k < otherPath.size(); k++) {
        if (result.isEmpty() == false) {
            result.append(separatorCharacter);
        }
        result.append(otherPath[k]);
    }   
    
    return result;
}

/**
 * Unexpected handler
 */
static void unexpectedHandler()
{
    std::cerr << "While running '" << caret_global_commandLine << "':" << std::endl;
    std::cerr << std::endl;
    std::cerr << "ERROR: unhandled exception." << std::endl;
    //if (theMainWindow != NULL) {
        const AString msg("Workbench will be terminating due to an unexpected exception.\n"
                          "abort() will be called and a core file may be created.");
    std::cerr << msg << std::endl;
        //QMessageBox::critical(theMainWindow, "ERROR", msg);
    //}
    
    std::cerr << SystemUtilities::getBackTrace() << std::endl;
    
    abort();
}

/**
 * New handler
 */
static void newHandler()
{
    std::cerr << "While running '" << caret_global_commandLine << "':" << std::endl;
    std::ostringstream str;
    str << "\n"
    << "OUT OF MEMORY\n"
    << "\n"
    << "This means that Workbench is unable to get memory that it needs.\n"
    << "Possible causes:\n"
    << "   (1) Your computer lacks sufficient RAM.\n"
    << "   (2) Swap space is too small (you might increase it).\n"
    << "   (3) Your computer may be using an non-English character \n"
    << "       set.  Try switching to the English character set.\n"
    << "\n";
    std::cerr << str.str().c_str() << std::endl;
    
    std::cerr << SystemUtilities::getBackTrace() << std::endl;
    
    abort();
    
    //if (theMainWindow != NULL) {
    //    QMessageBox::critical(theMainWindow, "OUT OF MEMORY",
    //                          "Out of memory, Caret terminating");
    //    std::exit(-1);
    //}
}

/**
 * Set the handler for an unexpected (uncaught) exception.
 */
void
SystemUtilities::setUnexpectedHandler()
{
    std::set_unexpected(unexpectedHandler);
}

/**
 * Set the handler for when "operator new" is unable to allocate memory.
 * This new handler will print a message to the terminal containing a
 * backtrace and then calls abort to end the program.
 *
 * NOTE: If this new handler is set, "operator new" WILL NOT
 * throw a std::bad_alloc exception.
 */
void
SystemUtilities::setNewHandler()
{
    std::set_new_handler(newHandler);
}
/**
 * Return the current directory as indicated 
 * by the system.  In most cases, use the
 * methods in Brain to get and set the current
 * directory since it may be possible to have
 * multiple Brains each of which has its current
 * directory set to the directory containing the
 * SpecFile that was read.
 *
 * @return The path of the current directory.
 */
AString 
SystemUtilities::systemCurrentDirectory()
{
    return QDir::currentPath(); 
}

/*
 * From http://developer.qt.nokia.com/wiki/How_to_create_a_splash_screen_with_an_induced_delay
 */ 
class Sleeper : public QThread
{
public:
    static void sleepSeconds(const float seconds) {
        if (seconds > 0.0) {
            const unsigned long milliseconds = seconds * 1000.0;
            QThread::msleep(milliseconds);
        }
    }
};

/**
 * Sleep for the specified number of seconds.  The minimum
 * is one millisecond (0.001).
 * @param numberOfSeconds
 *    Number of seconds to sleep.
 */
void 
SystemUtilities::sleepSeconds(const float numberOfSeconds)
{
    Sleeper::sleepSeconds(numberOfSeconds);
}

/**
 * Get the workbench home directory
 * @return workbenchHomeDirectory
 *    The path to the workbench installation
 */
AString
SystemUtilities::getWorkbenchHome()
{
    static QString workbenchHomeDirectory;
    if(workbenchHomeDirectory.isEmpty() == false)
    {
        return workbenchHomeDirectory;
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    workbenchHomeDirectory = env.value ( AString("WORKBENCH_HOME") );
    if (workbenchHomeDirectory.isEmpty()) {
            workbenchHomeDirectory = QCoreApplication::applicationDirPath();            
            if (workbenchHomeDirectory.isEmpty() == false) {                
#ifdef CARET_OS_MACOSX
                const bool appFlag = (workbenchHomeDirectory.indexOf(".app/") > 0);
                if (appFlag) {
                    QDir dir(workbenchHomeDirectory);
                    dir.cdUp();
                    dir.cdUp();
                    dir.cdUp();
                    workbenchHomeDirectory = dir.absolutePath();                    
                }
#endif
            }
        CaretLogFine("Workbench Home Directory: " + workbenchHomeDirectory);        
    }    
    
    return workbenchHomeDirectory  = QDir::toNativeSeparators(workbenchHomeDirectory);    
}

/**
 * @return The fully qualifed domain name for the local host.
 *
 *         If failure, an empty string is returned and a message
 *         is sent to the logger.
 * 
 *         If the address is a "loopback" (127.0.0.1) it typically 
 *         indicates that there is not a valid network connection
 *         and an empty string is returned and a message is logged.
 */
AString
SystemUtilities::getLocalHostName()
{
    /*
     * NOTE: THIS FUNCTION DOES NOT SEEM TO WORK ON MACOS 10.14 MOJAVE.
     * THE LOCALHOST NAME IS <XXX>.LOCAL WHERE <XXX> IS THE COMPUTER'NAME
     * AND QHostInfo::fromName() DOES NOT MAKE IT FULLY QUALIFIED
     */
    QString hostName;
    
    QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    if (hostInfo.error() == QHostInfo::NoError) {
        for (auto address : hostInfo.addresses()) {
            if (address.isLoopback()) {
                CaretLogFine("Lookup of host found loopback address (localhost).  May not have valid network connection");
                return "";
            }
        }
        hostName = hostInfo.hostName();
    }
    else {
        CaretLogWarning("Lookup of local host name failed: "
                        + hostInfo.errorString());
    }
    
    return hostName;
}


