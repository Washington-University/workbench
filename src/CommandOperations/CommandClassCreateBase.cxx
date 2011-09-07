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
#include <iterator>

#include "CaretAssert.h"
#include "CommandClassCreateBase.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 */
CommandClassCreateBase::CommandClassCreateBase(const AString& commandLineSwitch,
                                               const AString& operationShortDescription)
: CommandOperation(commandLineSwitch,
                         operationShortDescription)
{
    this->headerIndentation = 0;
    this->implementationIndentation = 0;
    this->spacing = 4;
    this->havePreviousAccessModifier = false;
    this->lastAccessValue = ACCESS_PUBLIC;
}

/**
 * Destructor.
 */
CommandClassCreateBase::~CommandClassCreateBase()
{
    
}


void 
CommandClassCreateBase::start(const AString& className,
                              const AString& derivedFromClassName,
                              const std::vector<AString> includeFileNames,
                              const std::vector<AString> forwardDeclaredClassNames) throw (CommandException)
{
    this->className = className;
    this->derivedFromClassName = derivedFromClassName;
    
    if (this->className.isEmpty()) {
        throw CommandException("Class name is empty.");
    }
    QChar firstCharacter = this->className[0];
    if (firstCharacter.isLower()) {
        throw CommandException("First letter in class name must be an upper-case letter.");
    }
    
    this->createFileNames();
    
    const int32_t classNameLength = this->className.length();
    this->ifndefName += "_";
    for (int32_t i = 0; i < classNameLength; i++) {
        QChar c = this->className[i];
        if (c.isUpper()) {
            this->ifndefName += "_";
        }
        this->ifndefName += c.toUpper();
    }
    this->ifdefNameStaticDelarations = this->ifndefName + "_DECLARE__";
    this->ifndefName += "__H_";
    
    this->header("#ifndef " + this->ifndefName);
    this->header("#define " + this->ifndefName);

    this->headerText.append(this->getCopyright());
    
    this->implementationText.append(this->getCopyright());
    
    if (includeFileNames.empty() == false) {
        for (std::vector<AString>::const_iterator iter = includeFileNames.begin();
             iter != includeFileNames.end();
             iter++) {
            const AString name(*iter);
            if (name.endsWith(".h")) {
                this->header("#include \"" + *iter + "\"");
            }
            else {
                this->header("#include <" + *iter + ">");
            }
        }
        this->header("");
    }
    
    this->header("namespace caret {");
    this->headerIndentation++;
    
    this->imp("#include \"" + this->className + ".h\"");
    
    if (forwardDeclaredClassNames.empty() == false) {
        for (std::vector<AString>::const_iterator iter = forwardDeclaredClassNames.begin();
             iter != forwardDeclaredClassNames.end();
             iter++) {
            const AString name(*iter);
            
            this->header("class " + name + ";");
            
            const QChar firstChar(name[0]);
            if (firstChar == 'Q') {
                this->imp("#include <" + name + ">");
            }
            else if (firstChar.isLower()) {
                this->imp("#include <" + name + ">");
            }
            else {
                this->imp("#include \"" + name + "\"");
            }
        }
        this->header("");
    }
    this->imp("");
    
    
    if (this->derivedFromClassName.isEmpty() == false) {
        this->header("class " + this->className + " public " + this->derivedFromClassName + " {");
    }
    else {
        this->header("class " + this->className + " {");
    }
    this->header("");
    this->headerIndentation++;
}

