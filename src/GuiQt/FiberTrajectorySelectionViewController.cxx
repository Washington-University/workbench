
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

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "ConnectivityLoaderFile.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesFiberTrajectory.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "CiftiFiberTrajectoryFile.h"
#include "FiberTrajectorySelectionViewController.h"
#include "GuiManager.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::FiberTrajectorySelectionViewController 
 * \brief View/Controller for fiber trajectories
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
    WuQtUtilities::setLayoutMargins(groupLayout, 2, 2);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    
    //QTabWidget* tabWidget = new QTabWidget();
    WuQTabWidget* tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    tabWidget->addTab(attributesWidget,
                      "Attributes");
    tabWidget->addTab(selectionWidget,
                      "Files");
    tabWidget->setCurrentWidget(attributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 2, 2);
    layout->addLayout(groupLayout);
    layout->addWidget(tabWidget->getWidget(), 0, Qt::AlignLeft);
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
    
    m_displayFibersCheckBox = new QCheckBox("Display Trajectories");
    m_displayFibersCheckBox->setToolTip("Display Trajectories");
    QObject::connect(m_displayFibersCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* thresholdProportionLabel = new QLabel("Threshold Proportion");
    m_thresholdProportionSpinBox = new QDoubleSpinBox();
    m_thresholdProportionSpinBox->setRange(0.0, 1.0);
    m_thresholdProportionSpinBox->setDecimals(2);
    m_thresholdProportionSpinBox->setSingleStep(0.1);
    m_thresholdProportionSpinBox->setToolTip("A streamline along an axis (X,Y,Z) is displayed only\n"
                                             "if the proportion (range 0.0 to 1.0) of streamlines in the\n"
                                             "axis are greater than or equal to this value.  The \n"
                                             "proportions of all of the axes sum to one.");
    QObject::connect(m_thresholdProportionSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* thresholdStreamlineLabel = new QLabel("Threshold Streamline");
    m_thresholdStreamlineSpinBox = new QDoubleSpinBox();
    m_thresholdStreamlineSpinBox->setRange(0.0, std::numeric_limits<float>::max());
    m_thresholdStreamlineSpinBox->setDecimals(2);
    m_thresholdStreamlineSpinBox->setSingleStep(1.0);
    m_thresholdStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                             "streamlines is greater than or equal to this value.");
    QObject::connect(m_thresholdStreamlineSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* minimumProportionLabel = new QLabel("Minimum Proportion");
    m_minimumProportionSpinBox = new QDoubleSpinBox();
    m_minimumProportionSpinBox->setRange(0.0, 1.0);
    m_minimumProportionSpinBox->setDecimals(2);
    m_minimumProportionSpinBox->setSingleStep(0.1);
    m_minimumProportionSpinBox->setToolTip("If the proporation for an axis is less than or equal\n"
                                           "to this value, the opacity will be zero (clear)");
    QObject::connect(m_minimumProportionSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QLabel* maximumProportionLabel = new QLabel("Maximum Proportion");
    m_maximumProportionSpinBox = new QDoubleSpinBox();
    m_maximumProportionSpinBox->setRange(0.0, 1.0);
    m_maximumProportionSpinBox->setDecimals(2);
    m_maximumProportionSpinBox->setSingleStep(0.1);
    m_maximumProportionSpinBox->setToolTip("If the proportion for an axis is greater than or equal\n"
                                           "to this value, the opacity will be one (opaque)");
    QObject::connect(m_maximumProportionSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 100);
    WuQtUtilities::setLayoutMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_displayFibersCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(maximumProportionLabel, row, 0);
    gridLayout->addWidget(m_maximumProportionSpinBox , row, 1);
    row++;
    gridLayout->addWidget(minimumProportionLabel, row, 0);
    gridLayout->addWidget(m_minimumProportionSpinBox , row, 1);
    row++;
    gridLayout->addWidget(thresholdProportionLabel, row, 0);
    gridLayout->addWidget(m_thresholdProportionSpinBox, row, 1);
    row++;
    gridLayout->addWidget(thresholdStreamlineLabel, row, 0);
    gridLayout->addWidget(m_thresholdStreamlineSpinBox, row, 1);
    row++;
    
    gridWidget->setSizePolicy(QSizePolicy::Fixed,
                              QSizePolicy::Fixed);

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(layout, 2, 2);
    layout->addWidget(gridWidget);
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
    
    dpfo->setDisplayed(displayGroup,
                       browserTabIndex,
                       m_displayFibersCheckBox->isChecked());
    
    dpfo->setThresholdProportion(displayGroup,
                       browserTabIndex,
                       m_thresholdProportionSpinBox->value());
    
    dpfo->setThresholdStreamline(displayGroup,
                        browserTabIndex,
                        m_thresholdStreamlineSpinBox->value());
    
    dpfo->setMinimumProportionOpacity(displayGroup,
                        browserTabIndex,
                        m_minimumProportionSpinBox->value());
    
    dpfo->setMaximumProportionOpacity(displayGroup,
                        browserTabIndex,
                        m_maximumProportionSpinBox->value());
    
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
    
    //dpfo->isDisplayed(displayGroup, browserTabIndex)
    
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
    m_displayFibersCheckBox->setChecked(dpfo->isDisplayed(displayGroup,
                                                        browserTabIndex));
    m_thresholdProportionSpinBox->setValue(dpfo->getThresholdProportion(displayGroup,
                                                      browserTabIndex));
    m_thresholdStreamlineSpinBox->setValue(dpfo->getThresholdStreamline(displayGroup,
                                                      browserTabIndex));
    m_maximumProportionSpinBox->setValue(dpfo->getMaximumProportionOpacity(displayGroup,
                                                      browserTabIndex));
    m_minimumProportionSpinBox->setValue(dpfo->getMinimumProportionOpacity(displayGroup,
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
    //dpfo->setDisplayed(displayGroup, browserTabIndex, m_fileSelectionCheckBoxes[iff]->isChecked());
    
    const int32_t numberOfFileCheckBoxes = static_cast<int32_t>(m_fileSelectionCheckBoxes.size());
    const int32_t numberOfFiberTrajFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    CaretAssert(numberOfFiberTrajFiles <= numberOfFileCheckBoxes);
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


