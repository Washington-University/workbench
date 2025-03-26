#ifndef __WORKBENCH_INSTALLATION_ASSISTANT_H__
#define __WORKBENCH_INSTALLATION_ASSISTANT_H__

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



#include <memory>

#include "CaretObject.h"
#include "FunctionResult.h"

class QDir;

namespace caret {

    class WorkbenchInstallationAssistant : public CaretObject {
        
    public:
        WorkbenchInstallationAssistant();
        
        virtual ~WorkbenchInstallationAssistant();
        
        FunctionResultString findBinDirectory() const;
        
        FunctionResultString testBinDirectoryInUsersPath(const AString& binDirectory) const;
        
        FunctionResultString getShellPathUpdateInstructions() const;
        
        static void setApplicationFilePath(const AString& applicationFilePath);
        
    private:
        WorkbenchInstallationAssistant(const WorkbenchInstallationAssistant&) = delete;

        WorkbenchInstallationAssistant& operator=(const WorkbenchInstallationAssistant&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        enum class OperatingSystem {
            OS_UNKNOWN,
            OS_LINUX,
            OS_MACOSX,
            OS_WINDOWS
        };
        
        void moveDown(QDir& dir,
                      const AString& subDirectoryName) const;
        
        void moveUp(QDir& dir) const;
        
        void verifyExists(const AString& pathName,
                          const AString& description) const;
        
        void verifyExists(const QDir& dir,
                          const AString& description) const;
        
        FunctionResult binDirectoryContainsScripts(const AString& binDirectoryName) const;
        
        FunctionResultString findBinDirectoryLinux(const QDir& appDirPath) const;
        
        FunctionResultString findBinDirectoryMacos(const QDir& appDirPath) const;
        
        FunctionResultString findBinDirectoryWindows(const QDir& appDirPath) const;
        
        OperatingSystem m_operatingSystem = OperatingSystem::OS_UNKNOWN;
        
        bool m_debugFlag = false;
        
        AString m_pathSeparator = ":";
        
        static AString s_applicationFilePath;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_INSTALLATION_ASSISTANT_DECLARE__
    AString WorkbenchInstallationAssistant::s_applicationFilePath;
#endif // __WORKBENCH_INSTALLATION_ASSISTANT_DECLARE__

} // namespace
#endif  //__WORKBENCH_INSTALLATION_ASSISTANT_H__
