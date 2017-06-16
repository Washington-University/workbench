
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __CHART_TWO_LINE_SERIES_HISTORY_DIALOG_DECLARE__
#include "ChartTwoLineSeriesHistoryDialog.h"
#undef __CHART_TWO_LINE_SERIES_HISTORY_DIALOG_DECLARE__

#include <QComboBox>
#include <QLabel>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTableWidget>
#include <QToolButton>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "CaretMappableDataFile.h"
#include "ChartTwoDataCartesian.h"
#include "ChartableTwoFileDelegate.h"
#include "ChartableTwoFileLineSeriesChart.h"
#include "ChartTwoLineSeriesHistory.h"
#include "ChartTwoOverlay.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "MapFileDataSelector.h"
#include "WuQImageLabel.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ChartTwoLineSeriesHistoryDialog 
 * \brief Dialog for controlling the display of line series chart data.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
ChartTwoLineSeriesHistoryDialog::ChartTwoLineSeriesHistoryDialog(QWidget* parent)
: WuQDialogNonModal("Line-Series History",
                    parent)
{
    const AString defaultColorToolTip("Color assigned to newly added line-series items");
    const AString displayMaximumToolTip("Maximum number of line-series items enabled for viewing\n"
                                        "As new items are added, older items are disabled for viewing");
    
    m_removeAllHistoryIcon = WuQtUtilities::loadIcon(":/SpecFileDialog/delete_icon.png");
    
    m_filenameLabel = new QLabel("");
    
    QLabel* defaultColorLabel = new QLabel("Default Color: ");
    defaultColorLabel->setToolTip(defaultColorToolTip);
    m_defaultColorComboBox = new CaretColorEnumComboBox(this);
    m_defaultColorComboBox->getWidget()->setToolTip(defaultColorToolTip);
    m_defaultColorComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    QObject::connect(m_defaultColorComboBox, &CaretColorEnumComboBox::colorSelected,
                     this, &ChartTwoLineSeriesHistoryDialog::defaultColorSelected);
    
    QToolButton* removeAllHistoryToolButton = new QToolButton();
    removeAllHistoryToolButton->setText("Remove All History...");
    QObject::connect(removeAllHistoryToolButton, &QToolButton::clicked,
                     this, &ChartTwoLineSeriesHistoryDialog::removeAllHistoryButtonClicked);
    
    QLabel* viewedMaximumLabel = new QLabel("Viewed Maximum: ");
    viewedMaximumLabel->setToolTip(displayMaximumToolTip);
    m_viewedMaximumCountSpinBox = new QSpinBox();
    m_viewedMaximumCountSpinBox->setToolTip(displayMaximumToolTip);
    m_viewedMaximumCountSpinBox->setRange(1, 10000);
    QObject::connect(m_viewedMaximumCountSpinBox, SIGNAL(valueChanged(int)),
                     this, SLOT(viewedMaximumSpinBoxValueChanged(int)));
    
    m_removeHistoryItemSignalMapper = new QSignalMapper(this);
    QObject::connect(m_removeHistoryItemSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(removeHistoryItemSelected(int)));
    
    m_colorItemSignalMapper = new QSignalMapper(this);
    QObject::connect(m_colorItemSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(colorItemSelected(int)));
    
    m_tableWidget = new QTableWidget();
    m_tableWidget->setSelectionBehavior(QTableWidget::SelectItems);
    m_tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    QObject::connect(m_tableWidget, &QTableWidget::cellChanged,
                     this, &ChartTwoLineSeriesHistoryDialog::tableWidgetCellChanged);
    
    QGridLayout* topLayout = new QGridLayout();
    topLayout->setColumnStretch(0, 0);
    topLayout->setColumnStretch(1, 0);
    topLayout->setColumnStretch(2, 0);
    topLayout->setColumnStretch(3, 100);
    topLayout->addWidget(defaultColorLabel, 0, 0);
    topLayout->addWidget(m_defaultColorComboBox->getWidget(), 0, 1);
    topLayout->addWidget(m_filenameLabel, 0, 2, 1, 2, Qt::AlignLeft);
    topLayout->addWidget(viewedMaximumLabel, 1, 0);
    topLayout->addWidget(m_viewedMaximumCountSpinBox, 1, 1);
    topLayout->addWidget(removeAllHistoryToolButton, 1, 2);
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addLayout(topLayout);
    dialogLayout->addWidget(m_tableWidget);
    
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
    
    setApplyButtonText("");
}

