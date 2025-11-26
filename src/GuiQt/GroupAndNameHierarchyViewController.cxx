
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

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "Brain.h"
#include "BorderFile.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CiftiBrainordinateLabelFile.h"
#include "EventGraphicsPaintSoonAllWindows.h"
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
#include "WuQMessageBoxTwo.h"
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
 * @param descriptiveNameForMacros
 *    Descriptive name for macros
 * @param parent
 *    Parent widget.
 */
GroupAndNameHierarchyViewController::GroupAndNameHierarchyViewController(const int32_t browserWindowIndex,
                                                                         const QString& objectNameForMacros,
                                                                         const QString& descriptiveNameForMacros,
                                                                         QWidget* parent)
: QWidget(parent)
{
    m_dataFileType = DataFileTypeEnum::UNKNOWN;
    m_displayGroup = DisplayGroupEnum::getDefaultValue();
    m_previousDisplayGroup = DisplayGroupEnum::getDefaultValue();
    m_previousBrowserTabIndex = -1;
    m_browserWindowIndex = browserWindowIndex;
    
    m_modelTreeWidgetLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(m_modelTreeWidgetLayout, 0, 0);
    m_modelTreeWidget = NULL;
    createTreeWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(createButtonsWidget(objectNameForMacros,
                                          descriptiveNameForMacros));
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
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * @return New instance of the buttons for collapse, expand, find, etc.
 * @param objectNameForMacros
 *    Name of this object for macros
 * @param descriptiveNameForMacros
 *    Descriptive name for macros
 */
QWidget*
GroupAndNameHierarchyViewController::createButtonsWidget(const QString& objectNameForMacros,
                                                         const QString& descriptiveNameForMacros)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

    m_collapseAllAction = new QAction("Collapse");
    m_collapseAllAction->setToolTip("Collapse all items");
    m_collapseAllAction->setObjectName(objectNameForMacros + ":Collapse");
    QObject::connect(m_collapseAllAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::collapseAllActionTriggered);
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setDefaultAction(m_collapseAllAction);
    macroManager->addMacroSupportToObject(m_collapseAllAction,
                                          "Collapse all in " + descriptiveNameForMacros);

    
    m_expandAllAction = new QAction("Expand");
    m_expandAllAction->setObjectName(objectNameForMacros + ":Expand");
    m_expandAllAction->setToolTip("Expand all items");
    QObject::connect(m_expandAllAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::expandAllActionTriggered);
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setDefaultAction(m_expandAllAction);
    macroManager->addMacroSupportToObject(m_expandAllAction,
                                          "Expand all in " + descriptiveNameForMacros);

    m_allOnAction = new QAction("On");
    m_allOnAction->setToolTip("Turn all items on (check all)");
    m_allOnAction->setObjectName(objectNameForMacros + ":AllOn");
    QObject::connect(m_allOnAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::allOnActionTriggered);
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setDefaultAction(m_allOnAction);
    macroManager->addMacroSupportToObject(m_allOnAction,
                                          "Turn all on in " + descriptiveNameForMacros);

    m_allOffAction = new QAction("Off");
    m_allOffAction->setObjectName(objectNameForMacros + ":AllOff");
    m_allOffAction->setToolTip("Turn all items off (uncheck all)");
    QObject::connect(m_allOffAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::allOffActionTriggered);
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setDefaultAction(m_allOffAction);
    macroManager->addMacroSupportToObject(m_allOffAction,
                                          "Turn all off in " + descriptiveNameForMacros);

    m_infoAction = new QAction("Info");
    m_infoAction->setObjectName(objectNameForMacros + ":InfoMenu");
    m_infoAction->setToolTip("Show information about selected label");
    m_infoAction->setEnabled(false);
    QObject::connect(m_infoAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::infoActionTriggered);
    m_infoToolButton = new QToolButton;
    m_infoToolButton->setDefaultAction(m_infoAction);
    
    m_findAction = new QAction("Find");
    m_findAction->setObjectName(objectNameForMacros + ":Find");
    m_findAction->setToolTip("Find the first item containing the text");
    m_findAction->setEnabled(false);
    QObject::connect(m_findAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::findActionTriggered);
    QToolButton* findToolButton(new QToolButton);
    findToolButton->setDefaultAction(m_findAction);
    macroManager->addMacroSupportToObject(m_findAction,
                                          "Find in " + descriptiveNameForMacros);

    m_nextAction = new QAction("Next");
    m_nextAction->setObjectName(objectNameForMacros + ":Next");
    m_nextAction->setToolTip("Move to the next item containing the text (will wrap)");
    m_nextAction->setEnabled(false);
    QObject::connect(m_nextAction, &QAction::triggered,
                     this, &GroupAndNameHierarchyViewController::nextActionTriggered);
    QToolButton* nextToolButton(new QToolButton);
    nextToolButton->setDefaultAction(m_nextAction);
    macroManager->addMacroSupportToObject(m_nextAction,
                                          "Find next in " + descriptiveNameForMacros);

    m_findTextLineEdit = new QLineEdit();
    m_findTextLineEdit->setObjectName(objectNameForMacros + ":FindText");
    m_findTextLineEdit->setToolTip("Enter find text here");
    QObject::connect(m_findTextLineEdit, &QLineEdit::returnPressed,
                     this, &GroupAndNameHierarchyViewController::findActionTriggered);
    QObject::connect(m_findTextLineEdit, &QLineEdit::textChanged,
                     this, &GroupAndNameHierarchyViewController::findTextLineEditTextChanged);
    
    QWidget* widget(new QWidget());
    QHBoxLayout* buttonsLayout(new QHBoxLayout(widget));
    buttonsLayout->setSpacing(buttonsLayout->spacing() / 2);
    buttonsLayout->setContentsMargins(2, 2, 2, 2);
    buttonsLayout->addWidget(allOnToolButton);
    buttonsLayout->addWidget(allOffToolButton);
    buttonsLayout->addWidget(collapseAllToolButton);
    buttonsLayout->addWidget(expandAllToolButton);
    buttonsLayout->addSpacing(4);
    buttonsLayout->addWidget(m_infoToolButton);
    buttonsLayout->addSpacing(4);
    buttonsLayout->addWidget(findToolButton);
    buttonsLayout->addWidget(nextToolButton);
    buttonsLayout->addWidget(m_findTextLineEdit,
                             100); /* stretch factor */
    return widget;
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
    m_modelTreeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_modelTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_modelTreeWidget, &WuQTreeWidget::itemCollapsed,
                     this, &GroupAndNameHierarchyViewController::itemWasCollapsed);
    QObject::connect(m_modelTreeWidget, &WuQTreeWidget::itemExpanded,
                     this, &GroupAndNameHierarchyViewController::itemWasExpanded);
    QObject::connect(m_modelTreeWidget, &WuQTreeWidget::itemChanged,
                     this, &GroupAndNameHierarchyViewController::itemWasChanged);
    QObject::connect(m_modelTreeWidget, &WuQTreeWidget::itemClicked,
                     this, &GroupAndNameHierarchyViewController::treeItemClicked);
    QObject::connect(m_modelTreeWidget, &WuQTreeWidget::itemDoubleClicked,
                     this, &GroupAndNameHierarchyViewController::treeItemDoubleClicked);
    QObject::connect(m_modelTreeWidget, &WuQTreeWidget::customContextMenuRequested,
                     this, &GroupAndNameHierarchyViewController::showTreeViewContextMenu);

    m_modelTreeWidgetLayout->addWidget(m_modelTreeWidget, 0);
    
    m_modelTreeWidget->blockSignals(false);
    
    m_findItems.clear();
    m_findItemsCurrentIndex = 0;
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


