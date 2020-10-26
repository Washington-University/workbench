
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
#include <QTreeWidget>

#include "CaretAssert.h"
#include "CaretLogger.h"
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
 */
DisplayGroupAndTabItemTreeWidgetItem::DisplayGroupAndTabItemTreeWidgetItem(const int32_t browserWindowIndex)
: QTreeWidgetItem(),
m_browserWindowIndex(browserWindowIndex)
{
    m_displayGroup = DisplayGroupEnum::DISPLAY_GROUP_A;
    m_tabIndex     = -1;
}


/**
 * Destructor.
 */
DisplayGroupAndTabItemTreeWidgetItem::~DisplayGroupAndTabItemTreeWidgetItem()
{
}

/**
 * @return A deep copy of the item.
 */
QTreeWidgetItem*
DisplayGroupAndTabItemTreeWidgetItem::clone() const
{
    const QString msg("Cloning of DisplayGroupAndTabItemTreeWidgetItem not allowed.");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
    
    return NULL;
}


/**
 * Update the content of this widget.
 *
 * @param displayGroupAndTabItem
 *     Display group and tab item for this instance.
 * @param treeWidget
 *     Tree widget that owns this item.
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     The tab index.
 */
void
DisplayGroupAndTabItemTreeWidgetItem::updateContent(DisplayGroupAndTabItemInterface *displayGroupAndTabItem,
                                                    QTreeWidget* treeWidget,
                                                    const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex)
{
    CaretAssert(displayGroupAndTabItem);
    
    m_displayGroup = displayGroup;
    m_tabIndex     = tabIndex;
    
    setText(NAME_COLUMN,
            displayGroupAndTabItem->getItemName());
    
    setDisplayGroupAndTabItem(displayGroupAndTabItem);
    
    Qt::CheckState qtCheckState = toQCheckState(displayGroupAndTabItem->getItemDisplaySelected(m_displayGroup,
                                                                        m_tabIndex));
    setCheckState(NAME_COLUMN,
                  qtCheckState);
    
    setItemIcon(treeWidget,
                displayGroupAndTabItem);
    
    const int32_t numExistingChildren = childCount();
    const int32_t numValidChildren    = displayGroupAndTabItem->getNumberOfItemChildren();
    
    const int32_t numberOfChildrenToAdd = numValidChildren - numExistingChildren;
    for (int32_t i = 0; i < numberOfChildrenToAdd; i++) {
        addChild(new DisplayGroupAndTabItemTreeWidgetItem(m_browserWindowIndex));
    }
    
    CaretAssert(childCount() >= numValidChildren);
    
    for (int32_t i = 0; i < numValidChildren; i++) {
        QTreeWidgetItem* treeWidgetChild = child(i);
        
        if (i < numValidChildren) {
            treeWidgetChild->setHidden(false);
            DisplayGroupAndTabItemTreeWidgetItem* dgtChild = dynamic_cast<DisplayGroupAndTabItemTreeWidgetItem*>(treeWidgetChild);
            CaretAssert(dgtChild);
            dgtChild->updateContent(displayGroupAndTabItem->getItemChild(i),
                                    treeWidget,
                                    displayGroup,
                                    tabIndex);
            
        }
        else {
            treeWidgetChild->setHidden(true);
            setDisplayGroupAndTabItem(NULL);
        }
    }
    
//    const bool expandedFlag = displayGroupAndTabItem->isItemExpanded(m_displayGroup,
//                                                                     m_tabIndex);
//    setExpanded(expandedFlag);
    
//    setSelected(displayGroupAndTabItem->isItemSelectedForEditingInWindow())
    for (int32_t i = (numExistingChildren - 1); i >= numValidChildren; i--) {
        /*
         * Take removes it from the parent but
         * does not destruct it.
         */
        QTreeWidgetItem* item = takeChild(i);
        delete item;
    }
}

/**
 * @return The data item in this tree widget item (may be NULL).
 */
DisplayGroupAndTabItemInterface*
DisplayGroupAndTabItemTreeWidgetItem::getDisplayGroupAndTabItem() const
{
    DisplayGroupAndTabItemInterface* myItem = NULL;
    void* myDataPtr = data(NAME_COLUMN, Qt::UserRole).value<void*>();
    if (myDataPtr != NULL) {
        myItem = (DisplayGroupAndTabItemInterface*)myDataPtr;
        CaretAssert(myItem);
    }
    return myItem;
}

