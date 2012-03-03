/*LICENSE_START*/ 
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

#include "XmlAttributes.h"
#include "XmlSaxParserException.h"
#include "CaretAssert.h"

using namespace caret;
    
/**
 * Constructor.
 */
XmlAttributes::XmlAttributes() {
    this->clear();
}

/**
 * Destructor.
 */
XmlAttributes::~XmlAttributes() {
    this->clear();
}

/**
 * Clear the attributes.
 */
void 
XmlAttributes::clear() {
    this->names.clear();
    this->values.clear();
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const AString& name, const AString& value) {
    AString s = name;
    names.push_back(AString(name));
    values.push_back(value);
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const AString& name, const int32_t value) {
    names.push_back(name);
    values.push_back(AString::number(value));
    
}

/**
 * Add an attribute.
 * @param name
 *    Name of attribute.
 * @param value 
 *    Vector of numbers.
 * @param separator
 *    Characters placed between each pair of numbers in text./
 */
void 
XmlAttributes::addAttribute(const AString& name, 
                            const std::vector<int32_t>& value, 
                            const AString& separator)
{
    AString s;
    for (uint32_t i = 0; i < value.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(value[i]);
    }
    names.push_back(name);
    values.push_back(s);
}

/**
 * Add an attribute.
 * @param name
 *    Name of attribute.
 * @param value 
 *    Vector of numbers.
 * @param separator
 *    Characters placed between each pair of numbers in text./
 */
void 
XmlAttributes::addAttribute(const AString& name, 
                            const std::vector<int64_t>& value, 
                            const AString& separator)
{
    AString s;
    for (uint64_t i = 0; i < value.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += AString::number(value[i]);
    }
    names.push_back(name);
    values.push_back(s);
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const AString& name, const int64_t value) {
    names.push_back(name);
    values.push_back(AString::number(value));
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const AString& name, const float value) {
    names.push_back(name);
    values.push_back(AString::number(value));
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const AString& name, const double value) {
    names.push_back(name);
    values.push_back(AString::number(value));
}

/**
 * Get the number of attributes.
 *
 * @return Number of attributes.
 */
int 
XmlAttributes::getNumberOfAttributes() const {
    return this->names.size();
}

/**
 * Get the name of an attribute.
 *
 * @param index - index of attribute.
 *
 * @return Name of attribute at index.
 */
AString 
XmlAttributes::getName(const int index) const{
    CaretAssertVectorIndex(this->names, index);
    return this->names.at(index);
}

/**
 * Get the value of an attribute.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
AString 
XmlAttributes::getValue(const int index) const{
    CaretAssertVectorIndex(this->values, index);
    return this->values.at(index);
}

/**
 * Get the value of an attribute as an int.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
int32_t
XmlAttributes::getValueAsInt(const int index) const{
    CaretAssertVectorIndex(this->values, index);
    AString value = this->values.at(index);
    bool ok = false;
    int32_t ret = value.toInt(&ok);
    if (!ok)
    {
        AString name = this->names[index];
        throw XmlSaxParserException(AString("integer required for XML attribute ") + name + ", got '" + value + "' instead");
    }
    return ret;
}
/**
 * Get the value of an attribute as a float.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
float 
XmlAttributes::getValueAsFloat(const int index) const
{
    CaretAssertVectorIndex(this->values, index);
    AString value = this->values.at(index);
    bool ok = false;
    float ret = value.toFloat(&ok);
    if (!ok)
    {
        AString name = this->names[index];
        throw XmlSaxParserException(AString("float required for XML attribute ") + name + ", got '" + value + "' instead");
    }
    return ret;
}

/**
 * Get the value of an attribute.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
AString 
XmlAttributes::getValue(const AString& name, const AString& defaultValue) const
{
    int index = getIndex(name);
    if (index == -1) return defaultValue;
    return this->values[index];
}

/**
 * Get the value of an attribute as an int.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
int32_t
XmlAttributes::getValueAsInt(const AString& name, const int32_t defaultValue) const{
    int index = getIndex(name);
    if (index == -1) return defaultValue;
    return getValueAsInt(index);
}
/**
 * Get the value of an attribute as a float.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
float 
XmlAttributes::getValueAsFloat(const AString& name, const float& defaultValue) const
{
    int index = getIndex(name);
    if (index == -1) return defaultValue;
    return getValueAsFloat(index);
}

int XmlAttributes::getIndex(const AString& name) const
{
    int num = (int)this->names.size();
    for (int i = 0; i < num; i++) {
        if (this->names[i] == name) {
            return i;
        }
    }
    return -1;
}

float XmlAttributes::getValueAsFloatRequired(const AString& name) const
{
    int index = getIndex(name);
    if (index == -1) throw XmlSaxParserException(AString("required attribute ") + name + " missing");
    return getValueAsFloat(index);
}

int32_t XmlAttributes::getValueAsIntRequired(const caret::AString& name) const
{
    int index = getIndex(name);
    if (index == -1) throw XmlSaxParserException(AString("required attribute ") + name + " missing");
    return getValueAsInt(index);
}

AString XmlAttributes::getValueRequired(const caret::AString& name) const
{
    int index = getIndex(name);
    if (index == -1) throw XmlSaxParserException(AString("required attribute ") + name + " missing");
    return getValue(index);
}