/**
 * Show a context menu for the tree view
 * @param pos
 *    Position in the tree view
 */
void
GroupAndNameHierarchyViewController::showTreeViewContextMenu(const QPoint& pos)
{
    /*
     * Note: GroupAndNameHierarchyTreeWidgetItem do not correspond to individual
     * borders and foci but to the aggreggated names and classes of the foci.
     * Since they are not individual borders or foci, info or editing is not possible.
     */
    const bool showMenuFlag(false);
    if ( ! showMenuFlag) {
        return;
    }
    if (m_modelTreeWidget != NULL) {
        const QModelIndex selectedIndex(m_modelTreeWidget->indexAt(pos));
        if (selectedIndex.isValid()) {
            const QTreeWidgetItem* item(m_modelTreeWidget->getItemFromIndex(selectedIndex));
            if (item != NULL) {
                const GroupAndNameHierarchyTreeWidgetItem* gnhItem(dynamic_cast<const GroupAndNameHierarchyTreeWidgetItem*>(item));
                if (gnhItem != NULL) {
                    const bool infoButtonFlag(false);
                    showSelectedItemMenu(gnhItem,
                                         m_modelTreeWidget->mapToGlobal(pos),
                                         infoButtonFlag);
                }
            }
        }
    }
}

/**
 * Show a menu for the selected label
 * @param item
 *    The  item that is selected
 * @param pos
 *    Position for the menu
 * @param infoButtonFlag
 *    If true, menu is for the Info button, else right-click menu on label
 */
void
GroupAndNameHierarchyViewController::showSelectedItemMenu(const GroupAndNameHierarchyTreeWidgetItem* item,
                                                            const QPoint& pos,
                                                            const bool /*infoButtonFlag*/)
{
    QMenu menu(this);
    
    QAction* infoAction(menu.addAction("Info..."));
    menu.addSeparator();
    if ( ! menu.actions().isEmpty()) {
        QAction* selectedAction(menu.exec(pos));
        if (selectedAction != NULL) {
            if (selectedAction == infoAction) {
                WuQMessageBoxTwo::information(this,
                                              "Info",
                                              item->text(0));
            }
            
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        }
    }
}

