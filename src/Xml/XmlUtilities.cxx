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

#include "CaretLogger.h"
#include "XmlSaxParserException.h"
#include "XmlUtilities.h"

using namespace caret;

/**
 * \class caret::XmlUtilities
 * \brief Contains static methods for assistance with XML processing.
 */

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

/**
 * Replace special characters (&<>'\) with XML
 * compatible encoded (&amp;  &lt  &gt  &apos  &quot).
 *
 * @param text 
 *    special characters are handled in this string.
 * @return
 *    Copy of input with special characters handled.
 */
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

/**
 * Replace encoded characters (&amp;  &lt  &gt  &apos  &quot)
 * with normal characters (&<>'\).
 *
 * @param text 
 *    special characters are handled in this string.
 * @return
 *    Copy of input with special characters handled.
 */
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

/**
 * Create a message for an invalid root element.
 *
 * @param validRootElementName
 *    The valid root element.
 * @param rootElementName
 *    The invalid root element that was encountered.
 * @return
 *    Message describing the invalid root element.
 */
AString 
XmlUtilities::createInvalidRootElementMessage(const AString& validRootElementName,
                                               const AString& rootElementName)
{
    AString txt("Root element is \"" 
                + rootElementName
                + "\" but should be "
                + validRootElementName
                + "\"");
    return txt;
}

/**
 * Create a message for an invalid child element.
 *
 * @param parentElementName
 *    The parent element.
 * @param invalidChildElementName
 *    The invalid child element that was encountered.
 * @return
 *    Message describing the invalid child element.
 */
AString 
XmlUtilities::createInvalidChildElementMessage(const AString& parentElementName,
                                                const AString& invalidChildElementName)
{
    AString txt("\""
                + parentElementName
                + " contains invalid child named \""
                + invalidChildElementName
                + "\"");
    return txt;
}

/**
 * Create an invalid attribute message.
 *
 * @param elementName
 *     Name of element.
 * @param attributeName
 *     Name of attribute.
 * @param invalidAttributeValue
 *     Invalid attribute value.
 * @return
 *     Message describing the invalid verion.
 */
AString 
XmlUtilities::createInvalidAttributeMessage(const AString& elementName,
                                            const AString& attributeName,
                                            const AString& invalidAttributeValue)
{
    AString txt("Element "
                + elementName 
                + " has an attribute named "
                + attributeName
                + " with the invalid value "
                + invalidAttributeValue);
    return txt;
}


/**
 * Create an invalid version message.
 *
 * @param supportedVersion
 *    The latest supported version number.
 * @param invalidVersion
 *     The invalid version number that was encountered.
 * @return
 *     Message describing the invalid verion.
 */
AString 
XmlUtilities::createInvalidVersionMessage(const float supportedVersion,
                                           const float invalidVersion)
{
    AString txt("File version is " 
                + AString::number(invalidVersion) 
                + " but versions newer than "
                + AString::number(supportedVersion)
                + " are not supported.  Check for update to your software.\n");
    return txt;
}

/**
 * Create an invalid number of elements message.
 * 
 * @param elementName
 *    Name of the element.
 * @param correctNumberOfElements
 *    Number of elements that should be present.
 * @param numberOfElements
 *    Actual number of elements.
 * @return
 *    Message describing the incorrect number of elements.
 */
AString 
XmlUtilities::createInvalidNumberOfElementsMessage(const AString& elementName,
                                                   const int32_t correctNumberOfElements,
                                                   const int32_t numberOfElements)
{
    AString txt(elementName
                + " should contain "
                + AString::number(correctNumberOfElements)
                + " elements but contains "
                + AString::number(numberOfElements));
    return txt;
}

/**
 * Extract numbers from the text.  If the count of numbers is not 
 * the correct amount, an exception is thrown.
 *
 * @param elementName
 *    Name of element for which numbers are desired.
 * @param text
 *    Text from which numbers are extracted.
 * @param correctVectorLength
 *    Count of numbers that are expected.
 * @param numbersOut
 *    Numbers are loaded into this vector.
 * @throw XmlSaxParserException
 *    If the correct count of numbers is not obtained.
 */
void 
XmlUtilities::getArrayOfNumbersFromText(const AString& elementName,
                                        const AString& text,
                                        const int32_t correctVectorLength,
                                        std::vector<float>& numbersOut)
{
    numbersOut.clear();
    AString::toNumbers(text, numbersOut);
    
    if (static_cast<int32_t>(numbersOut.size()) != correctVectorLength) {
        AString txt = XmlUtilities::createInvalidNumberOfElementsMessage(elementName,
                                                                         correctVectorLength,
                                                                         numbersOut.size());
        XmlSaxParserException e(txt);
        CaretLogThrowing(e);
        throw e;
    }
}

/**
 * Extract numbers from the text.  If the count of numbers is not 
 * the correct amount, an exception is thrown.
 *
 * @param elementName
 *    Name of element for which numbers are desired.
 * @param text
 *    Text from which numbers are extracted.
 * @param correctVectorLength
 *    Count of numbers that are expected.
 * @param numbersOut
 *    Numbers are loaded into this vector.
 * @throw XmlSaxParserException
 *    If the correct count of numbers is not obtained.
 */
void 
XmlUtilities::getArrayOfNumbersFromText(const AString& elementName,
                                        const AString& text,
                                        const int32_t correctVectorLength,
                                        std::vector<int32_t>& numbersOut)
{
    numbersOut.clear();
    AString::toNumbers(text, numbersOut);
    
    if (static_cast<int32_t>(numbersOut.size()) != correctVectorLength) {
        AString txt = XmlUtilities::createInvalidNumberOfElementsMessage(elementName,
                                                                         correctVectorLength,
                                                                         numbersOut.size());
        XmlSaxParserException e(txt);
        CaretLogThrowing(e);
        throw e;
    }
}
