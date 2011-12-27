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
    ifndefNameOut += "__H_";
    
    
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
    text.append(" * Copyright " + year + " Washington University,\n");
    text.append(" * All rights reserved.\n");
    text.append(" *\n");
    text.append(" * Connectome DB and Connectome Workbench are part of the integrated Connectome \n");
    text.append(" * Informatics Platform.\n");
    text.append(" *\n");
    text.append(" * Redistribution and use in source and binary forms, with or without\n");
    text.append(" * modification, are permitted provided that the following conditions are met:\n");
    text.append(" *    * Redistributions of source code must retain the above copyright\n");
    text.append(" *      notice, this list of conditions and the following disclaimer.\n");
    text.append(" *    * Redistributions in binary form must reproduce the above copyright\n");
    text.append(" *      notice, this list of conditions and the following disclaimer in the\n");
    text.append(" *      documentation and/or other materials provided with the distribution.\n");
    text.append(" *    * Neither the names of Washington University nor the\n");
    text.append(" *      names of its contributors may be used to endorse or promote products\n");
    text.append(" *      derived from this software without specific prior written permission.\n");
    text.append(" *\n");
    text.append(" * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n");
    text.append(" * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE \n");
    text.append(" * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n");
    text.append(" * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE\n");
    text.append(" * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  \n");
    text.append(" * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF \n");
    text.append(" * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n");
    text.append(" * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN \n");
    text.append(" * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) \n");
    text.append(" * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE \n");
    text.append(" * POSSIBILITY OF SUCH DAMAGE.\n");
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

