
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__
#include "BrainBrowserWindowToolBarChartAttributes.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "EnumComboBoxTemplate.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectionModel.h"
#include "ChartMatrixDisplayProperties.h"
#include "ChartModelDataSeries.h"
#include "ChartModelFrequencySeries.h"
#include "ChartModelTimeSeries.h"
#include "ChartableMatrixInterface.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "ModelChart.h"
#include "WuQFactory.h"
#include "WuQWidgetObjectGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarChartAttributes 
 * \brief Controls for chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *   The parent toolbar.
 */
BrainBrowserWindowToolBarChartAttributes::BrainBrowserWindowToolBarChartAttributes(BrainBrowserWindowToolBar* parentToolBar)
: BrainBrowserWindowToolBarComponent(parentToolBar)
{
    m_cartesianChartAttributesWidget = new CartesianChartAttributesWidget(this);
    
    m_matrixChartAttributesWidget = new MatrixChartAttributesWidget(this);
    
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->addWidget(m_cartesianChartAttributesWidget);
    m_stackedWidget->addWidget(m_matrixChartAttributesWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_stackedWidget);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarChartAttributes::~BrainBrowserWindowToolBarChartAttributes()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarChartAttributes::updateContent(BrowserTabContent* /*browserTabContent*/)
{
    ChartModelCartesian* cartesianChart = getCartesianChart();
    ChartMatrixDisplayProperties* matrixProperties = getChartableMatrixDisplayProperties();
    
    if (cartesianChart != NULL) {
        m_stackedWidget->setCurrentWidget(m_cartesianChartAttributesWidget);
        m_cartesianChartAttributesWidget->updateContent();
        m_stackedWidget->setEnabled(true);
    }
    else if (matrixProperties) {
        m_stackedWidget->setCurrentWidget(m_matrixChartAttributesWidget);
        m_matrixChartAttributesWidget->updateContent();
        m_stackedWidget->setEnabled(true);
    }
    else {
        m_stackedWidget->setEnabled(false);
    }
}

/**
 * @return Cartesian chart in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Cartesian Chart.
 */
ChartModelCartesian*
BrainBrowserWindowToolBarChartAttributes::getCartesianChart()
{
    ChartModelCartesian* cartesianChart = NULL;
    
    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            const ChartVersionOneDataTypeEnum::Enum chartType = modelChart->getSelectedChartOneDataType(tabIndex);
            
            switch (chartType) {
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    cartesianChart = modelChart->getSelectedDataSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    cartesianChart = modelChart->getSelectedFrequencySeriesChartModel(tabIndex);
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    cartesianChart = modelChart->getSelectedTimeSeriesChartModel(tabIndex);  //dynamic_cast<ChartModelDataSeries*>(chart);
                    break;
            }
        }
    }
    
    return cartesianChart;
}

/**
 * @return Matrix chart interface in this widget.  Returned value will
 * be NULL if the chart (or no chart) is not a Matrix Chart.
 */
ChartMatrixDisplayProperties*
BrainBrowserWindowToolBarChartAttributes::getChartableMatrixDisplayProperties()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = NULL;

    BrowserTabContent* browserTabContent = getTabContentFromSelectedTab();
    if (browserTabContent != NULL) {
        ModelChart* modelChart = browserTabContent->getDisplayedChartModel();
        
        if (modelChart != NULL) {
            const int32_t tabIndex = browserTabContent->getTabNumber();
            const ChartVersionOneDataTypeEnum::Enum chartType = modelChart->getSelectedChartOneDataType(tabIndex);
            
            switch (chartType) {
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_INVALID:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER:
                {
                    ChartableMatrixInterface* matrixInterface = modelChart->getChartableMatrixParcelFileSelectionModel(tabIndex)->getSelectedFileOfType<ChartableMatrixInterface>();
                    if (matrixInterface != NULL) {
                        matrixDisplayProperties = matrixInterface->getChartMatrixDisplayProperties(tabIndex);
                    }
                }
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_MATRIX_SERIES:
                {
                    CaretDataFileSelectionModel* fileModel = modelChart->getChartableMatrixSeriesFileSelectionModel(tabIndex);
                    CaretDataFile* caretFile = fileModel->getSelectedFile();
                    if (caretFile != NULL) {
                        ChartableMatrixInterface* matrixInterface = dynamic_cast<ChartableMatrixInterface*>(caretFile);
                        if (matrixInterface != NULL) {
                            matrixDisplayProperties = matrixInterface->getChartMatrixDisplayProperties(tabIndex);
                        }
                    }
                }
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_DATA_SERIES:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_FREQUENCY_SERIES:
                    break;
                case ChartVersionOneDataTypeEnum::CHART_DATA_TYPE_LINE_TIME_SERIES:
                    break;
            }
        }
    }
    
    return matrixDisplayProperties;
}


