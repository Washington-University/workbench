
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_DECLARE__
#include "BrainBrowserWindowToolBarVolumeMontage.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MONTAGE_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "WuQFactory.h"
#include "WuQMacroManager.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarVolumeMontage 
 * \brief Toolbar component for volume montage slice selections
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentObjectName
 *     Name of parent for macros
 * @param parentToolBar
 *     The parent toolbar
 */
BrainBrowserWindowToolBarVolumeMontage::BrainBrowserWindowToolBarVolumeMontage(const QString& parentObjectName,
                                                                               BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    const QString objectNamePrefix(parentObjectName
                                   + ":VolumeSliceMontage:");
    
    const int spinBoxWidth = 48;
    
    QLabel* rowsLabel = new QLabel("Rows:");
    rowsLabel->setToolTip("Select the number of rows in montage of volume slices");
    m_montageRowsSpinBox = WuQFactory::newSpinBox();
    m_montageRowsSpinBox->setRange(1, 20);
    m_montageRowsSpinBox->setMaximumWidth(spinBoxWidth);
    m_montageRowsSpinBox->setToolTip(rowsLabel->toolTip());
    QObject::connect(m_montageRowsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageRowsSpinBoxValueChanged(int)));
    m_montageRowsSpinBox->setObjectName(objectNamePrefix
                                        + "Rows");
    WuQMacroManager::instance()->addMacroSupportToObject(m_montageRowsSpinBox,
                                                         "Set volume montage rows");
    
    
    QLabel* columnsLabel = new QLabel("Cols:");
    columnsLabel->setToolTip("Select the number of columns in montage of volume slices");
    m_montageColumnsSpinBox = WuQFactory::newSpinBox();
    m_montageColumnsSpinBox->setRange(1, 20);
    m_montageColumnsSpinBox->setMaximumWidth(spinBoxWidth);
    m_montageColumnsSpinBox->setToolTip(columnsLabel->toolTip());
    QObject::connect(m_montageColumnsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageColumnsSpinBoxValueChanged(int)));
    m_montageColumnsSpinBox->setObjectName(objectNamePrefix
                                           + "Columns");
    WuQMacroManager::instance()->addMacroSupportToObject(m_montageColumnsSpinBox,
                                                         "Set volume montage columns");
    
    QLabel* spacingLabel = new QLabel("Step:");
    spacingLabel->setToolTip("Select the number of slices stepped (incremented) between displayed montage slices");
    m_montageSpacingSpinBox = WuQFactory::newSpinBox();
    m_montageSpacingSpinBox->setRange(1, 2500);
    m_montageSpacingSpinBox->setMaximumWidth(spinBoxWidth);
    m_montageSpacingSpinBox->setToolTip(spacingLabel->toolTip());
    QObject::connect(m_montageSpacingSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageSpacingSpinBoxValueChanged(int)));
    m_montageSpacingSpinBox->setObjectName(objectNamePrefix
                                           + "Step");
    WuQMacroManager::instance()->addMacroSupportToObject(m_montageSpacingSpinBox,
                                                         "Set volume montage spacing");
    
    m_showSliceCoordinateAction = new QAction("XYZ", this);
    m_showSliceCoordinateAction->setText("XYZ");
    m_showSliceCoordinateAction->setCheckable(true);
    m_showSliceCoordinateAction->setToolTip("Show coordinates on slices");
    QObject::connect(m_showSliceCoordinateAction, &QAction::triggered,
                     this, &BrainBrowserWindowToolBarVolumeMontage::showSliceCoordinateToolButtonClicked);
    m_showSliceCoordinateAction->setObjectName(objectNamePrefix
                                               + "ShowCoordinateOnSlice");
    WuQMacroManager::instance()->addMacroSupportToObject(m_showSliceCoordinateAction,
                                                         "Enable coordinates in volume montage");

    QToolButton* showSliceCoordToolButton = new QToolButton;
    showSliceCoordToolButton->setDefaultAction(m_showSliceCoordinateAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(showSliceCoordToolButton);

    QLabel* decimalsLabel = new QLabel("Prec:");
    decimalsLabel->setToolTip("Digits right of decimal in slice coordinates");
    m_sliceCoordinatePrecisionSpinBox = WuQFactory::newSpinBox();
    m_sliceCoordinatePrecisionSpinBox->setRange(0, 10);
    m_sliceCoordinatePrecisionSpinBox->setMaximumWidth(spinBoxWidth);
    m_sliceCoordinatePrecisionSpinBox->setToolTip(decimalsLabel->toolTip());
    QObject::connect(m_sliceCoordinatePrecisionSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarVolumeMontage::slicePrecisionSpinBoxValueChanged);
    m_sliceCoordinatePrecisionSpinBox->setObjectName(objectNamePrefix
                                                     + "Precision");
    WuQMacroManager::instance()->addMacroSupportToObject(m_sliceCoordinatePrecisionSpinBox,
                                                         "Set volume montage coordinate precision");


    QToolButton* montageEnabledToolButton = new QToolButton();
    m_montageEnabledAction = WuQtUtilities::createAction("On",
                                                         "View a montage of parallel slices",
                                                         montageEnabledToolButton,
                                                         this,
                                                         SLOT(montageEnabledActionToggled(bool)));
    m_montageEnabledAction->setCheckable(true);
    montageEnabledToolButton->setDefaultAction(m_montageEnabledAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(montageEnabledToolButton);
    m_montageEnabledAction->setObjectName(objectNamePrefix
                                          + "Enable");
    WuQMacroManager::instance()->addMacroSupportToObject(m_montageEnabledAction,
                                                         "Enable volume slice montage");

    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->setVerticalSpacing(2);
    gridLayout->addWidget(rowsLabel, 0, 0);
    gridLayout->addWidget(m_montageRowsSpinBox, 0, 1);
    gridLayout->addWidget(columnsLabel, 1, 0);
    gridLayout->addWidget(m_montageColumnsSpinBox, 1, 1);
    gridLayout->addWidget(spacingLabel, 2, 0);
    gridLayout->addWidget(m_montageSpacingSpinBox, 2, 1);
    gridLayout->addWidget(decimalsLabel, 3, 0);
    gridLayout->addWidget(m_sliceCoordinatePrecisionSpinBox, 3, 1);
    gridLayout->addWidget(showSliceCoordToolButton, 4, 0);
    gridLayout->addWidget(montageEnabledToolButton, 4, 1);
    
    m_volumeMontageWidgetGroup = new WuQWidgetObjectGroup(this);
    m_volumeMontageWidgetGroup->add(m_montageRowsSpinBox);
    m_volumeMontageWidgetGroup->add(m_montageColumnsSpinBox);
    m_volumeMontageWidgetGroup->add(m_montageSpacingSpinBox);
    m_volumeMontageWidgetGroup->add(m_montageEnabledAction);
    m_volumeMontageWidgetGroup->add(m_showSliceCoordinateAction);
    m_volumeMontageWidgetGroup->add(m_sliceCoordinatePrecisionSpinBox);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarVolumeMontage::~BrainBrowserWindowToolBarVolumeMontage()
{
}

/**
 * Update the surface montage options widget.
 *
 * @param browserTabContent
 *   The active model display controller (may be NULL).
 */
void
BrainBrowserWindowToolBarVolumeMontage::updateContent(BrowserTabContent* browserTabContent)
{
    m_volumeMontageWidgetGroup->blockAllSignals(true);
    
    switch (browserTabContent->getVolumeSliceDrawingType()) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            m_montageEnabledAction->setChecked(true);
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            m_montageEnabledAction->setChecked(false);
            break;
    }
    m_montageRowsSpinBox->setValue(browserTabContent->getVolumeMontageNumberOfRows());
    m_montageColumnsSpinBox->setValue(browserTabContent->getVolumeMontageNumberOfColumns());
    m_montageSpacingSpinBox->setValue(browserTabContent->getVolumeMontageSliceSpacing());
    
    m_showSliceCoordinateAction->setChecked(browserTabContent->isVolumeMontageAxesCoordinatesDisplayed());
    m_sliceCoordinatePrecisionSpinBox->setValue(browserTabContent->getVolumeMontageCoordinatePrecision());
    
    m_volumeMontageWidgetGroup->blockAllSignals(false);
}

/**
 * Called when montage enabled button toggled.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageEnabledActionToggled(bool)
{
    VolumeSliceDrawingTypeEnum::Enum drawingType = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE;
    
    if (m_montageEnabledAction->isChecked()) {
        drawingType = VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE;
    }
    
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setVolumeSliceDrawingType(drawingType);
    
    this->updateGraphicsWindowAndYokedWindows();
}


/**
 * Called when montage rows spin box value is changed.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageRowsSpinBoxValueChanged(int /*i*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setVolumeMontageNumberOfRows(m_montageRowsSpinBox->value());
    
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when montage columns spin box value is changed.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageColumnsSpinBoxValueChanged(int /*i*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setVolumeMontageNumberOfColumns(m_montageColumnsSpinBox->value());
    
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when montage spacing spin box value is changed.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageSpacingSpinBoxValueChanged(int /*i*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setVolumeMontageSliceSpacing(m_montageSpacingSpinBox->value());
    
    this->updateGraphicsWindowAndYokedWindows();
}

/**
 * Called when show slice coordinate tool button is clicked
 *
 * @param checked
 *     New checked status
 */
void
BrainBrowserWindowToolBarVolumeMontage::showSliceCoordinateToolButtonClicked(bool checked)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setVolumeMontageAxesCoordinatesDisplayed(checked);
    
    this->updateGraphicsWindowAndYokedWindows();
}


/**
 * Called when montage slice precision spin box value is changed.
 *
 * @param value
 *     New value
 */
void
BrainBrowserWindowToolBarVolumeMontage::slicePrecisionSpinBoxValueChanged(int value)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setVolumeMontageCoordinatePrecision(value);
    
    this->updateGraphicsWindowAndYokedWindows();
}

