
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
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QTableView>
#include <QTreeView>
#include <QToolButton>
#include <QVBoxLayout>

#define __FEATURE_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "FeatureSelectionViewController.h"
#undef __FEATURE_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesFeature.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "FeatureFile.h"
#include "GuiManager.h"
#include "FeatureLabelModel.h"
#include "FeatureItemModel.h"
#include "FeatureItem.h"
#include "SceneClass.h"
#include "WuQMacroManager.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::FeatureSelectionViewController 
 * \brief Widget for controlling display of features
 * \ingroup GuiQt
 *
 * Widget for controlling the display of features including
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
FeatureSelectionViewController::FeatureSelectionViewController(const int32_t browserWindowIndex,
                                                         const QString& parentObjectName,
                                                         QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":NueroAnn")
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Group");
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_displayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(displayGroupSelected(const DisplayGroupEnum::Enum)));
    m_displayGroupComboBox->getWidget()->setEnabled(false);
    
    /*
     * Hide group selection label and combo box
     */
    groupLabel->setVisible(false);
    m_displayGroupComboBox->getWidget()->setVisible(false);
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->setContentsMargins(0, 0, 0, 0);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    QLabel* fileLabel(new QLabel("File"));
    m_featureFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_featureFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     [=]() { this->updateFeatureItemsWidget(); });
    
    QLabel* volumeLabel(new QLabel("Volume"));
    m_volumeFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_volumeFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     [=]() { this->updateFeatureItemsWidget(); });

    QGridLayout* fileLayout(new QGridLayout());
    fileLayout->setColumnStretch(1, 100);
    fileLayout->setContentsMargins(0, 0, 0, 0);
    fileLayout->addWidget(fileLabel, 0, 0);
    fileLayout->addWidget(m_featureFileSelectionComboBox->getWidget(), 0, 1);
    fileLayout->addWidget(volumeLabel, 1, 0);
    fileLayout->addWidget(m_volumeFileSelectionComboBox->getWidget(), 1, 1);

    m_displayCheckBox = new QCheckBox("Display Points");
    m_displayCheckBox->setToolTip("Enable the display of points");
    QObject::connect(m_displayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_displayCheckBox->setObjectName(m_objectNamePrefix
                                            + ":DisplayFeatureItems");
    WuQMacroManager::instance()->addMacroSupportToObject(m_displayCheckBox,
                                                         "Enable points display");
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* featuresItemsWidget = this->createFeatureItemsWidget();
    QWidget* labelsWidget     = this->createLabelsWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                   this);
    m_tabWidget->getWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    m_tabWidget->addTab(featuresItemsWidget,
                        "Points");
    m_tabWidget->addTab(attributesWidget,
                        "Attributes");
    m_tabWidget->addTab(labelsWidget,
                        "Labels");
    m_tabWidget->setCurrentWidget(attributesWidget);
    m_tabWidget->getTabBar()->setToolTip("Select points tab");
    m_tabWidget->getTabBar()->setObjectName(m_objectNamePrefix
                                            + ":Tab");
    WuQMacroManager::instance()->addMacroSupportToObject(m_tabWidget->getTabBar(),
                                                         "Select features toolbox points tab");
    
    this->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_displayCheckBox);
    layout->addLayout(groupLayout);
    layout->addLayout(fileLayout);
    layout->addWidget(m_tabWidget->getWidget(), 100);
    layout->addStretch();

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    FeatureSelectionViewController::allFeatureSelectionViewControllers.insert(this);
    
    m_tabWidget->setCurrentIndex(0);
}

/**
 * Destructor.
 */
FeatureSelectionViewController::~FeatureSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    FeatureSelectionViewController::allFeatureSelectionViewControllers.erase(this);
}

/**
 * @return New instance of features selection widget
 */
