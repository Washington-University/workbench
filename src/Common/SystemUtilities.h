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

#include <QString>

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
    static QString getBackTrace();
    
    static QString getCurrentDirectory();

    static void setCurrentDirectory(const QString& path);

    static QString getTempDirectory();

    static QString getUsersHomeDirectory();

    static QString getUserName();

    static QString getDate();

    static QString getTime();

    static QString getDateAndTime();

    static QString getOperatingSystemName();

    static bool isWindowsOperatingSystem();

    static bool isMacOperatingSystem();

    static int32_t getNumberOfProcessors();

    static QString basename(const QString& name);

    static QString dirname(const QString& path);

    static QString getFileExtension(const QString& path);

    static QString createUniqueID();
    
    static void unitTest();

    static bool testRelativePath(
                    const QString& otherPath,
                    const QString& myPath,
                    const QString& correctResult);

    static QString relativePath(
                    const QString& otherPathIn,
                    const QString& myPathIn);

    static void deleteAllFilesInDirectory(
                    const QString& directoryPath,
                    const bool deleteDirectoryFlag);

    static QString createFileAbsolutePath(
                    const QString& directoryName,
                    const QString& fileName);
    
};

} // namespace

#endif // __SYSTEMUTILITIES_H__
