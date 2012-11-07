
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

#define __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__
#include "GroupAndNameHierarchyViewController.h"
#undef __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "Brain.h"
#include "BorderFile.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GroupAndNameHierarchyGroup.h"
#include "GroupAndNameHierarchyModel.h"
#include "GroupAndNameHierarchyName.h"
#include "GroupAndNameHierarchyTreeWidgetItem.h"
#include "FociFile.h"
#include "GuiManager.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "WuQTreeWidget.h"
#include "WuQtUtilities.h"
//#include "WuQTreeWidget.h"

using namespace caret;

/**
 * \class caret::GroupAndNameHierarchyViewController 
 * \brief View controller for ClassAndNameHierarchyModels
 *
 * A view controller for one or more ClassAndNameHierarchyModel
 * instances.
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget.
 */
GroupAndNameHierarchyViewController::GroupAndNameHierarchyViewController(const int32_t browserWindowIndex,
                                                                         QWidget* parent)
: QWidget(parent)
{
    m_displayGroup = DisplayGroupEnum::getDefaultValue();
    m_browserWindowIndex = browserWindowIndex;
    m_ignoreUpdates = false;
    
    QWidget* allOnOffWidget = createAllOnOffControls();

    m_modelTreeWidget = new WuQTreeWidget();
    QObject::connect(m_modelTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                     this, SLOT(itemWasCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_modelTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(itemWasExpanded(QTreeWidgetItem*)));
    QObject::connect(m_modelTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(itemWasChanged(QTreeWidgetItem*, int)));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(allOnOffWidget);
    layout->addSpacing(5);
    layout->addWidget(m_modelTreeWidget);
    
}

/**
 * Destructor.
 */
GroupAndNameHierarchyViewController::~GroupAndNameHierarchyViewController()
{
    removeAllModelItems();
}

/**
 * Gets called when an item is collapsed so that its children are not visible.
 *
 * @param item
 *    The QTreeWidgetItem that was collapsed.
 */
void
GroupAndNameHierarchyViewController::itemWasCollapsed(QTreeWidgetItem* item)
{
    std::cout << "Item collapsed: " << qPrintable(item->text(GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN)) << std::endl;
    GroupAndNameHierarchyTreeWidgetItem* treeItem = dynamic_cast<GroupAndNameHierarchyTreeWidgetItem*>(item);
    CaretAssert(treeItem);
    treeItem->setModelDataExpanded(false);
    
    updateGraphicsAndUserInterface();
}

/**
 * Gets called when an item is expaned so that its children are visible.
 *
 * @param item
 *    The QTreeWidgetItem that was expanded.
 */
void
GroupAndNameHierarchyViewController::itemWasExpanded(QTreeWidgetItem* item)
{
    std::cout << "Item expanded: " << qPrintable(item->text(GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN)) << std::endl;
    GroupAndNameHierarchyTreeWidgetItem* treeItem = dynamic_cast<GroupAndNameHierarchyTreeWidgetItem*>(item);
    CaretAssert(treeItem);
    treeItem->setModelDataExpanded(true);

    updateGraphicsAndUserInterface();
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
GroupAndNameHierarchyViewController::itemWasChanged(QTreeWidgetItem* item,
                                                   int /*column*/)
{
    GroupAndNameHierarchyTreeWidgetItem* treeItem = dynamic_cast<GroupAndNameHierarchyTreeWidgetItem*>(item);
    CaretAssert(treeItem);
    const Qt::CheckState checkState = item->checkState(GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN);
    const GroupAndNameCheckStateEnum::Enum itemCheckState = GroupAndNameHierarchyTreeWidgetItem::fromQCheckState(checkState);
    const bool newStatus = (itemCheckState != GroupAndNameCheckStateEnum::UNCHECKED);
    std::cout << "Item checkbox changed: "
        << qPrintable(item->text(GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN))
        << " GUIcheckState " << GroupAndNameCheckStateEnum::toName(itemCheckState)
        << " new status " << (int)newStatus << std::endl;
    treeItem->setModelDataSelected(newStatus);

    updateGraphicsAndUserInterface();
}

/**
 * Update graphics and the user-interface.
 */
void
GroupAndNameHierarchyViewController::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().addToolBox().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Create buttons for all on and off
 */
QWidget*
GroupAndNameHierarchyViewController::createAllOnOffControls()
{
    QLabel* allLabel = new QLabel("All: ");
    
    QPushButton* onPushButton = new QPushButton("On");
    QObject::connect(onPushButton, SIGNAL(clicked()),
                     this, SLOT(allOnPushButtonClicked()));
    
    QPushButton* offPushButton = new QPushButton("Off");
    QObject::connect(offPushButton, SIGNAL(clicked()),
                     this, SLOT(allOffPushButtonClicked()));
    
    QWidget* w = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(w);
    layout->addWidget(allLabel);
    layout->addWidget(onPushButton);
    layout->addWidget(offPushButton);
    layout->addStretch();
    return w;
}

/**
 * Called when all on push button clicked.
 */
void 
GroupAndNameHierarchyViewController::allOnPushButtonClicked()
{
    setAllSelected(true);
}

/**
 * Called when all off push button clicked.
 */
void 
GroupAndNameHierarchyViewController::allOffPushButtonClicked()
{
    setAllSelected(false);
}

/**
 * Set selection status of all items.
 * @param selected
 *    New selection status for all items.
 */
void 
GroupAndNameHierarchyViewController::setAllSelected(bool selected)
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    if (browserTabContent != NULL) {
        const int32_t browserTabIndex = browserTabContent->getTabNumber();
        
        std::vector<GroupAndNameHierarchyModel*> allModels = getAllModels();
        const int32_t numModels = static_cast<int32_t>(allModels.size());
        for (int32_t i = 0; i < numModels; i++) {
            GroupAndNameHierarchyModel* model = allModels[i];
            model->setAllSelected(m_displayGroup,
                                  browserTabIndex,
                                  selected);
        }
        
        updateContents(allModels);
    }
}

/**
 * @return All models in this view controller.
 */
std::vector<GroupAndNameHierarchyModel*>
GroupAndNameHierarchyViewController::getAllModels() const
{
    std::vector<GroupAndNameHierarchyModel*> allModels;
    
    const int32_t numItems = static_cast<int32_t>(m_treeWidgetItems.size());
    for (int32_t i = 0; i < numItems; i++) {
        GroupAndNameHierarchyTreeWidgetItem* treeItem = m_treeWidgetItems[i];
        GroupAndNameHierarchyModel* model = treeItem->getClassAndNameHierarchyModel();
        allModels.push_back(model);
    }
    
    return allModels;
}


/**
 * Update with border files.
 * @param borderFiles
 *    The border files.
 * @param displayGroup
 *    The selected display group.
 */
void 
GroupAndNameHierarchyViewController::updateContents(std::vector<BorderFile*> borderFiles,
                                                    const DisplayGroupEnum::Enum displayGroup)
{
    if (m_ignoreUpdates) {
        return;
    }
    
    std::vector<GroupAndNameHierarchyModel*> models;
    m_displayGroup = displayGroup;
    std::vector<GroupAndNameHierarchyModel*> classAndNameHierarchyModels;
    for (std::vector<BorderFile*>::iterator iter = borderFiles.begin();
         iter != borderFiles.end();
         iter++) {
        BorderFile* bf = *iter;
        CaretAssert(bf);
        models.push_back(bf->getGroupAndNameHierarchyModel());
    }
    
//    m_ignoreUpdates = true;
    updateContents(models);
//    m_ignoreUpdates = false;
}

/**
 * Update with border files.
 * @param borderFiles
 *    The border files.
 * @param displayGroup
 *    The selected display group.
 */
void 
GroupAndNameHierarchyViewController::updateContents(std::vector<FociFile*> fociFiles,
                                                    const DisplayGroupEnum::Enum displayGroup)
{
    if (m_ignoreUpdates) {
        return;
    }
    
//    m_displayGroup = displayGroup;
//    std::vector<GroupAndNameHierarchyModel*> classAndNameHierarchyModels;
//    for (std::vector<FociFile*>::iterator iter = fociFiles.begin();
//         iter != fociFiles.end();
//         iter++) {
//        FociFile* ff = *iter;
//        CaretAssert(ff);
//        classAndNameHierarchyModels.push_back(ff->getGroupAndNameHierarchyModel());
//    }
//    
//    updateContents(classAndNameHierarchyModels);    
}

/**
 * Update with label files.
 * @param labelFiles
 *    The label files.
 * @param displayGroup
 *    The selected display group.
 */
void
GroupAndNameHierarchyViewController::updateContents(std::vector<LabelFile*> labelFiles,
                                                    const DisplayGroupEnum::Enum displayGroup)
{
    if (m_ignoreUpdates) {
        return;
    }
//    
//    m_alwaysDisplayNames = true;
//    m_displayGroup = displayGroup;
//    std::vector<GroupAndNameHierarchyModel*> classAndNameHierarchyModels;
//    for (std::vector<LabelFile*>::iterator iter = labelFiles.begin();
//         iter != labelFiles.end();
//         iter++) {
//        LabelFile* lf = *iter;
//        CaretAssert(lf);
//        classAndNameHierarchyModels.push_back(lf->getGroupAndNameHierarchyModel());
//    }
//    
//    updateContents(classAndNameHierarchyModels);
//    
}

/**
 * Remove all model items.
 */
void
GroupAndNameHierarchyViewController::removeAllModelItems()
{
    m_modelTreeWidget->blockSignals(true);
    
    std::cout << "Before removeAllModelItems() layout contains "
    << m_treeWidgetItems.size() << " items." << std::endl;
    
    int32_t numberOfModels = static_cast<int32_t>(this->m_treeWidgetItems.size());
    for (int32_t iModel = (numberOfModels - 1); iModel >= 0; iModel--) {
        GroupAndNameHierarchyTreeWidgetItem* item = this->m_treeWidgetItems[iModel];
        m_modelTreeWidget->removeItemWidget(item,
                                            GroupAndNameHierarchyTreeWidgetItem::TREE_COLUMN);
        delete item;
    }
    this->m_treeWidgetItems.clear();
    
    std::cout << "After removeAllModelItems() layout contains "
    << m_treeWidgetItems.size() << " items." << std::endl;
    
    m_modelTreeWidget->blockSignals(false);
}


/**
 * Update the content of the view controller.
 * @param classAndNameHierarchyModels
 *    ClassAndNameHierarchyModels instances for display.
 * @param allowNamesWithZeroCounts
 *    If true, display names even if the usage count is zero.
 */
void 
GroupAndNameHierarchyViewController::updateContents(std::vector<GroupAndNameHierarchyModel*>& classAndNameHierarchyModels)
{
    if (m_ignoreUpdates) {
        return;
    }
    m_ignoreUpdates = true;
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    /*
     * See if an update is needed.
     */
    bool needUpdate = false;
    int32_t numberOfModels = static_cast<int32_t>(classAndNameHierarchyModels.size());
    if (numberOfModels != static_cast<int32_t>(this->m_treeWidgetItems.size())) {
        needUpdate = true;
    }
    else {
        for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
            if (classAndNameHierarchyModels[iModel]->needsUserInterfaceUpdate(m_displayGroup,
                                                                              browserTabIndex)) {
                needUpdate = true;
                break;
            }
        }
    }
    
    m_modelTreeWidget->blockSignals(true);
    
    if (needUpdate) {
        
        removeAllModelItems();
        m_modelTreeWidget->blockSignals(true); // gets reset

        /*
         * Copy the models
         */
        for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
            GroupAndNameHierarchyTreeWidgetItem* modelItem = new GroupAndNameHierarchyTreeWidgetItem(m_displayGroup,
                                                                                                 browserTabIndex,
                                                                                                 classAndNameHierarchyModels[iModel]);
            this->m_treeWidgetItems.push_back(modelItem);
            m_modelTreeWidget->addTopLevelItem(modelItem);
        }        
    }
    
    numberOfModels = static_cast<int32_t>(this->m_treeWidgetItems.size());
    for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
        m_treeWidgetItems[iModel]->updateSelections();
    }
    
    m_ignoreUpdates = false;
    m_modelTreeWidget->blockSignals(false);
}