QWidget* 
FeatureSelectionViewController::createFeatureItemsWidget()
{
    m_featureCollapseAllAction = new QAction("Collapse");
    m_featureCollapseAllAction->setToolTip("Collapse all items");
    QObject::connect(m_featureCollapseAllAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureCollapseAllActionTriggered);
    QToolButton* collapseAllToolButton(new QToolButton());
    collapseAllToolButton->setDefaultAction(m_featureCollapseAllAction);
    
    m_featureExpandAllAction = new QAction("Expand");
    m_featureExpandAllAction->setToolTip("Expand all items");
    QObject::connect(m_featureExpandAllAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureExpandAllActionTriggered);
    QToolButton* expandAllToolButton(new QToolButton());
    expandAllToolButton->setDefaultAction(m_featureExpandAllAction);
    
    m_featureAllOnAction = new QAction("On");
    m_featureAllOnAction->setToolTip("Turn all items on (check all)");
    QObject::connect(m_featureAllOnAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureAllOnActionTriggered);
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setDefaultAction(m_featureAllOnAction);
    
    m_featureAllOffAction = new QAction("Off");
    m_featureAllOffAction->setToolTip("Turn all items off (uncheck all)");
    QObject::connect(m_featureAllOffAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureAllOffActionTriggered);
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setDefaultAction(m_featureAllOffAction);
    
    m_featureMoreAction = new QAction("More");
    QObject::connect(m_featureMoreAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureMoreActionTriggered);
    m_featureMoreAction->setToolTip("Show additional option(s)");
    m_featureMoreAction->setEnabled(false);
    
    m_featureMoreToolButton = new QToolButton();
    m_featureMoreToolButton->setDefaultAction(m_featureMoreAction);
    
    m_featureInfoAction = new QAction("Info");
    m_featureInfoAction->setToolTip("Show information about selected label");
    m_featureInfoAction->setEnabled(false);
    QObject::connect(m_featureInfoAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureInfoActionTriggered);
    m_featureInfoToolButton = new QToolButton;
    m_featureInfoToolButton->setDefaultAction(m_featureInfoAction);
    
    m_featureFindAction = new QAction("Find");
    m_featureFindAction->setToolTip("Find the first item containing the text");
    m_featureFindAction->setEnabled(false);
    QObject::connect(m_featureFindAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureFindActionTriggered);
    QToolButton* findToolButton(new QToolButton);
    findToolButton->setDefaultAction(m_featureFindAction);
    
    m_featureNextAction = new QAction("Next");
    m_featureNextAction->setToolTip("Move to the next item containing the text (will wrap)");
    m_featureNextAction->setEnabled(false);
    QObject::connect(m_featureNextAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::featureNextActionTriggered);
    QToolButton* nextToolButton(new QToolButton);
    nextToolButton->setDefaultAction(m_featureNextAction);
    
    m_featureFindTextLineEdit = new QLineEdit();
    m_featureFindTextLineEdit->setToolTip("Enter find text here");
    QObject::connect(m_featureFindTextLineEdit, &QLineEdit::returnPressed,
                     this, &FeatureSelectionViewController::featureFindActionTriggered);
    QObject::connect(m_featureFindTextLineEdit, &QLineEdit::textChanged,
                     this, &FeatureSelectionViewController::featureFindTextLineEditTextChanged);
    
    QHBoxLayout* buttonsLayout(new QHBoxLayout());
    buttonsLayout->setSpacing(buttonsLayout->spacing() / 2);
    buttonsLayout->setContentsMargins(2, 2, 2, 2);
    buttonsLayout->addWidget(allOnToolButton);
    buttonsLayout->addWidget(allOffToolButton);
    buttonsLayout->addWidget(collapseAllToolButton);
    buttonsLayout->addWidget(expandAllToolButton);
    buttonsLayout->addSpacing(4);
    buttonsLayout->addWidget(m_featureInfoToolButton);
    buttonsLayout->addWidget(m_featureMoreToolButton);
    buttonsLayout->addSpacing(4);
    buttonsLayout->addWidget(findToolButton);
    buttonsLayout->addWidget(nextToolButton);
    buttonsLayout->addWidget(m_featureFindTextLineEdit,
                             100); /* stretch factor */

    m_featureItemTreeView = new QTreeView();
    QObject::connect(m_featureItemTreeView, &QTreeView::clicked,
                     this, &FeatureSelectionViewController::featureTreeItemClicked);
    
//    const int BIG_STRETCH(100);
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(layout->spacing() / 2);
    layout->addLayout(buttonsLayout);
    layout->addWidget(m_featureItemTreeView);//, BIG_STRETCH);
//    layout->addStretch();
    
    return widget;
}