void 
CommandClassCreateBase::finish() throw (CommandException)
{
    this->headerIndentation--;
    this->headerIndentation--;
    this->header("};");
    this->headerIndentation--;
    
    if (this->staticMemberDeclarations.empty() == false) {
        this->header("");
        this->header("#ifdef " + ifdefNameStaticDelarations);
        for (std::vector<AString>::const_iterator iter = this->staticMemberDeclarations.begin();
             iter != this->staticMemberDeclarations.end();
             iter++) {
            this->header("    " + *iter);
        }
        this->header("#endif // " + this->ifdefNameStaticDelarations);
        this->header("");
    }
    
    this->header("}   // namespace");
    
    std::ofstream headerStream(qPrintable(this->headerFileName));
    if (!headerStream) {
        throw CommandException("Unable to open " + this->headerFileName + " for writing.");
    }
    
    this->header("");
    this->header("#endif  //" + this->ifndefName);

    
    const int64_t headerDataSize = this->headerText.length();
    headerStream.write(qPrintable(this->headerText), headerDataSize);
    
    headerStream.close();
    
    std::ofstream impStream(qPrintable(this->implementationFileName));
    if (!impStream) {
        throw CommandException("Unable to open " + this->headerFileName + " for writing.");
    }
    const int64_t impDataSize = this->implementationText.length();
    impStream.write(qPrintable(this->implementationText), impDataSize);
    
    impStream.close();
}

void 
CommandClassCreateBase::createFileNames() throw (CommandException)
{
    this->headerFileName = this->className + ".h";
    this->implementationFileName = this->className + ".cxx";

    std::cout << "NEED TO VERIFY THAT CLASS FILES DO NOT EXIST" << std::endl;
}

void 
CommandClassCreateBase::writeConstructor(const AString& parameter1,
                                         const AString& parameter2,
                                         const AString& parameter3,
                                         const AString& parameter4,
                                         const AString& parameter5,
                                         const AString& parameter6,
                                         const AString& parameter7,
                                         const AString& parameter8,
                                         const AString& parameter9,
                                         const AString& parameter10)
{
    std::vector<AString> params;
    if (parameter1.isEmpty() == false)  params.push_back(parameter1);
    if (parameter2.isEmpty() == false)  params.push_back(parameter2);
    if (parameter3.isEmpty() == false)  params.push_back(parameter3);
    if (parameter4.isEmpty() == false)  params.push_back(parameter4);
    if (parameter5.isEmpty() == false)  params.push_back(parameter5);
    if (parameter6.isEmpty() == false)  params.push_back(parameter6);
    if (parameter7.isEmpty() == false)  params.push_back(parameter7);
    if (parameter8.isEmpty() == false)  params.push_back(parameter8);
    if (parameter9.isEmpty() == false)  params.push_back(parameter9);
    if (parameter10.isEmpty() == false) params.push_back(parameter10);

    AString methodCode;
    
    const int numParams = static_cast<int>(params.size());
    for (int i = 0; i < numParams; i++) {
        const int indx = params[i].lastIndexOf(' ');
        if (indx >= 0) {
            const AString name = params[i].mid(indx + 1);
            methodCode += 
            ("    this->"
             + name 
             + " = "
             + name
             + ";\n");             
        }
    }
    
    this->writeMethod(METHOD_NORMAL,
                      "", 
                      this->className, 
                      methodCode,
                      parameter1,
                      parameter2,
                      parameter3,
                      parameter4,
                      parameter5,
                      parameter6,
                      parameter7,
                      parameter8,
                      parameter9,
                      parameter10);
}

void 
CommandClassCreateBase::writeCopyConstructorAndAssignmentOperator(const bool allowed)
{
    Access savedAccess = this->lastAccessValue;
    
    AString paramName = "const " + this->className + "&";
    
    if (allowed) {
        this->writeAccess(ACCESS_PUBLIC);
        paramName += " obj";
    }
    else {
        this->writeAccess(ACCESS_PRIVATE);
    }
    
    this->header(this->className
                 + "("
                 + paramName
                 + ");");
    
    this->header("");
    
    this->header(this->className
                 + "& operator=("
                 + paramName
                 + ");");

    
    if (allowed) {
        this->imp(this->className
                  + "::"
                  + this->className
                  + "("
                  + paramName
                  + ")");
        if (this->derivedFromClassName.isEmpty() == false) {
            this->imp(" : " + this->derivedFromClassName + "(obj)");            
        }
        this->imp("{");
        this->imp("    this->copyHelper(obj);");
        this->imp("}");
        this->imp("");
        
        this->header(this->className + "&");
        this->header(this->className
                     + "& operator=("
                     + paramName
                     + ")");
        this->imp("{");
        this->imp("    if (this != &obj) {");
        this->imp("        this->copyHelper(obj);");
        this->imp("    }");
        this->imp("    return *this;");
        this->imp("}");
        this->imp("");
    }

    
    if (allowed) {
        this->writeAccess(ACCESS_PRIVATE);
        this->writeMethod(METHOD_NORMAL,
                          "void"
                          "copyHelper",
                          "",
                          paramName);
    }
    
    if (savedAccess != this->lastAccessValue) {
        this->writeAccess(savedAccess);
    }
    else {
        this->header("");
    }
}


