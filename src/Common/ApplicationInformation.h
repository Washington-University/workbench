#ifndef __APPLICATION_INFORMATION__H_
#define __APPLICATION_INFORMATION__H_

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

#include "ApplicationTypeEnum.h"
#include "CaretObject.h"
#include "WorkbenchSpecialVersionEnum.h"

namespace caret {

    class ApplicationInformation : public CaretObject {
        
    public:
        ApplicationInformation();
        
        virtual ~ApplicationInformation();
        
        AString getName() const;
        
        AString getNameForGuiLabel() const;
        
        AString getVersion() const;
        
        AString getCommit() const;
        
        AString getCommitDate() const;
        
        void getAllInformation(std::vector<AString>& informationValues) const;
        
        AString getAllInformationInString(const AString& separator) const;
        
        AString getSummaryInformationInString(const AString& separator) const;
        
        AString getCompiledWithDebugStatus() const;
        
        WorkbenchSpecialVersionEnum::Enum getWorkbenchSpecialVersion() const;
        
        static ApplicationTypeEnum::Enum getApplicationType();
        
        static void setApplicationType(const ApplicationTypeEnum::Enum applicationType);
        
    private:
        ApplicationInformation(const ApplicationInformation&);

        ApplicationInformation& operator=(const ApplicationInformation&);
        
        AString name;
        
        AString nameForGuiLabel;
        
        AString version;
        
        AString commit;
        
        AString commitDate;
        
        AString compiledWithDebugOn;
        
        AString operatingSystemName;
        
        AString compiledWithOpenMP;

        AString numberOfThreadsOpenMP;
        
        AString maximumNumberOfThreadsOpenMP;

        WorkbenchSpecialVersionEnum::Enum workbenchSpecialVersion;
        
        static ApplicationTypeEnum::Enum s_applicationType;
    };
    
#ifdef __APPLICATION_INFORMATION_DECLARE__
    ApplicationTypeEnum::Enum ApplicationInformation::s_applicationType = ApplicationTypeEnum::APPLICATION_TYPE_INVALID;
#endif // __APPLICATION_INFORMATION_DECLARE__

} // namespace
#endif  //__APPLICATION_INFORMATION__H_
