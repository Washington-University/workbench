
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

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
//#include <QTabWidget>
#include <QToolButton>

#define __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "LabelSelectionViewController.h"
#undef __LABEL_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesLabels.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "WuQDataEntryDialog.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::LabelSelectionViewController 
 * \brief Widget for controlling display of labels
 * \ingroup GuiQt
 *
 * Widget for controlling the display of labels including
 * different display groups.
 */

/**
 * Constructor.
 */
LabelSelectionViewController::LabelSelectionViewController(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Group");
    m_labelsDisplayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_labelsDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(labelDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_labelsDisplayGroupComboBox->getWidget());
    groupLayout->addStretch(); 
    
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
    layout->addLayout(groupLayout);
    layout->addWidget(m_tabWidget->getWidget(), 0, Qt::AlignLeft);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    LabelSelectionViewController::allLabelSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
LabelSelectionViewController::~LabelSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    LabelSelectionViewController::allLabelSelectionViewControllers.erase(this);
}


QWidget* 
LabelSelectionViewController::createSelectionWidget()
{
    m_labelClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex);
    
    return m_labelClassNameHierarchyViewController;
}

/**
 * @return The attributes widget.
 */
QWidget* 
LabelSelectionViewController::createAttributesWidget()
{
    QLabel* drawAsLabel = new QLabel("Draw as: ");
    m_labelDrawingTypeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(m_labelDrawingTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(processAttributesChanges()));
    m_labelDrawingTypeComboBox->setup<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>();
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    WuQtUtilities::setLayoutMargins(gridLayout, 8, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(drawAsLabel, row, 0);
    gridLayout->addWidget(m_labelDrawingTypeComboBox->getWidget(), row, 1);
    row++;
    gridWidget->setSizePolicy(QSizePolicy::Fixed,
                              QSizePolicy::Fixed);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(gridWidget);
    layout->addStretch();
        
    return widget;
}

/**
 * Called when a widget on the attributes page has 
 * its value changed.
 */
void 
LabelSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesLabels* dpl = GuiManager::get()->getBrain()->getDisplayPropertiesLabels();
    const LabelDrawingTypeEnum::Enum labelDrawingType = m_labelDrawingTypeComboBox->getSelectedItem<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>();


    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpl->getDisplayGroupForTab(browserTabIndex);

    dpl->setDrawingType(displayGroup,
                        browserTabIndex,
                        labelDrawingType);
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    processLabelSelectionChanges();
}

/**
 * Called when the label display group combo box is changed.
 */
void 
LabelSelectionViewController::labelDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    DisplayPropertiesLabels* dsb = brain->getDisplayPropertiesLabels();
    dsb->setDisplayGroupForTab(browserTabIndex,
                         displayGroup);
    
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
LabelSelectionViewController::updateLabelViewController()
{
    setWindowTitle("Labels");
    
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesLabels* dpb = brain->getDisplayPropertiesLabels();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
    
    m_labelsDisplayGroupComboBox->setSelectedDisplayGroup(dpb->getDisplayGroupForTab(browserTabIndex));
    
    m_labelDrawingTypeComboBox->setSelectedItem<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>(dpb->getDrawingType(displayGroup,
                                                                    browserTabIndex));
    /*
     * Get all of label files.
     */
    std::vector<LabelFile*> allLabelFiles;
    const int numBrainStructures = brain->getNumberOfBrainStructures();
    for (int32_t ibs = 0; ibs < numBrainStructures; ibs++) {
        BrainStructure* brainStructure = brain->getBrainStructure(ibs);
        const int32_t numLabelFiles = brainStructure->getNumberOfLabelFiles();
        for (int32_t ilf = 0; ilf < numLabelFiles; ilf++) {
            allLabelFiles.push_back(brainStructure->getLabelFile(ilf));
        }
    }
    
    /*
     * Get all CIFTI label files
     */
    std::vector<CiftiBrainordinateLabelFile*> allCiftiLabelFiles;
    const int32_t numCiftiLabelFiles = brain->getNumberOfConnectivityDenseLabelFiles();
    for (int32_t iclf = 0; iclf < numCiftiLabelFiles; iclf++) {
        allCiftiLabelFiles.push_back(brain->getConnectivityDenseLabelFile(iclf));
    }
    
    /*
     * Update the class/name hierarchy
     */
    m_labelClassNameHierarchyViewController->updateContents(allLabelFiles,
                                                            allCiftiLabelFiles,
                                                             displayGroup);
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
LabelSelectionViewController::updateOtherLabelViewControllers()
{
    for (std::set<LabelSelectionViewController*>::iterator iter = LabelSelectionViewController::allLabelSelectionViewControllers.begin();
         iter != LabelSelectionViewController::allLabelSelectionViewControllers.end();
         iter++) {
        LabelSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateLabelViewController();
        }
    }
}

/**
 * Gets called when label selections are changed.
 */
void 
LabelSelectionViewController::processLabelSelectionChanges()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    CaretAssert(browserTabContent);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesLabels* dsb = brain->getDisplayPropertiesLabels();
    dsb->setDisplayGroupForTab(browserTabIndex, 
                         m_labelsDisplayGroupComboBox->getSelectedDisplayGroup());
    
    
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
LabelSelectionViewController::processSelectionChanges()
{
    updateOtherLabelViewControllers();
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
LabelSelectionViewController::receiveEvent(Event* event)
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
        updateLabelViewController();
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
LabelSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                           const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionViewController",
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
LabelSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
}



