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

#include <string>

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
    static std::string getBackTrace();
    
    static std::string getCurrentDirectory();

    static void setCurrentDirectory(const std::string& path);

    static std::string getTempDirectory();

    static std::string getUsersHomeDirectory();

    static std::string getUserName();

    static std::string getDate();

    static std::string getTime();

    static std::string getDateAndTime();

    static std::string getOperatingSystemName();

    static bool isWindowsOperatingSystem();

    static bool isMacOperatingSystem();

    static int32_t getNumberOfProcessors();

    static std::string basename(const std::string& name);

    static std::string dirname(const std::string& path);

    static std::string getFileExtension(const std::string& path);

    static std::string createUniqueID();
    
    static void unitTest();

    static bool testRelativePath(
                    const std::string& otherPath,
                    const std::string& myPath,
                    const std::string& correctResult);

    static std::string relativePath(
                    const std::string& otherPathIn,
                    const std::string& myPathIn);

    static void deleteAllFilesInDirectory(
                    const std::string& directoryPath,
                    const bool deleteDirectoryFlag);

    static std::string createFileAbsolutePath(
                    const std::string& directoryName,
                    const std::string& fileName);
    
};

} // namespace

#endif // __SYSTEMUTILITIES_H__
