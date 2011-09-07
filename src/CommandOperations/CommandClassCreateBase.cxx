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
}

/**
 * Destructor.
 */
CommandClassCreateBase::~CommandClassCreateBase()
{
    
}

/**
 * Get the names for use with #ifdef declarations.
 * @param classNameIn
 *    Name of class.
 * @param ifndefNameOut
 *    Name for use with #ifndef at beginning of file.
 * @param ifdefNameStaticDeclarationOut
 *    Name for use when declarating static members at 
 *    end of the header file.
 */
void 
CommandClassCreateBase::getIfDefNames(const AString& classNameIn,
                                      AString& ifndefNameOut,
                                      AString& ifdefNameStaticDeclarationOut)
{
    ifndefNameOut = "";
    ifdefNameStaticDeclarationOut = "";
    
    const int32_t classNameLength = classNameIn.length();
    ifndefNameOut += "_";
    for (int32_t i = 0; i < classNameLength; i++) {
        QChar c = classNameIn[i];
        if (c.isUpper()) {
            ifndefNameOut += "_";
        }
        ifndefNameOut += c.toUpper();
    }
    
    ifdefNameStaticDeclarationOut = ifndefNameOut + "_DECLARE__";
    ifndefNameOut += "__H_";
    
    
}

/**
 * Get the copyright.
 * @return Text containing copyright.
 */
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


/**
 * Get the #include declaration for an include file.
 * @param includeFileName
 *     Name of include file.
 * @return
 *     Include declaration (no newline at end of line).
 */
AString 
CommandClassCreateBase::getIncludeDeclaration(const AString& includeFileName) const
{
    if (includeFileName.isEmpty()) {
        return "";
    }
    
    AString txt = "#include ";
    
    const QChar firstChar = includeFileName[0];
    
    if (firstChar.isLower()) {
        txt += ("<" + includeFileName + ">");
    }
    else if (firstChar == 'Q') {
        txt += ("<" + includeFileName + ">");
    }
    else {
        AString dotH = "";
        if (includeFileName.endsWith(".h") == false) {
            dotH = ".h";
        }
        txt += ("\"" + includeFileName + dotH + "\"");
    }
    
    return txt;
    
}

