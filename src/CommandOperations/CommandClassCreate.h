#ifndef __COMMAND_CLASS_CREATE__H__
#define __COMMAND_CLASS_CREATE__H__

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


#include "CommandClassCreateBase.h"

namespace caret {

    /// Command that creates class files.
    class CommandClassCreate : public CommandClassCreateBase {
        
    public:
        CommandClassCreate();
        
        virtual ~CommandClassCreate();

        virtual void executeOperation(ProgramParameters& parameters) 
            throw (CommandException,
                   ProgramParametersException);
        
    private:
        
        CommandClassCreate(const CommandClassCreate&);

        CommandClassCreate& operator=(const CommandClassCreate&);
        
        
        void createHeaderFile(const AString& outputFileName,
                              const AString& className,
                              const AString& derivedFromClassName,
                              const AString& ifdefName,
                              const AString& ifdefNameStaticDeclaration,
                              const bool hasCopyAndAssignment);
        
        void createImplementationFile(const AString& outputFileName,
                                      const AString& className,
                                      const AString& derivedFromClassName,
                                      const AString& ifdefNameStaticDeclaration,
                                      const bool hasCopyAndAssignment);
    };
    
} // namespace

#endif // __COMMAND_CLASS_CREATE_H__
