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

#include "XmlUtilities.h"

using namespace caret;


/**
 * Constructor.
 */
XmlUtilities::XmlUtilities() {
}

/**
 * Destructor.
 */
XmlUtilities::~XmlUtilities() {
}

AString 
XmlUtilities::encodeXmlSpecialCharacters(const AString& text)
{
    AString textOut = text;
    textOut.replace(AString("&"), AString("&amp;"));
    //AString::replace(text, "&", "&amp;"); // MUST BE FIRST
    textOut.replace("<", "&lt;");
    textOut.replace(">", "&gt;");
    textOut.replace("'", "&apos;");
    textOut.replace("\"", "&quot;");
    
    return textOut;
    
}

AString 
XmlUtilities::decodeXmlSpecialCharacters(const AString& text)
{
    AString textOut = text;
    textOut.replace("&amp;", "&");
    textOut.replace("&lt;", "<");
    textOut.replace("&gt;", ">");
    textOut.replace( "&apos;", "'");
    textOut.replace("&quot;", "\"");
    
    return textOut;
    
}


