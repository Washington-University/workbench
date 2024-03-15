
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
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventCaretDataFilesGet.h"
#include "EventGetDisplayedDataFiles.h"
#include "EventManager.h"
#include "FileIdentificationAttributes.h"
#include "FilePathNamePrefixCompactor.h"
#include "HistologySlicesFile.h"
#include "MediaFile.h"
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
    
    if (numberOfFiles != numExistingRows) {
        /*
         * Resize table to match number of files
         */
        setRowCount(numberOfFiles);
        setColumnCount(COLUMN_COUNT);
        
        if (numExistingRows > numberOfFiles) {
            /*
             * Remove extra items
             */
            m_dataFiles.resize(numberOfFiles);
            m_displayModeComboBoxes.resize(numberOfFiles);
            m_mapModeComboBoxes.resize(numberOfFiles);
            m_mapNameComboBoxes.resize(numberOfFiles);
        }
    }
    
    /*
     * If needed, add additional rows
     */
    for (int32_t iRow = numExistingRows; iRow < numberOfFiles; iRow++) {
        m_dataFiles.push_back(NULL);
        
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
                case COLUMN_DISPLAY_MODE_COMBO_BOX:
                {
                    EnumComboBoxTemplate* displayModeComboBox = new EnumComboBoxTemplate(this);
                    m_displayModeComboBoxes.push_back(displayModeComboBox);
                    
                    displayModeComboBox->setup<FileIdentificationDisplayModeEnum, FileIdentificationDisplayModeEnum::Enum>();
                    QObject::connect(displayModeComboBox, &EnumComboBoxTemplate::itemActivated,
                                     [=] { displayModeComboBoxChanged(iRow); });
                    widget = displayModeComboBox->getComboBox();
                    widget->setToolTip(FileIdentificationDisplayModeEnum::toToolTipForAllEnums());
                }
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
        case COLUMN_DISPLAY_MODE_COMBO_BOX:
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

    const std::vector<CaretDataFile*> allFiles(EventCaretDataFilesGet::getIdentifiableFilesSortedByName());
    
    const int32_t numFiles = static_cast<int32_t>(allFiles.size());
    
    updateTableRows(numFiles);
    
    CaretAssert(rowCount() >= numFiles);
    
    /*
     * There may be files with identical names but different paths.
     * This function will find files with identical names and append
     * the minimum path to disambiguate these files
     */
    std::vector<AString> displayNames;
    FilePathNamePrefixCompactor::removeMatchingPathPrefixFromCaretDataFiles(allFiles,
                                                                            displayNames);
    
    for (int32_t iRow = 0; iRow < numFiles; iRow++) {
        CaretAssertVectorIndex(allFiles, iRow);
        CaretDataFile* caretDataFile(allFiles[iRow]);
        CaretAssert(caretDataFile);
        CaretMappableDataFile* mapFile(caretDataFile->castToCaretMappableDataFile());
        MediaFile* mediaFile(caretDataFile->castToMediaFile());
        HistologySlicesFile* histologySlicesFile(caretDataFile->castToHistologySlicesFile());
        
        if ((mapFile == NULL)
            && (histologySlicesFile == NULL)
            && (mediaFile == NULL)) {
            const AString txt("File is neither mappable nor histology nor media");
            CaretAssertMessage(0, txt);
            CaretLogSevere(txt);
            continue;
        }
        CaretAssertVectorIndex(m_dataFiles, iRow);
        m_dataFiles[iRow] = caretDataFile;
        
        FileIdentificationAttributes* idAtts = caretDataFile->getFileIdentificationAttributes();
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
                    if (displayedFilesEvent.isDataFileDisplayed(caretDataFile)) {
                        tableItem->setText("Yes");
                    }
                    else {
                        tableItem->setText("   ");
                    }
                    break;
                case COLUMN_DISPLAY_MODE_COMBO_BOX:
                {
                    CaretAssert(widget);
                    QComboBox* comboBox = qobject_cast<QComboBox*>(widget);
                    CaretAssert(comboBox);
                    const FileIdentificationDisplayModeEnum::Enum displayMode = idAtts->getDisplayMode();
                    const int32_t integerValue = FileIdentificationDisplayModeEnum::toIntegerCode(displayMode);
                    comboBox->setCurrentIndex(integerValue);
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
                    
                    if (mapFile != NULL) {
                        for (int32_t jMap = 0; jMap < mapFile->getNumberOfMaps(); jMap++) {
                            comboBox->addItem(mapFile->getMapName(jMap));
                        }
                    }
                    else if (histologySlicesFile != NULL) {
                        for (int32_t sliceIndex = 0; sliceIndex < histologySlicesFile->getNumberOfHistologySlices(); sliceIndex++) {
                            const AString& sliceName(histologySlicesFile->getSliceNameBySliceIndex(sliceIndex));
                            comboBox->addItem(sliceName);
                        }
                    }
                    else if (mediaFile != NULL) {
                        for (int32_t jMap = 0; jMap < mediaFile->getNumberOfFrames(); jMap++) {
                            comboBox->addItem(mediaFile->getFrameName(jMap));
                        }
                    }
                    else {
                        CaretAssert(0);
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
            horizontalHeader()->setSectionResizeMode(COLUMN_DISPLAY_MODE_COMBO_BOX, QHeaderView::ResizeToContents);
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

    CaretAssertVectorIndex(m_dataFiles, row);
    FileIdentificationAttributes* atts = m_dataFiles[row]->getFileIdentificationAttributes();
    CaretAssert(atts);
    atts->setMapSelectionMode(value);
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
    
    CaretAssertVectorIndex(m_dataFiles, row);
    FileIdentificationAttributes* atts = m_dataFiles[row]->getFileIdentificationAttributes();
    CaretAssert(atts);
    atts->setMapIndex(mapIndex);
}

/**
 * Called when a display mode combo box is changed
 * @param row
 * Row containing the display mode combo box
 */
void
IdentificationFileFilteringTableWidget::displayModeComboBoxChanged(int row)
{
    CaretAssertVectorIndex(m_displayModeComboBoxes, row);
    const FileIdentificationDisplayModeEnum::Enum value =
    m_displayModeComboBoxes[row]->getSelectedItem<FileIdentificationDisplayModeEnum, FileIdentificationDisplayModeEnum::Enum>();
    
    CaretAssertVectorIndex(m_dataFiles, row);
    FileIdentificationAttributes* atts = m_dataFiles[row]->getFileIdentificationAttributes();
    CaretAssert(atts);
    atts->setDisplayMode(value);
}

