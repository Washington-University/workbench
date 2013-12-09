
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __WU_Q_TABLE_WIDGET_DECLARE__
#include "WuQTableWidget.h"
#undef __WU_Q_TABLE_WIDGET_DECLARE__

#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QFontDatabase>
#include <QHeaderView>
#include <QIcon>
#include <QSignalMapper>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "CaretAssert.h"
#include "WuQFactory.h"
#include "WuQImageLabel.h"
#include "WuQtUtilities.h"
#include "WuQTableWidgetModel.h"

using namespace caret;


    
/**
 * \class caret::WuQTableWidget 
 * \brief Simplified QTableWidget
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    Parent widget.
 */
WuQTableWidget::WuQTableWidget(QObject* parent)
: WuQWidget(parent)
{
    m_tableModel      = NULL;
    m_numberOfRows    = 0;
    m_numberOfColumns = 0;
    /*
     * Further initialize the table
     */
    m_tableWidget = new QTableWidget();
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setSelectionBehavior(QTableWidget::SelectItems);
    m_tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    QObject::connect(m_tableWidget, SIGNAL(cellChanged(int,int)),
                     this, SLOT(filesTableWidgetCellChanged(int,int)));
    QObject::connect(m_tableWidget, SIGNAL(cellClicked(int,int)),
                     this, SLOT(filesTableWidgetCellClicked(int,int)));
    QObject::connect(m_tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)),
                     this, SLOT(horizontalHeaderSelectedForSorting(int)));
    const QString headerToolTip = ("Click on the column names (of those columns that contain text) to sort.");
    m_tableWidget->horizontalHeader()->setToolTip(WuQtUtilities::createWordWrappedToolTipText(headerToolTip));

    /*
     * Signal mapper for custom widgets inserted into table
     */
    m_widgetSignalMapper = new QSignalMapper(this);
    QObject::connect(m_widgetSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(widgetSignalMapperSelected(int)));
    
    m_tableWidget->resizeColumnsToContents();
    m_tableWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                  QSizePolicy::MinimumExpanding);
}

/**
 * Destructor.
 */
WuQTableWidget::~WuQTableWidget()
{
}

/**
 * @return The actual widget composed in this object.
 */
QWidget*
WuQTableWidget::getWidget()
{
    return m_tableWidget;
}

void
WuQTableWidget::setModel(WuQTableWidgetModel* tableModel)
{
    CaretAssert(tableModel);
    m_tableModel = tableModel;
    
    setNumberOfDisplayedRows(0);
    
    /*
     * Set up the column header titles
     */
//    for (int32_t i = 0; i < m_numberOfColumns; i++) {
//        insertHorizontalHeaderTextItem(i,
//                                       columnHeaderTitles[i]);
//    }
}

/**
 * @return Number of rows in model.
 */
int32_t
WuQTableWidget::getNumberOfModelRows() const
{
    if (m_tableModel != NULL){
        return m_tableModel->getNumberOfRows();
    }
    return 0;
}

/**
 * @return Number of rows in model.
 */
int32_t
WuQTableWidget::getNumberOfModelColumns() const
{
    if (m_tableModel != NULL){
        return m_tableModel->getNumberOfColumns();
    }
    return 0;
}


/**
 * Slot called when user clicks a column header to sort the table.
 *
 */
void
WuQTableWidget::horizontalHeaderSelectedForSorting(int)
{
    std::cout << "Need to implement sorting" << std::endl;
}

/**
 * Called when a cell's content is changed.
 * 
 * @param rowIndex
 *    Row of cell.
 * @param columnIndex
 *    Column of cell.
 */
void
WuQTableWidget::filesTableWidgetCellChanged(int rowIndex, int columnIndex)
{
    std::cout << "Need to implement cell widget changed" << std::endl;
}

/**
 * Called when a cell's content is clicked.
 *
 * @param rowIndex
 *    Row of cell.
 * @param columnIndex
 *    Column of cell.
 */