/**
 * Called when tree item is
 * @param modelIndex
 *     Model index that is
 */
void
FeatureSelectionViewController::featureTreeItemClicked(const QModelIndex& modelIndex)
{
    if (m_selectedFeatureItemModel != NULL) {
        QStandardItem* item(m_selectedFeatureItemModel->itemFromIndex(modelIndex));
        if (item != NULL) {
            FeatureBase* featureBase(dynamic_cast<FeatureBase*>(item));
            const Qt::CheckState checkState(featureBase->checkState());
            featureBase->setAllChildrenChecked(checkState == Qt::Checked);
            m_selectedFeatureItemModel->updateCheckedStateOfAllItems();
            processSelectionChanges();
            
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        }
    }
}

/**
 * Called when tree item is
 * @param modelIndex
 *     Model index that is
 */
void
FeatureSelectionViewController::featureTreeItemDoubleClicked(const QModelIndex& /*modelIndex*/)
{
}


/**
 * Called when collapse all action is triggered
 */
void
FeatureSelectionViewController::featureCollapseAllActionTriggered()
{
    m_featureItemTreeView->collapseAll();
}

/**
 * Called when expand all action is triggered
 */
void
FeatureSelectionViewController::featureExpandAllActionTriggered()
{
    m_featureItemTreeView->expandAll();
}

/**
 * Called when expand all action is triggered
 */
void
FeatureSelectionViewController::featureAllOnActionTriggered()
{
    featureItemsAllOnOffButtonClicked(true);
}

/**
 * Called when expand all action is triggered
 */
void
FeatureSelectionViewController::featureAllOffActionTriggered()
{
    featureItemsAllOnOffButtonClicked(false);
}

/**
 * Called when Info button is clicked
 */
void
FeatureSelectionViewController::featureInfoActionTriggered()
{
// TBD
//
//    const LabelSelectionItem* labelItem(getLabelSelectionItemAtModelIndex(m_treeView->currentIndex()));
//    if (labelItem != NULL) {
//        const bool infoButtonFlag(true);
//        showSelectedItemMenu(labelItem,
//                             mapToGlobal(m_infoToolButton->pos()),
//                             infoButtonFlag);
//    }
//    else {
//        WuQMessageBoxTwo::information(this,
//                                      "Information",
//                                      "Select an item to get information about it");
//    }
}

/**
 * Called when more action triggered
 */
void
FeatureSelectionViewController::featureMoreActionTriggered()
{
// TBD
//
//    DisplayPropertiesLabels* dsl(GuiManager::get()->getBrain()->getDisplayPropertiesLabels());
//    
//    QMenu menu;
//    
//    QAction* showBranchesWithoutLabelsAction = menu.addAction("Show branches without labels");
//    showBranchesWithoutLabelsAction->setCheckable(true);
//    showBranchesWithoutLabelsAction->setChecked(dsl->isShowBranchesWithoutLabelsInHierarchies());
//    
//    QAction* unusedLabelsAction = menu.addAction("Show unused labels");
//    unusedLabelsAction->setCheckable(true);
//    unusedLabelsAction->setChecked(dsl->isShowUnusedLabelsInHierarchies());
//    
//    QAction* selectedAction = menu.exec(m_moreToolButton->mapToGlobal(QPoint(0, 0)));
//    if (selectedAction == showBranchesWithoutLabelsAction) {
//        dsl->setShowBranchesWithoutLabelsInHierarchies(showBranchesWithoutLabelsAction->isChecked());
//        resetFindItems();
//        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//    }
//    else if (selectedAction == unusedLabelsAction) {
//        dsl->setShowUnusedLabelsInHierarchies(unusedLabelsAction->isChecked());
//        resetFindItems();
//        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//    }
//    else if (selectedAction != NULL) {
//        CaretAssertMessage(0, "Has new menu item been added to More menu");
//    }
}

