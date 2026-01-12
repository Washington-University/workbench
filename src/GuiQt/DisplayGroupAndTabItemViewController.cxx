
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

#include <map>

#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

#include "Annotation.h"
#include "AnnotationGroup.h"
#include "AnnotationManager.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupAndTabItemInterface.h"
#include "DisplayGroupAndTabItemTreeWidgetItem.h"
#include "DisplayPropertiesAnnotation.h"
#include "DisplayPropertiesSamples.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMacroManager.h"
#include "WuQTreeWidget.h"

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
 * @param objectNameForMacros
 *     Name of this object for macros
 * @param descriptiveNameForMacros
 *     Descriptive name for macros
 * @param browserWindowIndex
 *     The browser window containing this instance.
 * @param parent
 *     Parent of this instance.
 */
DisplayGroupAndTabItemViewController::DisplayGroupAndTabItemViewController(const DataFileTypeEnum::Enum dataFileType,
                                                                           const int32_t browserWindowIndex,
                                                                           const QString& objectNameForMacros,
                                                                           const QString& descriptiveNameForMacros,
                                                                           QWidget* parent)
: QWidget(parent),
m_dataFileType(dataFileType),
m_browserWindowIndex(browserWindowIndex)
{
    m_treeWidget = new WuQTreeWidget();
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setSelectionMode(QTreeWidget::NoSelection);
    
    QObject::connect(m_treeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
                     this, SLOT(itemWasCollapsed(QTreeWidgetItem*)));
    QObject::connect(m_treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(itemWasExpanded(QTreeWidgetItem*)));
    QObject::connect(m_treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(itemWasChanged(QTreeWidgetItem*, int)));
    QObject::connect(m_treeWidget, SIGNAL(itemSelectionChanged()),
                     this, SLOT(itemsWereSelected()));
    m_treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_treeWidget, &QTreeWidget::customContextMenuRequested,
                     this, &DisplayGroupAndTabItemViewController::displayContextMenu);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(createButtonsWidget(objectNameForMacros,
                                          descriptiveNameForMacros));
    layout->addWidget(m_treeWidget);
    layout->addStretch();
    
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
 * @return New instance of the buttons for collapse, expand, find, etc.
 * @param objectNameForMacros
 *    Name of this object for macros
 * @param descriptiveNameForMacros
 *    Descriptive name for macros
 */
QWidget*
DisplayGroupAndTabItemViewController::createButtonsWidget(const QString& objectNameForMacros,
                                                          const QString& descriptiveNameForMacros)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    
    m_collapseAllAction = new QAction("Collapse");
    m_collapseAllAction->setToolTip("Collapse all items");
    m_collapseAllAction->setObjectName(objectNameForMacros + ":Collapse");
    QObject::connect(m_collapseAllAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::collapseAllActionTriggered);
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setDefaultAction(m_collapseAllAction);
    macroManager->addMacroSupportToObject(m_collapseAllAction,
                                          "Collapse all in " + descriptiveNameForMacros);
    
    
    m_expandAllAction = new QAction("Expand");
    m_expandAllAction->setObjectName(objectNameForMacros + ":Expand");
    m_expandAllAction->setToolTip("Expand all items");
    QObject::connect(m_expandAllAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::expandAllActionTriggered);
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setDefaultAction(m_expandAllAction);
    macroManager->addMacroSupportToObject(m_expandAllAction,
                                          "Expand all in " + descriptiveNameForMacros);
    
    m_allOnAction = new QAction("On");
    m_allOnAction->setToolTip("Turn all SELECTED items on");
    m_allOnAction->setObjectName(objectNameForMacros + ":AllOn");
    QObject::connect(m_allOnAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::allOnActionTriggered);
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setDefaultAction(m_allOnAction);
    macroManager->addMacroSupportToObject(m_allOnAction,
                                          "Turn all on in " + descriptiveNameForMacros);
    
    m_allOffAction = new QAction("Off");
    m_allOffAction->setObjectName(objectNameForMacros + ":AllOff");
    m_allOffAction->setToolTip("Turn all SELECTED items off");
    QObject::connect(m_allOffAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::allOffActionTriggered);
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setDefaultAction(m_allOffAction);
    macroManager->addMacroSupportToObject(m_allOffAction,
                                          "Turn all off in " + descriptiveNameForMacros);
    
    m_infoAction = new QAction("Info");
    m_infoAction->setObjectName(objectNameForMacros + ":InfoMenu");
    m_infoAction->setToolTip("Show information about selected label");
    m_infoAction->setEnabled(false);
    QObject::connect(m_infoAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::infoActionTriggered);
    m_infoToolButton = new QToolButton;
    m_infoToolButton->setDefaultAction(m_infoAction);
    
    m_findAction = new QAction("Find");
    m_findAction->setObjectName(objectNameForMacros + ":Find");
    m_findAction->setToolTip("Find the first item containing the text");
    m_findAction->setEnabled(false);
    QObject::connect(m_findAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::findActionTriggered);
    QToolButton* findToolButton(new QToolButton);
    findToolButton->setDefaultAction(m_findAction);
    macroManager->addMacroSupportToObject(m_findAction,
                                          "Find in " + descriptiveNameForMacros);
    
    m_nextAction = new QAction("Next");
    m_nextAction->setObjectName(objectNameForMacros + ":Next");
    m_nextAction->setToolTip("Move to the next item containing the text (will wrap)");
    m_nextAction->setEnabled(false);
    QObject::connect(m_nextAction, &QAction::triggered,
                     this, &DisplayGroupAndTabItemViewController::nextActionTriggered);
    QToolButton* nextToolButton(new QToolButton);
    nextToolButton->setDefaultAction(m_nextAction);
    macroManager->addMacroSupportToObject(m_nextAction,
                                          "Find next in " + descriptiveNameForMacros);
    
    m_findTextLineEdit = new QLineEdit();
    m_findTextLineEdit->setObjectName(objectNameForMacros + ":FindText");
    m_findTextLineEdit->setToolTip("Enter find text here");
    QObject::connect(m_findTextLineEdit, &QLineEdit::returnPressed,
                     this, &DisplayGroupAndTabItemViewController::findActionTriggered);
    QObject::connect(m_findTextLineEdit, &QLineEdit::textChanged,
                     this, &DisplayGroupAndTabItemViewController::findTextLineEditTextChanged);
    
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
 * Gets called when items are selected.
 */
