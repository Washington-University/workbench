
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

#define __SURFACE_PROPERTIES_EDITOR_DIALOG_DECLARE__
#include "SurfacePropertiesEditorDialog.h"
#undef __SURFACE_PROPERTIES_EDITOR_DIALOG_DECLARE__

#include <limits>

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
#include "EventSurfaceColoringInvalidate.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQMacroWidgetAction.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"
    
/**
 * \class caret::SurfacePropertiesEditorDialog 
 * \brief Dialog for adjusting surface display properties.
 * \ingroup GuitQt
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
    QObject::connect(m_surfaceDrawingTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(surfaceDisplayPropertyChanged()));
    m_surfaceDrawingTypeComboBox->setup<SurfaceDrawingTypeEnum, SurfaceDrawingTypeEnum::Enum>();
    
    QLabel* linkSizeLabel = new QLabel("Link Diameter: ");
    m_linkSizeSpinBox = WuQFactory::newDoubleSpinBox();
    m_linkSizeSpinBox->setRange(0.0, std::numeric_limits<float>::max());
    m_linkSizeSpinBox->setSingleStep(1.0);
    m_linkSizeSpinBox->setDecimals(1);
    m_linkSizeSpinBox->setSuffix("mm");
    QObject::connect(m_linkSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceDisplayPropertyChanged()));
    
    QLabel* nodeSizeLabel = new QLabel("Vertex Diameter: ");
    m_nodeSizeSpinBox = WuQFactory::newDoubleSpinBox();
    m_nodeSizeSpinBox->setRange(0.0, std::numeric_limits<float>::max());
    m_nodeSizeSpinBox->setSingleStep(1.0);
    m_nodeSizeSpinBox->setDecimals(1);
    m_nodeSizeSpinBox->setSuffix("mm");
    QObject::connect(m_nodeSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceDisplayPropertyChanged()));
    
    QLabel* displayNormalVectorsLabel = new QLabel("Display Normal Vectors: ");
    m_displayNormalVectorsComboBox = new WuQTrueFalseComboBox(this);
    QObject::connect(m_displayNormalVectorsComboBox, SIGNAL(statusChanged(bool)),
                     this,  SLOT(surfaceDisplayPropertyChanged()));
    
    QLabel* opacityLabel = new QLabel("Opacity: ");
    
    m_opacityMacroWidgetAction = WuQMacroManager::instance()->getMacroWidgetActionByName("SurfaceProperties:surfaceOpacity");
    if (m_opacityMacroWidgetAction != NULL) {
        QWidget* widget = m_opacityMacroWidgetAction->requestWidget(this);
        if (widget != NULL) {
            QDoubleSpinBox* dsb = qobject_cast<QDoubleSpinBox*>(widget);
            if (dsb != NULL) {
                m_opacitySpinBox = dsb;
            }
        }
    }
    if (m_opacitySpinBox == NULL) {
        m_opacityMacroWidgetAction = NULL;
        m_opacitySpinBox = WuQFactory::newDoubleSpinBox();
        QObject::connect(m_opacitySpinBox, SIGNAL(valueChanged(double)),
                         this, SLOT(surfaceDisplayPropertyChanged()));
    }
    m_opacitySpinBox->setRange(0.0, 1.0);
    m_opacitySpinBox->setSingleStep(0.1);
    m_opacitySpinBox->setDecimals(2);
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
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
SurfacePropertiesEditorDialog::~SurfacePropertiesEditorDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    if (m_opacityMacroWidgetAction != NULL) {
        m_opacityMacroWidgetAction->releaseWidget(m_opacitySpinBox);
    }
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
    dps->setOpacity(m_opacitySpinBox->value());
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
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
    m_opacitySpinBox->setValue(dps->getOpacity());
    
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



