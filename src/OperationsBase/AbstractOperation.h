#ifndef __ABSTRACT_OPERATION_H__
#define __ABSTRACT_OPERATION_H__

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

//make it easy to use these in an algorithm class, don't just forward declare them
#include "ProgressObject.h"
#include "CaretAssert.h"
#include "OperationParameters.h"

namespace caret {

    class AbstractOperation
    {
    protected:
        AbstractOperation();
        virtual ~AbstractOperation();
    public:

        ///override these to allow operation parsers to use your operation without writing an explicit command class
        static OperationParameters* getParameters() { CaretAssert(false); return NULL; }

        ///override these to allow operation parsers to use your operation without writing an explicit command class
        static void useParameters(OperationParameters*, ProgressObject*) { CaretAssert(false); }
        
        ///override this to set the command switch
        static AString getCommandSwitch() { CaretAssert(false); return ""; }
        
        ///override this to set the short description
        static AString getShortDescription() { CaretAssert(false); return ""; }
        
        ///override this if the operation doesn't take parameters
        static bool takesParameters() { return true; }
    };

    ///interface class for use by operation parsers - used because the above interface has only static methods, so to avoid neededing to instantiate the operation or template the parser code
    struct AutoOperationInterface
    {
        virtual OperationParameters* getParameters() = 0;
        virtual void useParameters(OperationParameters* a, ProgressObject* b) = 0;
        virtual AString getCommandSwitch() = 0;
        virtual AString getShortDescription() = 0;
        virtual bool takesParameters() = 0;
        virtual ~AutoOperationInterface();
    };

    ///templated interface class to pass through to something that inherits from AbstractOperation (or implements equivalent functions)
    ///this makes it easier to create a bridge between the static methods of the operation and an interface pointer that a parser can store
    template<typename T>
    struct TemplateAutoOperation : public AutoOperationInterface
    {
        TemplateAutoOperation() { }
        OperationParameters* getParameters() { return T::getParameters(); }
        void useParameters(OperationParameters* a, ProgressObject* b) { T::useParameters(a, b); }
        AString getCommandSwitch() { return T::getCommandSwitch(); }
        AString getShortDescription() { return T::getShortDescription(); }
        bool takesParameters() { return T::takesParameters(); }
    };

    ///interface class for parsers to inherit from
    class OperationParserInterface
    {
        OperationParserInterface();//must take an interface object, for its vtable to the real operation, so deny default construction
    protected:
        AutoOperationInterface* m_autoOper;
    public:
        OperationParserInterface(AutoOperationInterface* myAutoOper) : m_autoOper(myAutoOper) { }
        virtual ~OperationParserInterface();
    };
}
#endif //__ABSTRACT_OPERATION_H__
