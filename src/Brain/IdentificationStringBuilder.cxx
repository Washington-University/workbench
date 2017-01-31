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

#include "IdentificationStringBuilder.h"

using namespace caret;

/**
 * Constructor.
 * 
 *
 */
IdentificationStringBuilder::IdentificationStringBuilder()
    : HtmlStringBuilder()
{
}

/**
 * Destructor
 */
IdentificationStringBuilder::~IdentificationStringBuilder()
{
}

/**
 * Add text to the string.  A newline is added at the end.
 * A colon is added after the bold text but in normal text.
 * If "normalText" is an empty string, no text is output.
 * 
 * @param indentFlag   Indent the text.
 * @param boldText     The bold text that starts the line.
 * @param normalText   The normal text placed after the bold text.
 *
 */
void
IdentificationStringBuilder::addLine(
                   const bool indentFlag,
                   const AString& boldText,
                   const AString& normalText)
{
    if (normalText.length() <= 0) {
        return;
    }
    
    if (indentFlag) {
        addIndent();
    }
    addBold(boldText);
    add(": ");
    add(normalText);
    this->addLineBreak();
}

/**
 * Add text to the string.  A newline is added at the end.
 * If "normalText" is an empty string, no text is output.
 *
 * @param indentFlag   Indent the text.
 * @param normalText   The normal text placed after the bold text.
 *
 */
void
IdentificationStringBuilder::addLine(
                   const bool indentFlag,
                   const AString& normalText)
{
    if (normalText.length() <= 0) {
        return;
    }
    
    if (indentFlag) {
        addIndent();
    }
    add(normalText);
    this->addLineBreak();}

/**
 * Add text to the string.  A newline is added at the end.
 * A colon is added after the bold text but in normal text.
 * 
 * @param indentFlag   Indent the text.
 * @param boldText     The bold text that starts the line.
 * @param number       The number placed after the bold text.
 * @param displayOnlyIfNumberIsNonZeroFlag - only display text if
 *   number is non-zero.
 *
 */
void
IdentificationStringBuilder::addLine(
                   const bool indentFlag,
                   const AString& boldText,
                   const int32_t number,
                   const bool displayOnlyIfNumberIsNonZeroFlag)
{
    bool displayIt = true;
    if (displayOnlyIfNumberIsNonZeroFlag) {
        displayIt = (number != 0);
    }
    if (displayIt) {
        addLine(indentFlag,
                boldText,
                AString::number(number));
    }
}

/**
 * Add text to the string.  A newline is added at the end.
 * A colon is added after the bold text but in normal text.
 * 
 * @param indentFlag   Indent the text.
 * @param boldText     The bold text that starts the line.
 * @param number       The number placed after the bold text.
 * @param displayOnlyIfNumberIsNonZeroFlag - only display text if
 *   number is non-zero.
 *
 */
void
IdentificationStringBuilder::addLine(
                   const bool indentFlag,
                   const AString& boldText,
                   const float number,
                   const bool displayOnlyIfNumberIsNonZeroFlag)
{
    bool displayIt = true;
    if (displayOnlyIfNumberIsNonZeroFlag) {
        displayIt = (number != 0.0f);
    }
    if (displayIt) {
        addLine(indentFlag,
                boldText,
                AString::number(number));
    }
}

/**
 * Add text to the string.  A newline is added at the end.
 * A colon is added after the bold text but in normal text.
 * 
 * @param indentFlag   Indent the text.
 * @param boldText     The bold text that starts the line.
 * @param floatArray          The float array placed after the bold text.
 * @param floatArrayNumberOfElements  Number of elements in the array.
 * @param displayOnlyIfNonZeroElementInArrayFlag - only display text if
 *   array contains at least one non-zero element.
 *
 */
void
IdentificationStringBuilder::addLine(
                   const bool indentFlag,
                   const AString& boldText,
                   const float floatArray[],
                   const int floatArrayNumberOfElements,
                   const bool displayOnlyIfNonZeroElementInArrayFlag)
{
    bool displayIt = true;
    if (displayOnlyIfNonZeroElementInArrayFlag) {
        displayIt = false;
        for (int64_t i = 0; i < floatArrayNumberOfElements; i++) {
            if (floatArray[i] != 0.0f) {
                displayIt = true;
                break;
            }
        }
    }
    
    if (displayIt) {
        AString sbf;
        sbf.reserve(1024);
        for (int i = 0; i < floatArrayNumberOfElements; i++) {
            if (i == 0) {
                sbf.append("(");
            }
            else {
                sbf.append(", ");
            }
            sbf.append(AString::number(floatArray[i]));
            
            if (i == (floatArrayNumberOfElements - 1)) {
                sbf.append(")");
            }
        }
        
        addLine(indentFlag,
                boldText,
                sbf);
    }
}

/**
 * Add a list of objects using the "toString()" method.  If an
 * object in the list is null, a single blank character is placed
 * into the values list. 
 * @param indentFlag true if indentation is needed.
 * @param boldText Bold text that is displayed before the values.
 * @param objectList  List of objects that have the toString()
 * value displayed.
 * @param displayOnlyIfNonNullElementInArrayFlag  Text is only
 * displayed if one of the items in the list is not null.
 *
 */
void
IdentificationStringBuilder::addLine(
                   const bool indentFlag,
                   const AString& boldText,
                   const std::vector<const CaretObject*>& objectList,
                   const bool displayOnlyIfNonNullElementInArrayFlag)
{
    bool displayIt = true;
    if (displayOnlyIfNonNullElementInArrayFlag) {
        displayIt = false;
        const int64_t numObjects = static_cast<int64_t>(objectList.size());
        for (int64_t i = 0; i < numObjects; i++) {
            if (objectList[i] != NULL) {
                displayIt = true;
                break;
            }
        }
    }
    
    if (displayIt) {
        AString sbf;
        sbf.reserve(2048);
        int numItems = objectList.size();
        for (int i = 0; i < numItems; i++) {
            if (i == 0) {
                sbf.append("(");
            }
            else {
                sbf.append(", ");
            }
            const CaretObject* obj = objectList[i];
            if (obj != NULL) {
                sbf.append(obj->toString());
            }
            
            if (i == (numItems - 1)) {
                sbf.append(")");
            }
        }
        
        addLine(indentFlag,
                boldText,
                sbf);
    }
}

/**
 * Indent the line.
 */
void
IdentificationStringBuilder::addIndent()
{
    this->addSpaces(4);
}

