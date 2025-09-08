
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __PREFERENCES_DEVELOP_OPTIONS_WIDGET_DECLARE__
#include "PreferencesDevelopOptionsWidget.h"
#undef __PREFERENCES_DEVELOP_OPTIONS_WIDGET_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "Brain.h"
#include "BrainOpenGLMediaDrawing.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "EventVolumeColoringInvalidate.h"
#include "GraphicsTextureMagnificationFilterEnum.h"
#include "GraphicsTextureMinificationFilterEnum.h"
#include "GuiManager.h"
#include "PreferencesDialog.h"
#include "SessionManager.h"
#include "BrainOpenGLVolumeObliqueSliceDrawing.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::PreferencesDevelopOptionsWidget 
 * \brief Widget for Develop Preferences
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
PreferencesDevelopOptionsWidget::PreferencesDevelopOptionsWidget(QWidget* parent)
: QWidget(parent)
{
    std::vector<DeveloperFlagsEnum::Enum> developerFlags;
    DeveloperFlagsEnum::getAllEnumsSortedByGuiName(developerFlags);
    
    QGridLayout* gridLayout = new QGridLayout();
    if ( ! developerFlags.empty()) {
        for (const auto flag : developerFlags) {
            if (DeveloperFlagsEnum::isCheckable(flag)) {
                if (flag == DeveloperFlagsEnum::DEVELOPER_FLAG_UNUSED) {
                    continue;
                }

                WuQTrueFalseComboBox* comboBox = new WuQTrueFalseComboBox("On",
                                                                          "Off",
                                                                          this);
                QObject::connect(comboBox, &WuQTrueFalseComboBox::statusChanged,
                                 [=](const bool status) { this->developerFlagSelected(flag,
                                                                                      status); } );
                const AString toolTip(WuQtUtilities::createWordWrappedToolTipText(DeveloperFlagsEnum::toToolTip(flag)));
                comboBox->getWidget()->setToolTip(toolTip);
                
                QLabel* label = PreferencesDialog::addWidgetToLayout(gridLayout,
                                                                     DeveloperFlagsEnum::toGuiName(flag),
                                                                     comboBox->getWidget());
                label->setToolTip(toolTip);

                m_developerFlagsMap.insert(std::make_pair(comboBox,
                                                          flag));
            }
        }
    }

    /*
     * Oblique voxel interpolation type
     */
    m_obliqueVolumeInterpolationTypeComboBox = new EnumComboBoxTemplate(this);
    m_obliqueVolumeInterpolationTypeComboBox->setup<VoxelInterpolationTypeEnum,VoxelInterpolationTypeEnum::Enum>();
    QObject::connect(m_obliqueVolumeInterpolationTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &PreferencesDevelopOptionsWidget::obliqueVolumeInterpolationTypeComboBoxActivated);
    WuQtUtilities::setWordWrappedToolTip(m_obliqueVolumeInterpolationTypeComboBox->getWidget(),
                                         "Voxel Interpolation Type.  Cubic is best quality and slower; Enclosing Voxel is fastest");
    PreferencesDialog::addWidgetToLayout(gridLayout,
                                         "Volume: Oblique Interpolation Type",
                                         m_obliqueVolumeInterpolationTypeComboBox->getWidget());

    /*
     * Oblique voxel scaling
     */
    m_obliqueVoxelScalingSpinBox = new QDoubleSpinBox();
    m_obliqueVoxelScalingSpinBox->setRange(0.25, 100.0);
    m_obliqueVoxelScalingSpinBox->setSingleStep(0.25);
    m_obliqueVoxelScalingSpinBox->setToolTip("Scale oblique voxel size; larger is faster but lower quality");
    PreferencesDialog::addWidgetToLayout(gridLayout, "Volume: Oblique Voxel Scaling", m_obliqueVoxelScalingSpinBox);
    QObject::connect(m_obliqueVoxelScalingSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &PreferencesDevelopOptionsWidget::obliqueVoxelScalingSpinBoxValueChanged);
    
    QLabel* notesLabel = new QLabel("Note: These developer options are NOT saved in the user's preferences.  "
                                    "Therefore, any desired changes to these selections must be made each time "
                                    "the application is started.");
    notesLabel->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(gridLayout);
    layout->addStretch(5);
    layout->addWidget(notesLabel);
    layout->addStretch();
}

/**
 * Destructor.
 */
PreferencesDevelopOptionsWidget::~PreferencesDevelopOptionsWidget()
{
}

/**
 * Called when a developer flag is changed
 * @param flag
 *    Developer flag that is changed
 * @param status
 *    New on/off status
 */
void
PreferencesDevelopOptionsWidget::developerFlagSelected(const DeveloperFlagsEnum::Enum flag,
                                                       const bool status)
{
    DeveloperFlagsEnum::setFlag(flag,
                                status);
    if (flag == DeveloperFlagsEnum::DEVELOPER_FLAG_HISTOLOGY_CORRECT_FOR_NON_LINEAR_DISTORTION) {
        EventManager::get()->sendEvent(EventVolumeColoringInvalidate().getPointer());
    }
    updateGraphicsAndUserInterface();
}

/**
 * Update the graphics and the user interface
 */
void
PreferencesDevelopOptionsWidget::updateGraphicsAndUserInterface()
{
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Update the widget
 * @param preferences
 *    The preferences
 */
void
PreferencesDevelopOptionsWidget::updateContent(CaretPreferences* preferences)
{
    m_preferences = preferences;
    CaretAssert(m_preferences);
    
    for (auto& comboBoxFlag : m_developerFlagsMap) {
        WuQTrueFalseComboBox* comboBox(comboBoxFlag.first);
        DeveloperFlagsEnum::Enum flag(comboBoxFlag.second);
        
        comboBox->setStatus(DeveloperFlagsEnum::isFlag(flag));
    }
    
    m_obliqueVolumeInterpolationTypeComboBox->setSelectedItem<VoxelInterpolationTypeEnum,VoxelInterpolationTypeEnum::Enum>(BrainOpenGLVolumeObliqueSliceDrawing::getVoxelInterpolationType());
    QSignalBlocker voxelScaleBlocker(m_obliqueVoxelScalingSpinBox);
    m_obliqueVoxelScalingSpinBox->setValue(BrainOpenGLVolumeObliqueSliceDrawing::getVoxelStepScaling());
}

/**
 * Called when oblique volume interpolation type changed
 */
void
PreferencesDevelopOptionsWidget::obliqueVolumeInterpolationTypeComboBoxActivated()
{
    const VoxelInterpolationTypeEnum::Enum interpType = m_obliqueVolumeInterpolationTypeComboBox->getSelectedItem<VoxelInterpolationTypeEnum,VoxelInterpolationTypeEnum::Enum>();
    BrainOpenGLVolumeObliqueSliceDrawing::setVoxelInterpolationType(interpType);
    updateGraphicsAndUserInterface();
}

/**
 * Called when oblique voxel scaling value changed
 * @param value
 *    New value
 */
void
PreferencesDevelopOptionsWidget::obliqueVoxelScalingSpinBoxValueChanged(double value)
{
    BrainOpenGLVolumeObliqueSliceDrawing::setVoxelStepScaling(value);
    updateGraphicsAndUserInterface();
}
