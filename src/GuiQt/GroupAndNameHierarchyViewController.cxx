
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
#include "CiftiBrainordinateLabelFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
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
#include "VolumeFile.h"
#include "WuQMacroManager.h"
#include "WuQTreeWidget.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::GroupAndNameHierarchyViewController 
 * \brief View controller for ClassAndNameHierarchyModels
 * \ingroup GuiQt
 *
 * A view controller for one or more ClassAndNameHierarchyModel
 * instances.
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window
 * @param objectNameForMacros
 *    Name of this object for macros
 * @param parent
 *    Parent widget.
 */
GroupAndNameHierarchyViewController::GroupAndNameHierarchyViewController(const int32_t browserWindowIndex,
                                                                         const QString& objectNameForMacros,
                                                                         QWidget* parent)
: QWidget(parent)
{
    m_dataFileType = DataFileTypeEnum::UNKNOWN;
    m_displayGroup = DisplayGroupEnum::getDefaultValue();
    m_previousDisplayGroup = DisplayGroupEnum::getDefaultValue();
    m_previousBrowserTabIndex = -1;
    m_browserWindowIndex = browserWindowIndex;
    
    QWidget* allOnOffWidget = createAllOnOffControls(objectNameForMacros);

    m_modelTreeWidgetLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_modelTreeWidgetLayout, 0, 0);
    m_modelTreeWidget = NULL;
    createTreeWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(allOnOffWidget);
    layout->addSpacing(5);
    layout->addLayout(m_modelTreeWidgetLayout, 100);
    layout->addStretch();
    
    s_allViewControllers.insert(this);
}

/**
 * Destructor.
 */
GroupAndNameHierarchyViewController::~GroupAndNameHierarchyViewController()
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
GroupAndNameHierarchyViewController::itemWasCollapsed(QTreeWidgetItem* item)
{
    GroupAndNameHierarchyTreeWidgetItem* treeItem = dynamic_cast<GroupAndNameHierarchyTreeWidgetItem*>(item);
    CaretAssert(treeItem);
    treeItem->setModelDataExpanded(false);

    updateSelectedAndExpandedCheckboxes();
    updateSelectedAndExpandedCheckboxesInOtherViewControllers();
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
    GroupAndNameHierarchyTreeWidgetItem* treeItem = dynamic_cast<GroupAndNameHierarchyTreeWidgetItem*>(item);
    CaretAssert(treeItem);
    treeItem->setModelDataExpanded(true);

    updateSelectedAndExpandedCheckboxes();
    updateSelectedAndExpandedCheckboxesInOtherViewControllers();
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
    treeItem->setModelDataSelected(newStatus);

    updateSelectedAndExpandedCheckboxes();
    updateSelectedAndExpandedCheckboxesInOtherViewControllers();
    updateGraphics();
}

/**
 * Update graphics and, in some circumstances, surface node coloring.
 */
