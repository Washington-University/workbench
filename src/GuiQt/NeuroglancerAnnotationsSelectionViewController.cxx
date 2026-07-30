
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

#define __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "NeuroglancerAnnotationsSelectionViewController.h"
#undef __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionComboBox.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesNeuroglancerAnnotations.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "NeuroglancerAnnotationsFile.h"
#include "NeuroglancerAnnotationLabelModel.h"
#include "SceneClass.h"
#include "WuQMacroManager.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotationsSelectionViewController 
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
NeuroglancerAnnotationsSelectionViewController::NeuroglancerAnnotationsSelectionViewController(const int32_t browserWindowIndex,
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
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->setContentsMargins(0, 0, 0, 0);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    QLabel* fileLabel(new QLabel("File"));
    m_neuroAnnFileSelectionComboBox = new CaretDataFileSelectionComboBox(this);
    QObject::connect(m_neuroAnnFileSelectionComboBox, &CaretDataFileSelectionComboBox::fileSelected,
                     [=]() { this->updateAnnotationWidget(); });
    QHBoxLayout* fileLayout(new QHBoxLayout());
    fileLayout->setContentsMargins(0, 0, 0, 0);
    fileLayout->addWidget(fileLabel);
    const int BIG_STRETCH(100);
    fileLayout->addWidget(m_neuroAnnFileSelectionComboBox->getWidget(), BIG_STRETCH);

    m_displayCheckBox = new QCheckBox("Display Neuroglancer Annotations");
    m_displayCheckBox->setToolTip("Enable the display of neuroglancer annotations");
    QObject::connect(m_displayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_displayCheckBox->setObjectName(m_objectNamePrefix
                                            + ":DisplayNeuroglancerAnnotations");
    WuQMacroManager::instance()->addMacroSupportToObject(m_displayCheckBox,
                                                         "Enable neuroglancer annotation display");
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* annotationWidget = this->createAnnotationWidget();
    QWidget* labelsWidget     = this->createLabelsWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    m_tabWidget->getWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    m_tabWidget->addTab(annotationWidget,
                      "Annotation");
    m_tabWidget->addTab(attributesWidget,
                        "Attributes");
    m_tabWidget->addTab(labelsWidget,
                        "Labels");
    m_tabWidget->setCurrentWidget(attributesWidget);
    m_tabWidget->getTabBar()->setToolTip("Select neuroglancer annotations tab");
    m_tabWidget->getTabBar()->setObjectName(m_objectNamePrefix
                                            + ":Tab");
    WuQMacroManager::instance()->addMacroSupportToObject(m_tabWidget->getTabBar(),
                                                         "Select features toolbox neuroglancer annotations tab");
    
    this->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_displayCheckBox);
    layout->addLayout(groupLayout);
    layout->addLayout(fileLayout);
    layout->addWidget(m_tabWidget->getWidget(), 100);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    NeuroglancerAnnotationsSelectionViewController::allNeuroglancerAnnotationsSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
NeuroglancerAnnotationsSelectionViewController::~NeuroglancerAnnotationsSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    NeuroglancerAnnotationsSelectionViewController::allNeuroglancerAnnotationsSelectionViewControllers.erase(this);
}

/**
 * @return New instance of annotation selection widget
 */
QWidget* 
NeuroglancerAnnotationsSelectionViewController::createAnnotationWidget()
{
    QToolButton* allOnToolButton(new QToolButton());
    allOnToolButton->setText("All On");
    QObject::connect(allOnToolButton, &QToolButton::clicked,
                     [=]() { annotationsAllOnOffButtonClicked(true); });
    
    QToolButton* allOffToolButton(new QToolButton());
    allOffToolButton->setText("All Off");
    QObject::connect(allOffToolButton, &QToolButton::clicked,
                     [=]() { annotationsAllOnOffButtonClicked(false); });
    
    QHBoxLayout* allOnOffLayout(new QHBoxLayout());
    allOnOffLayout->setContentsMargins(0, 0, 0, 0);
    allOnOffLayout->addWidget(allOnToolButton);
    allOnOffLayout->addWidget(allOffToolButton);
    allOnOffLayout->addStretch();

    m_annotationTableView = new QTableView();
    m_annotationTableView->verticalHeader()->setVisible(false);
    QObject::connect(m_annotationTableView, &QTableView::clicked,
                     this, &NeuroglancerAnnotationsSelectionViewController::annotationTableViewItemClicked);
    
    const int BIG_STRETCH(100);
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(allOnOffLayout);
    layout->addWidget(m_annotationTableView, BIG_STRETCH);
    
    return widget;
}

