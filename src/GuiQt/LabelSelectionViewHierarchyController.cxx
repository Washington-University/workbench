
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
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QToolButton>
#include <QTreeView>

#define __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__
#include "LabelSelectionViewHierarchyController.h"
#undef __LABEL_SELECTION_VIEW_HIERARCHY_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretHierarchy.h"
#include "CaretLogger.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "DisplayPropertiesLabels.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "LabelSelectionItem.h"
#include "LabelSelectionItemModel.h"
#include "SceneClass.h"
#include "SceneStringArray.h"
#include "WuQMacroManager.h"

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
    
    std::vector<DataFileTypeEnum::Enum> dataFileTypes { DataFileTypeEnum::VOLUME };
    std::vector<SubvolumeAttributes::VolumeType> volumeTypes { SubvolumeAttributes::LABEL };
    std::vector<StructureEnum::Enum> structures;
    
    QLabel* fileLabel(new QLabel("File"));
    QLabel* mapLabel(new QLabel("Map"));
    m_labelFileAndMapSelector = new CaretMappableDataFileAndMapSelectorObject(dataFileTypes,
                                                                              volumeTypes,
                                                                              CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                              this);
    QObject::connect(m_labelFileAndMapSelector, &CaretMappableDataFileAndMapSelectorObject::selectionWasPerformed,
                     this, &LabelSelectionViewHierarchyController::processFileSelectionChanged);
    QWidget* mapFileComboBox(NULL);
    QWidget* mapIndexSpinBox(NULL);
    QWidget* mapNameComboBox(NULL);
    m_labelFileAndMapSelector->getWidgetsForAddingToLayout(mapFileComboBox,
                                                           mapIndexSpinBox,
                                                           mapNameComboBox);
    
    QLabel* allLabel(new QLabel("All: "));
    
    m_collapseAllAction = new QAction("Collpase");
    QObject::connect(m_collapseAllAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::collapseAllActionTriggered);
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setDefaultAction(m_collapseAllAction);
    
    m_expandAllAction = new QAction("Expand");
    QObject::connect(m_expandAllAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::expandAllActionTriggered);
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setDefaultAction(m_expandAllAction);

    m_allOnAction = new QAction("On");
    QObject::connect(m_allOnAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::allOnActionTriggered);
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setDefaultAction(m_allOnAction);
    
    m_allOffAction = new QAction("Off");
    QObject::connect(m_allOffAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::allOffActionTriggered);
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setDefaultAction(m_allOffAction);

    QHBoxLayout* collpaseExpandLayout(new QHBoxLayout());
    collpaseExpandLayout->setContentsMargins(2, 2, 2, 2);
    collpaseExpandLayout->addWidget(allLabel);
    collpaseExpandLayout->addWidget(allOnToolButton);
    collpaseExpandLayout->addWidget(allOffToolButton);
    collpaseExpandLayout->addWidget(collapseAllToolButton);
    collpaseExpandLayout->addWidget(expandAllToolButton);
    collpaseExpandLayout->addStretch();
    
    m_treeView = new QTreeView();
    m_treeView->setHeaderHidden(true);
    m_treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    QObject::connect(m_treeView, &QTreeView::clicked,
                     this, &LabelSelectionViewHierarchyController::treeItemClicked);
    
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
    layout->addLayout(collpaseExpandLayout, row, 0, 1, 3);
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
 * Called when tree item is
 * @param modelIndex
 *     Model index that is
 */
void
LabelSelectionViewHierarchyController::treeItemClicked(const QModelIndex& modelIndex)
{
    if (modelIndex.isValid()) {
        auto model(modelIndex.model());
        if (model != NULL) {
            const QAbstractItemModel* model(modelIndex.model());
            if (model != NULL) {
                if (model == m_labelHierarchyModel) {
                    QStandardItem* standardItem(m_labelHierarchyModel->itemFromIndex(modelIndex));
                    if (standardItem != NULL) {
                        LabelSelectionItem* labelItem(dynamic_cast<LabelSelectionItem*>(standardItem));
                        if (labelItem != NULL) {
                            const auto checkState(standardItem->checkState());
                            labelItem->setAllChildrenChecked(checkState == Qt::Checked);
                        }
                        else {
                            CaretLogSevere("Item in label hieararchy is not a LabelSelectionItem");
                        }
                        
                        processSelectionChanges();
                    }
                }
                else {
                    CaretAssert("Model in label hieararchy is not m_labelHierarchyModel");
                }
            }
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
    bool enableTreeViewFlag(false);
    bool enableWidgetFlag(false);
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent != NULL) {
        const int32_t browserTabIndex(browserTabContent->getTabNumber());
        
        std::pair<CaretMappableDataFile*, int32_t> fileAndMapIndex(getSelectedFileAndMapIndex());
        CaretMappableDataFile* mapFile(fileAndMapIndex.first);
        
        if (mapFile != NULL) {
            if (mapFile->isMappedWithLabelTable()) {
                const int32_t mapIndex(fileAndMapIndex.second);
                if ((mapIndex >= 0)
                    && (mapIndex < mapFile->getNumberOfMaps())) {
                    enableWidgetFlag = true;
                    
                    const DisplayPropertiesLabels* dsl(GuiManager::get()->getBrain()->getDisplayPropertiesLabels());
                    CaretAssert(dsl);
                    LabelSelectionItemModel* selectionModel(mapFile->getLabelSelectionHierarchyForMapAndTab(mapIndex,
                                                                                                            dsl->getDisplayGroupForTab(browserTabIndex),
                                                                                                            browserTabIndex));
                    if (selectionModel != NULL) {
                        if (selectionModel->isValid()) {
                            const LabelSelectionItemModel* oldHierarchyModel(m_labelHierarchyModel);
                            m_labelHierarchyModel = selectionModel;
                            m_treeView->setModel(m_labelHierarchyModel);
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
                            
                            enableTreeViewFlag = true;
                        }
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
        
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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
 * @return A pair containing the selected file and map index
 */
std::pair<CaretMappableDataFile*, int32_t>
LabelSelectionViewHierarchyController::getSelectedFileAndMapIndex()
{
    CaretAssert(m_labelFileAndMapSelector);
    CaretMappableDataFileAndMapSelectionModel* model = m_labelFileAndMapSelector->getModel();
    CaretAssert(model);
    m_labelFileAndMapSelector->updateFileAndMapSelector(model);
    
    CaretMappableDataFile* mapFile  = model->getSelectedFile();
    const int32_t          mapIndex = model->getSelectedMapIndex();

    return std::make_pair(mapFile, mapIndex);
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
            if (m_treeView->isExpanded(lsi->index())) {
                expandedNames.push_back(lsi->text());
            }
        }
        
        if ( ! expandedNames.empty()) {
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
        std::set<AString> expandedNames;
        const int32_t numNames(expandedNamesArray->getNumberOfArrayElements());
        for (int32_t i = 0; i < numNames; i++) {
            expandedNames.insert(expandedNamesArray->stringValue(i));
        }
        
        if ( ! expandedNames.empty()) {
            m_treeView->collapseAll();
            
            const std::vector<LabelSelectionItem*> items(m_labelHierarchyModel->getAllDescendants());
            for (LabelSelectionItem* lsi : items) {
                if (expandedNames.find(lsi->text()) != expandedNames.end()) {
                    m_treeView->expand(lsi->index());
                }
            }
        }
    }
}



