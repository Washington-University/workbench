
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

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretMappableDataFile.h"
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
    QLabel* wholeBrainVoxelDrawingModeLabel = new QLabel("Voxel Drawing Mode");
    m_wholeBrainVoxelDrawingModeComboBox = new EnumComboBoxTemplate(this);
    m_wholeBrainVoxelDrawingModeComboBox->setup<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>();
    QObject::connect(m_wholeBrainVoxelDrawingModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(applySelections()));
    
    const AString warningText("Drawing voxels in Whole Brain view with 3D cubes can be very slow "
                              "and should only be used with a volume that displays a limited number "
                              "of voxels.  One example is a label volume that identifies subcortical "
                              "structures.  Another example is a functional volume that is thresholded "
                              "so that a small number of voxels are displayed.  3D cubes should never "
                              "be used with anatomical volumes.");
//    QLabel* voxelWarningLabel = new QLabel(WuQtUtilities::createWordWrappedToolTipText(warningText));
    QLabel* voxelWarningLabel = new QLabel(warningText);
    voxelWarningLabel->setWordWrap(true);
    
    QGroupBox* wholeBraingroupBox = new QGroupBox("Whole Brain");
    wholeBraingroupBox->setFlat(true);
    QGridLayout* wholeBrainGridLayout = new QGridLayout(wholeBraingroupBox);
    wholeBrainGridLayout->addWidget(wholeBrainVoxelDrawingModeLabel, 0, 0);
    wholeBrainGridLayout->addWidget(m_wholeBrainVoxelDrawingModeComboBox->getWidget(), 0, 1);
    wholeBrainGridLayout->addWidget(voxelWarningLabel, 1, 0, 1, 2);
    wholeBraingroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(wholeBraingroupBox);
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
 * Update the content of widget.
 * 
 * @param overlay
 *     Overlay for display in this widget.
 */
void
MapSettingsLayerWidget::updateContent(Overlay* overlay)
{
    m_overlay = overlay;
    
    bool enableWidget = false;
    if (m_overlay != NULL) {
        m_wholeBrainVoxelDrawingModeComboBox->setSelectedItem<WholeBrainVoxelDrawingMode,
                                                              WholeBrainVoxelDrawingMode::Enum>(m_overlay->getWholeBrainVoxelDrawingMode());
        enableWidget = true;
    }
    
    setEnabled(enableWidget);
}

/**
 * Called when a control is changed.
 */
void
MapSettingsLayerWidget::applySelections()
{
    const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode = m_wholeBrainVoxelDrawingModeComboBox->getSelectedItem<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>();

    if (m_overlay != NULL) {
        m_overlay->setWholeBrainVoxelDrawingMode(wholeBrainVoxelDrawingMode);
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}
