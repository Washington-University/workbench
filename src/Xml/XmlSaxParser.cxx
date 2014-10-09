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

#include "XmlSaxParser.h"
#include "XmlSaxParserException.h"
#include "XmlSaxParserWithQt.h"

using namespace caret;

/**
 * Constructor.
 */
XmlSaxParser::XmlSaxParser()
{
    this->initializeMembersXmlSaxParser();
}

/**
 * Destructor.
 */
XmlSaxParser::~XmlSaxParser()
{
    
}

XmlSaxParser* 
XmlSaxParser::createXmlParser()
{
    XmlSaxParser* parser = new XmlSaxParserWithQt();
    
    return parser;
}

/**
 * Initialize members.
 */
void
XmlSaxParser::initializeMembersXmlSaxParser() 
{
    
}
