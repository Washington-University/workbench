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
#include "StringUtilities.h"
#include "XmlAttributes.h"

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
XmlAttributes::addAttribute(const QString& name, const QString& value) {
    QString s = name;
    names.push_back(QString(name));
    values.push_back(value);
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const QString& name, const int32_t value) {
    names.push_back(name);
    values.push_back(QString::number(value));
    
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
XmlAttributes::addAttribute(const QString& name, 
                            const std::vector<int32_t>& value, 
                            const QString& separator)
{
    QString s;
    for (uint32_t i = 0; i < value.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += QString::number(value[i]);
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
XmlAttributes::addAttribute(const QString& name, 
                            const std::vector<int64_t>& value, 
                            const QString& separator)
{
    QString s;
    for (uint64_t i = 0; i < value.size(); i++) {
        if (i > 0) {
            s += separator;
        }
        s += QString::number(value[i]);
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
XmlAttributes::addAttribute(const QString& name, const int64_t value) {
    names.push_back(name);
    values.push_back(QString::number(value));
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const QString& name, const float value) {
    names.push_back(name);
    values.push_back(QString::number(value));
}

/**
 * Add an attribute.
 *
 * @param name - Name of attribute.
 * @param value - Value of attribute.
 */
void 
XmlAttributes::addAttribute(const QString& name, const double value) {
    names.push_back(name);
    values.push_back(QString::number(value));
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
QString 
XmlAttributes::getName(const int index) const{
    return this->names.at(index);
}

/**
 * Get the value of an attribute.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
QString 
XmlAttributes::getValue(const int index) const{
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
    return this->values.at(index).toInt();
}
/**
 * Get the value of an attribute as a float.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
float 
XmlAttributes::getValueAsFloat(const int index) const{
    
    return this->values.at(index).toFloat();
}

/**
 * Get the value of an attribute.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
QString 
XmlAttributes::getValue(const QString& name) const{
    uint64_t num = this->names.size();
    for (uint64_t i = 0; i < num; i++) {
        if (this->names[i] == name) {
            return this->values[i];
        }
    }
    return "";
}

/**
 * Get the value of an attribute as an int.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
int32_t
XmlAttributes::getValueAsInt(const QString& name) const{
    QString value = this->getValue(name);
    return value.toInt();
}
/**
 * Get the value of an attribute as a float.
 *
 * @param index - index of attribute.
 *
 * @return Value of attribute at index.
 */
float 
XmlAttributes::getValueAsFloat(const QString& name) const
{
    QString value = this->getValue(name);
    return value.toFloat();
}

