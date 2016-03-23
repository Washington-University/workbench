
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__
#include "AnnotationSelectionViewController.h"
#undef __ANNOTATION_SELECTION_VIEW_CONTROLLER_DECLARE__

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "AnnotationFile.h"
#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "DisplayGroupAndTabItemViewController.h"
#include "DisplayGroupEnumComboBox.h"
#include "DisplayPropertiesAnnotation.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSelectionViewController 
 * \brief View controller for display of annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     The parent widget.
 */
AnnotationSelectionViewController::AnnotationSelectionViewController(const int32_t browserWindowIndex,
                                                                     QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* groupLabel = new QLabel("Group");
    
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this);
    QObject::connect(m_displayGroupComboBox, SIGNAL(displayGroupSelected(const DisplayGroupEnum::Enum)),
                     this, SLOT(displayGroupSelected(const DisplayGroupEnum::Enum)));
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();

    m_displayWindowAnnotationInSingleTabViewsCheckBox = new QCheckBox("Show Window Annotations in Single Tab View");
    QObject::connect(m_displayWindowAnnotationInSingleTabViewsCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    
    m_sceneAssistant = new SceneClassAssistant();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_displayWindowAnnotationInSingleTabViewsCheckBox);
    layout->addLayout(groupLayout);
    layout->addWidget(createSelectionWidget(), 100);
    
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
AnnotationSelectionViewController::~AnnotationSelectionViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
    delete m_sceneAssistant;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
AnnotationSelectionViewController::receiveEvent(Event* event)
{
    bool doUpdateFlag = false;
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE) {
        doUpdateFlag = true;
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
       EventUserInterfaceUpdate* eventUI = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUI);

        doUpdateFlag = true;
        
        eventUI->setEventProcessed();
    }
    
    if (doUpdateFlag) {
        updateAnnotationSelections();
    }
}

/**
 * Update the annotation selections.
 */
void
AnnotationSelectionViewController::updateAnnotationSelections()
{
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();
    
    m_displayWindowAnnotationInSingleTabViewsCheckBox->setChecked(dpa->isDisplayWindowAnnotationsInSingleTabViews(m_browserWindowIndex));
    
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<AnnotationFile*> annotationFiles;
    brain->getAllAnnotationFilesIncludingSceneAnnotationFile(annotationFiles);
    
    std::vector<DisplayGroupAndTabItemInterface*> fileItems;
    for (std::vector<AnnotationFile*>::iterator fileIter = annotationFiles.begin();
         fileIter != annotationFiles.end();
         fileIter++) {
        AnnotationFile* annFile = *fileIter;
        fileItems.push_back(annFile);
    }
    
    const DisplayGroupEnum::Enum displayGroup = dpa->getDisplayGroupForTab(browserTabIndex);
    m_displayGroupComboBox->setSelectedDisplayGroup(displayGroup);
    
    m_selectionViewController->updateContent(fileItems,
                                             displayGroup,
                                             browserTabIndex);
}

QWidget*
AnnotationSelectionViewController::createSelectionWidget()
{
    m_selectionViewController = new DisplayGroupAndTabItemViewController(DataFileTypeEnum::ANNOTATION,
                                                                         m_browserWindowIndex);
    return m_selectionViewController;
}


/**
 * Called when one of the checkboxes is clicked.
 */
void
AnnotationSelectionViewController::checkBoxToggled()
{
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }

    dpa->setDisplayWindowAnnotationsInSingleTabViews(m_browserWindowIndex,
                                     m_displayWindowAnnotationInSingleTabViewsCheckBox->isChecked());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when the display group combo box is changed.
 */
void
AnnotationSelectionViewController::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
    
    BrowserTabContent* browserTabContent =
    GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
    if (browserTabContent == NULL) {
        return;
    }
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    dpa->setDisplayGroupForTab(browserTabIndex, displayGroup);
    
    updateAnnotationSelections();
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
AnnotationSelectionViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationSelectionViewController",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
AnnotationSelectionViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