void
WuQTableWidget::filesTableWidgetCellClicked(int rowIndex, int columnIndex)
{
    std::cout << "Need to implement cell widget clicked" << std::endl;
    const WuQTableWidgetModelColumnContent* columnContent = m_tableModel->getColumnContentInfo(columnIndex);
    const WuQTableWidgetModelColumnContent::ColumnDataType dataType = columnContent->getColumnDataType();

    switch (dataType) {
        case WuQTableWidgetModelColumnContent::COLUMN_DATA_CHECK_BOX:
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_DATA_COLOR_SWATCH:
        {
            QTableWidgetItem* item = getTableWidgetItem(rowIndex,
                                                        columnIndex);
            CaretAssert(item);
            
            QColor color = item->backgroundColor();
            QColor newColor = QColorDialog::getColor(color,
                                                     m_tableWidget,
                                                     "Color",
                                                     (QColorDialog::DontUseNativeDialog
                                                      | QColorDialog::ShowAlphaChannel));
            if (newColor.isValid()) {
                if (color != newColor) {
                    const float rgba[4] = {
                        newColor.redF(),
                        newColor.greenF(),
                        newColor.blueF(),
                        newColor.alpha()
                    };
                    m_tableModel->setColorSwatch(rowIndex,
                                                 columnIndex,
                                                 rgba);
                    updateRow(rowIndex);
                }
            }
            
        }
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_DATA_DOUBLE_SPIN_BOX:
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_DATA_ICON:
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_DATA_INTEGER_SPIN_BOX:
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_DATA_TEXT:
            break;
    }
}

/**
 * Called by the widget signal mapper.
 *
 * @param twoDimIndex
 *    Two-dimensional index of item selected from which row and column
 *    are derived (row * numberOfColumns + column).
 */
void
WuQTableWidget::widgetSignalMapperSelected(int widgetMapperIndex)
{
    int32_t rowIndex = 0;
    int32_t columnIndex = 0;
    
    widgetSignalMapperIndexToRowAndColumn(widgetMapperIndex,
                                          rowIndex,
                                          columnIndex);
    
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    std::cout << "Selected widget in row/column: " << rowIndex << "   " << columnIndex << std::endl;
}

/**
 * Insert a horizontal header item into the table at the given column
 * with the given text.
 *
 * @param columnIndex
 *    The column for the header text item.
 * @param text
 *    Text that is displayed
 */
void
WuQTableWidget::setHorizontalHeaderTextItem(const int32_t columnIndex,
                                                 const QString& text)
{
    QTableWidgetItem* item = createTextItem(text);
    
    /*
     * Increte the font size to the next larger size
     * and use bold text.
     */
    QList<int> sizesList = QFontDatabase::standardSizes();
    std::sort(sizesList.begin(), sizesList.end());
    QFont font = item->font();
    QListIterator<int> sizeIter(sizesList);
    while (sizeIter.hasNext()) {
        const int nextSize = sizeIter.next();
        if (nextSize > font.pointSize()) {
            font.setPointSize(nextSize);
            break;
        }
    }
    font.setBold(true);
    item->setFont(font);
    
    m_tableWidget->setHorizontalHeaderItem(columnIndex,
                                         item);
}

/**
 * Encode row and column indices into a widget signal mapper index.
 *
 * @param rowIndex
 *    Index of row.
 * @param columnIndex
 *    Index of column.
 * @return
 *    Index for use with the widget signal mapper.
 */
int32_t
WuQTableWidget::widgetSignalMapperIndexFromRowAndColumn(const int32_t rowIndex,
                                          const int32_t columnIndex) const
{
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    const int32_t widgetSignalMapperIndex = ((rowIndex * m_numberOfColumns)
                                         + columnIndex);
    return widgetSignalMapperIndex;
}

/**
 * Decode row and column indices from a widget signal mapper index.
 *
 * @param widgetSignalMapperIndex
 *    Index from widget signal mapper.
 * @param rowIndexOut
 *    Index of row.
 * @param columnIndexOut
 *    Index of column.
 * @return
 *    Index for use with the widget signal mapper.
 */
void
WuQTableWidget::widgetSignalMapperIndexToRowAndColumn(const int32_t widgetSignalMapperIndex,
                                                      int32_t& rowIndexOut,
                                     int32_t& columnIndexOut) const
{
    if (m_numberOfColumns <= 0) {
        rowIndexOut    = 0;
        columnIndexOut = 0;
    }
    
    rowIndexOut = widgetSignalMapperIndex / m_numberOfColumns;
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndexOut);
    columnIndexOut = (widgetSignalMapperIndex
                      - (rowIndexOut * m_numberOfColumns));
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndexOut);
}



/**
 * Set the number of rows in the table.  May be called multiple time and rows
 * will be added/hidden as needed.
 *
 * @param numberOfRowsRequested
 *     New number of displayed rows.
 */