/**
 * Set the checked status of all children
 * @param item
 *    The item
 * @param checkState
 *    The check state
 */
void
GroupAndNameHierarchyViewController::setCheckedStatusOfAllChildren(GroupAndNameHierarchyTreeWidgetItem* /*item*/,
                                                                     const Qt::CheckState /*checkState*/)
{
}

/**
 * Called when collapse all action is triggered
 */
void
GroupAndNameHierarchyViewController::collapseAllActionTriggered()
{
    m_modelTreeWidget->collapseAll();
}

/**
 * Called when expand all action is triggered
 */
void
GroupAndNameHierarchyViewController::expandAllActionTriggered()
{
    m_modelTreeWidget->expandAll();
}

/**
 * Called when expand all action is triggered
 */
void
GroupAndNameHierarchyViewController::allOnActionTriggered()
{
    setAllSelected(true);
}

/**
 * Called when expand all action is triggered
 */
void
GroupAndNameHierarchyViewController::allOffActionTriggered()
{
    setAllSelected(false);
}

/**
 * Called when Info button is clicked
 */
void
GroupAndNameHierarchyViewController::infoActionTriggered()
{
    if (m_modelTreeWidget != NULL) {
        const QModelIndex selectedIndex(m_modelTreeWidget->currentIndex());
        if (selectedIndex.isValid()) {
            const QTreeWidgetItem* item(m_modelTreeWidget->currentItem());
            if (item != NULL) {
                const GroupAndNameHierarchyTreeWidgetItem* gnhItem(dynamic_cast<const GroupAndNameHierarchyTreeWidgetItem*>(item));
                if (gnhItem != NULL) {
                    const bool infoButtonFlag(true);
                    showSelectedItemMenu(gnhItem,
                                         mapToGlobal(m_infoToolButton->pos()), 
                                         infoButtonFlag);
                }
            }
        }
    }
}

/**
 * Called when find button is clicked or return is pressed in the find line edit
 */
void
GroupAndNameHierarchyViewController::findActionTriggered()
{
    m_findItems.clear();
    m_findItemsCurrentIndex = 0;
    
    if (m_modelTreeWidget != NULL) {
        const QString findText(m_findTextLineEdit->text().trimmed());
        
        const int modelColumn(0);
        m_findItems = m_modelTreeWidget->findItems(findText,
                                                   (Qt::MatchContains
                                                  | Qt::MatchRecursive),
                                                   modelColumn);
        if (m_findItems.isEmpty()) {
            GuiManager::get()->beep();
        }
        scrollTreeViewToFindItem();
    }
}

/**
 * Called when next button is clicked
 */
void
GroupAndNameHierarchyViewController::nextActionTriggered()
{
    scrollTreeViewToFindItem();
}

/**
 * Scroll the tree view to the next find item
 */
void
GroupAndNameHierarchyViewController::scrollTreeViewToFindItem()
{
    const int32_t numFindItems(m_findItems.size());
    if (numFindItems > 0) {
        if ((m_findItemsCurrentIndex < 0)
            || (m_findItemsCurrentIndex >= numFindItems)) {
            m_findItemsCurrentIndex = 0;
        }
        CaretAssertVectorIndex(m_findItems, m_findItemsCurrentIndex);
        const QTreeWidgetItem* item(m_findItems[m_findItemsCurrentIndex]);
        CaretAssert(item);
        const QModelIndex modelIndex(m_modelTreeWidget->getIndexFromItem(item));
        if (modelIndex.isValid()) {
            m_modelTreeWidget->setCurrentIndex(modelIndex);
            m_modelTreeWidget->scrollTo(modelIndex,
                                        QTreeView::PositionAtCenter);
        }
        
        /*
         * For 'next'
         */
        ++m_findItemsCurrentIndex;
    }
    
    m_nextAction->setEnabled(numFindItems > 1);
}


/**
 * Called when next button is clicked
 * @param text
 *    Text in the line edit
 */
void
GroupAndNameHierarchyViewController::findTextLineEditTextChanged(const QString& text)
{
    m_findAction->setEnabled( ! text.trimmed().isEmpty());
    m_nextAction->setEnabled(false);
    m_findItems.clear();
    m_findItemsCurrentIndex = 0;
}

/**
 * Called when tree item is double clicked
 * @param item
 *     Item that was clicked
 * @param column
 *     Column of click
 */
void
GroupAndNameHierarchyViewController::treeItemDoubleClicked(QTreeWidgetItem* /*item*/, int /*column*/)
{
}

/**
 * Called when tree item is  clicked
 * @param item
 *     Item that was clicked
 * @param column
 *     Column of click
 */
void
GroupAndNameHierarchyViewController::treeItemClicked(QTreeWidgetItem* /*item*/, int /*column*/)
{
}