void
DisplayGroupAndTabItemViewController::itemsWereSelected()
{
    QList<QTreeWidgetItem*> itemsSelected = m_treeWidget->selectedItems();
    
    
    if ( ! itemsSelected.empty()) {
        
        std::vector<DisplayGroupAndTabItemInterface*> itemInterfacesVector;
        QListIterator<QTreeWidgetItem*> itemsIter(itemsSelected);
        while (itemsIter.hasNext()) {
            QTreeWidgetItem* item = itemsIter.next();
            DisplayGroupAndTabItemTreeWidgetItem* widgetItem = dynamic_cast<DisplayGroupAndTabItemTreeWidgetItem*>(item);
            CaretAssert(widgetItem);
            DisplayGroupAndTabItemInterface* itemInterface = widgetItem->getDisplayGroupAndTabItem();
            CaretAssert(itemInterface);
            if (itemInterface != NULL) {
                itemInterfacesVector.push_back(itemInterface);
            }
        }
        
        if ( ! itemInterfacesVector.empty()) {
            if (m_dataFileType == DataFileTypeEnum::ANNOTATION) {
                processAnnotationDataSelection(itemInterfacesVector);
            }
            else if (m_dataFileType == DataFileTypeEnum::SAMPLES) {
                processAnnotationDataSelection(itemInterfacesVector);
            }
        }
    }
    
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    int32_t tabIndex = -1;
    getDisplayGroupAndTabIndex(displayGroup, tabIndex);
    updateSelectedAndExpandedCheckboxes(displayGroup,
                                        tabIndex);
    
    updateGraphics();
}

/**
 * Set the items displayed in the context-sensitive (right-click) menu
 * @param contextMenuItems
 *    Items for display in the menu
 */
void
DisplayGroupAndTabItemViewController::enableContextSensitiveMenu(const std::vector<ContextSensitiveMenuItemsEnum::Enum>& contextMenuItems)
{
    m_contextMenuItems.clear();
    m_contextMenuItems.insert(contextMenuItems.begin(),
                              contextMenuItems.end());
}


/**
 * Display a context sensitive (right-click) menu.
 *
 * @param pos
 *     Position for context menu
 */
