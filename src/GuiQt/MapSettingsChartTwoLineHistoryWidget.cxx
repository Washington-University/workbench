
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

#define __MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_DECLARE__
#include "MapSettingsChartTwoLineHistoryWidget.h"
#undef __MAP_SETTINGS_CHART_TWO_LINE_HISTORY_WIDGET_DECLARE__

#include <QComboBox>
#include <QLabel>
#include <QPainter>
#include <QSignalBlocker>
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
#include "WuQDoubleSpinBox.h"
#include "WuQImageLabel.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::MapSettingsChartTwoLineHistoryWidget 
 * \brief Dialog for controlling the display of line series chart data.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsChartTwoLineHistoryWidget::MapSettingsChartTwoLineHistoryWidget(QWidget* parent)
: QWidget(parent)
{
    const AString maxHistoryString(AString::number(ChartTwoLineSeriesHistory::getMaximumRetainedHistoryCount()));
    const AString defaultColorToolTip("Color assigned to newly added line-series items");
    const AString displayMaximumToolTip("Maximum number of line-series items enabled for viewing\n"
                                        "As new items are added, older items are disabled for viewing\n"
                                        "The number of history items retained (including those not\n"
                                        "displayed) is limited to this value or " + maxHistoryString + ", whichever is larger.");
    
    m_removeAllHistoryIcon = WuQtUtilities::loadIcon(":/SpecFileDialog/delete_icon.png");
    
    QLabel* defaultColorLabel = new QLabel("Default Color: ");
    defaultColorLabel->setToolTip(defaultColorToolTip);
    m_defaultColorComboBox = new CaretColorEnumComboBox(this);
    m_defaultColorComboBox->getWidget()->setToolTip(defaultColorToolTip);
    m_defaultColorComboBox->getComboBox()->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    QObject::connect(m_defaultColorComboBox, &CaretColorEnumComboBox::colorSelected,
                     this, &MapSettingsChartTwoLineHistoryWidget::defaultColorSelected);
    
    QLabel* defaultLineWidthLabel = new QLabel("Default Line Width: ");
    m_defaultLineWidthSpinBox = new WuQDoubleSpinBox(this);
    m_defaultLineWidthSpinBox->setRangePercentage(0.0, 100.0);
    m_defaultLineWidthSpinBox->setToolTip("Default width of new chart lines");
    defaultLineWidthLabel->setToolTip(m_defaultLineWidthSpinBox->getWidget()->toolTip());
    QObject::connect(m_defaultLineWidthSpinBox, static_cast<void (WuQDoubleSpinBox::*)(double)>(&WuQDoubleSpinBox::valueChanged),
                     this, &MapSettingsChartTwoLineHistoryWidget::defaultLineWidthChanged);
    
    QToolButton* removeAllHistoryToolButton = new QToolButton();
    removeAllHistoryToolButton->setText("Remove All History...");
    QObject::connect(removeAllHistoryToolButton, &QToolButton::clicked,
                     this, &MapSettingsChartTwoLineHistoryWidget::removeAllHistoryButtonClicked);
    
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
    
    m_lineWidthItemSignalMapper = new QSignalMapper(this);
    QObject::connect(m_lineWidthItemSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(lineWidthItemSelected(int)));
    
    m_tableWidget = new QTableWidget();
    m_tableWidget->setSelectionBehavior(QTableWidget::SelectItems);
    m_tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    QObject::connect(m_tableWidget, &QTableWidget::cellChanged,
                     this, &MapSettingsChartTwoLineHistoryWidget::tableWidgetCellChanged);
    
    QGridLayout* topLayout = new QGridLayout();
    topLayout->setColumnStretch(0, 0);
    topLayout->setColumnStretch(1, 0);
    topLayout->setColumnStretch(2, 0);
    topLayout->setColumnStretch(3, 0);
    topLayout->setColumnStretch(4, 100);
    int gridRow = 0;
    topLayout->addWidget(defaultLineWidthLabel, gridRow, 0);
    topLayout->addWidget(m_defaultLineWidthSpinBox->getWidget(), gridRow, 1);
    topLayout->addWidget(defaultColorLabel, gridRow, 2);
    topLayout->addWidget(m_defaultColorComboBox->getWidget(), gridRow, 3);
    gridRow++;
    topLayout->addWidget(viewedMaximumLabel, gridRow, 0);
    topLayout->addWidget(m_viewedMaximumCountSpinBox, gridRow, 1);
    topLayout->addWidget(removeAllHistoryToolButton, gridRow, 2, 1, 2, Qt::AlignLeft);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout, 0);
    layout->addWidget(m_tableWidget, 100);
}

