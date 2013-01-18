
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __MAP_SETTINGS_OVERLAY_WIDGET_DECLARE__
#include "MapSettingsOverlayWidget.h"
#undef __MAP_SETTINGS_OVERLAY_WIDGET_DECLARE__

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "Overlay.h"

using namespace caret;


    
/**
 * \class caret::MapSettingsOverlayWidget 
 * \brief Contains user-interface components for overlay parameters.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsOverlayWidget::MapSettingsOverlayWidget(QWidget* parent)
: QWidget(parent)
{
    QLabel* wholeBrainVoxelDrawingModeLabel = new QLabel("Voxel Drawing Mode");
    m_wholeBrainVoxelDrawingModeComboBox = new EnumComboBoxTemplate(this);
    m_wholeBrainVoxelDrawingModeComboBox->setup<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>();
    QObject::connect(m_wholeBrainVoxelDrawingModeComboBox, SIGNAL(itemSelected()),
                     this, SLOT(applySelections()));
    
    QGroupBox* wholeBrainGroupBox = new QGroupBox("Whole Brain");
    QGridLayout* wholeBrainGridLayout = new QGridLayout(wholeBrainGroupBox);
    wholeBrainGridLayout->addWidget(wholeBrainVoxelDrawingModeLabel, 0, 0);
    wholeBrainGridLayout->addWidget(m_wholeBrainVoxelDrawingModeComboBox->getWidget(), 0, 1);
    wholeBrainGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(wholeBrainGroupBox);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsOverlayWidget::~MapSettingsOverlayWidget()
{
    
}

/**
 * Update the content of widget.
 * 
 * @param overlay
 *     Overlay for display in this widget.
 */
void
MapSettingsOverlayWidget::updateContent(Overlay* overlay)
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
MapSettingsOverlayWidget::applySelections()
{
    const WholeBrainVoxelDrawingMode::Enum wholeBrainVoxelDrawingMode = m_wholeBrainVoxelDrawingModeComboBox->getSelectedItem<WholeBrainVoxelDrawingMode, WholeBrainVoxelDrawingMode::Enum>();

    if (m_overlay != NULL) {
        m_overlay->setWholeBrainVoxelDrawingMode(wholeBrainVoxelDrawingMode);
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
