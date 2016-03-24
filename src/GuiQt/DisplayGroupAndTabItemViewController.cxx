
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

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupAndTabItemInterface.h"
#include "DisplayGroupAndTabItemTreeWidgetItem.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"

using namespace caret;


    
/**
 * \class caret::DisplayGroupAndTabItemViewController 
 * \brief View controller for display group and tab item hierarchy
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * 
 * @param dataFileType
 *     Type of data file using this view controller.
 * @param browserWindowIndex
 *     The browser window containing this instance.
 * @param parent
 *     Parent of this instance.
 */
DisplayGroupAndTabItemViewController::DisplayGroupAndTabItemViewController(const DataFileTypeEnum::Enum dataFileType,
                                                                           const int32_t browserWindowIndex,
                                                                           QWidget* parent)
: QWidget(parent),
m_dataFileType(dataFileType),
m_browserWindowIndex(browserWindowIndex)
{
    m_treeWidget = new QTreeWidget();
    m_treeWidget->setHeaderHidden(true);
    //m_treeWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
    m_treeWidget->setSelectionMode(QTreeWidget::NoSelection);
    
    QObject::connect(m_treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                     this, SLOT(itemWasCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(itemWasExpanded(QTreeWidgetItem*)));
    QObject::connect(m_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(itemWasChanged(QTreeWidgetItem*, int)));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_treeWidget, 100);
    
    s_allViewControllers.insert(this);
}

/**
 * Destructor.
 */
DisplayGroupAndTabItemViewController::~DisplayGroupAndTabItemViewController()
{
    s_allViewControllers.erase(this);
}

/**
 * Gets called when an item is collapsed so that its children are not visible.
 *
 * @param item
 *    The QTreeWidgetItem that was collapsed.
 */
void
DisplayGroupAndTabItemViewController::itemWasCollapsed(QTreeWidgetItem* item)
{
    processItemExpanded(item,
                        false);
}

/**
 * Gets called when an item is expaned so that its children are visible.
 *
 * @param item
 *    The QTreeWidgetItem that was expanded.
 */
void
DisplayGroupAndTabItemViewController::itemWasExpanded(QTreeWidgetItem* item)
{
    processItemExpanded(item,
                        true);
}

/**
 * Called when an item is changed (checkbox selected/deselected).
 *
 * @param item
 *    The QTreeWidgetItem that was collapsed.
 * @param column
 *    Ignored.
 */
void
DisplayGroupAndTabItemViewController::itemWasChanged(QTreeWidgetItem* item,
                                                    int /*column*/)
{
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    int32_t tabIndex = -1;
    getDisplayGroupAndTabIndex(displayGroup, tabIndex);
    
    DisplayGroupAndTabItemInterface* dataItem = getDataItem(item);
    
    const Qt::CheckState checkState = item->checkState(DisplayGroupAndTabItemTreeWidgetItem::NAME_COLUMN);
    const TriStateSelectionStatusEnum::Enum itemCheckState = DisplayGroupAndTabItemTreeWidgetItem::fromQCheckState(checkState);
    dataItem->setItemDisplaySelected(displayGroup,
                              tabIndex,
                              itemCheckState);

    updateSelectedAndExpandedCheckboxes(displayGroup,
                                        tabIndex);
    updateSelectedAndExpandedCheckboxesInOtherViewControllers();
    
    updateGraphics();
}

/**
 * Process item expanded or collapsed.
 *
 * @param item
 *     The QTreeWidgetItem that was expanded or collapsed.
 * @param expandedStatus
 *     True if expanded, false if collapsed.
 */
void
DisplayGroupAndTabItemViewController::processItemExpanded(QTreeWidgetItem* item,
                                                          const bool expandedStatus)
{
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    int32_t tabIndex = -1;
    getDisplayGroupAndTabIndex(displayGroup, tabIndex);
    
    DisplayGroupAndTabItemInterface* dataItem = getDataItem(item);
    dataItem->setItemExpanded(displayGroup,
                              tabIndex,
                              expandedStatus);
    updateSelectedAndExpandedCheckboxes(displayGroup,
                                        tabIndex);
    updateSelectedAndExpandedCheckboxesInOtherViewControllers();
    
}

/**
 * Get the data item in the given tree widget item.
 * 
 * @param item
 *      The tree widget item.
 * @return
 *      The data item in the tree widget item.
 */
DisplayGroupAndTabItemInterface*
DisplayGroupAndTabItemViewController::getDataItem(QTreeWidgetItem* item) const
{
    DisplayGroupAndTabItemTreeWidgetItem* treeItem = dynamic_cast<DisplayGroupAndTabItemTreeWidgetItem*>(item);
    CaretAssert(treeItem);
    DisplayGroupAndTabItemInterface* dataItem = treeItem->getDisplayGroupAndTabItem();
    CaretAssert(dataItem);
    return dataItem;
}

/**
 * Get the display group and tab index currently active.
 */
void
DisplayGroupAndTabItemViewController::getDisplayGroupAndTabIndex(DisplayGroupEnum::Enum& displayGroupOut,
                                                                 int32_t& tabIndexOut) const
{
    BrowserTabContent* tabContent = GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    CaretAssert(tabContent);
    tabIndexOut= tabContent->getTabNumber();
    CaretAssert(tabIndexOut >= 0);
    
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    displayGroupOut = dpa->getDisplayGroupForTab(tabIndexOut);
}


