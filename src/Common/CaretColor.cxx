
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __CARET_COLOR_DECLARE__
#include "CaretColor.h"
#undef __CARET_COLOR_DECLARE__

#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::CaretColor 
 * \brief Caret color that is a CaretColorEnum but also supports custom color in one object
 * \ingroup Common
 */

/**
 * Constructor.
 */
CaretColor::CaretColor()
: CaretObject()
{
    m_color = CaretColorEnum::BLACK;
    m_customRGBA.fill(0);
    m_customRGBA[3] = 255;
}

/**
 * Destructor.
 */
CaretColor::~CaretColor()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
CaretColor::CaretColor(const CaretColor& obj)
: CaretObject(obj)
{
    this->copyHelperCaretColor(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
CaretColor&
CaretColor::operator=(const CaretColor& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperCaretColor(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
CaretColor::copyHelperCaretColor(const CaretColor& obj)
{
    m_color      = obj.m_color;
    m_customRGBA = obj.m_customRGBA;
}

/**
 * Equality operator.
 * @param obj
 *    Instance compared to this for equality.
 * @return 
 *    True if this instance and 'obj' instance are considered equal.
 */
bool
CaretColor::operator==(const CaretColor& obj) const
{
    if (this == &obj) {
        return true;    
    }

    if (m_color == obj.m_color) {
        if (m_color == CaretColorEnum::CUSTOM) {
            if (m_customRGBA == obj.m_customRGBA) {
                return true;
            }
        }
        else {
            return true;
        }
    }

    return false;    
}

/**
 * Inequality operator.
 * @param obj
 *    Instance compared to this for inequality.
 * @return
 *    True if this instance and 'obj' instance are considered not equal.
 */
bool
CaretColor::operator!=(const CaretColor& obj) const
{
    return ( ! (*this == obj));
}

/**
 * @return The current color
 */
CaretColorEnum::Enum
CaretColor::getCaretColorEnum() const
{
    return m_color;
}

/**
 * Set the current color
 * @param color
 * New value for color
 */
void
CaretColor::setCaretColorEnum(const CaretColorEnum::Enum color)
{
    m_color = color;
}

/**
 * @return RGBA components for custom color
 */
std::array<uint8_t, 4>
CaretColor::getCustomColorRGBA() const
{
    return m_customRGBA;
}

/**
 * @return Float RGBA components for custom color
 */
std::array<float, 4>
CaretColor::getCustomColorFloatRGBA() const
{
    std::array<uint8_t, 4> rgba = getCustomColorRGBA();
    std::array<float, 4> rgbaF {
        rgba[0] / 255.0f,
        rgba[1] / 255.0f,
        rgba[2] / 255.0f,
        rgba[3] / 255.0f
    };
    
    return rgbaF;
}


/**
 * Set the color components for the custom color
 * @param customRGBA
 *    RGBA components
 */
void CaretColor::setCustomColorRGBA(const std::array<uint8_t, 4>& customRGBA)
{
    m_customRGBA = customRGBA;
}

/**
 * @return RGBA components for current color
 */
std::array<uint8_t, 4>
CaretColor::getRGBA() const
{
    if (m_color == CaretColorEnum::CUSTOM) {
        return m_customRGBA;
    }
    std::array<uint8_t, 4> rgba;
    CaretColorEnum::toRGBAByte(m_color, rgba.data());
    return rgba;
}

/**
 * @return Float RGBA components for current color
 */
std::array<float, 4>
CaretColor::getFloatRGBA() const
{
    std::array<uint8_t, 4> rgba = getRGBA();
    std::array<float, 4> rgbaF {
        rgba[0] / 255.0f,
        rgba[1] / 255.0f,
        rgba[2] / 255.0f,
        rgba[3] / 255.0f
    };
    
    return rgbaF;
}


/**
 * @return String containing CaretColor in XML for use in scenes
 */
AString
CaretColor::encodeInSceneXML() const
{
    AString xml;
    
    QXmlStreamWriter writer(&xml);
    writer.writeStartElement(XML_TAG_CARET_COLOR);
    writer.writeAttribute(XML_ATTRIBUTE_VERSION, VERSION_ONE);
    writer.writeAttribute(XML_ATTRIBUTE_COLOR_NAME,         CaretColorEnum::toName(m_color));
    writer.writeAttribute(XML_ATTRIBUTE_CUSTOM_COLOR_RED,   QString::number(m_customRGBA[0]));
    writer.writeAttribute(XML_ATTRIBUTE_CUSTOM_COLOR_GREEN, QString::number(m_customRGBA[1]));
    writer.writeAttribute(XML_ATTRIBUTE_CUSTOM_COLOR_BLUE,  QString::number(m_customRGBA[2]));
    writer.writeAttribute(XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA, QString::number(m_customRGBA[3]));
    writer.writeEndElement();
    
    return xml;
}

/**
 * Decode CaretColor from XML used in Scenes
 * @param xml
 *    The XML text
 * @param errorMessageOut
 *    Output containing error message
 * @return
 *    True if decoding is successful, else false
 */
bool
CaretColor::decodeFromSceneXML(const AString& xml,
                               AString& errorMessageOut)
{
    errorMessageOut.clear();
    const QString errorPrefix("CaretColor xml decoding ");
    
    QXmlStreamReader reader(xml);
    if (reader.readNextStartElement()) {
        if (reader.text() == XML_TAG_CARET_COLOR) {
            const QXmlStreamAttributes atts = reader.attributes();
            const QStringRef versionText = atts.value(XML_ATTRIBUTE_VERSION, "");
            if (versionText == VERSION_ONE) {
                const QStringRef nameText = atts.value(XML_ATTRIBUTE_COLOR_NAME, "");
                bool validFlag(false);
                m_color = CaretColorEnum::fromName(nameText.toString(), &validFlag);
                if (validFlag) {
                    m_customRGBA[0] = atts.value(XML_ATTRIBUTE_CUSTOM_COLOR_RED,   0).toInt();
                    m_customRGBA[1] = atts.value(XML_ATTRIBUTE_CUSTOM_COLOR_GREEN, 0).toInt();
                    m_customRGBA[2] = atts.value(XML_ATTRIBUTE_CUSTOM_COLOR_BLUE,  0).toInt();
                    m_customRGBA[3] = atts.value(XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA, 0).toInt();
                    return true;
                }
                else {
                    errorMessageOut = (errorPrefix
                                       + "invalid color name=\""
                                       + nameText
                                       + "\"");
                }
            }
            else {
                errorMessageOut = (errorPrefix
                                   + "version invalid or missing=\""
                                   + versionText
                                   + "\"");
            }
        }
        else {
            errorMessageOut = (errorPrefix
                               + "invalid element name: "
                               + reader.text());
        }
    }
    else {
        errorMessageOut = (errorPrefix
                           + "start element not found");
    }
    
    m_color = CaretColorEnum::BLACK;
    m_customRGBA.fill(0);
    
    return false;
}

/**
 * @return String containing CaretColor in JSON for use in scenes
 */
AString
CaretColor::encodeInJson() const
{
    QJsonObject jso;
    jso.insert(XML_ATTRIBUTE_VERSION, VERSION_ONE);
    jso.insert(XML_ATTRIBUTE_COLOR_NAME,         CaretColorEnum::toName(m_color));
    jso.insert(XML_ATTRIBUTE_CUSTOM_COLOR_RED,   QString::number(m_customRGBA[0]));
    jso.insert(XML_ATTRIBUTE_CUSTOM_COLOR_GREEN, QString::number(m_customRGBA[1]));
    jso.insert(XML_ATTRIBUTE_CUSTOM_COLOR_BLUE,  QString::number(m_customRGBA[2]));
    jso.insert(XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA, QString::number(m_customRGBA[3]));
    
    AString text;
    
    return QJsonDocument(jso).toJson(QJsonDocument::Compact);
}

/**
 * Decode CaretColor from JSON used in Scenes
 * @param text
 *    The json text
 * @param errorMessageOut
 *    Output containing error message
 * @return
 *    True if decoding is successful, else false
 */
bool
CaretColor::decodeFromJson(const AString& text,
                           AString& errorMessageOut)
{
    errorMessageOut.clear();
    
    QJsonParseError parseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(text.toUtf8(),
                                                 &parseError);
    if (jdoc.isNull()) {
        errorMessageOut = parseError.errorString();
        return false;
    }
    
    const QJsonObject jso = jdoc.object();
    if (jso.contains(XML_ATTRIBUTE_VERSION)) {
        const QString version = jso[XML_ATTRIBUTE_VERSION].toString();
        if (version == VERSION_ONE) {
            if (jso.contains(XML_ATTRIBUTE_COLOR_NAME)
                && jso.contains(XML_ATTRIBUTE_CUSTOM_COLOR_RED)
                && jso.contains(XML_ATTRIBUTE_CUSTOM_COLOR_GREEN)
                && jso.contains(XML_ATTRIBUTE_CUSTOM_COLOR_BLUE)
                && jso.contains(XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA)) {
                bool validFlag(false);
                m_color = CaretColorEnum::fromName(jso[XML_ATTRIBUTE_COLOR_NAME].toString(),
                                                   &validFlag);
                if (validFlag) {
                    m_customRGBA[0] = jso.value(XML_ATTRIBUTE_CUSTOM_COLOR_RED).toString().toInt();
                    m_customRGBA[1] = jso.value(XML_ATTRIBUTE_CUSTOM_COLOR_GREEN).toString().toInt();
                    m_customRGBA[2] = jso.value(XML_ATTRIBUTE_CUSTOM_COLOR_BLUE).toString().toInt();
                    m_customRGBA[3] = jso.value(XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA).toString().toInt();
                    
                    return true;
                }
                
                errorMessageOut = ("Invalid color name: "
                                   + jso[XML_ATTRIBUTE_COLOR_NAME].toString());
            }
            else {
                errorMessageOut = ("Missing color name or color component in: "
                                   + text);
            }
        }
        else {
            errorMessageOut = ("Version invalid in: "
                               + text);
        }
    }
    else {
        errorMessageOut = ("Version attribute is missing from: "
                           + text);
    }

    return false;
}


/**
 * Get a description of this object's content for debugging or printing
 * @return String describing this object's content.
 */
AString 
CaretColor::toString() const
{
    AString s("CaretColor="
              + CaretColorEnum::toName(m_color));
    if (m_color == CaretColorEnum::CUSTOM) {
        s.append(", RGBA="
                 + AString::fromNumbers(m_customRGBA.data(), 4, ","));
    }
    return s;
}

