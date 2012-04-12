
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
 */
WuQTreeWidget::WuQTreeWidget(QWidget* parent)
: QTreeWidget(parent)
{
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
