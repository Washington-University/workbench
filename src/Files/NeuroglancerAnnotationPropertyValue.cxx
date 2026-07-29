
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

#define __NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_DECLARE__
#include "NeuroglancerAnnotationPropertyValue.h"
#undef __NEUROGLANCER_ANNOTATION_PROPERTY_VALUE_DECLARE__

#include <array>

#include "CaretAssert.h"
#include "CaretColor.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotationPropertyValue 
 * \brief Class for an annotation property
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
NeuroglancerAnnotationPropertyValue::NeuroglancerAnnotationPropertyValue(const AString& description,
                                                               const NeuroglancerAnnotationPropertyDataTypeEnum::Enum dataType,
                                                               const QVariant& value,
                                                               const AString& labelText)
: NeuroglancerAnnotationBase(NeuroglancerAnnotationBase::BaseType::PROPERTY),
m_description(description),
m_dataType(dataType),
m_value(value),
m_labelText(labelText)
{
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    
    AString dataText;
    switch (m_dataType) {
        case NeuroglancerAnnotationPropertyDataTypeEnum::INVALID:
            dataText = m_value.toString();
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::RGBA:
        {
            QColor color = m_value.value<QColor>();
            dataText = QColorToString(color);
        }
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::UNSIGNED_INTEGER:
            dataText = AString::number(m_value.toUInt());
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::INTEGER:
            dataText = AString::number(m_value.toInt());
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::FLOAT:
            dataText = AString::number(m_value.toFloat(), 'f', 3);
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::LABEL:
            dataText = m_labelText;
            break;
    }
    
    setText(dataText);
}

/**
 * Destructor.
 */
NeuroglancerAnnotationPropertyValue::~NeuroglancerAnnotationPropertyValue()
{
    std::cout << "Annotation Property Value destroyed" << std::endl;
}

/**
 * @return String representation of a QColor
 * @param color
 *   The QColor
 */
AString
NeuroglancerAnnotationPropertyValue::QColorToString(const QColor& color)
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
NeuroglancerAnnotationPropertyValue::toString() const
{
    AString txt("Description=\"" + m_description
                + "\", DataType=" + NeuroglancerAnnotationPropertyDataTypeEnum::toGuiName(m_dataType)
                + ", Value=" + text());
    
    switch (m_dataType) {
        case NeuroglancerAnnotationPropertyDataTypeEnum::INVALID:
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::RGBA:
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::UNSIGNED_INTEGER:
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::INTEGER:
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::FLOAT:
            break;
        case NeuroglancerAnnotationPropertyDataTypeEnum::LABEL:
            txt += (", Label=" + m_labelText);
            break;
    }

    return txt;
}


/**
 * @param The data type
 */
NeuroglancerAnnotationPropertyDataTypeEnum::Enum
NeuroglancerAnnotationPropertyValue::getDataType() const
{
    return m_dataType;
}

/**
 * @return Description of the property
 */
const AString&
NeuroglancerAnnotationPropertyValue::getDescription() const
{
    return m_description;
}


/**
 * @return The data value
 */
const QVariant&
NeuroglancerAnnotationPropertyValue::getValue() const
{
    return m_value;
}


/**
 * @return Text for when data type is label
 */
const AString&
NeuroglancerAnnotationPropertyValue::getLabelText() const
{
    return m_labelText;
}


