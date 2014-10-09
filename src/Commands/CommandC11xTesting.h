#ifndef __COMMAND_C11X_TESTING__H__
#define __COMMAND_C11X_TESTING__H__

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


#include "CommandOperation.h"

namespace caret {

    /// Command that adds members and getter/setter methods to a class
    class CommandC11xTesting : public CommandOperation {
        
    public:
        CommandC11xTesting();
        
        virtual ~CommandC11xTesting();

        virtual void executeOperation(ProgramParameters& parameters);
            
        AString getHelpInformation(const AString& /*programName*/);
        
    private:
        
        CommandC11xTesting(const CommandC11xTesting&);

        CommandC11xTesting& operator=(const CommandC11xTesting&); 
        
    };
    
#ifdef WORKBENCH_HAVE_C11X
    class Cpp11xTesting {
        
    public:
        Cpp11xTesting();
        
        Cpp11xTesting(const int value);
        
        // Intel does not suport 'noexcept' keyword
        //virtual void methodName() noexcept;
        
        virtual ~Cpp11xTesting();
        
        void test();
        
    private:
        Cpp11xTesting(const Cpp11xTesting&);
        
        Cpp11xTesting& operator=(const Cpp11xTesting&);
        
        /*
         * Initialization of a member's value.
         */
        int m_x = 5;
    };
    
    class SubClass : public Cpp11xTesting {
        
        SubClass() : Cpp11xTesting(5) { }
        
        virtual ~SubClass() { }
        
        // Intel does not support 'override' keyword
        //virtual void methodName() override noexcept;
    };
#endif // WORKBENCH_HAVE_C11X


} // namespace

#endif // __COMMAND_C11X_TESTING__H__