void
WuQTableWidget::setNumberOfDisplayedRows(const int32_t /*numberOfRowsRequested*/)
{
    if (m_tableModel == NULL) {
        m_tableWidget->setEnabled(false);
        m_numberOfRows    = 0;
        m_numberOfColumns = 0;
        return;
    }
    m_tableWidget->blockSignals(true);
    
    m_tableWidget->setEnabled(true);
    
    const int32_t currentNumberOfRows = m_tableWidget->rowCount();
    const int32_t currentNumberOfColumns = m_tableWidget->columnCount();
    
    const int32_t numberOfColumns = getNumberOfModelColumns();
    const int32_t modelNumberOfRows = m_tableModel->getNumberOfRows();
    
    m_numberOfRows = modelNumberOfRows;
    m_numberOfColumns = numberOfColumns;
    
    if (modelNumberOfRows > currentNumberOfRows) {
        m_tableWidget->setRowCount(modelNumberOfRows);
        m_tableWidget->setColumnCount(numberOfColumns);
    }

    if (currentNumberOfColumns == 0) {
        QStringList columnLabels;
        for (int32_t jCol = 0; jCol < numberOfColumns; jCol++) {
            const WuQTableWidgetModelColumnContent* columnContent = m_tableModel->getColumnContentInfo(jCol);
            columnLabels.append(columnContent->getColumnTitle());
        }
        m_tableWidget->setHorizontalHeaderLabels(columnLabels);
    }
    
    for (int32_t jCol = 0; jCol < numberOfColumns; jCol++) {
        const WuQTableWidgetModelColumnContent* columnContent = m_tableModel->getColumnContentInfo(jCol);
        const WuQTableWidgetModelColumnContent::ColumnDataType dataType = columnContent->getColumnDataType();
        
        const WuQTableWidgetModelColumnCheckBox* columnCheckBox = dynamic_cast<const WuQTableWidgetModelColumnCheckBox*>(columnContent);
        const WuQTableWidgetModelColumnColorSwatch* columnColorSwatch = dynamic_cast<const WuQTableWidgetModelColumnColorSwatch*>(columnContent);
        const WuQTableWidgetModelColumnDoubleSpinBox* columnDoubleSpinBox = dynamic_cast<const WuQTableWidgetModelColumnDoubleSpinBox*>(columnContent);
        const WuQTableWidgetModelColumnIcon* columnIcon = dynamic_cast<const WuQTableWidgetModelColumnIcon*>(columnContent);
        const WuQTableWidgetModelColumnIntegerSpinBox* columnIntSpinBox = dynamic_cast<const WuQTableWidgetModelColumnIntegerSpinBox*>(columnContent);
        const WuQTableWidgetModelColumnText* columnText = dynamic_cast<const WuQTableWidgetModelColumnText*>(columnContent);
        
        for (int32_t iRow = 0; iRow < modelNumberOfRows; iRow++) {
            if (iRow >= currentNumberOfRows) {
                switch (dataType) {
                    case WuQTableWidgetModelColumnContent::COLUMN_DATA_CHECK_BOX:
                        insertCheckableItem(columnCheckBox,
                                            iRow,
                                            jCol);
                        break;
                    case WuQTableWidgetModelColumnContent::COLUMN_DATA_COLOR_SWATCH:
                        insertColorSwatch(columnColorSwatch,
                                          iRow,
                                          jCol);
                        break;
                    case WuQTableWidgetModelColumnContent::COLUMN_DATA_DOUBLE_SPIN_BOX:
                        insertDoubleSpinBoxItem(columnDoubleSpinBox,
                                                iRow,
                                                jCol);
                        break;
                    case WuQTableWidgetModelColumnContent::COLUMN_DATA_ICON:
                        insertIconItem(columnIcon,
                                             iRow,
                                             jCol);
                        break;
                    case WuQTableWidgetModelColumnContent::COLUMN_DATA_INTEGER_SPIN_BOX:
                        insertIntegerSpinBoxItem(columnIntSpinBox,
                                                 iRow,
                                                jCol);
                        break;
                    case WuQTableWidgetModelColumnContent::COLUMN_DATA_TEXT:
                        insertTextItem(columnText,
                                       iRow,
                                       jCol);
                        break;
                }
            }
            else {
                m_tableWidget->setRowHidden(iRow, false);
            }
            
            
//            switch (dataType) {
//                case WuQTableWidgetModelColumnContent::COLUMN_DATA_CHECK_BOX:
//                {
//                    QTableWidgetItem* item = getTableWidgetItem(iRow, jCol);
//                    CaretAssert(item);
//                    const Qt::CheckState cs = WuQtUtilities::boolToCheckState(m_tableModel->isChecked(iRow, jCol));
//                    item->setCheckState(cs);
//                }
//                    break;
//                case WuQTableWidgetModelColumnContent::COLUMN_DATA_COLOR_SWATCH:
//                {
//                    QWidget* widget = m_tableWidget->cellWidget(iRow,
//                                                                jCol);
//                    CaretAssert(widget);
//                }
//                    break;
//                case WuQTableWidgetModelColumnContent::COLUMN_DATA_DOUBLE_SPIN_BOX:
//                {
//                    QWidget* widget  = m_tableWidget->cellWidget(iRow,
//                                                                 jCol);
//                    CaretAssert(widget);
//                    QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget);
//                    CaretAssert(spinBox);
//                    spinBox->setValue(m_tableModel->getDouble(iRow,
//                                                              jCol));
//                }
//                    break;
//                case WuQTableWidgetModelColumnContent::COLUMN_DATA_ICON:
//                {
//                    
//                }
//                    break;
//                case WuQTableWidgetModelColumnContent::COLUMN_DATA_INTEGER_SPIN_BOX:
//                {
//                    QWidget* widget  = m_tableWidget->cellWidget(iRow,
//                                                                 jCol);
//                    CaretAssert(widget);
//                    QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget);
//                    CaretAssert(spinBox);
//                    spinBox->setValue(m_tableModel->getInteger(iRow,
//                                                              jCol));
//                }
//                    break;
//                case WuQTableWidgetModelColumnContent::COLUMN_DATA_TEXT:
//                {
//                    QTableWidgetItem* item = getTableWidgetItem(iRow, jCol);
//                    CaretAssert(item);
//                    item->setText(m_tableModel->getText(iRow,
//                                                          jCol));
//                }
//                    break;
//            }
            
        }
    }
    
    for (int32_t iRow = 0;
         iRow < modelNumberOfRows;
         iRow++) {
        updateRow(iRow);
    }
    
    for (int32_t iRow = modelNumberOfRows;
         iRow < currentNumberOfRows;
         iRow++) {
        m_tableWidget->setRowHidden(iRow, true);
    }
    
    m_tableWidget->blockSignals(false);
}

