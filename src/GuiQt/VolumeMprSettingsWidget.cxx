
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __VOLUME_MPR_SETTINGS_WIDGET_DECLARE__
#include "VolumeMprSettingsWidget.h"
#undef __VOLUME_MPR_SETTINGS_WIDGET_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserTabGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "VolumeMprSettingsWidget.h"

using namespace caret;

/**
 * \class caret::VolumeMprSettingsWidget 
 * \brief Widget containing Volume MPR Settings
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
VolumeMprSettingsWidget::VolumeMprSettingsWidget(const QString& objectNamePrefix,
                                                 QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(objectNamePrefix
                   + ":ClippingPlanesWidget")
{
    setObjectName(m_objectNamePrefix);

    QLabel* modeLabel(new QLabel("MPR Mode: "));
    m_viewModeComboBox = new EnumComboBoxTemplate(this);
    m_viewModeComboBox->setup<VolumeMprViewModeEnum,VolumeMprViewModeEnum::Enum>();
    QObject::connect(m_viewModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &VolumeMprSettingsWidget::modeComboBoxActivated);
    m_viewModeComboBox->getWidget()->setToolTip("Select intensity projection");
    
    /*
     * Change color of Secondary Mode button text using the palette
     */
    m_viewModeComboBoxTextPalette = m_viewModeComboBox->getWidget()->palette();
    m_viewModeComboBoxRedTextPalette = m_viewModeComboBox->getWidget()->palette();
    m_viewModeComboBoxRedTextPalette.setColor(QPalette::Text, QColor(255, 0, 0));
    
    QLabel* orientationLabel(new QLabel("Orientation: "));
    m_orientationComboBox = new EnumComboBoxTemplate(this);
    m_orientationComboBox->setup<VolumeMprOrientationModeEnum,VolumeMprOrientationModeEnum::Enum>();
    QObject::connect(m_orientationComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &VolumeMprSettingsWidget::orientationComboBoxActivated);
    m_orientationComboBox->getWidget()->setToolTip("Select orientation");
    
    QLabel* thicknessLabel(new QLabel("Slice Thickness: "));
    m_sliceThicknessSpinBox = new QDoubleSpinBox();
    QObject::connect(m_sliceThicknessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     this, &VolumeMprSettingsWidget::sliceThicknessSpinBoxValueChanged);
    m_sliceThicknessSpinBox->setToolTip("Select thickness for sub-region of volume");
    
    m_axialSliceThicknessCheckBox = new QCheckBox("Apply Slice Thickness to Axial");
    QObject::connect(m_axialSliceThicknessCheckBox, &QCheckBox::clicked,
                     this, &VolumeMprSettingsWidget::axialThickessCheckBoxClicked);
    m_axialSliceThicknessCheckBox->setToolTip("Limit volume to thickness along axial axis");
    
    m_coronalSliceThicknessCheckBox = new QCheckBox("Apply Slice Thickness to Coronal");
    QObject::connect(m_coronalSliceThicknessCheckBox, &QCheckBox::clicked,
                     this, &VolumeMprSettingsWidget::coronalThickessCheckBoxClicked);
    m_coronalSliceThicknessCheckBox->setToolTip("Limit volume to thickness along coronal axis");

    m_parasagittalSliceThicknessCheckBox = new QCheckBox("Apply Slice Thickness to Parasagittal");
    QObject::connect(m_parasagittalSliceThicknessCheckBox, &QCheckBox::clicked,
                     this, &VolumeMprSettingsWidget::parasagittalThickessCheckBoxClicked);
    m_parasagittalSliceThicknessCheckBox->setToolTip("Limit volume to thickness along parasagittal axis");

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setHorizontalSpacing(gridLayout->horizontalSpacing() / 2);
    gridLayout->setVerticalSpacing((gridLayout->verticalSpacing() * 2) / 3);
    int row(0);
    gridLayout->addWidget(modeLabel, row, 0);
    gridLayout->addWidget(m_viewModeComboBox->getWidget(), row, 1);
    ++row;
    gridLayout->addWidget(orientationLabel, row, 0);
    gridLayout->addWidget(m_orientationComboBox->getWidget(), row, 1);
    ++row;
    gridLayout->addWidget(thicknessLabel, row, 0);
    gridLayout->addWidget(m_sliceThicknessSpinBox, row, 1);
    ++row;
    gridLayout->addWidget(m_axialSliceThicknessCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    ++row;
    gridLayout->addWidget(m_coronalSliceThicknessCheckBox, row, 0, 1, 2, Qt::AlignLeft);
    ++row;
    gridLayout->addWidget(m_parasagittalSliceThicknessCheckBox, row, 0, 1, 2, Qt::AlignLeft);
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
VolumeMprSettingsWidget::~VolumeMprSettingsWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the selected graphics window.
 */
void
VolumeMprSettingsWidget::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * @return Browser tab content for current tab index or NULL if invalid tab index
 */
BrowserTabContent*
VolumeMprSettingsWidget::getBrowserTabContent()
{
    EventBrowserTabGet tabEvent(m_tabIndex);
    EventManager::get()->sendEvent(tabEvent.getPointer());
    BrowserTabContent* tabContent(tabEvent.getBrowserTab());
    return tabContent;
}


/**
 * Update the content in the dialog
 * @param browserWindowIndexIn
 *    Index of the browser window.
 */
void
VolumeMprSettingsWidget::updateContent(const int32_t tabIndex)
{
    setEnabled(false);
    
    m_tabIndex = tabIndex;
    
    BrowserTabContent* btc = getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    
    m_viewModeComboBox->setSelectedItem<VolumeMprViewModeEnum, VolumeMprViewModeEnum::Enum>(btc->getVolumeMprViewMode());
    m_orientationComboBox->setSelectedItem<VolumeMprOrientationModeEnum,VolumeMprOrientationModeEnum::Enum>(btc->getVolumeMprOrientationMode());
    QSignalBlocker thicknessBlocker(m_sliceThicknessSpinBox);
    m_sliceThicknessSpinBox->setValue(btc->getVolumeMprSliceThickness());
    m_axialSliceThicknessCheckBox->setChecked(btc->isVolumeMprAxialSliceThicknessEnabled());
    m_coronalSliceThicknessCheckBox->setChecked(btc->isVolumeMprCoronalSliceThicknessEnabled());
    m_parasagittalSliceThicknessCheckBox->setChecked(btc->isVolumeMprParasagittalSliceThicknessEnabled());
    
    updateOrientationComboBoxColor();
    setEnabled(true);
}

/**
 * Update the color of the text in the orientation button
 */
void
VolumeMprSettingsWidget::updateOrientationComboBoxColor()
{
    BrowserTabContent* btc = getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    
    const VolumeMprOrientationModeEnum::Enum orientationMode(btc->getVolumeMprOrientationMode());
    switch (orientationMode) {
        case VolumeMprOrientationModeEnum::NEUROLOGICAL:
            m_orientationComboBox->getWidget()->setPalette(m_viewModeComboBoxTextPalette);
            break;
        case VolumeMprOrientationModeEnum::RADIOLOGICAL:
            m_orientationComboBox->getWidget()->setPalette(m_viewModeComboBoxRedTextPalette);
            break;
    }
}

/**
 * Called when mode combo box is activated
 */
void
VolumeMprSettingsWidget::modeComboBoxActivated()
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }
    
    const VolumeMprViewModeEnum::Enum viewMode = m_viewModeComboBox->getSelectedItem<VolumeMprViewModeEnum,VolumeMprViewModeEnum::Enum>();
    btc->setVolumeMprViewMode(viewMode);
    updateGraphicsWindow();
    
    /* Force update of combo box */
    m_viewModeComboBox->getWidget()->update();
}