/**
 * Destructor.
 */
ChartTwoLineSeriesHistoryDialog::~ChartTwoLineSeriesHistoryDialog()
{
}

/**
 * Update the dialog.
 */
void
ChartTwoLineSeriesHistoryDialog::updateDialog()
{
    updateDialogContentPrivate();
}

/**
 * @return The chart overlay in this dialog (May be NULL !)
 */
ChartTwoOverlay*
ChartTwoLineSeriesHistoryDialog::getChartOverlay()
{
    ChartTwoOverlay* chartOverlay = NULL;
    if (m_chartOverlayWeakPointer.expired()) {
        m_chartOverlayWeakPointer.reset();
    }
    else {
        chartOverlay = m_chartOverlayWeakPointer.lock().get();
    }
    
    return chartOverlay;
}


/**
 * Update dialog if the given chart overlay is in the dialog.
 * This may occur when the user changes the file in the chart overlay.
 *
 * @param chartOverlay
 *     Chart overlay for history display.
 */
void
ChartTwoLineSeriesHistoryDialog::updateIfThisChartOverlayIsInDialog(const ChartTwoOverlay* chartOverlay)
{
    ChartTwoOverlay* currentChartOverlay = getChartOverlay();
    
    if (currentChartOverlay != NULL) {
        if (chartOverlay != currentChartOverlay) {
            return;
        }
    }
    
    updateDialogContentPrivate();
}

/**
 * Update the content of the dialog.
 * 
 * @param chartOverlay
 *     Chart overlay for history display.
 */
void
ChartTwoLineSeriesHistoryDialog::updateDialogContent(ChartTwoOverlay* chartOverlay)
{
    CaretAssert(chartOverlay);
    m_chartOverlayWeakPointer = chartOverlay->getWeakPointerToSelf();
    updateDialogContentPrivate();
}

void
ChartTwoLineSeriesHistoryDialog::updateDialogContentPrivate()
{
    ChartTwoOverlay* chartOverlay = getChartOverlay();
    if (chartOverlay == NULL) {
        close();
        return;
    }
    
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    
    if (lineSeriesHistory != NULL) {
        const CaretMappableDataFile* mapFile = getMapFile();
        if (mapFile != NULL) {
            m_filenameLabel->setText("Filename: " + mapFile->getFileNameNoPath());
            m_filenameLabel->setToolTip(mapFile->getFileName());
        }
        m_defaultColorComboBox->setSelectedColor(lineSeriesHistory->getDefaultColor());
        
        m_viewedMaximumCountSpinBox->blockSignals(true);
        m_viewedMaximumCountSpinBox->setValue(lineSeriesHistory->getDisplayCount());
        m_viewedMaximumCountSpinBox->blockSignals(false);
        
        loadHistoryIntoTableWidget(lineSeriesHistory);
    }
    else {
        close();
    }
}


/**
 * @return Map file in the dialog.
 */
CaretMappableDataFile*
ChartTwoLineSeriesHistoryDialog::getMapFile()
{
    CaretMappableDataFile* mapFile = NULL;
    ChartTwoOverlay* chartOverlay = getChartOverlay();
    if (chartOverlay != NULL) {
        if (chartOverlay->getChartTwoDataType() == ChartTwoDataTypeEnum::CHART_DATA_TYPE_LINE_SERIES) {
            ChartTwoOverlay::SelectedIndexType indexType = ChartTwoOverlay::SelectedIndexType::INVALID;
            int32_t mapIndex = -1;
            chartOverlay->getSelectionData(mapFile, indexType, mapIndex);
        }
    }
    
    return mapFile;
}