/**
 * Set the data item in this tree widget item.
 *
 * @param displayGroupAndTabItem
 *     The data item (may be NULL).
 */
void
DisplayGroupAndTabItemTreeWidgetItem::setDisplayGroupAndTabItem(DisplayGroupAndTabItemInterface* displayGroupAndTabItem)
{
    setData(NAME_COLUMN,
            Qt::UserRole,
            QVariant::fromValue<void*>(displayGroupAndTabItem));
}



/**
 * Set the icon for this item.
 */
void
DisplayGroupAndTabItemTreeWidgetItem::setItemIcon(QTreeWidget* treeWidget,
                                                  DisplayGroupAndTabItemInterface* myDataItem)
{
    CaretAssert(myDataItem);
    
    float backgroundRGBA[4];
    float outlineRGBA[4];
    float textRGBA[4];
    myDataItem->getItemIconColorsRGBA(backgroundRGBA,
                                                    outlineRGBA,
                                                    textRGBA);
    if ((backgroundRGBA[3] > 0.0)
        || (outlineRGBA[3] > 0.0)
        || (textRGBA[3] > 0.0)) {
        
        const int pixmapSize = 24;
        QPixmap pixmap(pixmapSize,
                       pixmapSize);
        
        QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(treeWidget,
                                                                                              pixmap);
        if (backgroundRGBA[3] > 0.0) {
            painter->fillRect(pixmap.rect(),
                              QColor::fromRgbF(backgroundRGBA[0],
                                               backgroundRGBA[1],
                                               backgroundRGBA[2]));
        }
        
        if (outlineRGBA[3] > 0.0) {
            QPen pen = painter->pen();
            
            QColor outlineColor = QColor::fromRgbF(outlineRGBA[0],
                                                   outlineRGBA[1],
                                                   outlineRGBA[2]);
            painter->fillRect(0, 0, 3, pixmapSize, outlineColor);
            painter->fillRect(pixmapSize - 3, 0, 3, pixmapSize, outlineColor);
            painter->fillRect(0, 0, pixmapSize, 3, outlineColor);
            painter->fillRect(0, pixmapSize - 4, pixmapSize, 3, outlineColor);
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
 * Update the selected and expanded checkboxes.
 *
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab.
 */
void
DisplayGroupAndTabItemTreeWidgetItem::updateSelectedAndExpandedCheckboxes(const DisplayGroupEnum::Enum displayGroup,
                                                                          const int32_t tabIndex)
{
    const int32_t numChildren = childCount();
    for (int32_t iChild = 0; iChild < numChildren; iChild++) {
        QTreeWidgetItem* treeChild = child(iChild);
        CaretAssert(treeChild);
        
        DisplayGroupAndTabItemTreeWidgetItem* item = dynamic_cast<DisplayGroupAndTabItemTreeWidgetItem*>(treeChild);
        CaretAssert(item);
        
        DisplayGroupAndTabItemInterface* data = item->getDisplayGroupAndTabItem();
        if (data != NULL) {
            item->updateSelectedAndExpandedCheckboxes(displayGroup,
                                                      tabIndex);
        }
    }

    DisplayGroupAndTabItemInterface* myData = getDisplayGroupAndTabItem();
    if (myData != NULL) {
        Qt::CheckState checkState = toQCheckState(myData->getItemDisplaySelected(displayGroup,
                                                                          tabIndex));
        setCheckState(NAME_COLUMN,
                      checkState);
        
        setExpanded(myData->isItemExpanded(displayGroup,
                                           tabIndex));
        
        setSelected(myData->isItemSelectedForEditingInWindow(m_browserWindowIndex));
    }
}

/**
 * Convert QCheckState to GroupAndNameCheckStateEnum
 * @param checkState
 *    The QCheckState
 * @return GroupAndNameCheckStateEnum converted from QCheckState
 */
TriStateSelectionStatusEnum::Enum
DisplayGroupAndTabItemTreeWidgetItem::fromQCheckState(const Qt::CheckState checkState)
{
    switch (checkState) {
        case Qt::Unchecked:
            return TriStateSelectionStatusEnum::UNSELECTED;
            break;
        case Qt::PartiallyChecked:
            return TriStateSelectionStatusEnum::PARTIALLY_SELECTED;
            break;
        case Qt::Checked:
            return TriStateSelectionStatusEnum::SELECTED;
            break;
    }
    
    return TriStateSelectionStatusEnum::UNSELECTED;
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
