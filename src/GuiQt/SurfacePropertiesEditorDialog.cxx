
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

#define __SURFACE_PROPERTIES_EDITOR_DIALOG_DECLARE__
#include "SurfacePropertiesEditorDialog.h"
#undef __SURFACE_PROPERTIES_EDITOR_DIALOG_DECLARE__

#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>

using namespace caret;

#include "Brain.h"
#include "CaretAssert.h"
#include "DisplayPropertiesSurface.h"
#include "GuiManager.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"
    
/**
 * \class caret::SurfacePropertiesEditorDialog 
 * \brief Dialog for adjusting surface display properties.
 */

/**
 * Constructor.
 */
SurfacePropertiesEditorDialog::SurfacePropertiesEditorDialog(QWidget* parent)
: WuQDialogNonModal("Surface Properties",
                    parent)
{
    m_updateInProgress = true;
    
    QLabel* surfaceDrawingTypeLabel = new QLabel("Drawing Type: ");
    m_surfaceDrawingTypeComboBox = new EnumComboBoxTemplate(this);
    QObject::connect(m_surfaceDrawingTypeComboBox, SIGNAL(itemSelected()),
                     this, SLOT(surfaceDisplayPropertyChanged()));
    m_surfaceDrawingTypeComboBox->setup<SurfaceDrawingTypeEnum, SurfaceDrawingTypeEnum::Enum>();
    
    QLabel* linkSizeLabel = new QLabel("Link Size: ");
    m_linkSizeSpinBox = new QDoubleSpinBox();
    m_linkSizeSpinBox->setRange(0.0, std::numeric_limits<float>::max());
    m_linkSizeSpinBox->setSingleStep(1.0);
    m_linkSizeSpinBox->setDecimals(1);
    QObject::connect(m_linkSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceDisplayPropertyChanged()));
    
    QLabel* nodeSizeLabel = new QLabel("Node Size: ");
    m_nodeSizeSpinBox = new QDoubleSpinBox();
    m_nodeSizeSpinBox->setRange(0.0, std::numeric_limits<float>::max());
    m_nodeSizeSpinBox->setSingleStep(1.0);
    m_nodeSizeSpinBox->setDecimals(1);
    QObject::connect(m_nodeSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceDisplayPropertyChanged()));
    
    QLabel* displayNormalVectorsLabel = new QLabel("Display Normal Vectors: ");
    m_displayNormalVectorsComboBox = new WuQTrueFalseComboBox(this);
    QObject::connect(m_displayNormalVectorsComboBox, SIGNAL(statusChanged(bool)),
                     this,  SLOT(surfaceDisplayPropertyChanged()));
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    WuQtUtilities::setLayoutMargins(gridLayout, 2, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(displayNormalVectorsLabel, row, 0);
    gridLayout->addWidget(m_displayNormalVectorsComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(surfaceDrawingTypeLabel, row, 0);
    gridLayout->addWidget(m_surfaceDrawingTypeComboBox->getWidget(), row, 1);
    row++;
    gridLayout->addWidget(linkSizeLabel, row, 0);
    gridLayout->addWidget(m_linkSizeSpinBox, row, 1);
    row++;
    gridLayout->addWidget(nodeSizeLabel, row, 0);
    gridLayout->addWidget(m_nodeSizeSpinBox, row, 1);
    row++;

    setCentralWidget(w);
    
    updateDialog();
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    /*
     * No apply button
     */
    setApplyButtonText("");
}

/**
 * Destructor.
 */
SurfacePropertiesEditorDialog::~SurfacePropertiesEditorDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when a surface display property is changed.
 */
void
SurfacePropertiesEditorDialog::surfaceDisplayPropertyChanged()
{
    /*
     * Updating some widgets causes signals to be emitted
     */
    if (m_updateInProgress) {
        return;
    }
    
    const SurfaceDrawingTypeEnum::Enum surfaceDrawingType = m_surfaceDrawingTypeComboBox->getSelectedItem<SurfaceDrawingTypeEnum, SurfaceDrawingTypeEnum::Enum>();
    
    DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    dps->setSurfaceDrawingType(surfaceDrawingType);
    dps->setDisplayNormalVectors(m_displayNormalVectorsComboBox->isTrue());
    dps->setLinkSize(m_linkSizeSpinBox->value());
    dps->setNodeSize(m_nodeSizeSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the properties editor.
 */
void
SurfacePropertiesEditorDialog::updateDialog()
{
    m_updateInProgress = true;
    
    const DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    
    m_surfaceDrawingTypeComboBox->setSelectedItem<SurfaceDrawingTypeEnum, SurfaceDrawingTypeEnum::Enum>(dps->getSurfaceDrawingType());
    m_displayNormalVectorsComboBox->setStatus(dps->isDisplayNormalVectors());
    m_linkSizeSpinBox->setValue(dps->getLinkSize());
    m_nodeSizeSpinBox->setValue(dps->getNodeSize());
    
    m_updateInProgress = false;
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
SurfacePropertiesEditorDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* uiEvent = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiEvent);
        
        updateDialog();
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
SurfacePropertiesEditorDialog::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SurfacePropertiesEditorDialog",
                                            1);
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    sceneClass->addClass(swg.saveToScene(sceneAttributes,
                                         "geometry"));
    
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
SurfacePropertiesEditorDialog::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    /*
     * Position and size
     */
    SceneWindowGeometry swg(this);
    swg.restoreFromScene(sceneAttributes, sceneClass->getClass("geometry"));
}



