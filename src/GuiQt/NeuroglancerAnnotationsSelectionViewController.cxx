
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
#include <QLabel>
#include <QLayout>
#include <QToolButton>

#define __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "NeuroglancerAnnotationsSelectionViewController.h"
#undef __NEUROGLANCER_ANNOTATIONS_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesNeuroglancerAnnotations.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
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
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    m_displayCheckBox = new QCheckBox("Display Neuroglancer Annotations");
    m_displayCheckBox->setToolTip("Enable the display of neuroglancer annotations");
    QObject::connect(m_displayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    m_displayCheckBox->setObjectName(m_objectNamePrefix
                                            + ":DisplayNeuroglancerAnnotations");
    WuQMacroManager::instance()->addMacroSupportToObject(m_displayCheckBox,
                                                         "Enable neuroglancer annotation display");
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    m_tabWidget->addTab(attributesWidget, 
                      "Attributes");
    m_tabWidget->addTab(selectionWidget, 
                      "Selection");
    m_tabWidget->setCurrentWidget(attributesWidget);
    m_tabWidget->getTabBar()->setToolTip("Select neuroglancer annotations tab");
    m_tabWidget->getTabBar()->setObjectName(m_objectNamePrefix
                                            + ":Tab");
    WuQMacroManager::instance()->addMacroSupportToObject(m_tabWidget->getTabBar(),
                                                         "Select features toolbox neuroglancer annotations tab");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_displayCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(groupLayout);
    layout->addWidget(m_tabWidget->getWidget(), 100);
    layout->addStretch();
    
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
 * @return New instance of neuro ann selection widget
 */
QWidget* 
NeuroglancerAnnotationsSelectionViewController::createSelectionWidget()
{
    QWidget* widget(new QWidget());
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
    processNeuroAnnSelectionChanges();
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
 * Gets called when neuroglancer annotations selections are changed.
 */
void 
NeuroglancerAnnotationsSelectionViewController::processNeuroAnnSelectionChanges()
{
    processSelectionChanges();
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


