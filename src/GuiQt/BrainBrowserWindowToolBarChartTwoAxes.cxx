
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

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_DECLARE__
#include "BrainBrowserWindowToolBarChartTwoAxes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlaySet.h"
#include "EnumComboBoxTemplate.h"
#include "CaretMappableDataFile.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChartTwo.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartTwoAxes 
 * \brief Controls for chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *   The parent toolbar.
 */
BrainBrowserWindowToolBarChartTwoAxes::BrainBrowserWindowToolBarChartTwoAxes(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    /*
     * Axes selection
     */
    m_axisComboBox = new EnumComboBoxTemplate(this);
    m_axisComboBox->setup<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>();
    QObject::connect(m_axisComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisChanged);
    m_axisComboBox->getWidget()->setToolTip("Choose axis for editing");
    
    /*
     * Controls for axis parameters
     */
    m_axisNameToolButton = new QToolButton();
    m_axisNameToolButton->setText("Edit Axis Title");
    QObject::connect(m_axisNameToolButton, &QToolButton::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisNameToolButtonClicked);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_axisNameToolButton);
    m_axisNameToolButton->setToolTip("Edit name lable displayed in axis");
    
    m_autoUserRangeComboBox = new EnumComboBoxTemplate(this);
    m_autoUserRangeComboBox->setup<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>();
    QObject::connect(m_autoUserRangeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChanged);
    m_autoUserRangeComboBox->getWidget()->setToolTip("Choose auto or user axis scaling");
    
    const double bigValue = 999999.0;
    m_userMinimumValueSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigValue, bigValue, 1.0, 1);
    QObject::connect(m_userMinimumValueSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedDouble);
    m_userMinimumValueSpinBox->setToolTip("Set user scaling axis minimum value");
    
    m_userMaximumValueSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(-bigValue, bigValue, 1.0, 1);
    QObject::connect(m_userMaximumValueSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedDouble);
    m_userMaximumValueSpinBox->setToolTip("Set user scaling axis maximum value");
    
    m_showTickMarksCheckBox = new QCheckBox("Show Ticks");
    QObject::connect(m_showTickMarksCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool);
    m_showTickMarksCheckBox->setToolTip("Show ticks along the axis");
    
    m_userNumericFormatComboBox = new EnumComboBoxTemplate(this);
    m_userNumericFormatComboBox->setup<NumericFormatModeEnum, NumericFormatModeEnum::Enum>();
    QObject::connect(m_userNumericFormatComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChanged);
    m_userNumericFormatComboBox->getWidget()->setToolTip("Choose format of axis scale numeric values");
    
    m_userDigitsRightOfDecimalSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0, 10, 1);
    QObject::connect(m_userDigitsRightOfDecimalSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedInt);
    m_userDigitsRightOfDecimalSpinBox->setToolTip("Set digits right of decimal for\ndecimal or scientific format");
    
    m_autoSubdivisionsCheckBox = new QCheckBox("Auto");
    QObject::connect(m_autoSubdivisionsCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool);
    m_autoSubdivisionsCheckBox->setToolTip("Enable auto subdivisions");
    
    m_userSubdivisionsSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0, 100, 1);
    QObject::connect(m_userSubdivisionsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedInt); 
    m_userSubdivisionsSpinBox->setToolTip("Set subdivisions on the axis when Auto is not checked");
    
    /*
     * Group widgets for blocking signals
     */
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_axisNameToolButton);
    m_widgetGroup->add(m_autoUserRangeComboBox->getWidget());
    m_widgetGroup->add(m_userMinimumValueSpinBox);
    m_widgetGroup->add(m_userMaximumValueSpinBox);
    m_widgetGroup->add(m_showTickMarksCheckBox);
    m_widgetGroup->add(m_userNumericFormatComboBox->getWidget());
    m_widgetGroup->add(m_userDigitsRightOfDecimalSpinBox);
    m_widgetGroup->add(m_autoSubdivisionsCheckBox);
    m_widgetGroup->add(m_userSubdivisionsSpinBox);
    
    const int COLUMN_ONE   = 0;
    const int COLUMN_TWO   = 1;
    const int COLUMN_THREE = 2;
    const int COLUMN_FOUR  = 3;
    const int COLUMN_FIVE  = 4;
    const int COLUMN_SIX   = 5;
    /*
     * Layouts
     */
    QGridLayout* layout = new QGridLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 3, 0);
    int leftRow = 0;
    layout->addWidget(new QLabel("Axis"), leftRow, COLUMN_ONE, Qt::AlignLeft);
    layout->addWidget(m_axisComboBox->getWidget(), leftRow, COLUMN_TWO);
    leftRow++;
    layout->addWidget(new QLabel("Range"), leftRow, COLUMN_ONE, Qt::AlignLeft);
    layout->addWidget(m_autoUserRangeComboBox->getWidget(), leftRow, COLUMN_TWO);
    leftRow++;
    layout->addWidget(new QLabel("Maximum"), leftRow, COLUMN_ONE, Qt::AlignLeft);
    layout->addWidget(m_userMaximumValueSpinBox, leftRow, COLUMN_TWO);
    leftRow++;
    layout->addWidget(new QLabel("Minimum"), leftRow, COLUMN_ONE, Qt::AlignLeft);
    layout->addWidget(m_userMinimumValueSpinBox, leftRow, COLUMN_TWO);
    leftRow++;
    
    layout->setColumnMinimumWidth(COLUMN_THREE, 7);
    
    int rightRow = 0;
    layout->addWidget(m_axisNameToolButton, rightRow, COLUMN_FOUR, Qt::AlignLeft);
    layout->addWidget(m_showTickMarksCheckBox, rightRow, COLUMN_FIVE, 1, 2);
    rightRow++;
    layout->addWidget(new QLabel("Format"), rightRow, COLUMN_FOUR, Qt::AlignLeft);
    layout->addWidget(m_userNumericFormatComboBox->getWidget(), rightRow, COLUMN_FIVE, 1, 2);
    rightRow++;
    layout->addWidget(new QLabel("Decimals"), rightRow, COLUMN_FOUR, Qt::AlignLeft);
    layout->addWidget(m_userDigitsRightOfDecimalSpinBox, rightRow, COLUMN_FIVE, 1, 2);
    rightRow++;
    layout->addWidget(new QLabel("Subdivisions"), rightRow, COLUMN_FOUR, Qt::AlignLeft);
    layout->addWidget(m_autoSubdivisionsCheckBox, rightRow, COLUMN_FIVE, Qt::AlignHCenter);
    layout->addWidget(m_userSubdivisionsSpinBox, rightRow, COLUMN_SIX, Qt::AlignRight);
    rightRow++;

    QVBoxLayout* dialogLayout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(dialogLayout, 0, 2);
    dialogLayout->addLayout(layout);
    dialogLayout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartTwoAxes::~BrainBrowserWindowToolBarChartTwoAxes()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::updateContent(BrowserTabContent* browserTabContent)
{
    if (browserTabContent == NULL) {
        setEnabled(false);
        return;
    }
    
    ChartTwoCartesianAxis* selectedAxis = NULL;
    
    const ChartAxisLocationEnum::Enum defaultAxis = getSelectedAxisLocation();

    m_axisComboBox->blockSignals(true);
    m_axisComboBox->getComboBox()->clear();
    
    ChartTwoOverlaySet* chartOverlaySet = NULL;
    if (browserTabContent != NULL) {
        ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
        const int32_t tabIndex = browserTabContent->getTabNumber();
        if (modelChartTwo != NULL) {
            switch (modelChartTwo->getSelectedChartTwoDataType(tabIndex)) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    chartOverlaySet = modelChartTwo->getChartTwoOverlaySet(tabIndex);
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
            }
            
            if (chartOverlaySet != NULL) {
                int32_t defaultAxisIndex = 0;
                
                std::vector<ChartAxisLocationEnum::Enum> validAxesLocations;
                std::vector<ChartTwoCartesianAxis*> axes = chartOverlaySet->getDisplayedChartAxes();
                const int32_t numAxes = static_cast<int32_t>(axes.size());
                for (int32_t i = 0; i < numAxes; i++) {
                    const ChartAxisLocationEnum::Enum axisLocation = axes[i]->getAxisLocation();
                    if (defaultAxis == axisLocation) {
                        defaultAxisIndex = i;
                    }
                    validAxesLocations.push_back(axisLocation);
                }
                CaretAssert(validAxesLocations.size() == axes.size());
                
                if ( ! axes.empty()) {
                    CaretAssertVectorIndex(validAxesLocations, defaultAxisIndex);
                    const ChartAxisLocationEnum::Enum axisLocation = validAxesLocations[defaultAxisIndex];
                    m_axisComboBox->setupWithItems<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>(validAxesLocations);
                    m_axisComboBox->setSelectedItem<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>(axisLocation);
                    
                    CaretAssertVectorIndex(axes, defaultAxisIndex);
                    selectedAxis = axes[defaultAxisIndex];
                }
            }
        }
    }
    m_axisComboBox->blockSignals(false);

    updateControls(selectedAxis);
    
    setEnabled(selectedAxis != NULL);
}

