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

#include "CaretAssertion.h"
#include "CommandClassCreate.h"
#include "FileInformation.h"
#include "ProgramParameters.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreate::CommandClassCreate()
: CommandClassCreateBase("-class-create",
                         "CREATE CLASS SOURCE CODE FILES (.h and .cxx)")
{
    
}

/**
 * Destructor.
 */
CommandClassCreate::~CommandClassCreate()
{
    
}

AString 
CommandClassCreate::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Create class header (.h) and implementation (.cxx) files.\n"
                        "\n"
                        "Usage:  <class-name> [-copy]\n"
                        "                     [-no-parent]\n"
                        "                     [-parent <parent-class-name>]\n"
                        "\n"
                        "Options: \n"
                        "    -copy\n"
                        "        Adds copy constructor and assignment operator\n"
                        "    \n"
                        "    -no-parent\n"
                        "        Created class is not derived from any other\n"
                        "        class.  By default, the parent class is\n"
                        "        CaretObject.\n"
                        "    \n"
                        "    -parent <parent-class-name>\n"
                        "        Specify the parent (derived from) class.\n"
                        "        By default, the parent class is CaretObject.\n"
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
CommandClassCreate::executeOperation(ProgramParameters& parameters) throw (CommandException,
                                                               ProgramParametersException)
{
    const AString className = parameters.nextString("Class Name");
    AString derivedFromClassName = "CaretObject";
    
    bool hasCopyAndAssignment = false;
    while (parameters.hasNext()) {
        const AString& param = parameters.nextString("Create Class Parameter");
        if (param == "-copy") {
            hasCopyAndAssignment = true;
        }
        else if (param == "-parent") {
            derivedFromClassName = parameters.nextString("Parent Class Name");
            if (derivedFromClassName.isEmpty()) {
                throw CommandException("Parent class name is empty.");
            }
            else {
                if (derivedFromClassName[0].isUpper() == false) {
                    throw CommandException("Parent class name must begin with a Capital Letter");
                }
            }
        }
        else if (param == "-no-parent") {
            derivedFromClassName = "";
        }
        else {
            throw CommandException("Invalid parameter: " + param);
        }
    }
    
    if (className.isEmpty()) {
        throw CommandException("class name is empty.");
    }
    AString errorMessage;
    if (className[0].isUpper() == false) {
        errorMessage += "First letter of class name must be upper case.\n";
    }
    
    const AString headerFileName = className + ".h";
    const AString implementationFileName = className + ".cxx";
    
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
    this->getIfDefNames(className, 
                        ifndefName, 
                        ifdefNameStaticDeclarations);
    
    this->createHeaderFile(headerFileName,
                           className, 
                           derivedFromClassName,
                           ifndefName, 
                           ifdefNameStaticDeclarations, 
                           hasCopyAndAssignment);
    
    this->createImplementationFile(implementationFileName,
                                   className, 
                                   derivedFromClassName,
                                   ifdefNameStaticDeclarations, 
                                   hasCopyAndAssignment);
}

/**
 * Create and write the header (.h) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param className
 *    Name of class.
 * @param derivedFromClassName
 *    Name of class from which this class is derived.
 * @param ifdefName
 *    Name of "ifndef" value.
 * @param ifdefNameStaticDeclaration
 *    Name for "infdef" of static declarations.
 * @param hasCopyAndAssignment
 *    Has copy constructor and assignment operator.
 */
