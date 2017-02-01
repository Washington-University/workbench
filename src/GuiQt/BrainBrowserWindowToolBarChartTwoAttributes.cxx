
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
#include <QStackedWidget>
#include <QToolButton>

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_DECLARE__
#include "BrainBrowserWindowToolBarChartTwoAttributes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "EnumComboBoxTemplate.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "ChartTwoMatrixDisplayProperties.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChartTwo.h"
#include "WuQFactory.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartTwoAttributes 
 * \brief Controls for chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *   The parent toolbar.
 */
BrainBrowserWindowToolBarChartTwoAttributes::BrainBrowserWindowToolBarChartTwoAttributes(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_cartesianChartAttributesWidget = new CartesianChartTwoAttributesWidget(this);
    
    m_matrixChartTwoAttributesWidget = new MatrixChartTwoAttributesWidget(this);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_cartesianChartAttributesWidget);
    m_stackedWidget->addWidget(m_matrixChartTwoAttributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_stackedWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartTwoAttributes::~BrainBrowserWindowToolBarChartTwoAttributes()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartTwoAttributes::updateContent(BrowserTabContent* browserTabContent)
{
    if (browserTabContent != NULL) {
        ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
        const int32_t tabIndex = browserTabContent->getTabNumber();
        if (modelChartTwo != NULL) {
            switch (modelChartTwo->getSelectedChartTwoDataType(tabIndex)) {
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_HISTOGRAM:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES:
                    break;
                case ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX:
                    m_stackedWidget->setCurrentWidget(m_matrixChartTwoAttributesWidget);
                    m_matrixChartTwoAttributesWidget->updateContent();
                    break;
            }
        }
    }
    
//    ChartModelCartesian* cartesianChart = getCartesianChart();
//    ChartTwoMatrixDisplayProperties* matrixProperties = getChartableMatrixDisplayProperties();
//    
//    if (cartesianChart != NULL) {
//        m_stackedWidget->setCurrentWidget(m_cartesianChartAttributesWidget);
//        m_cartesianChartAttributesWidget->updateContent();
//        m_stackedWidget->setEnabled(true);
//    }
//    else if (matrixProperties) {
//        m_stackedWidget->setCurrentWidget(m_matrixChartAttributesWidget);
//        m_matrixChartAttributesWidget->updateContent();
//        m_stackedWidget->setEnabled(true);
//    }
//    else {
//        m_stackedWidget->setEnabled(false);
//    }
}

///**
// * @return Cartesian chart in this widget.  Returned value will
// * be NULL if the chart (or no chart) is not a Cartesian Chart.
// */
//ChartModelCartesian*
//BrainBrowserWindowToolBarChartTwoAttributes::getCartesianChart()
//{
//    ChartModelCartesian* cartesianChart = NULL;
//    
//    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
//    if (browserTabContent != NULL) {
//        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
//        
//        if (modelChart != NULL) {
//            const int32_t tabIndex = browserTabContent->getTabNumber();
//            const ChartOneDataTypeEnum::Enum chartType = modelChart->getSelectedChartOneDataType(tabIndex);
//            
//            switch (chartType) {
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
//                    cartesianChart = modelChart->getSelectedDataSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
//                    cartesianChart = modelChart->getSelectedFrequencySeriesChartModel(tabIndex);
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
//                    cartesianChart = modelChart->getSelectedTimeSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
//                    break;
//            }
//        }
//    }
//    
//    return cartesianChart;
//}

/**
 * @return Matrix chart interface in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Matrix Chart.
 */
ChartTwoMatrixDisplayProperties*
BrainBrowserWindowToolBarChartTwoAttributes::getChartableTwoMatrixDisplayProperties()
{
    ChartTwoMatrixDisplayProperties* matrixDisplayProperties = NULL;

    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChartTwo* modelChartTwo = browserTabContent->getDisplayedChartTwoModel();
        matrixDisplayProperties = modelChartTwo->getChartTwoMatrixDisplayProperties(browserTabContent->getTabNumber());
//
//        if (modelChart != NULL) {
//            const int32_t tabIndex = browserTabContent->getTabNumber();
//            const ChartOneDataTypeEnum::Enum chartType = modelChart->getSelectedChartOneDataType(tabIndex);
//            
//            switch (chartType) {
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
//                {
//                    ChartableMatrixInterface* matrixInterface = modelChart->getChartableMatrixParcelFileSelectionModel(tabIndex)->getSelectedFileOfType<ChartableMatrixInterface>();
//                    if (matrixInterface != NULL) {
//                        matrixDisplayProperties = matrixInterface->getChartTwoMatrixDisplayProperties(tabIndex);
//                    }
//                }
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
//                {
//                    CaretDataFileSelectionModel* fileModel = modelChart->getChartableMatrixSeriesFileSelectionModel(tabIndex);
//                    CaretDataFile* caretFile = fileModel->getSelectedFile();
//                    if (caretFile != NULL) {
//                        ChartableMatrixInterface* matrixInterface = dynamic_cast<ChartableMatrixInterface*>(caretFile);
//                        if (matrixInterface != NULL) {
//                            matrixDisplayProperties = matrixInterface->getChartTwoMatrixDisplayProperties(tabIndex);
//                        }
//                    }
//                }
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
//                    break;
//                case ChartOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
//                    break;
//            }
//        }
    }
    
    return matrixDisplayProperties;
}