/**
 * Called when axis is changed.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::axisChanged()
{
    updateContent(getTabContentFromSelectedTab());
}

/**
 * @return The selected axes location (will return left even if no valid selection).
 */
ChartAxisLocationEnum::Enum
BrainBrowserWindowToolBarChartTwoAxes::getSelectedAxisLocation() const
{
    ChartAxisLocationEnum::Enum axis = ChartAxisLocationEnum::CHART_AXIS_LOCATION_LEFT;
    
    if (m_axisComboBox->getComboBox()->count() > 0) {
        axis = m_axisComboBox->getSelectedItem<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>();
    }
    
    return axis;
}



/**
 * Update the content with the given axis.
 *
 * @param chartAxis
 *     New chart axis content.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::updateControls(ChartTwoCartesianAxis* chartAxis)
{
    m_chartAxis = chartAxis;
    
    if (m_chartAxis != NULL) {
        m_widgetGroup->blockAllSignals(true);
        //m_axisNameToolButton->setText(m_chartAxis->getLabelText());
        m_autoUserRangeComboBox->setSelectedItem<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>(m_chartAxis->getScaleRangeMode());
        m_userMinimumValueSpinBox->setValue(m_chartAxis->getUserScaleMinimumValue());
        m_userMaximumValueSpinBox->setValue(m_chartAxis->getUserScaleMaximumValue());
        m_showTickMarksCheckBox->setChecked(m_chartAxis->isShowTickmarks());
        const NumericFormatModeEnum::Enum numericFormat = m_chartAxis->getUserNumericFormat();
        m_userNumericFormatComboBox->setSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>(numericFormat);
        m_userDigitsRightOfDecimalSpinBox->setValue(m_chartAxis->getUserDigitsRightOfDecimal());
        m_userDigitsRightOfDecimalSpinBox->setEnabled(numericFormat != NumericFormatModeEnum::AUTO);
        m_autoSubdivisionsCheckBox->setChecked(m_chartAxis->isAutoSubdivisionsEnabled());
        m_userSubdivisionsSpinBox->setValue(m_chartAxis->getUserNumberOfSubdivisions());
        m_userSubdivisionsSpinBox->setEnabled( ! m_chartAxis->isAutoSubdivisionsEnabled());
        
        m_widgetGroup->blockAllSignals(false);
    }
}

/**
 * Called when a widget is changed by the user.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::valueChanged()
{
    CaretAssert(m_chartAxis);
    if (m_chartAxis != NULL) {
        m_chartAxis->setScaleRangeMode(m_autoUserRangeComboBox->getSelectedItem<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>());
        m_chartAxis->setUserScaleMinimumValue(m_userMinimumValueSpinBox->value());
        m_chartAxis->setUserScaleMaximumValue(m_userMaximumValueSpinBox->value());
        m_chartAxis->setShowTickmarks(m_showTickMarksCheckBox->isChecked());
        m_chartAxis->setUserNumericFormat(m_userNumericFormatComboBox->getSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>());
        m_chartAxis->setUserDigitsRightOfDecimal(m_userDigitsRightOfDecimalSpinBox->value());
        m_chartAxis->setAutoSubdivisionsEnabled(m_autoSubdivisionsCheckBox->isChecked());
        m_chartAxis->setUserNumberOfSubdivisions(m_userSubdivisionsSpinBox->value());
    }

    updateContent(getTabContentFromSelectedTab());
    
    updateGraphics();
}

/**
 * Update the graphics.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}


/**
 * Called when a widget is changed by a slot using a bool parameter.
 * Parameters must match when using function pointers.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool(bool)
{
    valueChanged();
}

/**
 * Called when a widget is changed by a slot using a double parameter.
 * Parameters must match when using function pointers.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::valueChangedDouble(double)
{
    valueChanged();
}

/**
 * Called when a widget is changed by a slot using a int parameter.
 * Parameters must match when using function pointers.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::valueChangedInt(int)
{
    valueChanged();
}

/**
 * Called when name toolbutton is clicked to change axis name.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::axisNameToolButtonClicked(bool)
{
    CaretAssert(m_chartAxis);
    if (m_chartAxis != NULL) {
        WuQDataEntryDialog newNameDialog("Axis Title",
                                         m_axisNameToolButton);
        QLineEdit* lineEdit = newNameDialog.addLineEditWidget("Axis Name");
        lineEdit->setText(m_chartAxis->getAxisTitle());
        if (newNameDialog.exec() == WuQDataEntryDialog::Accepted) {
            const AString name = lineEdit->text().trimmed();
            m_chartAxis->setAxisTitle(name);
            valueChanged();
        }
    }
}