void
GroupAndNameHierarchyViewController::updateGraphics()
{
    if (m_selectionInvalidatesSurfaceNodeColoring) {
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    }
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Create buttons for all on and off
 *
 * @param objectNameForMacros
 *    Name of this object for macros
 */
QWidget*
GroupAndNameHierarchyViewController::createAllOnOffControls(const QString& objectNameForMacros)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    QLabel* allLabel = new QLabel("All: ");
    
    QPushButton* onPushButton = new QPushButton("On");
    onPushButton->setToolTip("Turn all on");
    onPushButton->setObjectName(objectNameForMacros
                                + ":AllOn");
    QObject::connect(onPushButton, SIGNAL(clicked()),
                     this, SLOT(allOnPushButtonClicked()));
    macroManager->addMacroSupportToObject(onPushButton);
    
    QPushButton* offPushButton = new QPushButton("Off");
    offPushButton->setToolTip("Turn all of");
    offPushButton->setObjectName(objectNameForMacros
                                + ":AllOff");
    QObject::connect(offPushButton, SIGNAL(clicked()),
                     this, SLOT(allOffPushButtonClicked()));
    macroManager->addMacroSupportToObject(offPushButton);
    
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

        updateSelectedAndExpandedCheckboxesInOtherViewControllers();
        updateSelectedAndExpandedCheckboxes();
        updateGraphics();
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
GroupAndNameHierarchyViewController::updateContents(std::vector<BorderFile*>& borderFiles,
                                                    const DisplayGroupEnum::Enum displayGroup)
{
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
    
    updateContents(models,
                   DataFileTypeEnum::BORDER,
                   false);
}

/**
 * Update with border files.
 * @param borderFiles
 *    The border files.
 * @param displayGroup
 *    The selected display group.
 */
void 
GroupAndNameHierarchyViewController::updateContents(std::vector<FociFile*>& fociFiles,
                                                    const DisplayGroupEnum::Enum displayGroup)
{
    std::vector<GroupAndNameHierarchyModel*> models;
    m_displayGroup = displayGroup;
    std::vector<GroupAndNameHierarchyModel*> classAndNameHierarchyModels;
    for (std::vector<FociFile*>::iterator iter = fociFiles.begin();
         iter != fociFiles.end();
         iter++) {
        FociFile* ff = *iter;
        CaretAssert(ff);
        models.push_back(ff->getGroupAndNameHierarchyModel());
    }
    
    updateContents(models,
                   DataFileTypeEnum::FOCI,
                   false);
}

/**
 * Update with label files.
 * @param labelFiles
 *    The label files.
 * @param ciftiLabelFiles
 *    The CIFTI label files.
 * @param volumeLabelFiles
 *    The volume label files.
 * @param displayGroup
 *    The selected display group.
 */
void
GroupAndNameHierarchyViewController::updateContents(std::vector<LabelFile*>& labelFiles,
                                                    std::vector<CiftiBrainordinateLabelFile*>& ciftiLabelFiles,
                                                    std::vector<VolumeFile*>& volumeLabelFiles,
                                                    const DisplayGroupEnum::Enum displayGroup)
{
    std::vector<GroupAndNameHierarchyModel*> models;
    m_displayGroup = displayGroup;
    
    std::vector<GroupAndNameHierarchyModel*> classAndNameHierarchyModels;
    
    for (std::vector<LabelFile*>::iterator iter = labelFiles.begin();
         iter != labelFiles.end();
         iter++) {
        LabelFile* lf = *iter;
        CaretAssert(lf);
        models.push_back(lf->getGroupAndNameHierarchyModel());
    }
    
    for (std::vector<CiftiBrainordinateLabelFile*>::iterator iter = ciftiLabelFiles.begin();
         iter != ciftiLabelFiles.end();
         iter++) {
        CiftiBrainordinateLabelFile* clf = *iter;
        CaretAssert(clf);
        models.push_back(clf->getGroupAndNameHierarchyModel());
    }
    
    for (std::vector<VolumeFile*>::iterator iter = volumeLabelFiles.begin();
         iter != volumeLabelFiles.end();
         iter++) {
        VolumeFile* vf = *iter;
        CaretAssert(vf);
        models.push_back(vf->getGroupAndNameHierarchyModel());
    }
    
    updateContents(models,
                   DataFileTypeEnum::LABEL,
                   true);
}

/**
 * Create/recreate the tree widget.
 */
void
GroupAndNameHierarchyViewController::createTreeWidget()
{
    
    /*
     * Delete and recreate the tree widget
     * Seems that adding and removing items from tree widget eventually
     * causes a crash.
     */
    
    m_treeWidgetItems.clear();
    if (m_modelTreeWidget != NULL) {
        m_modelTreeWidget->blockSignals(true);
        m_modelTreeWidget->clear();
        m_modelTreeWidgetLayout->removeWidget(m_modelTreeWidget);
        delete m_modelTreeWidget;
    }
    
    m_modelTreeWidget = new WuQTreeWidget();
    QObject::connect(m_modelTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                     this, SLOT(itemWasCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_modelTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(itemWasExpanded(QTreeWidgetItem*)));
    QObject::connect(m_modelTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(itemWasChanged(QTreeWidgetItem*, int)));
    
    m_modelTreeWidgetLayout->addWidget(m_modelTreeWidget);
    
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
GroupAndNameHierarchyViewController::updateContents(std::vector<GroupAndNameHierarchyModel*>& classAndNameHierarchyModels,
                                                    const DataFileTypeEnum::Enum dataFileType,
                                                    const bool selectionInvalidatesSurfaceNodeColoring)
{
    m_dataFileType= dataFileType;
    m_selectionInvalidatesSurfaceNodeColoring = selectionInvalidatesSurfaceNodeColoring;
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    CaretAssert(browserTabContent);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    /*
     * May need an update
     */
    bool needUpdate = false;
    int32_t numberOfModels = static_cast<int32_t>(classAndNameHierarchyModels.size());
    
    /*
     * Has the number of models changed?
     */
    if (numberOfModels != static_cast<int32_t>(this->m_treeWidgetItems.size())) {
        needUpdate = true;
    }
//    else if (m_displayGroup != m_previousDisplayGroup) {
//        needUpdate = true;
//    }
//    else if (browserTabIndex != m_previousBrowserTabIndex) {
//        needUpdate = true;
//    }
    else {
        /*
         * Have the displayed models changed?
         */
        for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
            if (classAndNameHierarchyModels[iModel] != this->m_treeWidgetItems[iModel]->getClassAndNameHierarchyModel()) {
                needUpdate = true;
                break;
            }
            else if (classAndNameHierarchyModels[iModel]->getChildren().size()
                     != this->m_treeWidgetItems[iModel]->getClassAndNameHierarchyModel()->getChildren().size()) {
                needUpdate = true;
                break;
            }
        }
        /*
         * Has the model's content been altered?
         */
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
        
        createTreeWidget();
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
    else {
        for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
            this->m_treeWidgetItems[iModel]->updateIconColorIncludingChildren();
        }
    }
    
    updateSelectedAndExpandedCheckboxes();
    
    m_previousBrowserTabIndex = browserTabIndex;
    m_previousDisplayGroup = m_displayGroup;
    m_modelTreeWidget->blockSignals(false);
    
    if (needUpdate) {
        m_modelTreeWidget->resizeToFitContent();
    }
}

/**
 * Update the selection and expansion controls.
 */
void
GroupAndNameHierarchyViewController::updateSelectedAndExpandedCheckboxes()
{
    if (m_modelTreeWidget == NULL) {
        return;
    }
    
    m_modelTreeWidget->blockSignals(true);
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    CaretAssert(browserTabContent);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const int32_t numberOfModels = static_cast<int32_t>(this->m_treeWidgetItems.size());
    for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
        m_treeWidgetItems[iModel]->updateSelections(m_displayGroup,
                                                    browserTabIndex);
    }
    m_modelTreeWidget->blockSignals(false);
}

/**
 * Update the selection and expansion controls in other view controllers
 * that are set to the same display group (not tab) and contain the
 * same type of data.
 */
void
GroupAndNameHierarchyViewController::updateSelectedAndExpandedCheckboxesInOtherViewControllers()
{
    if (m_displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        return;
    }
    
    for (std::set<GroupAndNameHierarchyViewController*>::iterator iter = s_allViewControllers.begin();
         iter != s_allViewControllers.end();
         iter++) {
        GroupAndNameHierarchyViewController* vc = *iter;
        if (vc != this) {
            if (vc->m_displayGroup == m_displayGroup) {
                if (vc->m_dataFileType == m_dataFileType) {
                    vc->updateSelectedAndExpandedCheckboxes();
                }
            }
        }
    }
}


