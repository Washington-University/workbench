
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

#define __COMMA_SEPARATED_VALUES_FILE_DECLARE__
#include "CommaSeparatedValuesFile.h"
#undef __COMMA_SEPARATED_VALUES_FILE_DECLARE__

#include <cstdint>

#include <QFile>
#include <QStack>
#include <QStandardItemModel>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"

#include "qxtcsvmodel.h"

using namespace caret;


    
/**
 * \class caret::CommaSeparatedValuesFile
 * \brief Reads a Comma Separated Values File into a QxtCsvModel
 * \ingroup Files
 */

/**
 * Constructor.
 */
CommaSeparatedValuesFile::CommaSeparatedValuesFile()
: DataFile()
{
    
}

/**
 * Destructor.
 */
CommaSeparatedValuesFile::~CommaSeparatedValuesFile()
{
}

/**
 * Print the csv model
 * @param csvModel
 *   The model read from the CSV file
 */
void
CommaSeparatedValuesFile::printCsvModelContent(QxtCsvModel* csvModel)
{
    const int32_t numColumns(csvModel->columnCount());
    const int32_t numRows(csvModel->rowCount());
    
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        for (int32_t iCol = 0; iCol < numColumns; iCol++) {
            QModelIndex modelIndex(csvModel->index(iRow, iCol));
            const QVariant modelData(csvModel->data(modelIndex));
            std::cout << modelData.toString().toStdString() << " || ";
        }
        std::cout << std::endl;
    }
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
CommaSeparatedValuesFile::readFile(const AString& filename)
{
    QFile file(filename);
    if ( ! file.open(QFile::ReadOnly)) {
        throw DataFileException("Unable to open "
                                + filename
                                + " for reading.");
    }
    
    const bool hasHeaderFlag(false);
    const QChar separator(',');
    m_csvModel.reset(new QxtCsvModel());
    m_csvModel->setSource(&file,
                          hasHeaderFlag,
                          separator);
    
    const int32_t numColumns(m_csvModel->columnCount());
    const int32_t numRows(m_csvModel->rowCount());
    
    AString errorMessage;
    if (numRows <= 0) {
        errorMessage.appendWithNewLine("Number of rows is zero.:");
    }
    if (numColumns <= 0) {
        errorMessage.appendWithNewLine("Number of columns is zero.:");
    }
    
    if ( ! errorMessage.isEmpty()) {
        throw DataFileException(errorMessage);
    }
    
    /*printCsvModelContent(m_csvModel.get());*/
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
CommaSeparatedValuesFile::writeFile(const AString& /*filename*/)
{
    throw DataFileException("Writing of Comma Separated Values File not supported");
}

QxtCsvModel*
CommaSeparatedValuesFile::getCsvModel() const
{
    return m_csvModel.get();
}

/**
 * @return Is the file empty
 */
bool
CommaSeparatedValuesFile::isEmpty() const
{
    if (m_csvModel) {
        if (m_csvModel->rowCount() > 0) {
            return false;
        }
    }
    return true;
}

/**
 * @return The input text with first and last characters removed if the first
 * and last characters are double quotes.
 */
AString
CommaSeparatedValuesFile::stripDoubleQuotes(const QString& text) const
{
    static const QChar DOUBLE_QUOTE_CHARACTER('"');

    AString textOut(text.trimmed());
    const int32_t numChars(textOut.length());
    if (numChars >= 2) {
        if (textOut.startsWith(DOUBLE_QUOTE_CHARACTER)
            && textOut.endsWith(DOUBLE_QUOTE_CHARACTER)) {
            textOut = textOut.mid(1, (numChars - 2));
        }
    }
    return textOut;
}



