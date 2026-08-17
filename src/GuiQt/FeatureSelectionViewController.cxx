
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
#include <QTableView>
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
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    FeatureSelectionViewController::allFeatureSelectionViewControllers.insert(this);
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
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setText("All On");
    QObject::connect(allOnToolButton, &QToolButton::clicked,
                     [=]() { featureItemsAllOnOffButtonClicked(true); });
    
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setText("All Off");
    QObject::connect(allOffToolButton, &QToolButton::clicked,
                     [=]() { featureItemsAllOnOffButtonClicked(false); });
    
    QHBoxLayout* allOnOffLayout(new QHBoxLayout());
    allOnOffLayout->setContentsMargins(0, 0, 0, 0);
    allOnOffLayout->addWidget(allOnToolButton);
    allOnOffLayout->addWidget(allOffToolButton);
    allOnOffLayout->addStretch();

    m_featureItemTableView = new QTableView();
    QObject::connect(m_featureItemTableView, &QTableView::clicked,
                     this, &FeatureSelectionViewController::featureItemTableViewItemClicked);
    
    const int BIG_STRETCH(100);
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(allOnOffLayout);
    layout->addWidget(m_featureItemTableView, BIG_STRETCH);
    
    return widget;
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
    allOnToolButton->setText("All On");
    QObject::connect(allOnToolButton, &QToolButton::clicked,
                     [=]() { labelsAllOnOffButtonClicked(true); });
    
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setText("All Off");
    QObject::connect(allOffToolButton, &QToolButton::clicked,
                     [=]() { labelsAllOnOffButtonClicked(false); });
    
    QHBoxLayout* allOnOffLayout(new QHBoxLayout());
    allOnOffLayout->setContentsMargins(0, 0, 0, 0);
    allOnOffLayout->addWidget(allOnToolButton);
    allOnOffLayout->addWidget(allOffToolButton);
    allOnOffLayout->addStretch();
    m_labelsTableView = new QTableView();
    m_labelsTableView->horizontalHeader()->setVisible(false);
    m_labelsTableView->verticalHeader()->setVisible(false);
    QObject::connect(m_labelsTableView, &QTableView::clicked,
                     this, &FeatureSelectionViewController::labelTableViewItemClicked);

    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(fileLayout);
    layout->addLayout(allOnOffLayout);
    layout->addWidget(m_labelsTableView, BIG_STRETCH);
    
    return widget;
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
    QWidget* widget = new QWidget();
    
    QGridLayout* layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setRowStretch(1000, 100);
    layout->addWidget(symbolScaleLabel, 0, 0);
    layout->addWidget(m_symbolScaleSpinBox, 0, 1);
    
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
    DisplayPropertiesFeature* dpna = brain->getDisplayPropertiesFeature();
    m_featureFileSelectionComboBox->updateComboBox(dpna->getFeatureFileSelectionModel());
    
    CaretDataFile* cdf(m_featureFileSelectionComboBox->getSelectedFile());
    if (cdf != NULL) {
        FeatureFile* featureFile(dynamic_cast<FeatureFile*>(cdf));
        CaretAssert(featureFile);
        
        m_volumeFileSelectionComboBox->updateComboBox(featureFile->getVolumeFileSelectionModel());
        
        m_featureItemTableView->setModel(featureFile->getFeatureItemModel());
        const int32_t numCols(featureFile->getFeatureItemModel()->columnCount());
        for (int32_t i = 0; i < numCols; i++) {
            m_featureItemTableView->resizeColumnToContents(i);
        }
    }
    else {
        m_featureItemTableView->setModel(NULL);
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
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesFeature* dpna = brain->getDisplayPropertiesFeature();
    FeatureItemModel* annModel(dpna->getSelectedFeatureItemModel());
    if (annModel != NULL) {
        annModel->setAllFeaturesDisplayed(onFlag);
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
 * Called when user clicks on an item in the feature items table view
 */
void
FeatureSelectionViewController::featureItemTableViewItemClicked(const QModelIndex& /*index*/)
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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
}