/**
 * Called when find button is clicked or return is pressed in the find line edit
 */
void
FeatureSelectionViewController::featureFindActionTriggered()
{
    m_featureFindItemModelIndices.clear();
    m_featureFindItemsCurrentIndex = 0;
    
    if (m_selectedFeatureItemModel != NULL) {
        const QString findText(m_featureFindTextLineEdit->text().trimmed());
        
        const int modelColumn(0);
        QList<QStandardItem*> matchingItems(m_selectedFeatureItemModel->findItems(findText,
                                                                             (Qt::MatchContains
                                                                              | Qt::MatchRecursive),
                                                                             modelColumn));
        for (QStandardItem* item : matchingItems) {
            const QModelIndex modelIndex(m_selectedFeatureItemModel->indexFromItem(item));
            if (modelIndex.isValid()) {
                m_featureFindItemModelIndices.push_back(modelIndex);
            }
        }
    }
    if (m_featureFindItemModelIndices.empty()) {
        GuiManager::get()->beep();
    }
    featureScrollTreeViewToFindItem();
}

/**
 * Called when next button is clicked
 */
void
FeatureSelectionViewController::featureNextActionTriggered()
{
    featureScrollTreeViewToFindItem();
}

/**
 * Scroll the tree view to the next find item
 */
void
FeatureSelectionViewController::featureScrollTreeViewToFindItem()
{
    const int32_t numFindItems(m_featureFindItemModelIndices.size());
    if (numFindItems > 0) {
        if ((m_featureFindItemsCurrentIndex < 0)
            || (m_featureFindItemsCurrentIndex >= numFindItems)) {
            m_featureFindItemsCurrentIndex = 0;
        }
        CaretAssertVectorIndex(m_featureFindItemModelIndices, m_featureFindItemsCurrentIndex);
        const QModelIndex modelIndex(m_featureFindItemModelIndices[m_featureFindItemsCurrentIndex]);
        if (modelIndex.isValid()) {
            m_featureItemTreeView->setCurrentIndex(modelIndex);
            m_featureItemTreeView->scrollTo(modelIndex,
                                            QTreeView::PositionAtCenter);
        }
        
        /*
         * For 'next'
         */
        ++m_featureFindItemsCurrentIndex;
    }
    
    m_featureNextAction->setEnabled(numFindItems > 1);
}

/**
 * Called when next button is clicked
 * @param text
 *    Text in the line edit
 */
void
FeatureSelectionViewController::featureFindTextLineEditTextChanged(const QString& text)
{
    m_featureFindAction->setEnabled( ! text.trimmed().isEmpty());
    m_featureNextAction->setEnabled(false);
    m_featureFindItemModelIndices.clear();
    m_featureFindItemsCurrentIndex = 0;
}

/**
 * Reset find items but and clear find text
 */
void
FeatureSelectionViewController::featureResetFindItemsAndFindText()
{
    m_featureFindTextLineEdit->clear();
    featureResetFindItems();
}

/**
 * Reset find items but do not clear find text
 */
void
FeatureSelectionViewController::featureResetFindItems()
{
    featureFindTextLineEditTextChanged(m_featureFindTextLineEdit->text());
}


/**
 * @return New instance of labels selection widget
 */
