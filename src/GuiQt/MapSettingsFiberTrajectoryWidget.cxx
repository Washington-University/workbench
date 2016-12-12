
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

#define __MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET_DECLARE__
#include "MapSettingsFiberTrajectoryWidget.h"
#undef __MAP_SETTINGS_FIBER_TRAJECTORY_WIDGET_DECLARE__

#include <limits>

#include <QButtonGroup>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>

#include "CiftiFiberTrajectoryFile.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FiberTrajectoryColorModel.h"
#include "FiberTrajectoryMapProperties.h"
#include "GuiManager.h"
#include "WuQFactory.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::MapSettingsFiberTrajectoryWidget 
 * \brief View/Controller for fiber trajectories
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param browserWindowIndex
 *   Index of browser window in which this view controller is displayed.
 * @param parent
 *   Parent of this object.
 */
MapSettingsFiberTrajectoryWidget::MapSettingsFiberTrajectoryWidget(QWidget* parent)
: QWidget(parent)
{
    m_updateInProgress = true;
 
    QWidget* attributesWidget  = createAttributesWidget();
    QWidget* displayModeWidget = createDisplayModeWidget();
    QWidget* dataMappingWidget = createDataMappingWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(attributesWidget,
                      0,
                      Qt::AlignLeft);
    layout->addWidget(displayModeWidget,
                      0,
                      Qt::AlignLeft);
    layout->addWidget(dataMappingWidget,
                      0,
                      Qt::AlignLeft);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsFiberTrajectoryWidget::~MapSettingsFiberTrajectoryWidget()
{
}

QWidget*
MapSettingsFiberTrajectoryWidget::createAttributesWidget()
{
    QLabel* colorLabel = new QLabel("Coloring: ");
    m_colorSelectionComboBox = WuQFactory::newComboBoxSignalInt(this,
                                                                SLOT(processAttributesChanges()));
    
    QGroupBox* attributesGroupBox = new QGroupBox("Attributes");
    QGridLayout* attributesGridLayout = new QGridLayout(attributesGroupBox);
    int row = 0;
    attributesGridLayout->addWidget(colorLabel, row, 0);
    attributesGridLayout->addWidget(m_colorSelectionComboBox, row, 1);
    
    return attributesGroupBox;
}


/**
 * @return Create and return the display mode widget.
 */
QWidget*
MapSettingsFiberTrajectoryWidget::createDisplayModeWidget()
{
    m_displayModeButtonGroup = new QButtonGroup(this);
    QObject::connect(m_displayModeButtonGroup, SIGNAL(buttonClicked(int)),
                     this, SLOT(processAttributesChanges()));
    
    std::vector<FiberTrajectoryDisplayModeEnum::Enum> displayModes;
    FiberTrajectoryDisplayModeEnum::getAllEnums(displayModes);
    const int32_t numDisplayModes = static_cast<int32_t>(displayModes.size());
    for (int32_t i = 0; i < numDisplayModes; i++) {
        const FiberTrajectoryDisplayModeEnum::Enum mode = displayModes[i];
        QRadioButton* radioButton = new QRadioButton(FiberTrajectoryDisplayModeEnum::toGuiName(mode));
        m_displayModeButtonGroup->addButton(radioButton, i);
        m_displayModeRadioButtons.push_back(radioButton);
        m_displayModeRadioButtonData.push_back(mode);
    }
    
    QGroupBox* modeGroupBox = new QGroupBox("Display Mode");
    QVBoxLayout* modeGroupLayout = new QVBoxLayout(modeGroupBox);
    for (int32_t i = 0; i < numDisplayModes; i++) {
        modeGroupLayout->addWidget(m_displayModeRadioButtons[i]);
    }
    
    return modeGroupBox;
}


/**
 * @return Create and return the data mapping widget.
 */
QWidget*
MapSettingsFiberTrajectoryWidget::createDataMappingWidget()
{
    const int spinBoxWidth = 85;
    
    m_proportionStreamlineSpinBox = WuQFactory::newDoubleSpinBox();
    m_proportionStreamlineSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_proportionStreamlineSpinBox->setSingleStep(5);
    m_proportionStreamlineSpinBox->setFixedWidth(spinBoxWidth);
    m_proportionStreamlineSpinBox->setDecimals(3);
    m_proportionStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                             "streamlines is greater than or equal to this value");
    QObject::connect(m_proportionStreamlineSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_proportionMinimumSpinBox = WuQFactory::newDoubleSpinBox();
    m_proportionMinimumSpinBox->setRange(0.0, 1.0);
    m_proportionMinimumSpinBox->setDecimals(3);
    m_proportionMinimumSpinBox->setSingleStep(0.05);
    m_proportionMinimumSpinBox->setFixedWidth(spinBoxWidth);
    m_proportionMinimumSpinBox->setToolTip("If the proportion for an axis is less than or equal\n"
                                           "to this value, the opacity will be zero (clear)");
    QObject::connect(m_proportionMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_proportionMaximumSpinBox = WuQFactory::newDoubleSpinBox();
    m_proportionMaximumSpinBox->setRange(0.0, 1.0);
    m_proportionMaximumSpinBox->setDecimals(3);
    m_proportionMaximumSpinBox->setSingleStep(0.05);
    m_proportionMaximumSpinBox->setFixedWidth(spinBoxWidth);
    m_proportionMaximumSpinBox->setToolTip("If the proportion for an axis is greater than or equal\n"
                                           "to this value, the opacity will be one (opaque)");
    QObject::connect(m_proportionMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    
    m_countStreamlineSpinBox = WuQFactory::newDoubleSpinBox();
    m_countStreamlineSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_countStreamlineSpinBox->setDecimals(3);
    m_countStreamlineSpinBox->setSingleStep(5);
    m_countStreamlineSpinBox->setFixedWidth(spinBoxWidth);
    m_countStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                              "streamlines is greater than or equal to this value");
    QObject::connect(m_countStreamlineSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_countMinimumSpinBox = WuQFactory::newDoubleSpinBox();
    m_countMinimumSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_countMinimumSpinBox->setDecimals(3);
    m_countMinimumSpinBox->setSingleStep(5);
    m_countMinimumSpinBox->setFixedWidth(spinBoxWidth);
    m_countMinimumSpinBox->setToolTip("If the number of fibers for an axis is less than or equal\n"
                                           "to this value, the opacity will be zero (clear)");
    QObject::connect(m_countMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_countMaximumSpinBox = WuQFactory::newDoubleSpinBox();
    m_countMaximumSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_countMaximumSpinBox->setDecimals(3);
    m_countMaximumSpinBox->setSingleStep(5);
    m_countMaximumSpinBox->setFixedWidth(spinBoxWidth);
    m_countMaximumSpinBox->setToolTip("If the number of fibers for an axis is greater than or equal\n"
                                           "to this value, the opacity will be one (opaque)");
    QObject::connect(m_countMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    
    
    m_distanceStreamlineSpinBox = WuQFactory::newDoubleSpinBox();
    m_distanceStreamlineSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_distanceStreamlineSpinBox->setDecimals(3);
    m_distanceStreamlineSpinBox->setSingleStep(5);
    m_distanceStreamlineSpinBox->setFixedWidth(spinBoxWidth);
    m_distanceStreamlineSpinBox->setToolTip("A fiber is displayed only if the total number of its\n"
                                         "streamlines is greater than or equal to this value");
    QObject::connect(m_distanceStreamlineSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_distanceMinimumSpinBox = WuQFactory::newDoubleSpinBox();
    m_distanceMinimumSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_distanceMinimumSpinBox->setDecimals(3);
    m_distanceMinimumSpinBox->setSingleStep(5);
    m_distanceMinimumSpinBox->setFixedWidth(spinBoxWidth);
    m_distanceMinimumSpinBox->setToolTip("If count times distance for an axis is less than or equal\n"
                                      "to this value, the opacity will be zero (clear)");
    QObject::connect(m_distanceMinimumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    m_distanceMaximumSpinBox = WuQFactory::newDoubleSpinBox();
    m_distanceMaximumSpinBox->setRange(0, std::numeric_limits<float>::max());
    m_distanceMaximumSpinBox->setDecimals(3);
    m_distanceMaximumSpinBox->setSingleStep(5);
    m_distanceMaximumSpinBox->setFixedWidth(spinBoxWidth);
    m_distanceMaximumSpinBox->setToolTip("If the count times distance for an axis is greater than or equal\n"
                                      "to this value, the opacity will be one (opaque)");
    QObject::connect(m_distanceMaximumSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(processAttributesChanges()));
    
    QGroupBox* dataMappingGroupBox = new QGroupBox("Data Mapping");
    QGridLayout* dataMappingLayout = new QGridLayout(dataMappingGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(dataMappingLayout, 4, 2);
    int row = dataMappingLayout->rowCount();
    
    int columnCounter = 0;
    const int COLUMN_LABELS    = columnCounter++;
    const int COLUMN_THRESHOLD = columnCounter++;
    const int COLUMN_MINIMUM   = columnCounter++;
    const int COLUMN_MAXIMUM   = columnCounter++;
    
    dataMappingLayout->addWidget(new QLabel("<html>Display<br>Mode<html>"), row, COLUMN_LABELS, Qt::AlignLeft);
    dataMappingLayout->addWidget(new QLabel("<html>Streamline<br>Threshold<html>"), row, COLUMN_THRESHOLD, Qt::AlignLeft);
    dataMappingLayout->addWidget(new QLabel("<html>Map to<br>Clear<html>"), row, COLUMN_MINIMUM, Qt::AlignLeft);
    dataMappingLayout->addWidget(new QLabel("<html>Map to<br>Opaque<html>"), row, COLUMN_MAXIMUM, Qt::AlignLeft);
    row++;
    
    dataMappingLayout->addWidget(new QLabel("Absolute"), row, COLUMN_LABELS, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_countStreamlineSpinBox, row, COLUMN_THRESHOLD, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_countMinimumSpinBox, row, COLUMN_MINIMUM, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_countMaximumSpinBox, row, COLUMN_MAXIMUM, Qt::AlignHCenter);
    row++;

    dataMappingLayout->addWidget(new QLabel("Distance"), row, COLUMN_LABELS, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_distanceStreamlineSpinBox, row, COLUMN_THRESHOLD, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_distanceMinimumSpinBox, row, COLUMN_MINIMUM, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_distanceMaximumSpinBox, row, COLUMN_MAXIMUM, Qt::AlignHCenter);
    row++;

    dataMappingLayout->addWidget(new QLabel("Proportion"), row, COLUMN_LABELS, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_proportionStreamlineSpinBox, row, COLUMN_THRESHOLD, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_proportionMinimumSpinBox, row, COLUMN_MINIMUM, Qt::AlignHCenter);
    dataMappingLayout->addWidget(m_proportionMaximumSpinBox, row, COLUMN_MAXIMUM, Qt::AlignHCenter);
    row++;
    
    return dataMappingGroupBox;
}

/**
 * Called when a widget on the attributes page has
 * its value changed.
 */
void
MapSettingsFiberTrajectoryWidget::processAttributesChanges()
{
    if (m_updateInProgress) {
        return;
    }
    if (m_fiberTrajectoryFile == NULL) {
        return;
    }
    
    FiberTrajectoryMapProperties* ftmp = m_fiberTrajectoryFile->getFiberTrajectoryMapProperties();
    
    const int32_t selectedColorIndex = m_colorSelectionComboBox->currentIndex();
    if (selectedColorIndex >= 0) {
        void* ptr = m_colorSelectionComboBox->itemData(selectedColorIndex,
                                                       Qt::UserRole).value<void*>();
        const FiberTrajectoryColorModel::Item* item = (FiberTrajectoryColorModel::Item*)ptr;
        ftmp->getFiberTrajectoryColorModel()->setSelectedItem(item);
    }
    
    const int32_t selectedModeRadioButtonIndex = m_displayModeButtonGroup->checkedId();
    const FiberTrajectoryDisplayModeEnum::Enum displayMode = m_displayModeRadioButtonData[selectedModeRadioButtonIndex];
    ftmp->setDisplayMode(displayMode);
    
    ftmp->setProportionStreamline(m_proportionStreamlineSpinBox->value());;
    
    ftmp->setProportionMinimumOpacity(m_proportionMinimumSpinBox->value());
    
    ftmp->setProportionMaximumOpacity(m_proportionMaximumSpinBox->value());
    
    ftmp->setCountStreamline(m_countStreamlineSpinBox->value());
    ftmp->setCountMaximumOpacity(m_countMaximumSpinBox->value());
    ftmp->setCountMinimumOpacity(m_countMinimumSpinBox->value());
    
    ftmp->setDistanceStreamline(m_distanceStreamlineSpinBox->value());
    ftmp->setDistanceMaximumOpacity(m_distanceMaximumSpinBox->value());
    ftmp->setDistanceMinimumOpacity(m_distanceMinimumSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Update with the given fiber trajectory file.
 */
void
MapSettingsFiberTrajectoryWidget::updateEditor(CiftiFiberTrajectoryFile* fiberTrajectoryFile)
{
    m_fiberTrajectoryFile = fiberTrajectoryFile;
    
    if (m_fiberTrajectoryFile == NULL) {
        return;
    }

    m_updateInProgress = true;
    
    FiberTrajectoryMapProperties* ftmp = m_fiberTrajectoryFile->getFiberTrajectoryMapProperties();
    
    FiberTrajectoryColorModel* colorModel = ftmp->getFiberTrajectoryColorModel();
    std::vector<FiberTrajectoryColorModel::Item*> colorItems = colorModel->getValidItems();
    const FiberTrajectoryColorModel::Item* selectedItem = colorModel->getSelectedItem();
    const int32_t numColorItems = static_cast<int32_t>(colorItems.size());
    
    m_colorSelectionComboBox->blockSignals(true);
    m_colorSelectionComboBox->clear();
    int32_t defaultIndex = 0;
    for (int32_t i = 0; i < numColorItems; i++) {
        FiberTrajectoryColorModel::Item* item = colorItems[i];
        if (item == selectedItem) {
            defaultIndex = i;
        }
        m_colorSelectionComboBox->addItem(item->getName(),
                                          qVariantFromValue((void*)item));
    }
    if ((defaultIndex >= 0)
        && (defaultIndex < m_colorSelectionComboBox->count())) {
        m_colorSelectionComboBox->setCurrentIndex(defaultIndex);
    }
    m_colorSelectionComboBox->blockSignals(false);
    
    const FiberTrajectoryDisplayModeEnum::Enum selectedDisplayMode = ftmp->getDisplayMode();
    const int32_t numDisplayModeRadioButtons = m_displayModeButtonGroup->buttons().size();
    for (int32_t i = 0; i < numDisplayModeRadioButtons; i++) {
        if (m_displayModeRadioButtonData[i] == selectedDisplayMode) {
            m_displayModeRadioButtons[i]->setChecked(true);
            break;
        }
    }
    
    /*
     * Update the attributes
     */
    m_proportionStreamlineSpinBox->setValue(ftmp->getProportionStreamline());
    m_proportionMaximumSpinBox->setValue(ftmp->getProportionMaximumOpacity());
    m_proportionMinimumSpinBox->setValue(ftmp->getProportionMinimumOpacity());
    
    m_countStreamlineSpinBox->setValue(ftmp->getCountStreamline());
    m_countMaximumSpinBox->setValue(ftmp->getCountMaximumOpacity());
    m_countMinimumSpinBox->setValue(ftmp->getCountMinimumOpacity());
    
    m_distanceStreamlineSpinBox->setValue(ftmp->getDistanceStreamline());
    m_distanceMaximumSpinBox->setValue(ftmp->getDistanceMaximumOpacity());
    m_distanceMinimumSpinBox->setValue(ftmp->getDistanceMinimumOpacity());
    
    m_updateInProgress = false;
}


/**
 * Update the fiber trajectory widget.
 */
void
MapSettingsFiberTrajectoryWidget::updateWidget()
{
    updateEditor(m_fiberTrajectoryFile);
}
