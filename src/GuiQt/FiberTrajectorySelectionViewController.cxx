
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __FIBER_ORIENTATION_TRAJECTORY_VIEW_CONTROLLER_DECLARE__
#include "FiberTrajectorySelectionViewController.h"
#undef __FIBER_ORIENTATION_TRAJECTORY_VIEW_CONTROLLER_DECLARE__

#include <limits>

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CiftiFiberTrajectoryFile.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesFiberTrajectory.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberTrajectorySelectionViewController.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "WuQFactory.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::FiberTrajectorySelectionViewController 
 * \brief View/Controller for fiber trajectories
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param browserWindowIndex
 *   Index of browser window in which this view controller is displayed.
 * @param parent
 *   Parent of this object.
 */
FiberTrajectorySelectionViewController::FiberTrajectorySelectionViewController(const int32_t browserWindowIndex,
                                                                                 QWidget* parent)
: QWidget(parent),
  m_browserWindowIndex(browserWindowIndex)
{
    QLabel* groupLabel = new QLabel("Group");
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_displayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(displayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(groupLayout, 2, 2);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    m_displayTrajectoriesCheckBox = new QCheckBox("Display Trajectories");
    m_displayTrajectoriesCheckBox->setToolTip("Display Trajectories");
    QObject::connect(m_displayTrajectoriesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processSelectionChanges()));
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    
    m_tabWidget->addTab(attributesWidget,
                      "Attributes");
    m_tabWidget->addTab(selectionWidget,
                      "Selection");
    m_tabWidget->setCurrentWidget(attributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addLayout(groupLayout);
    layout->addSpacing(10);
    layout->addWidget(m_displayTrajectoriesCheckBox);
    layout->addWidget(m_tabWidget->getWidget(), 0, Qt::AlignLeft);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    s_allViewControllers.insert(this);
}

/**
 * Destructor.
 */
FiberTrajectorySelectionViewController::~FiberTrajectorySelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    s_allViewControllers.erase(this);
}

/**
 * @return The selection widget.
 */
QWidget*
FiberTrajectorySelectionViewController::createSelectionWidget()
{
    m_selectionWidgetLayout = new QVBoxLayout();
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(m_selectionWidgetLayout);
    layout->addStretch();
    return widget;
}

/**
 * @return The attributes widget.
 */
