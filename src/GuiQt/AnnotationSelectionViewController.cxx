
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
#include <QVBoxLayout>

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
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
    m_displayModelAnnotationCheckBox = new QCheckBox("Show Stereotaxic Annotations");
    QObject::connect(m_displayModelAnnotationCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    
    m_displaySurfaceAnnotationCheckBox = new QCheckBox("Show Surface Annotations");
    QObject::connect(m_displaySurfaceAnnotationCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    
    m_displayTabAnnotationCheckBox = new QCheckBox("Show Tab Annotations");
    QObject::connect(m_displayTabAnnotationCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    
    m_displayWindowAnnotationCheckBox = new QCheckBox("Show Window Annotations in Window "
                                                      + QString::number(m_browserWindowIndex + 1));
    QObject::connect(m_displayWindowAnnotationCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(checkBoxToggled()));
    
    m_sceneAssistant = new SceneClassAssistant();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_displayModelAnnotationCheckBox);
    layout->addWidget(m_displaySurfaceAnnotationCheckBox);
    layout->addWidget(m_displayTabAnnotationCheckBox);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(m_displayWindowAnnotationCheckBox);
    layout->addStretch();
    
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
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
       EventUserInterfaceUpdate* eventUI = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUI);

        DisplayPropertiesAnnotation* dpa = GuiManager::get()->getBrain()->getDisplayPropertiesAnnotation();
        
        BrowserTabContent* browserTabContent =
        GuiManager::get()->getBrowserTabContentForBrowserWindow(m_browserWindowIndex, true);
        if (browserTabContent == NULL) {
            return;
        }
        const int32_t browserTabIndex = browserTabContent->getTabNumber();
        
        const QString inTabText("in Tab "
                                + QString::number(browserTabIndex + 1));
        
        m_displayModelAnnotationCheckBox->setChecked(dpa->isDisplayModelAnnotationsInTab(browserTabIndex));
        m_displayModelAnnotationCheckBox->setText("Display Stereotaxic Annotations "
                                                  + inTabText);
        
        m_displaySurfaceAnnotationCheckBox->setChecked(dpa->isDisplaySurfaceAnnotationsInTab(browserTabIndex));
        m_displaySurfaceAnnotationCheckBox->setText("Display Surface Annotations "
                                                  + inTabText);
        
        m_displayTabAnnotationCheckBox->setChecked(dpa->isDisplayTabAnnotationsInTab(browserTabIndex));
        m_displayTabAnnotationCheckBox->setText("Display Tab Annotations "
                                                  + inTabText);
        
        m_displayWindowAnnotationCheckBox->setChecked(dpa->isDisplayWindowAnnotationsInTab(m_browserWindowIndex));
        
        eventUI->setEventProcessed();
    }
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
    const int32_t browserTabIndex = browserTabContent->getTabNumber();

    dpa->setDisplayModelAnnotationsInTab(browserTabIndex,
                                    m_displayModelAnnotationCheckBox->isChecked());
    dpa->setDisplaySurfaceAnnotationsInTab(browserTabIndex,
                                      m_displaySurfaceAnnotationCheckBox->isChecked());
    dpa->setDisplayTabAnnotationsInTab(browserTabIndex,
                                  m_displayTabAnnotationCheckBox->isChecked());
    dpa->setDisplayWindowAnnotationsInTab(m_browserWindowIndex,
                                     m_displayWindowAnnotationCheckBox->isChecked());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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