QWidget*
FeatureSelectionViewController::createLabelsWidget()
{
    const int BIG_STRETCH(100);

    QLabel* fileLabel(new QLabel("Labels"));
    m_labelModelSelectionComboBox = new QComboBox();
    QObject::connect(m_labelModelSelectionComboBox, QOverload<int>::of(&QComboBox::activated),
                     this,&FeatureSelectionViewController::labelModelComboBoxActivated);
    QHBoxLayout* fileLayout(new QHBoxLayout());
    fileLayout->addWidget(fileLabel);
    fileLayout->addWidget(m_labelModelSelectionComboBox, BIG_STRETCH);

    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setText("On");
    QObject::connect(allOnToolButton, &QToolButton::clicked,
                     [=]() { labelsAllOnOffButtonClicked(true); });
    
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setText("Off");
    QObject::connect(allOffToolButton, &QToolButton::clicked,
                     [=]() { labelsAllOnOffButtonClicked(false); });
    
    m_labelFindAction = new QAction("Find");
    m_labelFindAction->setToolTip("Find the first item containing the text");
    m_labelFindAction->setEnabled(false);
    QObject::connect(m_labelFindAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::labelFindActionTriggered);
    QToolButton* findToolButton(new QToolButton);
    findToolButton->setDefaultAction(m_labelFindAction);
    
    m_labelNextAction = new QAction("Next");
    m_labelNextAction->setToolTip("Move to the next item containing the text (will wrap)");
    m_labelNextAction->setEnabled(false);
    QObject::connect(m_labelNextAction, &QAction::triggered,
                     this, &FeatureSelectionViewController::labelNextActionTriggered);
    QToolButton* nextToolButton(new QToolButton);
    nextToolButton->setDefaultAction(m_labelNextAction);
    
    m_labelFindTextLineEdit = new QLineEdit();
    m_labelFindTextLineEdit->setToolTip("Enter find text here");
    QObject::connect(m_labelFindTextLineEdit, &QLineEdit::returnPressed,
                     this, &FeatureSelectionViewController::labelFindActionTriggered);
    QObject::connect(m_labelFindTextLineEdit, &QLineEdit::textChanged,
                     this, &FeatureSelectionViewController::labelFindTextLineEditTextChanged);
    
    QHBoxLayout* buttonsLayout(new QHBoxLayout());
    buttonsLayout->setSpacing(buttonsLayout->spacing() / 2);
    buttonsLayout->setContentsMargins(2, 2, 2, 2);
    buttonsLayout->addWidget(allOnToolButton);
    buttonsLayout->addWidget(allOffToolButton);
    buttonsLayout->addWidget(findToolButton);
    buttonsLayout->addWidget(nextToolButton);
    buttonsLayout->addWidget(m_labelFindTextLineEdit, 100);

    m_labelsTableView = new QTableView();
    m_labelsTableView->horizontalHeader()->setVisible(false);
    m_labelsTableView->verticalHeader()->setVisible(false);
    QObject::connect(m_labelsTableView, &QTableView::clicked,
                     this, &FeatureSelectionViewController::labelTableViewItemClicked);

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(layout->spacing() / 2);
    layout->addLayout(fileLayout);
    layout->addLayout(buttonsLayout);
    layout->addWidget(m_labelsTableView);//, BIG_STRETCH);
//    layout->addStretch();

    return widget;
}

/**
 * Called when find button is clicked or return is pressed in the find line edit
 */
void
FeatureSelectionViewController::labelFindActionTriggered()
{
    m_labelFindItemModelIndices.clear();
    m_labelFindItemsCurrentIndex = 0;
    
    FeatureLabelModel* labelModel(getSelectedLabelModel());
    if (labelModel != NULL) {
        const QString findText(m_labelFindTextLineEdit->text().trimmed());
        
        const int modelColumn(0);
        QList<QStandardItem*> matchingItems(labelModel->findItems(findText,
                                                                  (Qt::MatchContains
                                                                   | Qt::MatchRecursive),
                                                                  modelColumn));
        for (QStandardItem* item : matchingItems) {
            const QModelIndex modelIndex(labelModel->indexFromItem(item));
            if (modelIndex.isValid()) {
                m_labelFindItemModelIndices.push_back(modelIndex);
            }
        }
    }
    if (m_labelFindItemModelIndices.empty()) {
        GuiManager::get()->beep();
    }
    labelScrollTreeViewToFindItem();
}

