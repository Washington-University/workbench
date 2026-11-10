
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

#define __NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER_DECLARE__
#include "NeuroglancerAnnotationViewController.h"
#undef __NEUROGLANCER_ANNOTATION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "ComboBoxItemInterfaceComboBox.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesNeuroglancerAnnotation.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "FeatureLabel.h"
#include "FeatureLabelModel.h"
#include "NeuroglancerAnnotationFile.h"
#include "NeuroglancerPropertyInfo.h"
#include "SceneClass.h"
#include "WuQMacroManager.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotationViewController 
 * \brief Widget for controlling display of neuroglancer annotations
 * \ingroup GuiQt
 *
 * Widget for controlling the display of neuroglancer annotations including
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
NeuroglancerAnnotationViewController::NeuroglancerAnnotationViewController(const int32_t browserWindowIndex,
                                                         const QString& parentObjectName,
                                                         QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(parentObjectName
                   + ":NueroAnn")
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Display Group");
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_displayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(displayGroupSelected(const DisplayGroupEnum::Enum)));
    m_displayGroupComboBox->getWidget()->setEnabled(false);
    
    /*
     * Hide group selectigvon label and combo box
     */
    groupLabel->setVisible(false);
    m_displayGroupComboBox->getWidget()->setVisible(false);
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->setContentsMargins(0, 0, 0, 0);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    QLabel* fileLabel(new QLabel("File"));
    m_fileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_fileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     [=]() { this->updateViewController(); });

    QLabel* colorLabel(new QLabel("Color"));
    m_colorPropertySelectionComboBox = new ComboBoxItemInterfaceComboBox(this);
    QObject::connect(m_colorPropertySelectionComboBox, &ComboBoxItemInterfaceComboBox::itemActivated,
                     this, &NeuroglancerAnnotationViewController::colorPropertySelected);
    
    QGridLayout* fileLayout(new QGridLayout());
    fileLayout->setColumnStretch(1, 100);
    fileLayout->setContentsMargins(0, 0, 0, 0);
    fileLayout->addWidget(fileLabel, 0, 0);
    fileLayout->addWidget(m_fileSelectionComboBox->getWidget(), 0, 1);
    fileLayout->addWidget(colorLabel, 1, 0);
    fileLayout->addWidget(m_colorPropertySelectionComboBox->getWidget(), 1, 1);

    m_displayCheckBox = new QCheckBox("Display Points");
    m_displayCheckBox->setToolTip("Enable the display of points");
    QObject::connect(m_displayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_displayCheckBox->setObjectName(m_objectNamePrefix
                                            + ":DisplayNeuroglancerAnnotationItems");
    WuQMacroManager::instance()->addMacroSupportToObject(m_displayCheckBox,
                                                         "Enable points display");
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* labelsWidget     = this->createLabelsWidget();
    QWidget* transWidget      = this->createTransformationWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                   this);
    m_tabWidget->getWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    m_tabWidget->addTab(attributesWidget,
                        "Attributes");
    m_tabWidget->addTab(labelsWidget,
                        "Labels");
    m_tabWidget->addTab(transWidget,
                        "Transformation");
    m_tabWidget->setCurrentWidget(attributesWidget);
    m_tabWidget->getTabBar()->setToolTip("Select points tab");
    m_tabWidget->getTabBar()->setObjectName(m_objectNamePrefix
                                            + ":Tab");
    WuQMacroManager::instance()->addMacroSupportToObject(m_tabWidget->getTabBar(),
                                                         "Select neuroglancer annotation toolbox tab");
    
    this->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_displayCheckBox);
    layout->addLayout(groupLayout);
    layout->addLayout(fileLayout);
    layout->addWidget(m_tabWidget->getWidget(), 100);
    layout->addStretch();

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    NeuroglancerAnnotationViewController::allNeuroglancerAnnotationViewControllers.insert(this);
    
    m_tabWidget->setCurrentIndex(0);
}

/**
 * Destructor.
 */
NeuroglancerAnnotationViewController::~NeuroglancerAnnotationViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    NeuroglancerAnnotationViewController::allNeuroglancerAnnotationViewControllers.erase(this);
}

/**
 * @return New instance of labels selection widget
 */
