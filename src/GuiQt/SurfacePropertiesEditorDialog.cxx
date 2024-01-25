
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

#include <QCheckBox>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QToolButton>

using namespace caret;

#include "Brain.h"
#include "CaretAssert.h"
#include "DisplayPropertiesSurface.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SceneClass.h"
#include "SceneWindowGeometry.h"
#include "WuQFactory.h"
#include "WuQMacroCommand.h"
#include "WuQMacroCommandParameter.h"
#include "WbMacroCustomOperationSurfaceDefaultColor.h"
#include "WuQMacroManager.h"
#include "WuQMacroWidgetAction.h"
#include "WbMacroWidgetActionNames.h"
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
    
    m_backfaceCullingCheckBox = new QCheckBox("Backface Culling");
    QObject::connect(m_backfaceCullingCheckBox, &QCheckBox::clicked,
                     this, &SurfacePropertiesEditorDialog::backfaceCullingCheckBoxClicked);
    
    WuQMacroManager* mm = WuQMacroManager::instance();
    CaretAssert(mm);
    
    QLabel* surfaceDrawingTypeLabel = new QLabel("Drawing Type: ");
    QWidget* drawTypeWidget = mm->getWidgetForMacroWidgetActionByName(WbMacroWidgetActionNames::getSurfacePropertiesDrawingTypeName());
    if (drawTypeWidget != NULL) {
        m_surfaceDrawingTypeComboBox = qobject_cast<QComboBox*>(drawTypeWidget);
        CaretAssert(m_surfaceDrawingTypeComboBox);
    }
    if (m_surfaceDrawingTypeComboBox == NULL) {
        m_surfaceDrawingTypeComboBox = new QComboBox();
        m_surfaceDrawingTypeComboBox->setEnabled(false);
    }
    
    QLabel* linkSizeLabel = new QLabel("Link Diameter: ");
    
    QWidget* linkSizeMacroWidget = mm->getWidgetForMacroWidgetActionByName(WbMacroWidgetActionNames::getSurfacePropertiesLinkDiameterName());
    if (linkSizeMacroWidget != NULL) {
        m_linkSizeSpinBox = qobject_cast<QDoubleSpinBox*>(linkSizeMacroWidget);
        CaretAssert(m_linkSizeSpinBox);
    }
    if (m_linkSizeSpinBox == NULL) {
        m_linkSizeSpinBox = WuQFactory::newDoubleSpinBox();
        m_linkSizeSpinBox->setEnabled(false);
    }
    m_linkSizeSpinBox->setSuffix("mm");
    
    QLabel* nodeSizeLabel = new QLabel("Vertex Diameter: ");
    
    QWidget* nodeSizeWidget = mm->getWidgetForMacroWidgetActionByName(WbMacroWidgetActionNames::getSurfacePropertiesVertexDiameterName());
    if (nodeSizeWidget != NULL) {
        m_nodeSizeSpinBox = qobject_cast<QDoubleSpinBox*>(nodeSizeWidget);
        CaretAssert(m_nodeSizeSpinBox);
    }
    if (m_nodeSizeSpinBox == NULL) {
        m_nodeSizeSpinBox = WuQFactory::newDoubleSpinBox();
        m_nodeSizeSpinBox->setEnabled(false);
    }
    m_nodeSizeSpinBox->setSuffix("mm");
    
    QWidget* displayNormalsWidget = mm->getWidgetForMacroWidgetActionByName(WbMacroWidgetActionNames::getSurfacePropertiesDisplayNormalVectorsName());
    if (displayNormalsWidget != NULL) {
        m_displayNormalVectorsCheckBox = qobject_cast<QCheckBox*>(displayNormalsWidget);
        CaretAssert(m_displayNormalVectorsCheckBox);
    }
    if (m_displayNormalVectorsCheckBox == NULL) {
        m_displayNormalVectorsCheckBox = new QCheckBox();
        m_displayNormalVectorsCheckBox->setEnabled(false);
    }
    m_displayNormalVectorsCheckBox->setText("Display Normal Vectors");
    
    QLabel* opacityLabel = new QLabel("Opacity: ");
    
    QWidget* opacityMacroWidget = mm->getWidgetForMacroWidgetActionByName(WbMacroWidgetActionNames::getSurfacePropertiesOpacityName());
    if (opacityMacroWidget != NULL) {
        m_opacitySpinBox = qobject_cast<QDoubleSpinBox*>(opacityMacroWidget);
        CaretAssert(m_opacitySpinBox);
    }
    if (m_opacitySpinBox == NULL) {
        m_opacitySpinBox = new QDoubleSpinBox();
        m_opacitySpinBox->setEnabled(false);
    }
    
    QLabel* surfaceColorLabel = new QLabel("Default Color: ");
    QString colorToolTip("Click to set the default surface color that is applied when no overlays color a vertex");
    QToolButton* surfaceSetColorToolButton = new QToolButton();
    surfaceSetColorToolButton->setText("Set...");
    QObject::connect(surfaceSetColorToolButton, &QToolButton::clicked,
                     this, &SurfacePropertiesEditorDialog::surfaceSetColorToolButtonClicked);
    WuQtUtilities::setWordWrappedToolTip(surfaceSetColorToolButton,
                                         colorToolTip);
    
    QToolButton* surfaceResetColorToolButton = new QToolButton();
    surfaceResetColorToolButton->setText("Reset");
    QObject::connect(surfaceResetColorToolButton, &QToolButton::clicked,
                     this, &SurfacePropertiesEditorDialog::surfaceResetColorToolButtonClicked);
    
    m_surfaceColorWidget = new QWidget();
    m_surfaceColorWidget->setMinimumHeight(10);
    m_surfaceColorWidget->setMinimumWidth(50);
    
    QWidget* w = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(w);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(m_backfaceCullingCheckBox, row, 0, 1, 3);
    row++;
    gridLayout->addWidget(m_displayNormalVectorsCheckBox, row, 0, 1, 4);
    row++;
    gridLayout->addWidget(surfaceDrawingTypeLabel, row, 0);
    gridLayout->addWidget(m_surfaceDrawingTypeComboBox, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(linkSizeLabel, row, 0);
    gridLayout->addWidget(m_linkSizeSpinBox, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(nodeSizeLabel, row, 0);
    gridLayout->addWidget(m_nodeSizeSpinBox, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(opacityLabel, row, 0);
    gridLayout->addWidget(m_opacitySpinBox, row, 1, 1, 3);
    row++;
    gridLayout->addWidget(surfaceColorLabel, row, 0);
    gridLayout->addWidget(surfaceSetColorToolButton, row, 1);
    gridLayout->addWidget(surfaceResetColorToolButton, row, 2);
    gridLayout->addWidget(m_surfaceColorWidget, row, 3);
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
    
    WuQMacroManager::instance()->releaseWidgetFromMacroWidgetAction(m_surfaceDrawingTypeComboBox,
                                                                    m_linkSizeSpinBox,
                                                                    m_nodeSizeSpinBox,
                                                                    m_opacitySpinBox,
                                                                    m_displayNormalVectorsCheckBox);
}

/**
 * Update the properties editor.
 */
void
SurfacePropertiesEditorDialog::updateDialog()
{
    m_updateInProgress = true;
    
    updateDefaultSurfaceColorWidget();
    
    WuQMacroManager::instance()->updateValueInWidgetFromMacroWidgetAction(m_surfaceDrawingTypeComboBox,
                                                                          m_linkSizeSpinBox,
                                                                          m_nodeSizeSpinBox,
                                                                          m_opacitySpinBox,
                                                                          m_displayNormalVectorsCheckBox);
    
    m_updateInProgress = false;
    
    DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    m_backfaceCullingCheckBox->setChecked(dps->isBackfaceCullingEnabled());

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
 * Called when set surface color tool button is clicked
 */
void
SurfacePropertiesEditorDialog::surfaceSetColorToolButtonClicked()
{
    DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    std::array<uint8_t, 3> rgb = dps->getDefaultColorRGB();

    const QColor initialColor(rgb[0],
                              rgb[1],
                              rgb[2]);
    
    QColorDialog colorDialog(this);
    colorDialog.setOption(QColorDialog::DontUseNativeDialog);
    colorDialog.setWindowTitle("Default Surface Color RGB");
    colorDialog.setCurrentColor(initialColor);
    
    if (colorDialog.exec() == QColorDialog::Accepted) {
        const QColor newColor = colorDialog.currentColor();
        rgb[0] = newColor.red();
        rgb[1] = newColor.green();
        rgb[2] = newColor.blue();
        dps->setDefaultColorRGB(rgb);
    }
    
    updateDefaultSurfaceColorWidget();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    
    /*
     * Get new instance of custom macro
     */
    WbMacroCustomOperationSurfaceDefaultColor commandOperation;
    WuQMacroCommand* command = commandOperation.createCommand();
    if (command->getNumberOfParameters() >= 3) {
        command->getParameterAtIndex(0)->setValue(rgb[0]);
        command->getParameterAtIndex(1)->setValue(rgb[1]);
        command->getParameterAtIndex(2)->setValue(rgb[2]);

        WuQMacroManager* macroManager = WuQMacroManager::instance();
        CaretAssert(macroManager);
        if ( ! macroManager->addMacroCommandToRecording(command)) {
            /* not recording */
            delete command;
        }
    }
}

/**
 * Called when reset surface color tool button is clicked
 */
void
SurfacePropertiesEditorDialog::surfaceResetColorToolButtonClicked()
{
    DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    dps->resetDefaultColorRGB();

    updateDefaultSurfaceColorWidget();
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Update the default surface color widget with the color in the surface properties
 */
void
SurfacePropertiesEditorDialog::updateDefaultSurfaceColorWidget()
{
    const DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    const std::array<uint8_t, 3> colorRGB = dps->getDefaultColorRGB();
    m_surfaceColorWidget->setStyleSheet("background-color: rgb("
                                        + AString::number(colorRGB[0])
                                        + ", " + AString::number(colorRGB[1])
                                        + ", " + AString::number(colorRGB[2])
                                        + ");");
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

/**
 * Called when backface culling check box clicked
 */
void
SurfacePropertiesEditorDialog::backfaceCullingCheckBoxClicked(bool checked)
{
    DisplayPropertiesSurface* dps = GuiManager::get()->getBrain()->getDisplayPropertiesSurface();
    dps->setBackfaceCullingEnabled(checked);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