/**
 * Update the graphics.
 */
void
BrainBrowserWindowToolBarChartTwoAttributes::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}




/* ===========================================================================*/

/**
 * \class caret::CartesianChartTwoAttributesWidget
 * \brief Controls for cartesian chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param brainBrowserWindowToolBarChartAttributes
 *   The parent attributes widget.
 */
CartesianChartTwoAttributesWidget::CartesianChartTwoAttributesWidget(BrainBrowserWindowToolBarChartTwoAttributes* brainBrowserWindowToolBarChartAttributes)
: QWidget(brainBrowserWindowToolBarChartAttributes)
{
    m_brainBrowserWindowToolBarChartAttributes = brainBrowserWindowToolBarChartAttributes;
    
    QLabel* cartesianLineWidthLabel = new QLabel("Line width ");
    m_cartesianLineWidthDoubleSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.0,
                                                                                                       10000.0,
                                                                                                       0.1,
                                                                                                       1,
                                                                                                       this,
                                                                                                       SLOT(cartesianLineWidthValueChanged(double)));
    m_cartesianLineWidthDoubleSpinBox->setFixedWidth(65);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 0, 0);
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(0, 100);
    gridLayout->addWidget(cartesianLineWidthLabel, 0, 0);
    gridLayout->addWidget(m_cartesianLineWidthDoubleSpinBox, 0, 1);
    
    this->setFixedSize(this->sizeHint());
}

/**
 * Destructor.
 */
CartesianChartTwoAttributesWidget::~CartesianChartTwoAttributesWidget()
{
    
}

/**
 * Update the content of this widget.
 */
void
CartesianChartTwoAttributesWidget::updateContent()
{
//    ChartModelCartesian* chart = m_brainBrowserWindowToolBarChartAttributes->getCartesianChart();
//    if (chart != NULL) {
//        m_cartesianLineWidthDoubleSpinBox->blockSignals(true);
//        m_cartesianLineWidthDoubleSpinBox->setValue(chart->getLineWidth());
//        m_cartesianLineWidthDoubleSpinBox->blockSignals(false);
//    }
}

/**
 * Called when the cartesian line width is changed.
 *
 * @param value
 *    New value for line width.
 */
void
CartesianChartTwoAttributesWidget::cartesianLineWidthValueChanged(double value)
{
//    ChartModelCartesian* chart = m_brainBrowserWindowToolBarChartAttributes->getCartesianChart();
//    if (chart != NULL) {
//        chart->setLineWidth(value);
//        
//        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
//    }
}




/* ===========================================================================*/



/**
 * \class caret::MatrixChartTwoAttributesWidget
 * \brief Controls for matrix chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param brainBrowserWindowToolBarChartAttributes
 *   The parent attributes widget.
 */