/**
 * Called when next button is clicked
 */
void
FeatureSelectionViewController::labelNextActionTriggered()
{
    labelScrollTreeViewToFindItem();
}

/**
 * Scroll the tree view to the next find item
 */
void
FeatureSelectionViewController::labelScrollTreeViewToFindItem()
{
    const int32_t numFindItems(m_labelFindItemModelIndices.size());
    if (numFindItems > 0) {
        if ((m_labelFindItemsCurrentIndex < 0)
            || (m_labelFindItemsCurrentIndex >= numFindItems)) {
            m_labelFindItemsCurrentIndex = 0;
        }
        CaretAssertVectorIndex(m_labelFindItemModelIndices, m_labelFindItemsCurrentIndex);
        const QModelIndex modelIndex(m_labelFindItemModelIndices[m_labelFindItemsCurrentIndex]);
        if (modelIndex.isValid()) {
            m_labelsTableView->setCurrentIndex(modelIndex);
            m_labelsTableView->scrollTo(modelIndex,
                                        QTreeView::PositionAtCenter);
        }
        
        /*
         * For 'next'
         */
        ++m_labelFindItemsCurrentIndex;
    }
    
    m_labelNextAction->setEnabled(numFindItems > 1);
}

/**
 * Called when next button is clicked
 * @param text
 *    Text in the line edit
 */
void
FeatureSelectionViewController::labelFindTextLineEditTextChanged(const QString& text)
{
    m_labelFindAction->setEnabled( ! text.trimmed().isEmpty());
    m_labelNextAction->setEnabled(false);
    m_labelFindItemModelIndices.clear();
    m_labelFindItemsCurrentIndex = 0;
}

/**
 * Reset find items but and clear find text
 */
void
FeatureSelectionViewController::labelResetFindItemsAndFindText()
{
    m_labelFindTextLineEdit->clear();
    labelResetFindItems();
}

/**
 * Reset find items but do not clear find text
 */
void
FeatureSelectionViewController::labelResetFindItems()
{
    labelFindTextLineEditTextChanged(m_labelFindTextLineEdit->text());
}

/**
 * @return The attributes widget.
 */
QWidget*
FeatureSelectionViewController::createAttributesWidget()
{
    //WuQMacroManager* macroManager = WuQMacroManager::instance();

    QLabel* symbolScaleLabel(new QLabel("Symbol Scale"));
    m_symbolScaleSpinBox = new QDoubleSpinBox();
    m_symbolScaleSpinBox->setRange(0.01, 10000.0);
    m_symbolScaleSpinBox->setSingleStep(1.0);
    QObject::connect(m_symbolScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=]() { processAttributesChanges(); });
    
    QLabel* distanceToVolumeSliceLabel = new QLabel("Distanct to Volume Slice");
    m_distanceToVolumeSliceSpinBox = new QDoubleSpinBox();
    m_distanceToVolumeSliceSpinBox->setRange(0.0, 10000.0);
    m_distanceToVolumeSliceSpinBox->setSingleStep(1.0);
    m_distanceToVolumeSliceSpinBox->setSuffix("mm");
    QObject::connect(m_distanceToVolumeSliceSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=]() { processAttributesChanges(); });

    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    layout->setVerticalSpacing(layout->verticalSpacing() / 2);
//    layout->setAlignment(Qt::AlignTop);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    layout->setContentsMargins(0, 0, 0, 0);
    int32_t row(0);
    layout->addWidget(symbolScaleLabel, row, 0);
    layout->addWidget(m_symbolScaleSpinBox, row, 1);
    ++row;
    layout->addWidget(distanceToVolumeSliceLabel, row, 0);
    layout->addWidget(m_distanceToVolumeSliceSpinBox, row, 1);
    ++row;
    layout->setRowStretch(row, 100);

    return widget;
}

/**
 * Called when a widget on the attributes page has 
 * its value changed.
 */