/**
 * Create a tree widget item that is checkable.
 * @param name
 *    Text displayed in item.
 * @param iconColorRGBA
 *    Color for icon (valid when alpha >= 0.0)
 * @param isSelected
 *    True if item is selected.
 * @param selectionInfo
 *    Selection information for item.
 */
//QTreeWidgetItem*
//GroupAndNameHierarchyViewController::createTreeWidgetItem(const AString& name,
//                                                          const float* iconColorRGBA,
//                                                          const GroupAndNameCheckStateEnum::Enum checkedStatus,
//                                                          GroupAndNameHierarchySelectedItem* selectionInfo)
//{
//    CaretAssert(selectionInfo);
//    QTreeWidgetItem* twi = new QTreeWidgetItem();
//    twi->setText(0, name);
//    twi->setFlags(Qt::ItemIsUserCheckable
//                  | Qt::ItemIsEnabled
//                  | Qt::ItemIsTristate);
//    
//    const Qt::CheckState checkState = toQCheckState(checkedStatus);
//    twi->setCheckState(0, checkState);
//    twi->setData(0, Qt::UserRole, qVariantFromValue((void*)selectionInfo));
//    
//    if (iconColorRGBA != NULL) {
//        if (iconColorRGBA[3] > 0.0) {
//            QPixmap pm(10, 10);
//            pm.fill(QColor::fromRgbF(iconColorRGBA[0],
//                                     iconColorRGBA[1],
//                                     iconColorRGBA[2]));
//            QIcon icon(pm);
//            twi->setIcon(0, icon);
//        }
//    }
//    
//    m_itemSelectionInfo.push_back(selectionInfo);
//    
//    return twi;
//
////    QTreeWidgetItem* twi = new QTreeWidgetItem();
////    twi->setText(0, name);
////    twi->setFlags(Qt::ItemIsUserCheckable
////                  | Qt::ItemIsEnabled);
////    if (isSelected) {
////        twi->setCheckState(0, Qt::Checked);
////    }
////    else {
////        twi->setCheckState(0, Qt::Unchecked);
////    }
////    twi->setData(0, Qt::UserRole, qVariantFromValue((void*)selectionInfo));
////
////    if (iconColorRGBA != NULL) {
////        if (iconColorRGBA[3] > 0.0) {
////            QPixmap pm(10, 10);
////            pm.fill(QColor::fromRgbF(iconColorRGBA[0],
////                                     iconColorRGBA[1],
////                                     iconColorRGBA[2]));
////            QIcon icon(pm);
////            twi->setIcon(0, icon);
////        }
////    }
////    
////    m_itemSelectionInfo.push_back(selectionInfo);
////    
////    return twi;
///*
// */
//}