void 
CommandClassCreate::createHeaderFile(const AString& outputFileName,
                                     const AString& className,
                                     const AString& derivedFromClassName,
                                         const AString& ifndefName,
                                         const AString& ifdefNameStaticDeclaration,
                                         const bool hasCopyAndAssignment)
{
    AString t;
    
    AString derivedFromDeclaration;
    if (derivedFromClassName.isEmpty() == false) {
        derivedFromDeclaration = (" : public " + derivedFromClassName);
    }
    
    t += ("#ifndef " + ifndefName + "\n");
    t += ("#define " + ifndefName + "\n");
    t += this->getCopyright();
    t += ("\n");
    
    t += (this->getIncludeDeclaration(derivedFromClassName) + "\n");
    t += ("\n");
    t += ("namespace caret {\n");
    t += ("\n");

    t += ("    class " + className + derivedFromDeclaration + " {\n");
    t += ("        \n");
    if (derivedFromClassName.startsWith("Q")) {
        t += ("        Q_OBJECT\n");
        t += ("\n");
    }
    t += ("    public:\n");
    t += ("        " + className + "();\n");
    t += ("        \n");
    t += ("        virtual ~" + className + "();\n");
    t += ("        \n");
    
    if (hasCopyAndAssignment) {
        t += ("        " + className + "(const " + className + "& obj);\n");
        t += ("\n");
        t += ("        " + className + "& operator=(const " + className + "& obj);\n");
        t += ("        \n");
    }
    else {
        t += ("    private:\n");
        t += ("        " + className + "(const " + className + "&);\n");
        t += ("\n");
        t += ("        " + className + "& operator=(const " + className + "&);\n");
        t += ("        \n");
        t += ("    public:\n");
    }
    
    t += ("\n");
    t += ("        " + getNewMethodsString() + "\n");
    t += ("\n");          

    if (derivedFromClassName == "CaretObject") {
        t += ("        virtual AString toString() const;\n");
        t += ("        \n");
    }
    
    t += ("    private:\n");
    
    if (hasCopyAndAssignment) {
        t += ("        void copyHelper" + className + "(const " + className + "& obj);\n");
    }
    else {
        
    }
    t += ("\n");
    t += ("        " + getNewMembersString() + "\n");
    t += ("\n");
    t += ("    };\n");
    t += ("    \n");
    t += ("#ifdef " + ifdefNameStaticDeclaration + "\n");
    t += ("    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>\n");
    t += ("#endif // " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += ("} // namespace\n");
    
    t += ("#endif  //" + ifndefName + "\n");
    
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
 * @param className
 *    Name of class.
 * @param ifdefNameStaticDeclaration
 *    Name for "infdef" of static declarations.
 * @param hasCopyAndAssignment
 *    Has copy constructor and assignment operator.
 */
void 
CommandClassCreate::createImplementationFile(const AString& outputFileName,
                                             const AString& className,
                                             const AString& derivedFromClassName,
                                                 const AString& ifdefNameStaticDeclaration,
                                                 const bool hasCopyAndAssignment)
{
    AString t;
    
    t += this->getCopyright();
    
    t += ("#define " + ifdefNameStaticDeclaration + "\n");
    t += ("#include \"" + className + ".h\"\n");
    t += ("#undef " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += ("using namespace caret;\n");
    t += ("\n");
    t += ("\n");
    t += ("    \n");
    t += ("/**\n");
    t += (" * \\class caret::" + className + " \n");
    t += (" * \\brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>\n");
    t += (" *\n");
    t += (" * <REPLACE-WITH-THOROUGH DESCRIPTION>\n");
    t += (" */\n"); 
    t += ("\n");
    t += ("/**\n");
    t += (" * Constructor.\n");
    t += (" */\n");
    t += ("" + className + "::" + className + "()\n");
    if (derivedFromClassName.isEmpty() == false) {
        t += (": " + derivedFromClassName + "()\n");
    }
    t += ("{\n");
    t += ("    \n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Destructor.\n");
    t += (" */\n");
    t += ("" + className + "::~" + className + "()\n");
    t += ("{\n");
    t += ("    \n");
    t += ("}\n");
    t += ("\n");
    
    if (hasCopyAndAssignment) {
        t += ("/**\n");
        t += (" * Copy constructor.\n");
        t += (" * @param obj\n");
        t += (" *    Object that is copied.\n");
        t += (" */\n");
        t += ("" + className + "::" + className + "(const " + className + "& obj)\n");
        t += (": " + derivedFromClassName + "(obj)\n");
        t += ("{\n");
        t += ("    this->copyHelper" + className + "(obj);\n");
        t += ("}\n");
        t += ("\n");
        t += ("/**\n");
        t += (" * Assignment operator.\n");
        t += (" * @param obj\n");
        t += (" *    Data copied from obj to this.\n");
        t += (" * @return \n");
        t += (" *    Reference to this object.\n");
        t += (" */\n");
        t += ("" + className + "&\n");
        t += ("" + className + "::operator=(const " + className + "& obj)\n");
        t += ("{\n");
        t += ("    if (this != &obj) {\n");
        t += ("        " + derivedFromClassName + "::operator=(obj);\n");
        t += ("        this->copyHelper" + className + "(obj);\n");
        t += ("    }\n");
        t += ("    return *this;    \n");
        t += ("}\n");
        t += ("\n");
        t += ("/**\n");
        t += (" * Helps with copying an object of this type.\n");
        t += (" * @param obj\n");
        t += (" *    Object that is copied.\n");
        t += (" */\n");
        t += ("void \n");
        t += ("" + className + "::copyHelper" + className + "(const " + className + "& obj)\n");
        t += ("{\n");
        t += ("    \n");
        t += ("}\n");
        t += ("\n");
    }
    
    if (derivedFromClassName == "CaretObject") {
        t += ("/**\n");
        t += (" * Get a description of this object's content.\n");
        t += (" * @return String describing this object's content.\n");
        t += (" */\n");
        t += ("AString \n");
        t += ("" + className + "::toString() const\n");
        t += ("{\n");
        t += ("    return \"" + className + "\";\n");
        t += ("}\n");
    }

    TextFile tf;
    tf.replaceText(t);
    
    try {
        tf.writeFile(outputFileName);
    }
    catch (const DataFileException& e) {
        throw CommandException(e);
    }
}