void 
FeatureSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesFeature* dpna = GuiManager::get()->getBrain()->getDisplayPropertiesFeature();
        
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpna->getDisplayGroupForTab(browserTabIndex);
    
    dpna->setDisplayed(displayGroup,
                       browserTabIndex,
                       m_displayCheckBox->isChecked());
    
    dpna->setSymbolScale(m_symbolScaleSpinBox->value());
    dpna->setDistanceFromVolumeSliceTolerance(m_distanceToVolumeSliceSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
    updateOtherFeatureViewControllers();
}

/**
 * Called when the features display group combo box is changed.
 */
void 
FeatureSelectionViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    /*
     * Update selected display group in model.
     */
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesFeature* dpna = brain->getDisplayPropertiesFeature();
    dpna->setDisplayGroupForTab(browserTabIndex,
                                displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateFeatureViewController();
    
    /*
     * Apply the changes.
     */
    processSelectionChanges();
}

/**
 * Update the features widget.
 */
void 
FeatureSelectionViewController::updateFeatureViewController()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesFeature* dpna = brain->getDisplayPropertiesFeature();
    const DisplayGroupEnum::Enum displayGroup = dpna->getDisplayGroupForTab(browserTabIndex);
    
    setWindowTitle("Points");
    
    m_displayGroupComboBox->setSelectedDisplayGroup(dpna->getDisplayGroupForTab(browserTabIndex));
    m_displayCheckBox->setChecked(dpna->isDisplayed(displayGroup, browserTabIndex));
    
    QSignalBlocker symbolSizeBlocker(m_symbolScaleSpinBox);
    m_symbolScaleSpinBox->setValue(dpna->getSymbolScale());
    
    QSignalBlocker distanceToVolumeSlice(m_distanceToVolumeSliceSpinBox);
    m_distanceToVolumeSliceSpinBox->setValue(dpna->getDistanceFromVolumeSliceTolerance());
    
    updateFeatureItemsWidget();
    
    updateLabelWidget();
}

/**
 * Update the feature items tab
 */
void
FeatureSelectionViewController::updateFeatureItemsWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    
    QAbstractItemModel* previousModel(m_featureItemTreeView->model());
    DisplayPropertiesFeature* dpna = brain->getDisplayPropertiesFeature();
    m_featureFileSelectionComboBox->updateComboBox(dpna->getFeatureFileSelectionModel());
    
    m_selectedFeatureItemModel = NULL;
    
    CaretDataFile* cdf(m_featureFileSelectionComboBox->getSelectedFile());
    if (cdf != NULL) {
        FeatureFile* featureFile(dynamic_cast<FeatureFile*>(cdf));
        CaretAssert(featureFile);
        
        m_volumeFileSelectionComboBox->updateComboBox(featureFile->getVolumeFileSelectionModel());
        
        m_selectedFeatureItemModel = featureFile->getFeatureItemModel();
        CaretAssert(m_selectedFeatureItemModel);
        
        m_featureItemTreeView->setModel(m_selectedFeatureItemModel);
        m_featureItemTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        
        if (previousModel != m_selectedFeatureItemModel) {
            m_featureItemTreeView->expandAll();
        }
    }
    else {
        m_featureItemTreeView->setModel(NULL);
        m_volumeFileSelectionComboBox->updateComboBox(NULL);
    }
}

/**
 * Called when feature items all on/off button clicked
 * @param onFlag
 *   True if on clicked, false if off clicked
 */
void
FeatureSelectionViewController::featureItemsAllOnOffButtonClicked(const bool onFlag)
{
    if (m_selectedFeatureItemModel != NULL) {
        m_selectedFeatureItemModel->setCheckedStatusOfAllItems(onFlag);
    }
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Update other features view controllers.
 */
void 
FeatureSelectionViewController::updateOtherFeatureViewControllers()
{
    for (std::set<FeatureSelectionViewController*>::iterator iter = FeatureSelectionViewController::allFeatureSelectionViewControllers.begin();
         iter != FeatureSelectionViewController::allFeatureSelectionViewControllers.end();
         iter++) {
        FeatureSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateFeatureViewController();
        }
    }
}

