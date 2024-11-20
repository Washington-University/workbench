
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
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QTimer>
#include <QToolButton>

#define __CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_DECLARE__
#include "ChartTwoAxisPropertiesEditorWidget.h"
#undef __CHART_TWO_AXIS_PROPERTIES_EDITOR_WIDGET_DECLARE__

#include "CaretAssert.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoCartesianCustomSubdivisionsEditorWidget.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartableTwoFileBaseChart.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "DisplayGroupEnumComboBox.h"
#include "EnumComboBoxTemplate.h"
#include "EventChartTwoCartesianAxisDisplayGroup.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "ModelChartTwo.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMacroManager.h"
#include "WuQSpinBox.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * Constructor.
 *
 * @param parent
 *   The parent
 * @param axisLocation
 *   Location of the axis
 * @param parentObjectName
 *   Name of parent object for macros
 */
ChartTwoAxisPropertiesEditorWidget::ChartTwoAxisPropertiesEditorWidget(const ChartAxisLocationEnum::Enum axisLocation,
                                                                       const QString& parentObjectName,
                                                                       QWidget* parent)
: QWidget(parent),
m_chartOverlaySet(NULL),
m_chartAxis(NULL),
m_axisLocation(axisLocation)
{
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    const QString objectNamePrefix(parentObjectName
                                   + ":ChartAxesEditor:"
                                   + ChartAxisLocationEnum::toGuiName(axisLocation)
                                   + ":");
    
    m_showTickMarksCheckBox = new QCheckBox("Ticks");
    QObject::connect(m_showTickMarksCheckBox, &QCheckBox::clicked,
                     [=](bool clicked) { m_chartAxis->setShowTickmarks(clicked); valueChanged(); });
    m_showTickMarksCheckBox->setToolTip("Show ticks along the axis");
    m_showTickMarksCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowTicks");
    macroManager->addMacroSupportToObject(m_showTickMarksCheckBox,
                                          "Enable chart axis ticks");
    
    m_showLabelCheckBox = new QCheckBox("Title");
    QObject::connect(m_showLabelCheckBox, &QCheckBox::clicked,
                     [=](bool clicked) { m_chartAxis->setShowLabel(clicked); valueChanged(); });
    m_showLabelCheckBox->setToolTip("Show title on axis");
    m_showLabelCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowLabel");
    macroManager->addMacroSupportToObject(m_showLabelCheckBox,
                                          "Enable chart axis title");
    
    m_showNumericsCheckBox = new QCheckBox("Labels");
    QObject::connect(m_showNumericsCheckBox, &QCheckBox::clicked,
                     [=](bool clicked) { m_chartAxis->setNumericsTextDisplayed(clicked); valueChanged(); });
    m_showNumericsCheckBox->setToolTip("Show labels (numeric values) on axis");
    m_showNumericsCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowNumerics");
    macroManager->addMacroSupportToObject(m_showNumericsCheckBox,
                                          "Enable chart axis labels");
    
    m_rotateNumericsCheckBox = new QCheckBox("Rotate");
    QObject::connect(m_rotateNumericsCheckBox, &QCheckBox::clicked,
                     [=](bool clicked) { m_chartAxis->setNumericsTextRotated(clicked); valueChanged(); });
    m_rotateNumericsCheckBox->setToolTip("Rotate numeric scale values on axis");
    m_rotateNumericsCheckBox->setObjectName(objectNamePrefix
                                                 + "EnableNumericsRotate");
    macroManager->addMacroSupportToObject(m_rotateNumericsCheckBox,
                                          "Enable rotation of chart axis numerics");
    
    /*
     * Controls for layer selection and label editing
     */
    m_axisLabelToolButton = new QToolButton();
    m_axisLabelToolButton->setText("Edit Title...");
    QObject::connect(m_axisLabelToolButton, &QToolButton::clicked,
                     this, &ChartTwoAxisPropertiesEditorWidget::axisLabelToolButtonClicked);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_axisLabelToolButton);
    m_axisLabelToolButton->setToolTip("Edit the axis name for the file in the selected overlay");
    m_axisLabelToolButton->setObjectName(objectNamePrefix
                                                 + "EditAxis");
    macroManager->addMacroSupportToObject(m_axisLabelToolButton,
                                          "Edit chart axis label");
    
    m_axisLabelFromOverlayComboBox = new QComboBox();
    m_axisLabelFromOverlayComboBox->setToolTip("Title for axis is from file in selected layer");
    QObject::connect(m_axisLabelFromOverlayComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                     [=](int index) { m_chartAxis->setLabelOverlayIndex(index); valueChanged(); });
    m_axisLabelFromOverlayComboBox->setObjectName(objectNamePrefix
                                                 + "LabelFromOverlay");
    macroManager->addMacroSupportToObject(m_axisLabelFromOverlayComboBox,
                                          "Select chart axis title overlay source");
    
    /*
     * Numerics controls
     */
    m_userNumericFormatComboBox = new EnumComboBoxTemplate(this);
    m_userNumericFormatComboBox->setup<NumericFormatModeEnum, NumericFormatModeEnum::Enum>();
    QObject::connect(m_userNumericFormatComboBox, &EnumComboBoxTemplate::itemActivated,
                     [=]() { m_chartAxis->setUserNumericFormat(m_userNumericFormatComboBox->getSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>());  valueChanged(); });
    m_userNumericFormatComboBox->getWidget()->setToolTip("Choose format of axis scale numeric values");
    m_userNumericFormatComboBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "Format");
    macroManager->addMacroSupportToObject(m_userNumericFormatComboBox->getWidget(),
                                          "Select chart axis numeric format");
    
    m_userDigitsRightOfDecimalSpinBox = new WuQSpinBox();
    m_userDigitsRightOfDecimalSpinBox->setRange(0, 10);
    m_userDigitsRightOfDecimalSpinBox->setSingleStep(1);
    QObject::connect(m_userDigitsRightOfDecimalSpinBox, static_cast<void (WuQSpinBox::*)(int)>(&WuQSpinBox::valueChanged),
                     [=](int value) { m_chartAxis->setUserDigitsRightOfDecimal(value); valueChanged(); });
    m_userDigitsRightOfDecimalSpinBox->setToolTip("Set digits right of decimal for\ndecimal or scientific format");
    m_userDigitsRightOfDecimalSpinBox->setObjectName(objectNamePrefix
                                                 + "DigitsRightOfDecimal");
    macroManager->addMacroSupportToObject(m_userDigitsRightOfDecimalSpinBox,
                                          "Set chart axis digits right of decimal");
    
    m_numericSubdivisionsModeComboBox = new EnumComboBoxTemplate(this);
    m_numericSubdivisionsModeComboBox->setup<ChartTwoNumericSubdivisionsModeEnum, ChartTwoNumericSubdivisionsModeEnum::Enum>();
    QObject::connect(m_numericSubdivisionsModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     [=]() { m_chartAxis->setNumericSubdivsionsMode(m_numericSubdivisionsModeComboBox->getSelectedItem<ChartTwoNumericSubdivisionsModeEnum, ChartTwoNumericSubdivisionsModeEnum::Enum>()); valueChanged();} );
    m_numericSubdivisionsModeComboBox->getWidget()->setToolTip("Numeric subdivisions mode");
    m_numericSubdivisionsModeComboBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "NumericSubdivisionsMode");
    macroManager->addMacroSupportToObject(m_numericSubdivisionsModeComboBox->getWidget(),
                                          "Set chart axis numeric subdivisions mode");
    
    m_userSubdivisionsSpinBox = new WuQSpinBox();
    m_userSubdivisionsSpinBox->setRange(0, 99);
    m_userSubdivisionsSpinBox->setSingleStep(1);
    QObject::connect(m_userSubdivisionsSpinBox, static_cast<void (WuQSpinBox::*)(int)>(&WuQSpinBox::valueChanged),
                     [=](int value) { m_chartAxis->setUserNumberOfSubdivisions(value); valueChanged(); });
    m_userSubdivisionsSpinBox->setToolTip("Set subdivisions on the axis when Auto is not checked");
    m_userSubdivisionsSpinBox->setObjectName(objectNamePrefix
                                                 + "NumberOfSubdivisions");
    macroManager->addMacroSupportToObject(m_userSubdivisionsSpinBox,
                                          "Set chart axis number of subivisions");
    
    /*
     * Size spin boxes
     */
    m_labelSizeSpinBox = new WuQDoubleSpinBox(this);
    m_labelSizeSpinBox->setRangePercentage(0.0, 99.0);
    m_labelSizeSpinBox->setDecimals(1);
    QObject::connect(m_labelSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     [=](double value) { m_chartAxis->setLabelTextSize(value); valueChanged(); });
    m_labelSizeSpinBox->setToolTip("Set height of title as percentage of tab height for selected axis");
    m_labelSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "LabelHeight");
    macroManager->addMacroSupportToObject(m_labelSizeSpinBox->getWidget(),
                                          "Set chart axis title height");
    
    m_numericsSizeSpinBox = new WuQDoubleSpinBox(this);
    m_numericsSizeSpinBox->setRangePercentage(0.0, 99.0);
    m_numericsSizeSpinBox->setDecimals(1);
    QObject::connect(m_numericsSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     [=](double value) { m_chartAxis->setNumericsTextSize(value); valueChanged(); });
    m_numericsSizeSpinBox->setToolTip("Set height of labels (numeric values) as percentage of tab height for selected axis");
    m_numericsSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "NumericValueHeight");
    macroManager->addMacroSupportToObject(m_numericsSizeSpinBox->getWidget(),
                                          "Set chart axis numerics height");
    
    const QString linesToolTip(WuQtUtilities::createWordWrappedToolTipText("This is a TAB property (all axes in this tab use this line thickness).  "
                                                                           "The line thickness is a percentage of the tab's height."));
    QLabel* linesLabel = new QLabel("Lines");
    linesLabel->setToolTip(linesToolTip);
    m_linesTicksSizeSpinBox = new WuQDoubleSpinBox(this);
    m_linesTicksSizeSpinBox->setDecimals(1);
    m_linesTicksSizeSpinBox->setRangePercentage(0.0, 99.0);
    QObject::connect(m_linesTicksSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &ChartTwoAxisPropertiesEditorWidget::axisLineThicknessChanged);
    m_linesTicksSizeSpinBox->setToolTip(linesToolTip);
    m_linesTicksSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "TicksSize");
    macroManager->addMacroSupportToObject(m_linesTicksSizeSpinBox->getWidget(),
                                          "Set chart axis ticks height");
    
    m_paddingSizeSpinBox = new WuQDoubleSpinBox(this);
    m_paddingSizeSpinBox->setDecimals(1);
    m_paddingSizeSpinBox->setRangePercentage(0.0, 99.0);
    QObject::connect(m_paddingSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     [=](double value) { m_chartAxis->setPaddingSize(value); valueChanged(); });
    m_paddingSizeSpinBox->setToolTip("Set padding (space between edge and labels/ticks) as percentage of tab height for selected axis");
    m_paddingSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "PaddingSize");
    macroManager->addMacroSupportToObject(m_paddingSizeSpinBox->getWidget(),
                                          "Set chart axis padding height");
    
    /*
     * Group widgets for blocking signals
     */
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_axisLabelToolButton);
    m_widgetGroup->add(m_showTickMarksCheckBox);
    m_widgetGroup->add(m_showLabelCheckBox);
    m_widgetGroup->add(m_showNumericsCheckBox);
    m_widgetGroup->add(m_rotateNumericsCheckBox);
    m_widgetGroup->add(m_axisLabelFromOverlayComboBox);
    m_widgetGroup->add(m_userNumericFormatComboBox->getWidget());
    m_widgetGroup->add(m_userDigitsRightOfDecimalSpinBox);
    m_widgetGroup->add(m_numericSubdivisionsModeComboBox->getWidget());
    m_widgetGroup->add(m_userSubdivisionsSpinBox);
    m_widgetGroup->add(m_labelSizeSpinBox);
    m_widgetGroup->add(m_numericsSizeSpinBox);
    m_widgetGroup->add(m_linesTicksSizeSpinBox);
    m_widgetGroup->add(m_paddingSizeSpinBox);
    
    /*
     * Size layout
     */
    QWidget* sizesWidget = new QWidget();
    QGridLayout* sizesLayout = new QGridLayout(sizesWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(sizesLayout, 4, 0);
    int32_t sizesRow = 0;
    sizesLayout->addWidget(new QLabel("Sizes"), sizesRow, 0, 1, 2, Qt::AlignHCenter);
    sizesRow++;
    sizesLayout->addWidget(new QLabel("Labels"), sizesRow, 0);
    sizesLayout->addWidget(m_numericsSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    sizesLayout->addWidget(new QLabel("Pad"), sizesRow, 0);
    sizesLayout->addWidget(m_paddingSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    sizesLayout->addWidget(new QLabel("Title"), sizesRow, 0);
    sizesLayout->addWidget(m_labelSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    sizesLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), sizesRow, 0, 1, 2);
    sizesRow++;
    sizesLayout->addWidget(linesLabel, sizesRow, 0);
    sizesLayout->addWidget(m_linesTicksSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;

    /*
     * Show widgets layout
     */
    QWidget* showWidget = new QWidget();
    QGridLayout* showLayout = new QGridLayout(showWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(showLayout, 4, 0);
    int32_t axisRow = 0;
    showLayout->addWidget(new QLabel("Show"), axisRow, 0, Qt::AlignHCenter);
    axisRow++;
    showLayout->addWidget(m_showNumericsCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_rotateNumericsCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_showTickMarksCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_showLabelCheckBox, axisRow, 0);
    axisRow++;
    showWidget->setSizePolicy(showWidget->sizePolicy().horizontalPolicy(),
                              QSizePolicy::Fixed);
    
    QHBoxLayout* subdivLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(subdivLayout, 3, 0);
    subdivLayout->addWidget(new QLabel("Subdiv"));
    subdivLayout->addWidget(m_numericSubdivisionsModeComboBox->getWidget());
    subdivLayout->addWidget(m_userSubdivisionsSpinBox);
    subdivLayout->addStretch();
    
    /*
     * Numerics widgets layout
     */
    QWidget* stdNumericsWidget = new QWidget();
    QGridLayout* stdNumericsLayout = new QGridLayout(stdNumericsWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(stdNumericsLayout, 6, 0);
    int stdNumericsRow = 0;
    stdNumericsLayout->addWidget(new QLabel("Format"), stdNumericsRow, 0);
    stdNumericsLayout->addWidget(m_userNumericFormatComboBox->getWidget(), stdNumericsRow, 1);
    stdNumericsLayout->addWidget(new QLabel("Decimals"), stdNumericsRow, 2);
    stdNumericsLayout->addWidget(m_userDigitsRightOfDecimalSpinBox, stdNumericsRow, 3);
    stdNumericsRow++;
    stdNumericsLayout->addLayout(subdivLayout, stdNumericsRow, 0, 1, 4);
    stdNumericsWidget->setFixedSize(stdNumericsWidget->sizeHint());

    /*
     * Custom axis numerics editor widget
     */
    m_customSubdivisionsEditorWidget = new ChartTwoCartesianCustomSubdivisionsEditorWidget();
    QObject::connect(m_customSubdivisionsEditorWidget, &ChartTwoCartesianCustomSubdivisionsEditorWidget::widgetSizeChanged,
                     [=]() {
        QWidget* parent = parentWidget();
        if (parent != NULL) {
            parent->adjustSize();
        }
    });
    
    /*
     * Subdivisions mode
     */
    m_chartSubdivisionsModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_chartSubdivisionsModeEnumComboBox->setup<ChartTwoCartesianSubdivisionsModeEnum,ChartTwoCartesianSubdivisionsModeEnum::Enum>();
    QObject::connect(m_chartSubdivisionsModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &ChartTwoAxisPropertiesEditorWidget::chartSubdivisionsModeEnumComboBoxItemActivated);
    /*
     * Numerics layout for its tab bar and stacked widget
     */
    m_numericsStackedWidget = new QStackedWidget();
    m_numericsStackedWidgetStandardSubdivsionsIndex = m_numericsStackedWidget->addWidget(stdNumericsWidget);
    m_numericsStackedWidgetCustomSubdivsionsIndex   = m_numericsStackedWidget->addWidget(m_customSubdivisionsEditorWidget);
    m_numericsStackedWidgetMapNamesIndex            = m_numericsStackedWidget->addWidget(new QWidget()); /* no special controls */
    
    QWidget* numericsWidget = new QWidget();
    QGridLayout* numericsLayout = new QGridLayout(numericsWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(numericsLayout, 0, 0);
    numericsLayout->addWidget(new QLabel("Numerics"), 0, 0, 1, 3, Qt::AlignHCenter);
    numericsLayout->addWidget(new QLabel("Mode"), 1, 0);
    numericsLayout->addWidget(m_chartSubdivisionsModeEnumComboBox->getWidget(), 1, 1);
    numericsLayout->addWidget(m_numericsStackedWidget, 2, 0, 1, 3, Qt::AlignLeft);
    numericsLayout->setColumnStretch(0, 0);
    numericsLayout->setColumnStretch(1, 0);
    numericsLayout->setColumnStretch(2, 100);

    /*
     * Title layout
     */
    QWidget* labelWidget = new QWidget();
    QGridLayout* labelLayout = new QGridLayout(labelWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(labelLayout, 3, 0);
    int labelRow(0);
    labelLayout->addWidget(new QLabel("Axis Title"), labelRow, 0);
    labelRow++;
    labelLayout->addWidget(m_axisLabelFromOverlayComboBox, labelRow, 0);
    labelRow++;
    labelLayout->addWidget(m_axisLabelToolButton, labelRow, 0);
    labelRow++;
    
    /*
     * Grid layout containing layouts
     */
    QHBoxLayout* controlsLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(controlsLayout, 3, 4);
    controlsLayout->addWidget(showWidget, 0, Qt::AlignTop);
    controlsLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    controlsLayout->addWidget(sizesWidget, 0, Qt::AlignTop);
    controlsLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    controlsLayout->addWidget(numericsWidget, 0, Qt::AlignTop);
    controlsLayout->addWidget(WuQtUtilities::createVerticalLineWidget());
    controlsLayout->addWidget(labelWidget, 0, Qt::AlignTop);
    controlsLayout->addStretch();
    
    QLabel* groupLabel = new QLabel("Group");
    m_displayGroupComboBox = new DisplayGroupEnumComboBox(this,
                                                                (objectNamePrefix
                                                                 + ":DisplayGroup"),
                                                                "AxisDisplayGroup");
    QObject::connect(m_displayGroupComboBox, &DisplayGroupEnumComboBox::displayGroupSelected,
                     this, &ChartTwoAxisPropertiesEditorWidget::displayGroupSelected);
    
    QHBoxLayout* groupLayout = new QHBoxLayout();
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_displayGroupComboBox->getWidget());
    groupLayout->addStretch();
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 3, 0);
    layout->addLayout(groupLayout);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addLayout(controlsLayout);
}

/**
 * Destructor.
 */
ChartTwoAxisPropertiesEditorWidget::~ChartTwoAxisPropertiesEditorWidget()
{
}

/**
 * Update the controls
 * @param chartOverlaySet
 *    The chart overlay set
 * @param chartAxis
 *    The chart axis
 */
void
ChartTwoAxisPropertiesEditorWidget::updateControls(ChartTwoOverlaySet* chartOverlaySet,
                                                   ChartTwoCartesianAxis* chartAxis)
{
    m_widgetGroup->blockAllSignals(true);

    m_chartOverlaySet = chartOverlaySet;
    m_chartAxis       = chartAxis;
    
    if ((m_chartOverlaySet != NULL)
        && (m_chartAxis != NULL)) {
        m_displayGroupComboBox->setSelectedDisplayGroup(m_chartAxis->getDisplayGroup());
        
        const ChartTwoCartesianSubdivisionsModeEnum::Enum subdivisionsMode = m_chartAxis->getSubdivisionsMode();
        m_chartSubdivisionsModeEnumComboBox->setSelectedItem<ChartTwoCartesianSubdivisionsModeEnum,ChartTwoCartesianSubdivisionsModeEnum::Enum>(subdivisionsMode);
        switch (subdivisionsMode) {
            case ChartTwoCartesianSubdivisionsModeEnum::CUSTOM:
                m_numericsStackedWidget->setCurrentIndex(m_numericsStackedWidgetCustomSubdivsionsIndex);
                break;
            case ChartTwoCartesianSubdivisionsModeEnum::STANDARD:
                m_numericsStackedWidget->setCurrentIndex(m_numericsStackedWidgetStandardSubdivsionsIndex);
                break;
            case ChartTwoCartesianSubdivisionsModeEnum::MAP_NAMES:
                m_numericsStackedWidget->setCurrentIndex(m_numericsStackedWidgetMapNamesIndex);
                break;
        }

        m_showTickMarksCheckBox->setChecked(m_chartAxis->isShowTickmarks());
        m_showLabelCheckBox->setChecked(m_chartAxis->isShowLabel());
        m_showNumericsCheckBox->setChecked(m_chartAxis->isNumericsTextDisplayed());
        m_rotateNumericsCheckBox->setChecked(m_chartAxis->isNumericsTextRotated());
        const NumericFormatModeEnum::Enum numericFormat = m_chartAxis->getUserNumericFormat();
        m_userNumericFormatComboBox->setSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>(numericFormat);
        m_userDigitsRightOfDecimalSpinBox->setValue(m_chartAxis->getUserDigitsRightOfDecimal());
        m_userDigitsRightOfDecimalSpinBox->setEnabled(numericFormat != NumericFormatModeEnum::AUTO);
        m_numericSubdivisionsModeComboBox->setSelectedItem<ChartTwoNumericSubdivisionsModeEnum, ChartTwoNumericSubdivisionsModeEnum::Enum>(m_chartAxis->getNumericSubdivsionsMode());
        m_userSubdivisionsSpinBox->setValue(m_chartAxis->getUserNumberOfSubdivisions());
        m_userSubdivisionsSpinBox->setEnabled( m_chartAxis->getNumericSubdivsionsMode() == ChartTwoNumericSubdivisionsModeEnum::USER);
        
        m_labelSizeSpinBox->setValue(m_chartAxis->getLabelTextSize());
        m_numericsSizeSpinBox->setValue(m_chartAxis->getNumericsTextSize());
        m_linesTicksSizeSpinBox->setValue(m_chartOverlaySet->getAxisLineThickness());
        m_paddingSizeSpinBox->setValue(m_chartAxis->getPaddingSize());
        
        const int32_t overlayCount = m_chartOverlaySet->getNumberOfDisplayedOverlays();
        int32_t selectedOverlayIndex = m_chartAxis->getLabelOverlayIndex(overlayCount);
        const int32_t comboBoxCount = m_axisLabelFromOverlayComboBox->count();
        if (overlayCount < comboBoxCount) {
            m_axisLabelFromOverlayComboBox->setMaxCount(overlayCount);
        }
        else if (overlayCount > comboBoxCount) {
            for (int32_t j = comboBoxCount; j < overlayCount; j++) {
                m_axisLabelFromOverlayComboBox->addItem(" From Layer " + AString::number(j + 1));
            }
        }
        
        if ((selectedOverlayIndex >= 0)
            && (selectedOverlayIndex < m_axisLabelFromOverlayComboBox->count())) {
            m_axisLabelFromOverlayComboBox->setCurrentIndex(selectedOverlayIndex);
        }
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
    
    ChartTwoCartesianCustomSubdivisions* customSubdivsions = ((m_chartAxis != NULL)
                                                              ? m_chartAxis->getCustomSubdivisions()
                                                              : NULL);
    m_customSubdivisionsEditorWidget->updateContent(customSubdivsions);
    
    m_widgetGroup->blockAllSignals(false);
    
    layout()->invalidate();
    adjustSize();
    parentWidget()->adjustSize();
}

/**
 * Called when subdivisions mode is changed
 */
void
ChartTwoAxisPropertiesEditorWidget::chartSubdivisionsModeEnumComboBoxItemActivated()
{
    CaretAssert(m_chartAxis);
    if (m_chartAxis != NULL) {
        const ChartTwoCartesianSubdivisionsModeEnum::Enum subdivisionsMode = m_chartSubdivisionsModeEnumComboBox->getSelectedItem<ChartTwoCartesianSubdivisionsModeEnum,ChartTwoCartesianSubdivisionsModeEnum::Enum>();
        m_chartAxis->setSubdivisionsMode(subdivisionsMode);
        /*
         * Need to update controls as some items (such as numeric format
         * combo box) affect enabled status of items (numeric digits
         * right of decimal)
         */
        valueChanged();
    }
}


/**
 * Called when the axis line thickness changes.
 */
void
ChartTwoAxisPropertiesEditorWidget::axisLineThicknessChanged(double)
{
    /*
     * Line thickness is in parent overlay set
     */
    if (m_chartOverlaySet != NULL) {
        m_chartOverlaySet->setAxisLineThickness(m_linesTicksSizeSpinBox->value());
        updateGraphics();
    }
}


/**
 * Called when a widget is changed by the user.
 */
void
ChartTwoAxisPropertiesEditorWidget::valueChanged()
{
    CaretAssert(m_chartAxis);
    if (m_chartAxis != NULL) {
        /*
         * Need to update controls as some items (such as numeric format
         * combo box) affect enabled status of items (numeric digits
         * right of decimal)
         */
        updateControls(m_chartOverlaySet,
                       m_chartAxis);
    }

    updateGraphics();
}

/**
 * Update the graphics.
 */
void
ChartTwoAxisPropertiesEditorWidget::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when name toolbutton is clicked to change axis label.
 */
void
ChartTwoAxisPropertiesEditorWidget::axisLabelToolButtonClicked(bool)
{
    if ((m_chartOverlaySet != NULL)
        && (m_chartAxis != NULL)) {
        WuQDataEntryDialog newNameDialog("Axis Title",
                                         m_axisLabelToolButton);
        QLineEdit* lineEdit = newNameDialog.addLineEditWidget("Title");
        lineEdit->setText(m_chartOverlaySet->getAxisLabel(m_chartAxis));
        if (newNameDialog.exec() == WuQDataEntryDialog::Accepted) {
            const AString name = lineEdit->text().trimmed();
            m_chartOverlaySet->setAxisLabel(m_chartAxis, name);
            valueChanged();
        }
    }
}

/**
 * Called when the label display group combo box is changed.
 * @param displayGroup
 *   Newly selected group
 */
void
ChartTwoAxisPropertiesEditorWidget::displayGroupSelected(const DisplayGroupEnum::Enum displayGroup)
{
    if (m_chartAxis != NULL) {
        /*
         * Newly selected group is NOT TAB so it must be a GROUP
         */
        if (displayGroup != DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            /*
             * New group is different from old group so have
             * transitioned from TAB to a GROUP or from
             * a GROUP to a DIFFERENT GROUP
             */
            if (displayGroup != m_chartAxis->getDisplayGroup()) {
                /*
                 * if NO axes are yoked to the display group, this axis parameters
                 * are copied to the display group
                 */
                EventChartTwoCartesianAxisDisplayGroup::initializeUnyokedDisplayGroupAxis(displayGroup,
                                                                                          m_chartAxis);
            }
        }
        m_chartAxis->setDisplayGroup(displayGroup);
        valueChanged();
    }
}
