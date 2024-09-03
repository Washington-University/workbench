
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __LABEL_SELECTION_VIEW_WIDGET_DECLARE__
#include "LabelSelectionViewWidget.h"
#undef __LABEL_SELECTION_VIEW_WIDGET_DECLARE__

#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesLabels.h"
#include "EnumComboBoxTemplate.h"
#include "EventManager.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "LabelSelectionViewController.h"
#include "LabelSelectionViewHierarchyController.h"
#include "LabelViewModeEnum.h"
#include "GuiManager.h"
#include "SceneClass.h"

using namespace caret;


    
/**
 * \class caret::LabelSelectionViewWidget 
 * \brief Widget for Label tab of toolbox
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param browserWindowIndex
 *    Index of browser window
 * @param parentObjectName
 *    Name of parent object
 * @param parent
 *    The parent object
 */
LabelSelectionViewWidget::LabelSelectionViewWidget(const int32_t browserWindowIndex,
                                                   const QString& parentObjectName,
                                                   QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* labelViewLabel(new QLabel("View"));
    m_labelViewModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_labelViewModeEnumComboBox->setup<LabelViewModeEnum,LabelViewModeEnum::Enum>();
    QObject::connect(m_labelViewModeEnumComboBox, SIGNAL(itemActivated()),
                      this, SLOT(labelViewModeEnumComboBoxItemActivated()));
    
    QLabel* groupLabel = new QLabel("Group");
    m_labelsDisplayGroupComboBox = new DisplayGroupEnumComboBox(this,
                                                                (parentObjectName
                                                                 + ":DisplayGroup"),
                                                                "labels");
    QObject::connect(m_labelsDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(labelDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    

    m_labelViewController = new LabelSelectionViewController(browserWindowIndex,
                                                             parentObjectName,
                                                             parent);
    
    m_labelViewHierarchyController = new LabelSelectionViewHierarchyController(browserWindowIndex,
                                                                               parentObjectName,
                                                                               parent);
    
    m_stackedWidget = new QStackedWidget();
    m_labelViewControllerTabIndex = m_stackedWidget->addWidget(m_labelViewController);
    m_labelViewHierarchyControllerTabIndex = m_stackedWidget->addWidget(m_labelViewHierarchyController);
    
    QGridLayout* gridLayout(new QGridLayout(this));
    gridLayout->setContentsMargins(2, 2, 2, 2);
    gridLayout->setVerticalSpacing(gridLayout->verticalSpacing() / 2);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 0);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 0);
    gridLayout->setColumnStretch(4, 100);
    int row(0);
    gridLayout->addWidget(labelViewLabel, row, 0);
    gridLayout->addWidget(m_labelViewModeEnumComboBox->getWidget(), row, 1);
    gridLayout->addWidget(groupLabel, row, 2);
    gridLayout->addWidget(m_labelsDisplayGroupComboBox->getWidget(), row, 3, Qt::AlignLeft);
    ++row;
    gridLayout->addWidget(m_stackedWidget, row, 0, 1, 5);
    gridLayout->setRowStretch(row, 100);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
LabelSelectionViewWidget::~LabelSelectionViewWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when the label display group combo box is changed.
 */
void
LabelSelectionViewWidget::labelDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    
    labelViewModeEnumComboBoxItemActivated();
}

/**
 * Called when label view mode combo box is changed
 */
void
LabelSelectionViewWidget::labelViewModeEnumComboBoxItemActivated()
{
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex(browserTabContent->getTabNumber());
    
    Brain* brain(GuiManager::get()->getBrain());
    DisplayPropertiesLabels* dsl(brain->getDisplayPropertiesLabels());
    const LabelViewModeEnum::Enum labelMode = m_labelViewModeEnumComboBox->getSelectedItem<LabelViewModeEnum,LabelViewModeEnum::Enum>();
    dsl->setLabelViewModeForTab(browserTabIndex,
                                labelMode);

    switch (labelMode) {
        case LabelViewModeEnum::HIERARCHY:
            m_labelViewHierarchyController->updateLabelViewController();
            m_stackedWidget->setCurrentWidget(m_labelViewHierarchyController);
            break;
        case LabelViewModeEnum::LIST:
            m_labelViewController->updateLabelViewController();
            m_stackedWidget->setCurrentWidget(m_labelViewController);
            break;
    }
    
    updateAfterSelectionChanges();
}
/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
LabelSelectionViewWidget::receiveEvent(Event* event)
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
        BrowserTabContent* browserTabContent =
        GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
        if (browserTabContent == NULL) {
            return;
        }
        const int32_t browserTabIndex(browserTabContent->getTabNumber());
        
        Brain* brain(GuiManager::get()->getBrain());
        const DisplayPropertiesLabels* dpl(brain->getDisplayPropertiesLabels());

        m_labelViewModeEnumComboBox->setSelectedItem<LabelViewModeEnum,LabelViewModeEnum::Enum>(dpl->getLabelViewModeForTab(browserTabIndex));
        
        const DisplayGroupEnum::Enum displayGroup = dpl->getDisplayGroupForTab(browserTabIndex);
        
        m_labelsDisplayGroupComboBox->setSelectedDisplayGroup(displayGroup);

        switch (dpl->getLabelViewModeForTab(browserTabIndex)) {
            case LabelViewModeEnum::HIERARCHY:
                m_labelViewHierarchyController->updateLabelViewController();
                m_stackedWidget->setCurrentWidget(m_labelViewHierarchyController);
                break;
            case LabelViewModeEnum::LIST:
                m_labelViewController->updateLabelViewController();
                m_stackedWidget->setCurrentWidget(m_labelViewController);
                break;
        }
    }
}

/**
 * Update after selection changes are made
 */
void
LabelSelectionViewWidget::updateAfterSelectionChanges()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
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
LabelSelectionViewWidget::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionViewHierarchyController",
                                            1);
    
    sceneClass->addClass(m_labelViewController->saveToScene(sceneAttributes, 
                                                            "m_labelViewController"));
    sceneClass->addClass(m_labelViewHierarchyController->saveToScene(sceneAttributes, 
                                                                     "m_labelViewHierarchyController"));
    
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
LabelSelectionViewWidget::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_labelViewController->restoreFromScene(sceneAttributes,
                                            sceneClass->getClass("m_labelViewController"));
    m_labelViewHierarchyController->restoreFromScene(sceneAttributes,
                                                     sceneClass->getClass("m_labelViewHierarchyController"));
}