QWidget*
FiberTrajectorySelectionViewController::createAttributesWidget()
{
    m_updateInProgress = true;
    const int spinBoxWidth = 65;
    
    m_displayModeButtonGroup = new QButtonGroup(this);
    QObject::connect(m_displayModeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(processAttributesChanges()));
    
    std::vector<FiberTrajectoryDisplayModeEnum::Enum> displayModes;
    FiberTrajectoryDisplayModeEnum::getAllEnums(displayModes);
    const int32_t numDisplayModes = static_cast<int32_t>(displayModes.size());
    for (int32_t i = 0; i < numDisplayModes; i++) {
        const FiberTrajectoryDisplayModeEnum::Enum mode = displayModes[i];
        QRadioButton* radioButton = new QRadioButton(FiberTrajectoryDisplayModeEnum::toGuiName(mode));
        m_displayModeButtonGroup->addButton(radioButton, i);
        m_displayModeRadioButtons.push_back(radioButton);
        m_displayModeRadioButtonData.push_back(mode);
    }
    
    QGroupBox* modeGroupBox = new QGroupBox("Display Mode");
    QVBoxLayout* modeGroupLayout = new QVBoxLayout(modeGroupBox);
    for (int32_t i = 0; i < numDisplayModes; i++) {
        modeGroupLayout->addWidget(m_displayModeRadioButtons[i]);
    }
    
    m_proportionStreamlineSpinBox = WuQFactory::newSpinBox();
    m_proportionStreamlineSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_proportionStreamlineSpinBox->setSingleStep(5);
    m_proportionStreamlineSpinBox->setFixedWidth(spinBoxWidth);
    m_proportionStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                             "streamlines is greater than or equal to this value");
    QObject::connect(m_proportionStreamlineSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    m_proportionMinimumSpinBox = WuQFactory::newDoubleSpinBox();
    m_proportionMinimumSpinBox->setRange(0.0, 1.0);
    m_proportionMinimumSpinBox->setDecimals(2);
    m_proportionMinimumSpinBox->setSingleStep(0.05);
    m_proportionMinimumSpinBox->setFixedWidth(spinBoxWidth);
    m_proportionMinimumSpinBox->setToolTip("If the proportion for an axis is less than or equal\n"
                                           "to this value, the opacity will be zero (clear)");
    QObject::connect(m_proportionMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_proportionMaximumSpinBox = WuQFactory::newDoubleSpinBox();
    m_proportionMaximumSpinBox->setRange(0.0, 1.0);
    m_proportionMaximumSpinBox->setDecimals(2);
    m_proportionMaximumSpinBox->setSingleStep(0.05);
    m_proportionMaximumSpinBox->setFixedWidth(spinBoxWidth);
    m_proportionMaximumSpinBox->setToolTip("If the proportion for an axis is greater than or equal\n"
                                           "to this value, the opacity will be one (opaque)");
    QObject::connect(m_proportionMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    
    m_countStreamlineSpinBox = WuQFactory::newSpinBox();
    m_countStreamlineSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_countStreamlineSpinBox->setSingleStep(5);
    m_countStreamlineSpinBox->setFixedWidth(spinBoxWidth);
    m_countStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                              "streamlines is greater than or equal to this value");
    QObject::connect(m_countStreamlineSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    m_countMinimumSpinBox = WuQFactory::newSpinBox();
    m_countMinimumSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_countMinimumSpinBox->setSingleStep(5);
    m_countMinimumSpinBox->setFixedWidth(spinBoxWidth);
    m_countMinimumSpinBox->setToolTip("If the number of fibers for an axis is less than or equal\n"
                                           "to this value, the opacity will be zero (clear)");
    QObject::connect(m_countMinimumSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    m_countMaximumSpinBox = WuQFactory::newSpinBox();
    m_countMaximumSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_countMaximumSpinBox->setSingleStep(5);
    m_countMaximumSpinBox->setFixedWidth(spinBoxWidth);
    m_countMaximumSpinBox->setToolTip("If the number of fibers for an axis is greater than or equal\n"
                                           "to this value, the opacity will be one (opaque)");
    QObject::connect(m_countMaximumSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    
    
    m_distanceStreamlineSpinBox = WuQFactory::newSpinBox();
    m_distanceStreamlineSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_distanceStreamlineSpinBox->setSingleStep(5);
    m_distanceStreamlineSpinBox->setFixedWidth(spinBoxWidth);
    m_distanceStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                         "streamlines is greater than or equal to this value");
    QObject::connect(m_distanceStreamlineSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    m_distanceMinimumSpinBox = WuQFactory::newSpinBox();
    m_distanceMinimumSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_distanceMinimumSpinBox->setSingleStep(5);
    m_distanceMinimumSpinBox->setFixedWidth(spinBoxWidth);
    m_distanceMinimumSpinBox->setToolTip("If count times distance for an axis is less than or equal\n"
                                      "to this value, the opacity will be zero (clear)");
    QObject::connect(m_distanceMinimumSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    m_distanceMaximumSpinBox = WuQFactory::newSpinBox();
    m_distanceMaximumSpinBox->setRange(0, std::numeric_limits<int32_t>::max());
    m_distanceMaximumSpinBox->setSingleStep(5);
    m_distanceMaximumSpinBox->setFixedWidth(spinBoxWidth);
    m_distanceMaximumSpinBox->setToolTip("If the count times distance for an axis is greater than or equal\n"
                                      "to this value, the opacity will be one (opaque)");
    QObject::connect(m_distanceMaximumSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(processAttributesChanges()));
    
    QGroupBox* dataMappingGroupBox = new QGroupBox("Data Mapping");
    QGridLayout* dataMappingLayout = new QGridLayout(dataMappingGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(dataMappingLayout, 4, 2);
    int row = dataMappingLayout->rowCount();
    
    int columnCounter = 0;
    const int COLUMN_LABELS    = columnCounter++;
    const int COLUMN_THRESHOLD = columnCounter++;
    const int COLUMN_MINIMUM   = columnCounter++;
    const int COLUMN_MAXIMUM   = columnCounter++;
    
    dataMappingLayout->addWidget(new QLabel("<html>Display<br>Mode<html>"), row, COLUMN_LABELS, Qt::AlignLeft);
    dataMappingLayout->addWidget(new QLabel("<html>Streamline<br>Threshold<html>"), row, COLUMN_THRESHOLD, Qt::AlignLeft);
    dataMappingLayout->addWidget(new QLabel("<html>Map to<br>Clear<html>"), row, COLUMN_MINIMUM, Qt::AlignLeft);
    dataMappingLayout->addWidget(new QLabel("<html>Map to<br>Opaque<html>"), row, COLUMN_MAXIMUM, Qt::AlignLeft);
    row++;
    
    dataMappingLayout->addWidget(new QLabel("Absolute"), row, COLUMN_LABELS);
    dataMappingLayout->addWidget(m_countStreamlineSpinBox, row, COLUMN_THRESHOLD, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_countMinimumSpinBox, row, COLUMN_MINIMUM, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_countMaximumSpinBox, row, COLUMN_MAXIMUM, Qt::AlignHCenter);
    row++;

    dataMappingLayout->addWidget(new QLabel("Distance"), row, COLUMN_LABELS);
    dataMappingLayout->addWidget(m_distanceStreamlineSpinBox, row, COLUMN_THRESHOLD, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_distanceMinimumSpinBox, row, COLUMN_MINIMUM, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_distanceMaximumSpinBox, row, COLUMN_MAXIMUM, Qt::AlignHCenter);
    row++;

    dataMappingLayout->addWidget(new QLabel("Proportion"), row, COLUMN_LABELS);
    dataMappingLayout->addWidget(m_proportionStreamlineSpinBox, row, COLUMN_THRESHOLD, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_proportionMinimumSpinBox, row, COLUMN_MINIMUM, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_proportionMaximumSpinBox, row, COLUMN_MAXIMUM, Qt::AlignHCenter);
    row++;
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(modeGroupBox);
    layout->addWidget(dataMappingGroupBox);
    layout->addStretch();
    
    m_updateInProgress = false;
    
    return widget;
}

/**
 * Called when a widget on the attributes page has
 * its value changed.
 */
void
FiberTrajectorySelectionViewController::processAttributesChanges()
{
    if (m_updateInProgress) {
        return;
    }
    
    DisplayPropertiesFiberTrajectory* dpfo = GuiManager::get()->getBrain()->getDisplayPropertiesFiberTrajectory();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(browserTabIndex);
    
    const int32_t selectedModeRadioButtonIndex = m_displayModeButtonGroup->checkedId();
    const FiberTrajectoryDisplayModeEnum::Enum displayMode = m_displayModeRadioButtonData[selectedModeRadioButtonIndex];
    dpfo->setDisplayMode(displayGroup,
                         browserTabIndex,
                         displayMode);
    
    dpfo->setProportionStreamline(displayGroup,
                        browserTabIndex,
                                  m_proportionStreamlineSpinBox->value());;
    
    dpfo->setProportionMinimumOpacity(displayGroup,
                        browserTabIndex,
                        m_proportionMinimumSpinBox->value());
    
    dpfo->setProportionMaximumOpacity(displayGroup,
                        browserTabIndex,
                        m_proportionMaximumSpinBox->value());
    
    dpfo->setCountStreamline(displayGroup,
                             browserTabIndex,
                             m_countStreamlineSpinBox->value());
    dpfo->setCountMaximumOpacity(displayGroup,
                                 browserTabIndex,
                                 m_countMaximumSpinBox->value());
    dpfo->setCountMinimumOpacity(displayGroup,
                                 browserTabIndex,
                                 m_countMinimumSpinBox->value());
    
    dpfo->setDistanceStreamline(displayGroup,
                                browserTabIndex,
                                m_distanceStreamlineSpinBox->value());
    dpfo->setDistanceMaximumOpacity(displayGroup,
                                    browserTabIndex,
                                    m_distanceMaximumSpinBox->value());
    dpfo->setDistanceMinimumOpacity(displayGroup,
                                    browserTabIndex,
                                    m_distanceMinimumSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    updateOtherViewControllers();
}

/**
 * Called when the fiber trajectory display group combo box is changed.
 */
void
FiberTrajectorySelectionViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    if (m_updateInProgress) {
        return;
    }
    
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
    DisplayPropertiesFiberTrajectory* dpfo = brain->getDisplayPropertiesFiberTrajectory();
    dpfo->setDisplayGroupForTab(browserTabIndex,
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
 * Update the fiber trajectory widget.
 */
void
FiberTrajectorySelectionViewController::updateViewController()
{
    m_updateInProgress = true;
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesFiberTrajectory* dpfo = brain->getDisplayPropertiesFiberTrajectory();

    const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(browserTabIndex);
    m_displayGroupComboBox->setSelectedDisplayGroup(displayGroup);
    
    const FiberTrajectoryDisplayModeEnum::Enum selectedDisplayMode = dpfo->getDisplayMode(displayGroup, browserTabIndex);
    const int32_t numDisplayModeRadioButtons = m_displayModeButtonGroup->buttons().size();
    for (int32_t i = 0; i < numDisplayModeRadioButtons; i++) {
        if (m_displayModeRadioButtonData[i] == selectedDisplayMode) {
            m_displayModeRadioButtons[i]->setChecked(true);
            break;
        }
    }
    
    /*
     * Update file selection checkboxes
     */
    const int32_t numberOfFileCheckBoxes = static_cast<int32_t>(m_fileSelectionCheckBoxes.size());
    const int32_t numberOfFiberTrajFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    for (int32_t iff = 0; iff < numberOfFiberTrajFiles; iff++) {
        CiftiFiberTrajectoryFile* cftf = brain->getConnectivityFiberTrajectoryFile(iff);
        QCheckBox* cb = NULL;
        if (iff < numberOfFileCheckBoxes) {
            cb = m_fileSelectionCheckBoxes[iff];
        }
        else {
            cb = new QCheckBox("");
            QObject::connect(cb, SIGNAL(clicked(bool)),
                             this, SLOT(processSelectionChanges()));
            m_fileSelectionCheckBoxes.push_back(cb);
            m_selectionWidgetLayout->addWidget(cb);
        }
        cb->setText(cftf->getFileNameNoPath());
        cb->setChecked(cftf->isDisplayed(displayGroup,
                                         browserTabIndex));
        cb->setVisible(true);
    }
    
    /*
     * Hide unused file selection checkboxes
     */
    for (int32_t iff = numberOfFiberTrajFiles; iff < numberOfFileCheckBoxes; iff++) {
        m_fileSelectionCheckBoxes[iff]->setVisible(false);
    }
    
    
    /*
     * Update the attributes
     */
    m_displayTrajectoriesCheckBox->setChecked(dpfo->isDisplayed(displayGroup,
                                                        browserTabIndex));
    m_proportionStreamlineSpinBox->setValue(dpfo->getProportionStreamline(displayGroup,
                                                      browserTabIndex));
    m_proportionMaximumSpinBox->setValue(dpfo->getProportionMaximumOpacity(displayGroup,
                                                      browserTabIndex));
    m_proportionMinimumSpinBox->setValue(dpfo->getProportionMinimumOpacity(displayGroup,
                                                            browserTabIndex));
    
    m_countStreamlineSpinBox->setValue(dpfo->getCountStreamline(displayGroup,
                                                                browserTabIndex));
    m_countMaximumSpinBox->setValue(dpfo->getCountMaximumOpacity(displayGroup,
                                                                 browserTabIndex));
    m_countMinimumSpinBox->setValue(dpfo->getCountMinimumOpacity(displayGroup,
                                                                 browserTabIndex));
    
    m_distanceStreamlineSpinBox->setValue(dpfo->getDistanceStreamline(displayGroup,
                                                                      browserTabIndex));
    m_distanceMaximumSpinBox->setValue(dpfo->getDistanceMaximumOpacity(displayGroup,
                                                                       browserTabIndex));
    m_distanceMinimumSpinBox->setValue(dpfo->getDistanceMinimumOpacity(displayGroup,
                                                                       browserTabIndex));
    m_updateInProgress = false;
}

/**
 * Issue update events after selections are changed.
 */
void
FiberTrajectorySelectionViewController::processSelectionChanges()
{
    if (m_updateInProgress) {
        return;
    }

    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    DisplayPropertiesFiberTrajectory* dpfo = brain->getDisplayPropertiesFiberTrajectory();
    
    const DisplayGroupEnum::Enum displayGroup = dpfo->getDisplayGroupForTab(browserTabIndex);
    
    dpfo->setDisplayed(displayGroup,
                       browserTabIndex,
                       m_displayTrajectoriesCheckBox->isChecked());
    
    
    const int32_t numberOfFiberTrajFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    CaretAssert(numberOfFiberTrajFiles <= static_cast<int32_t>(m_fileSelectionCheckBoxes.size()));
    for (int32_t iff = 0; iff < numberOfFiberTrajFiles; iff++) {
        CiftiFiberTrajectoryFile* cftf = brain->getConnectivityFiberTrajectoryFile(iff);
            cftf->setDisplayed(displayGroup,
                               browserTabIndex,
                               m_fileSelectionCheckBoxes[iff]->isChecked());
    }
    
    updateOtherViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update other fiber orientation view controllers.
 */
void
FiberTrajectorySelectionViewController::updateOtherViewControllers()
{
    for (std::set<FiberTrajectorySelectionViewController*>::iterator iter = s_allViewControllers.begin();
         iter != s_allViewControllers.end();
         iter++) {
        FiberTrajectorySelectionViewController* fosvc = *iter;
        if (fosvc != this) {
            fosvc->updateViewController();
        }
    }
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
FiberTrajectorySelectionViewController::receiveEvent(Event* event)
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
FiberTrajectorySelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "FiberTrajectorySelectionViewController",
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
FiberTrajectorySelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
}



