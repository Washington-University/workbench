#ifndef __SYSTEMUTILITIES_H__
#define __SYSTEMUTILITIES_H__

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


#include "CaretObject.h"


#include <stdint.h>

#include <AString.h>

namespace caret {

/**
 * Methods to help out with files and directories.
 */
class SystemUtilities : public CaretObject {

private:
    SystemUtilities();

public:
    virtual ~SystemUtilities();

public:
    static AString getBackTrace();
    
    static void getBackTrace(std::vector<AString>& backTraceOut);
    
    static AString getCurrentDirectory();

    static void setCurrentDirectory(const AString& path);

    static AString getTempDirectory();

    static AString getUsersHomeDirectory();

    static AString getUserName();

    static AString getDate();

    static AString getTime();

    static AString getDateAndTime();

    static AString getOperatingSystemName();

    static bool isWindowsOperatingSystem();

    static bool isMacOperatingSystem();

    static int32_t getNumberOfProcessors();

    static AString basename(const AString& name);

    static AString dirname(const AString& path);

    static AString getFileExtension(const AString& path);

    static AString createUniqueID();
    
    static void unitTest();

    static bool testRelativePath(
                    const AString& otherPath,
                    const AString& myPath,
                    const AString& correctResult);

    static AString relativePath(
                    const AString& otherPathIn,
                    const AString& myPathIn);

    static void deleteAllFilesInDirectory(
                    const AString& directoryPath,
                    const bool deleteDirectoryFlag);

    static AString createFileAbsolutePath(
                    const AString& directoryName,
                    const AString& fileName);
    
    static void setHandlersForUnexpected();
};

} // namespace

#endif // __SYSTEMUTILITIES_H__