/**
 * Issue update events after selections are changed.
 */
void 
FeatureSelectionViewController::processSelectionChanges()
{
    updateOtherFeatureViewControllers();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
FeatureSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdate = false;
    
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        if (uiEvent->isUpdateForWindow(m_browserWindowIndex)) {
            if (uiEvent->isToolBoxUpdate()) {
                doUpdate = true;
                uiEvent->setEventProcessed();
            }
        }
    }

    if (doUpdate) {
        updateFeatureViewController();
    }
}

/**
 * Called when a label model is selected
 * @parain index
 *    Index of item selected
 */
void
FeatureSelectionViewController::labelModelComboBoxActivated(int /*index*/)
{
    FeatureLabelModel* labelModel(getSelectedLabelModel());
    m_labelsTableView->setModel(labelModel);
    if (labelModel != NULL) {
        const int32_t numCols(labelModel->columnCount());
        for (int32_t i = 0; i < numCols; i++) {
            m_labelsTableView->resizeColumnToContents(i);
        }
    }
}

/**
 * Update the label widget.
 */
void
FeatureSelectionViewController::updateLabelWidget()
{
    FeatureLabelModel* previousSelectedLabelModel(getSelectedLabelModel());
    m_labelModelSelectionComboBox->clear();
    
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesFeature* dpna = brain->getDisplayPropertiesFeature();
    FeatureFile* featureFile(dpna->getSelectedFeatureFile());
    if (featureFile != NULL) {
        int32_t selectedIndex(-1);
        const int32_t numLabelModels(featureFile->getNumberOfLabelModels());
        for (int32_t i = 0; i < numLabelModels; i++) {
            FeatureLabelModel* labelModel(featureFile->getLabelModel(i));
            if (labelModel == previousSelectedLabelModel) {
                selectedIndex = m_labelModelSelectionComboBox->count();
            }
            m_labelModelSelectionComboBox->addItem(labelModel->getDescription(),
                                                   QVariant::fromValue(labelModel));
        }
        
        if (selectedIndex >= 0) {
            m_labelModelSelectionComboBox->setCurrentIndex(selectedIndex);
        }
    }
    
    if (previousSelectedLabelModel != getSelectedLabelModel()) {
        labelModelComboBoxActivated(m_labelModelSelectionComboBox->currentIndex());
    }
}

/**
 * @return The selected label model
 */
FeatureLabelModel*
FeatureSelectionViewController::getSelectedLabelModel()
{
    FeatureLabelModel* labelModel(NULL);
    
    if (m_labelModelSelectionComboBox->count() > 0) {
        const QVariant data(m_labelModelSelectionComboBox->currentData());
        labelModel = data.value<FeatureLabelModel*>();
    }
    
    return labelModel;
}


/**
 * Called when user clicks on an item in the label table view
 */
void
FeatureSelectionViewController::labelTableViewItemClicked(const QModelIndex& /*index*/)
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when labels all on/off button clicked
 * @param onFlag
 *   True if on clicked, false if off clicked
 */
void
FeatureSelectionViewController::labelsAllOnOffButtonClicked(const bool onFlag)
{
    FeatureLabelModel* labelModel(getSelectedLabelModel());
    if (labelModel != NULL) {
        labelModel->setAllLabelsDisplayed(onFlag);
    }
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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
FeatureSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FeatureSelectionViewController",
                                            1);
    sceneClass->addClass(m_tabWidget->saveToScene(sceneAttributes,
                                                  "m_tabWidget"));
    sceneClass->addInteger("selectedTabIndex",
                           m_tabWidget->currentIndex());
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
FeatureSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
    const int32_t tabIndex(sceneClass->getIntegerValue("selectedTabIndex",
                                                       0));
    m_tabWidget->setCurrentIndex(tabIndex);
}