void 
CommandClassCreateBase::writeDestructor()
{
    this->writeMethod(METHOD_NORMAL, "", "~" + this->className, "");
}

void 
CommandClassCreateBase::writeMethod(const MethodType methodType,
                                    const AString& returnType,
                                    const AString& methodName,
                                    const AString& methodCode,
                                    const AString& parameter1,
                                    const AString& parameter2,
                                    const AString& parameter3,
                                    const AString& parameter4,
                                    const AString& parameter5,
                                    const AString& parameter6,
                                    const AString& parameter7,
                                    const AString& parameter8,
                                    const AString& parameter9,
                                    const AString& parameter10)
{
    bool isStaticMethod = false;
    bool isConstMethod  = false;
    switch (methodType) {
        case METHOD_CONST:
            isConstMethod = true;
            break;
        case METHOD_NORMAL:
            break;
        case METHOD_STATIC:
            isStaticMethod = true;
            break;
    }
    std::vector<AString> params;
    if (parameter1.isEmpty() == false)  params.push_back(parameter1);
    if (parameter2.isEmpty() == false)  params.push_back(parameter2);
    if (parameter3.isEmpty() == false)  params.push_back(parameter3);
    if (parameter4.isEmpty() == false)  params.push_back(parameter4);
    if (parameter5.isEmpty() == false)  params.push_back(parameter5);
    if (parameter6.isEmpty() == false)  params.push_back(parameter6);
    if (parameter7.isEmpty() == false)  params.push_back(parameter7);
    if (parameter8.isEmpty() == false)  params.push_back(parameter8);
    if (parameter9.isEmpty() == false)  params.push_back(parameter9);
    if (parameter10.isEmpty() == false) params.push_back(parameter10);
    const int numParams = static_cast<int>(params.size());
    
    this->implementationText += this->createMethodComment(returnType, 
                                                          methodName,
                                                          params);

    AString suffix = ")";
    if (isConstMethod) {
        suffix = ") const";
    }
    
    AString headerLine = "";
    
    if (isStaticMethod) {
        headerLine += "static ";
    }
    
    if (returnType.isEmpty() == false) {
        headerLine += (returnType + " ");
        this->imp(returnType);
    }
    const int32_t staticTypeIndexNumberOfCharacters = headerLine.length();
    
    this->implementationText += 
        (this->className
         + "::"
         + methodName
         + "(");
    
    if (numParams > 0) {
        headerLine += (methodName 
                       + "(");
        const int32_t numberOfImpIndentationCharacters =
                  (this->implementationIndentation * this->spacing) 
                  + methodName.length() 
                  + this->className.length()
                  + 3;
        const AString implementationIndentText = QString(numberOfImpIndentationCharacters, ' ');
        
        const int32_t numberOfHeaderIndentationCharacters =
            (this->headerIndentation * this->spacing) 
            + methodName.length() 
            + staticTypeIndexNumberOfCharacters
            + 1;
        const AString headerIndentText = QString(numberOfHeaderIndentationCharacters, ' ');
        
        for (int i = 0; i < numParams; i++) {
            AString p = params[i];
            if (i < (numParams - 1)) {
                p += ",";
            }
            else {
                p += (suffix + ";");
            }
            
            if (i > 0) {
                headerLine += "\n" + headerIndentText;
            }
            headerLine += p;
            
            if (i > 0) {
                this->imp(implementationIndentText + p);
            }
            else {
                this->imp(p);
            }
        }
    }
    else {
        headerLine += (methodName + "(" + suffix + ";");
        this->imp(suffix);
    }
    this->header(headerLine);
    this->header("");
    
    if (methodName == this->className) {
        if (this->derivedFromClassName.isEmpty() == false) {
            this->imp(" : " + this->derivedFromClassName + "()");
        }
    }
    this->imp("{");
    if (methodCode.isEmpty() == false) {
        this->implementationText += methodCode;
    }
    this->imp("}");
    this->imp("");
}

