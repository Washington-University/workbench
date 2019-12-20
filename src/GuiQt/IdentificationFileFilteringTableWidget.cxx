
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_DECLARE__
#include "IdentificationFileFilteringTableWidget.h"
#undef __IDENTIFICATION_FILE_FILTERING_TABLE_WIDGET_DECLARE__

#include <QComboBox>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "FileIdentificationAttributes.h"
#include "FilePathNamePrefixCompactor.h"
#include "SessionManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::IdentificationFileFilteringTableWidget
 * \brief Widget containing data files for identificaiton selection
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
IdentificationFileFilteringTableWidget::IdentificationFileFilteringTableWidget()
: QTableWidget()
{
    setAlternatingRowColors(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    QObject::connect(this, &QTableWidget::cellClicked,
                     this, &IdentificationFileFilteringTableWidget::tableCellClicked);
}

/**
 * Destructor.
 */
IdentificationFileFilteringTableWidget::~IdentificationFileFilteringTableWidget()
{
}

/**
 * Update number of rows in table
 * @param numberOfFiles
 *  Number of files for table
 */
void
IdentificationFileFilteringTableWidget::updateTableRows(const int32_t numberOfFiles)
{
    const int32_t numExistingRows = rowCount();
    
    if (numberOfFiles > numExistingRows) {
        setRowCount(numberOfFiles);
        setColumnCount(COLUMN_COUNT);
    }
    
    for (int32_t iRow = numExistingRows; iRow < numberOfFiles; iRow++) {
        m_mapFiles.push_back(NULL);
        
        for (int32_t iCol = 0; iCol < COLUMN_COUNT; iCol++) {
            QWidget* widget(NULL);
            QTableWidgetItem* tableItem(NULL);
            bool checkedItemFlag(false);
            const COLUMNS column = static_cast<COLUMNS>(iCol);
            switch (column) {
                case COLUMN_COUNT:
                    CaretAssert(0);
                    break;
                case COLUMN_DISPLAYED_LABEL:
                    tableItem = new QTableWidgetItem();
                    break;
                case COLUMN_ENABLED_CHECKBOX:
                    tableItem = new QTableWidgetItem();
                    checkedItemFlag = true;
                    break;
                case COLUMN_FILE_NAME_LABEL:
                    tableItem = new QTableWidgetItem();
                    break;
                case COLUMN_MAP_MODE_COMBO_BOX:
                {
                    EnumComboBoxTemplate* mapModeComboBox = new EnumComboBoxTemplate(this);
                    m_mapModeComboBoxes.push_back(mapModeComboBox);

                    mapModeComboBox->setup<FileIdentificationMapSelectionEnum, FileIdentificationMapSelectionEnum::Enum>();
                    QObject::connect(mapModeComboBox, &EnumComboBoxTemplate::itemActivated,
                                     [=] { mapModeComboBoxChanged(iRow); });
                    
                    widget = mapModeComboBox->getComboBox();
                }
                    break;
                case COLUMN_MAP_NAME_COMBO_BOX:
                {
                    QComboBox* comboBox = new QComboBox();
                    m_mapNameComboBoxes.push_back(comboBox);
                    QObject::connect(comboBox, QOverload<int>::of(&QComboBox::activated),
                                     [=] { mapNameComboBoxChanged(iRow); });

                    widget = comboBox;
                    
                }
                    break;
            }
            
            if (tableItem != NULL) {
                Qt::ItemFlags flags(tableItem->flags());
                flags.setFlag(Qt::ItemIsUserCheckable,
                              checkedItemFlag);
                tableItem->setFlags(flags);
                tableItem->setData(Qt::UserRole, iRow);
                setItem(iRow, iCol, tableItem);
            }
            else if (widget != NULL) {
                setCellWidget(iRow, iCol, widget);
            }
            else {
                CaretAssert(0);
            }
        }
    }
    
    /*
     * Keep but clear extra cell items
     */
    const int32_t numRows = rowCount();
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        CaretAssertVectorIndex(m_mapFiles, iRow);
        m_mapFiles[iRow] = NULL;
        
        for (int32_t iCol = 0; iCol < COLUMN_COUNT; iCol++) {
            const COLUMNS column = static_cast<COLUMNS>(iCol);
            
            QTableWidgetItem* tableItem(item(iRow, iCol));
            QWidget* widget(cellWidget(iRow, iCol));
            switch (column) {
                case COLUMN_COUNT:
                    CaretAssert(0);
                    break;
                case COLUMN_DISPLAYED_LABEL:
                    CaretAssert(tableItem);
                    break;
                case COLUMN_ENABLED_CHECKBOX:
                    CaretAssert(tableItem);
                    break;
                case COLUMN_FILE_NAME_LABEL:
                    CaretAssert(tableItem);
                    break;
                case COLUMN_MAP_MODE_COMBO_BOX:
                    CaretAssert(widget);
                    break;
                case COLUMN_MAP_NAME_COMBO_BOX:
                    CaretAssert(widget);
                    break;
            }
            
            if (iRow < numberOfFiles) {
                if (tableItem != NULL) {
                    Qt::ItemFlags flags = tableItem->flags();
                    flags.setFlag(Qt::ItemIsEnabled,
                                  true);
                    tableItem->setFlags(flags);
                }
                
                if (widget != NULL) {
                    widget->setEnabled(true);
                    widget->setVisible(true);
                }
            }
            else {
                if (tableItem != NULL) {
                    Qt::ItemFlags flags = tableItem->flags();
                    flags.setFlag(Qt::ItemIsEnabled,
                                  false);
                    tableItem->setFlags(flags);
                    
                    tableItem->setText("");
                    
                    if (flags.testFlag(Qt::ItemIsUserCheckable)) {
                        tableItem->setCheckState(Qt::Unchecked);
                    }
                }
                
                if (widget != NULL) {
                    CaretAssert(tableItem == NULL);
                    widget->setEnabled(false);
                    widget->setVisible(false);
                }
            }
        }
    }
}

