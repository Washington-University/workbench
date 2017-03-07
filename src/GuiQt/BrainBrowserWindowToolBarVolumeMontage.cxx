
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
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "WuQFactory.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarVolumeMontage 
 * \brief Toolbar component for volume montage slice selections
 * \ingroup GuiQt
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
BrainBrowserWindowToolBarVolumeMontage::BrainBrowserWindowToolBarVolumeMontage(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QLabel* rowsLabel = new QLabel("Rows:");
    QLabel* columnsLabel = new QLabel("Cols:");
    QLabel* spacingLabel = new QLabel("Step:");
    
    const int spinBoxWidth = 48;
    
    m_montageRowsSpinBox = WuQFactory::newSpinBox();
    m_montageRowsSpinBox->setRange(1, 20);
    m_montageRowsSpinBox->setMaximumWidth(spinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_montageRowsSpinBox,
                                          "Select the number of rows in montage of volume slices");
    QObject::connect(m_montageRowsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageRowsSpinBoxValueChanged(int)));
    
    m_montageColumnsSpinBox = WuQFactory::newSpinBox();
    m_montageColumnsSpinBox->setRange(1, 20);
    m_montageColumnsSpinBox->setMaximumWidth(spinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_montageColumnsSpinBox,
                                          "Select the number of columns in montage of volume slices");
    QObject::connect(m_montageColumnsSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageColumnsSpinBoxValueChanged(int)));
    
    m_montageSpacingSpinBox = WuQFactory::newSpinBox();
    m_montageSpacingSpinBox->setRange(1, 2500);
    m_montageSpacingSpinBox->setMaximumWidth(spinBoxWidth);
    WuQtUtilities::setToolTipAndStatusTip(m_montageSpacingSpinBox,
                                          "Select the number of slices stepped (incremented) between displayed montage slices");
    QObject::connect(m_montageSpacingSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(montageSpacingSpinBoxValueChanged(int)));
    

    m_montageEnabledAction = WuQtUtilities::createAction("On",
                                                         "View a montage of parallel slices",
                                                         this,
                                                         this,
                                                         SLOT(montageEnabledActionToggled(bool)));
    m_montageEnabledAction->setCheckable(true);
    QToolButton* montageEnabledToolButton = new QToolButton();
    montageEnabledToolButton->setDefaultAction(m_montageEnabledAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(montageEnabledToolButton);

    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->setVerticalSpacing(2);
    gridLayout->addWidget(rowsLabel, 0, 0);
    gridLayout->addWidget(m_montageRowsSpinBox, 0, 1);
    gridLayout->addWidget(columnsLabel, 1, 0);
    gridLayout->addWidget(m_montageColumnsSpinBox, 1, 1);
    gridLayout->addWidget(spacingLabel, 2, 0);
    gridLayout->addWidget(m_montageSpacingSpinBox, 2, 1);
    gridLayout->addWidget(montageEnabledToolButton, 3, 0, 1, 2, Qt::AlignHCenter);
    
    m_volumeMontageWidgetGroup = new WuQWidgetObjectGroup(this);
    m_volumeMontageWidgetGroup->add(m_montageRowsSpinBox);
    m_volumeMontageWidgetGroup->add(m_montageColumnsSpinBox);
    m_volumeMontageWidgetGroup->add(m_montageSpacingSpinBox);
    m_volumeMontageWidgetGroup->add(m_montageEnabledAction);
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
    
    switch (browserTabContent->getSliceDrawingType()) {
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_MONTAGE:
            m_montageEnabledAction->setChecked(true);
            break;
        case VolumeSliceDrawingTypeEnum::VOLUME_SLICE_DRAW_SINGLE:
            m_montageEnabledAction->setChecked(false);
            break;
    }
    m_montageRowsSpinBox->setValue(browserTabContent->getMontageNumberOfRows());
    m_montageColumnsSpinBox->setValue(browserTabContent->getMontageNumberOfColumns());
    m_montageSpacingSpinBox->setValue(browserTabContent->getMontageSliceSpacing());
    
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
    
    btc->setSliceDrawingType(drawingType);
    
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}


/**
 * Called when montage rows spin box value is changed.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageRowsSpinBoxValueChanged(int /*i*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setMontageNumberOfRows(m_montageRowsSpinBox->value());
    
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when montage columns spin box value is changed.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageColumnsSpinBoxValueChanged(int /*i*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    btc->setMontageNumberOfColumns(m_montageColumnsSpinBox->value());
    
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

/**
 * Called when montage spacing spin box value is changed.
 */
void
BrainBrowserWindowToolBarVolumeMontage::montageSpacingSpinBoxValueChanged(int /*i*/)
{
    BrowserTabContent* btc = this->getTabContentFromSelectedTab();
    
    btc->setMontageSliceSpacing(m_montageSpacingSpinBox->value());
    
    this->updateGraphicsWindow();
    this->updateOtherYokedWindows();
}

