
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __JSON_HELPER_DECLARE__
#include "JsonHelper.h"
#undef __JSON_HELPER_DECLARE__

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::JsonHelper 
 * \brief Helper class for JSON processing.
 * \ingroup Common
 */

/**
 * Constructor.
 */
JsonHelper::JsonHelper()
{
    
}

/**
 * Destructor.
 */
JsonHelper::~JsonHelper()
{
}

/**
 * Convert the JSON Array to a String representation
 *
 * @param jsonValue
 *     The JSON Value
 * @return
 *     The string representation of the value.
 */
AString
JsonHelper::jsonArrayToString(const QJsonArray& jsonArray)
{
    const QJsonDocument contentJson = QJsonDocument(jsonArray);
    const AString s = contentJson.toJson(QJsonDocument::Compact);
    return s;
}


/**
 * Determine type of content in the value and return its string representation
 *
 * @param jsonValue
 *     The JSON Value
 * @return
 *     The string representation of the value.
 */
AString
JsonHelper::jsonValueToString(const QJsonValue& jsonValue)
{
    AString typeString("Invalid: ");
    AString contentString;
    QJsonDocument contentJson;
    
    switch (jsonValue.type()) {
        case QJsonValue::Array:
            typeString = "Array: ";
            contentJson = QJsonDocument(jsonValue.toArray());
            break;
        case QJsonValue::Bool:
            typeString = "Bool: ";
            contentString = AString::fromBool(jsonValue.toBool());
            break;
        case QJsonValue::Double:
            typeString = "Double: ";
            contentString = AString::fromBool(jsonValue.toDouble());
            break;
        case QJsonValue::Null:
            typeString = "Null: ";
            contentString = "";
            break;
        case QJsonValue::Object:
            typeString = "Object: ";
            contentJson = QJsonDocument(jsonValue.toObject());
            break;
        case QJsonValue::String:
            typeString = "String: ";
            contentString = jsonValue.toString();
            break;
        case QJsonValue::Undefined:
            typeString = "Undefined: ";
            contentString = "";
            break;
    }
    
    AString s(typeString);
    
    if (contentJson.isNull()) {
        s.append(contentString);
    }
    else {
        s.append(contentJson.toJson(QJsonDocument::Compact));
    }
    
    return s;
}
