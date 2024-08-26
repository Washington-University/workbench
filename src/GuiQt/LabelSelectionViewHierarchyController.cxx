
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
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "DisplayPropertiesLabels.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
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
    
    QAction* collapseAllAction(new QAction("Collpase All"));
    QObject::connect(collapseAllAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::collapseAllActionTriggered);
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setDefaultAction(collapseAllAction);
    
    QAction* expandAllAction(new QAction("Expand All"));
    QObject::connect(expandAllAction, &QAction::triggered,
                     this, &LabelSelectionViewHierarchyController::expandAllActionTriggered);
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setDefaultAction(expandAllAction);

    QHBoxLayout* collpaseExpandLayout(new QHBoxLayout());
    collpaseExpandLayout->setContentsMargins(2, 2, 2, 2);
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
    std::cout << "Clicked " << std::endl;
    if (modelIndex.isValid()) {
        auto model(modelIndex.model());
        if (model != NULL) {
            const QAbstractItemModel* model(modelIndex.model());
            if (model != NULL) {
                if (model == m_labelHierarchyModel) {
                    QStandardItem* standardItem(m_labelHierarchyModel->itemFromIndex(modelIndex));
                    if (standardItem != NULL) {
                        AString checkText;
                        const auto checkState(standardItem->checkState());
                        switch (checkState) {
                            case Qt::Unchecked:
                                checkText = "Unchecked";
                                standardItem->setCheckState(checkState);
                                setCheckedStatusOfAllChildren(standardItem,
                                                              checkState);
                                break;
                            case Qt::PartiallyChecked:
                                checkText = "Partially Checked";
                                break;
                            case Qt::Checked:
                                checkText = "Checked";
                                standardItem->setCheckState(checkState);
                                setCheckedStatusOfAllChildren(standardItem, 
                                                              checkState);
                                break;
                        }
                        std::cout << "Row: " << modelIndex.row()
                        << " Column: " << modelIndex.column()
                        << " Name: "
                        << standardItem->text()
                        << " Checked: "
                        << checkText << std::endl;
                        
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
    processLabelSelectionChanges();
}

/**
 * Update the label selection widget.
 */
void 
LabelSelectionViewHierarchyController::updateLabelViewController()
{
    setWindowTitle("Labels");
    
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        setEnabled(false);
        return;
    }
    
    CaretMappableDataFileAndMapSelectionModel* model = m_labelFileAndMapSelector->getModel();
    m_labelFileAndMapSelector->updateFileAndMapSelector(model);
    
    CaretMappableDataFile* mapFile  = model->getSelectedFile();
    const int32_t          mapIndex = model->getSelectedMapIndex();
    
    if (mapFile == NULL) {
        m_treeView->setEnabled(false);
        return;
    }
    
    setEnabled(true);
    
    if ((mapIndex < 0)
        || (mapIndex >= mapFile->getNumberOfMaps())) {
        m_treeView->setEnabled(false);
        return;
    }
    if ( ! mapFile->isMappedWithLabelTable()) {
        m_treeView->setEnabled(false);
        return;
    }

    const GiftiLabelTable* labelTable(mapFile->getMapLabelTable(mapIndex));
    if (labelTable == NULL) {
        m_treeView->setEnabled(false);
        return;
    }
    
    const CaretHierarchy& caretHierarchy = labelTable->getHierarchy();
    if (caretHierarchy.isEmpty()) {
        m_treeView->setEnabled(false);
        return;
    }
    
    const auto oldModel(m_labelHierarchyModel);
    m_labelHierarchyModel = caretHierarchy.buildQSIModel();
    m_treeView->setModel(m_labelHierarchyModel);
    m_treeView->setEnabled(true);
    if (oldModel != m_treeView->model()) {
        m_treeView->expandAll();
    }
    m_treeView->adjustSize();    
}

/**
 * Gets called when label selections are changed.
 */
void 
LabelSelectionViewHierarchyController::processLabelSelectionChanges()
{
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
LabelSelectionViewHierarchyController::processSelectionChanges()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
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
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionViewHierarchyController",
                                            1);
    
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
    if (sceneClass == NULL) {
        return;
    }
}