QWidget*
NeuroglancerAnnotationViewController::createLabelsWidget()
{
    const int BIG_STRETCH(100);

    QLabel* fileLabel(new QLabel("Labels"));
    m_labelModelSelectionComboBox = new QComboBox();
    QObject::connect(m_labelModelSelectionComboBox, QOverload<int>::of(&QComboBox::activated),
                     this,&NeuroglancerAnnotationViewController::labelModelComboBoxActivated);
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
                     this, &NeuroglancerAnnotationViewController::labelFindActionTriggered);
    QToolButton* findToolButton(new QToolButton);
    findToolButton->setDefaultAction(m_labelFindAction);
    
    m_labelNextAction = new QAction("Next");
    m_labelNextAction->setToolTip("Move to the next item containing the text (will wrap)");
    m_labelNextAction->setEnabled(false);
    QObject::connect(m_labelNextAction, &QAction::triggered,
                     this, &NeuroglancerAnnotationViewController::labelNextActionTriggered);
    QToolButton* nextToolButton(new QToolButton);
    nextToolButton->setDefaultAction(m_labelNextAction);
    
    m_labelFindTextLineEdit = new QLineEdit();
    m_labelFindTextLineEdit->setToolTip("Enter find text here");
    QObject::connect(m_labelFindTextLineEdit, &QLineEdit::returnPressed,
                     this, &NeuroglancerAnnotationViewController::labelFindActionTriggered);
    QObject::connect(m_labelFindTextLineEdit, &QLineEdit::textChanged,
                     this, &NeuroglancerAnnotationViewController::labelFindTextLineEditTextChanged);
    
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
                     this, &NeuroglancerAnnotationViewController::labelTableViewItemClicked);

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(layout->spacing() / 2);
    layout->addLayout(fileLayout);
    layout->addLayout(buttonsLayout);
    layout->addWidget(m_labelsTableView);

    return widget;
}

/**
 * Called when find button is clicked or return is pressed in the find line edit
 */
void
NeuroglancerAnnotationViewController::labelFindActionTriggered()
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
NeuroglancerAnnotationViewController::labelNextActionTriggered()
{
    labelScrollTreeViewToFindItem();
}

/**
 * Scroll the tree view to the next find item
 */
void
NeuroglancerAnnotationViewController::labelScrollTreeViewToFindItem()
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
NeuroglancerAnnotationViewController::labelFindTextLineEditTextChanged(const QString& text)
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
NeuroglancerAnnotationViewController::labelResetFindItemsAndFindText()
{
    m_labelFindTextLineEdit->clear();
    labelResetFindItems();
}

/**
 * Reset find items but do not clear find text
 */
void
NeuroglancerAnnotationViewController::labelResetFindItems()
{
    labelFindTextLineEditTextChanged(m_labelFindTextLineEdit->text());
}

/**
 * @return The attributes widget.
 */
QWidget*
NeuroglancerAnnotationViewController::createAttributesWidget()
{
    //WuQMacroManager* macroManager = WuQMacroManager::instance();

    QLabel* symbolScaleLabel(new QLabel("Symbol Scale"));
    m_symbolScaleSpinBox = new QDoubleSpinBox();
    m_symbolScaleSpinBox->setRange(0.01, 10000.0);
    m_symbolScaleSpinBox->setSingleStep(1.0);
    QObject::connect(m_symbolScaleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=]() { processAttributesChanges(); });
    
    QLabel* distanceToVolumeSliceLabel = new QLabel("Distance to Volume Slice");
    m_distanceToVolumeSliceSpinBox = new QDoubleSpinBox();
    m_distanceToVolumeSliceSpinBox->setRange(0.0, 10000.0);
    m_distanceToVolumeSliceSpinBox->setSingleStep(1.0);
    m_distanceToVolumeSliceSpinBox->setSuffix("mm");
    QObject::connect(m_distanceToVolumeSliceSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=]() { processAttributesChanges(); });
    
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    layout->setVerticalSpacing(layout->verticalSpacing() / 2);
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
NeuroglancerAnnotationViewController::processAttributesChanges()
{
    DisplayPropertiesNeuroglancerAnnotation* dpna = GuiManager::get()->getBrain()->getDisplayPropertiesNeuroglancerAnnotation();
        
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
    
    updateOtherViewControllers();
}

/**
 * Called when the neuroglancer annotation display group combo box is changed.
 */
void 
NeuroglancerAnnotationViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    DisplayPropertiesNeuroglancerAnnotation* dpna = brain->getDisplayPropertiesNeuroglancerAnnotation();
    dpna->setDisplayGroupForTab(browserTabIndex,
                                displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateViewController();
    
    /*
     * Apply the changes.
     */
    processSelectionChanges();
}

/**
 * Update the widget.
 */
