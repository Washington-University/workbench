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

#include <QDir>

#include "CaretAssert.h"
#include "CommandClassAddMember.h"
#include "CommandClassCreate.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "ProgramParameters.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassAddMember::CommandClassAddMember()
: CommandOperation("-class-add-member",
                   "ADD MEMBER AND GETTER/SETTER TO SOURCE CODE FILES (.h and .cxx)")
{
    
}

/**
 * Destructor.
 */
CommandClassAddMember::~CommandClassAddMember()
{
    
}

AString 
CommandClassAddMember::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Add members to class header (.h) and implementation (.cxx) files.\n"
                        "\n"
                        "Usage:  <class-name> \n"
                        "        [-add-to-files]\n"
                        "        [-m <member-name> <data-type> <description>]...\n"
                        "\n"
                        "    If the -add-to-files is not specified, the code for the\n"
                        "    header and implementation files is printed to the \n"
                        "    terminal.\n"
                        "\n"
                        "    If the -add-to-files is specified, the class files are\n"
                        "    expected to be in the current directory and named\n"
                        "    <class-name>.h and <class-name>.cxx.  The header \n"
                        "    file must contain this text in its private section:\n"
                        "        " + CommandClassCreate::getNewMembersString() + "\n"
                        "    The implementation file must contain this text in\n"
                        "    its public section:\n"
                        "        " + CommandClassCreate::getNewMethodsString() + "\n"
                        "    If either of these text string are missing, the code \n"
                        "    that would have been added to the file(s) is printed\n"
                        "    to the terminal.\n"
                        "    \n"
                        "    For each member, three text strings separated by a space\n"
                        "    must be provided and they are the name of the member\n"
                        "    its data type, and a description of the member.  If the\n"
                        "    description contains spaces the description must be\n"
                        "    enclosed in double quotes (\"\").\n"
                        "    \n"
                        "    If the data type begins with a capital letter, it is\n"
                        "    assumed to be the name of a class.  In this case, both\n"
                        "    const and non-const getters are created but not setter\n"
                        "    is created.  Otherwise, the data type is expected to be\n"
                        "    a primitive type and both a getter and a setter are\n"
                        "    created.  Note that AString and QString are treated as\n"
                        "    primitive types.\n"
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
CommandClassAddMember::executeOperation(ProgramParameters& parameters)
{
    bool isAddToFiles = false;
    AString headerMemberCode = "";
    AString headerMethodCode = "";
    AString implementationCode = "";
    
    const AString indentText = "    ";
    
    /*
     * Process parameters
     */
    const AString className = parameters.nextString("Class Name");
    while (parameters.hasNext()) {
        const AString param = parameters.nextString("");
        if (param == "-add-to-files") {
            isAddToFiles = true;
        }
        else if (param == "-m") {
            /*
             * Adding a member
             */
            AString name = parameters.nextString("Member name");
            if (name.indexOf(" ") >= 0) {
                throw CommandException("Member named \""
                                       + name
                                       + "\" cannot contain spaces.");
            }
            if (name.isEmpty()) {
                throw CommandException("A member name is empty.");
            }
            else {
                if (name[0].isLetter() == false) {
                    throw CommandException("Member named \""
                                           + name
                                           + "\" must begin with a letter.");
                }
                if (name.indexOf(" ") >= 0) {
                    throw CommandException("Member named \""
                                           + name
                                           + "\" cannot contain spaces.");
                }
            }
            AString dataType = parameters.nextString("Data type for member " + name);
            if (dataType.isEmpty()) {
                throw CommandException("A data type is empty.");
            }
            if (dataType.indexOf(" ") >= 0) {
                throw CommandException("Data Type named \""
                                       + name
                                       + "\" cannot contain spaces.");
            }
            const AString description = parameters.nextString("Description for member " + name);
            
            /*
             * If first letter of data type is upper-case, assume it
             * is a class and use a pointer for the class.  However,
             * treat AString and QString as primitive types.
             */
            bool isEnum   = false;
            bool isString = false;
            bool isClass = false;
            const QChar firstDataTypeChar = dataType[0];
            if (firstDataTypeChar.isUpper()) {
                if ((dataType == "AString")
                    || (dataType == "QString")) {
                    isString = true;
                }
                else if (dataType.endsWith("Enum")) {
                    isEnum = true;
                }
                else {
                    isClass = true;
                }
            }
            const AString returnType = dataType + (isClass ? "*" : "");
            
            /*
             * Determine partial name for getter and setter
             * handling removal or addition of "m_" for 
             * member name.
             */
            AString getSetName = name;
            AString parameterName = name;
            if (name.startsWith("m_")) {
                getSetName = name.mid(2);
                parameterName = name.mid(2);
            }
            else {
                name = "m_" + name;
            }
            
            /*
             * Create the names for the getter and setter methods
             */
            QChar firstGetSetNameChar = getSetName[0];
            if (firstGetSetNameChar.isLower()) {
                firstGetSetNameChar = firstGetSetNameChar.toUpper();
                getSetName[0] = firstGetSetNameChar;
            }
            const AString getterName = (((dataType == "bool")
                                         ? "is"
                                         : "get") + getSetName);
            const AString setterName = ("set" + getSetName);
            
            /*
             * Declare the member for the header file
             */
            if (isClass) {
                headerMemberCode += ("#include <memory>\n"
                                     "\n");
            }
            if (description.isEmpty() == false) {
                headerMemberCode += (indentText + "/** " + description + "*/\n");
            }
            headerMemberCode += (indentText
                                 + (isClass ? "std::unique_ptr<" : "")
                                 + dataType //returnType
                                 + (isClass ? ">" : "")
                                 + " "
                                 + name
                                 + ";"
                                 + "\n"
                                 + "\n");
            
            
            const AString classColonName = (className + "::");
            
            if (isClass) {
                implementationCode += ("\n"
                                       + indentText
                                       + name
                                       + " = std::unique_ptr<"
                                       + dataType
                                       + ">(new"
                                       + dataType
                                       + "());"
                                       + "\n"
                                       + "\n");
                
                /*
                 * For class members, create only getter methods
                 * that are both const and non-const
                 */
                for (int32_t i = 0; i < 2; i++) {
                    const bool isConstMethod = (i == 0);
                    headerMethodCode += (indentText
                                         + (isConstMethod ? "const " : "")
                                         + returnType
                                         + " "
                                         + getterName
                                         + "()"
                                         + (isConstMethod ? " const" : "")
                                         + ";\n"
                                         + "\n");
                    
                    
                    implementationCode += ("/**\n"
                                           " * @return " + description + "\n"
                                           " */\n"
                                           + (isConstMethod ? "const " : "")
                                           + returnType + "\n"
                                           + classColonName 
                                           + getterName
                                           + "()"
                                           + (isConstMethod ? " const" : "")
                                           + "\n"
                                           + "{\n"
                                           + "    return " + name + ".get();\n"
                                           + "}\n"
                                           + "\n");
                }
            }
            else {
                /*
                 * Getter and setter in header file
                 */
                headerMethodCode += (indentText
                                    + returnType
                                     + " "
                                    + getterName
                                    + "() const;\n"
                                    + "\n");
                
                headerMethodCode += (indentText
                                    + "void "
                                    + setterName
                                    + "(const "
                                    + dataType
                                    + (isString ? "& " : " ")
                                    + parameterName
                                    + ");\n"
                                    + "\n");
                
                /*
                 * Getter and setter in implementation file
                 */
                implementationCode += ("/**\n"
                                       " * @return " + description + "\n"
                                       " */\n"
                                       + returnType + "\n"
                                       + classColonName 
                                       + getterName
                                       + "() const\n"
                                       + "{\n"
                                       + "    return " + name + ";\n"
                                       + "}\n"
                                       + "\n");
                
                implementationCode += ("/**\n"
                                       " * Set " + description + "\n"
                                       " *\n"
                                       " * @param " + parameterName + "\n"
                                       " *    New value for " + description + "\n"
                                       " */\n"
                                       + "void\n"
                                       + classColonName 
                                     + setterName
                                     + "(const "
                                     + dataType
                                     + (isString ? "& " : " ")
                                     + parameterName
                                     + ")\n"
                                       + "{\n"
                                       + "    " + name + " = " + parameterName + ";\n"
                                       + "}\n"
                                     + "\n");
            }

        }
        else {
            throw ProgramParametersException("Unrecognized parameter: " + param);
        }
    }
    
    if (headerMemberCode.isEmpty()) {
        throw CommandException("No members were specified.");
    }

    AString errorMessage;
    
    bool isWriteToTerminal = true;
    
    if (isAddToFiles) {
        const AString headerFileName = className + ".h";
        const AString implementationFileName = className + ".cxx";
        
        FileInformation headerInfo(headerFileName);
        if (headerInfo.exists() == false) {
            errorMessage += headerFileName + " was not found.";
        }
        FileInformation impInfo(implementationFileName);
        if (impInfo.exists() == false) {
            errorMessage += implementationFileName + " was not found.";
        }
        
        
        /*
         * Copy existing files to the temporary directory
         */
        QDir tempDir = QDir::temp();
        AString tempHeaderFileName = tempDir.absoluteFilePath(headerFileName);
        AString tempImplementationFileName = tempDir.absoluteFilePath(implementationFileName);
        
        try {
            TextFile headerTextFile;
            headerTextFile.readFile(headerFileName);
            headerTextFile.writeFile(tempHeaderFileName);
            
            TextFile implementationTextFile;
            implementationTextFile.readFile(implementationFileName);
            implementationTextFile.writeFile(tempImplementationFileName);
            
            std::cout << "Backup of header file: " << qPrintable(tempHeaderFileName) << std::endl;
            std::cout << "Backup of implementation file: " << qPrintable(tempImplementationFileName) << std::endl;
            
            AString headerText = headerTextFile.getText();
            const int newMemberOffset = findInsertionOffset(headerText, 
                                                            CommandClassCreate::getNewMembersString());
            if (newMemberOffset >= 0) {
                headerMemberCode += "\n";
                headerText.insert(newMemberOffset,
                                  headerMemberCode);
            }
            else {
                if (errorMessage.isEmpty()) {
                    errorMessage += "\n";
                }
                errorMessage += ("Unable to insert new members in header file.  Did not find text "
                                 + CommandClassCreate::getNewMembersString()
                                 + " in the file "
                                 + headerFileName);
            }
            
            const int newMethodDeclareOffset = findInsertionOffset(headerText, 
                                                                   CommandClassCreate::getNewMethodsString());
            if (newMethodDeclareOffset >= 0) {
                headerMethodCode += "\n";
                headerText.insert(newMethodDeclareOffset,
                                  headerMethodCode);
            }
            else {
                if (errorMessage.isEmpty()) {
                    errorMessage += "\n";
                }
                errorMessage += ("Unable to insert new methods in header file.  Did not find text "
                                 + CommandClassCreate::getNewMethodsString()
                                 + " in the file "
                                 + headerFileName);
            }
            
            if ((newMemberOffset >= 0) 
                && (newMethodDeclareOffset >= 0)) {
                headerTextFile.replaceText(headerText);
                isWriteToTerminal = false;
                headerTextFile.writeFile(headerFileName);
            }
            
            implementationCode += "\n";
            implementationTextFile.addText("\n" + implementationCode);
            implementationTextFile.writeFile(implementationFileName);
        }
        catch (const DataFileException& dfe) {
            throw CommandException(dfe);
        }
    }
    
    if (isWriteToTerminal) {
        std::cout << std::endl;
        std::cout << "Header Code Getter Setter -------------------------------------" << std::endl;
        std::cout << qPrintable(headerMethodCode) << std::endl;
        std::cout << "Header Code Declaration ---------------------------------------" << std::endl;
        std::cout << qPrintable(headerMemberCode) << std::endl;
        std::cout << "Implementation Code -------------------------------------------" << std::endl;
        std::cout << qPrintable(implementationCode) << std::endl;
    }

    if (errorMessage.isEmpty() == false) {
        throw CommandException(errorMessage);
    }        
}

/**
 * Find the offset of the new line or carriage return in the given text
 * using the given search text.  Offset is first character after the
 * new line or carriage return.
 *
 * @param text
 *    Text that is searched.
 * @param searchForText
 *    Text that is searched for.
 * @return
 *    Offset of the search text in the text or negative if
 *    the search text is not found.
 */
int 
CommandClassAddMember::findInsertionOffset(const AString& text,
                                           const AString& searchForText)
{
    int indx = text.indexOf(searchForText);
    if (indx >= 0) {
        while (indx >= 0) {
            const QChar ch = text[indx];
            if ((ch == '\n')
                || (ch == '\r')) {
                indx++;
                break;
            }
            indx--;
        }
        return indx;
    }
    
    return -1;
}
