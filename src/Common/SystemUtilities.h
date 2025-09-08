#ifndef __SYSTEMUTILITIES_H__
#define __SYSTEMUTILITIES_H__

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

#include <memory>
#include <vector>
#include <stdint.h>

#include <AString.h>

#include "FunctionResult.h"

namespace caret {


    class InfoItem;
    class SystemBacktrace;
/**
 * Methods to help out with files and directories.
 */
class SystemUtilities {

private:
    SystemUtilities();

public:
    virtual ~SystemUtilities();

public:
    static AString getBackTrace();
    
    static void getBackTrace(std::vector<AString>& backTraceOut);

    static void getBackTrace(SystemBacktrace& backTraceOut);

    static AString getTempDirectory();

    static AString getUserName();

    static AString getYear();
    
    static AString getDate();

    static AString getTime();

    static AString getDateAndTime();

    static bool isWindowsOperatingSystem();

    static bool isMacOperatingSystem();

    static int32_t getNumberOfProcessors();

    static AString createUniqueID();
    
    static void unitTest(std::ostream& stream,
                         const bool isVerbose);

    static bool testRelativePath(
                    const AString& otherPath,
                    const AString& myPath,
                    const AString& correctResult);

    static AString relativePath(
                    const AString& otherPathIn,
                    const AString& myPathIn);

    static void setUnexpectedHandler();
    
    static void setNewHandler();
    
    static AString systemCurrentDirectory();
    
    static void sleepSeconds(const float numberOfSeconds);

    static AString getWorkbenchHome();
    
    static AString getExecutableName();
    
    static AString getLocalHostName();
    
    static std::vector<std::unique_ptr<InfoItem>> getSystemInfo();
    
    static FunctionResultString getEnvironmentVariable(const AString& name);
};

} // namespace

#endif // __SYSTEMUTILITIES_H__