/**
 * Update the row at the given index.
 *
 * @param rowIndex
 *    Index of the row.
 */
void
WuQTableWidget::updateRow(const int32_t rowIndex)
{
    for (int32_t jCol = 0; jCol < m_numberOfColumns; jCol++) {
        const WuQTableWidgetModelColumnContent* columnContent = m_tableModel->getColumnContentInfo(jCol);
        const WuQTableWidgetModelColumnContent::ColumnDataType dataType = columnContent->getColumnDataType();

        switch (dataType) {
            case WuQTableWidgetModelColumnContent::COLUMN_DATA_CHECK_BOX:
            {
                QTableWidgetItem* item = getTableWidgetItem(rowIndex, jCol);
                CaretAssert(item);
                const Qt::CheckState cs = WuQtUtilities::boolToCheckState(m_tableModel->isChecked(rowIndex, jCol));
                item->setCheckState(cs);
            }
                break;
            case WuQTableWidgetModelColumnContent::COLUMN_DATA_COLOR_SWATCH:
            {
                float rgba[4];
                m_tableModel->getColorSwatch(rowIndex,
                                             jCol,
                                             rgba);
                QColor color;
                color.setRgbF(rgba[0], rgba[1], rgba[2]);
                
                QTableWidgetItem* item = getTableWidgetItem(rowIndex, jCol);
                item->setBackgroundColor(color);
            }
                break;
            case WuQTableWidgetModelColumnContent::COLUMN_DATA_DOUBLE_SPIN_BOX:
            {
                QWidget* widget  = m_tableWidget->cellWidget(rowIndex,
                                                             jCol);
                CaretAssert(widget);
                QDoubleSpinBox* spinBox = qobject_cast<QDoubleSpinBox*>(widget);
                CaretAssert(spinBox);
                spinBox->setValue(m_tableModel->getDouble(rowIndex,
                                                          jCol));
            }
                break;
            case WuQTableWidgetModelColumnContent::COLUMN_DATA_ICON:
            {
                QWidget* widget  = m_tableWidget->cellWidget(rowIndex,
                                                             jCol);
                CaretAssert(widget);
                WuQImageLabel* imageLabel = qobject_cast<WuQImageLabel*>(widget);
                CaretAssert(imageLabel);
                
                const QIcon* icon = m_tableModel->getIcon(rowIndex,
                                                    jCol);
                imageLabel->updateIconText(icon,
                                           "");
            }
                break;
            case WuQTableWidgetModelColumnContent::COLUMN_DATA_INTEGER_SPIN_BOX:
            {
                QWidget* widget  = m_tableWidget->cellWidget(rowIndex,
                                                             jCol);
                CaretAssert(widget);
                QSpinBox* spinBox = qobject_cast<QSpinBox*>(widget);
                CaretAssert(spinBox);
                spinBox->setValue(m_tableModel->getInteger(rowIndex,
                                                           jCol));
            }
                break;
            case WuQTableWidgetModelColumnContent::COLUMN_DATA_TEXT:
            {
                QTableWidgetItem* item = getTableWidgetItem(rowIndex,
                                                            jCol);
                CaretAssert(item);
                item->setText(m_tableModel->getText(rowIndex,
                                                    jCol));
            }
                break;
        }
    }
}


