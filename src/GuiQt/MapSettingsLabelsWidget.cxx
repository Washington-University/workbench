
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __MAP_SETTINGS_LABELS_WIDGET_DECLARE__
#include "MapSettingsLabelsWidget.h"
#undef __MAP_SETTINGS_LABELS_WIDGET_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventVolumeColoringInvalidate.h"
#include "GiftiLabelTableEditor.h"
#include "LabelDrawingTypeEnum.h"
#include "LabelDrawingProperties.h"
#include "Overlay.h"
#include "VolumeFile.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::MapSettingsLabelsWidget 
 * \brief Labels page for overlay and map settings.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsLabelsWidget::MapSettingsLabelsWidget(QWidget* parent)
: QWidget(parent)
{
    QLabel* drawingTypeLabel = new QLabel("Drawing Type");
    m_drawingTypeComboBox = new EnumComboBoxTemplate(this);
    m_drawingTypeComboBox->setup<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>();
    QObject::connect(m_drawingTypeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(applySelections()));
    
    QLabel* outlineColorLabel = new QLabel("Outline Color");
    m_outlineColorComboBox = new CaretColorEnumComboBox(this);
    QObject::connect(m_outlineColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(applySelections()));
    
    const AString mprOutlineToolTip("When viewing volume slices in MPR mode, the label outlines are "
                                    "computed in 3D using '26-connected' voxel neighbors.  This can "
                                    "be changed to 8-connected voxel neighbors in one of the volumes "
                                    "slice planes which will improve the outlines for that axis view "
                                    "but may also degrade the other axis views.");
    QLabel* mprOutlineModeLabel(new QLabel("MPR Outline Axis Bias"));
    m_mprOutline2dModeComboBox = new EnumComboBoxTemplate(this);
    m_mprOutline2dModeComboBox->setup<VolumeSliceViewPlaneEnum, VolumeSliceViewPlaneEnum::Enum>();
    QObject::connect(m_mprOutline2dModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(applySelections()));
    CaretAssert(m_mprOutline2dModeComboBox->getComboBox()->count() == 4);
    m_mprOutline2dModeComboBox->getComboBox()->setItemText(0, "None");
    WuQtUtilities::setWordWrappedToolTip(m_mprOutline2dModeComboBox->getComboBox(),
                                         mprOutlineToolTip);    

    m_drawMedialWallFilledCheckBox = new QCheckBox("Draw Surface Medial Wall Filled");
    QObject::connect(m_drawMedialWallFilledCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(applySelections()));
    m_drawMedialWallFilledCheckBox->setToolTip(WuQtUtilities::createWordWrappedToolTipText("When an outline type drawing is selected and if the "
                                                                                           "selected map contains vertices identified as the "
                                                                                           "'medial wall', checking this option will fill the medial "
                                                                                           "wall vertices."));
    
    QPushButton* editLabelsPushButton = new QPushButton("Edit Map's Labels");
    QObject::connect(editLabelsPushButton, SIGNAL(clicked()),
                     this, SLOT(editLabelTablePushButtonClicked()));
    editLabelsPushButton->setSizePolicy(QSizePolicy::Fixed,
                                        QSizePolicy::Fixed);
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    gridLayout->addWidget(drawingTypeLabel, 0, 0);
    gridLayout->addWidget(m_drawingTypeComboBox->getWidget(), 0, 1);
    gridLayout->addWidget(outlineColorLabel, 1, 0);
    gridLayout->addWidget(m_outlineColorComboBox->getWidget(), 1, 1);
    gridLayout->addWidget(mprOutlineModeLabel, 2, 0);
    gridLayout->addWidget(m_mprOutline2dModeComboBox->getWidget(), 2, 1);
    gridLayout->addWidget(m_drawMedialWallFilledCheckBox,
                          3, 0, 1, 2, Qt::AlignLeft);
    gridLayout->addWidget(editLabelsPushButton,
                          4, 0, 1, 2, Qt::AlignLeft);
    gridWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(gridWidget);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsLabelsWidget::~MapSettingsLabelsWidget()
{
}

/**
 * Update the content of widget.
 *
 * @param overlay
 *     Overlay for display in this widget.
 */
void
MapSettingsLabelsWidget::updateContent(Overlay* overlay)
{
    m_overlay = overlay;
    
    bool enableWidget = false;
    if (m_overlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = -1;
        m_overlay->getSelectionData(mapFile,
                                  mapIndex);
        
        if (mapFile != NULL) {
            if (mapFile->isMappedWithLabelTable()) {
                const LabelDrawingProperties* labelProps = mapFile->getLabelDrawingProperties();
                m_drawingTypeComboBox->setSelectedItem<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>(labelProps->getDrawingType());
                m_mprOutline2dModeComboBox->setSelectedItem<VolumeSliceViewPlaneEnum, VolumeSliceViewPlaneEnum::Enum>(labelProps->getMprOutline2dMode());
                m_outlineColorComboBox->setSelectedColor(labelProps->getOutlineColor());
                m_drawMedialWallFilledCheckBox->setChecked(labelProps->isDrawMedialWallFilled());
                m_drawMedialWallFilledCheckBox->setEnabled(mapFile->isMedialWallLabelInMapLabelTable(mapIndex));
                
                enableWidget = true;
            }
        }
    }
    
    setEnabled(enableWidget);
}

/**
 * Called when a control is changed.
 */
void
MapSettingsLabelsWidget::applySelections()
{
    if (m_overlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = -1;
        m_overlay->getSelectionData(mapFile,
                                  mapIndex);
        
        if (mapFile != NULL) {
            if (mapFile->isMappedWithLabelTable()) {
                const LabelDrawingTypeEnum::Enum drawType = m_drawingTypeComboBox->getSelectedItem<LabelDrawingTypeEnum, LabelDrawingTypeEnum::Enum>();
                const CaretColorEnum::Enum outlineColor   = m_outlineColorComboBox->getSelectedColor();
                const VolumeSliceViewPlaneEnum::Enum mprOutlineMode(m_mprOutline2dModeComboBox->getSelectedItem<VolumeSliceViewPlaneEnum, VolumeSliceViewPlaneEnum::Enum>());

                LabelDrawingProperties* labelProps = mapFile->getLabelDrawingProperties();
                labelProps->setDrawingType(drawType);
                labelProps->setOutlineColor(outlineColor);
                labelProps->setMprOutline2dMode(mprOutlineMode);
                labelProps->setDrawMedialWallFilled(m_drawMedialWallFilledCheckBox->isChecked());
                
                VolumeFile* volumeFile(dynamic_cast<VolumeFile*>(mapFile));
                if (volumeFile != NULL) {
                    EventManager::get()->sendEvent(EventVolumeColoringInvalidate(volumeFile).getPointer());
                }
                EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
                EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
            }
        }
    }
}

/**
 * Called when the edit label table button is clicked.
 */
void
MapSettingsLabelsWidget::editLabelTablePushButtonClicked()
{
    if (m_overlay != NULL) {
        CaretMappableDataFile* mapFile = NULL;
        int32_t mapIndex = -1;
        m_overlay->getSelectionData(mapFile,
                                    mapIndex);
        
        if (mapFile != NULL) {
            if (mapFile->isMappedWithLabelTable()) {
                if (mapIndex >= 0) {
                    const uint32_t options(GiftiLabelTableEditor::OPTION_ADD_APPLY_BUTTON);
                    GiftiLabelTableEditor labelTableEditor(mapFile,
                                                           mapIndex,
                                                           "Edit Labels",
                                                           options,
                                                           this);
                    labelTableEditor.exec();
                }
            }
        }
    }
}


