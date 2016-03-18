
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

#define __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_DECLARE__
#include "DisplayGroupAndTabItemViewController.h"
#undef __DISPLAY_GROUP_AND_TAB_ITEM_VIEW_CONTROLLER_DECLARE__

#include <QTreeWidget>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "DisplayGroupAndTabItemInterface.h"
#include "DisplayGroupAndTabItemTreeWidgetItem.h"

using namespace caret;


    
/**
 * \class caret::DisplayGroupAndTabItemViewController 
 * \brief View controller for display group and tab item hierarchy
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * 
 * @param parent
 *     The parent widget.
 */
DisplayGroupAndTabItemViewController::DisplayGroupAndTabItemViewController(const int32_t browserWindowIndex,
                                                                           QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setHeaderHidden(true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_treeWidget, 100);
}

/**
 * Destructor.
 */
DisplayGroupAndTabItemViewController::~DisplayGroupAndTabItemViewController()
{
}

/**
 * Update the content.
 *
 * @param displayGroupAndTabItem
 *     Display group for this instance.
 */
void
DisplayGroupAndTabItemViewController::updateContent(std::vector<DisplayGroupAndTabItemInterface*>& contentItems,
                                                    const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex)
{
    m_contentItems = contentItems;
    m_contentItemWidgets.clear();
    
    m_treeWidget->clear();

    for (std::vector<DisplayGroupAndTabItemInterface*>::iterator itemIterator = m_contentItems.begin();
         itemIterator != m_contentItems.end();
         itemIterator++) {
        DisplayGroupAndTabItemTreeWidgetItem* itemWidget = new DisplayGroupAndTabItemTreeWidgetItem(*itemIterator);
        m_treeWidget->addTopLevelItem(itemWidget);
        itemWidget->updateContent(displayGroup,
                                  tabIndex);
        m_contentItemWidgets.push_back(itemWidget);
    }
}