/**
 * @return The line series history for the current chart overlay
 */
ChartTwoLineSeriesHistory*
ChartTwoLineSeriesHistoryDialog::getLineSeriesHistory()
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = NULL;
    
    CaretMappableDataFile* mapFile = getMapFile();
    if (mapFile != NULL) {
        ChartableTwoFileLineSeriesChart* chartTwoLineSeries = mapFile->getChartingDelegate()->getLineSeriesCharting();
        lineSeriesHistory = chartTwoLineSeries->getHistory();
    }

    return lineSeriesHistory;
}


/**
 * Load the line-series history into the table widget.
 *
 * @param lineSeriesHistory
 *     The line series history.
 */
void
ChartTwoLineSeriesHistoryDialog::loadHistoryIntoTableWidget(ChartTwoLineSeriesHistory* lineSeriesHistory)
{
    CaretAssert(lineSeriesHistory);
    
    m_tableWidget->blockSignals(true);
    
    const int32_t preNumRows = m_tableWidget->rowCount();
    const int32_t numHistory = lineSeriesHistory->getHistoryCount();
    
    /*
     * Setup table dimensions
     */
    m_tableWidget->setRowCount(numHistory);
    m_tableWidget->setColumnCount(COLUMN_COUNT);
    
    /*
     * May need to reduce color combo boxes
     */
    m_colorComboBoxes.resize(preNumRows);
    
    /*
     * If needed, add cells
     */
    for (int32_t iRow = preNumRows; iRow < numHistory; iRow++) {
        for (int32_t j = 0; j < COLUMN_COUNT; j++) {
            if (j == COLUMN_VIEW) {
                QTableWidgetItem* item = new QTableWidgetItem("");
                item->setFlags(Qt::ItemIsEnabled
                               | Qt::ItemIsSelectable
                               | Qt::ItemIsUserCheckable);
                m_tableWidget->setItem(iRow, j, item);
            }
            else if (j == COLUMN_REMOVE) {
                WuQImageLabel* removeHistoryLabel = new WuQImageLabel(m_removeAllHistoryIcon,
                                                                      "Remove");
                QObject::connect(removeHistoryLabel, SIGNAL(clicked()),
                                 m_removeHistoryItemSignalMapper, SLOT(map()));
                m_removeHistoryItemSignalMapper->setMapping(removeHistoryLabel, iRow);
                m_tableWidget->setCellWidget(iRow,
                                             COLUMN_REMOVE,
                                             removeHistoryLabel);
            }
            else if (j == COLUMN_COLOR) {
                CaretColorEnumComboBox* caretColorComboBox = new CaretColorEnumComboBox(this);
                QObject::connect(caretColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                                 m_colorItemSignalMapper, SLOT(map()));
                m_colorItemSignalMapper->setMapping(caretColorComboBox, iRow);
                m_tableWidget->setCellWidget(iRow,
                                             COLUMN_COLOR,
                                             caretColorComboBox->getWidget());
                m_colorComboBoxes.push_back(caretColorComboBox);
            }
            else if (j == COLUMN_DESCRIPTION) {
                QTableWidgetItem* item = new QTableWidgetItem("");
                item->setFlags(Qt::ItemIsEnabled
                               | Qt::ItemIsSelectable);
                m_tableWidget->setItem(iRow, j, item);
           }
            else {
                CaretAssert(0);
            }
        }
    }
    
    /*
     * Load cells
     */
    for (int32_t iRow = 0; iRow < numHistory; iRow++) {
        ChartTwoDataCartesian* data = lineSeriesHistory->getHistoryItem(iRow);
        const MapFileDataSelector* mapFileDataSelector = data->getMapFileDataSelector();
        
        m_tableWidget->item(iRow, COLUMN_VIEW)->setCheckState(data->isSelected()
                                                           ? Qt::Checked
                                                           : Qt::Unchecked);
        
        CaretAssertVectorIndex(m_colorComboBoxes, iRow);
        m_colorComboBoxes[iRow]->setSelectedColor(data->getColor());
        
        QTableWidgetItem* item = m_tableWidget->item(iRow, COLUMN_DESCRIPTION);
        item->setText(mapFileDataSelector->toString());
    }
    
    /*
     * If table was empty, add column titles
     * and adjust column widths
     */
    if ((preNumRows == 0)
        && (numHistory > 0)) {
        m_tableWidget->setHorizontalHeaderItem(COLUMN_VIEW,
                                               new QTableWidgetItem("View"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_REMOVE,
                                               new QTableWidgetItem("Remove"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_COLOR,
                                               new QTableWidgetItem("Color"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_DESCRIPTION,
                                               new QTableWidgetItem("Description"));
        
        m_tableWidget->resizeColumnToContents(COLUMN_VIEW);
        m_tableWidget->resizeColumnToContents(COLUMN_REMOVE);
        m_tableWidget->resizeColumnToContents(COLUMN_COLOR);
        m_tableWidget->setColumnWidth(COLUMN_DESCRIPTION,
                                      350);
        
        QSize tableSize = WuQtUtilities::estimateTableWidgetSize(m_tableWidget);
        
        WuQtUtilities::resizeWindow(this,
                                    tableSize.width() + 30,
                                    sizeHint().height());
    }
    
    m_tableWidget->blockSignals(false);
}

/**
 * Called when the content of a cell changes.
 * Update corresponding item in the spec file.
 *
 * @param rowIndex
 *    The row of the cell that was clicked.
 * @param columnIndex
 *    The columnof the cell that was clicked.
 */
void
ChartTwoLineSeriesHistoryDialog::tableWidgetCellChanged(int rowIndex, int columnIndex)
{
    QTableWidgetItem* item = m_tableWidget->item(rowIndex, columnIndex);
    if (item != NULL) {
        if (columnIndex == COLUMN_VIEW) {
            ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
            if (lineSeriesHistory != NULL) {
                ChartTwoDataCartesian* data = lineSeriesHistory->getHistoryItem(rowIndex);
                CaretAssert(data);
                data->setSelected(WuQtUtilities::checkStateToBool(item->checkState()));
                
                EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            }
        }
    }
}

/**
 * Remove a history item selected
 *
 * @param rowIndex
 *     Row index of item.
 */
void
ChartTwoLineSeriesHistoryDialog::removeHistoryItemSelected(int rowIndex)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    lineSeriesHistory->removeHistoryItem(rowIndex);
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Color item selected
 *
 * @param rowIndex
 *     Row index of item.
 */
void
ChartTwoLineSeriesHistoryDialog::colorItemSelected(int rowIndex)
{
    CaretAssertVectorIndex(m_colorComboBoxes, rowIndex);
    CaretAssert(m_colorComboBoxes[rowIndex]);
    
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        ChartTwoDataCartesian* data = lineSeriesHistory->getHistoryItem(rowIndex);
        CaretAssert(data);
        data->setColor(m_colorComboBoxes[rowIndex]->getSelectedColor());
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when the "Remove All History" button is clicked.
 */
void
ChartTwoLineSeriesHistoryDialog::removeAllHistoryButtonClicked()
{
    if (WuQMessageBox::warningOkCancel(this,
                                       ("Remove all line series history from the file "
                                        + getMapFile()->getFileNameNoPath()))) {
        ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
        lineSeriesHistory->clearHistory();
        updateDialog();
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when the default color is changed.
 *
 * @param color
 *     New default color.
 */
void
ChartTwoLineSeriesHistoryDialog::defaultColorSelected(const CaretColorEnum::Enum color)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        lineSeriesHistory->setDefaultColor(color);
    }
}

/**
 * Gets called when display last count is changed.
 *
 * @param num
 *     Number of history items to display.
 */
void
ChartTwoLineSeriesHistoryDialog::viewedMaximumSpinBoxValueChanged(int num)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        lineSeriesHistory->setDisplayCount(num);
        updateDialog();
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}