void
CommandClassCreateBase::writeAccess(const Access access)
{
    AString accessText = "";
    switch (access) {
        case ACCESS_PRIVATE:
            accessText = "private:";
            break;
        case ACCESS_PROTECTED:
            accessText = "protected:";
            break;
        case ACCESS_PUBLIC:
            accessText = "public:";
            break;
    }
    if (havePreviousAccessModifier) {
        this->headerIndentation--;
    }
    
    this->header("");
    this->header(accessText);
    this->header("");
    
    this->headerIndentation++;
    
    havePreviousAccessModifier = true;
    
    this->lastAccessValue = access;
}

void 
CommandClassCreateBase::writeMember(const AString& dataType,
                                    const AString& memberName,
                                    const bool isStatic,
                                    const AString& initialStaticValue)
{
    AString text = "";
    
    if (isStatic) text += "static ";
    text += dataType;
    text += " ";
    text += memberName;
    text += ";";
    
    this->header(text);
    this->header("");
    
    if (isStatic) {
        AString txt = 
        dataType
        + " "
        + this->className
        + "::"
        + memberName;
        
        if (initialStaticValue.isEmpty() == false) {
            txt += 
            " = "
            + initialStaticValue;
        }
        
        txt += ";";
        
        this->staticMemberDeclarations.push_back(txt);
    }
}

void 
CommandClassCreateBase::header(const AString& text)
{
    CaretAssert(this->headerIndentation >= 0);
    this->headerText += AString(this->headerIndentation * this->spacing, ' ');
    this->headerText += text;
    this->headerText += "\n";
}
void 
CommandClassCreateBase::imp(const AString& text)
{
    CaretAssert(this->implementationIndentation >= 0);
    this->implementationText += AString(this->implementationIndentation * this->spacing, ' ');
    this->implementationText += text;
    this->implementationText += "\n";
}

void 
CommandClassCreateBase::getIfDefNames(const AString& classNameIn,
                                      AString& ifdefNameOut,
                                      AString& ifdefNameStaticDeclarationOut)
{
    ifdefNameOut = "";
    ifdefNameStaticDeclarationOut = "";
    
    const int32_t classNameLength = classNameIn.length();
    ifdefNameOut += "_";
    for (int32_t i = 0; i < classNameLength; i++) {
        QChar c = classNameIn[i];
        if (c.isUpper()) {
            ifdefNameOut += "_";
        }
        ifdefNameOut += c.toUpper();
    }
    
    ifdefNameStaticDeclarationOut = this->ifndefName + "_DECLARE__";
    ifdefNameOut += "__H_";
    
    
}

AString 
CommandClassCreateBase::getCopyright()
{
    const AString year = "2011";
    AString text;
    
    text.append("\n");
    text.append("/*LICENSE_START*/\n");
    text.append("/* \n");
    text.append(" *  Copyright 1995-" + year + " Washington University School of Medicine \n");
    text.append(" * \n");
    text.append(" *  http://brainmap.wustl.edu \n");
    text.append(" * \n");
    text.append(" *  This file is part of CARET. \n");
    text.append(" * \n");
    text.append(" *  CARET is free software; you can redistribute it and/or modify \n");
    text.append(" *  it under the terms of the GNU General Public License as published by \n");
    text.append(" *  the Free Software Foundation; either version 2 of the License, or \n");
    text.append(" *  (at your option) any later version. \n");
    text.append(" * \n");
    text.append(" *  CARET is distributed in the hope that it will be useful, \n");
    text.append(" *  but WITHOUT ANY WARRANTY; without even the implied warranty of \n");
    text.append(" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n");
    text.append(" *  GNU General Public License for more details. \n");
    text.append(" * \n");
    text.append(" *  You should have received a copy of the GNU General Public License \n");
    text.append(" *  along with CARET; if not, write to the Free Software \n");
    text.append(" *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA \n");
    text.append(" * \n");
    text.append(" */ \n");
    text.append("\n");
    
    return text;
}