/**
 * Update the graphics.
 */
void
BrainBrowserWindowToolBarChartAttributes::updateGraphics()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}




/* ===========================================================================*/

/**
 * \class caret::CartesianChartAttributesWidget
 * \brief Controls for cartesian chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param brainBrowserWindowToolBarChartAttributes
 *   The parent attributes widget.
 */
CartesianChartAttributesWidget::CartesianChartAttributesWidget(BrainBrowserWindowToolBarChartAttributes* brainBrowserWindowToolBarChartAttributes)
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
CartesianChartAttributesWidget::~CartesianChartAttributesWidget()
{
    
}

/**
 * Update the content of this widget.
 */
void
CartesianChartAttributesWidget::updateContent()
{
    ChartModelCartesian* chart = m_brainBrowserWindowToolBarChartAttributes->getCartesianChart();
    if (chart != NULL) {
        m_cartesianLineWidthDoubleSpinBox->blockSignals(true);
        m_cartesianLineWidthDoubleSpinBox->setValue(chart->getLineWidth());
        m_cartesianLineWidthDoubleSpinBox->blockSignals(false);
    }
}

/**
 * Called when the cartesian line width is changed.
 *
 * @param value
 *    New value for line width.
 */
void
CartesianChartAttributesWidget::cartesianLineWidthValueChanged(double value)
{
    ChartModelCartesian* chart = m_brainBrowserWindowToolBarChartAttributes->getCartesianChart();
    if (chart != NULL) {
        chart->setLineWidth(value);
        
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}




/* ===========================================================================*/



/**
 * \class caret::MatrixChartAttributesWidget
 * \brief Controls for matrix chart attributes.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param brainBrowserWindowToolBarChartAttributes
 *   The parent attributes widget.
 */
MatrixChartAttributesWidget::MatrixChartAttributesWidget(BrainBrowserWindowToolBarChartAttributes* brainBrowserWindowToolBarChartAttributes)
: QWidget(brainBrowserWindowToolBarChartAttributes),
EventListenerInterface()
{
    m_brainBrowserWindowToolBarChartAttributes = brainBrowserWindowToolBarChartAttributes;
    
    QLabel* cellWidthLabel = new QLabel("Cell Width");
    m_cellWidthSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(1.0,
                                                                                        1000.0,
                                                                                        0.1,
                                                                                        2,
                                                                                        this,
                                                                                        SLOT(cellWidthSpinBoxValueChanged(double)));
    m_cellWidthSpinBox->setKeyboardTracking(true);
    
    QLabel* cellHeightLabel = new QLabel("Cell Height");
    m_cellHeightSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(1.0,
                                                                                         1000.0,
                                                                                         0.1,
                                                                                         2,
                                                                                         this,
                                                                                         SLOT(cellHeightSpinBoxValueChanged(double)));
    m_cellHeightSpinBox->setKeyboardTracking(true);
    
    QAction* resetButtonAction = WuQtUtilities::createAction("Reset",
                                                             "Reset panning (SHIFT-mouse),zooming (CTRL-mouse), and scale matrix to fit window",
                                                             this,
                                                             this,
                                                             SLOT(resetButtonClicked()));
    QToolButton* resetToolButton = new QToolButton();
    resetToolButton->setDefaultAction(resetButtonAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(resetToolButton);
    
    WuQtUtilities::matchWidgetWidths(m_cellHeightSpinBox,
                                     m_cellWidthSpinBox);
    
    m_highlightSelectionCheckBox = new QCheckBox("Highlight Selection");
    QObject::connect(m_highlightSelectionCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(highlightSelectionCheckBoxClicked(bool)));
    
    m_displayGridLinesCheckBox = new QCheckBox("Show Grid Outline");
    QObject::connect(m_displayGridLinesCheckBox, SIGNAL(clicked(bool)),
                     this, SLOT(displayGridLinesCheckBoxClicked(bool)));
    
    m_manualWidgetsGroup = new WuQWidgetObjectGroup(this);
    m_manualWidgetsGroup->add(m_cellWidthSpinBox);
    m_manualWidgetsGroup->add(m_cellHeightSpinBox);
    m_manualWidgetsGroup->add(m_displayGridLinesCheckBox);
    m_manualWidgetsGroup->add(resetToolButton);
    
    const int32_t COLUMN_LABEL = 0;
    const int32_t COLUMN_WIDGET = 1;
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 2);
    int32_t rowIndex = gridLayout->rowCount();
    gridLayout->addWidget(cellWidthLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_cellWidthSpinBox, rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(cellHeightLabel, rowIndex, COLUMN_LABEL);
    gridLayout->addWidget(m_cellHeightSpinBox, rowIndex, COLUMN_WIDGET);
    rowIndex++;
    gridLayout->addWidget(resetToolButton, rowIndex, COLUMN_LABEL, 1, 2, Qt::AlignHCenter);
    rowIndex++;
    gridLayout->addWidget(m_highlightSelectionCheckBox, rowIndex, COLUMN_LABEL, 1, 2, Qt::AlignLeft);
    rowIndex++;
    gridLayout->addWidget(m_displayGridLinesCheckBox, rowIndex, COLUMN_LABEL, 1, 2, Qt::AlignLeft);
    rowIndex++;
    
    this->setFixedSize(this->sizeHint());
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
}

/**
 * Destructor.
 */
MatrixChartAttributesWidget::~MatrixChartAttributesWidget()
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
MatrixChartAttributesWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        EventBrowserWindowGraphicsRedrawn* redrawEvent = dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(event);
        
        /*
         * When the matrix view mode is auto, the OpenGL graphics update the size of
         * the matrix cells for use by manual mode (cell sizes are in pixels).
         */
        ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
        if (matrixDisplayProperties != NULL) {
            const BrowserTabContent* tabContent = m_brainBrowserWindowToolBarChartAttributes->getTabContentFromSelectedTab();
            if (tabContent->getTabNumber() == redrawEvent->getBrowserWindowIndex()) {
                updateContent();
            }
        }
    }
}


