
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __DING_ONTOLOGY_TERMS_FILE_DECLARE__
#include "DingOntologyTermsFile.h"
#undef __DING_ONTOLOGY_TERMS_FILE_DECLARE__

#include <cstdint>

#include <QFile>
#include <QStack>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"

#include "qxtcsvmodel.h"

using namespace caret;


    
/**
 * \class caret::DingOntologyTermsFile 
 * \brief Reads a CSV file containing Ding Ontology Terms
 * \ingroup Files
 *
 * The Ding Ontology File is in an Excel Spreadsheet.
 * To create a CSV file:
 *    * Load the Excel file.
 *    * Select File -> Save As
 *    * Set the File FOrmat to CSV UTF=-8 (Comman delimited)(.csv)
 *    * Save the file to caret7_source/src/Resources/Data/DingOntology/DingOntologyTerms.csv
 */

/**
 * @return Name of Ding Ontology File in the source code's Resources directory
 */
AString
DingOntologyTermsFile::getDingOntologyResourcesFileName()
{
    return ":/DingOntology/DingOntologyTerms.csv";
}

/**
 * Constructor.
 */
DingOntologyTermsFile::DingOntologyTermsFile()
: CommaSeparatedValuesFile()
{
    
}

/**
 * Destructor.
 */
DingOntologyTermsFile::~DingOntologyTermsFile()
{
}

/**
 * @return Role value for the abbreviated name
 */
int32_t
DingOntologyTermsFile::getAbbreviatedNameItemRole() const
{
    return m_abbreviatedNameItemRole;
}


/**
 * Create the standard item model that can be loaded into a QTreeView
 * @param csvModel
 *   The model read from the CSV file
 */
void
DingOntologyTermsFile::createModels(QxtCsvModel* csvModel)
{
    /*
     * "Roles" for storing data in a QStandardItem
     */
    int32_t roleCounter(1);
    m_abbreviatedNameItemRole = Qt::UserRole + roleCounter++;
    m_descriptiveNameItemRole = Qt::UserRole + roleCounter++;
    
    CaretAssert(csvModel);
    
    const int32_t csvNumColumns(csvModel->columnCount());
    const int32_t csvNumRows(csvModel->rowCount());
    CaretAssert(csvNumColumns > 0);
    CaretAssert(csvNumRows > 0);
    
    /*
     * In the file, the number of commas indicate the hierarchy:
     * Br,,brain,,,,,,,,,,,,,,,,,
     * F,,,forebrain (prosencephalon),,,,,,,,,,,,,,,,
     * FGM,,,,gray matter of forebrain,,,,,,,,,,,,,,,
     * Tel,,,,,telencephalon,,,,,,,,,,,,,,
     * Cx,,,,,,cerebral cortex,,,,,,,,,,,,,
     * NCx,,,,,,,neocortex (isocortex),,,,,,,,,,,,
     */
    
    /*
     * In the pair, 'first' is the QStandardItem that contains the abbreviated
     * name and descriptive name.  Second item is the number of commas between
     * the abbreviated name and descriptive name and indicates level in hierarchy.
     */
    QStack<std::pair<QStandardItem*, int32_t>> itemAndLevelStack;

    /*
     * Create the standard item model that will contain a table structure
     */
    m_tableModel.reset(new QStandardItemModel());
    QStringList tableColumnNames;
    tableColumnNames << getAbbreviatedNameTitle() << getDescriptiveNameTitle();
    m_tableModel->setHorizontalHeaderLabels(tableColumnNames);

    /*
     * Create the standard item model that will contain a tree structure
     */
    m_treeModel.reset(new QStandardItemModel());
    /*
     * Push the invisible root item onto the stack
     */
    itemAndLevelStack.push(std::make_pair(m_treeModel->invisibleRootItem(), -1));
    
    /*
     * Loop through the rows of the CSV model
     */
    for (int32_t iRow = 0; iRow < csvNumRows; iRow++) {
        AString abbreviatedName;
        AString descriptiveName;
        int32_t columnOffset(0);
        
        /*
         * Get the abbreviated and descriptive names and level in the hierarchy
         */
        for (int32_t iCol = 0; iCol < csvNumColumns; iCol++) {
            QModelIndex modelIndex(csvModel->index(iRow, iCol));
            const AString text(csvModel->data(modelIndex).toString());
            if (iCol == 0) {
                abbreviatedName = text.trimmed();
            }
            else if ( ! text.isEmpty()) {
                descriptiveName = text;
                columnOffset = iCol;
                break;
            }
        }
        
        /*
         * Remove any double quotes at both ends
         */
        abbreviatedName = stripDoubleQuotes(abbreviatedName);
        descriptiveName = stripDoubleQuotes(descriptiveName);
        
        /*
         * Ignore empty lines
         */
        if (abbreviatedName.isEmpty()) {
            continue;
        }
        
        QStandardItem* newTreeItem(createStandardItem(abbreviatedName,
                                                      descriptiveName));
                               
        if (itemAndLevelStack.isEmpty()) {
            CaretLogSevere("Program Failure: Stack for creating standard item is empty for abbreviated name "
                           + abbreviatedName);
            return;
        }
        else if (columnOffset > itemAndLevelStack.top().second) {
            /*
             * Item is a child of item on top of the stack
             */
            itemAndLevelStack.top().first->appendRow(newTreeItem);
            itemAndLevelStack.push(std::make_pair(newTreeItem, columnOffset));
        }
        else  {
            /*
             * Remove items from stack until we get back to the
             * same indentation level
             */
            while (itemAndLevelStack.top().second >= columnOffset) {
                itemAndLevelStack.pop();
                if (itemAndLevelStack.isEmpty()) {
                    CaretLogSevere("Error creating Ding Ontology Tree - EMPTY STACK");
                    break;
                }
            }
            CaretAssert( ! itemAndLevelStack.isEmpty());
            itemAndLevelStack.top().first->appendRow(newTreeItem);
            itemAndLevelStack.push(std::make_pair(newTreeItem, columnOffset));
        }
        
        {
            /*
             * Items for the table.
             * Each column uses a separate item.
             * Note: QCompleter uses the EditRole
             */
            QStandardItem* tableAbbreviatedItem(createStandardItem(abbreviatedName,
                                                             descriptiveName));
            tableAbbreviatedItem->setData(tableAbbreviatedItem->text(),
                                          Qt::EditRole);

            tableAbbreviatedItem->setText(abbreviatedName);
            
            QStandardItem* tableDescriptiveItem(createStandardItem(abbreviatedName,
                                                            descriptiveName));
            tableDescriptiveItem->setData(descriptiveName,
                                          Qt::EditRole);
            tableDescriptiveItem->setText(descriptiveName);
            
            QStandardItem* completerItem(createStandardItem(abbreviatedName,
                                                            descriptiveName));
            completerItem->setData(completerItem->text(),
                                   Qt::EditRole);
                                         
            QList<QStandardItem*> itemList;
            itemList << tableAbbreviatedItem << tableDescriptiveItem << completerItem;
            
            m_tableModel->appendRow(itemList);
        }
    }
}

