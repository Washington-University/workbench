#ifndef __COMMAND_CLASS_CREATE_BASE_H__
#define __COMMAND_CLASS_CREATE_BASE_H__

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


#include "CommandOperation.h"

namespace caret {

    /**
     * Base class for creating class files.
     */
    class CommandClassCreateBase : public CommandOperation {
        
    protected:
        CommandClassCreateBase(const AString& commandLineSwitch,
                               const AString& operationShortDescription);
        
    public:
        virtual ~CommandClassCreateBase();

    private:
        
        CommandClassCreateBase(const CommandClassCreateBase&);

        CommandClassCreateBase& operator=(const CommandClassCreateBase&);

    protected:
        enum Access {
            ACCESS_PRIVATE,
            ACCESS_PROTECTED,
            ACCESS_PUBLIC
        };
        
        enum MethodType {
            METHOD_NORMAL,
            METHOD_CONST,
            METHOD_STATIC
        };
        
        void start(const AString& className,
                   const AString& derivedFromClassName,
                   const std::vector<AString> includeFileNames,
                   const std::vector<AString> forwardDeclaredClassNames) throw (CommandException);
        
        void finish() throw (CommandException);
        
        void writeAccess(const Access access);
        
        void writeConstructor(const AString& parameter1 = "",
                              const AString& parameter2 = "",
                              const AString& parameter3 = "",
                              const AString& parameter4 = "",
                              const AString& parameter5 = "",
                              const AString& parameter6 = "",
                              const AString& parameter7 = "",
                              const AString& parameter8 = "",
                              const AString& parameter9 = "",
                              const AString& parameter10 = "");
        
        void writeDestructor();
        
        void writeCopyConstructorAndAssignmentOperator(const bool allowed);
        
        void writeMethod(const MethodType methodType,
                         const AString& returnType,
                         const AString& methodName,
                         const AString& methodCode,
                         const AString& parameter1 = "",
                         const AString& parameter2 = "",
                         const AString& parameter3 = "",
                         const AString& parameter4 = "",
                         const AString& parameter5 = "",
                         const AString& parameter6 = "",
                         const AString& parameter7 = "",
                         const AString& parameter8 = "",
                         const AString& parameter9 = "",
                         const AString& parameter10 = "");
        
        void writeMember(const AString& dataType,
                         const AString& memberName,
                         const bool isStatic = false,
                         const AString& initialStaticValue = "");
        
        void writeHeaderComment(const AString& parameter1 = "",
                                const AString& parameter2 = "",
                                const AString& parameter3 = "",
                                const AString& parameter4 = "",
                                const AString& parameter5 = "",
                                const AString& parameter6 = "",
                                const AString& parameter7 = "",
                                const AString& parameter8 = "",
                                const AString& parameter9 = "",
                                const AString& parameter10 = "");

        void writeImplementationComment(const AString& parameter1 = "",
                                        const AString& parameter2 = "",
                                        const AString& parameter3 = "",
                                        const AString& parameter4 = "",
                                        const AString& parameter5 = "",
                                        const AString& parameter6 = "",
                                        const AString& parameter7 = "",
                                        const AString& parameter8 = "",
                                        const AString& parameter9 = "",
                                        const AString& parameter10 = "");
        
        void header(const AString& text);
        
        void imp(const AString& text);
        
        void addCopyright(AString& text);
        
        void createFileNames() throw (CommandException);
        
        AString createMethodComment(const AString& returnValue,
                                    const AString& methodName,
                                    const std::vector<AString>& parameters);
        
        AString className;
        
        AString derivedFromClassName;
        
        AString headerText;
        
        int32_t headerIndentation;
        
        AString implementationText;
        
        int32_t implementationIndentation;
        
        int32_t spacing;
        
        AString ifndefName;
        
        AString ifdefNameStaticDelarations;
        
        AString headerFileName;
        
        AString implementationFileName;
        
        std::vector<AString> staticMemberDeclarations;
        
        bool havePreviousAccessModifier;
        
        Access lastAccessValue;
    };
    
} // namespace

#endif // __COMMAND_CLASS_CREATE_BASE_H__
