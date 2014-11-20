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

#include <fstream>
#include <ostream>
#include <iostream>

#include "CaretAssertion.h"
#include "CommandClassCreateOperation.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "ProgramParameters.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreateOperation::CommandClassCreateOperation()
: CommandClassCreateBase("-class-create-operation",
                         "CREATE SOURCE CODE CLASS FILES (.h, .cxx) FOR OPERATION")
{
    
}

/**
 * Destructor.
 */
CommandClassCreateOperation::~CommandClassCreateOperation()
{
    
}

AString 
CommandClassCreateOperation::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Create Operation Class header (.h) and implementation (.cxx) files.\n"
                        "\n"
                        "Usage:  <operation-class-name>\n"
                        "        <command-line-switch>\n"
                        "        <short-description>\n"
                        "        [-no-parameters]\n"
                        "\n"
                        "    operation-class-name\n"
                        "        Required name of the operation class that MUST start with \"Operation\"\n"
                        "    \n"
                        "    command-line-switch\n"
                        "        Required command line switch for operation.\n"
                        "    \n"
                        "    short-description\n"
                        "        Required short description within double quotes.\n"
                        "    \n"
                        "    -no-parameters\n"
                        "        Optional parameter if the operation does not use parameters.\n"
                        "    \n"
                        );
    return helpInfo; 
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
CommandClassCreateOperation::executeOperation(ProgramParameters& parameters)
{
    const AString operationClassName = parameters.nextString("Operation Class Name");
    const AString commandLineSwitch  = parameters.nextString("Command Line Switch");
    const AString shortDescription   = parameters.nextString("Short Description");
    bool hasParametersFlag = true;
    while (parameters.hasNext()) {
        const AString& param = parameters.nextString("Create Class Parameter");
        if (param == "-no-parameters") {
            hasParametersFlag = false;
        }
        else {
            throw CommandException("Invalid parameter: " + param);
        }
    }
    
    AString errorMessage;
    if (operationClassName.isEmpty()) {
        throw CommandException("Operation Class Name is empty.");
    }
    else {
        if (operationClassName.startsWith("Operation") == false) {
            throw CommandException("Operation Class Name must start with \"Operation\".\n");
        }
        if (operationClassName == "Operation") {
            throw CommandException("\"Operation\" is not allowed for Operation Class Name");
        }
    }
    
    if (commandLineSwitch.isEmpty()) {
        throw CommandException("Command line switch is empty.");
    }
    else if (commandLineSwitch.startsWith("-") == false) {
        throw CommandException("Command line must begin with \"-\".");
    }
    
    if (shortDescription.isEmpty()) {
        throw CommandException("Short description is empty.");
    }
    const AString headerFileName = operationClassName + ".h";
    const AString implementationFileName = operationClassName + ".cxx";
    
    FileInformation headerInfo(headerFileName);
    if (headerInfo.exists()) {
        errorMessage += headerFileName + " exists and this command will not overwrite it.\n";
    }
    FileInformation impInfo(implementationFileName);
    if (impInfo.exists()) {
        errorMessage += implementationFileName + " exists and this command will not overwrite it.\n";
    }
    
    if (errorMessage.isEmpty() == false) {
        throw CommandException(errorMessage);
    }    
    
    AString ifndefName;
    AString ifdefNameStaticDeclarations;
    this->getIfDefNames(operationClassName,
                        ifndefName, 
                        ifdefNameStaticDeclarations);
    
    this->createHeaderFile(headerFileName,
                           operationClassName,
                           ifndefName, 
                           hasParametersFlag);
    
    this->createImplementationFile(implementationFileName,
                                   operationClassName,
                                   commandLineSwitch,
                                   shortDescription);
}

/**
 * Create and write the header (.h) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param operationClassName
 *    Name of operation type class.
 * @param ifndefName
 *    Name of "ifndef" value.
 * @param hasParameters
 *    True if the operation has parameters.
 */