void
DisplayGroupAndTabItemViewController::displayContextMenu(const QPoint& pos)
{
    QList<QTreeWidgetItem*> itemsSelected = m_treeWidget->selectedItems();
    
    if (itemsSelected.isEmpty()) {
        return;
    }
    
    QMenu menu(this);
    QAction* onAction(menu.addAction("Turn all selected items ON"));
    QAction* offAction(menu.addAction("Turn all selected items OFF"));
    
    std::map<QAction*, ContextSensitiveMenuItemsEnum::Enum> menuActionEnumMap;
    std::vector<ContextSensitiveMenuItemsEnum::Enum> allContextMenuEnums;
    ContextSensitiveMenuItemsEnum::getAllEnums(allContextMenuEnums);
    for (ContextSensitiveMenuItemsEnum::Enum menuEnum : allContextMenuEnums) {
        if (m_contextMenuItems.find(menuEnum) != m_contextMenuItems.end()) {
            QAction* action = menu.addAction(ContextSensitiveMenuItemsEnum::toGuiName(menuEnum));
            menuActionEnumMap.insert(std::make_pair(action, menuEnum));
        }
    }
    
    QSignalBlocker blocker(m_treeWidget);
    QAction* selectedAction = menu.exec(m_treeWidget->mapToGlobal(pos));
    if (selectedAction == onAction) {
        const bool newStatus(true);
        setCheckedStatusOfSelectedItems(newStatus);
    }
    else if (selectedAction == offAction) {
        const bool newStatus(false);
        setCheckedStatusOfSelectedItems(newStatus);
    }
    else if (selectedAction != NULL) {
        for (auto& actionEnum : menuActionEnumMap) {
            if (actionEnum.first == selectedAction) {
                const ContextSensitiveMenuItemsEnum::Enum itemEnum(actionEnum.second);
                emit contextMenuItemSelected(itemsSelected,
                                             itemEnum);
                break;
            }
        }
    }
}

/**
 * Process the selection of annotations.
 *
 * @param interfaceItems
 *     Items that should be annotations !
 */
void
DisplayGroupAndTabItemViewController::processAnnotationDataSelection(const std::vector<DisplayGroupAndTabItemInterface*>& interfaceItems)
{
    std::set<Annotation*> annotationSet;
    
    for (std::vector<DisplayGroupAndTabItemInterface*>::const_iterator iter = interfaceItems.begin();
         iter != interfaceItems.end();
         iter++) {
        Annotation* ann = dynamic_cast<Annotation*>(*iter);
        if (ann != NULL) {
            annotationSet.insert(ann);
        }
        else {
            AnnotationGroup* annGroup = dynamic_cast<AnnotationGroup*>(*iter);
            if (annGroup != NULL) {
                std::vector<Annotation*> groupAnns;
                annGroup->getAllAnnotations(groupAnns);
                
                annotationSet.insert(groupAnns.begin(),
                                     groupAnns.end());
            }
        }
    }
    
    if ( ! annotationSet.empty()) {
        std::vector<Annotation*> selectedAnnotations(annotationSet.begin(),
                                                     annotationSet.end());
        if (m_dataFileType == DataFileTypeEnum::ANNOTATION) {
            AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::ANNOTATIONS);
            annMan->setAnnotationsForEditing(m_browserWindowIndex,
                                             selectedAnnotations);
        }
        else if (m_dataFileType == DataFileTypeEnum::SAMPLES) {
            AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::SAMPLES_EDITING);
            annMan->setAnnotationsForEditing(m_browserWindowIndex,
                                             selectedAnnotations);
        }
    }
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
    
    if (m_dataFileType == DataFileTypeEnum::ANNOTATION) {
        DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
        displayGroupOut = dpa->getDisplayGroupForTab(tabIndexOut);
    }
    else if (m_dataFileType == DataFileTypeEnum::SAMPLES) {
        DisplayPropertiesSamples* dps(GuiManager::get()->getBrain()->getDisplayPropertiesSamples());
        displayGroupOut = dps->getDisplayGroupForTab(tabIndexOut);
    }
}


/**
 * Update the content.
 *
 * @param contentItemsIn
 *     Items that are displayed.
 * @param displayGroup
 *     The display group.
 * @param tabIndex
 *     Index of the tab
 * @param allowSelectionFlag
 *     Allows selection of items by user clicking items.
 */
