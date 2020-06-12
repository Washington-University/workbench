
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
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_DECLARE__
#include "BrainBrowserWindowToolBarChartTwoAxes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_AXES_DECLARE__

#include "AnnotationPercentSizeText.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoCartesianAxis.h"
#include "ChartTwoOverlay.h"
#include "ChartTwoOverlaySet.h"
#include "ChartableTwoFileBaseChart.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileHistogramChart.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventChartTwoAttributesChanged.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChartTwo.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQDoubleSpinBox.h"
#include "WuQMacroManager.h"
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
 * @param parentObjectName
 *   Name of parent object for macros
 */
BrainBrowserWindowToolBarChartTwoAxes::BrainBrowserWindowToolBarChartTwoAxes(BrainBrowserWindowToolBar* parentToolBar,
                                                                             const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_chartOverlaySet = NULL;
    m_chartAxis = NULL;
    
    WuQMacroManager* macroManager = WuQMacroManager::instance();
    const QString objectNamePrefix(parentObjectName
                                   + ":ChartAxes:");
    
    /*
     * 'Show' checkboxes
     */
    m_axisDisplayedByUserCheckBox = new QCheckBox("Axis");
    m_axisDisplayedByUserCheckBox->setToolTip("Show/hide the axis");
    QObject::connect(m_axisDisplayedByUserCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChanged);
    m_axisDisplayedByUserCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowAxis");
    macroManager->addMacroSupportToObject(m_axisDisplayedByUserCheckBox,
                                          "Enable chart axis");
    

    m_showTickMarksCheckBox = new QCheckBox("Ticks");
    QObject::connect(m_showTickMarksCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool);
    m_showTickMarksCheckBox->setToolTip("Show ticks along the axis");
    m_showTickMarksCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowTicks");
    macroManager->addMacroSupportToObject(m_showTickMarksCheckBox,
                                          "Enable chart axis ticks");
    
    m_showLabelCheckBox = new QCheckBox("Label");
    QObject::connect(m_showLabelCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool);
    m_showLabelCheckBox->setToolTip("Show label on axis");
    m_showLabelCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowLabel");
    macroManager->addMacroSupportToObject(m_showLabelCheckBox,
                                          "Enable chart axis label");
    
    m_showNumericsCheckBox = new QCheckBox("Nums");
    QObject::connect(m_showNumericsCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool);
    m_showNumericsCheckBox->setToolTip("Show numeric scale values on axis");
    m_showNumericsCheckBox->setObjectName(objectNamePrefix
                                                 + "ShowNumerics");
    macroManager->addMacroSupportToObject(m_showNumericsCheckBox,
                                          "Enable chart axis numerics");
    
    m_rotateNumericsCheckBox = new QCheckBox("Rotate");
    QObject::connect(m_rotateNumericsCheckBox, &QCheckBox::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedBool);
    m_rotateNumericsCheckBox->setToolTip("Rotate numeric scale values on axis");
    m_rotateNumericsCheckBox->setObjectName(objectNamePrefix
                                                 + "EnableNumericsRotate");
    macroManager->addMacroSupportToObject(m_rotateNumericsCheckBox,
                                          "Enable rotation of chart axis numerics");
    
    /*
     * Axes selection
     */
    m_axisComboBox = new EnumComboBoxTemplate(this);
    m_axisComboBox->setup<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>();
    QObject::connect(m_axisComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisChanged);
    m_axisComboBox->getWidget()->setToolTip("Choose axis for editing");
    m_axisComboBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "ChooseAxis");
    macroManager->addMacroSupportToObject(m_axisComboBox->getComboBox(),
                                          "Select chart axis");
    
    /*
     * Controls for layer selection and label editing
     */
    m_axisLabelToolButton = new QToolButton();
    m_axisLabelToolButton->setText("Edit Label...");
    QObject::connect(m_axisLabelToolButton, &QToolButton::clicked,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisLabelToolButtonClicked);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_axisLabelToolButton);
    m_axisLabelToolButton->setToolTip("Edit the axis name for the file in the selected overlay");
    m_axisLabelToolButton->setObjectName(objectNamePrefix
                                                 + "EditAxis");
    macroManager->addMacroSupportToObject(m_axisLabelToolButton,
                                          "Edit chart axis label");
    
    QLabel* axisLabelFromOverlayLabel = new QLabel("Label From File In");
    m_axisLabelFromOverlayComboBox = new QComboBox();
    m_axisLabelFromOverlayComboBox->setToolTip("Label for axis is from file in selected layer");
    QObject::connect(m_axisLabelFromOverlayComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedInt);
    m_axisLabelFromOverlayComboBox->setObjectName(objectNamePrefix
                                                 + "LabelFromOverlay");
    macroManager->addMacroSupportToObject(m_axisLabelFromOverlayComboBox,
                                          "Select chart axis overlay source");
    
    /*
     * Range controls
     */
    const AString rangeTooltip("Auto - Adjusts axis range to fit data with some\n"
                               "       padding so that scale value are usually\n"
                               "       whole numbers\n"
                               "Data - Axis range is limited to minimum and \n"
                               "       maximum values of the data\n"
                               "User - Axis range is controlled by user");
    m_autoUserRangeComboBox = new EnumComboBoxTemplate(this);
    m_autoUserRangeComboBox->setup<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>();
    QObject::connect(m_autoUserRangeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChanged);
    m_autoUserRangeComboBox->getWidget()->setToolTip(rangeTooltip);
    m_autoUserRangeComboBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "RangeMode");
    macroManager->addMacroSupportToObject(m_autoUserRangeComboBox->getWidget(),
                                          "Select chart axis range mode");
    
    m_userMinimumValueSpinBox = new WuQDoubleSpinBox(this);
    m_userMinimumValueSpinBox->setDecimalsModeAuto();
    m_userMinimumValueSpinBox->setSingleStepPercentage(1.0);
    QObject::connect(m_userMinimumValueSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisMinimumValueChanged);
    m_userMinimumValueSpinBox->setToolTip("Set user scaling axis minimum value");
    m_userMinimumValueSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "ScaleMinimum");
    macroManager->addMacroSupportToObject(m_userMinimumValueSpinBox->getWidget(),
                                          "Set chart axis minimum");
    
    m_userMaximumValueSpinBox = new WuQDoubleSpinBox(this);
    m_userMaximumValueSpinBox->setDecimalsModeAuto();
    m_userMaximumValueSpinBox->setSingleStepPercentage(1.0);
    QObject::connect(m_userMaximumValueSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisMaximumValueChanged);
    m_userMaximumValueSpinBox->setToolTip("Set user scaling axis maximum value");
    m_userMaximumValueSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "ScaleMaximum");
    macroManager->addMacroSupportToObject(m_userMaximumValueSpinBox->getWidget(),
                                          "See chart axis maximum");
    
    /*
     * Format controls
     */
    m_userNumericFormatComboBox = new EnumComboBoxTemplate(this);
    m_userNumericFormatComboBox->setup<NumericFormatModeEnum, NumericFormatModeEnum::Enum>();
    QObject::connect(m_userNumericFormatComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChanged);
    m_userNumericFormatComboBox->getWidget()->setToolTip("Choose format of axis scale numeric values");
    m_userNumericFormatComboBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "Format");
    macroManager->addMacroSupportToObject(m_userNumericFormatComboBox->getWidget(),
                                          "Select chart axis numeric format");
    
    m_userDigitsRightOfDecimalSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0, 10, 1);
    QObject::connect(m_userDigitsRightOfDecimalSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedInt);
    m_userDigitsRightOfDecimalSpinBox->setToolTip("Set digits right of decimal for\ndecimal or scientific format");
    m_userDigitsRightOfDecimalSpinBox->setObjectName(objectNamePrefix
                                                 + "DigitsRightOfDecimal");
    macroManager->addMacroSupportToObject(m_userDigitsRightOfDecimalSpinBox,
                                          "Set chart axis digits right of decimal");
    
    m_numericSubdivisionsModeComboBox = new EnumComboBoxTemplate(this);
    m_numericSubdivisionsModeComboBox->setup<ChartTwoNumericSubdivisionsModeEnum, ChartTwoNumericSubdivisionsModeEnum::Enum>();
    QObject::connect(m_numericSubdivisionsModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChanged);
    m_numericSubdivisionsModeComboBox->getWidget()->setToolTip("Numeric subdivisions mode");
    m_numericSubdivisionsModeComboBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "NumericSubdivisionsMode");
    macroManager->addMacroSupportToObject(m_numericSubdivisionsModeComboBox->getWidget(),
                                          "Set chart axis numeric subdivisions mode");
    
    m_userSubdivisionsSpinBox = WuQFactory::newSpinBoxWithMinMaxStep(0, 99, 1);
    QObject::connect(m_userSubdivisionsSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedInt); 
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
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedDouble);
    m_labelSizeSpinBox->setToolTip("Set height of label as percentage of tab height for selected axis");
    m_labelSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "LabelHeight");
    macroManager->addMacroSupportToObject(m_labelSizeSpinBox->getWidget(),
                                          "Set chart axis label height");
    
    m_numericsSizeSpinBox = new WuQDoubleSpinBox(this);
    m_numericsSizeSpinBox->setRangePercentage(0.0, 99.0);
    m_numericsSizeSpinBox->setDecimals(1);
    QObject::connect(m_numericsSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedDouble);
    m_numericsSizeSpinBox->setToolTip("Set height of numeric values as percentage of tab height for selected axis");
    m_numericsSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "NumericValueHeight");
    macroManager->addMacroSupportToObject(m_numericsSizeSpinBox->getWidget(),
                                          "Set chart axis numerics height");
    
    m_linesTicksSizeSpinBox = new WuQDoubleSpinBox(this);
    m_linesTicksSizeSpinBox->setDecimals(1);
    m_linesTicksSizeSpinBox->setRangePercentage(0.0, 99.0);
    QObject::connect(m_linesTicksSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::axisLineThicknessChanged);
    m_linesTicksSizeSpinBox->setToolTip("Set thickness of axis lines as percentage of tab height for ALL axes");
    m_linesTicksSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "TicksSize");
    macroManager->addMacroSupportToObject(m_linesTicksSizeSpinBox->getWidget(),
                                          "Set chart axis ticks height");
    
    m_paddingSizeSpinBox = new WuQDoubleSpinBox(this);
    m_paddingSizeSpinBox->setDecimals(1);
    m_paddingSizeSpinBox->setRangePercentage(0.0, 99.0);
    QObject::connect(m_paddingSizeSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &BrainBrowserWindowToolBarChartTwoAxes::valueChangedDouble);
    m_paddingSizeSpinBox->setToolTip("Set padding (space between edge and labels) as percentage of tab height for selected axis");
    m_paddingSizeSpinBox->getWidget()->setObjectName(objectNamePrefix
                                                 + "PaddingSize");
    macroManager->addMacroSupportToObject(m_paddingSizeSpinBox->getWidget(),
                                          "Set chart axis padding height");
    
    /*
     * Group widgets for blocking signals
     */
    m_widgetGroup = new WuQWidgetObjectGroup(this);
    m_widgetGroup->add(m_axisComboBox);
    m_widgetGroup->add(m_axisDisplayedByUserCheckBox);
    m_widgetGroup->add(m_axisLabelToolButton);
    m_widgetGroup->add(m_autoUserRangeComboBox->getWidget());
    m_widgetGroup->add(m_userMinimumValueSpinBox);
    m_widgetGroup->add(m_userMaximumValueSpinBox);
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
    WuQtUtilities::setLayoutSpacingAndMargins(sizesLayout, 2, 0);
    int32_t sizesRow = 0;
    sizesLayout->addWidget(new QLabel("Label"), sizesRow, 0);
    sizesLayout->addWidget(m_labelSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    sizesLayout->addWidget(new QLabel("Scale"), sizesRow, 0);
    sizesLayout->addWidget(m_numericsSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    sizesLayout->addWidget(new QLabel("Pad"), sizesRow, 0);
    sizesLayout->addWidget(m_paddingSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    sizesLayout->addWidget(new QLabel("Lines"), sizesRow, 0);
    sizesLayout->addWidget(m_linesTicksSizeSpinBox->getWidget(), sizesRow, 1);
    sizesRow++;
    
    /*
     * Show widgets layout
     */
    const bool displayLabelShowAtTopFlag = false;
    QWidget* showWidget = new QWidget();
    QGridLayout* showLayout = new QGridLayout(showWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(showLayout, 1, 0);
    int32_t axisRow = 0;
    if (displayLabelShowAtTopFlag) {
        showLayout->addWidget(new QLabel("Show"), axisRow, 0, Qt::AlignHCenter);
        axisRow++;
    }
    showLayout->addWidget(m_axisDisplayedByUserCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_showLabelCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_showNumericsCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_rotateNumericsCheckBox, axisRow, 0);
    axisRow++;
    showLayout->addWidget(m_showTickMarksCheckBox, axisRow, 0);
    axisRow++;
    showWidget->setSizePolicy(showWidget->sizePolicy().horizontalPolicy(),
                              QSizePolicy::Fixed);
    
    /*
     * Range widgets layout
     */
    QWidget* rangeWidget = new QWidget();
    QGridLayout* rangeLayout = new QGridLayout(rangeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(rangeLayout, 3, 0);
    int rangeRow = 0;
    rangeLayout->addWidget(new QLabel("Range"), rangeRow, 0);
    m_autoUserRangeComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    rangeLayout->addWidget(m_autoUserRangeComboBox->getWidget(), rangeRow, 1);
    rangeRow++;
    m_userMinimumValueSpinBox->setFixedWidth(90);
    m_userMaximumValueSpinBox->setFixedWidth(90);
    rangeLayout->addWidget(new QLabel("Max"), rangeRow, 0);
    rangeLayout->addWidget(m_userMaximumValueSpinBox->getWidget(), rangeRow, 1);
    rangeRow++;
    rangeLayout->addWidget(new QLabel("Min"), rangeRow, 0);
    rangeLayout->addWidget(m_userMinimumValueSpinBox->getWidget(), rangeRow, 1);

    QHBoxLayout* subdivLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(subdivLayout, 3, 0);
    subdivLayout->addWidget(new QLabel("Subdiv"));
    subdivLayout->addWidget(m_numericSubdivisionsModeComboBox->getWidget());
    subdivLayout->addWidget(m_userSubdivisionsSpinBox);
    
    /*
     * Numerics widgets layout
     */
    QWidget* numericsWidget = new QWidget();
    QGridLayout* numericsLayout = new QGridLayout(numericsWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(numericsLayout, 3, 0);
    int numericsRow = 0;
    numericsLayout->addWidget(new QLabel("Format"), numericsRow, 0);
    numericsLayout->addWidget(m_userNumericFormatComboBox->getWidget(), numericsRow, 1, 1, 2);
    numericsRow++;
    numericsLayout->addWidget(new QLabel("Decimals"), numericsRow, 0);
    numericsLayout->addWidget(m_userDigitsRightOfDecimalSpinBox, numericsRow, 1, 1, 2);
    numericsRow++;
    numericsLayout->addLayout(subdivLayout, numericsRow, 0, 1, 3);
    
    /*
     * Top layout
     */
    QHBoxLayout* topLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(topLayout, 3, 0);
    topLayout->addWidget(new QLabel("Edit Axis "));
    topLayout->addWidget(m_axisComboBox->getWidget());
    topLayout->addSpacing(3);
    topLayout->addStretch();
    topLayout->addWidget(axisLabelFromOverlayLabel);
    topLayout->addWidget(m_axisLabelFromOverlayComboBox);
    topLayout->addSpacing(3);
    topLayout->addWidget(m_axisLabelToolButton);
    
    /*
     * Grid layout containing layouts
     */
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(2);
    gridLayout->setVerticalSpacing(1);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->addLayout(topLayout, 0, 0, 1, 7);
    gridLayout->addWidget(showWidget, 1, 0, Qt::AlignTop);
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 1, 1);
    gridLayout->addWidget(sizesWidget, 1, 2, Qt::AlignTop);
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 1, 3);
    gridLayout->addWidget(rangeWidget, 1, 4, Qt::AlignTop);
    gridLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 1, 5);
    gridLayout->addWidget(numericsWidget, 1, 6, Qt::AlignTop);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addLayout(gridLayout);
    layout->addStretch();

    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartTwoAxes::~BrainBrowserWindowToolBarChartTwoAxes()
{
    EventManager::get()->removeEventFromListener(this,
                                                 EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
}

/**
 * Receive an event.
 *
 * @param event
 *    The event.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        CaretAssert(dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event));
        updateContent(getTabContentFromSelectedTab());
    }
    else {
        BrainBrowserWindowToolBarComponent::receiveEvent(event);
    }
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
    
    updateControls(browserTabContent);
}

/**
 * Get the selection data.
 *
 * @param browserTabContent
 *     The tab content.
 * @param chartOverlaySetOut
 *     The chart overlay set (may be NULL)
 * @param validAxesLocationsOut
 *     The valid axes locations.
 * @param selectedAxisOut
 *     Output with selected axis (may be NULL)
 */
void
BrainBrowserWindowToolBarChartTwoAxes::getSelectionData(BrowserTabContent* browserTabContent,
                                                        ChartTwoOverlaySet* &chartOverlaySetOut,
                                                        std::vector<ChartAxisLocationEnum::Enum>& validAxesLocationsOut,
                                                        ChartTwoCartesianAxis* &selectedAxisOut) const
{
    chartOverlaySetOut = NULL;
    validAxesLocationsOut.clear();
    selectedAxisOut = NULL;
    
    if (browserTabContent == NULL) {
        return;
    }
    
    const ChartAxisLocationEnum::Enum lastSelectedAxis = getSelectedAxisLocation();
    
    if (browserTabContent != NULL) {
        ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
        const int32_t tabIndex = browserTabContent->getTabNumber();
        if (modelChartTwo != NULL) {
            switch (modelChartTwo->getSelectedChartTwoDataType(tabIndex)) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    chartOverlaySetOut = modelChartTwo->getChartTwoOverlaySet(tabIndex);
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_LAYER:
                    chartOverlaySetOut = modelChartTwo->getChartTwoOverlaySet(tabIndex);
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    chartOverlaySetOut = modelChartTwo->getChartTwoOverlaySet(tabIndex);
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    break;
            }
            
            if (chartOverlaySetOut != NULL) {
                int32_t defaultAxisIndex = -1;
                
                std::vector<ChartTwoCartesianAxis*> axes;
                chartOverlaySetOut->getDisplayedChartAxes(axes);
                const int32_t numAxes = static_cast<int32_t>(axes.size());
                for (int32_t i = 0; i < numAxes; i++) {
                    const ChartAxisLocationEnum::Enum axisLocation = axes[i]->getAxisLocation();
                    if (lastSelectedAxis == axisLocation) {
                        defaultAxisIndex = i;
                    }
                    validAxesLocationsOut.push_back(axisLocation);
                }
                CaretAssert(validAxesLocationsOut.size() == axes.size());
                
                if (defaultAxisIndex < 0) {
                    /*
                     * If selected axis not found, switch to opposite axis
                     * User may have switched vertical axis from left to right
                     */
                    ChartAxisLocationEnum::Enum oppositeAxis = ChartAxisLocationEnum::getOppositeAxis(lastSelectedAxis);
                    for (int32_t i = 0; i < numAxes; i++) {
                        if (oppositeAxis == axes[i]->getAxisLocation()) {
                            defaultAxisIndex = i;
                            break;
                        }
                    }
                }
                
                if ( ! axes.empty()) {
                    if (defaultAxisIndex < 0) {
                        defaultAxisIndex = 0;
                    }
                    CaretAssertVectorIndex(axes, defaultAxisIndex);
                    selectedAxisOut = axes[defaultAxisIndex];
                }
            }
        }
    }
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


void
BrainBrowserWindowToolBarChartTwoAxes::updateControls(BrowserTabContent* browserTabContent)
{
    m_chartOverlaySet = NULL;
    std::vector<ChartAxisLocationEnum::Enum> validAxesLocations;
    ChartTwoCartesianAxis* selectedAxis = NULL;
    
    getSelectionData(browserTabContent,
                     m_chartOverlaySet,
                     validAxesLocations,
                     selectedAxis);
    
    m_widgetGroup->blockAllSignals(true);
    
    m_chartAxis = selectedAxis;
    if ((m_chartOverlaySet != NULL)
        && (m_chartAxis != NULL)) {
        m_axisComboBox->setupWithItems<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>(validAxesLocations);
        m_axisComboBox->setSelectedItem<ChartAxisLocationEnum, ChartAxisLocationEnum::Enum>(m_chartAxis->getAxisLocation());
        
        m_axisDisplayedByUserCheckBox->setChecked(m_chartAxis->isDisplayedByUser());
        m_autoUserRangeComboBox->setSelectedItem<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>(m_chartAxis->getScaleRangeMode());
        float rangeMin(0.0f), rangeMax(0.0f);
        m_chartAxis->getDataRange(rangeMin, rangeMax);
        m_userMinimumValueSpinBox->setRangeExceedable(rangeMin, rangeMax);
        m_userMinimumValueSpinBox->setValue(m_chartAxis->getUserScaleMinimumValue());
        m_userMaximumValueSpinBox->setRangeExceedable(rangeMin, rangeMax);
        m_userMaximumValueSpinBox->setValue(m_chartAxis->getUserScaleMaximumValue());
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
                m_axisLabelFromOverlayComboBox->addItem("Layer " + AString::number(j + 1));
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
    
    m_widgetGroup->blockAllSignals(false);
}

/**
 * Called when the axis line thickness changes.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::axisLineThicknessChanged(double)
{
    if (m_chartOverlaySet != NULL) {
        m_chartOverlaySet->setAxisLineThickness(m_linesTicksSizeSpinBox->value());
        
        const BrowserTabContent* tabContent = getTabContentFromSelectedTab();
        CaretAssert(tabContent);
        
        const YokingGroupEnum::Enum yokingGroup = tabContent->getChartModelYokingGroup();
        if (yokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
            const ModelChartTwo* modelChartTwo = tabContent->getDisplayedChartTwoModel();
            CaretAssert(modelChartTwo);
            const int32_t tabIndex = tabContent->getTabNumber();
            EventChartTwoAttributesChanged attributesEvent;
            attributesEvent.setLineThicknessChanged(yokingGroup,
                                                    modelChartTwo->getSelectedChartTwoDataType(tabIndex),
                                                    m_linesTicksSizeSpinBox->value());
            EventManager::get()->sendEvent(attributesEvent.getPointer());
        }
    }
    
    updateGraphics();
    
    updateContent(getTabContentFromSelectedTab());
}


/**
 * Called when a widget is changed by the user.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::valueChanged()
{
    CaretAssert(m_chartAxis);
    if (m_chartAxis != NULL) {
        m_chartAxis->setLabelOverlayIndex(m_axisLabelFromOverlayComboBox->currentIndex());
        m_chartAxis->setDisplayedByUser(m_axisDisplayedByUserCheckBox->isChecked());
        m_chartAxis->setScaleRangeMode(m_autoUserRangeComboBox->getSelectedItem<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>());
        m_chartAxis->setUserScaleMinimumValue(m_userMinimumValueSpinBox->value());
        m_chartAxis->setUserScaleMaximumValue(m_userMaximumValueSpinBox->value());
        m_chartAxis->setShowTickmarks(m_showTickMarksCheckBox->isChecked());
        m_chartAxis->setShowLabel(m_showLabelCheckBox->isChecked());
        m_chartAxis->setNumericsTextDisplayed(m_showNumericsCheckBox->isChecked());
        m_chartAxis->setNumericsTextRotated(m_rotateNumericsCheckBox->isChecked());
        m_chartAxis->setUserNumericFormat(m_userNumericFormatComboBox->getSelectedItem<NumericFormatModeEnum, NumericFormatModeEnum::Enum>());
        m_chartAxis->setUserDigitsRightOfDecimal(m_userDigitsRightOfDecimalSpinBox->value());
        m_chartAxis->setNumericSubdivsionsMode(m_numericSubdivisionsModeComboBox->getSelectedItem<ChartTwoNumericSubdivisionsModeEnum, ChartTwoNumericSubdivisionsModeEnum::Enum>());
        m_chartAxis->setUserNumberOfSubdivisions(m_userSubdivisionsSpinBox->value());
        
        m_chartAxis->setLabelTextSize(m_labelSizeSpinBox->value());
        m_chartAxis->setNumericsTextSize(m_numericsSizeSpinBox->value());
        m_chartAxis->setPaddingSize(m_paddingSizeSpinBox->value());
        
        const BrowserTabContent* tabContent = getTabContentFromSelectedTab();
        CaretAssert(tabContent);
        
        const YokingGroupEnum::Enum yokingGroup = tabContent->getChartModelYokingGroup();
        if (yokingGroup != YokingGroupEnum::YOKING_GROUP_OFF) {
            const ModelChartTwo* modelChartTwo = tabContent->getDisplayedChartTwoModel();
            CaretAssert(modelChartTwo);
            const int32_t tabIndex = tabContent->getTabNumber();
            EventChartTwoAttributesChanged attributesEvent;
            attributesEvent.setCartesianAxisChanged(yokingGroup,
                                                    modelChartTwo->getSelectedChartTwoDataType(tabIndex),
                                                    m_chartAxis);
            EventManager::get()->sendEvent(attributesEvent.getPointer());
        }
    }

    updateGraphics();
    
    updateContent(getTabContentFromSelectedTab());
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
 * Called when the minimum value is changed.
 *
 * @param minimumValue
 *     New minimum value.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::axisMinimumValueChanged(double minimumValue)
{
    if (m_chartAxis != NULL) {
        /*
         * If the minimum or maximum value is modified by user,
         * ensure Auto/User Range selection is USER
         */
        m_autoUserRangeComboBox->getWidget()->blockSignals(true);
        m_autoUserRangeComboBox->setSelectedItem<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>(ChartTwoAxisScaleRangeModeEnum::USER);
        m_autoUserRangeComboBox->getWidget()->blockSignals(false);
        
        /*
         * Ensure maximum value is always greater than or equal to minimum
         */
        if (minimumValue > m_userMaximumValueSpinBox->value()) {
            m_userMaximumValueSpinBox->getWidget()->blockSignals(true);
            m_userMaximumValueSpinBox->setValue(minimumValue);
            m_userMaximumValueSpinBox->getWidget()->blockSignals(false);
        }
        
        valueChanged();
    }
}

/**
 * Called when the maximum value is changed.
 *
 * @param maximumValue
 *     New maximum value.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::axisMaximumValueChanged(double maximumValue)
{
    if (m_chartAxis != NULL) {
        /*
         * If the minimum or maximum value is modified by user,
         * ensure Auto/User Range selection is USER
         */
        m_autoUserRangeComboBox->getWidget()->blockSignals(true);
        m_autoUserRangeComboBox->setSelectedItem<ChartTwoAxisScaleRangeModeEnum, ChartTwoAxisScaleRangeModeEnum::Enum>(ChartTwoAxisScaleRangeModeEnum::USER);
        m_autoUserRangeComboBox->getWidget()->blockSignals(false);

        /*
         * Ensure minimum value is always less than or equal to maximum
         */
        if (maximumValue < m_userMinimumValueSpinBox->value()) {
            m_userMinimumValueSpinBox->getWidget()->blockSignals(true);
            m_userMinimumValueSpinBox->setValue(maximumValue);
            m_userMinimumValueSpinBox->getWidget()->blockSignals(false);
        }
        
        valueChanged();
    }
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
 * Called when name toolbutton is clicked to change axis label.
 */
void
BrainBrowserWindowToolBarChartTwoAxes::axisLabelToolButtonClicked(bool)
{
    ChartTwoOverlaySet* chartOverlaySet = NULL;
    std::vector<ChartAxisLocationEnum::Enum> validAxesLocations;
    ChartTwoCartesianAxis* selectedAxis = NULL;
    
    getSelectionData(getTabContentFromSelectedTab(),
                     chartOverlaySet,
                     validAxesLocations,
                     selectedAxis);
    
    if ((chartOverlaySet != NULL)
        && (selectedAxis != NULL)) {
        WuQDataEntryDialog newNameDialog("Axis Label",
                                         m_axisLabelToolButton);
        QLineEdit* lineEdit = newNameDialog.addLineEditWidget("Label");
        lineEdit->setText(chartOverlaySet->getAxisLabel(selectedAxis));
        if (newNameDialog.exec() == WuQDataEntryDialog::Accepted) {
            const AString name = lineEdit->text().trimmed();
            chartOverlaySet->setAxisLabel(selectedAxis, name);
            valueChanged();
        }
    }
}




