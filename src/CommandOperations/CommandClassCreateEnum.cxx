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

#include <fstream>
#include <ostream>
#include <iostream>

#include "CaretAssertion.h"
#include "CommandClassCreateEnum.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreateEnum::CommandClassCreateEnum()
: CommandClassCreateBase("-class-create-enum",
                         "CREATE CLASS FOR ENUMERATED TYPE")
{
    
}

/**
 * Destructor.
 */
CommandClassCreateEnum::~CommandClassCreateEnum()
{
    
}

/**
 * Execute the operation.
 * 
 * @param parameters
 *   Parameters for the operation.
 * @throws CommandException
 *   If the command failed.
 * @throws ProgramParametersException
 *   If there is an error in the parameters.
 */
void 
CommandClassCreateEnum::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    const AString enumClassName = parameters.nextString("Enum Class Name");
    if (enumClassName.endsWith("Enum") == false) {
        throw CommandException("Name of class MUST end with \"Enum\".");
    }
    
    std::vector<AString> includeFileNames;
    includeFileNames.push_back("stdint.h");
    includeFileNames.push_back("vector");
    includeFileNames.push_back("AString.h");
    
    std::vector<AString> forwardDeclaredClassNames;
    
    this->start(enumClassName, 
                "", 
                includeFileNames, 
                forwardDeclaredClassNames);
    
    this->writeAccess(ACCESS_PUBLIC);
    
    this->writeHeaderComment("The enumerated values");
    this->header("enum Enum {");
    this->header("};");
    this->header("");
    
    this->writeAccess(ACCESS_PRIVATE);
    
    this->writeConstructor("const Enum e",
                           "const int32_t integerCode",
                           "const AString& name",
                           "const AString& guiName");
    
    this->writeDestructor();
    
    this->writeMethod(METHOD_STATIC,
                      "AString", 
                      "toName", 
                      "",
                      "Enum e");
    
    this->writeMethod(METHOD_STATIC,
                      "Enum", 
                      "fromName", 
                      "",
                      "const AString& s", 
                      "bool* isValidOut");

    this->writeMethod(METHOD_STATIC,
                      "AString", 
                      "toGuiName", 
                      "",
                      "Enum e");
    
    this->writeMethod(METHOD_STATIC,
                      "Enum", 
                      "fromGuiName", 
                      "",
                      "const AString& s", 
                      "bool* isValidOut");
    
    this->writeMethod(METHOD_STATIC,
                      "int32_t", 
                      "toIntegerCode", 
                      "",
                      "Enum e");
    
    this->writeMethod(METHOD_STATIC,
                      "Enum", 
                      "fromIntegerCode", 
                      "",
                      "const int32_t integerCode", 
                      "bool* isValidOut");
    
    this->writeAccess(ACCESS_PRIVATE);

    this->writeCopyConstructorAndAssignmentOperator(false);
    
    this->writeMethod(METHOD_STATIC,
                      "const " + enumClassName + "*",
                      "findData",
                      "",
                      "const Enum e");

    this->writeMethod(METHOD_STATIC,
                      "void",
                      "initialize",
                      "");
    
    this->writeMember("std::vector<" + enumClassName + ">", "enumData", true);
    
    this->writeMember("Enum", "e", false);
    
    this->writeMember("bool", "initializedFlag", true, "false");
    
    this->writeMember("int32_t", "integerCode", false);
    
    this->writeMember("AString", "name", false);
    
    this->writeMember("AString", "guiName", false);
    
    this->finish();
}