MatrixChartTwoAttributesWidget::MatrixChartTwoAttributesWidget(BrainBrowserWindowToolBarChartTwoAttributes* brainBrowserWindowToolBarChartAttributes)
: QWidget(brainBrowserWindowToolBarChartAttributes),
EventListenerInterface()
{
    m_brainBrowserWindowToolBarChartAttributes = brainBrowserWindowToolBarChartAttributes;
    
    QLabel* cellWidthLabel = new QLabel("Cell Width");
    const float minPercent = 1.0;
    const float maxPercent = 100000.0;
    m_cellWidthPercentageSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(minPercent,
                                                                                        maxPercent,
                                                                                        1.0,
                                                                                        1,
                                                                                        this,
                                                                                        SLOT(cellWidthPercentageSpinBoxValueChanged(double)));
    m_cellWidthPercentageSpinBox->setKeyboardTracking(true);
    m_cellWidthPercentageSpinBox->setSuffix("%");
    
    QLabel* cellHeightLabel = new QLabel("Cell Height");
    m_cellHeightPercentageSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(minPercent,
                                                                                         maxPercent,
                                                                                         1.0,
                                                                                         1,
                                                                                         this,
                                                                                         SLOT(cellHeightPercentageSpinBoxValueChanged(double)));
    m_cellHeightPercentageSpinBox->setKeyboardTracking(true);
    m_cellHeightPercentageSpinBox->setSuffix("%");
    
    WuQtUtilities::matchWidgetWidths(m_cellHeightPercentageSpinBox,
                                     m_cellWidthPercentageSpinBox);
    
    m_highlightSelectionCheckBox = new QCheckBox("Highlight Selection");
    QObject::connect(m_highlightSelectionCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(highlightSelectionCheckBoxClicked(bool)));
    
    m_displayGridLinesCheckBox = new QCheckBox("Show Grid Outline");
    QObject::connect(m_displayGridLinesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(displayGridLinesCheckBoxClicked(bool)));
    
    m_manualWidgetsGroup = new WuQWidgetObjectGroup(this);
    m_manualWidgetsGroup->add(m_cellWidthPercentageSpinBox);
    m_manualWidgetsGroup->add(m_cellHeightPercentageSpinBox);
    m_manualWidgetsGroup->add(m_displayGridLinesCheckBox);
    
    const int32_t COLUMN_LABEL  = 0;
    const int32_t COLUMN_WIDGET = 1;
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(cellWidthLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_cellWidthPercentageSpinBox, rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(cellHeightLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_cellHeightPercentageSpinBox, rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(m_highlightSelectionCheckBox, rowIndex, COLUMN_LABEL, 1, 2, Qt::AlignLeft);
    rowIndex++;
    gridLayout->addWidget(m_displayGridLinesCheckBox, rowIndex, COLUMN_LABEL, 1, 2, Qt::AlignLeft);
    rowIndex++;
    
    this->setFixedSize(this->sizeHint());
}

/**
 * Destructor.
 */
MatrixChartTwoAttributesWidget::~MatrixChartTwoAttributesWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    The event.
 */
void
MatrixChartTwoAttributesWidget::receiveEvent(Event* event)
{
}


/**
 * Update the content of this widget.
 */
void
MatrixChartTwoAttributesWidget::updateContent()
{
    ChartTwoMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableTwoMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        m_cellWidthPercentageSpinBox->blockSignals(true);
        m_cellWidthPercentageSpinBox->setValue(matrixDisplayProperties->getCellPercentageZoomWidth());
        m_cellWidthPercentageSpinBox->blockSignals(false);
        
        m_cellHeightPercentageSpinBox->blockSignals(true);
        m_cellHeightPercentageSpinBox->setValue(matrixDisplayProperties->getCellPercentageZoomHeight());
        m_cellHeightPercentageSpinBox->blockSignals(false);
        
        m_highlightSelectionCheckBox->blockSignals(true);
        m_highlightSelectionCheckBox->setChecked(matrixDisplayProperties->isSelectedRowColumnHighlighted());
        m_highlightSelectionCheckBox->blockSignals(false);
        
        m_displayGridLinesCheckBox->blockSignals(true);
        m_displayGridLinesCheckBox->setChecked(matrixDisplayProperties->isGridLinesDisplayed());
        m_displayGridLinesCheckBox->blockSignals(false);
    }
}

/**
 * Called when the cell width spin box value is changed.
 *
 * @param value
 *    New value for cell width.
 */
void
MatrixChartTwoAttributesWidget::cellWidthPercentageSpinBoxValueChanged(double value)
{
    ChartTwoMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableTwoMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setCellPercentageZoomWidth(value);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the cell height spin box value is changed.
 *
 * @param value
 *    New value for cell height.
 */
void
MatrixChartTwoAttributesWidget::cellHeightPercentageSpinBoxValueChanged(double value)
{
    ChartTwoMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableTwoMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setCellPercentageZoomHeight(value);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the show selection check box is checked.
 *
 * @param checked
 *    New checked status.
 */
void
MatrixChartTwoAttributesWidget::highlightSelectionCheckBoxClicked(bool checked)
{
    ChartTwoMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableTwoMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setSelectedRowColumnHighlighted(checked);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the display grid lines check box is checked.
 *
 * @param checked
 *    New checked status.
 */
void
MatrixChartTwoAttributesWidget::displayGridLinesCheckBoxClicked(bool checked)
{
    ChartTwoMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableTwoMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setGridLinesDisplayed(checked);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}


