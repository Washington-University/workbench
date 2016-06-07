
/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <iostream>

#define __CARET_JSON_OBJECT_DECLARE__
#include "CaretJsonObject.h"
#undef __CARET_JSON_OBJECT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "MathFunctions.h"

using namespace caret;

static bool debugFlag = false;
    
/**
 * \class caret::CaretJsonObject 
 * \brief Supports parsing of simple json object consisting of key/value pairs until Qt 5 available.
 * \ingroup Common
 *
 * Supports parsing of simple json object consisting of key/value pairs as
 * returned by BALSA database.  Some of API is based upon Qt5's QJsonObject.
 */

/**
 * Constructor that splits the given text into key/value pairs.
 *
 * Example: {"written":false,"fileName":"balsa_upload_to_balsa_dot_wustl_dot_edu-1.zip",
 *           "status":500,
 *           "statusText":"RB6l/balsa_upload_to_balsa_dot_wustl_dot_edu-1.zip (No such file or directory)"}
 *
 * @param jsonText
 *    Text containing a JSON object.
 */
CaretJsonObject::CaretJsonObject(const AString& jsonText)
: CaretObject()
{
    AString text(jsonText);
    const int32_t firstBracket = text.indexOf("{");
    const int32_t lastBracket  = text.lastIndexOf("}");
    
    if ((firstBracket >= 0)
        && (lastBracket > (firstBracket + 3))) {
        const int32_t startIndex = firstBracket + 1;
        const int32_t len = lastBracket - startIndex;
        text = text.mid(startIndex, len);
        if (debugFlag) std::cout << "Brackets removed: " << text << std::endl;
    }
    else {
        CaretLogInfo("JSON missing opening and closing brackets.");
        return;
    }
    
    /*
     * Iterate through string looking for commas.
     * Note: Need to avoid commas in name value strings in double quotes)
     */
    const int32_t textLen = text.length();
    std::vector<int32_t> commas;
    int32_t doubleQuoteCounter = 0;
    int32_t lastComma = -1;
    for (int32_t i = 0; i < textLen; i++) {
        const QChar ch = text[i];
        if (ch == ',') {
            if ( ! MathFunctions::isOddNumber(doubleQuoteCounter)) {
                const int32_t start = lastComma + 1;
                const int32_t len   = i - start;
                const AString nameValuePair = text.mid(start, len);
                if (debugFlag) std::cout << "Name/Value pair: " << nameValuePair << std::endl;
                processNameValueString(nameValuePair);
                lastComma = i;
            }
        }
        else if (ch == '\"') {
            doubleQuoteCounter++;
        }
    }
    if ( ! MathFunctions::isOddNumber(doubleQuoteCounter)) {
        const int32_t start = lastComma + 1;
        const int32_t len   = textLen - start;
        if (len > 2) {
            const AString nameValuePair = text.mid(start, len);
            if (debugFlag) std::cout << "Name/Value pair: " << nameValuePair << std::endl;
            processNameValueString(nameValuePair);
        }
    }
}

/**
 * Destructor.
 */
CaretJsonObject::~CaretJsonObject()
{
}

/**
 * Process a name value string by splitting into name and value.
 *
 * @param nameValueString
 *     Name value string that is split, ie: "fileName":"balsa_upload_to_balsa_dot_wustl_dot_edu-1.zip"
 */
void
CaretJsonObject::processNameValueString(const AString& nameValueString)
{
    const int32_t colonIndex = nameValueString.indexOf(":");
    if (colonIndex > 0) {
        AString name = nameValueString.left(colonIndex).trimmed();
        AString value = nameValueString.mid(colonIndex + 1).trimmed();
        
        trimLeadingTrailingDoubleQuotes(name);
        trimLeadingTrailingDoubleQuotes(value);
        
        m_data.insert(std::make_pair(name, value));

        if (debugFlag) std::cout << "   " << name << " -> " << value << std::endl;
    }
}

/**
 * Trim the leading and trailing quotes from the given text.
 *
 * @param text
 *     In/Out text that has any leading and/or trailing quotes removed.
 */
void
CaretJsonObject::trimLeadingTrailingDoubleQuotes(AString& text)
{
    if (text.endsWith("\"")) {
        text.resize(text.size() - 1);
    }
    if (text.startsWith("\"")) {
        text = text.mid(1);
    }
}

/**
 * @return Number of key/value pairs.
 */
int32_t
CaretJsonObject::count() const
{
    return m_data.size();
}

/**
 * @return True if no key/value pairs.
 */
bool
CaretJsonObject::empty() const
{
    return m_data.empty();
}

/**
 * @return The keys.
 */
std::vector<AString>
CaretJsonObject::keys() const
{
    std::vector<AString> keysOut;
    
    for (std::map<AString, AString>::const_iterator iter = m_data.begin();
         iter != m_data.end();
         iter++) {
        keysOut.push_back(iter->first);
    }
    
    return keysOut;
}

/**
 * @return True if the key exists.
 */
bool
CaretJsonObject::hasKey(const AString& key) const
{
    const std::map<AString, AString>::const_iterator iter = m_data.find(key);
    return (iter != m_data.end());
}

/**
 * @return Value for the given key.  Empty string if key is missing.
 */
AString
CaretJsonObject::value(const AString& key) const
{
    const std::map<AString, AString>::const_iterator iter = m_data.find(key);
    if (iter != m_data.end()) {
        return iter->second;
    }
    
    return "";
}

