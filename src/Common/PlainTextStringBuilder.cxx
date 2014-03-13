
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

#define __PLAIN_TEXT_STRING_BUILDER_DECLARE__
#include "PlainTextStringBuilder.h"
#undef __PLAIN_TEXT_STRING_BUILDER_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::PlainTextStringBuilder 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup Common
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
PlainTextStringBuilder::PlainTextStringBuilder()
: CaretObject()
{
    m_currentIndentationCount   = 0;
    m_indentationNumberOfSpaces = 3;
}

/**
 * Constructor.
 */
PlainTextStringBuilder::PlainTextStringBuilder(const int32_t indentationNumberOfSpaces)
: CaretObject()
{
    m_currentIndentationCount   = 0;
    m_indentationNumberOfSpaces = indentationNumberOfSpaces;
}

/**
 * Destructor.
 */
PlainTextStringBuilder::~PlainTextStringBuilder()
{
}

/**
 * Clear the text.  Does not change amount of indentation.
 */
void
PlainTextStringBuilder::clear()
{
    m_text = "";
}

/**
 * Increase the indentation.
 */
void
PlainTextStringBuilder::pushIndentation()
{
    m_currentIndentationCount += m_indentationNumberOfSpaces;
}

/**
 * Decrease the indentation.
 */
void
PlainTextStringBuilder::popIndentation()
{
    m_currentIndentationCount -= m_indentationNumberOfSpaces;
    if (m_currentIndentationCount < 0) {
        CaretLogSevere("Indentation pops exceeds pushes (indent < 0)");
        m_currentIndentationCount = 0;
    }
}

/** 
 * Add the text preceded by indentation and followed by a newline.
 *
 * @param text
 *     Text that is added.
 */
void
PlainTextStringBuilder::addLine(const AString& text)
{
    m_text += (AString().fill(' ', m_currentIndentationCount)
               + text
               + "\n");
}

/** 
 * @return The text 
 */
AString
PlainTextStringBuilder::getText() const
{
    return m_text;
}

