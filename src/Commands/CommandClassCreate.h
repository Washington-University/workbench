#ifndef __COMMAND_CLASS_CREATE__H__
#define __COMMAND_CLASS_CREATE__H__

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

    /// Command that creates class files.
    class CommandClassCreate : public CommandClassCreateBase {
        
    public:
        CommandClassCreate();
        
        virtual ~CommandClassCreate();

        virtual void executeOperation(ProgramParameters& parameters);
            
        AString getHelpInformation(const AString& /*programName*/);
        
        /** This String is used by the command that adds new members to a class */
        static AString getNewMembersString() { return "// ADD_NEW_MEMBERS_HERE"; }
        
        /** This String is used by the command that adds new METHODS to a class */
        static AString getNewMethodsString() { return "// ADD_NEW_METHODS_HERE"; }
        
    private:
        
        CommandClassCreate(const CommandClassCreate&);

        CommandClassCreate& operator=(const CommandClassCreate&);
        
        
        void createHeaderFile(const AString& outputFileName,
                              const AString& className,
                              const AString& derivedFromClassName,
                              const AString& ifdefName,
                              const AString& ifdefNameStaticDeclaration,
                              const bool hasCopyAndAssignment,
                              const bool hasEqualityOperator,
                              const bool hasEventListener,
                              const bool hasSceneInterface,
                              const bool hasSubClassSceneSaving);
        
        void createImplementationFile(const AString& outputFileName,
                                      const AString& className,
                                      const AString& derivedFromClassName,
                                      const AString& eventTypeEnumName,
                                      const AString& ifdefNameStaticDeclaration,
                                      const bool hasCopyAndAssignment,
                                      const bool hasEqualityOperator,
                                      const bool hasEventListener,
                                      const bool hasSceneInterface,
                                      const bool hasSubClassSceneSaving);
    };
    
} // namespace

#endif // __COMMAND_CLASS_CREATE_H__