/**
 * Insert a checkable item into the table at the given row and column.
 *
 * @param columnInfo
 *    Checkbox column info.
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 */
void
WuQTableWidget::insertCheckableItem(const WuQTableWidgetModelColumnCheckBox* columnInfo,
                                    const int32_t rowIndex,
                                    const int32_t columnIndex)
{
    CaretAssert(columnInfo);
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    QTableWidgetItem* item = createTextItem("");
    setItemTextAlignment(columnInfo,
                         item);

    Qt::ItemFlags flags = item->flags();
    flags |= (Qt::ItemIsUserCheckable);
    item->setFlags(flags);
    item->setCheckState(Qt::Unchecked);

    setTableWidgetItem(rowIndex,
                       columnIndex,
                       item);
}

/**
 * Insert a text item into the table at the given row and column.
 *
 * @param columnInfo
 *    Text column info.
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 */
void
WuQTableWidget::insertTextItem(const WuQTableWidgetModelColumnText* columnInfo,
                               const int32_t rowIndex,
                                 const int32_t columnIndex)
{
    CaretAssert(columnInfo);
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    QTableWidgetItem* item = createTextItem("");
    switch (columnInfo->getTextEditable()) {
        case WuQTableWidgetModelColumnText::TEXT_EDITABLE_NO:
            break;
        case WuQTableWidgetModelColumnText::TEXT_EDITABLE_YES:
            item->setFlags(item->flags()
                           | Qt::ItemIsEditable);
            break;
    }
    
    setItemTextAlignment(columnInfo,
                         item);
    
    setTableWidgetItem(rowIndex,
                       columnIndex,
                       item);
}

/**
 * Set the alignment of the given item.
 *
 * @param columnInfo
 *    Column info.
 * @param item
 *    Item whose text alignment is set.
 */
void
WuQTableWidget::setItemTextAlignment(const WuQTableWidgetModelColumnContent* columnInfo,
                                     QTableWidgetItem* item)
{
    int qtAlignment = 0;
    switch (columnInfo->getColumnAlignment()) {
        case WuQTableWidgetModelColumnContent::COLUMN_ALIGN_CENTER:
            qtAlignment = Qt::AlignCenter;
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_ALIGN_LEFT:
            qtAlignment = (Qt::AlignLeft
                           | Qt::AlignVCenter);
            break;
        case WuQTableWidgetModelColumnContent::COLUMN_ALIGN_RIGHT:
            qtAlignment = (Qt::AlignRight
                           | Qt::AlignVCenter);
            break;
    }
    item->setTextAlignment(qtAlignment);
}


/**
 * Create a table widget text item.
 *
 * @param text
 *    The default text.
 * @return
 *    The text item.
 */
QTableWidgetItem*
WuQTableWidget::createTextItem(const AString& text)
{
    QTableWidgetItem* item = new QTableWidgetItem();
    Qt::ItemFlags flags(Qt::ItemIsEnabled);
    item->setFlags(flags);
    item->setText(text);
    
    return item;
}

