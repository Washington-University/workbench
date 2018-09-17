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


#include "HtmlStringBuilder.h"

using namespace caret;

/**
 * \class caret::HtmlStringBuilder
 * \brief Assists with constructing HTML text.
 */

/**
 * Constructor.mp
 *
 */
HtmlStringBuilder::HtmlStringBuilder()
    : CaretObject()
{
    this->clear();
}

/**
 * Destructor
 */
HtmlStringBuilder::~HtmlStringBuilder()
{
}

/**
 * Append plain text (same as addText()).
 *
 * @param text - Plain text to add.
 *
 */
void
HtmlStringBuilder::append(const AString& text)
{
    this->stringBuilder.append(text);
}

/**
 * Clear the text.
 *
 */
void
HtmlStringBuilder::clear()
{
    this->stringBuilder.reserve(4096);
    this->stringBuilder = "";
}

/**
 * Add plain text.
 *
 * @param text - Plain text to add.
 *
 */
void
HtmlStringBuilder::add(const AString& text)
{
    this->stringBuilder.append(text);
}

/**
 * Add plain text followed by a line break.
 * @param text - Plain text to add.
 *
 */
void
HtmlStringBuilder::addLine(const AString& text)
{
    this->stringBuilder.append(text);
    this->addLineBreak();
}

/**
 * Append an integer.
 *
 * @param num   Integer that is to be appended.
 *
 */
void
HtmlStringBuilder::add(const int32_t num)
{
    this->stringBuilder.append(AString::number(num));
}

/**
 * Append a float.
 *
 * @param num  Float that is to be appended.
 *
 */
void
HtmlStringBuilder::add(const float num)
{
    this->stringBuilder.append(AString::number(num));
}

/**
 * Add bold text.
 *
 * @param text - Text that is made bold.
 *
 */
void
HtmlStringBuilder::addBold(const AString& text)
{
    this->stringBuilder.append("<BOLD>");
    this->stringBuilder.append(text);
    this->stringBuilder.append("</BOLD>");
}

/**
 * Add bold float.
 *
 * @param num - Number that is made bold.
 *
 */
void
HtmlStringBuilder::addBold(const float num)
{
    this->stringBuilder.append("<BOLD>");
    this->stringBuilder.append(AString::number(num));
    this->stringBuilder.append("</BOLD>");
}

/**
 * Add bold int.
 *
 * @param num - Number that is made bold.
 *
 */
void
HtmlStringBuilder::addBold(const int32_t num)
{
    this->stringBuilder.append("<BOLD>");
    this->stringBuilder.append(AString::number(num));
    this->stringBuilder.append("</BOLD>");
}

/**
 * Add a hyperlink to the text.
 * @param urlText - URL that is target.
 * @param linkText - Text displayed as hyperlink.
 *
 */
void
HtmlStringBuilder::addHyperlink(
                   const AString& urlText,
                   const AString& linkText)
{
    this->stringBuilder.append("<a href=\"");
    this->stringBuilder.append("http://");
    this->stringBuilder.append(urlText);
    this->stringBuilder.append("/");
    this->stringBuilder.append(linkText);
    this->stringBuilder.append("\">");
    this->stringBuilder.append(linkText);
    this->stringBuilder.append("</a>");
}

/**
 * Ends a paragraph.
 *
 */
void
HtmlStringBuilder::addParagraph()
{
    this->stringBuilder.append("<P></P>");
}

/**
 * Add a line break.
 *
 */
void
HtmlStringBuilder::addLineBreak()
{
    this->stringBuilder.append("<BR></BR>");
}

/**
 * Add a line breaks.
 * @param numLineBreaks - Number of line breaks to add.
 *
 */
void
HtmlStringBuilder::addLineBreaks(const int32_t numLineBreaks)
{
    for (int i = 0; i < numLineBreaks; i++) {
        this->addLineBreak();
    }
}

/**
 * Add a space.
 *
 */
void
HtmlStringBuilder::addSpace()
{
    this->stringBuilder.append("&nbsp;");
}

/**
 * Add a spaces.
 * @param numSpaces - Number of spaces.
 *
 */
void
HtmlStringBuilder::addSpaces(const int32_t numSpaces)
{
    for (int i = 0; i < numSpaces; i++) {
        this->addSpace();
    }
}

/**
 * Get the total text length (includes HTML markup).
 * @return Length of text.
 *
 */
int32_t
HtmlStringBuilder::length()
{
    return this->stringBuilder.length();
}

/**
 * Convert to a string in HTML format WITHOUT leading and trailing
 * HTML and BODY tags.
 *
 * @return String containing text.
 *
 */
AString
HtmlStringBuilder::toString() const
{
    return this->stringBuilder;
}

/**
 * Convert to a string in HTML format WITH leading and trailing
 * HTML and BODY tags.
 *
 * @return String containing text.
 *
 */
AString
HtmlStringBuilder::toStringWithHtmlBodyForToolTip()
{
    return toStringWithHtmlBodyPrivate(true);
}

/**
 * Convert to a string in HTML format WITH leading and trailing
 * HTML and BODY tags.
 *
 * @return String containing text.
 *
 */
AString
HtmlStringBuilder::toStringWithHtmlBody()
{
    return toStringWithHtmlBodyPrivate(false);
}

/**
 * Convert to a string in HTML format WITH leading and trailing
 * HTML and BODY tags.
 *
 * @param toolTipFlag
 *      If true, set the style so text does not wrap when
 *      placed into a QToolTip (see QToolTip documentation).
 * @return String containing text.
 *
 */
AString
HtmlStringBuilder::toStringWithHtmlBodyPrivate(const bool toolTipFlag)
{
    AString sb;
    sb.reserve(this->stringBuilder.length() + 100);
    
    sb.append("<HTML><BODY>");
    if (toolTipFlag) {
        sb.append("<p style='white-space:pre'>");
    }
    
    /*
     * If the string ends with a line break, remove the line break
     */
    const AString lastBreak("<BR></BR>");
    if (this->stringBuilder.endsWith(lastBreak)) {
        this->stringBuilder.resize(this->stringBuilder.length() - lastBreak.length());
    }
    sb.append(this->stringBuilder);
    
    if (toolTipFlag) {
        sb.append("</p>");
    }
    sb.append("</BODY></HTML>");
    
    return sb;
}