/**
 * Called when orientation combo box is activated
 */
void
VolumeMprSettingsWidget::orientationComboBoxActivated()
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }

    const VolumeMprOrientationModeEnum::Enum orientationMode = m_orientationComboBox->getSelectedItem<VolumeMprOrientationModeEnum,VolumeMprOrientationModeEnum::Enum>();
    btc->setVolumeMprOrientationMode(orientationMode);
    updateGraphicsWindow();
    
    /* Force update of combo box */
    updateOrientationComboBoxColor();
    m_orientationComboBox->getWidget()->update();
}


/**
 * Called when slice thickness value is changed
 * @param sliceThickness
 *    New slice thickness
 */
void
VolumeMprSettingsWidget::sliceThicknessSpinBoxValueChanged(double sliceThickness)
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }
    
    btc->setVolumeMprSliceThickness(sliceThickness);
    updateGraphicsWindow();
}

/**
 * Called when axial slice checkbox is toggle
 * @bool checked
 *    New checked status
 */
void
VolumeMprSettingsWidget::axialThickessCheckBoxClicked(bool)
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }
    
    btc->setVolumeMprAxialSliceThicknessEnabled(m_axialSliceThicknessCheckBox->isChecked());
    updateGraphicsWindow();
}

/**
 * Called when coronal slice checkbox is toggle
 * @bool checked
 *    New checked status
 */
void
VolumeMprSettingsWidget::coronalThickessCheckBoxClicked(bool)
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }

    btc->setVolumeMprCoronalSliceThicknessEnabled(m_coronalSliceThicknessCheckBox->isChecked());
    updateGraphicsWindow();
}

/**
 * Called when parasagittal slice checkbox is toggle
 * @bool checked
 *    New checked status
 */
void
VolumeMprSettingsWidget::parasagittalThickessCheckBoxClicked(bool)
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }
    
    btc->setVolumeMprParasagittalSliceThicknessEnabled(m_parasagittalSliceThicknessCheckBox->isChecked());
    updateGraphicsWindow();
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
VolumeMprSettingsWidget::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

