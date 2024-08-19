#ifndef __COMMAND_CLASS_CREATE_ENUM_H__
#define __COMMAND_CLASS_CREATE_ENUM_H__

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


#include "CommandClassCreateBase.h"

namespace caret {

    /// Command that creates class files for an enumerated type.
    class CommandClassCreateEnum : public CommandClassCreateBase {
        
    public:
        CommandClassCreateEnum();
        
        virtual ~CommandClassCreateEnum();

        virtual void executeOperation(ProgramParameters& parameters);
        
        AString getHelpInformation(const AString& /*programName*/);
            
    private:
        
        CommandClassCreateEnum(const CommandClassCreateEnum&);

        CommandClassCreateEnum& operator=(const CommandClassCreateEnum&);
        
        void createHeaderFile(const AString& outputFileName,
                              const AString& enumClassName,
                              const AString& ifdefName,
                              const AString& ifdefNameStaticDeclaration,
                              const int32_t numberOfEnumValues,
                              const std::vector<AString>& enumValueNames,
                              const bool isAutoNumber);
        
        void createImplementationFile(const AString& outputFileName,
                                      const AString& enumClassName,
                                      const AString& ifdefNameStaticDeclaration,
                                      const int32_t numberOfEnumValues,
                                      const std::vector<AString>& enumValueNames,
                                      const bool isAutoNumber,
                                      const AString description);
        
        AString getEnumComboBoxTemplateHelpInfo(const AString& enumClassName) const;
        
        AString createGuiNameFromName(const AString& name) const;
    };
    
} // namespace

#endif // __COMMAND_CLASS_CREATE_ENUM_H__