void 
CommandClassCreateBase::writeHeaderComment(const AString& parameter1,
                                           const AString& parameter2,
                                           const AString& parameter3,
                                           const AString& parameter4,
                                           const AString& parameter5,
                                           const AString& parameter6,
                                           const AString& parameter7,
                                           const AString& parameter8,
                                           const AString& parameter9,
                                           const AString& parameter10)
{
    this->header("/**");

    if (parameter1.isEmpty() == false)  this->header(" * " + parameter1);
    if (parameter2.isEmpty() == false)  this->header(" * " + parameter2);
    if (parameter3.isEmpty() == false)  this->header(" * " + parameter3);
    if (parameter4.isEmpty() == false)  this->header(" * " + parameter4);
    if (parameter5.isEmpty() == false)  this->header(" * " + parameter5);
    if (parameter6.isEmpty() == false)  this->header(" * " + parameter6);
    if (parameter7.isEmpty() == false)  this->header(" * " + parameter7);
    if (parameter8.isEmpty() == false)  this->header(" * " + parameter8);
    if (parameter9.isEmpty() == false)  this->header(" * " + parameter9);
    if (parameter10.isEmpty() == false) this->header(" * " + parameter10);
    
    this->header(" */");
    
}

void 
CommandClassCreateBase::writeImplementationComment(const AString& parameter1,
                                                   const AString& parameter2,
                                                   const AString& parameter3,
                                                   const AString& parameter4,
                                                   const AString& parameter5,
                                                   const AString& parameter6,
                                                   const AString& parameter7,
                                                   const AString& parameter8,
                                                   const AString& parameter9,
                                                   const AString& parameter10)
{
    this->imp("/**");
    
    if (parameter1.isEmpty() == false)  this->imp(" * " + parameter1);
    if (parameter2.isEmpty() == false)  this->imp(" * " + parameter2);
    if (parameter3.isEmpty() == false)  this->imp(" * " + parameter3);
    if (parameter4.isEmpty() == false)  this->imp(" * " + parameter4);
    if (parameter5.isEmpty() == false)  this->imp(" * " + parameter5);
    if (parameter6.isEmpty() == false)  this->imp(" * " + parameter6);
    if (parameter7.isEmpty() == false)  this->imp(" * " + parameter7);
    if (parameter8.isEmpty() == false)  this->imp(" * " + parameter8);
    if (parameter9.isEmpty() == false)  this->imp(" * " + parameter9);
    if (parameter10.isEmpty() == false) this->imp(" * " + parameter10);
    
    this->header(" */");
    
}

AString 
CommandClassCreateBase::createMethodComment(const AString& returnValue,
                                            const AString& methodName,
                                            const std::vector<AString>& parameters)
{
    AString txt;
    
    txt += "/**\n";
    
    if (methodName == this->className) {
        txt += " * Constructor.\n";
    }
    else if (methodName == ("~" + this->className)) {
        txt += " * Destructor.\n";
    }
    else {
        txt += " *\n";
    }
    for (std::vector<AString>::const_iterator iter = parameters.begin();
         iter != parameters.end();
         iter++) {
        txt += " * @param ";
        
        const int lastBlank = iter->lastIndexOf(' ');
        if (lastBlank >= 0) {
            txt += iter->mid(lastBlank);
        }
        else {
            txt += *iter;
        }
        txt += "\n";
        txt += " *\n";
    }
    
    if (returnValue.isEmpty() == false) {
        if (returnValue != "void") {
            txt += " * @return\n";
            txt += " *\n";
        }
    }
    
    txt += " */\n";
    
    return txt;
}