/**
 * Update the content.
 *
 * @param displayGroupAndTabItemIn
 *     Display group for this instance.
 */
void
DisplayGroupAndTabItemViewController::updateContent(std::vector<DisplayGroupAndTabItemInterface*>& contentItemsIn,
                                                    const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex)
{
    /*
     * Ignore items without children
     */
    std::vector<DisplayGroupAndTabItemInterface*> contentItems;
    for (std::vector<DisplayGroupAndTabItemInterface*>::iterator contIter = contentItemsIn.begin();
         contIter != contentItemsIn.end();
         contIter++) {
        DisplayGroupAndTabItemInterface* item = *contIter;
        if (item->getNumberOfItemChildren() > 0) {
            contentItems.push_back(item);
        }
    }
    
    /*
     * Updating the tree will cause signals so block them until update is done
     */
    m_treeWidget->blockSignals(true);
    
    const int32_t numExistingChildren = m_treeWidget->topLevelItemCount();
    const int32_t numValidChildren    = contentItems.size();
    const int32_t maxCount = std::max(numExistingChildren,
                                      numValidChildren);
    
    const int32_t numberOfChildrenToAdd = numValidChildren - numExistingChildren;
    for (int32_t i = 0; i < numberOfChildrenToAdd; i++) {
        m_treeWidget->addTopLevelItem(new DisplayGroupAndTabItemTreeWidgetItem());
    }
    
    CaretAssert(m_treeWidget->topLevelItemCount() >= numValidChildren);
    CaretAssert(m_treeWidget->topLevelItemCount() >= maxCount);
    
    for (int32_t i = 0; i < numValidChildren; i++) {
        QTreeWidgetItem* treeWidgetChild = m_treeWidget->topLevelItem(i);
        CaretAssert(treeWidgetChild);
        DisplayGroupAndTabItemTreeWidgetItem* dgtChild = dynamic_cast<DisplayGroupAndTabItemTreeWidgetItem*>(treeWidgetChild);
        CaretAssert(dgtChild);
        
            treeWidgetChild->setHidden(false);
            
            CaretAssertVectorIndex(contentItems, i);
            CaretAssert(contentItems[i]);
            DisplayGroupAndTabItemInterface* displayGroupAndTabItem = contentItems[i];
            dgtChild->updateContent(displayGroupAndTabItem,
                                    m_treeWidget,
                                    displayGroup,
                                    tabIndex);
    }
    
    for (int32_t i = (numExistingChildren - 1); i >= numValidChildren; i--) {
        /*
         * Take removes it from the parent but
         * does not destruct it.
         */
        QTreeWidgetItem* item = m_treeWidget->takeTopLevelItem(i);
        delete item;
    }
    
    /*
     * Allow signals now that updating is done
     */
    m_treeWidget->blockSignals(false);
}

/**
 * Update graphics and, in some circumstances, surface node coloring.
 */
void
DisplayGroupAndTabItemViewController::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the selected and expanded checkboxes.
 */
void
DisplayGroupAndTabItemViewController::updateSelectedAndExpandedCheckboxes(const DisplayGroupEnum::Enum displayGroup,
                                                                          const int32_t tabIndex)
{
    m_treeWidget->blockSignals(true);
    
    const int32_t numChildren = m_treeWidget->topLevelItemCount();
    for (int32_t itemIndex = 0; itemIndex < numChildren; itemIndex++) {
        QTreeWidgetItem* treeChild = m_treeWidget->topLevelItem(itemIndex);
        CaretAssert(treeChild);
        
        DisplayGroupAndTabItemTreeWidgetItem* item = dynamic_cast<DisplayGroupAndTabItemTreeWidgetItem*>(treeChild);
        CaretAssert(item);
        
        DisplayGroupAndTabItemInterface* data = item->getDisplayGroupAndTabItem();
        if (data != NULL) {
            item->updateSelectedAndExpandedCheckboxes(displayGroup,
                                                      tabIndex);
        }
    }
    
    m_treeWidget->blockSignals(false);
}

/**
 * Update the selection and expansion controls in ALL other view controllers.
 * All of them need to be updated since window annotation selection is not
 * affected by the display group and tab selection.
 */
void
DisplayGroupAndTabItemViewController::updateSelectedAndExpandedCheckboxesInOtherViewControllers()
{
    for (std::set<DisplayGroupAndTabItemViewController*>::iterator iter = s_allViewControllers.begin();
         iter != s_allViewControllers.end();
         iter++) {
        DisplayGroupAndTabItemViewController* otherViewController = *iter;
        if (otherViewController != this) {
            if (otherViewController->m_dataFileType == m_dataFileType) {
                DisplayGroupEnum::Enum otherDisplayGroup = DisplayGroupEnum::DISPLAY_GROUP_TAB;
                int32_t otherTabIndex = -1;
                otherViewController->getDisplayGroupAndTabIndex(otherDisplayGroup,
                                                                otherTabIndex);
                otherViewController->updateSelectedAndExpandedCheckboxes(otherDisplayGroup,
                                                                         otherTabIndex);
            }
        }
    }
}

