
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

#define __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__
#include "CiftiScalarDataSeriesFile.h"
#undef __CIFTI_SCALAR_DATA_SERIES_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "ChartMatrixDisplayProperties.h"
#include "CiftiFile.h"
#include "FastStatistics.h"
#include "NodeAndVoxelColoring.h"

using namespace caret;


    
/**
 * \class caret::CiftiScalarDataSeriesFile 
 * \brief CIFTI Scalar Data Series File
 * \ingroup Files
 */

/**
 * Constructor.
 */
CiftiScalarDataSeriesFile::CiftiScalarDataSeriesFile()
: CiftiMappableDataFile(DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_chartingEnabledForTab[i] = false;
        m_chartMatrixDisplayProperties[i] = new ChartMatrixDisplayProperties();
    }
    
}

/**
 * Destructor.
 */
CiftiScalarDataSeriesFile::~CiftiScalarDataSeriesFile()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        delete m_chartMatrixDisplayProperties[i];
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    The event.
 */
void
CiftiScalarDataSeriesFile::receiveEvent(Event* event)
{
    
}


/**
 * Get the matrix dimensions.
 *
 * @param numberOfRowsOut
 *    Number of rows in the matrix.
 * @param numberOfColumnsOut
 *    Number of columns in the matrix.
 */
void
CiftiScalarDataSeriesFile::getMatrixDimensions(int32_t& numberOfRowsOut,
                                                       int32_t& numberOfColumnsOut) const
{
    helpMapFileGetMatrixDimensions(numberOfRowsOut,
                                   numberOfColumnsOut);
}

/**
 * Get the matrix RGBA coloring for this matrix data creator.
 *
 * @param numberOfRowsOut
 *    Number of rows in the coloring matrix.
 * @param numberOfColumnsOut
 *    Number of rows in the coloring matrix.
 * @param rgbaOut
 *    RGBA coloring output with number of elements
 *    (numberOfRowsOut * numberOfColumnsOut * 4).
 * @return
 *    True if data output data is valid, else false.
 */
bool
CiftiScalarDataSeriesFile::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                                                     int32_t& numberOfColumnsOut,
                                                     std::vector<float>& rgbaOut) const
{
    std::vector<int32_t> rowIndices;
    return helpMatrixFileLoadChartDataMatrixRGBA(numberOfRowsOut,
                                                 numberOfColumnsOut,
                                                 rowIndices,
                                                 rgbaOut);
}

/**
 * Get the value, row name, and column name for a cell in the matrix.
 *
 * @param rowIndex
 *     The row index.
 * @param columnIndex
 *     The column index.
 * @param cellValueOut
 *     Output containing value in the cell.
 * @param rowNameOut
 *     Name of row corresponding to row index.
 * @param columnNameOut
 *     Name of column corresponding to column index.
 * @return
 *     True if the output values are valid (valid row/column indices).
 */
bool
CiftiScalarDataSeriesFile::getMatrixCellAttributes(const int32_t rowIndex,
                                                           const int32_t columnIndex,
                                                           AString& cellValueOut,
                                                           AString& rowNameOut,
                                                           AString& columnNameOut) const
{
    if ((rowIndex >= 0)
        && (rowIndex < m_ciftiFile->getNumberOfRows())
        && (columnIndex >= 0)
        && (columnIndex < m_ciftiFile->getNumberOfColumns())) {
        const CiftiXML& xml = m_ciftiFile->getCiftiXML();
        
        const std::vector<CiftiParcelsMap::Parcel>& rowsParcelsMap = xml.getParcelsMap(CiftiXML::ALONG_COLUMN).getParcels();
        CaretAssertVectorIndex(rowsParcelsMap, rowIndex);
        rowNameOut = rowsParcelsMap[rowIndex].m_name;
        
        const std::vector<CiftiParcelsMap::Parcel>& columnsParcelsMap = xml.getParcelsMap(CiftiXML::ALONG_ROW).getParcels();
        CaretAssertVectorIndex(columnsParcelsMap, columnIndex);
        columnNameOut = columnsParcelsMap[columnIndex].m_name;
        
        const int32_t numberOfElementsInRow = m_ciftiFile->getNumberOfColumns();
        std::vector<float> rowData(numberOfElementsInRow);
        m_ciftiFile->getRow(&rowData[0],
                            rowIndex);
        CaretAssertVectorIndex(rowData, columnIndex);
        cellValueOut = AString::number(rowData[columnIndex], 'f', 6);
        
        return true;
    }
    
    return false;
}

/**
 * @return Is charting enabled for this file?
 */
bool
CiftiScalarDataSeriesFile::isMatrixChartingEnabled(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_chartingEnabledForTab[tabIndex];
}

/**
 * @return Return true if the file's current state supports
 * charting data, else false.  Typically a brainordinate file
 * is chartable if it contains more than one map.
 */
bool
CiftiScalarDataSeriesFile::isMatrixChartingSupported() const
{
    return true;
}

/**
 * Set charting enabled for this file.
 *
 * @param enabled
 *    New status for charting enabled.
 */
void
CiftiScalarDataSeriesFile::setMatrixChartingEnabled(const int32_t tabIndex,
                                                            const bool enabled)
{
    CaretAssertArrayIndex(m_chartingEnabledForTab,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    m_chartingEnabledForTab[tabIndex] = enabled;
}

/**
 * Get chart data types supported by the file.
 *
 * @param chartDataTypesOut
 *    Chart types supported by this file.
 */
void
CiftiScalarDataSeriesFile::getSupportedMatrixChartDataTypes(std::vector<ChartDataTypeEnum::Enum>& chartDataTypesOut) const
{
    chartDataTypesOut.clear();
    chartDataTypesOut.push_back(ChartDataTypeEnum::CHART_DATA_TYPE_MATRIX_LAYER);
}

/**
 * @return Chart matrix display properties (const method).
 */
const ChartMatrixDisplayProperties*
CiftiScalarDataSeriesFile::getChartMatrixDisplayProperties(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
}

/**
 * @return Chart matrix display properties.
 */
ChartMatrixDisplayProperties*
CiftiScalarDataSeriesFile::getChartMatrixDisplayProperties(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_chartMatrixDisplayProperties, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_chartMatrixDisplayProperties[tabIndex];
}


