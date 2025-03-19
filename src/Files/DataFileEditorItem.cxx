
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __DATA_FILE_EDITOR_ITEM_DECLARE__
#include "DataFileEditorItem.h"
#undef __DATA_FILE_EDITOR_ITEM_DECLARE__

#include <QCollator>

#include "Annotation.h"
#include "Border.h"
#include "CaretAssert.h"
#include "Focus.h"
#include "Vector3D.h"

using namespace caret;

/**
 * \class caret::DataFileEditorItem 
 * \brief Item for data file editor
 * \ingroup Files
 */

/**
 * Constructor for an annotation.
 * @param dataItemType
 *    Type of the data item
 * @param annotation
 *    Annotation for display in the editor.
 * @param text
 *    Text displayed by this item
 * @param sortingKeyText
 *    Text used when sorting by this item
 * @param iconRGBA
 *    Color used for this item's optional icon
 */
DataFileEditorItem::DataFileEditorItem(const DataFileEditorItemTypeEnum::Enum dataItemType,
                                       std::shared_ptr<Annotation> annotation,
                                       const AString& text,
                                       const AString& sortingKeyText,
                                       const float iconRGBA[4])
: QStandardItem(),
m_dataItemType(dataItemType)
{
    m_annotation = annotation;
    setText(text);
    m_sortingKeyText = sortingKeyText;
    
    switch (m_dataItemType) {
        case DataFileEditorItemTypeEnum::CLASS_NAME:
            break;
        case DataFileEditorItemTypeEnum::COORDINATES:
            break;
        case DataFileEditorItemTypeEnum::GROUP_NAME:
            break;
        case DataFileEditorItemTypeEnum::IDENTIFIER:
            break;
        case DataFileEditorItemTypeEnum::NAME:
            setIcon(createIcon(iconRGBA));
            break;
    }
}

/**
 * Constructor for a border.
 * @param dataItemType
 *    Type of the data item
 * @param border
 *    Border for display in the editor.
 * @param text
 *    Text displayed by this item
 * @param sortingKeyText
 *    Text used when sorting by this item
 * @param iconRGBA
 *    Color used for this item's optional icon
 */
DataFileEditorItem::DataFileEditorItem(const DataFileEditorItemTypeEnum::Enum dataItemType,
                                       std::shared_ptr<Border> border,
                                       const AString& text,
                                       const AString& sortingKeyText,
                                       const float iconRGBA[4])
: QStandardItem(),
m_dataItemType(dataItemType)
{
    m_border = border;
    setText(text);
    m_sortingKeyText = sortingKeyText;
    
    switch (m_dataItemType) {
        case DataFileEditorItemTypeEnum::CLASS_NAME:
            setIcon(createIcon(iconRGBA));
            break;
        case DataFileEditorItemTypeEnum::COORDINATES:
            break;
        case DataFileEditorItemTypeEnum::GROUP_NAME:
            break;
        case DataFileEditorItemTypeEnum::IDENTIFIER:
            break;
        case DataFileEditorItemTypeEnum::NAME:
            setIcon(createIcon(iconRGBA));
            break;
    }
}

/**
 * Constructor for a focus.
 * @param dataItemType
 *    Type of the data item
 * @param focus
 *    Focus for display in the editor.
 * @param text
 *    Text displayed by this item
 * @param sortingKeyText
 *    Text used when sorting by this item
 * @param iconRGBA
 *    Color used for this item's optional icon
 */
DataFileEditorItem::DataFileEditorItem(const DataFileEditorItemTypeEnum::Enum dataItemType,
                                       std::shared_ptr<Focus> focus,
                                       const AString& text,
                                       const AString& sortingKeyText,
                                       const float iconRGBA[4])
: QStandardItem(),
m_dataItemType(dataItemType)
{
    m_focus = focus;
    setText(text);
    m_sortingKeyText = sortingKeyText;

    switch (m_dataItemType) {
        case DataFileEditorItemTypeEnum::CLASS_NAME:
            setIcon(createIcon(iconRGBA));
            break;
        case DataFileEditorItemTypeEnum::COORDINATES:
            break;
        case DataFileEditorItemTypeEnum::GROUP_NAME:
            break;
        case DataFileEditorItemTypeEnum::IDENTIFIER:
            break;
        case DataFileEditorItemTypeEnum::NAME:
            setIcon(createIcon(iconRGBA));
            break;
    }
}

/**
 * Destructor.
 */
DataFileEditorItem::~DataFileEditorItem()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
DataFileEditorItem::DataFileEditorItem(const DataFileEditorItem& obj)
: QStandardItem(obj),
m_dataItemType(obj.m_dataItemType)
{
    setText(obj.text());
    
    switch (m_dataItemType) {
        case DataFileEditorItemTypeEnum::CLASS_NAME:
            break;
        case DataFileEditorItemTypeEnum::COORDINATES:
            break;
        case DataFileEditorItemTypeEnum::GROUP_NAME:
            break;
        case DataFileEditorItemTypeEnum::IDENTIFIER:
            break;
        case DataFileEditorItemTypeEnum::NAME:
            break;
    }
    
    /*
     * Copy icon if it is valid
     */
    if ( ! icon().isNull()) {
        setIcon(icon());
    }

    if (obj.m_annotation) {
        m_annotation.reset(obj.m_annotation->clone());
    }
    if (obj.m_border) {
        m_border.reset(new Border(*obj.m_border));
    }
    if (obj.m_focus) {
        m_focus.reset(new Focus(*(obj.m_focus)));
    }
}

/**
 * @return A copy of this item
 */
QStandardItem*
DataFileEditorItem::clone() const
{
    return new DataFileEditorItem(*this);
}

/**
 * @return QStandardItem::Type of this item
 */
int
DataFileEditorItem::type() const
{
    return DataFileEditorItemTypeEnum::toQStandardItemType(m_dataItemType);
}

/**
 * @return true if this item is less than other; otherwise returns false.
 * @param other
 *   Other item for comparison
 *
 */
bool
DataFileEditorItem::operator<(const QStandardItem &other) const
{
    if (s_collator == NULL) {
        s_collator = new QCollator();
        s_collator->setNumericMode(true);
        s_collator->setCaseSensitivity(Qt::CaseSensitive);
    }
    CaretAssert(s_collator);
    
    const DataFileEditorItem* otherItem(dynamic_cast<const DataFileEditorItem*>(&other));
    CaretAssert(otherItem);
    
    const int32_t result(s_collator->compare(m_sortingKeyText,
                                             otherItem->m_sortingKeyText));
    if (result < 0) {
        return true;
    }
    return false;
}

/**
 * @return Icon for the given color
 * @param rgba
 *    The color
 */
QIcon
DataFileEditorItem::createIcon(const float rgba[4]) const
{
    QPixmap pixmap(16, 16);
    pixmap.fill(QColor((int)(rgba[0] * 255.0),
                       (int)(rgba[1] * 255.0),
                       (int)(rgba[2] * 255.0)));
    return QIcon(pixmap);
}

/**
 * @return Annotation in this item
 */
const Annotation*
DataFileEditorItem::getAnnotation() const
{
    return m_annotation.get();
}

/**
 * @return Border in this item
 */
const Border*
DataFileEditorItem::getBorder() const
{
    return m_border.get();
}

/**
 * @return Focus in this item
 */
const Focus*
DataFileEditorItem::getFocus() const
{
    return m_focus.get();
}

/**
 * @return Sample in this item
 */
const Annotation*
DataFileEditorItem::getSample() const
{
    return m_annotation.get();
}


