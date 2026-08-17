
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __FEATURE_PROPERTY_VALUE_DECLARE__
#include "FeaturePropertyValue.h"
#undef __FEATURE_PROPERTY_VALUE_DECLARE__

#include <array>

#include "CaretAssert.h"
#include "CaretColor.h"

using namespace caret;


    
/**
 * \class caret::FeaturePropertyValue 
 * \brief Class for a feature value that is stored in a FeatureItem
 * \ingroup Files
 */

/**
 * Constructor.
 * @param description
 *    Description of property
 * @param dataType
 *    Type of data
 * @param value
 *    Value of data
 * @param labelText
 *    Text for when data type is label
 */
FeaturePropertyValue::FeaturePropertyValue(const AString& description,
                                                                         const FeaturePropertyDataTypeEnum::Enum dataType,
                                                                         const QVariant& value,
                                                                         const AString& labelText,
                                                                         const FeatureLabelModel* labelModel)
: FeatureBase(FeatureBase::BaseType::FEATURE_PROPERTY),
m_description(description),
m_dataType(dataType),
m_value(value),
m_labelText(labelText),
m_labelModel(labelModel)
{
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    
    AString dataText;
    switch (m_dataType) {
        case FeaturePropertyDataTypeEnum::INVALID:
            dataText = m_value.toString();
            break;
        case FeaturePropertyDataTypeEnum::RGBA:
        {
            QColor color = m_value.value<QColor>();
            dataText = QColorToString(color);
        }
            break;
        case FeaturePropertyDataTypeEnum::UNSIGNED_INTEGER:
            dataText = AString::number(m_value.toUInt());
            break;
        case FeaturePropertyDataTypeEnum::INTEGER:
            dataText = AString::number(m_value.toInt());
            break;
        case FeaturePropertyDataTypeEnum::FLOAT:
            dataText = AString::number(m_value.toFloat(), 'f', 3);
            break;
        case FeaturePropertyDataTypeEnum::LABEL:
            dataText = m_labelText;
            break;
    }
    
    setText(dataText);
}

/**
 * Destructor.
 */
FeaturePropertyValue::~FeaturePropertyValue()
{
}

/**
 * @return String representation of a QColor
 * @param color
 *   The QColor
 */
AString
FeaturePropertyValue::QColorToString(const QColor& color)
{
    return AString("("
                   + QString::number(color.red())
                   + ","
                   + QString::number(color.green())
                   + ","
                   + QString::number(color.blue())
                   + ","
                   + QString::number(color.alpha())
                   + ")");
}

/**
 * @return String representation
 */
AString
FeaturePropertyValue::toString() const
{
    AString txt("Description=\"" + m_description
                + "\", DataType=" + FeaturePropertyDataTypeEnum::toGuiName(m_dataType)
                + ", Value=" + text());
    
    switch (m_dataType) {
        case FeaturePropertyDataTypeEnum::INVALID:
            break;
        case FeaturePropertyDataTypeEnum::RGBA:
            break;
        case FeaturePropertyDataTypeEnum::UNSIGNED_INTEGER:
            break;
        case FeaturePropertyDataTypeEnum::INTEGER:
            break;
        case FeaturePropertyDataTypeEnum::FLOAT:
            break;
        case FeaturePropertyDataTypeEnum::LABEL:
            txt += (", Label=" + m_labelText);
            break;
    }

    return txt;
}


/**
 * @param The data type
 */
FeaturePropertyDataTypeEnum::Enum
FeaturePropertyValue::getDataType() const
{
    return m_dataType;
}

/**
 * @return Description of the property
 */
const AString&
FeaturePropertyValue::getDescription() const
{
    return m_description;
}


/**
 * @return The data value
 */
const QVariant&
FeaturePropertyValue::getValue() const
{
    return m_value;
}


/**
 * @return Text for when data type is label
 */
const AString&
FeaturePropertyValue::getLabelText() const
{
    return m_labelText;
}

/**
 * @return The label model
 */
const FeatureLabelModel*
FeaturePropertyValue::getLabelModel() const
{
    return m_labelModel;
}


