
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

#define __DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_DECLARE__
#include "DisplayGroupAndTabItemTreeWidgetItem.h"
#undef __DISPLAY_GROUP_AND_TAB_ITEM_TREE_WIDGET_ITEM_DECLARE__

#include <QPainter>
#include <QPen>

#include "CaretAssert.h"
#include "DisplayGroupAndTabItemInterface.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::DisplayGroupAndTabItemTreeWidgetItem 
 * \brief Item for display group and tab selection hierarchy
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param displayGroupAndTabItem
 *     Item for display in this instance.
 */
DisplayGroupAndTabItemTreeWidgetItem::DisplayGroupAndTabItemTreeWidgetItem(DisplayGroupAndTabItemInterface *displayGroupAndTabItem)
: QTreeWidgetItem(),
m_displayGroupAndTabItem(displayGroupAndTabItem),
m_displayGroup(DisplayGroupEnum::DISPLAY_GROUP_A)
{
    CaretAssert(m_displayGroupAndTabItem);
    
    setText(NAME_COLUMN,
            m_displayGroupAndTabItem->getItemName());
    
    std::vector<DisplayGroupAndTabItemInterface*> itemChildren = m_displayGroupAndTabItem->getItemChildren();
    for (std::vector<DisplayGroupAndTabItemInterface*>::iterator childIter = itemChildren.begin();
         childIter != itemChildren.end();
         childIter++) {
        DisplayGroupAndTabItemInterface* itemChild = *childIter;
        
        DisplayGroupAndTabItemTreeWidgetItem* childWidget = new DisplayGroupAndTabItemTreeWidgetItem(itemChild);
        addChild(childWidget);
        
        m_childWidgets.push_back(childWidget);
    }
}

/**
 * Destructor.
 */
DisplayGroupAndTabItemTreeWidgetItem::~DisplayGroupAndTabItemTreeWidgetItem()
{
}

/**
 * Update the content of this widget.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     The tab index.
 */
void
DisplayGroupAndTabItemTreeWidgetItem::updateContent(const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex)
{
    m_displayGroup = displayGroup;
    m_tabIndex     = tabIndex;
    
    Qt::CheckState qtCheckState = toQCheckState(m_displayGroupAndTabItem->getItemSelected(m_displayGroup,
                                                                                          m_tabIndex));
    setCheckState(NAME_COLUMN,
                  qtCheckState);
    
    setItemIcon();
    
    for (std::vector<DisplayGroupAndTabItemTreeWidgetItem*>::iterator childIter = m_childWidgets.begin();
         childIter != m_childWidgets.end();
         childIter++) {
        DisplayGroupAndTabItemTreeWidgetItem* child = *childIter;
        child->updateContent(displayGroup,
                             tabIndex);
    }

    const bool expandedFlag = m_displayGroupAndTabItem->isItemExpanded(m_displayGroup,
                                                                       m_tabIndex);
    std::cout << "Expanded flag: " << qPrintable(AString::fromBool(expandedFlag)) << std::endl;
    setExpanded(expandedFlag);
}

/**
 * Set the icon for this item.
 */
void
DisplayGroupAndTabItemTreeWidgetItem::setItemIcon()
{
    CaretAssert(m_displayGroupAndTabItem);
    
    float backgroundRGBA[4];
    float outlineRGBA[4];
    float textRGBA[4];
    m_displayGroupAndTabItem->getItemIconColorsRGBA(backgroundRGBA,
                                                    outlineRGBA,
                                                    textRGBA);
    if ((backgroundRGBA[3] > 0.0)
        || (outlineRGBA[3] > 0.0)
        || (textRGBA[3] > 0.0)) {
        
        AString msg(m_displayGroupAndTabItem->getItemName()
                    + " background="
                    + AString::fromNumbers(backgroundRGBA, 4, ",")
                    + " outline="
                    + AString::fromNumbers(outlineRGBA, 4, ",")
                    + " text="
                    + AString::fromNumbers(textRGBA, 4, ","));
        std::cout << qPrintable(msg) << std::endl;
        const AString msg2("   Alphas="
                           + AString::number(backgroundRGBA[3], 'f', 6)
                           + ","
                           + AString::number(outlineRGBA[3], 'f', 6)
                           + ","
                           + AString::number(textRGBA[3], 'f', 6));
        std::cout << qPrintable(msg2) << std::endl;
        
        const int pixmapSize = 24;
        QPixmap pixmap(pixmapSize,
                       pixmapSize);
        
        QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapPainterOriginBottomLeft(pixmap,
                                                                                              backgroundRGBA);
        if (outlineRGBA[3] > 0.0) {
            QPen pen = painter->pen();
            
            QColor outlineColor = QColor::fromRgbF(outlineRGBA[0],
                                                   outlineRGBA[1],
                                                   outlineRGBA[2]);
            painter->fillRect(0, 0, 3, pixmapSize, outlineColor);
            painter->fillRect(pixmapSize - 3, 0, 3, pixmapSize, outlineColor);
            painter->fillRect(0, 0, pixmapSize, 3, outlineColor);
            painter->fillRect(0, pixmapSize - 3, pixmapSize, 3, outlineColor);
        }
        
        if (textRGBA[3] > 0.0) {
            QColor textColor = QColor::fromRgbF(textRGBA[0],
                                                textRGBA[1],
                                                textRGBA[2]);
            const int rectSize = 8;
            const int cornerXY = (pixmapSize / 2) - (rectSize / 2);
            painter->fillRect(cornerXY, cornerXY, rectSize, rectSize, textColor);
        }
        
        setIcon(NAME_COLUMN,
                QIcon(pixmap));
    }
    else {
        setIcon(NAME_COLUMN,
                QIcon());
    }
}

/**
 * Convert the tri state selection status to Qt::CheckState
 *
 * @param triStateStatus
 *     The tri state selection status.
 * @return
 *     Qt::CheckState status.
 */
Qt::CheckState
DisplayGroupAndTabItemTreeWidgetItem::toQCheckState(const TriStateSelectionStatusEnum::Enum triStateStatus)
{
    switch (triStateStatus) {
        case TriStateSelectionStatusEnum::PARTIALLY_SELECTED:
            return Qt::PartiallyChecked;
            break;
        case TriStateSelectionStatusEnum::SELECTED:
            return Qt::Checked;
            break;
        case TriStateSelectionStatusEnum::UNSELECTED:
            return Qt::Unchecked;
            break;
    }

    return Qt::Unchecked;
}