void
DisplayGroupAndTabItemViewController::updateContent(std::vector<DisplayGroupAndTabItemInterface*>& contentItemsIn,
                                                    const DisplayGroupEnum::Enum displayGroup,
                                                    const int32_t tabIndex,
                                                    const bool allowSelectionFlag)
{
    if (allowSelectionFlag) {
        m_treeWidget->setSelectionMode(QTreeWidget::ExtendedSelection);
    }
    else {
        m_treeWidget->setSelectionMode(QTreeWidget::NoSelection);
    }
    
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
    
    const int32_t numberOfChildrenToAdd = numValidChildren - numExistingChildren;
    for (int32_t i = 0; i < numberOfChildrenToAdd; i++) {
        m_treeWidget->addTopLevelItem(new DisplayGroupAndTabItemTreeWidgetItem(m_browserWindowIndex));
    }
    
    CaretAssert(m_treeWidget->topLevelItemCount() >= numValidChildren);
    
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

    updateSelectedAndExpandedCheckboxes(displayGroup,
                                        tabIndex);
    
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
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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

/**
 * Turn on all selected items
 */
void
DisplayGroupAndTabItemViewController::turnOnSelectedItemsTriggered()
{
    setCheckedStatusOfSelectedItems(true);
}

/**
 * Turn off all selected items
 */
void
DisplayGroupAndTabItemViewController::turnOffSelectedItemsTriggered()
{
    setCheckedStatusOfSelectedItems(false);
}

/**
 * Set the checked status of all selected items.  If none are selected select all
 *
 * @param checkedStatus
 *     Checked status
 */
void
DisplayGroupAndTabItemViewController::setCheckedStatusOfSelectedItems(const bool checkedStatus)
{
    QList<QTreeWidgetItem*> itemsSelected = m_treeWidget->selectedItems();
    
//    bool allFlag(false);
//    if (itemsSelected.isEmpty()) {
//        if (m_treeWidget != NULL) {
//            m_treeWidget->selectAll();
//            itemsSelected = m_treeWidget->selectedItems();
//            allFlag = true;
//        }
//    }
    
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    int32_t tabIndex = -1;
    getDisplayGroupAndTabIndex(displayGroup, tabIndex);
    
    
    const Qt::CheckState newCheckState = (checkedStatus
                                          ? Qt::Checked
                                          : Qt::Unchecked);
    QListIterator<QTreeWidgetItem*> iter(itemsSelected);
    while (iter.hasNext()) {
        QTreeWidgetItem* item = iter.next();
        DisplayGroupAndTabItemInterface* dataItem = getDataItem(item);
        
        const TriStateSelectionStatusEnum::Enum itemCheckState = DisplayGroupAndTabItemTreeWidgetItem::fromQCheckState(newCheckState);
        dataItem->setItemDisplaySelected(displayGroup,
                                         tabIndex,
                                         itemCheckState);
    }
    
    updateSelectedAndExpandedCheckboxes(displayGroup,
                                        tabIndex);
    updateSelectedAndExpandedCheckboxesInOtherViewControllers();
    
//    if (allFlag) {
//        m_treeWidget->clearSelection();
//    }

    updateGraphics();
}




/**
 * Called when collapse all action is triggered
 */
void
DisplayGroupAndTabItemViewController::collapseAllActionTriggered()
{
    m_treeWidget->collapseAll();
}

/**
 * Called when expand all action is triggered
 */
void
DisplayGroupAndTabItemViewController::expandAllActionTriggered()
{
    m_treeWidget->expandAll();
}

/**
 * Called when expand all action is triggered
 */
void
DisplayGroupAndTabItemViewController::allOnActionTriggered()
{
    if (m_treeWidget != NULL) {
        setCheckedStatusOfSelectedItems(true);
    }
}

/**
 * Called when expand all action is triggered
 */
void
DisplayGroupAndTabItemViewController::allOffActionTriggered()
{
    if (m_treeWidget != NULL) {
        setCheckedStatusOfSelectedItems(false);
    }
}

/**
 * Called when Info button is clicked
 */
void
DisplayGroupAndTabItemViewController::infoActionTriggered()
{
    //    const QModelIndex selectedIndex(m_treeView->currentIndex());
    //    if (selectedIndex.isValid()) {
    //        if (m_labelHierarchyModel != NULL) {
    //            QStandardItem* item(m_labelHierarchyModel->itemFromIndex(selectedIndex));
    //            if (item != NULL) {
    //                const LabelSelectionItem* labelItem(dynamic_cast<LabelSelectionItem*>(item));
    //                if (labelItem != NULL) {
    //                    const bool infoButtonFlag(true);
    //                    showSelectedItemMenu(labelItem,
    //                                         mapToGlobal(m_infoToolButton->pos()),
    //                                         infoButtonFlag);
    //                }
    //            }
    //        }
    //    }
}

/**
 * Called when find button is clicked or return is pressed in the find line edit
 */
void
DisplayGroupAndTabItemViewController::findActionTriggered()
{
    m_findItems.clear();
    m_findItemsCurrentIndex = 0;
    
    if (m_treeWidget != NULL) {
        const QString findText(m_findTextLineEdit->text().trimmed());
        
        const int modelColumn(0);
        m_findItems = m_treeWidget->findItems(findText,
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
DisplayGroupAndTabItemViewController::nextActionTriggered()
{
    scrollTreeViewToFindItem();
}

/**
 * Scroll the tree view to the next find item
 */
void
DisplayGroupAndTabItemViewController::scrollTreeViewToFindItem()
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
        const QModelIndex modelIndex(m_treeWidget->getIndexFromItem(item));
        if (modelIndex.isValid()) {
            m_treeWidget->setCurrentIndex(modelIndex);
            m_treeWidget->scrollTo(modelIndex,
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
DisplayGroupAndTabItemViewController::findTextLineEditTextChanged(const QString& text)
{
    m_findAction->setEnabled( ! text.trimmed().isEmpty());
    m_nextAction->setEnabled(false);
    m_findItems.clear();
    m_findItemsCurrentIndex = 0;
}

