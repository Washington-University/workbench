
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

#define __VOLUME_PROPERTIES_EDITOR_DIALOG_DECLARE__
#include "VolumePropertiesEditorDialog.h"
#undef __VOLUME_PROPERTIES_EDITOR_DIALOG_DECLARE__

#include <limits>

#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>

using namespace caret;

#include "Brain.h"
#include "CaretAssert.h"
#include "DisplayPropertiesVolume.h"
#include "GuiManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"
    
/**
 * \class caret::VolumePropertiesEditorDialog
 * \brief Dialog for adjusting volume display properties.
 * \ingroup GuitQt
 */

/**
 * Constructor.
 */
VolumePropertiesEditorDialog::VolumePropertiesEditorDialog(QWidget* parent)
: WuQDialogNonModal("Volume Properties",
                    parent)
{
    QLabel* opacityLabel = new QLabel("Opacity: ");
    m_opacitySpinBox = WuQFactory::newDoubleSpinBox();
    m_opacitySpinBox->setRange(0.0, 1.0);
    m_opacitySpinBox->setSingleStep(0.1);
    m_opacitySpinBox->setDecimals(2);
    QObject::connect(m_opacitySpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(displayPropertyChanged()));
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(opacityLabel, row, 0);
    gridLayout->addWidget(m_opacitySpinBox, row, 1);
    row++;

    setCentralWidget(w,
                     WuQDialog::SCROLL_AREA_NEVER);
    
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
VolumePropertiesEditorDialog::~VolumePropertiesEditorDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Called when a display property is changed.
 */
void
VolumePropertiesEditorDialog::displayPropertyChanged()
{
    DisplayPropertiesVolume* dps = GuiManager::get()->getBrain()->getDisplayPropertiesVolume();
    dps->setOpacity(m_opacitySpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update the properties editor.
 */
void
VolumePropertiesEditorDialog::updateDialog()
{
    const DisplayPropertiesVolume* dpv = GuiManager::get()->getBrain()->getDisplayPropertiesVolume();
    
    QSignalBlocker blocker(m_opacitySpinBox);
    m_opacitySpinBox->setValue(dpv->getOpacity());
}

/**
 * Receive events from the event manager.
 *
 * @param event
 *   Event sent by event manager.
 */
void
VolumePropertiesEditorDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        CaretAssert(dynamic_cast<EventUserInterfaceUpdate*>(event) != NULL);
        
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
VolumePropertiesEditorDialog::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumePropertiesEditorDialog",
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
VolumePropertiesEditorDialog::restoreFromScene(const SceneAttributes* sceneAttributes,
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



