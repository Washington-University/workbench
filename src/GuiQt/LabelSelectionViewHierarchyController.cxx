
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

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QTreeView>

#define __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__
#include "LabelSelectionViewHierarchyController.h"
#undef __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretHierarchy.h"
#include "CaretLogger.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "CaretPreferences.h"
#include "Cluster.h"
#include "DisplayPropertiesLabels.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventIdentificationHighlightLocation.h"
#include "EventIdentificationHighlightStereotaxicLocationsInTabs.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "LabelSelectionItem.h"
#include "LabelSelectionItemModel.h"
#include "LabelSelectionItemModelProxyFilter.h"
#include "SceneClass.h"
#include "SceneStringArray.h"
#include "SessionManager.h"
#include "WuQMacroManager.h"
#include "WuQMessageBoxTwo.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::LabelSelectionViewHierarchyController 
 * \brief Widget for controlling display of labels
 * \ingroup GuiQt
 *
 * Widget for controlling the display of labels including
 * different display groups.
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *    Index of browser window
 * @param parentObjectName
 *    Name of parent object
 * @param parent
 *    The parent object
 */
LabelSelectionViewHierarchyController::LabelSelectionViewHierarchyController(const int32_t browserWindowIndex,
                                                           const QString& parentObjectName,
                                                           QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":LabelHierarchy")
{
    m_browserWindowIndex = browserWindowIndex;
    
    
    m_proxyFilter = new LabelSelectionItemModelProxyFilter(this);

    std::vector<DataFileTypeEnum::Enum> dataFileTypes {
        DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL,
        DataFileTypeEnum::LABEL,
        DataFileTypeEnum::VOLUME
    };
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes { SubvolumeAttributes::LABEL };
    std::vector<StructureEnum::Enum> structures;
    
    QLabel* fileLabel(new QLabel("File"));
    QLabel* mapLabel(new QLabel("Map"));
    m_labelFileAndMapSelector = new CaretMappableDataFileAndMapSelectorObject(CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                              this);
    QObject::connect(m_labelFileAndMapSelector, &CaretMappableDataFileAndMapSelectorObject::selectionWasPerformed,
                     this, &LabelSelectionViewHierarchyController::processFileSelectionChanged);
    QWidget* mapFileComboBox(NULL);
    QWidget* mapIndexSpinBox(NULL);
    QWidget* mapNameComboBox(NULL);
    m_labelFileAndMapSelector->getWidgetsForAddingToLayout(mapFileComboBox,
                                                           mapIndexSpinBox,
                                                           mapNameComboBox);
    
    QLabel* showNameLabel(new QLabel("Show"));
    m_showNameComboBox = new QComboBox();
    QHBoxLayout* showNameLayout(new QHBoxLayout());
    showNameLayout->setContentsMargins(0, 0, 0, 0);
    showNameLayout->addWidget(showNameLabel, 0);
    showNameLayout->addWidget(m_showNameComboBox, 100);
    showNameLayout->addStretch();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QObject::connect(m_showNameComboBox, &QComboBox::textActivated,
#else
    QObject::connect(m_showNameComboBox, QOverload<const QString&>::of(&QComboBox::activated),
#endif
                     this, &LabelSelectionViewHierarchyController::showNameComboBoxActivated);
    
    m_collapseAllAction = new QAction("Collapse");
    m_collapseAllAction->setToolTip("Collapse all items");
    QObject::connect(m_collapseAllAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::collapseAllActionTriggered);
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setDefaultAction(m_collapseAllAction);
    
    m_expandAllAction = new QAction("Expand");
    m_expandAllAction->setToolTip("Expand all items");
    QObject::connect(m_expandAllAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::expandAllActionTriggered);
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setDefaultAction(m_expandAllAction);

    m_allOnAction = new QAction("On");
    m_allOnAction->setToolTip("Turn all items on (check all)");
    QObject::connect(m_allOnAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::allOnActionTriggered);
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setDefaultAction(m_allOnAction);
    
    m_allOffAction = new QAction("Off");
    m_allOffAction->setToolTip("Turn all items off (uncheck all)");
    QObject::connect(m_allOffAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::allOffActionTriggered);
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setDefaultAction(m_allOffAction);

    QAction* moreAction(new QAction("More"));
    QObject::connect(moreAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::moreActionTriggered);
    moreAction->setToolTip("Show additional option(s)");
                     
    m_moreToolButton = new QToolButton();
    m_moreToolButton->setDefaultAction(moreAction);
                     
    m_infoAction = new QAction("Info");
    m_infoAction->setToolTip("Show information about selected label");
    m_infoAction->setEnabled(false);
    QObject::connect(m_infoAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::infoActionTriggered);
    m_infoToolButton = new QToolButton;
    m_infoToolButton->setDefaultAction(m_infoAction);
    
    m_findAction = new QAction("Find");
    m_findAction->setToolTip("Find the first item containing the text");
    m_findAction->setEnabled(false);
    QObject::connect(m_findAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::findActionTriggered);
    QToolButton* findToolButton(new QToolButton);
    findToolButton->setDefaultAction(m_findAction);
    
    m_nextAction = new QAction("Next");
    m_nextAction->setToolTip("Move to the next item containing the text (will wrap)");
    m_nextAction->setEnabled(false);
    QObject::connect(m_nextAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::nextActionTriggered);
    QToolButton* nextToolButton(new QToolButton);
    nextToolButton->setDefaultAction(m_nextAction);

    m_findTextLineEdit = new QLineEdit();
    m_findTextLineEdit->setToolTip("Enter find text here");
    QObject::connect(m_findTextLineEdit, &QLineEdit::returnPressed,
                     this, &LabelSelectionViewHierarchyController::findActionTriggered);
    QObject::connect(m_findTextLineEdit, &QLineEdit::textChanged,
                     this, &LabelSelectionViewHierarchyController::findTextLineEditTextChanged);

    QHBoxLayout* buttonsLayout(new QHBoxLayout());
    buttonsLayout->setSpacing(buttonsLayout->spacing() / 2);
    buttonsLayout->setContentsMargins(2, 2, 2, 2);
    buttonsLayout->addWidget(allOnToolButton);
    buttonsLayout->addWidget(allOffToolButton);
    buttonsLayout->addWidget(collapseAllToolButton);
    buttonsLayout->addWidget(expandAllToolButton);
    buttonsLayout->addSpacing(4);
    buttonsLayout->addWidget(m_infoToolButton);
    buttonsLayout->addWidget(m_moreToolButton);
    buttonsLayout->addSpacing(4);
    buttonsLayout->addWidget(findToolButton);
    buttonsLayout->addWidget(nextToolButton);
    buttonsLayout->addWidget(m_findTextLineEdit,
                             100); /* stretch factor */
    
    m_treeView = new QTreeView();
    m_treeView->setEditTriggers(QTreeView::NoEditTriggers); /* prevent editing text if double-clicked */
    m_treeView->setExpandsOnDoubleClick(false); /* do not collapse/expand if double-clicked */
    m_treeView->setHeaderHidden(true);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(m_treeView, &QTreeView::clicked,
                     this, &LabelSelectionViewHierarchyController::treeItemClicked);
    QObject::connect(m_treeView, &QTreeView::doubleClicked,
                     this, &LabelSelectionViewHierarchyController::treeItemDoubleClicked);
    QObject::connect(m_treeView, &QTreeView::customContextMenuRequested,
                     this, &LabelSelectionViewHierarchyController::showTreeViewContextMenu);

    QGridLayout* layout(new QGridLayout(this));
    layout->setVerticalSpacing(layout->verticalSpacing() / 2);
    layout->setColumnStretch(2, 100);
    layout->setContentsMargins(0, 0, 0, 0);
    int row(0);
    layout->addWidget(fileLabel, row, 0);
    layout->addWidget(mapFileComboBox, row, 1, 1, 2);
    ++row;
    layout->addWidget(mapLabel, row, 0);
    layout->addWidget(mapIndexSpinBox, row, 1);
    layout->addWidget(mapNameComboBox, row, 2);
    ++row;
    layout->addLayout(showNameLayout, row, 0, 1, 3);
    ++row;
    layout->addLayout(buttonsLayout, row, 0, 1, 3);
    ++row;
    layout->addWidget(m_treeView, row, 0, 1, 3);
    layout->setRowStretch(row, 100);
}

/**
 * Destructor.
 */
LabelSelectionViewHierarchyController::~LabelSelectionViewHierarchyController()
{
}

/**
 * @return The browser tab index
 */
int32_t
LabelSelectionViewHierarchyController::getBrowserTabIndex() const
{
    int32_t tabIndex(-1);
    BrainBrowserWindow* bbw(GuiManager::get()->getBrowserWindowByWindowIndex(m_browserWindowIndex));
    CaretAssert(bbw);
    const BrowserTabContent* btc(bbw->getBrowserTabContent());
    if (btc != NULL) {
        tabIndex = btc->getTabNumber();
    }
    return tabIndex;
}


/**
 * Called when tree item is
 * @param modelIndex
 *     Model index that is
 */
void
LabelSelectionViewHierarchyController::treeItemDoubleClicked(const QModelIndex& /*modelIndex*/)
{
}

/**
 * @return The LabelSelectionItem at the model index or NULL if not available.
 * @param modelIndex
 *    The model index.
 */
LabelSelectionItem*
LabelSelectionViewHierarchyController::getLabelSelectionItemAtModelIndex(const QModelIndex& modelIndex)
{
    /*
     * The input model index is a model index in the proxy filter.
     * We need to to remap to a model index in the label selection model
     * to get the actual label item.
     */
    if (modelIndex.isValid()) {
        const QAbstractItemModel* constModel(modelIndex.model());
        if (constModel != NULL) {
            const LabelSelectionItemModelProxyFilter* proxyFilter(dynamic_cast<const LabelSelectionItemModelProxyFilter*>(constModel));
            if (proxyFilter != NULL) {
                const QModelIndex sourceModelIndex(proxyFilter->mapToSource(modelIndex));
                if (sourceModelIndex.isValid()) {
                    if (m_labelHierarchyModel != NULL) {
                        QStandardItem* standardItem(m_labelHierarchyModel->itemFromIndex(sourceModelIndex));
                        if (standardItem != NULL) {
                            LabelSelectionItem* lsi(dynamic_cast<LabelSelectionItem*>(standardItem));
                            return lsi;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

/**
 * Called when tree item is
 * @param modelIndex
 *     Model index that is
 */
void
LabelSelectionViewHierarchyController::treeItemClicked(const QModelIndex& modelIndex)
{
    LabelSelectionItem* labelItem(getLabelSelectionItemAtModelIndex(modelIndex));
    if (labelItem != NULL) {
        const auto checkState(labelItem->checkState());
        labelItem->setAllChildrenChecked(checkState == Qt::Checked);
        
        m_infoAction->setEnabled(true);

        processSelectionChanges();
    }
}

/**
 * Show a context menu for the tree view
 * @param pos
 *    Position in the tree view
 */
void
LabelSelectionViewHierarchyController::showTreeViewContextMenu(const QPoint& pos)
{
    const QModelIndex modelIndex(m_treeView->indexAt(pos));
    LabelSelectionItem* labelItem(getLabelSelectionItemAtModelIndex(modelIndex));
    if (labelItem != NULL) {
        const bool infoButtonFlag(false);
        showSelectedItemMenu(labelItem,
                             m_treeView->mapToGlobal(pos),
                             infoButtonFlag);
    }
}

/**
 * Show a menu for the selected label
 * @param labelItem
 *    The label item that is selected
 * @param pos
 *    Position for the menu
 * @param infoButtonFlag
 *    If true, menu is for the Info button, else right-click menu on label
 */
void
LabelSelectionViewHierarchyController::showSelectedItemMenu(const LabelSelectionItem* labelItem,
                                                            const QPoint& pos,
                                                            const bool /*infoButtonFlag*/)
{
    CaretAssert(labelItem);
    const QString labelName(labelItem->text());
    
    const int32_t browserTabIndex(getBrowserTabIndex());
    if (browserTabIndex < 0) {
        return;
    }
    
    QMenu menu(this);
    
    QAction* infoAction(menu.addAction("Info..."));

    QAction* copyNameToClipboardAction(menu.addAction("Copy name to clipboard"));
    
    menu.addSeparator();

    /*
     * Crosshair/pointer icon
     */
    QIcon volumeCrossHairIcon;
    const bool volumeCrossHairIconValid =
    WuQtUtilities::loadIcon(":/ToolBar/volume-crosshair-pointer.png",
                            volumeCrossHairIcon);

    /*
     * My clusters
     */
    std::vector<QAction*> clusterActions;
    std::vector<ClusterTypeEnum::Enum> clusterTypes;
    std::vector<Vector3D> clusterXYZs;
    const LabelSelectionItem::CogSet* allCogSet(labelItem->getMyAndChildrenCentersOfGravity());
    if (allCogSet != NULL) {
        const std::vector<const LabelSelectionItem::COG*> cogs(allCogSet->getCOGs());
        for (const LabelSelectionItem::COG* c : cogs) {
            QAction* a(menu.addAction(c->getTitle()));
            if (volumeCrossHairIconValid) {
                a->setIcon(volumeCrossHairIcon);
                a->setIconVisibleInMenu(true);
            }
            clusterActions.push_back(a);
            clusterXYZs.push_back(c->getXYZ());
            clusterTypes.push_back(c->getClusterType());
        }
        CaretAssert(clusterActions.size() == clusterXYZs.size());
        CaretAssert(clusterActions.size() == clusterTypes.size());
    }
    const LabelSelectionItem::CogSet* cogSet(labelItem->getCentersOfGravity());
    if (cogSet != NULL) {
        if ( ! clusterActions.empty()) {
            menu.addSeparator();
        }
        const std::vector<const LabelSelectionItem::COG*> cogs(cogSet->getCOGs());
        for (const LabelSelectionItem::COG* c : cogs) {
            QAction* a(menu.addAction(c->getTitle()));
            if (volumeCrossHairIconValid) {
                a->setIcon(volumeCrossHairIcon);
                a->setIconVisibleInMenu(true);
            }
            clusterActions.push_back(a);
            clusterXYZs.push_back(c->getXYZ());
            clusterTypes.push_back(c->getClusterType());
        }
        CaretAssert(clusterActions.size() == clusterXYZs.size());
        CaretAssert(clusterActions.size() == clusterTypes.size());
    }
    
    if ( ! menu.actions().isEmpty()) {
        QAction* selectedAction(menu.exec(pos));
        if (selectedAction != NULL) {
            if (selectedAction == infoAction) {
                WuQMessageBoxTwo::information(this, 
                                              "Info",
                                              labelItem->getTextForInfoDisplay());
            }
            else if (selectedAction == copyNameToClipboardAction) {
                const AString name(labelItem->text().trimmed());
                QApplication::clipboard()->setText(name,
                                                   QClipboard::Clipboard);
            }
            else {
                /*
                 * User selected a label cluster
                 */
                const bool labelIdFlag(true);
                for (int32_t i = 0; i < static_cast<int32_t>(clusterActions.size()); i++) {
                    if (selectedAction == clusterActions[i]) {
                        CaretAssertVectorIndex(clusterXYZs, i);
                        const Vector3D cogXYZ(clusterXYZs[i]);
                        if (labelIdFlag) {
                            CaretAssertVectorIndex(clusterTypes, i);
                            const ClusterTypeEnum::Enum clusterType(clusterTypes[i]);
                            
                            /*
                             * Get the label file and map and its hierarchy text
                             */
                            std::pair<CaretMappableDataFile*, int32_t> fileAndMapIndex(getSelectedFileAndMapIndex());
                            CaretMappableDataFile* mapFile(fileAndMapIndex.first);
                            CaretAssert(mapFile);
                            const int32_t mapIndex(fileAndMapIndex.second);
                            CaretAssert((mapIndex >= 0)
                                        && (mapIndex < mapFile->getNumberOfMaps()));
                            const GiftiMetaData* metadata(mapFile->getMapMetaData(mapIndex));
                            CaretAssert(metadata);
                            const AString caretHierarchyText(metadata->get("CaretHierarchy"));
                            
                            if (caretHierarchyText.isEmpty()) {
                                WuQMessageBoxTwo::critical(this, "Error",
                                                           ("CaretHierarchy not found in metadata for map"
                                                            + AString::number(mapIndex)));
                            }

                            /*
                             * label selected by the user
                             */
                            EventIdentificationHighlightStereotaxicLocationsInTabs highlightEvent(caretHierarchyText);
                            CaretAssertVectorIndex(clusterActions, i);
                            highlightEvent.addLabelAndStereotaxicXYZ(labelName,
                                                                     clusterType,
                                                                     cogXYZ);
                            
                            /*
                             * Also include the parent labels
                             */
                            std::vector<LabelSelectionItem*> parentItems(labelItem->getAncestors());
                            for (LabelSelectionItem* pi : parentItems) {
                                /*
                                 * For parent's always use a COG that is parent and its children
                                 * Many parents do not have just 'LABEL' COGs
                                 */
                                ClusterTypeEnum::Enum parentClusterType(clusterType);
                                switch (clusterType) {
                                    case ClusterTypeEnum::LABEL_AND_CHILDREN_ALL:
                                        break;
                                    case ClusterTypeEnum::LABEL_AND_CHILDREN_CENTRAL:
                                        break;
                                    case ClusterTypeEnum::LABEL_AND_CHILDREN_LEFT:
                                        break;
                                    case ClusterTypeEnum::LABEL_AND_CHILDREN_RIGHT:
                                        break;
                                    case ClusterTypeEnum::LABEL_ALL:
                                        parentClusterType = ClusterTypeEnum::LABEL_AND_CHILDREN_ALL;
                                        break;
                                    case ClusterTypeEnum::LABEL_CENTRAL:
                                        parentClusterType = ClusterTypeEnum::LABEL_AND_CHILDREN_CENTRAL;
                                        break;
                                    case ClusterTypeEnum::LABEL_LEFT:
                                        parentClusterType = ClusterTypeEnum::LABEL_AND_CHILDREN_LEFT;
                                        break;
                                    case ClusterTypeEnum::LABEL_RIGHT:
                                        parentClusterType = ClusterTypeEnum::LABEL_AND_CHILDREN_RIGHT;
                                        break;
                                }
                                const LabelSelectionItem::COG* cog(pi->getCogWithClusterType(parentClusterType));
                                if (cog != NULL) {
                                    highlightEvent.addLabelAndStereotaxicXYZ(pi->text(),
                                                                             cog->getClusterType(),
                                                                             cog->getXYZ());
                                }
//                                pi->getAllCOGS();
//                                const LabelSelectionItem::CogSet* cogSet(pi->getMyAndChildrenCentersOfGravity());
//                                if (cogSet != NULL) {
//                                    const LabelSelectionItem::COG* cog(cogSet->getAllCOG());
//                                    if (cog != NULL) {
//                                        highlightEvent.addLabelAndStereotaxicXYZ(pi->text(),
//                                                                                 cog->getClusterType(),
//                                                                                 cog->getXYZ());
//                                    }
//                                }
                            }
                            EventManager::get()->sendEvent(highlightEvent.getPointer());
                            
                            /*
                             * Browser tabs that receive the event will add
                             * identification text to the event
                             */
                            const AString resultsText(highlightEvent.getIdentificationText());
                            if (highlightEvent.getEventProcessCount() > 0) {
                                if ( ! resultsText.isEmpty()) {
                                    EventUpdateInformationWindows infoEvent(resultsText);
                                    EventManager::get()->sendEvent(infoEvent.getPointer());
                                }
                            }
                            else {
                                WuQMessageBoxTwo::warning(this,
                                                          "Warning",
                                                          "No matching labels were found in tabs");
                            }
                        }
                        else {
                            EventIdentificationHighlightLocation highlightLocation(browserTabIndex,
                                                                                   cogXYZ,
                                                                                   cogXYZ,
                                                                                   EventIdentificationHighlightLocation::LOAD_FIBER_ORIENTATION_SAMPLES_MODE_NO);
                            EventManager::get()->sendEvent(highlightLocation.getPointer());
                        }
                        break;
                    }
                }
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
LabelSelectionViewHierarchyController::setCheckedStatusOfAllChildren(QStandardItem* item,
                                                                     const Qt::CheckState checkState)
{
    CaretAssert(item);
    const int32_t numChildren(item->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* child(item->child(iRow));
        child->setCheckState(checkState);
        setCheckedStatusOfAllChildren(child,
                                      checkState);
    }
}

/**
 * Called when the file selection is changed
 */
void
LabelSelectionViewHierarchyController::processFileSelectionChanged()
{
    /*
     * Since display group has changed, need to update controls
     */
    updateLabelViewController();
    
    /*
     * Apply the changes.
     */
    processSelectionChanges();
}

/**
 * Update the label selection widget.
 */
void
LabelSelectionViewHierarchyController::updateLabelViewController()
{
    const int32_t browserTabIndex(getBrowserTabIndex());
    if (browserTabIndex < 0) {
        setDisabled(true);
        return;
    }
    setEnabled(true);

    DisplayPropertiesLabels* dpl(GuiManager::get()->getBrain()->getDisplayPropertiesLabels());
    CaretAssert(dpl);
    const DisplayGroupEnum::Enum displayGroup(dpl->getDisplayGroupForTab(browserTabIndex));

    m_labelFileAndMapSelector->updateFileAndMapSelector(dpl->getFileSelectionModel(displayGroup,
                                                                                   browserTabIndex));
    bool enableTreeViewFlag(false);
    bool enableWidgetFlag(false);
    
    m_showNameComboBox->clear();
    
    std::pair<CaretMappableDataFile*, int32_t> fileAndMapIndex(getSelectedFileAndMapIndex());
    CaretMappableDataFile* mapFile(fileAndMapIndex.first);
    
    if (mapFile != NULL) {
        if (mapFile->isMappedWithLabelTable()) {
            const int32_t mapIndex(fileAndMapIndex.second);
            if ((mapIndex >= 0)
                && (mapIndex < mapFile->getNumberOfMaps())) {
                enableWidgetFlag = true;
                
                LabelSelectionItemModel* selectionModel(mapFile->getLabelSelectionHierarchyForMapAndTab(mapIndex,
                                                                                                        displayGroup,
                                                                                                        browserTabIndex));
                if (selectionModel != NULL) {
                    if (selectionModel->isValid()) {
                        const LabelSelectionItemModel* oldHierarchyModel(m_labelHierarchyModel);
                        m_labelHierarchyModel = selectionModel;
                        m_proxyFilter->setSourceModel(m_labelHierarchyModel);
                        m_treeView->setModel(m_proxyFilter);
                        m_treeView->setEnabled(true);
                        if (m_labelHierarchyModel != oldHierarchyModel) {
                            /*
                             * If model has changed and NO top level items are expanded,
                             * expand all items
                             */
                            bool topLevelItemExpandedFlag(false);
                            const std::vector<LabelSelectionItem*> topLevelItems(m_labelHierarchyModel->getTopLevelItems());
                            for (const LabelSelectionItem* item : topLevelItems) {
                                if (m_treeView->isExpanded(item->index())) {
                                    topLevelItemExpandedFlag = true;
                                    break;
                                }
                            }
                            if ( ! topLevelItemExpandedFlag) {
                                m_treeView->expandAll();
                            }
                        }
                        m_treeView->adjustSize();
                        
                        resetFindItems();
                        
                        const AString selectedName(m_labelHierarchyModel->getSelectedAlternativeName());
                        int32_t showNameIndex(0);
                        const std::vector<AString> altNamesList(m_labelHierarchyModel->getAllAlternativeNames());
                        const int32_t numNames(altNamesList.size());
                        for (int32_t i = 0; i < numNames; i++) {
                            CaretAssertVectorIndex(altNamesList, i);
                            m_showNameComboBox->addItem(altNamesList[i]);
                            if (selectedName == altNamesList[i]) {
                                showNameIndex = i;
                            }
                        }
                        if ((showNameIndex >= 0) &&
                            (showNameIndex < numNames)) {
                            m_showNameComboBox->setCurrentIndex(showNameIndex);
                        }
                        enableTreeViewFlag = true;
                    }
                }
            }
        }
    }

    if (enableTreeViewFlag) {
        m_treeView->setEnabled(true);
    }
    else {
        m_labelHierarchyModel = NULL;
        m_treeView->setModel(NULL);
        m_treeView->setEnabled(false);
        
        resetFindItemsAndFindText();
    }
    
    m_treeView->adjustSize();
    
    m_allOnAction->setEnabled(enableTreeViewFlag);
    m_allOffAction->setEnabled(enableTreeViewFlag);
    m_collapseAllAction->setEnabled(enableTreeViewFlag);
    m_expandAllAction->setEnabled(enableTreeViewFlag);
    
    setEnabled(enableWidgetFlag);
}

/**
 * Issue update events after selections are changed.
 */
void 
LabelSelectionViewHierarchyController::processSelectionChanges()
{
    if (m_labelHierarchyModel != NULL) {
        m_labelHierarchyModel->updateCheckedStateOfAllItems();
        
        std::pair<CaretMappableDataFile*, int32_t> fileAndMapIndex(getSelectedFileAndMapIndex());
        CaretMappableDataFile* mapFile(fileAndMapIndex.first);
        const int32_t mapIndex(fileAndMapIndex.second);
        
        if (mapFile != NULL) {
            mapFile->updateScalarColoringForMap(mapIndex);
            if (mapFile->isSurfaceMappable()) {
                EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
            }
        }
        
        const bool copyToLabelTableFlag(true);
        m_labelHierarchyModel->synchronizeSelectionsWithLabelTable(copyToLabelTableFlag);
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when show name combo box has an item selected by user
 * @param text
 *    Text that was selected
 */
void
LabelSelectionViewHierarchyController::showNameComboBoxActivated(const AString& text)
{
    if (m_labelHierarchyModel != NULL) {
        m_labelHierarchyModel->setSelectedAlternativeName(text);
    }
}

/**
 * Called when collapse all action is triggered
 */
void
LabelSelectionViewHierarchyController::collapseAllActionTriggered()
{
    m_treeView->collapseAll();
}

/**
 * Called when expand all action is triggered
 */
void
LabelSelectionViewHierarchyController::expandAllActionTriggered()
{
    m_treeView->expandAll();
}

/**
 * Called when expand all action is triggered
 */
void
LabelSelectionViewHierarchyController::allOnActionTriggered()
{
    if (m_labelHierarchyModel != NULL) {
        m_labelHierarchyModel->setCheckedStatusOfAllItems(true);
        processSelectionChanges();
    }
}

/**
 * Called when expand all action is triggered
 */
void
LabelSelectionViewHierarchyController::allOffActionTriggered()
{
    if (m_labelHierarchyModel != NULL) {
        m_labelHierarchyModel->setCheckedStatusOfAllItems(false);
        processSelectionChanges();
    }
}

/**
 * Called when Info button is clicked
 */
void
LabelSelectionViewHierarchyController::infoActionTriggered()
{
    const LabelSelectionItem* labelItem(getLabelSelectionItemAtModelIndex(m_treeView->currentIndex()));
    if (labelItem != NULL) {
        const bool infoButtonFlag(true);
        showSelectedItemMenu(labelItem,
                             mapToGlobal(m_infoToolButton->pos()),
                             infoButtonFlag);
    }
    else {
        WuQMessageBoxTwo::information(this,
                                      "Information",
                                      "Select an item to get information about it");
    }
}

/**
 * Called when more action triggered
 */
void
LabelSelectionViewHierarchyController::moreActionTriggered()
{
    DisplayPropertiesLabels* dsl(GuiManager::get()->getBrain()->getDisplayPropertiesLabels());

    QMenu menu;
    
    QAction* showBranchesWithoutLabelsAction = menu.addAction("Show branches without labels");
    showBranchesWithoutLabelsAction->setCheckable(true);
    showBranchesWithoutLabelsAction->setChecked(dsl->isShowBranchesWithoutLabelsInHierarchies());
    
    QAction* unusedLabelsAction = menu.addAction("Show unused labels");
    unusedLabelsAction->setCheckable(true);
    unusedLabelsAction->setChecked(dsl->isShowUnusedLabelsInHierarchies());
    
    QAction* selectedAction = menu.exec(m_moreToolButton->mapToGlobal(QPoint(0, 0)));
    if (selectedAction == showBranchesWithoutLabelsAction) {
        dsl->setShowBranchesWithoutLabelsInHierarchies(showBranchesWithoutLabelsAction->isChecked());
        resetFindItems();
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else if (selectedAction == unusedLabelsAction) {
        dsl->setShowUnusedLabelsInHierarchies(unusedLabelsAction->isChecked());
        resetFindItems();
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    else if (selectedAction != NULL) {
        CaretAssertMessage(0, "Has new menu item been added to More menu");
    }
}

/**
 * Called when find button is clicked or return is pressed in the find line edit
 */
void
LabelSelectionViewHierarchyController::findActionTriggered()
{
    m_findItemModelIndices.clear();
    m_findItemsCurrentIndex = 0;
    
    if (m_labelHierarchyModel != NULL) {
        const QString findText(m_findTextLineEdit->text().trimmed());
        
        const int modelColumn(0);
        QList<QStandardItem*> matchingItems(m_labelHierarchyModel->findItems(findText,
                                                                             (Qt::MatchContains
                                                                              | Qt::MatchRecursive),
                                                                             modelColumn));
        for (QStandardItem* item : matchingItems) {
            const QModelIndex sourceModelIndex(m_labelHierarchyModel->indexFromItem(item));
            if (sourceModelIndex.isValid()) {
                const QModelIndex modelIndex(m_proxyFilter->mapFromSource(sourceModelIndex));
                if (modelIndex.isValid()) {
                    m_findItemModelIndices.push_back(modelIndex);
                }
            }
        }
    }
    if (m_findItemModelIndices.empty()) {
        GuiManager::get()->beep();
    }
    scrollTreeViewToFindItem();
}

/**
 * Called when next button is clicked
 */
void
LabelSelectionViewHierarchyController::nextActionTriggered()
{
    scrollTreeViewToFindItem();
}

/**
 * Scroll the tree view to the next find item
 */
void
LabelSelectionViewHierarchyController::scrollTreeViewToFindItem()
{
    const int32_t numFindItems(m_findItemModelIndices.size());
    if (numFindItems > 0) {
        if ((m_findItemsCurrentIndex < 0)
            || (m_findItemsCurrentIndex >= numFindItems)) {
            m_findItemsCurrentIndex = 0;
        }
        CaretAssertVectorIndex(m_findItemModelIndices, m_findItemsCurrentIndex);
        const QModelIndex modelIndex(m_findItemModelIndices[m_findItemsCurrentIndex]);
        if (modelIndex.isValid()) {
            m_treeView->setCurrentIndex(modelIndex);
            m_treeView->scrollTo(modelIndex,
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
LabelSelectionViewHierarchyController::findTextLineEditTextChanged(const QString& text)
{
    m_findAction->setEnabled( ! text.trimmed().isEmpty());
    m_nextAction->setEnabled(false);
    m_findItemModelIndices.clear();
    m_findItemsCurrentIndex = 0;
}

/**
 * Reset find items but and clear find text
 */
void
LabelSelectionViewHierarchyController::resetFindItemsAndFindText()
{
    m_findTextLineEdit->clear();
    resetFindItems();
}

/**
 * Reset find items but do not clear find text
 */
void
LabelSelectionViewHierarchyController::resetFindItems()
{
    findTextLineEditTextChanged(m_findTextLineEdit->text());
}


/**
 * @return A pair containing the selected file and map index
 */
std::pair<CaretMappableDataFile*, int32_t>
LabelSelectionViewHierarchyController::getSelectedFileAndMapIndex()
{
    CaretMappableDataFile* mapFile(NULL);
    int32_t mapIndex(-1);

    CaretAssert(m_labelFileAndMapSelector);
    CaretMappableDataFileAndMapSelectionModel* model = m_labelFileAndMapSelector->getModel();
    if (model != NULL) {
        mapFile  = model->getSelectedFile();
        mapIndex = model->getSelectedMapIndex();
    }

    return std::make_pair(mapFile, mapIndex);
}

/**
 * @return Get the label selection model from the abstract model
 * @param abstractItemModel
 *    The abstract item mode.
 */
LabelSelectionItemModel*
LabelSelectionViewHierarchyController::getLabelSelectionModel(QAbstractItemModel* abstractItemModel) const
{
    if (abstractItemModel != NULL) {
        LabelSelectionItemModelProxyFilter* model(dynamic_cast<LabelSelectionItemModelProxyFilter*>(abstractItemModel));
        if (model != NULL) {
            LabelSelectionItemModel* labelModel(dynamic_cast<LabelSelectionItemModel*>(model->sourceModel()));
            return labelModel;
        }
    }
    return NULL;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
LabelSelectionViewHierarchyController::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                           const AString& instanceName)
{
    /*
     * Make sure current model (m_labelHierarchyModel) is valid
     */
    updateLabelViewController();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionViewHierarchyController",
                                            1);
    
    if (m_labelHierarchyModel != NULL) {
        std::vector<LabelSelectionItem*> items(m_labelHierarchyModel->getAllDescendants());
        std::vector<AString> expandedNames;
        for (const LabelSelectionItem* lsi : items) {
            const QModelIndex sourceModelIndex(lsi->index());
            if (sourceModelIndex.isValid()) {
                QModelIndex modelIndex(m_proxyFilter->mapFromSource(sourceModelIndex));
                if (modelIndex.isValid()) {
                    if (m_treeView->isExpanded(modelIndex)) {
                        expandedNames.push_back(lsi->getPrimaryName());
                    }
                }
            }
        }
        
        /* Always add, even if empty that occurs when all items are collapsed */
        if (expandedNames.empty()) {
            sceneClass->addStringArray("expandedNames", NULL, 0);
        }
        else {
            sceneClass->addStringArray("expandedNames", &expandedNames[0], expandedNames.size());
        }
    }
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
LabelSelectionViewHierarchyController::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                                const SceneClass* sceneClass)
{
    updateLabelViewController();
    m_treeView->expandAll();
    
    if ((sceneClass == NULL)
        || (m_labelHierarchyModel == NULL)) {
        return;
    }
    
    const ScenePrimitiveArray* expandedNamesArray(sceneClass->getPrimitiveArray("expandedNames"));
    if (expandedNamesArray != NULL) {
        /* Could be empty in which case all items are collapsed */
        std::set<AString> expandedNames;
        const int32_t numNames(expandedNamesArray->getNumberOfArrayElements());
        for (int32_t i = 0; i < numNames; i++) {
            expandedNames.insert(expandedNamesArray->stringValue(i));
        }
        
        m_treeView->collapseAll();
        
        const std::vector<LabelSelectionItem*> items(m_labelHierarchyModel->getAllDescendants());
        for (LabelSelectionItem* lsi : items) {
            if (expandedNames.find(lsi->getPrimaryName()) != expandedNames.end()) {
                const QModelIndex sourceModelIndex(lsi->index());
                if (sourceModelIndex.isValid()) {
                    const QModelIndex modelIndex(m_proxyFilter->mapFromSource(sourceModelIndex));
                    if (modelIndex.isValid()) {
                        m_treeView->expand(modelIndex);
                    }
                }
            }
        }
    }
}