/**
 * @return A new standard item created from the abbreviated and descriptive names
 * @param abbreviatedName
 *    The abbreviated name
 * @param descriptiveName
 *    The descriptive name
 */
QStandardItem*
DingOntologyTermsFile::createStandardItem(const QString& abbreviatedName,
                                          const QString& descriptiveName) const
{
    const QString itemName(abbreviatedName
                           + "  -  "
                           + descriptiveName);
    
    QStandardItem* item(new QStandardItem(itemName));
    item->setData(abbreviatedName,
                  m_abbreviatedNameItemRole);
    item->setData(descriptiveName,
                  m_descriptiveNameItemRole);
    return item;
}

/**
 * @return The table model
 */
QStandardItemModel*
DingOntologyTermsFile::getTableModel() const
{
    return m_tableModel.get();
}


/**
 * @return The standard item model for use in a QTreeView
 */
QStandardItemModel*
DingOntologyTermsFile::getTreeModel() const
{
    return m_treeModel.get();
}

/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
DingOntologyTermsFile::readFile(const AString& filename)
{
    CommaSeparatedValuesFile::readFile(filename);
        
    createModels(getCsvModel());
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
DingOntologyTermsFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("Writing of Ding Ontology Terms file not supported");
}

/**
 * @return Is the file empty
 */
bool
DingOntologyTermsFile::isEmpty() const
{
    if (m_tableModel) {
        if (m_tableModel->rowCount() > 0) {
            return false;
        }
    }
    return true;
}

/**
 * @return Abbreviated name associated with the given standard item
 * @param item
 *    The standard item
 */
QString
DingOntologyTermsFile::getAbbreviatedName(const QStandardItem* item) const
{
    CaretAssert(item);
    return item->data(m_abbreviatedNameItemRole).toString();
}

/**
 * @return Descriptive name associated with the given standard item
 * @param item
 *    The standard item
 */
QString
DingOntologyTermsFile::getDescriptiveName(const QStandardItem* item) const
{
    CaretAssert(item);
    return item->data(m_descriptiveNameItemRole).toString();
}