/**
 * Insert an icon into the table at the given row and column.
 *
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 */
void
WuQTableWidget::insertIconItem(const WuQTableWidgetModelColumnIcon* columnInfo,
                                     const int32_t rowIndex,
                                           const int32_t columnIndex)
{
    CaretAssert(columnInfo);
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    WuQImageLabel* imageLabel = new WuQImageLabel(QIcon(),
                                                  "");
    connectToWidgetSignalMapper(rowIndex,
                                columnIndex,
                                imageLabel,
                                SIGNAL(clicked()));
    
    m_tableWidget->setCellWidget(rowIndex,
                  columnIndex,
                  imageLabel);
}

/**
 * Insert a color swatch into the table at the given row and column.
 *
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 */
void
WuQTableWidget::insertColorSwatch(const WuQTableWidgetModelColumnColorSwatch* columnInfo,
                                  const int32_t rowIndex,
                       const int32_t columnIndex)
{
    CaretAssert(columnInfo);
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    QTableWidgetItem* item = createTextItem("     ");
    item->setTextAlignment(columnInfo->getColumnAlignment());
    
    setTableWidgetItem(rowIndex,
                       columnIndex,
                       item);

//    CaretAssert(columnInfo);
//    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
//    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
//    
//    WuQImageLabel* imageLabel = new WuQImageLabel(QIcon(),
//                                                  "");
//    connectToWidgetSignalMapper(rowIndex,
//                                columnIndex,
//                                imageLabel,
//                                SIGNAL(clicked()));
//    
//    m_tableWidget->setCellWidget(rowIndex,
//                                 columnIndex,
//                                 imageLabel);
}

/**
 * Insert an integer spin box into the table at the given row and column.
 *
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 */
void
WuQTableWidget::insertIntegerSpinBoxItem(const WuQTableWidgetModelColumnIntegerSpinBox* columnInfo,
                                         const int32_t rowIndex,
                       const int32_t columnIndex)
{
    CaretAssert(columnInfo);
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    QSpinBox* spinBox = WuQFactory::newSpinBoxWithMinMaxStep(columnInfo->getMinimumValue(),
                                                                     columnInfo->getMaximumValue(),
                                                                     columnInfo->getStepSize());
    connectToWidgetSignalMapper(rowIndex,
                                columnIndex,
                                spinBox,
                                SIGNAL(valueChanged(int)));

    m_tableWidget->setCellWidget(rowIndex,
                  columnIndex,
                  spinBox);
}

/**
 * Connect a signal from an object to the widget signal mapper for
 * the given row and column indices.
 *
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 * @param object
 *    The object that emits a signal
 * @param objectSignal
 *    Signal emitted by the object.
 */
void
WuQTableWidget::connectToWidgetSignalMapper(const int32_t rowIndex,
                                            const int32_t columnIndex,
                                            QObject* object,
                                 const char* objectSignal)
{
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    QObject::connect(object, objectSignal,
                     m_widgetSignalMapper, SLOT(map()));
    const int mapperIndex = widgetSignalMapperIndexFromRowAndColumn(rowIndex,
                                                                    columnIndex);
    m_widgetSignalMapper->setMapping(object,
                                     mapperIndex);
    
}

/**
 * Insert a double spin box into the table at the given row and column.
 *
 * @param rowIndex
 *    The row for the text item.
 * @param columnIndex
 *    The column for the text item.
 */
void
WuQTableWidget::insertDoubleSpinBoxItem(const WuQTableWidgetModelColumnDoubleSpinBox* columnInfo,
                                        const int32_t rowIndex,
                             const int32_t columnIndex)
{
    CaretAssert(columnInfo);
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    QDoubleSpinBox* spinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimals(columnInfo->getMinimumValue(),
                                                                                 columnInfo->getMaximumValue(),
                                                                                 columnInfo->getStepSize(),
                                                                                 columnInfo->getDigitsRightOfDecimalPoint());
    connectToWidgetSignalMapper(rowIndex,
                                columnIndex,
                                spinBox,
                                SIGNAL(valueChanged(double)));

    m_tableWidget->setCellWidget(rowIndex,
                  columnIndex,
                  spinBox);
}


/**
 * Get the table widget item at the given row and column.  If compiled
 * debug the assertions will fail if the row or column is invalid.
 *
 * @param rowIndex
 *    The row of the desired cell.
 * @param columnIndex
 *    The column of the desired cell.
 * @return
 *    item at row and column.
 */
QTableWidgetItem*
WuQTableWidget::getTableWidgetItem(const int32_t rowIndex,
                                     const int32_t columnIndex)
{
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    
    return m_tableWidget->item(rowIndex,
                columnIndex);
    
}