/**
 * @return Name for column index
 * @param columnIndex
 * Index of column
 */
AString
IdentificationFileFilteringTableWidget::getColumnName(const int32_t columnIndex) const
{
    AString name;
    
    const COLUMNS column = static_cast<COLUMNS>(columnIndex);
    switch (column) {
        case COLUMN_COUNT:
            name = "PROGRAM ERROR";
            CaretAssert(0);
            break;
        case COLUMN_DISPLAYED_LABEL:
            name = "Displayed";
            break;
        case COLUMN_ENABLED_CHECKBOX:
            name = "Show";
            break;
        case COLUMN_FILE_NAME_LABEL:
            name = "Filename";
            break;
        case COLUMN_MAP_MODE_COMBO_BOX:
            name = "Map Mode";
            break;
        case COLUMN_MAP_NAME_COMBO_BOX:
            name = "Map Name";
            break;
    }
    
    return name;
}


/**
 * Update the content with data files
 */
void
IdentificationFileFilteringTableWidget::updateContent()
{
    EventGetDisplayedDataFiles displayedFilesEvent(EventGetDisplayedDataFiles::Mode::FILES_IN_VIEWED_TABS);
    EventManager::get()->sendEvent(displayedFilesEvent.getPointer());

    EventCaretMappableDataFilesGet mapFilesEvent;
    EventManager::get()->sendEvent(mapFilesEvent.getPointer());
    
    std::vector<CaretMappableDataFile*> allMapFiles;
    mapFilesEvent.getAllFilesSortedByName(allMapFiles);
    
    const int32_t numFiles = static_cast<int32_t>(allMapFiles.size());
    
    updateTableRows(numFiles);
    
    CaretAssert(rowCount() >= numFiles);
    
    /*
     * There may be files with identical names but different paths.
     * This function will find files with identical names and append
     * the minimum path to disambiguate these files
     */
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(allMapFiles,
                                                                            displayNames);
    
    for (int32_t iRow = 0; iRow < numFiles; iRow++) {
        CaretAssertVectorIndex(allMapFiles, iRow);
        CaretMappableDataFile* cmdf = allMapFiles[iRow];
        CaretAssert(cmdf);
        
        CaretAssertVectorIndex(m_mapFiles, iRow);
        m_mapFiles[iRow] = cmdf;
        
        FileIdentificationAttributes* idAtts = cmdf->getFileIdentificationAttributes();
        CaretAssert(idAtts);
        
        for (int32_t iCol = 0; iCol < COLUMN_COUNT; iCol++) {
            const COLUMNS column = static_cast<COLUMNS>(iCol);
            
            QTableWidgetItem* tableItem(item(iRow, iCol));
            QWidget* widget(cellWidget(iRow, iCol));
            switch (column) {
                case COLUMN_COUNT:
                    CaretAssert(0);
                    break;
                case COLUMN_DISPLAYED_LABEL:
                    CaretAssert(tableItem);
                    if (displayedFilesEvent.isDataFileDisplayed(cmdf)) {
                        tableItem->setText("Yes");
                    }
                    else {
                        tableItem->setText("   ");
                    }
                    break;
                case COLUMN_ENABLED_CHECKBOX:
                    CaretAssert(tableItem);
                    if (idAtts->isEnabled()) {
                        tableItem->setCheckState(Qt::Checked);
                    }
                    else {
                        tableItem->setCheckState(Qt::Unchecked);
                    }
                    break;
                case COLUMN_FILE_NAME_LABEL:
                    CaretAssert(tableItem);
                    CaretAssertVectorIndex(displayNames, iRow);
                    tableItem->setText(displayNames[iRow]);
                    break;
                case COLUMN_MAP_MODE_COMBO_BOX:
                {
                    CaretAssert(widget);
                    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
                    CaretAssert(comboBox);
                    const FileIdentificationMapSelectionEnum::Enum mapMode = idAtts->getMapSelectionMode();
                    const int32_t integerValue = FileIdentificationMapSelectionEnum::toIntegerCode(mapMode);
                    comboBox->setCurrentIndex(integerValue);
                }
                    break;
                case COLUMN_MAP_NAME_COMBO_BOX:
                {
                    CaretAssert(widget);
                    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
                    CaretAssert(comboBox);
                    comboBox->clear();
                    for (int32_t jMap = 0; jMap < cmdf->getNumberOfMaps(); jMap++) {
                        comboBox->addItem(cmdf->getMapName(jMap));
                    }
                    comboBox->setCurrentIndex(idAtts->getMapIndex());
                }
                    break;
            }
        }
    }
    
    /*
     * First time files inserted?
     */
    if (m_lastNumberOfFiles == 0) {
        if (numFiles > 0) {
            QStringList columnNames;
            for (int32_t i = 0; i < COLUMN_COUNT; i++) {
                columnNames << getColumnName(i);
            }
            setHorizontalHeaderLabels(columnNames);
            
            resizeColumnsToContents();
            horizontalHeader()->setSectionResizeMode(COLUMN_DISPLAYED_LABEL, QHeaderView::ResizeToContents);
            horizontalHeader()->setSectionResizeMode(COLUMN_ENABLED_CHECKBOX, QHeaderView::ResizeToContents);
            horizontalHeader()->setSectionResizeMode(COLUMN_MAP_MODE_COMBO_BOX, QHeaderView::ResizeToContents);
            horizontalHeader()->setSectionResizeMode(COLUMN_MAP_NAME_COMBO_BOX, QHeaderView::ResizeToContents);
        }
    }
    
    m_lastNumberOfFiles = numFiles;
}