/**
 * Destructor.
 */
MapSettingsChartTwoLineHistoryWidget::~MapSettingsChartTwoLineHistoryWidget()
{
}

/**
 * @return The chart overlay in this dialog (May be NULL !)
 */
ChartTwoOverlay*
MapSettingsChartTwoLineHistoryWidget::getChartOverlay()
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
MapSettingsChartTwoLineHistoryWidget::updateIfThisChartOverlayIsInDialog(const ChartTwoOverlay* chartOverlay)
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
 * Update the content of the widget.
 * 
 * @param chartOverlay
 *     Chart overlay for history display.
 */
void
MapSettingsChartTwoLineHistoryWidget::updateContent(ChartTwoOverlay* chartOverlay)
{
    CaretAssert(chartOverlay);
    m_chartOverlayWeakPointer = chartOverlay->getWeakPointerToSelf();
    updateDialogContentPrivate();
}

void
MapSettingsChartTwoLineHistoryWidget::updateDialogContentPrivate()
{
    ChartTwoOverlay* chartOverlay = getChartOverlay();
    if (chartOverlay == NULL) {
        close();
        return;
    }
    
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    
    if (lineSeriesHistory != NULL) {
        m_defaultColorComboBox->setSelectedColor(lineSeriesHistory->getDefaultColor());
        m_defaultLineWidthSpinBox->blockSignals(true);
        m_defaultLineWidthSpinBox->setValue(lineSeriesHistory->getDefaultLineWidth());
        m_defaultLineWidthSpinBox->blockSignals(false);
        
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
MapSettingsChartTwoLineHistoryWidget::getMapFile()
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
MapSettingsChartTwoLineHistoryWidget::getLineSeriesHistory()
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
MapSettingsChartTwoLineHistoryWidget::loadHistoryIntoTableWidget(ChartTwoLineSeriesHistory* lineSeriesHistory)
{
    CaretAssert(lineSeriesHistory);
    
    /*
     * NOTE: We never remove items from a table due to issues
     * with widget ownership (and destruction) and the signal mappers.
     *
     * When new rows are needed, they are added.
     * When there are two many rows, they are hidden.
     */
    
    const QSignalBlocker tableSignalBlocker(m_tableWidget);
    
    const int32_t oldRowCount = m_tableWidget->rowCount();
    const int32_t numHistory = lineSeriesHistory->getHistoryCount();
    
    /*
     * Setup table dimensions
     */
    m_tableWidget->setRowCount(std::max(numHistory, oldRowCount));
    m_tableWidget->setColumnCount(COLUMN_COUNT);
    
    /*
     * If needed, add cells
     */
    for (int32_t iRow = oldRowCount; iRow < numHistory; iRow++) {
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
            else if (j == COLUMN_MOVE) {
                QLabel dummyLabel("");
                QPixmap moveDownPixmap = createIcon(&dummyLabel, IconType::ARROW_DOWN);
                WuQImageLabel* moveDownLabel = new WuQImageLabel(moveDownPixmap,
                                                                 "Move Down");
                QObject::connect(moveDownLabel, &WuQImageLabel::clicked,
                                 this, [=] { moveDownHistoryItemSelected(iRow); });
                
                QPixmap moveUpPixmap = createIcon(&dummyLabel, IconType::ARROW_UP);
                WuQImageLabel* moveUpLabel = new WuQImageLabel(moveUpPixmap,
                                                               "Move Up");
                QObject::connect(moveUpLabel, &WuQImageLabel::clicked,
                                 this, [=] { moveUpHistoryItemSelected(iRow); });

                QWidget* moveWidget = new QWidget;
                QHBoxLayout* moveLayout = new QHBoxLayout(moveWidget);
                moveLayout->setContentsMargins(4, 0, 4, 0);
                moveLayout->setSpacing(6);
                moveLayout->addWidget(moveDownLabel);
                moveLayout->addWidget(moveUpLabel);
                moveWidget->setSizePolicy(QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
                
                m_tableWidget->setCellWidget(iRow,
                                             COLUMN_MOVE,
                                             moveWidget);
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
            else if (j == COLUMN_LINE_WIDTH) {
                WuQDoubleSpinBox* lineWidthSpinBox = new WuQDoubleSpinBox(this);
                lineWidthSpinBox->setRangePercentage(0.0, 100.0);
                QObject::connect(lineWidthSpinBox, SIGNAL(valueChanged(double)),
                                 m_lineWidthItemSignalMapper, SLOT(map()));
                m_lineWidthItemSignalMapper->setMapping(lineWidthSpinBox, iRow);
                m_tableWidget->setCellWidget(iRow,
                                             COLUMN_LINE_WIDTH,
                                             lineWidthSpinBox->getWidget());
                m_lineWidthSpinBoxes.push_back(lineWidthSpinBox);
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
    
    CaretAssert(m_lineWidthSpinBoxes.size() == m_colorComboBoxes.size());
    
    /*
     * Hide rows not needed
     */
    for (int32_t iRow = numHistory; iRow < oldRowCount; iRow++) {
        m_tableWidget->setRowHidden(iRow, true);
    }
    
    /*
     * Load cells
     */
    CaretAssert(numHistory <= m_tableWidget->rowCount());
    for (int32_t iRow = 0; iRow < numHistory; iRow++) {
        ChartTwoDataCartesian* historyData = lineSeriesHistory->getHistoryItem(iRow);
        const MapFileDataSelector* mapFileDataSelector = historyData->getMapFileDataSelector();
        
        m_tableWidget->item(iRow, COLUMN_VIEW)->setCheckState(historyData->isSelected()
                                                           ? Qt::Checked
                                                           : Qt::Unchecked);
        
        CaretAssertVectorIndex(m_colorComboBoxes, iRow);
        m_colorComboBoxes[iRow]->setSelectedColor(historyData->getColorEnum());
        
        CaretAssertVectorIndex(m_lineWidthSpinBoxes, iRow);
        m_lineWidthSpinBoxes[iRow]->setValue(historyData->getLineWidth());
        
        QTableWidgetItem* item = m_tableWidget->item(iRow, COLUMN_DESCRIPTION);
        item->setText(mapFileDataSelector->toString());

        m_tableWidget->setRowHidden(iRow, false);
    }
    
    /*
     * If table was empty, add column titles
     * and adjust column widths
     */
    if ((oldRowCount == 0)
        && (numHistory > 0)) {
        m_tableWidget->setHorizontalHeaderItem(COLUMN_VIEW,
                                               new QTableWidgetItem("View"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_REMOVE,
                                               new QTableWidgetItem("Remove"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_MOVE,
                                               new QTableWidgetItem("Move"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_COLOR,
                                               new QTableWidgetItem("Color"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_LINE_WIDTH,
                                               new QTableWidgetItem("Line Width"));
        m_tableWidget->setHorizontalHeaderItem(COLUMN_DESCRIPTION,
                                               new QTableWidgetItem("Description"));
        
        m_tableWidget->resizeColumnToContents(COLUMN_VIEW);
        m_tableWidget->resizeColumnToContents(COLUMN_REMOVE);
        m_tableWidget->resizeColumnToContents(COLUMN_MOVE);
        m_tableWidget->resizeColumnToContents(COLUMN_COLOR);
        m_tableWidget->resizeColumnToContents(COLUMN_LINE_WIDTH);
        m_tableWidget->setColumnWidth(COLUMN_DESCRIPTION,
                                      350);
        
        QSize tableSize = WuQtUtilities::estimateTableWidgetSize(m_tableWidget);
        
        WuQtUtilities::resizeWindow(this,
                                    tableSize.width() + 30,
                                    sizeHint().height());
    }
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
MapSettingsChartTwoLineHistoryWidget::tableWidgetCellChanged(int rowIndex, int columnIndex)
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
MapSettingsChartTwoLineHistoryWidget::removeHistoryItemSelected(int rowIndex)
{
    /*
     * May be possible for user to press another remove button
     * before this one is finished
     */
    const QSignalBlocker removeSignalBlocker(m_removeHistoryItemSignalMapper);
    
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    lineSeriesHistory->removeHistoryItem(rowIndex);
    updateDialogContentPrivate();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Move a history item up
 *
 * @param rowIndex
 *     Row index of item.
 */
void
MapSettingsChartTwoLineHistoryWidget::moveUpHistoryItemSelected(int rowIndex)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    lineSeriesHistory->moveUpHistoryItem(rowIndex);
    updateDialogContentPrivate();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Move a history item down
 *
 * @param rowIndex
 *     Row index of item.
 */
void
MapSettingsChartTwoLineHistoryWidget::moveDownHistoryItemSelected(int rowIndex)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    lineSeriesHistory->moveDownHistoryItem(rowIndex);
    updateDialogContentPrivate();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Color item selected
 *
 * @param rowIndex
 *     Row index of item.
 */
void
MapSettingsChartTwoLineHistoryWidget::colorItemSelected(int rowIndex)
{
    CaretAssertVectorIndex(m_colorComboBoxes, rowIndex);
    CaretAssert(m_colorComboBoxes[rowIndex]);
    
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        ChartTwoDataCartesian* data = lineSeriesHistory->getHistoryItem(rowIndex);
        CaretAssert(data);
        data->setColorEnum(m_colorComboBoxes[rowIndex]->getSelectedColor());
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * line width item selected
 *
 * @param rowIndex
 *     Row index of item.
 */
void
MapSettingsChartTwoLineHistoryWidget::lineWidthItemSelected(int rowIndex)
{
    CaretAssertVectorIndex(m_lineWidthSpinBoxes, rowIndex);
    CaretAssert(m_lineWidthSpinBoxes[rowIndex]);
    
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        ChartTwoDataCartesian* data = lineSeriesHistory->getHistoryItem(rowIndex);
        CaretAssert(data);
        data->setLineWidth(m_lineWidthSpinBoxes[rowIndex]->value());
        
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Called when the "Remove All History" button is clicked.
 */
void
MapSettingsChartTwoLineHistoryWidget::removeAllHistoryButtonClicked()
{
    if (WuQMessageBox::warningOkCancel(this,
                                       ("Remove all line series history from the file "
                                        + getMapFile()->getFileNameNoPath()))) {
        ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
        lineSeriesHistory->clearHistory();
        updateDialogContentPrivate();
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
MapSettingsChartTwoLineHistoryWidget::defaultColorSelected(const CaretColorEnum::Enum color)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        lineSeriesHistory->setDefaultColor(color);
    }
}

/**
 * Called when line width is changed
 *
 * @param defaultLineWidth
 *    New default line width.
 */
void
MapSettingsChartTwoLineHistoryWidget::defaultLineWidthChanged(double defaultLineWidth)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        lineSeriesHistory->setDefaultLineWidth(defaultLineWidth);
    }
}

/**
 * Gets called when display last count is changed.
 *
 * @param num
 *     Number of history items to display.
 */
void
MapSettingsChartTwoLineHistoryWidget::viewedMaximumSpinBoxValueChanged(int num)
{
    ChartTwoLineSeriesHistory* lineSeriesHistory = getLineSeriesHistory();
    if (lineSeriesHistory != NULL) {
        lineSeriesHistory->setDisplayCount(num);
        updateDialogContentPrivate();
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}

/**
 * Create a pixmap for the given widget
 *
 * @param editButton
 *     The edit button identifier
 * @return
 *     Pixmap for the given button
 */
QPixmap
MapSettingsChartTwoLineHistoryWidget::createIcon(const QWidget* widget,
                                                 const IconType iconType) const
{
    CaretAssert(widget);
    const qreal pixmapSize = 22.0;
    const qreal maxValue = pixmapSize / 2.0 - 1.0;
    const qreal arrowTip = maxValue * (2.0 / 3.0);
    
    uint32_t pixmapOptions(static_cast<uint32_t>(WuQtUtilities::PixMapCreationOptions::TransparentBackground));
    
    QPixmap pixmap(static_cast<int>(pixmapSize),
                   static_cast<int>(pixmapSize));
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginCenter(widget,
                                                                                            pixmap,
                                                                                            pixmapOptions);
    QPen pen(painter->pen());
    pen.setWidth(3);
    painter->setPen(pen);
    
    switch (iconType) {
        case IconType::ARROW_DOWN:
            /*
             * Down arrow
             */
            painter->drawLine(QPointF(0, maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(0, -maxValue), QPointF(arrowTip,  -maxValue + arrowTip));
            painter->drawLine(QPointF(0, -maxValue), QPointF(-arrowTip, -maxValue + arrowTip));
            break;
        case IconType::ARROW_UP:
            /*
             * Up arrow
             */
            painter->drawLine(QPointF(0, maxValue), QPointF(0, -maxValue));
            painter->drawLine(QPointF(0, maxValue), QPointF(arrowTip,  maxValue - arrowTip));
            painter->drawLine(QPointF(0, maxValue), QPointF(-arrowTip, maxValue - arrowTip));
            break;
    }
    
    return pixmap;
}

