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
#include "CommandClassCreateEnum.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "ProgramParameters.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreateEnum::CommandClassCreateEnum()
: CommandClassCreateBase("-class-create-enum",
                         "CREATE SOURCE CODE CLASS FILES (.h, .cxx) FOR ENUMERATED TYPE")
{
    
}

/**
 * Destructor.
 */
CommandClassCreateEnum::~CommandClassCreateEnum()
{
    
}

AString 
CommandClassCreateEnum::getHelpInformation(const AString& /*programName*/) 
{
    AString helpInfo = ("\n"
                        "Create enumerated type header (.h) and implementation (.cxx) files.\n"
                        "\n"
                        "Usage:  <enum-class-name>\n"
                        "        <number-of-values>\n"
                        "        <auto-number>\n"
                        "        [-description <text description of enum>]\n"
                        "\n"
                        "    enum-class-name\n"
                        "        Name of the enumerated type.  Must end in \"Enum\"\n"
                        "    \n"
                        "    number-of-values\n"
                        "        Number of values in the enumerated type.\n"
                        "    \n"
                        "    auto-number\n"
                        "        Automatically generated integer codes corresponding\n"
                        "        to the enumerated values.  Value for this parameter\n"
                        "        are \"true\" and \"false\".\n"
                        "    \n"
                        "    [enum-name-1] [enum-name-2]...[enum-name-N]\n"
                        "        Optional names for the enumerated values.  \n"
                        "        \n"
                        "        If the number of names listed is greater than\n"
                        "        the \"number-of-values\" parameter, the \"number-of-values\"\n"
                        "        will become the number of names.  If the number\n"
                        "        of names is is less than the \"number-of-values\",\n"
                        "        empty entries will be created.\n"
                        "        \n"
                        "    [-description <text description of enum>]\n"
                        "        Optional description of enum's purpose/function for doxygen comment\n"
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
CommandClassCreateEnum::executeOperation(ProgramParameters& parameters)
{
    const AString enumClassName = parameters.nextString("Enum Class Name");
    int32_t numberOfEnumValues = parameters.nextInt("Number of Enum Values");
    const bool isAutoNumber = parameters.nextBoolean("Auto Number (true/false)");

    AString description;
    std::vector<AString> enumValueNames;
    while (parameters.hasNext()) {
        const AString paramValue(parameters.nextString("Parameter"));
        if (paramValue == "-description") {
            if (parameters.hasNext()) {
                description = parameters.nextString("Description text");
            }
            else {
                throw CommandException("Description missing for -description option");
            }
        }
        else {
            enumValueNames.push_back(paramValue.toUpper());
        }
    }
    const int32_t numEnumValueNames = static_cast<int32_t>(enumValueNames.size());
    if (numEnumValueNames > 0) {
        if (numEnumValueNames > numberOfEnumValues) {
            numberOfEnumValues = numEnumValueNames;
        }
    }
    
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

    const AString headerFileName = enumClassName + ".h";
    const AString implementationFileName = enumClassName + ".cxx";
    
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
    this->getIfDefNames(enumClassName, 
                        ifndefName, 
                        ifdefNameStaticDeclarations);
    
    this->createHeaderFile(headerFileName,
                           enumClassName, 
                           ifndefName, 
                           ifdefNameStaticDeclarations, 
                           numberOfEnumValues,
                           enumValueNames,
                           isAutoNumber);
    
    this->createImplementationFile(implementationFileName,
                                   enumClassName, 
                                   ifdefNameStaticDeclarations, 
                                   numberOfEnumValues,
                                   enumValueNames,
                                   isAutoNumber,
                                   description);
}

/**
 * Create and write the header (.h) file.
 *     
 * @param outputFileName
 *    Name for file that is written.
 * @param enumClassName
 *    Name of enumerated type class.
 * @param ifdefName
 *    Name of "ifndef" value.
 * @param ifdefNameStaticDeclaration
 *    Name for "infdef" of static declarations.
 * @param numberOfEnumValues
 *    Number of enumerated type values.
 * @param enumValueNames
 *    Names for the enumerated values.
 * @param isAutoNumber
 *    Automatically assign numers/indices to the enumerated values.
 */
void 
CommandClassCreateEnum::createHeaderFile(const AString& outputFileName,
                                         const AString& enumClassName,
                                         const AString& ifndefName,
                                         const AString& ifdefNameStaticDeclaration,
                                         const int32_t numberOfEnumValues,
                                         const std::vector<AString>& enumValueNames,
                                         const bool isAutoNumber)
{
    AString t;

    t += ("#ifndef " + ifndefName + "\n");
    t += ("#define " + ifndefName + "\n");
    t += this->getCopyright();
    t += ("\n");
    t += ("#include <stdint.h>\n");
    t += ("#include <vector>\n");
    t += ("#include \"AString.h\"\n");
    t += ("\n");
    t += ("namespace caret {\n");
    t += ("\n");
    t += ("class " + enumClassName + " {\n");
    t += ("\n");
    t += ("public:\n");
    t += ("    /**\n");
    t += ("     * Enumerated values.\n");
    t += ("     */\n");
    t += ("    enum Enum {\n");
    
    for (int indx = 0; indx < numberOfEnumValues; indx++) {
        t += ("        /** " + createGuiNameFromName(enumValueNames[indx]) + " */\n");
        
        t += ("        ");
        if (indx < static_cast<int32_t>(enumValueNames.size())) {
            t +=enumValueNames[indx];
        }
        
        if (indx < (numberOfEnumValues - 1)) {
            t += (",\n");
        }
        else {
            t += ("\n");
        }
    }
    
    t += ("    };\n");
    t += ("\n");
    t += ("\n");
    t += ("    ~" + enumClassName + "();\n");
    t += ("\n");
    t += ("    static AString toName(Enum enumValue);\n");
    t += ("    \n");
    t += ("    static Enum fromName(const AString& name, bool* isValidOut);\n");
    t += ("    \n");
    t += ("    static AString toGuiName(Enum enumValue);\n");
    t += ("    \n");
    t += ("    static Enum fromGuiName(const AString& guiName, bool* isValidOut);\n");
    t += ("    \n");
    t += ("    static int32_t toIntegerCode(Enum enumValue);\n");
    t += ("    \n");
    t += ("    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);\n");
    t += ("\n");
    t += ("    static void getAllEnums(std::vector<Enum>& allEnums);\n");
    t += ("\n");
    t += ("    static void getAllNames(std::vector<AString>& allNames, const bool isSorted);\n");
    t += ("\n");
    t += ("    static void getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted);\n");
    t += ("\n");
    t += ("private:\n");
    t += ("    " + enumClassName + "(const Enum enumValue, \n");
    if (isAutoNumber == false) {
        t += ("                 const int32_t integerCode, \n");
    }
    t += ("                 const AString& name,\n");
    t += ("                 const AString& guiName);\n");
    t += ("\n");
    t += ("    static const " + enumClassName + "* findData(const Enum enumValue);\n");
    t += ("\n");
    t += ("    /** Holds all instance of enum values and associated metadata */\n");
    t += ("    static std::vector<" + enumClassName + "> enumData;\n");
    t += ("\n");
    t += ("    /** Initialize instances that contain the enum values and metadata */\n");
    t += ("    static void initialize();\n");
    t += ("\n");
    t += ("    /** Indicates instance of enum values and metadata have been initialized */\n");
    t += ("    static bool initializedFlag;\n");
    t += ("    \n");
    if (isAutoNumber) {
        t += ("    /** Auto generated integer codes */\n");
        t += ("    static int32_t integerCodeCounter;\n");
        t += ("    \n");
    }
    t += ("    /** The enumerated type value for an instance */\n");
    t += ("    Enum enumValue;\n");
    t += ("\n");
    t += ("    /** The integer code associated with an enumerated value */\n");
    t += ("    int32_t integerCode;\n");
    t += ("\n");
    t += ("    /** The name, a text string that is identical to the enumerated value */\n");
    t += ("    AString name;\n");
    t += ("    \n");
    t += ("    /** A user-friendly name that is displayed in the GUI */\n");
    t += ("    AString guiName;\n");
    t += ("};\n");
    t += ("\n");
    t += ("#ifdef " + ifdefNameStaticDeclaration + "\n");
    t += ("std::vector<" + enumClassName + "> " + enumClassName + "::enumData;\n");
    t += ("bool " + enumClassName + "::initializedFlag = false;\n");
    if (isAutoNumber) {
        t += ("int32_t " + enumClassName + "::integerCodeCounter = 0; \n");
    }
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
 * @param enumClassName
 *    Name of enumerated type class.
 * @param ifdefNameStaticDeclaration
 *    Name for "infdef" of static declarations.
 * @param numberOfEnumValues
 *    Number of enumerated type values.
 * @param enumValueNames
 *    Names for the enumerated values.
 * @param isAutoNumber
 *    Automatically assign numers/indices to the enumerated values.
 * @param description
 *    Description of enum
 */
void 
CommandClassCreateEnum::createImplementationFile(const AString& outputFileName,
                                                 const AString& enumClassName,
                                                 const AString& ifdefNameStaticDeclaration,
                                                 const int32_t numberOfEnumValues,
                                                 const std::vector<AString>& enumValueNames,
                                                 const bool isAutoNumber,
                                                 const AString description)
{
    AString t;
    
    t += this->getCopyright();
    
    t += ("#include <algorithm>\n");
    t += ("#define " + ifdefNameStaticDeclaration + "\n");
    t += ("#include \"" + enumClassName + ".h\"\n");
    t += ("#undef " + ifdefNameStaticDeclaration + "\n");
    t += ("\n");
    t += ("#include \"CaretAssert.h\"\n");
    t += ("\n");
    t += ("using namespace caret;\n");
    t += ("\n");
    t += ("    \n");
    t += ("/**\n");
    t += (" * \\class caret::" + enumClassName + " \n");
    t += (" * \\brief " + description + "\n");
    t += (" *\n");
    t += (" *\n");
    t += (getEnumComboBoxTemplateHelpInfo(enumClassName));
    t += (" */\n");
    t += ("\n");
    t += ("/*\n");
    t += ("switch (value) {\n");
    for (int indx = 0; indx < numberOfEnumValues; indx++) {
        t += ("    case " + enumClassName + "::" + enumValueNames[indx] + ":\n");
        t += ("        break;\n");
    }
    t += ("}\n");
    t += ("*/\n");
    t += ("\n");
    t += ("/**\n");
    t += (" * Constructor.\n");
    t += (" *\n");
    t += (" * @param enumValue\n");
    t += (" *    An enumerated value.\n");
    if (isAutoNumber == false) {
        t += (" * @param integerCode\n");
        t += (" *    Integer code for this enumerated value.\n");
        t += (" *\n");
    }
    t += (" * @param name\n");
    t += (" *    Name of enumerated value.\n");
    t += (" *\n");
    t += (" * @param guiName\n");
    t += (" *    User-friendly name for use in user-interface.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::" + enumClassName + "(const Enum enumValue,\n");
    if (isAutoNumber == false) {
        t += ("                           const int32_t integerCode,\n");
    }
    t += ("                           const AString& name,\n");
    t += ("                           const AString& guiName)\n");
    t += ("{\n");
    t += ("    this->enumValue = enumValue;\n");
    if (isAutoNumber) {
        t += ("    this->integerCode = integerCodeCounter++;\n");
    }
    else {
        t += ("    this->integerCode = integerCode;\n");
    }
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
        AString name = "";
        AString guiName = name;
        if (indx < static_cast<int32_t>(enumValueNames.size())) {
            name = enumValueNames[indx];
            
            /*
             *    Name: "NAME_OF_ENUM_VALUE"
             * guiName: "Name Of Enum Value"
             */
            guiName = createGuiNameFromName(name);
//            guiName = name.toLower();
//            const int numChars = guiName.length();
//            for (int32_t k = 0; k < numChars; k++) {
//                if (k == 0) {
//                    guiName[k] = guiName[k].toUpper();
//                }
//                else if (guiName[k] == '_') {
//                    guiName[k] = ' ';
//                    if (k < (numChars - 1)) {
//                        k++;
//                        guiName[k] = guiName[k].toUpper();
//                    }
//                }
//            }
        }
        t += ("    enumData.push_back(" + enumClassName + "(" + name + ", \n");
        if (isAutoNumber == false) {
            t += ("                                    " + AString::number(indx) + ", \n");
        }
        t += ("                                    \"" + name + "\", \n");
        t += ("                                    \"" + guiName + "\"));\n");
        t += ("    \n");
    }
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Find the data for and enumerated value.\n");
    t += (" * @param enumValue\n");
    t += (" *     The enumerated value.\n");
    t += (" * @return Pointer to data for this enumerated type\n");
    t += (" * or NULL if no data for type or if type is invalid.\n");
    t += (" */\n");
    t += ("const " + enumClassName + "*\n");
    t += ("" + enumClassName + "::findData(const Enum enumValue)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("\n");
    t += ("    size_t num = enumData.size();\n");
    t += ("    for (size_t i = 0; i < num; i++) {\n");
    t += ("        const " + enumClassName + "* d = &enumData[i];\n");
    t += ("        if (d->enumValue == enumValue) {\n");
    t += ("            return d;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("\n");
    t += ("    return NULL;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Get a string representation of the enumerated type.\n");
    t += (" * @param enumValue \n");
    t += (" *     Enumerated value.\n");
    t += (" * @return \n");
    t += (" *     String representing enumerated value.\n");
    t += (" */\n");
    t += ("AString \n");
    t += ("" + enumClassName + "::toName(Enum enumValue) {\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    const " + enumClassName + "* enumInstance = findData(enumValue);\n");
    t += ("    return enumInstance->name;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Get an enumerated value corresponding to its name.\n");
    t += (" * @param name \n");
    t += (" *     Name of enumerated value.\n");
    t += (" * @param isValidOut \n");
    t += (" *     If not NULL, it is set indicating that a\n");
    t += (" *     enum value exists for the input name.\n");
    t += (" * @return \n");
    t += (" *     Enumerated value.\n");
    t += (" */\n");
    t += ("" + enumClassName + "::Enum \n");
    t += ("" + enumClassName + "::fromName(const AString& name, bool* isValidOut)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    bool validFlag = false;\n");
    t += ("    Enum enumValue = " + enumClassName + "::enumData[0].enumValue;\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        const " + enumClassName + "& d = *iter;\n");
    t += ("        if (d.name == name) {\n");
    t += ("            enumValue = d.enumValue;\n");
    t += ("            validFlag = true;\n");
    t += ("            break;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isValidOut != 0) {\n");
    t += ("        *isValidOut = validFlag;\n");
    t += ("    }\n");
    t += ("    else if (validFlag == false) {\n");
    t += ("        CaretAssertMessage(0, AString(\"Name \" + name + \" failed to match enumerated value for type " + enumClassName + "\"));\n");
    t += ("    }\n");
    t += ("    return enumValue;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Get a GUI string representation of the enumerated type.\n");
    t += (" * @param enumValue \n");
    t += (" *     Enumerated value.\n");
    t += (" * @return \n");
    t += (" *     String representing enumerated value.\n");
    t += (" */\n");
    t += ("AString \n");
    t += ("" + enumClassName + "::toGuiName(Enum enumValue) {\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    const " + enumClassName + "* enumInstance = findData(enumValue);\n");
    t += ("    return enumInstance->guiName;\n");
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
    t += ("" + enumClassName + "::fromGuiName(const AString& guiName, bool* isValidOut)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    bool validFlag = false;\n");
    t += ("    Enum enumValue = " + enumClassName + "::enumData[0].enumValue;\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        const " + enumClassName + "& d = *iter;\n");
    t += ("        if (d.guiName == guiName) {\n");
    t += ("            enumValue = d.enumValue;\n");
    t += ("            validFlag = true;\n");
    t += ("            break;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isValidOut != 0) {\n");
    t += ("        *isValidOut = validFlag;\n");
    t += ("    }\n");
    t += ("    else if (validFlag == false) {\n");
    t += ("        CaretAssertMessage(0, AString(\"guiName \" + guiName + \" failed to match enumerated value for type " + enumClassName + "\"));\n");
    t += ("    }\n");
    t += ("    return enumValue;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Get the integer code for a data type.\n");
    t += (" *\n");
    t += (" * @return\n");
    t += (" *    Integer code for data type.\n");
    t += (" */\n");
    t += ("int32_t\n");
    t += ("" + enumClassName + "::toIntegerCode(Enum enumValue)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    const " + enumClassName + "* enumInstance = findData(enumValue);\n");
    t += ("    return enumInstance->integerCode;\n");
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
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    bool validFlag = false;\n");
    t += ("    Enum enumValue = " + enumClassName + "::enumData[0].enumValue;\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        const " + enumClassName + "& enumInstance = *iter;\n");
    t += ("        if (enumInstance.integerCode == integerCode) {\n");
    t += ("            enumValue = enumInstance.enumValue;\n");
    t += ("            validFlag = true;\n");
    t += ("            break;\n");
    t += ("        }\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isValidOut != 0) {\n");
    t += ("        *isValidOut = validFlag;\n");
    t += ("    }\n");
    t += ("    else if (validFlag == false) {\n");
    t += ("        CaretAssertMessage(0, AString(\"Integer code \" + AString::number(integerCode) + \" failed to match enumerated value for type " + enumClassName + "\"));\n");
    t += ("    }\n");
    t += ("    return enumValue;\n");
    t += ("}\n");
    t += ("\n");
    
    
    t += ("/**\n");
    t += (" * Get all of the enumerated type values.  The values can be used\n");
    t += (" * as parameters to toXXX() methods to get associated metadata.\n"); 
    t += (" *\n");
    t += (" * @param allEnums\n");
    t += (" *     A vector that is OUTPUT containing all of the enumerated values.\n");
    t += (" */\n");
    t += ("void\n");
    t += ("" + enumClassName + "::getAllEnums(std::vector<" + enumClassName + "::Enum>& allEnums)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    allEnums.clear();\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        allEnums.push_back(iter->enumValue);\n");
    t += ("    }\n");
    t += ("}\n");
    t += ("\n");

    
    t += ("/**\n");
    t += (" * Get all of the names of the enumerated type values.\n"); 
    t += (" *\n");
    t += (" * @param allNames\n");
    t += (" *     A vector that is OUTPUT containing all of the names of the enumerated values.\n");
    t += (" * @param isSorted\n");
    t += (" *     If true, the names are sorted in alphabetical order.\n");
    t += (" */\n");
    t += ("void\n");
    t += ("" + enumClassName + "::getAllNames(std::vector<AString>& allNames, const bool isSorted)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    allNames.clear();\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        allNames.push_back(" + enumClassName + "::toName(iter->enumValue));\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isSorted) {\n");
    t += ("        std::sort(allNames.begin(), allNames.end());\n");
    t += ("    }\n");
   t += ("}\n");
    t += ("\n");
    
    t += ("/**\n");
    t += (" * Get all of the GUI names of the enumerated type values.\n"); 
    t += (" *\n");
    t += (" * @param allNames\n");
    t += (" *     A vector that is OUTPUT containing all of the GUI names of the enumerated values.\n");
    t += (" * @param isSorted\n");
    t += (" *     If true, the names are sorted in alphabetical order.\n");
    t += (" */\n");
    t += ("void\n");
    t += ("" + enumClassName + "::getAllGuiNames(std::vector<AString>& allGuiNames, const bool isSorted)\n");
    t += ("{\n");
    t += ("    if (initializedFlag == false) initialize();\n");
    t += ("    \n");
    t += ("    allGuiNames.clear();\n");
    t += ("    \n");
    t += ("    for (std::vector<" + enumClassName + ">::iterator iter = enumData.begin();\n");
    t += ("         iter != enumData.end();\n");
    t += ("         iter++) {\n");
    t += ("        allGuiNames.push_back(" + enumClassName + "::toGuiName(iter->enumValue));\n");
    t += ("    }\n");
    t += ("    \n");
    t += ("    if (isSorted) {\n");
    t += ("        std::sort(allGuiNames.begin(), allGuiNames.end());\n");
    t += ("    }\n");
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
}

/**
 * @return the GUI name created from the all uppercase name
 */
AString 
CommandClassCreateEnum::createGuiNameFromName(const AString& name) const
{
    AString guiName = name.toLower();
    const int numChars = guiName.length();
    for (int32_t k = 0; k < numChars; k++) {
        if (k == 0) {
            guiName[k] = guiName[k].toUpper();
        }
        else if (guiName[k] == '_') {
            guiName[k] = ' ';
            if (k < (numChars - 1)) {
                k++;
                guiName[k] = guiName[k].toUpper();
            }
        }
    }
    return guiName;
}

/**
 * Get a string containing information on how to use this enumerated type
 * with the EnumComboBoxTemplate in the GUI.
 *
 * @param enumClassName
 *    Name of the enumerated type.
 * @return 
 *    String containing usage information.
 */
AString
CommandClassCreateEnum::getEnumComboBoxTemplateHelpInfo(const AString& enumClassName) const
{
    const AString firstLetter = enumClassName.left(1).toLower();
    const AString memberName = ("m_"
                                  + firstLetter
                                  + enumClassName.mid(1)
                                  + "ComboBox");
    const AString slotName = (memberName.mid(2)
                              + "ItemActivated()");
    
    const AString templateParameter = ("<"
                                       + enumClassName
                                       + ","
                                       + enumClassName
                                       + "::Enum>");
    const AString setupIndentString(memberName.length() + QString("->setup<").length(),
                                    ' ');
    
    AString s(" * Using this enumerated type in the GUI with an EnumComboBoxTemplate\n"
              " * \n"
              " * Header File (.h)\n"
              " *     Forward declare the data type:\n"
              " *         class EnumComboBoxTemplate;\n"
              " * \n"
              " *     Declare the member:\n"
              " *         EnumComboBoxTemplate* " + memberName + ";\n"
              " * \n"
              " *     Declare a slot that is called when user changes selection\n"
              " *         private slots:\n"
              " *             void " + slotName + ";\n"
              " * \n"
              " * Implementation File (.cxx)\n"
              " *     Include the header files\n"
              " *         #include \"EnumComboBoxTemplate.h\"\n"
              " *         #include \"" + enumClassName + ".h\"\n"
              " * \n"
              " *     Instatiate:\n"
              " *         " + memberName + " = new EnumComboBoxTemplate(this);\n"
              " *         " + memberName + "->setup" + templateParameter + "();\n"
              " * \n"
              " *     Get notified when the user changes the selection: \n"
              " *         QObject::connect(" + memberName + ", SIGNAL(itemActivated()),\n"
              " *                          this, SLOT(" + slotName + "));\n"
              " * \n"
              " *     Update the selection:\n"
              " *         " + memberName + "->setSelectedItem" + templateParameter + "(NEW_VALUE);\n"
              " * \n"
              " *     Read the selection:\n"
              " *         const " + enumClassName + "::Enum VARIABLE = " + memberName + "->getSelectedItem" + templateParameter + "();\n"
              " * \n"
              );
    
    return s;
}



