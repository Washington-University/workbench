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
/*LICENSE_END*/

#include "AString.h"
#include "CaretLogger.h"
#include <iostream>

using namespace caret;

std::ostream& operator << (std::ostream &lhs, const AString &rhs) 
{ 
    return lhs << rhs.toStdString(); 
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int32_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint32_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<int64_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<uint64_t>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}


/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<float>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert a vector of values into a string.
 * @param v
 *   The vector of values.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the vector's values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const std::vector<double>& v, const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < v.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(v[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const float* array, const int64_t numberOfElements, const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const int32_t* array,
                             const int64_t numberOfElements,
                             const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const int64_t* array,
                     const int64_t numberOfElements,
                     const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert an array of values into a string.
 * @param array
 *   The array of values.
 * @param numberOfElements
 *   Number of elements in the array.
 * @param separator
 *   Inserted between each pair of values.
 * @return
 *   String containing the array values separated
 *   by the separator.
 */
AString
AString::fromNumbers(const double* array,
                             const int64_t numberOfElements,
                             const AString& separator)
{
    AString s;
    for (int64_t i = 0; i < numberOfElements; i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(array[i]);
    }
    return s;
}

/**
 * Convert the boolean value to a string.
 * @param b
 *    The boolean value.
 * @return "true" if true, else "false".
 */
AString 
AString::fromBool(const bool b)
{
    if (b) {
        return "true";
    }
    return "false";
}

/**
 * Convert any URLs in this string to  
 * HTML hyperlinks.
 * "http://www.wustl.edu" becomes "<a href=http://www.wustl.edu>http://wwww.wustl.edu</a>"
 * @param sin
 *    String that may contain URLs.
 * @return
 *    Input string with any URLs replace with hyperlinks.
 */
AString 
AString::convertURLsToHyperlinks() const
{
    std::vector<AString> url;
    std::vector<int> urlStart;
    
    const AString& sin = *this;
    
    if (sin.indexOf("http://") == -1) {
        return sin;
    }
    else {
        //
        // Create a modifiable copy
        //
        AString s(sin);
        
        //
        // loop since there may be more than one URL
        //
        bool done = false;
        int startPos = 0;
        while(done == false) {
            //
            // Find the beginning of the URL 
            //
            const int httpStart = s.indexOf("http://", startPos); 
            
            //
            // Was the start of a URL found
            //
            if (httpStart == -1) {
                done = true;
            }
            else {
                //
                // Find the end of the URL
                //
                int httpEnd = s.indexOfAnyChar(" \t\n\r", httpStart + 1);
                
                //
                // May not find end since end of string
                //
                int httpLength;
                if (httpEnd == -1) {
                    httpLength = s.length() - httpStart;
                }
                else {
                    httpLength = httpEnd - httpStart;
                }
                
                //
                // Get the http URL
                //
                const AString httpString = s.mid(httpStart, httpLength);
                url.push_back(httpString);
                urlStart.push_back(httpStart);
                
                //
                // Prepare for next search
                //
                startPos = httpStart;
                //if (startPos > 0) {
                startPos = startPos + 1;
                //}
            }
        }
        
        if (url.empty() == false) {
            const int startNum = static_cast<int>(url.size()) - 1;
            for (int i = startNum; i >= 0; i--) {
                const int len = url[i].length();
                
                //
                // Create the trailing part of the hyperlink and insert it
                //
                AString trailingHyperLink("\">");
                trailingHyperLink.append(url[i]);
                trailingHyperLink.append("</a>");
                s.insert(urlStart[i] + len, trailingHyperLink);
                
                //
                // Insert the beginning of the hyperlink
                //
                s.insert(urlStart[i], " <a href=\"");
            }
        }
        return s;
    }
}

/**
 * Returns the index position of any character in 
 * 'str' in this string.
 * @param str  Characters that are searched
 *    for in this string.
 * @param from String position (default is first character).
 */
int32_t
AString::indexOfAnyChar(const AString& str,
                        const int from) const
{
    const AString& s = *this;
    const int len = s.length();
    const int len2 = str.length();
    for (int i = from; i < len; i++) {
        for (int j = 0; j < len2; j++) {
            if (s[i] == str[j]) {
                return i;
            }
        }
    }
    return -1;
}

/**
 * Return a 'C' char array containing the value 
 * of the string.  This method is necessary since
 * on an instance of Ubuntu Linux, an invalid ASCII
 * character is found on the end of data returned 
 * by toLocal8Bit().constData();.  This method
 * will replace any non-ascii characters with "_".
 * All trailing non ASCII characters will be removed.
 * 
 * @return
 *    Char array of ASCII characters with a string
 *    terminator '\0' at the end.
 */
char* 
AString::toCharArray() const 
{ 
    bool haveNonAsciiCharacters = false;
    
    /*
     * Convert to a byte array
     */
    QByteArray byteArray = this->toLocal8Bit();
    const int32_t numBytes = byteArray.length();
    if (numBytes > 0) {
        char* charOut = new char[numBytes + 1];
        
        int32_t lastAsciiChar = -1;
        for (int32_t i = 0; i < numBytes; i++) {
            char c = byteArray.at(i);
            if ((c >= 32) && (c <= 126)) {
                charOut[i] = c;
                lastAsciiChar = i;
            }
            else {
                charOut[i] = '_';
                haveNonAsciiCharacters = true;
            }
        }
        charOut[lastAsciiChar + 1] = '\0';
        
        if (haveNonAsciiCharacters) {
            CaretLogWarning("Non-ASCII characters were removed, result is \""
                            + AString(charOut)
                            + "\"");
        }
        
        return charOut;
    }
    
    char* s = new char[1];
    s[0] = 0;
    return s;
}

