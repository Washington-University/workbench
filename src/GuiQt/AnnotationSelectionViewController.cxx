
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

#define __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "AnnotationSelectionViewController.h"
#undef __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__

#include "Brain.h"
#include "BrainOpenGL.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "GroupAndNameHierarchyViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesAnnotation.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQTabWidget.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSelectionViewController 
 * \brief Widget for controlling display of annotations
 *
 * Widget for controlling the display of annotations including
 * different display groups.
 */

/**
 * Constructor.
 */
AnnotationSelectionViewController::AnnotationSelectionViewController(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QWidget(parent)
{
    m_browserWindowIndex = browserWindowIndex;
    
    QLabel* groupLabel = new QLabel("Group");
    m_annotationsDisplayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_annotationsDisplayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(annotationDisplayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_annotationsDisplayGroupComboBox->getWidget());
    groupLayout->addStretch(); 
    
    m_annotationsDisplayCheckBox = new QCheckBox("Display Annotations");
    QObject::connect(m_annotationsDisplayCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(processAttributesChanges()));
    
    QWidget* attributesWidget = this->createAttributesWidget();
    QWidget* selectionWidget = this->createSelectionWidget();
    
    m_tabWidget = new WuQTabWidget(WuQTabWidget::TAB_ALIGN_LEFT,
                                               this);
    const int attributesTabIndex = m_tabWidget->addTab(attributesWidget,
                      "Attributes");
    m_tabWidget->addTab(selectionWidget, 
                      "Selection");
    m_tabWidget->setCurrentWidget(selectionWidget);
    

    /*
     * DISABLE ATTRIBUTES TAB SINCE UNUSED
     */
    m_tabWidget->setTabEnabled(attributesTabIndex,
                               false);
    
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addLayout(groupLayout);
    layout->addSpacing(10);
    layout->addWidget(m_annotationsDisplayCheckBox);
    layout->addWidget(m_tabWidget->getWidget(), 0, Qt::AlignLeft);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    AnnotationSelectionViewController::allAnnotationSelectionViewControllers.insert(this);
}

/**
 * Destructor.
 */
AnnotationSelectionViewController::~AnnotationSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    AnnotationSelectionViewController::allAnnotationSelectionViewControllers.erase(this);
}


QWidget* 
AnnotationSelectionViewController::createSelectionWidget()
{
    m_annotationClassNameHierarchyViewController = new GroupAndNameHierarchyViewController(m_browserWindowIndex);
    
    return m_annotationClassNameHierarchyViewController;
}

/**
 * @return The attributes widget.
 */
QWidget* 
AnnotationSelectionViewController::createAttributesWidget()
{
//    QObject::connect(m_annotationsContralateralCheckBox, SIGNAL(clicked(bool)),
//                     this, SLOT(processAttributesChanges()));
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addStretch();
        
    return widget;
}

/**
 * Called when a widget on the attributes page has 
 * its value changed.
 */
void 
AnnotationSelectionViewController::processAttributesChanges()
{
    DisplayPropertiesAnnotation* dpb = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
    dpb->setDisplayed(displayGroup,
                      browserTabIndex,
                      m_annotationsDisplayCheckBox->isChecked());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    
    updateOtherAnnotationViewControllers();
}

/**
 * Called when the annotation display group combo box is changed.
 */
void 
AnnotationSelectionViewController::annotationDisplayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
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
    DisplayPropertiesAnnotation* dsb = brain->getDisplayPropertiesAnnotation();
    dsb->setDisplayGroupForTab(browserTabIndex,
                         displayGroup);
    
    /*
     * Since display group has changed, need to update controls
     */
    updateAnnotationViewController();
    
    /*
     * Apply the changes.
     */
    processAnnotationSelectionChanges();
}

/**
 * Update the annotation selection widget.
 */
void 
AnnotationSelectionViewController::updateAnnotationViewController()
{
    setWindowTitle("Annotations");
    
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesAnnotation* dpb = brain->getDisplayPropertiesAnnotation();
    const DisplayGroupEnum::Enum displayGroup = dpb->getDisplayGroupForTab(browserTabIndex);
    
    m_annotationsDisplayGroupComboBox->setSelectedDisplayGroup(dpb->getDisplayGroupForTab(browserTabIndex));
    
    /*;
     * Get all of annotation files.
     */
    std::vector<AnnotationFile*> allAnnotationFiles;
    brain->getAllAnnotationFilesIncludingSceneAnnotationFile(allAnnotationFiles);
    const int32_t numberOfAnnotationFiles = static_cast<int32_t>(allAnnotationFiles.size());
    
    /*
     * Update the class/name hierarchy
     */
    m_annotationClassNameHierarchyViewController->updateContents(allAnnotationFiles,
                                                                 displayGroup);

    
    m_annotationsDisplayCheckBox->setChecked(dpb->isDisplayed(displayGroup,
                                                          browserTabIndex));
}

/**
 * Update other selection toolbox since they should all be the same.
 */
void 
AnnotationSelectionViewController::updateOtherAnnotationViewControllers()
{
    for (std::set<AnnotationSelectionViewController*>::iterator iter = AnnotationSelectionViewController::allAnnotationSelectionViewControllers.begin();
         iter != AnnotationSelectionViewController::allAnnotationSelectionViewControllers.end();
         iter++) {
        AnnotationSelectionViewController* bsw = *iter;
        if (bsw != this) {
            bsw->updateAnnotationViewController();
        }
    }
}

/**
 * Gets called when annotation selections are changed.
 */
void 
AnnotationSelectionViewController::processAnnotationSelectionChanges()
{
    BrowserTabContent* browserTabContent = 
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, false);
    CaretAssert(browserTabContent);
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    Brain* brain = GuiManager::get()->getBrain();
    DisplayPropertiesAnnotation* dsb = brain->getDisplayPropertiesAnnotation();
    dsb->setDisplayGroupForTab(browserTabIndex, 
                         m_annotationsDisplayGroupComboBox->getSelectedDisplayGroup());
    
    
    processSelectionChanges();
}

/**
 * Issue update events after selections are changed.
 */
void 
AnnotationSelectionViewController::processSelectionChanges()
{
    updateOtherAnnotationViewControllers();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Receive events from the event manager.
 * 
 * @param event
 *   Event sent by event manager.
 */
void 
AnnotationSelectionViewController::receiveEvent(Event* event)
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
        updateAnnotationViewController();
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
AnnotationSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                               const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationSelectionViewController",
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
AnnotationSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                    const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_tabWidget->restoreFromScene(sceneAttributes,
                                  sceneClass->getClass("m_tabWidget"));
}