void 
NeuroglancerAnnotationViewController::updateViewController()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesNeuroglancerAnnotation* dpna = brain->getDisplayPropertiesNeuroglancerAnnotation();
    const DisplayGroupEnum::Enum displayGroup = dpna->getDisplayGroupForTab(browserTabIndex);
    
    setWindowTitle("Points");
    
    m_displayGroupComboBox->setSelectedDisplayGroup(dpna->getDisplayGroupForTab(browserTabIndex));
    m_displayCheckBox->setChecked(dpna->isDisplayed(displayGroup, browserTabIndex));
    
    QSignalBlocker symbolSizeBlocker(m_symbolScaleSpinBox);
    m_symbolScaleSpinBox->setValue(dpna->getSymbolScale());
    
    QSignalBlocker volumeDistanceBlocker(m_distanceToVolumeSliceSpinBox);
    m_distanceToVolumeSliceSpinBox->setValue(dpna->getDistanceFromVolumeSliceTolerance());
    
    m_fileSelectionComboBox->updateComboBox(dpna->getFileSelectionModel());
    
    std::vector<ComboBoxItemInterface*> colorItems;
    ComboBoxItemInterface* selectedColorItem(NULL);
    NeuroglancerAnnotationFile* neuroAnnFile(getSelectedFile());
    if (neuroAnnFile != NULL) {
        CaretAssert(neuroAnnFile);
        colorItems = neuroAnnFile->getColorPropertiesForComboBox();
        selectedColorItem = neuroAnnFile->getSelectedColorProperty();
    }
    m_colorPropertySelectionComboBox->updateComboBox(colorItems);
    m_colorPropertySelectionComboBox->setCurrentItem(selectedColorItem);
    
    updateLabelWidget();
    
    updateTransformationWidget();
    
    setEnabled(neuroAnnFile != NULL);
}

/**
 * @return The selected neuroglancer annotation file or NULL if no file selected
 */
NeuroglancerAnnotationFile*
NeuroglancerAnnotationViewController::getSelectedFile()
{
    NeuroglancerAnnotationFile* selectedFile(NULL);
    CaretDataFile* selFile(m_fileSelectionComboBox->getSelectedFile());
    if (selFile != NULL) {
        selectedFile = selFile->castToNeuroglancerAnnotationFile();
    }

    return selectedFile;
}


/**
 * Update other view controllers.
 */
void 
NeuroglancerAnnotationViewController::updateOtherViewControllers()
{
    for (std::set<NeuroglancerAnnotationViewController*>::iterator iter = NeuroglancerAnnotationViewController::allNeuroglancerAnnotationViewControllers.begin();
         iter != NeuroglancerAnnotationViewController::allNeuroglancerAnnotationViewControllers.end();
         iter++) {
        NeuroglancerAnnotationViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateViewController();
        }
    }
}

/**
 * Issue update events after selections are changed.
 */
void 
NeuroglancerAnnotationViewController::processSelectionChanges()
{
    updateOtherViewControllers();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when a color property is selected
 * @param item
 *     Color item that was selected
 */
void
NeuroglancerAnnotationViewController::colorPropertySelected(ComboBoxItemInterface* item)
{
    NeuroglancerAnnotationFile* selectedFile(getSelectedFile());
    if (selectedFile != NULL) {
        NeuroglancerPropertyInfo* propInfo(NULL);
        if (item != NULL) {
            propInfo = dynamic_cast<NeuroglancerPropertyInfo*>(item);
        }
        selectedFile->setSelectedColorProperty(propInfo);
    }
    
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
NeuroglancerAnnotationViewController::receiveEvent(Event* event)
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
        updateViewController();
    }
}

/**
 * Called when a label model is selected
 * @parain index
 *    Index of item selected
 */