/**
 * Set the table widget item at the given row and column.  If compiled
 * debug the assertions will fail if the row or column is invalid.
 *
 * @param rowIndex
 *    The row of the desired cell.
 * @param columnIndex
 *    The column of the desired cell.
 * @param item
 *    The item to add.
 */
void
WuQTableWidget::setTableWidgetItem(const int32_t rowIndex,
                        const int32_t columnIndex,
                        QTableWidgetItem* item)
{
    CaretAssertArrayIndex(this, m_numberOfRows, rowIndex);
    CaretAssertArrayIndex(this, m_numberOfColumns, columnIndex);
    CaretAssert(item);
    
    blockSignals(true);
    m_tableWidget->setItem(rowIndex,
                         columnIndex,
                         item);
    blockSignals(false);
}

QSize
WuQTableWidget::estimateTableWidgetSize()
{
    QSize tableSize(0, 0);
    
    /*
     * Table widget has a default size of 640 x 480.
     * So estimate the size of the dialog with the table fully
     * expanded.
     */
    const int numberOfRows = m_tableWidget->rowCount();
    const int cellGap = (m_tableWidget->showGrid()
                         ? 3
                         : 0);
    if ((numberOfRows > 0)
        && (m_numberOfColumns > 0)) {
        int tableWidth = 10; // start out with a little extra space
        int tableHeight = 0;
        
        if (m_tableWidget->horizontalHeader()->isHidden() == false) {
            QHeaderView* columnHeader = m_tableWidget->horizontalHeader();
            const int columnHeaderHeight = columnHeader->sizeHint().height();
            tableHeight += columnHeaderHeight;
        }
        
        if (m_tableWidget->verticalHeader()->isHidden() == false) {
            QHeaderView* rowHeader = m_tableWidget->verticalHeader();
            const int rowHeaderHeight = rowHeader->sizeHint().width();
            tableHeight += rowHeaderHeight;
        }
        
        std::vector<int> columnWidths(m_numberOfColumns, 0);
        std::vector<int> rowHeights(numberOfRows, 0);
        
        for (int iCol = 0; iCol < m_numberOfColumns; iCol++) {
            columnWidths[iCol] = m_tableWidget->columnWidth(iCol) + cellGap;
        }
        
        for (int jRow = 0; jRow < numberOfRows; jRow++) {
            rowHeights[jRow]= (m_tableWidget->rowHeight(jRow) + cellGap);
        }
        
        for (int iCol = 0; iCol < m_numberOfColumns; iCol++) {
            for (int jRow = 0; jRow < numberOfRows; jRow++) {
                QWidget* widget = m_tableWidget->cellWidget(jRow, iCol);
                if (widget != NULL) {
                    const QSize widgetSizeHint = widget->sizeHint();
                    columnWidths[iCol] = std::max(columnWidths[iCol],
                                                  widgetSizeHint.width());
                    rowHeights[jRow] = std::max(rowHeights[jRow],
                                                widgetSizeHint.height());
                }
                
                QTableWidgetItem* twi = m_tableWidget->item(jRow, iCol);
                if (twi != NULL) {
                    int itemWidth = 0;
                    int itemHeight = 0;
                    if (twi->flags() && Qt::ItemIsUserCheckable) {
                        itemWidth += 12;
                    }
                    
                    QFont font = twi->font();
                    const QString text = twi->text();
                    if (text.isEmpty() == false) {
                        QFont font = twi->font();
                        QFontMetrics fontMetrics(font);
                        const int textWidth =  fontMetrics.width(text);
                        const int textHeight = fontMetrics.height();
                        
                        itemWidth += textWidth;
                        itemHeight = std::max(itemHeight,
                                              textHeight);
                    }
                    
                    columnWidths[iCol] = std::max(columnWidths[iCol],
                                                  itemWidth);
                    rowHeights[jRow] = std::max(rowHeights[jRow],
                                                itemHeight);
                }
            }
        }
        
        for (int iCol = 0; iCol < m_numberOfColumns; iCol++) {
            tableWidth += columnWidths[iCol];
        }
        
        for (int jRow = 0; jRow < numberOfRows; jRow++) {
            tableHeight += (rowHeights[jRow] - 2);
        }
        
        tableSize.setWidth(tableWidth);
        tableSize.setHeight(tableHeight);
    }
    
    return tableSize;
}