/**
 * @return New instance of labels selection widget
 */
QWidget*
NeuroglancerAnnotationsSelectionViewController::createLabelsWidget()
{
    const int BIG_STRETCH(100);

    QLabel* fileLabel(new QLabel("Labels"));
    m_labelModelSelectionComboBox = new QComboBox();
    QObject::connect(m_labelModelSelectionComboBox, QOverload<int>::of(&QComboBox::activated),
                     this,&NeuroglancerAnnotationsSelectionViewController::labelModelComboBoxActivated);
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
                     this, &NeuroglancerAnnotationsSelectionViewController::labelTableViewItemClicked);

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
NeuroglancerAnnotationsSelectionViewController::createAttributesWidget()
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();

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
NeuroglancerAnnotationsSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesNeuroglancerAnnotations* dpna = GuiManager::get()->getBrain()->getDisplayPropertiesNeuroglancerAnnotations();
        
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
    
    updateOtherNeuroAnnViewControllers();
}

/**
 * Called when the neuroglancer annotations display group combo box is changed.
 */
void 
NeuroglancerAnnotationsSelectionViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    DisplayPropertiesNeuroglancerAnnotations* dpna = brain->getDisplayPropertiesNeuroglancerAnnotations();
    dpna->setDisplayGroupForTab(browserTabIndex,
                                displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateNeuroAnnViewController();
    
    /*
     * Apply the changes.
     */
    processSelectionChanges();
}

/**
 * Update the neuroglancer annotations widget.
 */
void 
NeuroglancerAnnotationsSelectionViewController::updateNeuroAnnViewController()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesNeuroglancerAnnotations* dpna = brain->getDisplayPropertiesNeuroglancerAnnotations();
    const DisplayGroupEnum::Enum displayGroup = dpna->getDisplayGroupForTab(browserTabIndex);
    
    setWindowTitle("Neuroglancer Annotations");
    
    m_displayGroupComboBox->setSelectedDisplayGroup(dpna->getDisplayGroupForTab(browserTabIndex));
    m_displayCheckBox->setChecked(dpna->isDisplayed(displayGroup, browserTabIndex));
    
    QSignalBlocker symbolSizeBlocker(m_symbolScaleSpinBox);
    m_symbolScaleSpinBox->setValue(dpna->getSymbolScale());
    
    updateAnnotationWidget();
    
    updateLabelWidget();
}

/**
 * Update the annotationt tab
 */
void
NeuroglancerAnnotationsSelectionViewController::updateAnnotationWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesNeuroglancerAnnotations* dpna = brain->getDisplayPropertiesNeuroglancerAnnotations();
    m_neuroAnnFileSelectionComboBox->updateComboBox(dpna->getNeuroglancerAnnotationFileSelectionModel());
    
    CaretDataFile* cdf(m_neuroAnnFileSelectionComboBox->getSelectedFile());
    if (cdf != NULL) {
        NeuroglancerAnnotationsFile* neuroAnnFile(dynamic_cast<NeuroglancerAnnotationsFile*>(cdf));
        CaretAssert(neuroAnnFile);
        
        m_annotationTableView->setModel(neuroAnnFile->getModel());
        const int32_t numCols(neuroAnnFile->getModel()->columnCount());
        for (int32_t i = 0; i < numCols; i++) {
            m_annotationTableView->resizeColumnToContents(i);
        }
    }
    else {
        m_annotationTableView->setModel(NULL);
    }
}

