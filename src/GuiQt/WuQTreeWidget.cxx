
/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <iostream>

#include <QHeaderView>

#define __WU_Q_TREE_WIDGET_DECLARE__
#include "WuQTreeWidget.h"
#undef __WU_Q_TREE_WIDGET_DECLARE__

using namespace caret;


    
/**
 * \class caret::WuQTreeWidget 
 * \brief Tree Widget that can size to its content's size.
 *
 * A QTreeWidget normally gets a size hint of (256, 256) and
 * does not increase in size when the scroll bars are turned
 * off.  If fitToContentSizeWithoutScrollBars() is called
 * this tree widget will resize to the size of its content.
 */
/**
 * Constructor.
 * @param allowResizeFlag
 *    Will fit size to fit content
 * @param parent
 *    The parent widget
 */
WuQTreeWidget::WuQTreeWidget(QWidget* parent)
: QTreeWidget(parent)
{
    this->setHeaderHidden(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    
    QObject::connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                     this, SLOT(itemExpandedOrCollapsed(QTreeWidgetItem*)));
    QObject::connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(itemExpandedOrCollapsed(QTreeWidgetItem*)));
}

/**
 * Destructor.
 */
WuQTreeWidget::~WuQTreeWidget()
{
    
}

/**
 * Called when an item is expanded or collapsed to
 * update the fixed size.
 */
void 
WuQTreeWidget::itemExpandedOrCollapsed(QTreeWidgetItem*)
{
    this->resizeToFitContent();
}

/**
 * Size the widget so that it is the size of
 * its content's without any scroll bars.
 */
void 
WuQTreeWidget::resizeToFitContent()
{
    const int height = this->calculateHeight() + 6;
    this->setFixedHeight(height);
    
    int totalColumnWidths = 20; // space for arrows
    const int numCols = this->columnCount();
    for (int i = 0; i < numCols; i++) {
        totalColumnWidths += this->sizeHintForColumn(i);
    }
    if (totalColumnWidths < 256) {
        totalColumnWidths = 256;
    }
    this->setFixedWidth(totalColumnWidths);
}



/*
 * From http://qt-project.org/forums/viewthread/2533
 */
int
WuQTreeWidget::calculateHeight() const
{
    int h = 0;
   
    int topLevelCount = this->topLevelItemCount();
 
    for (int i = 0;i < topLevelCount;i++) {
        QTreeWidgetItem * item = topLevelItem(i);
        h += this->calculateHeightRec(item);
    }
   
    if (h != 0) {
        h += header()->sizeHint().height();
    }
    
    return h;
}
 
/*
 *
 */
int WuQTreeWidget::calculateHeightRec(QTreeWidgetItem * item) const
{
    if (item == NULL)
        return 0;
   
    QModelIndex index = indexFromItem(item);
 
    if (item->isExpanded() == false) {
        int h = rowHeight(index);
        return h;
    }
 
    /*
    int h = 0;
    for (int j = 0; j < item->columnCount(); j++) {
        const int itemHeight = item->sizeHint(j).height() + 2;
        if (itemHeight > h) {
            h = itemHeight;
        }
    }
    */
    //int h = item->sizeHint(0).height() + 2;
    //std::cout << "EXPANDED h=" << h << " rowSizeHint=" << indexRowSizeHint(index);
    int h = indexRowSizeHint(index);
    int childCount = item->childCount();
    for (int i = 0; i < childCount;i++)
    {
        h += this->calculateHeightRec(item->child(i));
    }
   
    return h;
}

/*
 * In Qt 5 this function is protected, in Qt 6 this function is public
 * QModelIndex    indexFromItem(const QTreeWidgetItem *item, int column = 0) const;
 *
 * @return the QModelIndex associated with the given item in the given column.
 * @param item
 *    The widget item
 * @param column
 *    The column
 */
QModelIndex 
WuQTreeWidget::getIndexFromItem(const QTreeWidgetItem *item, int column) const
{
    return indexFromItem(item,
                         column);
}

/*
 * In Qt 5 this function is protected, in Qt 6 this function is public
 * QTreeWidgetItem *QTreeWidget::itemFromIndex(const QModelIndex &index) const
 *
 * @return a pointer to the QTreeWidgetItem associated with the given index.
 * @param index
 *   The item's model index.
 */
QTreeWidgetItem*    
WuQTreeWidget::getItemFromIndex(const QModelIndex &index) const
{
    return itemFromIndex(index);
}


