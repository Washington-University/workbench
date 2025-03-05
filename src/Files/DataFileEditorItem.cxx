
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
 * @param colorTable
 *    Color table for coloring by name
 */
DataFileEditorItem::DataFileEditorItem(const ItemType dataItemType,
                                       std::shared_ptr<Annotation> annotation,
                                       const AString& text,
                                       const float iconRGBA[4])
: QStandardItem(),
m_dataItemType(dataItemType)
{
    m_annotation = annotation;
    setText(text);
    switch (m_dataItemType) {
        case ItemType::NAME:
            setIcon(createIcon(iconRGBA));
            break;
        case ItemType::CLASS:
            break;
        case ItemType::XYZ:
            break;
    }
}

/**
 * Constructor for a border.
 * @param dataItemType
 *    Type of the data item
 * @param border
 *    Border for display in the editor.
 * @param colorTable
 *    Color table for coloring by name
 */
DataFileEditorItem::DataFileEditorItem(const ItemType dataItemType,
                                       std::shared_ptr<Border> border,
                                       const AString& text,
                                       const float iconRGBA[4])
: QStandardItem(),
m_dataItemType(dataItemType)
{
    m_border = border;
    setText(text);
    switch (m_dataItemType) {
        case ItemType::NAME:
            setIcon(createIcon(iconRGBA));
            break;
        case ItemType::CLASS:
            setIcon(createIcon(iconRGBA));
            break;
        case ItemType::XYZ:
            break;
    }
}

/**
 * Constructor for a focus.
 * @param dataItemType
 *    Type of the data item
 * @param focus
 *    Focus for display in the editor.
 * @param colorTable
 *    Color table for coloring by name
 */
DataFileEditorItem::DataFileEditorItem(const ItemType dataItemType,
                                       std::shared_ptr<Focus> focus,
                                       const AString& text,
                                       const float iconRGBA[4])
: QStandardItem(),
m_dataItemType(dataItemType)
{
    m_focus = focus;
    setText(text);
    switch (m_dataItemType) {
        case ItemType::NAME:
            setIcon(createIcon(iconRGBA));
            break;
        case ItemType::CLASS:
            setIcon(createIcon(iconRGBA));
            break;
        case ItemType::XYZ:
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
        case ItemType::NAME:
            setIcon(icon());
            break;
        case ItemType::CLASS:
            if ( ! icon().isNull()) {
                setIcon(icon());
            }
            break;
        case ItemType::XYZ:
            break;
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
 * @return Type of this item
 */
int
DataFileEditorItem::type() const
{
    return static_cast<int>(m_dataItemType);
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
    
    const Annotation* annotation(m_annotation.get());
    if (annotation != NULL) {
        const DataFileEditorItem* otherItem(dynamic_cast<const DataFileEditorItem*>(&other));
        CaretAssert(otherItem);
        const Annotation* otherAnnotation(otherItem->m_annotation.get());
        CaretAssert(otherAnnotation);
        
        CaretAssert(s_collator);
        switch (m_dataItemType) {
            case ItemType::NAME:
            {
                
                /*
                 * Sort by name and then class
                 */
                const int32_t nameResult(s_collator->compare(annotation->getName(),
                                                             otherAnnotation->getName()));
                if (nameResult < 0) {
                    return true;
                }
                else if (nameResult > 0) {
                    return false;
                }
//                return (s_collator->compare(this->text(1),
//                                            otherItem->text(1)));
            }
                break;
            case ItemType::CLASS:
            {
//                /*
//                 * Sort by class and then name
//                 */
//                const int32_t classResult(s_collator->compare(this->text(1),
//                                                              otherItem->text(1)));
//                if (classResult < 0) {
//                    return true;
//                }
//                else if (classResult > 0) {
//                    return false;
//                }
//                return (s_collator->compare(annotation->getName(),
//                                            otherAnnotation->getName()));
            }
                break;
            case ItemType::XYZ:
            {
                return (AnnotationCoordinateSpaceEnum::toGuiName(annotation->getCoordinateSpace())
                        < AnnotationCoordinateSpaceEnum::toGuiName(otherAnnotation->getCoordinateSpace()));
            }
                break;
        }
    }
    
    const Border* border(m_border.get());
    if (border != NULL) {
        const DataFileEditorItem* otherItem(dynamic_cast<const DataFileEditorItem*>(&other));
        CaretAssert(otherItem);
        const Border* otherBorder(otherItem->m_border.get());
        CaretAssert(otherBorder);
        
        CaretAssert(s_collator);
        switch (m_dataItemType) {
            case ItemType::NAME:
            {
                
                /*
                 * Sort by name and then class
                 */
                const int32_t nameResult(s_collator->compare(border->getName(),
                                                             otherBorder->getName()));
                if (nameResult < 0) {
                    return true;
                }
                else if (nameResult > 0) {
                    return false;
                }
                return (border->getClassName() < otherBorder->getClassName());
            }
                break;
            case ItemType::CLASS:
            {
                /*
                 * Sort by class and then name
                 */
                const int32_t classResult(s_collator->compare(border->getClassName(),
                                                              otherBorder->getClassName()));
                if (classResult < 0) {
                    return true;
                }
                else if (classResult > 0) {
                    return false;
                }
                return (border->getName() < otherBorder->getName());
            }
                break;
            case ItemType::XYZ:
            {
                /*
                 * Sort by XYZ
                 */
                if ((border->getNumberOfPoints() > 0)
                    && (otherItem->m_border->getNumberOfPoints() > 0)) {
                    Vector3D xyz;
                    m_border->getPoint(0)->getStereotaxicXYZ(xyz);
                    Vector3D otherXyz;
                    otherBorder->getPoint(0)->getStereotaxicXYZ(otherXyz);
                    return (xyz < otherXyz);
                }
            }
                break;
        }
    }
    
    const Focus* focus(m_focus.get());
    if (focus != NULL) {
        const DataFileEditorItem* otherItem(dynamic_cast<const DataFileEditorItem*>(&other));
        CaretAssert(otherItem);
        const Focus* otherFocus(otherItem->m_focus.get());
        CaretAssert(focus);
        
        CaretAssert(s_collator);
        switch (m_dataItemType) {
            case ItemType::NAME:
            {
                
                /*
                 * Sort by name and then class
                 */
                const int32_t nameResult(s_collator->compare(focus->getName(),
                                                             otherFocus->getName()));
                if (nameResult < 0) {
                    return true;
                }
                else if (nameResult > 0) {
                    return false;
                }
                return (focus->getClassName() < otherFocus->getClassName());
            }
                break;
            case ItemType::CLASS:
            {
                /*
                 * Sort by class and then name
                 */
                const int32_t classResult(s_collator->compare(focus->getClassName(),
                                                              otherFocus->getClassName()));
                if (classResult < 0) {
                    return true;
                }
                else if (classResult > 0) {
                    return false;
                }
                return (focus->getName() < otherFocus->getName());
            }
                break;
            case ItemType::XYZ:
            {
                /*
                 * Sort by XYZ
                 */
                if ((m_focus->getNumberOfProjections() > 0)
                    && (otherItem->m_focus->getNumberOfProjections() > 0)) {
                    Vector3D xyz;
                    m_focus->getProjection(0)->getStereotaxicXYZ(xyz);
                    Vector3D otherXyz;
                    otherFocus->getProjection(0)->getStereotaxicXYZ(otherXyz);
                    return (xyz < otherXyz);
                }
            }
                break;
        }
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


