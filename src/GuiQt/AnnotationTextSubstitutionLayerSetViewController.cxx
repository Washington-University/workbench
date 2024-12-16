
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

#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_VIEW_CONTROLLER_DECLARE__
#include "AnnotationTextSubstitutionLayerSetViewController.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_VIEW_CONTROLLER_DECLARE__

#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>

#include "AnnotationTextSubstitutionFile.h"
#include "AnnotationTextSubstitutionLayer.h"
#include "AnnotationTextSubstitutionLayerSet.h"
#include "AnnotationTextSubstitutionLayerViewController.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EventUserInterfaceUpdate.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitutionLayerSetViewController
 * \brief View controller for display of annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parentObjectName
 *     Name of parent object for macros
 * @param parent
 *     The parent widget.
 */
AnnotationTextSubstitutionLayerSetViewController::AnnotationTextSubstitutionLayerSetViewController(const QString& parentObjectName,
                                                                                                   QWidget* parent)
: QWidget(parent)
{
    m_objectName = (parentObjectName + ":AnnTextSubsLayerSetViewController");
    m_sceneAssistant = new SceneClassAssistant();
    
    m_layersGridLayout = new QGridLayout();
    
    QWidget* layersWidget(new QWidget());
    QVBoxLayout* layerLayout(new QVBoxLayout(layersWidget));
    layerLayout->setContentsMargins(0, 0, 0, 0);
    layerLayout->addLayout(m_layersGridLayout);
    layerLayout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(layersWidget);
    scrollArea->setWidgetResizable(true);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea, 100);
    layout->addStretch();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    updateSelections();
}

/**
 * Destructor.
 */
AnnotationTextSubstitutionLayerSetViewController::~AnnotationTextSubstitutionLayerSetViewController()
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
AnnotationTextSubstitutionLayerSetViewController::receiveEvent(Event* event)
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
        updateSelections();
    }
}


/**
 * Update the annotation selections.
 */
void
AnnotationTextSubstitutionLayerSetViewController::updateSelections()
{
    Brain* brain = GuiManager::get()->getBrain();
    CaretAssert(brain);
    
    AnnotationTextSubstitutionLayerSet* layerSet(brain->getAnnotationTextSubstitutionLayerSet());
    CaretAssert(layerSet);
    
    const int32_t numDisplayedLayers(layerSet->getNumberOfLayers());
    int32_t numLayersViewControllers(m_layerViewControllers.size());
    
    for (int32_t iLayer = numLayersViewControllers; iLayer < numDisplayedLayers; iLayer++) {
        AnnotationTextSubstitutionLayerViewController* vc(new AnnotationTextSubstitutionLayerViewController(m_layersGridLayout,
                                                                                                            iLayer,
                                                                                                            m_objectName,
                                                                                                            this));
        m_layerViewControllers.push_back(vc);
    }
    
    numLayersViewControllers = static_cast<int32_t>(m_layerViewControllers.size());
    for (int32_t iLayer = 0; iLayer < numLayersViewControllers; iLayer++) {
        CaretAssertVectorIndex(m_layerViewControllers, iLayer);
        AnnotationTextSubstitutionLayerViewController* lvc(m_layerViewControllers[iLayer]);
        CaretAssert(lvc);
        
        if (iLayer < numDisplayedLayers) {
            lvc->updateViewController(layerSet->getLayer(iLayer));
            lvc->setVisible(true);
        }
        else {
            lvc->setVisible(false);
        }
    }
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
AnnotationTextSubstitutionLayerSetViewController::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationTextSubstitutionLayerSetViewController",
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
AnnotationTextSubstitutionLayerSetViewController::restoreFromScene(const SceneAttributes* sceneAttributes,
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