/**
 * Called when annotations all on/off button clicked
 * @param onFlag
 *   True if on clicked, false if off clicked
 */
void
NeuroglancerAnnotationsSelectionViewController::annotationsAllOnOffButtonClicked(const bool onFlag)
{
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesNeuroglancerAnnotations* dpna = brain->getDisplayPropertiesNeuroglancerAnnotations();
    NeuroglancerAnnotationsFile* neuroAnnFile(dpna->getSelectedNeuroglancerAnnotationFile());
    if (neuroAnnFile != NULL) {
        neuroAnnFile->setAllAnnotationsDisplayed(onFlag);
    }
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Update other neuroglancer annotations view controllers.
 */
void 
NeuroglancerAnnotationsSelectionViewController::updateOtherNeuroAnnViewControllers()
{
    for (std::set<NeuroglancerAnnotationsSelectionViewController*>::iterator iter = NeuroglancerAnnotationsSelectionViewController::allNeuroglancerAnnotationsSelectionViewControllers.begin();
         iter != NeuroglancerAnnotationsSelectionViewController::allNeuroglancerAnnotationsSelectionViewControllers.end();
         iter++) {
        NeuroglancerAnnotationsSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateNeuroAnnViewController();
        }
    }
}

/**
 * Issue update events after selections are changed.
 */
void 
NeuroglancerAnnotationsSelectionViewController::processSelectionChanges()
{
    updateOtherNeuroAnnViewControllers();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
NeuroglancerAnnotationsSelectionViewController::receiveEvent(Event* event)
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
        updateNeuroAnnViewController();
    }
}

/**
 * Called when user clicks on an item in the annotation table view
 */
void
NeuroglancerAnnotationsSelectionViewController::annotationTableViewItemClicked(const QModelIndex& /*index*/)
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when a label model is selected
 * @parain index
 *    Index of item selected
 */
void
NeuroglancerAnnotationsSelectionViewController::labelModelComboBoxActivated(int /*index*/)
{
    NeuroglancerAnnotationLabelModel* labelModel(getSelectedLabelModel());
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
NeuroglancerAnnotationsSelectionViewController::updateLabelWidget()
{
    NeuroglancerAnnotationLabelModel* previousSelectedLabelModel(getSelectedLabelModel());
    m_labelModelSelectionComboBox->clear();
    
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesNeuroglancerAnnotations* dpna = brain->getDisplayPropertiesNeuroglancerAnnotations();
    NeuroglancerAnnotationsFile* neuroAnnFile(dpna->getSelectedNeuroglancerAnnotationFile());
    if (neuroAnnFile != NULL) {
        int32_t selectedIndex(-1);
        const int32_t numLabelModels(neuroAnnFile->getNumberOfLabelModels());
        for (int32_t i = 0; i < numLabelModels; i++) {
            NeuroglancerAnnotationLabelModel* labelModel(neuroAnnFile->getLabelModel(i));
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
NeuroglancerAnnotationLabelModel*
NeuroglancerAnnotationsSelectionViewController::getSelectedLabelModel()
{
    NeuroglancerAnnotationLabelModel* labelModel(NULL);
    
    if (m_labelModelSelectionComboBox->count() > 0) {
        const QVariant data(m_labelModelSelectionComboBox->currentData());
        labelModel = data.value<NeuroglancerAnnotationLabelModel*>();
    }
    
    return labelModel;
}


/**
 * Called when user clicks on an item in the label table view
 */
void
NeuroglancerAnnotationsSelectionViewController::labelTableViewItemClicked(const QModelIndex& /*index*/)
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when labels all on/off button clicked
 * @param onFlag
 *   True if on clicked, false if off clicked
 */
void
NeuroglancerAnnotationsSelectionViewController::labelsAllOnOffButtonClicked(const bool onFlag)
{
    NeuroglancerAnnotationLabelModel* labelModel(getSelectedLabelModel());
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
NeuroglancerAnnotationsSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "NeuroglancerAnnotationsSelectionViewController",
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
NeuroglancerAnnotationsSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
}