/**
 * Update the content of this widget.
 */
void
MatrixChartAttributesWidget::updateContent()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        m_cellWidthSpinBox->blockSignals(true);
        m_cellWidthSpinBox->setValue(matrixDisplayProperties->getCellWidth());
        m_cellWidthSpinBox->blockSignals(false);
        
        m_cellHeightSpinBox->blockSignals(true);
        m_cellHeightSpinBox->setValue(matrixDisplayProperties->getCellHeight());
        m_cellHeightSpinBox->blockSignals(false);
        
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
MatrixChartAttributesWidget::cellWidthSpinBoxValueChanged(double value)
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL);
        matrixDisplayProperties->setCellWidth(value);
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
MatrixChartAttributesWidget::cellHeightSpinBoxValueChanged(double value)
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setScaleMode(ChartMatrixScaleModeEnum::CHART_MATRIX_SCALE_MANUAL);
        matrixDisplayProperties->setCellHeight(value);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}

/**
 * Called when the reset button is clicked.
 */
void
MatrixChartAttributesWidget::resetButtonClicked()
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->resetPropertiesToDefault();
        updateContent();
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
MatrixChartAttributesWidget::highlightSelectionCheckBoxClicked(bool checked)
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
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
MatrixChartAttributesWidget::displayGridLinesCheckBoxClicked(bool checked)
{
    ChartMatrixDisplayProperties* matrixDisplayProperties = m_brainBrowserWindowToolBarChartAttributes->getChartableMatrixDisplayProperties();
    if (matrixDisplayProperties != NULL) {
        matrixDisplayProperties->setGridLinesDisplayed(checked);
        m_brainBrowserWindowToolBarChartAttributes->updateGraphics();
    }
}


