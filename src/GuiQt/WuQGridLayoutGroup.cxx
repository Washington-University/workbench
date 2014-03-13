
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

#include <QGridLayout>
#include <QWidget>

#define __WU_Q_GRID_LAYOUT_GROUP_DECLARE__
#include "WuQGridLayoutGroup.h"
#undef __WU_Q_GRID_LAYOUT_GROUP_DECLARE__

using namespace caret;


    
/**
 * \class caret::WuQGridLayoutGroup 
 * \brief Controls display of a group of widgets in a QGridLayout
 *
 * When displaying QWidgets in a QGridLayout one may want to 
 * hide the widgets in a row.  However, even though the widgets
 * are hidden and QGridLayout removes the space occupied by
 * the widgets, it seems to keep the spacing around the widgets
 * and the layout does not full shrink.
 *
 * This group will remove and add the widgets as the group
 * is set visible or hidden.  This seems to remove the extra
 * spacing and allows the QGridLayout to fully shrink.
 */

/**
 * Constructor.
 * @param gridLayout
 *    Grid layout of the widgets.
 * @param parent
 *    Parent object.
 */
WuQGridLayoutGroup::WuQGridLayoutGroup(QGridLayout* gridLayout,
                                       QObject* parent)
: QObject(parent)
{
    this->gridLayout = gridLayout;
    this->areWidgetsInLayout = true;
}

/**
 * Destructor.
 */
WuQGridLayoutGroup::~WuQGridLayoutGroup()
{
    const int numItems = this->layoutItems.size();
    for (int i = 0; i < numItems; i++) {
        delete this->layoutItems[i];
    }
    this->layoutItems.clear();
}

/**
 * Add a widget to the group.
 * @param widget
 *    Widget added to the layout
 * @param row
 *    Row for widget.
 * @param column
 *    Column for widget.
 * @param alignment
 *    Alignment of widget.
 */
void 
WuQGridLayoutGroup::addWidget(QWidget* widget,
               int row,
               int column,
               Qt::Alignment alignment)
{
    this->addWidget(widget, 
                    row, 
                    column, 
                    1, 
                    1, 
                    alignment);
}

/**
 * Add a widget to the group.
 * @param widget
 *    Widget added to the layout
 * @param fromRow
 *    Row for widget.
 * @param fromColumn
 *    Column for widget.
 * @param rowSpan
 *    Rows for widget.
 * @param columnSpan
 *    Columns for widget.
 * @param alignment
 *    Alignment of widget.
 */
void 
WuQGridLayoutGroup::addWidget(QWidget* widget,
               int fromRow,
               int fromColumn,
               int rowSpan,
               int columnSpan,
               Qt::Alignment alignment)
{
    ItemRowCol* irc = new ItemRowCol(widget,
                                     fromRow,
                                     fromColumn,
                                     rowSpan,
                                     columnSpan,
                                     alignment);
    
    this->layoutItems.push_back(irc);
    
    this->gridLayout->addWidget(irc->widget, 
                                irc->fromRow, 
                                irc->fromColumn, 
                                irc->rowSpan, 
                                irc->columnSpan, 
                                irc->alignment);
}

/**
 * Set the visibility of the widgets in the group.
 */
void 
WuQGridLayoutGroup::setVisible(bool visible)
{
    if (visible) {
        if (this->areWidgetsInLayout) {
            return;
        }
        
        const int numItems = this->layoutItems.size();
        for (int i = 0; i < numItems; i++) {
            ItemRowCol* irc = this->layoutItems[i];
            this->gridLayout->addWidget(irc->widget, 
                                        irc->fromRow, 
                                        irc->fromColumn, 
                                        irc->rowSpan, 
                                        irc->columnSpan, 
                                        irc->alignment);
            irc->widget->setVisible(true);
        }
        
        this->areWidgetsInLayout = true;
    }
    else {
        if (this->areWidgetsInLayout) {
            
            const int numItems = this->layoutItems.size();
            for (int i = 0; i < numItems; i++) {
                ItemRowCol* irc = this->layoutItems[i];
                irc->widget->setVisible(false);
                this->gridLayout->removeWidget(irc->widget);
            }
            
            this->areWidgetsInLayout = false;
        }
    }
}

/**
 * Constructor for widget.
 *
 * @param widget
 *    Widget added to the layout
 * @param fromRow
 *    Row for widget.
 * @param fromColumn
 *    Column for widget.
 * @param rowSpan
 *    Rows for widget.
 * @param columnSpan
 *    Columns for widget.
 * @param alignment
 *    Alignment of widget.
 */
WuQGridLayoutGroup::ItemRowCol::ItemRowCol(QWidget *widget,
                                           int fromRow,
                                           int fromColumn,
                                           int rowSpan,
                                           int columnSpan,
                                           Qt::Alignment alignment)
{
    this->widget = widget;
    this->fromRow = fromRow;
    this->fromColumn = fromColumn;
    this->rowSpan = rowSpan;
    this->columnSpan = columnSpan;
    this->alignment = alignment;
}

/**
 * @return Number of rows in grid layout.
 */
int 
WuQGridLayoutGroup::rowCount() const
{
    return this->gridLayout->rowCount();
}

/**
 * @return Number of columns in grid layout.
 */
int 
WuQGridLayoutGroup::columnCount() const
{
    return this->gridLayout->columnCount();
}