/**
 * Called when a table item is clicked
 * @param row
 * Row clicked
 * @param column
 * Column clicked
 */
void
IdentificationFileFilteringTableWidget::tableCellClicked(int row, int column)
{
    QTableWidgetItem* cellItem = item(row, column);
    if (cellItem != NULL) {
        if (column == COLUMN_ENABLED_CHECKBOX) {
            bool checkedFlag(false);
            switch (cellItem->checkState()) {
                case Qt::Checked:
                    checkedFlag = true;
                    break;
                case Qt::PartiallyChecked:
                    CaretAssert(0);
                    break;
                case Qt::Unchecked:
                    checkedFlag = false;
                    break;
            }
            
            FileIdentificationAttributes* atts = m_mapFiles[row]->getFileIdentificationAttributes();
            CaretAssert(atts);
            atts->setEnabled(checkedFlag);
        }
    }
}

/**
 * Called when a map mode combo box is changed
 * @param row
 * Row containing the map mode combo box
 */
void
IdentificationFileFilteringTableWidget::mapModeComboBoxChanged(int row)
{
    CaretAssertVectorIndex(m_mapModeComboBoxes, row);
    const FileIdentificationMapSelectionEnum::Enum value =
       m_mapModeComboBoxes[row]->getSelectedItem<FileIdentificationMapSelectionEnum, FileIdentificationMapSelectionEnum::Enum>();

    CaretAssertVectorIndex(m_mapFiles, row);
    FileIdentificationAttributes* atts = m_mapFiles[row]->getFileIdentificationAttributes();
    CaretAssert(atts);
    atts->setMapSelectionMode(value);
    
    std::cout << "Mode row: " << row << std::endl;
}


/**
 * Called when a map name combo box is changed
 * @param row
 * Row containing the map name combo box
 */
void
IdentificationFileFilteringTableWidget::mapNameComboBoxChanged(int row)
{
    CaretAssertVectorIndex(m_mapNameComboBoxes, row);
    const int32_t mapIndex = m_mapNameComboBoxes[row]->currentIndex();
    
    CaretAssertVectorIndex(m_mapFiles, row);
    FileIdentificationAttributes* atts = m_mapFiles[row]->getFileIdentificationAttributes();
    CaretAssert(atts);
    atts->setMapIndex(mapIndex);
    
    std::cout << "Map name row: " << row << std::endl;
}