void 
CommandClassCreateOperation::createHeaderFile(const AString& outputFileName,
                                              const AString& operationClassName,
                                              const AString& ifndefName,
                                              const bool hasParameters)
{
    AString t;

    t += ("#ifndef " + ifndefName + "\n");
    t += ("#define " + ifndefName + "\n");
    t += this->getCopyright();
    t += ("\n");
    
    t += ("#include \"AbstractOperation.h\"\n");
    t += ("\n");
    t += ("namespace caret {\n");
    t += ("\n");
    t += ("    class " + operationClassName + " : public AbstractOperation {\n");
    t += ("\n");
    t += ("    public:\n");
    t += ("        static OperationParameters* getParameters();\n");
    t += ("\n");
    t += ("        static void useParameters(OperationParameters* myParams, \n");
    t += ("                                  ProgressObject* myProgObj);\n");
    t += ("\n");
    t += ("        static AString getCommandSwitch();\n");
    t += ("\n");
    t += ("        static AString getShortDescription();\n");
    t += ("\n");
    
    if (hasParameters == false) {
        t += ("        static bool takesParameters() { return false; }\n");
        t += ("\n");
    }
    
    t += ("    };\n");
    t += ("\n");
    t += ("    typedef TemplateAutoOperation<" + operationClassName + "> Auto" + operationClassName + ";\n");
    t += ("\n");
    t += ("} // namespace\n");
    t += ("\n");
    
    t += ("#endif  //" + ifndefName + "\n");
    t += ("\n");

    TextFile tf;
    tf.replaceText(t);
    
    try {
        tf.writeFile(outputFileName);
    }
    catch (const DataFileException& e) {
        throw CommandException(e);
    }
}

/**
 * Create and write the implementation (.cxx) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param operationClassName
 *    Name of operation type class.
 * @param commandLineSwitch
 *    Command line switch for operation.
 * @param shortDescription
 *    Short description of operation.
 */
void 
CommandClassCreateOperation::createImplementationFile(const AString& outputFileName,
                                                      const AString& operationClassName,
                                                      const AString& commandLineSwitch,
                                                      const AString& shortDescription)
{
    AString t;
    
    t += this->getCopyright();
    
    t += ("#include \"CaretAssert.h\"\n");
    t += ("#include \"CaretLogger.h\"\n");
    t += ("\n");
    t += ("#include \"" + operationClassName + ".h\"\n");
    t += ("#include \"OperationException.h\"\n");
    t += ("\n");
    t += ("using namespace caret;\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * \\class caret::" + operationClassName + " \n");
    t += (" * \\brief " + shortDescription + "\n");
    t += (" *\n");
    t += (" * <REPLACE-WITH-THOROUGH DESCRIPTION>\n");
    t += (" */\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * @return Command line switch\n");
    t += (" */\n");
    t += ("AString\n");
    t += ("" + operationClassName + "::getCommandSwitch()\n");
    t += ("{\n");
    t += ("    return \"" + commandLineSwitch + "\";\n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * @return Short description of operation\n");
    t += (" */\n");
    t += ("AString\n");
    t += ("" + operationClassName + "::getShortDescription()\n");
    t += ("{\n");
    t += ("    return \"" + shortDescription.toUpper() + "\";\n");
    t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * @return Parameters for operation\n");
    t += (" */\n");
    t += ("OperationParameters*\n");
    t += ("" + operationClassName + "::getParameters()\n");
    t += ("{\n");
    t += ("    OperationParameters* ret = new OperationParameters();\n");
    t += ("    \n");
    t += ("    AString helpText;\n");
    t += ("    \n");
    t += ("    ret->setHelpText(helpText);\n");
    t += ("    \n");
    t += ("    return ret;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Use Parameters and perform operation\n");
    t += (" */\n");
    t += ("void\n");
    t += ("" + operationClassName + "::useParameters(OperationParameters* myParams,\n");
    t += ("                                          ProgressObject* myProgObj)\n");
    t += ("{\n");
    t += ("    LevelProgress myProgress(myProgObj);\n");
    t += ("    \n");
    t += ("}\n");
    t += ("\n");
    
    TextFile tf;
    tf.replaceText(t);
    
    try {
        tf.writeFile(outputFileName);
    }
    catch (const DataFileException& e) {
        throw CommandException(e);
    }
    
    std::cout << std::endl;
    std::cout << "Operation was created successfully." << std::endl;
    std::cout << std::endl;
    std::cout << "Add the class files to Operations/CMakeLists.txt:" << std::endl;
    std::cout << "   " << qPrintable(operationClassName) << ".h" << std::endl;
    std::cout << "   " << qPrintable(operationClassName) << ".cxx" << std::endl;
    std::cout << std::endl;
    std::cout << "Add the header file and operation to Commands/CommandOperationManager.cxx:" << std::endl;
    std::cout << "   #include \"" << qPrintable(operationClassName) << ".h\"" << std::endl;
    std::cout << "   this->commandOperations.push_back(new CommandParser(new Auto" << operationClassName << "()));" << std::endl;
    std::cout << std::endl;
}

