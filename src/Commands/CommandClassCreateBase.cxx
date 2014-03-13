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
#include <iterator>

#include "CaretAssert.h"
#include "CommandClassCreateBase.h"
#include "ProgramParameters.h"
#include "SystemUtilities.h"

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
    ifndefNameOut += "_H__";
    
    
}

/**
 * Get the copyright.
 * @return Text containing copyright.
 */
AString 
CommandClassCreateBase::getCopyright()
{
    
    const AString year = SystemUtilities::getYear();
    AString text;
    
    text.append("\n");
    text.append("/*LICENSE_START*/\n");
    text.append("/*\n");
    text.append(" *  Copyright (C) " + year + " Washington University School of Medicine\n");
    text.append(" *\n");
    text.append(" *  This program is free software; you can redistribute it and/or modify\n");
    text.append(" *  it under the terms of the GNU General Public License as published by\n");
    text.append(" *  the Free Software Foundation; either version 2 of the License, or\n");
    text.append(" *  (at your option) any later version.\n");
    text.append(" *\n");
    text.append(" *  This program is distributed in the hope that it will be useful,\n");
    text.append(" *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    text.append(" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    text.append(" *  GNU General Public License for more details.\n");
    text.append(" *\n");
    text.append(" *  You should have received a copy of the GNU General Public License along\n");
    text.append(" *  with this program; if not, write to the Free Software Foundation, Inc.,\n");
    text.append(" *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n");
    text.append(" */\n");
    text.append("/*LICENSE_END*/\n");
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