void
NeuroglancerAnnotationViewController::labelModelComboBoxActivated(int /*index*/)
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
NeuroglancerAnnotationViewController::updateLabelWidget()
{
    FeatureLabelModel* previousSelectedLabelModel(getSelectedLabelModel());
    m_labelModelSelectionComboBox->clear();
    
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesNeuroglancerAnnotation* dpna(brain->getDisplayPropertiesNeuroglancerAnnotation());
    NeuroglancerAnnotationFile* neuroAnnFile(dpna->getSelectedFile());
    if (neuroAnnFile != NULL) {
        int32_t selectedIndex(-1);
        std::vector<FeatureLabelModel*> labelModels(neuroAnnFile->getLabelModels());
        const int32_t numLabelModels(labelModels.size());
        for (int32_t i = 0; i < numLabelModels; i++) {
            CaretAssertVectorIndex(labelModels, i);
            FeatureLabelModel* labelModel(labelModels[i]);
            CaretAssert(labelModel);
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
NeuroglancerAnnotationViewController::getSelectedLabelModel()
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
NeuroglancerAnnotationViewController::labelTableViewItemClicked(const QModelIndex& index)
{
    FeatureLabelModel* labelModel(getSelectedLabelModel());
    if (labelModel != NULL) {
        QStandardItem* standardItem(labelModel->itemFromIndex(index));
        if (standardItem != NULL) {
            FeatureLabel* fetatureLabel(dynamic_cast<FeatureLabel*>(standardItem));
            CaretAssert(fetatureLabel);
            
            NeuroglancerAnnotationFile* neuroAnnFile(getSelectedFile());
            CaretAssert(neuroAnnFile);
            neuroAnnFile->updateDisplayedAnnotations();
        }
    }
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when labels all on/off button clicked
 * @param onFlag
 *   True if on clicked, false if off clicked
 */
void
NeuroglancerAnnotationViewController::labelsAllOnOffButtonClicked(const bool onFlag)
{
    FeatureLabelModel* labelModel(getSelectedLabelModel());
    if (labelModel != NULL) {
        labelModel->setAllLabelsDisplayed(onFlag);
        
        NeuroglancerAnnotationFile* neuroAnnFile(getSelectedFile());
        CaretAssert(neuroAnnFile);
        neuroAnnFile->updateDisplayedAnnotations();
    }
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * @return New instance of transformation widget
 */
QWidget*
NeuroglancerAnnotationViewController::createTransformationWidget()
{
    QLabel* matrixLabel(new QLabel("IJK to XYZ Transformation Matrix"));
    QGridLayout* matrixLayout(new QGridLayout());
    for (int32_t iRow = 0; iRow < 4; iRow++) {
        for (int32_t jCol = 0; jCol < 4; jCol++) {
            QDoubleSpinBox* spinBox = new QDoubleSpinBox();
            spinBox->setRange(-999999.0, 999999.0);
            spinBox->setDecimals(2);
            spinBox->setSingleStep(0.1);
            QObject::connect(spinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                             [=](double value) {
                transformationSpinBoxValueChanged(iRow, jCol, value);
            } );
            matrixLayout->addWidget(spinBox, iRow, jCol);
            
            m_matrixSpinBoxes[iRow][jCol] = spinBox;
        }
    }
    
    m_nXpZpYOrientationCheckBox = new QCheckBox("Orientation is Neg X, Pos Z, Pos Y");
    m_nXpZpYOrientationCheckBox->setToolTip("Orientation: \n"
                                            "Left: Negative X\n"
                                            "Anterior: Positive Z\n"
                                            "Inferior: Positive Y");
    QObject::connect(m_nXpZpYOrientationCheckBox, &QCheckBox::clicked,
                     this, &NeuroglancerAnnotationViewController::nXpZpYOrientationCheckBoxClicked);
    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout( new QVBoxLayout(widget));
    layout->addWidget(matrixLabel);
    layout->addLayout(matrixLayout);
    layout->addWidget(m_nXpZpYOrientationCheckBox);
    layout->addStretch();
    
    return widget;
}

/**
 * Update the transformation widget
 */
void
NeuroglancerAnnotationViewController::updateTransformationWidget()
{
    NeuroglancerAnnotationFile* neuroAnnFile(getSelectedFile());
    if (neuroAnnFile != NULL) {
        const Matrix4x4 m(neuroAnnFile->getIjkToXyzTransformationMatrix());
        for (int32_t iRow = 0; iRow < 4; iRow++) {
            for (int32_t jCol = 0; jCol < 4; jCol++) {
                QDoubleSpinBox* spinBox(m_matrixSpinBoxes[iRow][jCol]);
                CaretAssert(spinBox);
                QSignalBlocker blocker(spinBox);
                const float value(m.getMatrixElement(iRow, jCol));
                spinBox->setValue(value);
            }
        }
        m_nXpZpYOrientationCheckBox->setChecked(neuroAnnFile->isLeftNegxPosteriorPoszInferiorPosYOrientation());
    }
}

/**
 * Called when a transformation matrix value is changed
 * @param row
 *    Row of element changed
 * @param column
 *    Column of element changed
 * @param value
 *    Value of element changed
 */
void
NeuroglancerAnnotationViewController::transformationSpinBoxValueChanged(const int32_t row,
                                                                        const int32_t column,
                                                                        const double value)
{
    NeuroglancerAnnotationFile* neuroAnnFile(getSelectedFile());
    if (neuroAnnFile != NULL) {
        neuroAnnFile->setIjkToXyzTransformationMatrixComponent(row,
                                                               column,
                                                               value);
        
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

/**
 * Called if orientation checkbox is checked
 * @param checked
 *    New checked status
 */
void
NeuroglancerAnnotationViewController::nXpZpYOrientationCheckBoxClicked(bool checked)
{
    NeuroglancerAnnotationFile* neuroAnnFile(getSelectedFile());
    if (neuroAnnFile != NULL) {
        neuroAnnFile->setLeftNegxPosteriorPoszInferiorPosYOrientation(checked);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
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
NeuroglancerAnnotationViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "NeuroglancerAnnotationViewController",
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
NeuroglancerAnnotationViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
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


