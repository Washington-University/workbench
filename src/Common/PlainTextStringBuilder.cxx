
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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

