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
    const int32_t numberOfEnumValues = 1;
    
    if (enumClassName.isEmpty()) {
        throw CommandException("Enum class name is empty.");
    }

    AString errorMessage;
    if (enumClassName.endsWith("Enum") == false) {
        errorMessage += "Name of class MUST end with \"Enum\".\n";
    }
    if (enumClassName[0].isLower()) {
        errorMessage += "First letter of class name must be upper case.\n";
    }
    if (errorMessage.isEmpty() == false) {
        throw CommandException(errorMessage);
    }    
    
    this->createHeaderFile(enumClassName, 
                           ifndefName, 
                           ifdefNameStaticDelarations, 
                           numberOfEnumValues);
    
    this->createImplementationFile(enumClassName, 
                                   ifdefNameStaticDelarations, 
                                   numberOfEnumValues);
}

void 
CommandClassCreateEnum::createHeaderFile(const AString& enumClassName,
                                         const AString& ifdefName,
                                         const AString& ifdefNameStaticDeclaration,
                                         const int32_t numberOfEnumValues)
{
    AString t;

    t += ("#ifdef " + ifdefName + "\n");
    t += ("#define " + ifdefName + "\n");
    t += this->getCopyright();
    t += ("\n");
    
    t += ("#include <stdint.h>\n");
    t += ("#include <vector>\n");
    t += ("#include <QString>\n");
    t += ("\n");
    t += ("namespace caret {\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Enumerated type for \n");
    t += (" */\n");
    t += ("class " + enumClassName + " {\n");
    t += ("\n");
    t += ("public:\n");
    t += ("    /**\n");
    t += ("     * Enumerated values.\n");
    t += ("     */\n");
    t += ("    enum Enum {\n");
    
    for (int indx = 0; indx < numberOfEnumValues; indx++) {
        t += ("        /** */\n");
        if (indx < (numberOfEnumValues - 1)) {
            t += ("        ,\n");
        }
        else {
            t += ("        \n");
        }
    }
    
    t += ("    };\n");
    t += ("\n");
    t += ("\n");
    t += ("    ~" + enumClassName + "();\n");
    t += ("\n");
    t += ("    static QString toName(Enum e);\n");
    t += ("    \n");
    t += ("    static Enum fromName(const QString& s, bool* isValidOut);\n");
    t += ("    \n");
    t += ("    static QString toGuiName(Enum e);\n");
    t += ("    \n");
    t += ("    static Enum fromGuiName(const QString& s, bool* isValidOut);\n");
    t += ("    \n");
    t += ("    static int32_t toIntegerCode(Enum e);\n");
    t += ("    \n");
    t += ("    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);\n");
    t += ("\n");
    t += ("private:\n");
    t += ("    " + enumClassName + "(const Enum e, \n");
    t += ("                 const int32_t integerCode, \n");
    t += ("                 const QString& name,\n");
    t += ("                 const QString& guiName);\n");
    t += ("\n");
    t += ("    static const " + enumClassName + "* findData(const Enum e);\n");
    t += ("\n");
    t += ("    static std::vector<" + enumClassName + "> enumData;\n");
    t += ("\n");
    t += ("    static void initialize();\n");
    t += ("\n");
    t += ("    static bool initializedFlag;\n");
    t += ("\n");
    t += ("    Enum e;\n");
    t += ("\n");
    t += ("    int32_t integerCode;\n");
    t += ("\n");
    t += ("    QString name;\n");
    t += ("    \n");
    t += ("    QString guiName;\n");
    t += ("};\n");
    t += ("\n");
    t += ("#ifdef " + ifdefNameStaticDeclaration + "\n");
    t += ("std::vector<" + enumClassName + "> " + enumClassName + "::enumData;\n");
    t += ("bool " + enumClassName + "::initializedFlag = false;\n");
    t += ("#endif // " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += ("} // namespace\n");
    
    t += ("#endif  //" + ifdefName + "\n");
}

void 
CommandClassCreateEnum::createImplementationFile(const AString& enumClassName,
                                                 const AString& ifdefNameStaticDeclaration,
                                                 const int32_t numberOfEnumValues)
{
    AString t;
    
    t += this->getCopyright();
    
    t += ("#define " + ifdefNameStaticDeclaration + "\n");
    t += ("#include \"" + enumClassName + ".h\"\n");
    t += ("#undef " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += ("using namespace caret;\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Constructor.\n");
    t += (" *\n");
    t += (" * @param e\n");
    t += (" *    An enumerated value.\n");
    t += (" * @param name\n");
    t += (" *    Name of enumberated value.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::" + enumClassName + "(const Enum e,\n");
    t += ("                           const int32_t integerCode,\n");
    t += ("                           const QString& name,\n");
    t += ("                           const QString& guiName)\n");
    t += ("{\n");
    t += ("    this->e = e;\n");
    t += ("    this->integerCode = integerCode;\n");
    t += ("    this->name = name;\n");
    t += ("    this->guiName = guiName;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Destructor.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::~" + enumClassName + "()\n");
    t += ("{\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Initialize the enumerated metadata.\n");
    t += (" */\n");
    t += ("void\n");
    t += ("" + enumClassName + "::initialize()\n");
    t += ("{\n");
    t += ("    if (initializedFlag) {\n");
    t += ("        return;\n");
    t += ("    }\n");
    t += ("    initializedFlag = true;\n");
    t += ("\n");
    
    for (int32_t indx = 0; indx < numberOfEnumValues; indx++) {
        t += ("    enumData.push_back(" + enumClassName + "(, \n");
        t += ("                                    " + AString::number(indx) + ", \n");
        t += ("                                    \"\", \n");
        t += ("                                    \"\"));\n");
        t += ("    \n");
    }
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Find the data for and enumerated value.\n");
    t += (" * @param e\n");
    t += (" *     The enumerated value.\n");
    t += (" * @return Pointer to data for this enumerated type\n");
    t += (" * or NULL if no data for type or if type is invalid.\n");
    t += (" */\n");
    t += ("const " + enumClassName + "*\n");
    t += ("" + enumClassName + "::findData(const Enum e)\n");
    t += ("{\n");
    t += ("    initialize();\n");
    t += ("\n");
    t += ("    size_t num = enumData.size();\n");
    t += ("    for (size_t i = 0; i < num; i++) {\n");
    t += ("        const " + enumClassName + "* d = &enumData[i];\n");
    t += ("        if (d->e == e) {\n");
    t += ("            return d;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("\n");
    t += ("    return NULL;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Get a string representation of the enumerated type.\n");
    t += (" * @param e \n");
    t += (" *     Enumerated value.\n");
    t += (" * @return \n");
    t += (" *     String representing enumerated value.\n");
    t += (" */\n");
    t += ("QString \n");
    t += ("" + enumClassName + "::toName(Enum e) {\n");
    t += ("    initialize();\n");
    t += ("    \n");
    t += ("    const " + enumClassName + "* gaio = findData(e);\n");
    t += ("    return gaio->name;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Get an enumerated value corresponding to its name.\n");
    t += (" * @param s \n");
    t += (" *     Name of enumerated value.\n");
    t += (" * @param isValidOut \n");
    t += (" *     If not NULL, it is set indicating that a\n");
    t += (" *     enum value exists for the input name.\n");
    t += (" * @return \n");
    t += (" *     Enumerated value.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::Enum \n");
    t += ("" + enumClassName + "::fromName(const QString& s, bool* isValidOut)\n");
    t += ("{\n");
    t += ("    initialize();\n");
    t += ("    \n");
    t += ("    bool validFlag = false;\n");
    t += ("    Enum e;\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        const " + enumClassName + "& d = *iter;\n");
    t += ("        if (d.name == s) {\n");
    t += ("            e = d.e;\n");
    t += ("            validFlag = true;\n");
    t += ("            break;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isValidOut != 0) {\n");
    t += ("        *isValidOut = validFlag;\n");
    t += ("    }\n");
    t += ("    return e;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Get a GUI string representation of the enumerated type.\n");
    t += (" * @param e \n");
    t += (" *     Enumerated value.\n");
    t += (" * @return \n");
    t += (" *     String representing enumerated value.\n");
    t += (" */\n");
    t += ("QString \n");
    t += ("" + enumClassName + "::toGuiName(Enum e) {\n");
    t += ("    initialize();\n");
    t += ("    \n");
    t += ("    const " + enumClassName + "* gaio = findData(e);\n");
    t += ("    return gaio->guiName;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Get an enumerated value corresponding to its GUI name.\n");
    t += (" * @param s \n");
    t += (" *     Name of enumerated value.\n");
    t += (" * @param isValidOut \n");
    t += (" *     If not NULL, it is set indicating that a\n");
    t += (" *     enum value exists for the input name.\n");
    t += (" * @return \n");
    t += (" *     Enumerated value.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::Enum \n");
    t += ("" + enumClassName + "::fromGuiName(const QString& s, bool* isValidOut)\n");
    t += ("{\n");
    t += ("    initialize();\n");
    t += ("    \n");
    t += ("    bool validFlag = false;\n");
    t += ("    Enum e;\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        const " + enumClassName + "& d = *iter;\n");
    t += ("        if (d.guiName == s) {\n");
    t += ("            e = d.e;\n");
    t += ("            validFlag = true;\n");
    t += ("            break;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isValidOut != 0) {\n");
    t += ("        *isValidOut = validFlag;\n");
    t += ("    }\n");
    t += ("    return e;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Get the integer code for a data type.\n");
    t += (" *\n");
    t += (" * @return\n");
    t += (" *    Integer code for data type.\n");
    t += (" */\n");
    t += ("int32_t\n");
    t += ("" + enumClassName + "::toIntegerCode(Enum e)\n");
    t += ("{\n");
    t += ("    initialize();\n");
    t += ("    const " + enumClassName + "* ndt = findData(e);\n");
    t += ("    return ndt->integerCode;\n");
    t += ("}\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Find the data type corresponding to an integer code.\n");
    t += (" *\n"); 
    t += (" * @param integerCode\n");
    t += (" *     Integer code for enum.\n");
    t += (" * @param isValidOut\n");
    t += (" *     If not NULL, on exit isValidOut will indicate if\n");
    t += (" *     integer code is valid.\n");
    t += (" * @return\n");
    t += (" *     Enum for integer code.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::Enum\n");
    t += ("" + enumClassName + "::fromIntegerCode(const int32_t integerCode, bool* isValidOut)\n");
    t += ("{\n");
    t += ("    initialize();\n");
    t += ("    \n");
    t += ("    bool validFlag = false;\n");
    t += ("    Enum e = VIEW_MODE_INVALID;\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        const " + enumClassName + "& ndt = *iter;\n");
    t += ("        if (ndt.integerCode == integerCode) {\n");
    t += ("            e = ndt.e;\n");
    t += ("            validFlag = true;\n");
    t += ("            break;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isValidOut != 0) {\n");
    t += ("        *isValidOut = validFlag;\n");
    t += ("    }\n");
    t += ("    return e;\n");
    t += ("}\n");
    t += ("\n");

}

