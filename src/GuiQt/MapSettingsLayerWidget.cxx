
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

#define __MAP_SETTINGS_LAYER_WIDGET_DECLARE__
#include "MapSettingsLayerWidget.h"
#undef __MAP_SETTINGS_LAYER_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretMappableDataFile.h"
#include "CursorDisplayScoped.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "Overlay.h"
#include "VolumeMappableInterface.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::MapSettingsLayerWidget 
 * \brief Contains user-interface components for overlay parameters.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsLayerWidget::MapSettingsLayerWidget(QWidget* parent)
: QWidget(parent)
{
    m_wholeBrainWidget = createWholeBrainWidget();
    m_volumeToImageWidget = createVolumeToImageWidget();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_wholeBrainWidget, 0, Qt::AlignLeft);
    layout->addWidget(m_volumeToImageWidget, 0, Qt::AlignLeft);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsLayerWidget::~MapSettingsLayerWidget()
{
    
}

/**
 * @return The whole brain widget
 */
QWidget*
MapSettingsLayerWidget::createWholeBrainWidget()
{
    QLabel* wholeBrainVoxelDrawingModeLabel = new QLabel("Voxel Drawing Mode");
    m_wholeBrainVoxelDrawingModeComboBox = new EnumComboBoxTemplate(this);
    m_wholeBrainVoxelDrawingModeComboBox->setup<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>();
    QObject::connect(m_wholeBrainVoxelDrawingModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &MapSettingsLayerWidget::applyWholeBrainSelections);
    
    const AString warningText("Drawing voxels in Whole Brain view with 3D cubes can be very slow "
                              "and should only be used with a volume that displays a limited number "
                              "of voxels.  One example is a label volume that identifies subcortical "
                              "structures.  Another example is a functional volume that is thresholded "
                              "so that a small number of voxels are displayed.  3D cubes should never "
                              "be used with anatomical volumes.");
    //    QLabel* voxelWarningLabel = new QLabel(WuQtUtilities::createWordWrappedToolTipText(warningText));
    QLabel* voxelWarningLabel = new QLabel(warningText);
    voxelWarningLabel->setWordWrap(true);
    
    QGridLayout* wholeBrainGridLayout = new QGridLayout();
    wholeBrainGridLayout->addWidget(wholeBrainVoxelDrawingModeLabel, 0, 0);
    wholeBrainGridLayout->addWidget(m_wholeBrainVoxelDrawingModeComboBox->getWidget(), 0, 1);
    wholeBrainGridLayout->addWidget(voxelWarningLabel, 1, 0, 1, 2);
    
    QWidget* widget(new QGroupBox("Whole Brain"));
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(wholeBrainGridLayout);
    layout->addStretch();
    
//    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return widget;
}

/**
 * @return The volume to image widget
 */
QWidget*
MapSettingsLayerWidget::createVolumeToImageWidget()
{
    QLabel* mapModeLabel = new QLabel("Mapping Volume to Image Mode");
    m_volumeToImageModeComboBox = new EnumComboBoxTemplate(this);
    m_volumeToImageModeComboBox->setup<VolumeToImageMappingModeEnum, VolumeToImageMappingModeEnum::Enum>();
    QObject::connect(m_volumeToImageModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &MapSettingsLayerWidget::applyVolumeToImageSelections);
    
    QLabel* thicknessLabel(new QLabel("Thickness"));
    m_volumeToImageThicknessSpinBox = new QDoubleSpinBox();
    m_volumeToImageThicknessSpinBox->setMinimum(0.0);
    m_volumeToImageThicknessSpinBox->setMaximum(1000.0);
    m_volumeToImageThicknessSpinBox->setSingleStep(0.1);
    m_volumeToImageThicknessSpinBox->setDecimals(1);
    QObject::connect(m_volumeToImageThicknessSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                     [=]() { applyVolumeToImageSelections(); });
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    gridLayout->addWidget(mapModeLabel, 0, 0);
    gridLayout->addWidget(m_volumeToImageModeComboBox->getWidget(), 0, 1);
    gridLayout->addWidget(thicknessLabel, 1, 0);
    gridLayout->addWidget(m_volumeToImageThicknessSpinBox, 1, 1);
    gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QWidget* widget(new QGroupBox("Volume to Image Mapping"));
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(gridWidget);
    layout->addStretch();
    
//    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return widget;
}

/**
 * Update the content of widget.
 * 
 * @param overlay
 *     Overlay for display in this widget.
 */
void
MapSettingsLayerWidget::updateContent(Overlay* overlay)
{
    m_overlay = overlay;
    
    m_wholeBrainWidget->setEnabled(false);
    if (m_overlay != NULL) {
        m_wholeBrainVoxelDrawingModeComboBox->setSelectedItem<WholeBrainVoxelDrawingMode,
                                                              WholeBrainVoxelDrawingMode::Enum>(m_overlay->getWholeBrainVoxelDrawingMode());
        m_wholeBrainWidget->setEnabled(true);
    }
    
    m_volumeToImageWidget->setEnabled(false);
    if (m_overlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = -1;
        m_overlay->getSelectionData(mapFile,
                                    mapIndex);
        
        m_volumeToImageModeComboBox->setSelectedItem<VolumeToImageMappingModeEnum, VolumeToImageMappingModeEnum::Enum>(overlay->getVolumeToImageMappingMode());
        QSignalBlocker blocker(m_volumeToImageThicknessSpinBox);
        m_volumeToImageThicknessSpinBox->setValue(overlay->getVolumeToImageMappingThickness());
        
        m_volumeToImageWidget->setEnabled(true);
    }
}

/**
 * Called when a whole brain control is changed.
 */
void
MapSettingsLayerWidget::applyWholeBrainSelections()
{
    const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode = m_wholeBrainVoxelDrawingModeComboBox->getSelectedItem<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>();

    if (m_overlay != NULL) {
        m_overlay->setWholeBrainVoxelDrawingMode(wholeBrainVoxelDrawingMode);
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when a volume to image control is changed.
 */
void
MapSettingsLayerWidget::applyVolumeToImageSelections()
{
    if (m_overlay != NULL) {
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        m_overlay->setVolumeToImageMappingMode(m_volumeToImageModeComboBox->getSelectedItem<VolumeToImageMappingModeEnum, VolumeToImageMappingModeEnum::Enum>());
        m_overlay->setVolumeToImageMappingThickness(m_volumeToImageThicknessSpinBox->value());
        
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

