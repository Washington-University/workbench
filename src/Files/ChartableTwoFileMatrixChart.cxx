
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

#define __CHARTABLE_TWO_FILE_MATRIX_CHART_DECLARE__
#include "ChartableTwoFileMatrixChart.h"
#undef __CHARTABLE_TWO_FILE_MATRIX_CHART_DECLARE__

#include "CaretAssert.h"
#include "CiftiConnectivityMatrixParcelFile.h"
#include "CiftiParcelLabelFile.h"
#include "CiftiParcelReordering.h"
#include "CiftiParcelScalarFile.h"
#include "CiftiParcelSeriesFile.h"
#include "CiftiXML.h"
#include "CiftiScalarDataSeriesFile.h"
#include "ConnectivityDataLoaded.h"
#include "EventCaretMappableDataFileMapsViewedInOverlays.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::ChartableTwoFileMatrixChart
 * \brief Implementation of base chart delegate for matrix charts.
 * \ingroup Files
 */

/**
 * Constructor.
 *
 * @param matrixContentType
 *     Content type of the matrix.
 * @param parentCaretMappableDataFile
 *     Parent caret mappable data file that this delegate supports.
 * @param validRowColumnSelectionDimensions
 *      Valid row/column selection dimensions for loading and selection.
 */
ChartableTwoFileMatrixChart::ChartableTwoFileMatrixChart(const ChartTwoMatrixContentTypeEnum::Enum matrixContentType,
                                                         CaretMappableDataFile* parentCaretMappableDataFile,
                                                         std::vector<ChartTwoMatrixLoadingDimensionEnum::Enum>& validRowColumnSelectionDimensions)
: ChartableTwoFileBaseChart(ChartTwoDataTypeEnum::CHART_DATA_TYPE_MATRIX,
                                    parentCaretMappableDataFile),
m_matrixContentType(matrixContentType),
m_validRowColumnSelectionDimensions(validRowColumnSelectionDimensions)
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_numberOfRows = 0;
    m_numberOfColumns = 0;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_parcelLabelFileSelectedColumn[i] = 0;
        m_parcelScalarFileSelectedColumn[i] = 0;
        m_parcelSeriesFileSelectedColumn[i] = 0;
    }
    m_sceneAssistant->addTabIndexedIntegerArray("m_parcelLabelFileSelectedColumn",
                                                m_parcelLabelFileSelectedColumn);
    m_sceneAssistant->addTabIndexedIntegerArray("m_parcelScalarFileSelectedColumn",
                                                m_parcelScalarFileSelectedColumn);
    m_sceneAssistant->addTabIndexedIntegerArray("m_parcelSeriesFileSelectedColumn",
                                                m_parcelSeriesFileSelectedColumn);
    
    if (m_matrixContentType != ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED) {
        CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
        CaretAssert(ciftiMapFile);
        ciftiMapFile->helpMapFileGetMatrixDimensions(m_numberOfRows,
                                                     m_numberOfColumns);
        
        switch (ciftiMapFile->getDataFileType()) {
            case DataFileTypeEnum::ANNOTATION:
                break;
            case DataFileTypeEnum::BORDER:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                break;
            case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL:
                m_matrixDataFileType = MatrixDataFileType::PARCEL;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
                m_matrixDataFileType = MatrixDataFileType::PARCEL_LABEL;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
                m_matrixDataFileType = MatrixDataFileType::PARCEL_SCALAR;
                break;
            case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
                m_matrixDataFileType = MatrixDataFileType::PARCEL_SERIES;
                break;
            case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
                m_matrixDataFileType = MatrixDataFileType::SCALAR_DATA_SERIES;
                break;
            case DataFileTypeEnum::FOCI:
                break;
            case DataFileTypeEnum::IMAGE:
                break;
            case DataFileTypeEnum::LABEL:
                break;
            case DataFileTypeEnum::METRIC:
                break;
            case DataFileTypeEnum::PALETTE:
                break;
            case DataFileTypeEnum::RGBA:
                break;
            case DataFileTypeEnum::SCENE:
                break;
            case DataFileTypeEnum::SPECIFICATION:
                break;
            case DataFileTypeEnum::SURFACE:
                break;
            case DataFileTypeEnum::UNKNOWN:
                break;
            case DataFileTypeEnum::VOLUME:
                break;
        }
        
        bool hasColumnParcelsFlag  = false;
        bool hasColumnMapNamesFlag = false;
        bool hasRowParcelsFlag     = false;
        bool hasRowNumbersFlag       = false;
        switch (m_matrixDataFileType) {
            case MatrixDataFileType::INVALID:
                CaretAssert(0);
                return;
                break;
            case MatrixDataFileType::PARCEL:
                m_parcelFile = dynamic_cast<CiftiConnectivityMatrixParcelFile*>(ciftiMapFile);
                CaretAssert(m_parcelFile);
                m_hasRowSelectionFlag    = true;
                hasRowParcelsFlag        = true;
                m_hasColumnSelectionFlag = true;
                hasColumnParcelsFlag     = true;
                break;
            case MatrixDataFileType::PARCEL_LABEL:
                m_parcelLabelFile = dynamic_cast<CiftiParcelLabelFile*>(ciftiMapFile);
                CaretAssert(m_parcelLabelFile);
                m_hasColumnSelectionFlag  = true;
                hasColumnMapNamesFlag = true;
                break;
            case MatrixDataFileType::PARCEL_SCALAR:
                m_parcelScalarFile = dynamic_cast<CiftiParcelScalarFile*>(ciftiMapFile);
                CaretAssert(m_parcelScalarFile);
                m_hasColumnSelectionFlag  = true;
                hasColumnMapNamesFlag = true;
                break;
            case MatrixDataFileType::PARCEL_SERIES:
                m_parcelSeriesFile = dynamic_cast<CiftiParcelSeriesFile*>(ciftiMapFile);
                CaretAssert(m_parcelSeriesFile);
                m_hasColumnSelectionFlag  = true;
                hasColumnMapNamesFlag = true;
                break;
            case MatrixDataFileType::SCALAR_DATA_SERIES:
                m_scalarDataSeriesFile = dynamic_cast<CiftiScalarDataSeriesFile*>(ciftiMapFile);
                CaretAssert(m_scalarDataSeriesFile);
                break;
        }
        
        if (m_hasRowSelectionFlag) {
            if (hasRowParcelsFlag) {
                const CiftiParcelsMap* rowParcelsMap = ciftiMapFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_COLUMN);
                if (rowParcelsMap != NULL) {
                    const int32_t numRowParcels = rowParcelsMap->getLength();
                    CaretAssert(numRowParcels == m_numberOfRows);
                    for (int32_t i = 0; i < numRowParcels; i++) {
                        m_rowNames.push_back(rowParcelsMap->getIndexName(i));
                        m_rowNumberAndNames.push_back("Row " + AString::number(i + 1) + ": " + rowParcelsMap->getIndexName(i));
                    }
                }
                else {
                    const CiftiParcelsMap* columnParcelsMap = ciftiMapFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_ROW);
                    if (columnParcelsMap != NULL) {
                        CaretAssertMessage(0, ("Trying to use row parcels but parcels are in columns.  There are "
                                               + AString::number(columnParcelsMap->getLength())
                                               + " parcels in the column for the file "
                                               + getCaretMappableDataFile()->getFileNameNoPath()));
                    }
                    else {
                        CaretAssertMessage(0, "Trying to use row parcels but there are none for the file "
                                           + getCaretMappableDataFile()->getFileNameNoPath());
                    }
                }
            }
            else if (hasRowNumbersFlag) {
                for (int32_t i = 0; i < m_numberOfRows; i++) {
                    m_rowNames.push_back(AString::number(i));
                    m_rowNumberAndNames.push_back("Row " + AString::number(i + 1) + ": " + AString::number(i + 1));
                }
            }
        }
        
        if (m_hasColumnSelectionFlag) {
            if (hasColumnParcelsFlag) {
                const CiftiParcelsMap* colParcelsMap = ciftiMapFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_ROW);
                if (colParcelsMap != NULL) {
                    const int32_t numColParcels = colParcelsMap->getLength();
                    CaretAssert(numColParcels == m_numberOfColumns);
                    for (int32_t i = 0; i < numColParcels; i++) {
                        m_columnNames.push_back(colParcelsMap->getIndexName(i));
                        m_columnNumberAndNames.push_back("Column " + AString::number(i + 1) + ": " + colParcelsMap->getIndexName(i));
                    }
                }
                else {
                    const CiftiParcelsMap* rowParcelsMap = ciftiMapFile->getCiftiParcelsMapForDirection(CiftiXML::ALONG_COLUMN);
                    if (rowParcelsMap != NULL) {
                        CaretAssertMessage(0, ("Trying to use column parcels but parcels are in rows.  There are "
                                               + AString::number(rowParcelsMap->getLength())
                                               + " parcels in the row for the file "
                                               + getCaretMappableDataFile()->getFileNameNoPath()));
                    }
                    else {
                        CaretAssertMessage(0, "Trying to use column parcels but there are none for the file "
                                           + getCaretMappableDataFile()->getFileNameNoPath());
                    }
                }
            }
            else if (hasColumnMapNamesFlag) {
                const int32_t numMaps = ciftiMapFile->getNumberOfMaps();
                CaretAssert(m_numberOfColumns == numMaps);
                for (int32_t i = 0; i < numMaps; i++) {
                    m_columnNames.push_back(ciftiMapFile->getMapName(i));
                    m_columnNumberAndNames.push_back("Column " + AString::number(i + 1) + ": " + ciftiMapFile->getMapName(i));
                }
            }
            
//            if (hasColumnMapSelectionFlag) {
//                m_columnNames.clear();
//                for (int32_t i = 0; i < m_numberOfColumns; i++) {
//                    m_columnNames.push_back(ciftiMapFile->getMapName(i));
//                }
//                if (m_parcelScalarFile != NULL) {
//                    for (int32_t i = 0; i < m_numberOfColumns; i++) {
//                        m_columnNames.push_back(m_parcelScalarFile->getMapName(i));
//                    }
//                }
//                if (m_parcelSeriesFile != NULL) {
//                    for (int32_t i = 0; i < m_numberOfColumns; i++) {
//                        m_columnNames.push_back(m_parcelSeriesFile->getMapName(i));
//                    }
//                }
//            }
        }
    }
    
    m_matrixTriangularViewingModeSupportedFlag = false;
    if ((m_numberOfRows > 0)
        && (m_numberOfColumns > 0)) {
        const CiftiConnectivityMatrixParcelFile* matrixFile = dynamic_cast<const CiftiConnectivityMatrixParcelFile*>(parentCaretMappableDataFile);
        if (matrixFile != NULL) {
            m_matrixTriangularViewingModeSupportedFlag = matrixFile->hasSymetricRowColumnNames();
        }
    }
    else {
        m_matrixContentType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED;
    }
    
    
    updateChartTwoCompoundDataTypeAfterFileChanges(ChartTwoCompoundDataType::newInstanceForMatrix(m_numberOfRows,
                                                                                                  m_numberOfColumns));
}

/**
 * Destructor.
 */
ChartableTwoFileMatrixChart::~ChartableTwoFileMatrixChart()
{
    delete m_sceneAssistant;
}

/**
 * @return Content type of the matrix.
 */
ChartTwoMatrixContentTypeEnum::Enum
ChartableTwoFileMatrixChart::getMatrixContentType() const
{
    return m_matrixContentType;
}

/**
 * @return Is this charting valid ?
 */
bool
ChartableTwoFileMatrixChart::isValid() const
{
    return (m_matrixContentType != ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED);
}

/**
 * @retrurn Is this charting empty (no data at this time)
 */
bool
ChartableTwoFileMatrixChart::isEmpty() const
{
    int32_t numRows = 0;
    int32_t numCols = 0;
    getMatrixDimensions(numRows,
                        numCols);
    if ((numRows > 0)
        || (numCols > 0)) {
        return false;
    }
    
    return true;
}

/**
 * @return Is matrix triangular viewing modes supported?
 */
bool
ChartableTwoFileMatrixChart::isMatrixTriangularViewingModeSupported() const
{
    return m_matrixTriangularViewingModeSupportedFlag;
}


/**
 * Get the matrix dimensions.
 *
 * @param numberOfRowsOut
 *    Number of rows in the matrix.
 * @param numberOfColumnsOut
 *    Number of rows in the matrix.
 */
void
ChartableTwoFileMatrixChart::getMatrixDimensions(int32_t& numberOfRowsOut,
                         int32_t& numberOfColumnsOut) const
{
    numberOfRowsOut = 0;
    numberOfColumnsOut = 0;
    
    if (m_matrixContentType == ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED) {
        return;
    }
    
    const CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
    CaretAssert(ciftiMapFile);
    ciftiMapFile->helpMapFileGetMatrixDimensions(numberOfRowsOut,
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
ChartableTwoFileMatrixChart::getMatrixDataRGBA(int32_t& numberOfRowsOut,
                       int32_t& numberOfColumnsOut,
                       std::vector<float>& rgbaOut) const
{
    
    const CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
    CaretAssert(ciftiMapFile);
    
    return ciftiMapFile->getMatrixForChartingRGBA(numberOfRowsOut,
                                                  numberOfColumnsOut,
                                                  rgbaOut);
}

/**
 * @return The selected row/column dimension.
 */
ChartTwoMatrixLoadingDimensionEnum::Enum
ChartableTwoFileMatrixChart::getSelectedRowColumnDimension() const
{
    ChartTwoMatrixLoadingDimensionEnum::Enum loadDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
    
    switch (m_matrixDataFileType) {
        case MatrixDataFileType::INVALID:
            CaretAssert(0);
            break;
        case MatrixDataFileType::PARCEL:
            CaretAssert(m_parcelFile);
            switch (m_parcelFile->getMatrixLoadingDimension()) {
                case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                    loadDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
                    break;
                case ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                    loadDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
                    break;
            }
            break;
        case MatrixDataFileType::PARCEL_LABEL:
            CaretAssert(m_parcelLabelFile);
            loadDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
            break;
        case MatrixDataFileType::PARCEL_SCALAR:
            CaretAssert(m_parcelScalarFile);
            loadDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
            break;
        case MatrixDataFileType::PARCEL_SERIES:
            CaretAssert(m_parcelSeriesFile);
            loadDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
            break;
        case MatrixDataFileType::SCALAR_DATA_SERIES:
            CaretAssert(m_scalarDataSeriesFile);
            break;
    }
    
    return loadDimension;
}

/**
 * Set the selected row/column dimension.
 *
 * @param rowColumnDimension
 *     New value for row/column dimension
 */
void
ChartableTwoFileMatrixChart::setSelectedRowColumnDimension(const ChartTwoMatrixLoadingDimensionEnum::Enum rowColumnDimension)
{
    switch (m_matrixDataFileType) {
        case MatrixDataFileType::INVALID:
            CaretAssert(0);
            break;
        case MatrixDataFileType::PARCEL:
        {
            CaretAssert(m_parcelFile);
            ChartMatrixLoadingDimensionEnum::Enum oldLoadDim = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
            switch (rowColumnDimension) {
                case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                    oldLoadDim = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
                    break;
                case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                    oldLoadDim = ChartMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN;
                    break;
            }
            m_parcelFile->setMatrixLoadingDimension(oldLoadDim);
        }
            break;
        case MatrixDataFileType::PARCEL_LABEL:
            CaretAssert(m_parcelLabelFile);
            break;
        case MatrixDataFileType::PARCEL_SCALAR:
            CaretAssert(m_parcelScalarFile);
            break;
        case MatrixDataFileType::PARCEL_SERIES:
            CaretAssert(m_parcelSeriesFile);
            break;
        case MatrixDataFileType::SCALAR_DATA_SERIES:
            CaretAssert(m_scalarDataSeriesFile);
            break;
    }
}

/**
 * Get the valid row/column dimensions.
 *
 * @param validRowColumnSelectionDimensionsOut
 *    Output with valid row/column dimensions for selection.
 */
void
ChartableTwoFileMatrixChart::getValidRowColumnSelectionDimensions(std::vector<ChartTwoMatrixLoadingDimensionEnum::Enum>& validRowColumnSelectionDimensionsOut) const
{
    validRowColumnSelectionDimensionsOut = m_validRowColumnSelectionDimensions;
}

/**
 * @param tabIndex
 *     Index of the tab.
 * @param selectedRowIndicesOut
 *     Indices if indices are for rows or columns.
 * @param selectedRowIndicesOut
 *     Output with row indices selected.
 * @param selectedColumnIndicesOut
 *     Output with column indices selected.
 */
void
ChartableTwoFileMatrixChart::getSelectedRowColumnIndices(const int32_t tabIndex,
                                                         ChartTwoMatrixLoadingDimensionEnum::Enum& rowColumnDimensionOut,
                                                         std::vector<int32_t>& selectedRowIndicesOut,
                                                         std::vector<int32_t>& selectedColumnIndicesOut) const
{
    rowColumnDimensionOut = getSelectedRowColumnDimension();
    selectedRowIndicesOut.clear();
    selectedColumnIndicesOut.clear();
    
    std::set<int32_t> rowIndicesSet;
    std::set<int32_t> columnIndicesSet;
    
    switch (m_matrixDataFileType) {
        case MatrixDataFileType::INVALID:
            CaretAssert(0);
            break;
        case MatrixDataFileType::PARCEL:
        {
            CaretAssert(m_parcelFile);
            const ConnectivityDataLoaded* connDataLoaded = m_parcelFile->getConnectivityDataLoaded();
            if (connDataLoaded != NULL) {
                int64_t loadedRowIndex = -1;
                int64_t loadedColumnIndex = -1;
                connDataLoaded->getRowColumnLoading(loadedRowIndex,
                                                    loadedColumnIndex);
                switch (rowColumnDimensionOut) {
                    case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                        if (loadedColumnIndex >= 0) {
                            columnIndicesSet.insert(loadedColumnIndex);
                        }
                        break;
                    case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                        if (loadedRowIndex >= 0) {
                            rowIndicesSet.insert(loadedRowIndex);
                        }
                        break;
                }
            }
        }
            break;
        case MatrixDataFileType::PARCEL_LABEL:
        {
            CaretAssertArrayIndex(m_parcelLabelFileSelectedColumn, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            columnIndicesSet.insert(m_parcelLabelFileSelectedColumn[tabIndex]);
//            CaretAssert(m_parcelLabelFile);
//            EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(m_parcelLabelFile);
//            EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//            for (auto indx : mapOverlayEvent.getSelectedMapIndices()) {
//                columnIndicesSet.insert(indx);
//            }
        }
            break;
        case MatrixDataFileType::PARCEL_SCALAR:
            CaretAssertArrayIndex(m_parcelScalarFileSelectedColumn, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            columnIndicesSet.insert(m_parcelScalarFileSelectedColumn[tabIndex]);
            break;
        case MatrixDataFileType::PARCEL_SERIES:
            CaretAssertArrayIndex(m_parcelSeriesFileSelectedColumn, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            columnIndicesSet.insert(m_parcelSeriesFileSelectedColumn[tabIndex]);
            break;
        case MatrixDataFileType::SCALAR_DATA_SERIES:
        {
            CaretAssert(m_scalarDataSeriesFile);
            const int32_t scalarDataSeriesMapIndex = m_scalarDataSeriesFile->getSelectedMapIndex(tabIndex);
            if (scalarDataSeriesMapIndex >= 0) {
                rowIndicesSet.insert(scalarDataSeriesMapIndex);
            }
        }
            break;
    }
    
    selectedRowIndicesOut.insert(selectedRowIndicesOut.end(),
                         rowIndicesSet.begin(),
                         rowIndicesSet.end());
    selectedColumnIndicesOut.insert(selectedColumnIndicesOut.end(),
                         columnIndicesSet.begin(),
                         columnIndicesSet.end());
}

/**
 * Set the row or column index using the currently selected row/column dimension.
 *
 * @param tabIndex
 *     Index of tab.
 * @param rowColumnIndex
 *     New row/column index.
 */
void
ChartableTwoFileMatrixChart::setSelectedRowColumnIndex(const int32_t tabIndex,
                                                       const int32_t rowColumnIndex)
{
    /*
     * Ignore an invalid row/column index.  It may occur when the chart overlay
     * is changed.
     */
    if (rowColumnIndex < 0) {
        return;
    }
    
    switch (m_matrixDataFileType) {
        case MatrixDataFileType::INVALID:
            CaretAssert(0);
            break;
        case MatrixDataFileType::PARCEL:
        {
            CaretAssert(m_parcelFile);
            int32_t numRows = -1;
            int32_t numCols = -1;
            getMatrixDimensions(numRows, numCols);
            switch (getSelectedRowColumnDimension()) {
                case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                    if ((rowColumnIndex >= 0)
                        && (rowColumnIndex < numCols)) {
                        m_parcelFile->loadDataForColumnIndex(rowColumnIndex);
                        m_parcelFile->invalidateColoringInAllMaps();
                    }
                    break;
                case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                    if ((rowColumnIndex >= 0)
                        && (rowColumnIndex < numRows)) {
                        m_parcelFile->loadDataForRowIndex(rowColumnIndex);
                        m_parcelFile->invalidateColoringInAllMaps();
                    }
                    break;
            }
        }
            break;
        case MatrixDataFileType::PARCEL_LABEL:
        {
            CaretAssert(m_parcelLabelFile);
            CaretAssertArrayIndex(m_parcelLabelFileSelectedColumn, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            m_parcelLabelFileSelectedColumn[tabIndex] = rowColumnIndex;
            //        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(m_parcelLabelFile);
            //        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
            //        for (auto indx : mapOverlayEvent.getSelectedMapIndices()) {
            //            columnIndicesSet.insert(indx);
            //        }
        }
            break;
        case MatrixDataFileType::PARCEL_SCALAR:
        {
            CaretAssert(m_parcelScalarFile);
            CaretAssertArrayIndex(m_parcelScalarFileSelectedColumn, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            m_parcelScalarFileSelectedColumn[tabIndex] = rowColumnIndex;
        }
            break;
        case MatrixDataFileType::PARCEL_SERIES:
            CaretAssert(m_parcelSeriesFile);
            CaretAssertArrayIndex(m_parcelSeriesFileSelectedColumn, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
            m_parcelSeriesFileSelectedColumn[tabIndex] = rowColumnIndex;
            break;
        case MatrixDataFileType::SCALAR_DATA_SERIES:
            CaretAssert(m_scalarDataSeriesFile);
            m_scalarDataSeriesFile->setSelectedMapIndex(tabIndex,
                                                      rowColumnIndex);
            break;
    }
}

/**
 * @return True if the file supports row selection.
 */
bool
ChartableTwoFileMatrixChart::hasRowSelection() const
{
    return m_hasRowSelectionFlag;
}

/**
 * @return True if the file supports column selection.
 */
bool
ChartableTwoFileMatrixChart::hasColumnSelection() const
{
    return m_hasColumnSelectionFlag;
}

/**
 * @return Name of row at the given index.
 *
 * @param rowIndex
 *     Index of the row.
 */
AString
ChartableTwoFileMatrixChart::getRowName(const int32_t rowIndex) const
{
    if ((rowIndex >= 0)
        && (rowIndex < m_numberOfRows)) {
        if (rowIndex < static_cast<int32_t>(m_rowNames.size())) {
            CaretAssertVectorIndex(m_rowNames, rowIndex);
            return m_rowNames[rowIndex];
        }
        return ("Row: " + AString::number(rowIndex));
    }
    
    return "";
}

/**
 * @return Name of column at the given index.
 *
 * @param columnIndex
 *     Index of the row.
 */
AString
ChartableTwoFileMatrixChart::getColumnName(const int32_t columnIndex) const
{
    if ((columnIndex >= 0)
        && (columnIndex < m_numberOfColumns)) {
        if (columnIndex < static_cast<int32_t>(m_columnNames.size())) {
            CaretAssertVectorIndex(m_columnNames, columnIndex);
            return m_columnNames[columnIndex];
        }
        return ("Column: " + AString::number(columnIndex));
    }
    
    return "";
}

/**
 * @return Name of row at the given index.
 *
 * @param rowIndex
 *     Index of the row.
 */
AString
ChartableTwoFileMatrixChart::getRowNumberAndName(const int32_t rowIndex) const
{
    if ((rowIndex >= 0)
        && (rowIndex < m_numberOfRows)) {
        if (rowIndex < static_cast<int32_t>(m_rowNumberAndNames.size())) {
            CaretAssertVectorIndex(m_rowNumberAndNames, rowIndex);
            return m_rowNumberAndNames[rowIndex];
        }
        return ("Row: " + AString::number(rowIndex));
    }
    
    return "";
}

/**
 * @return Name of column at the given index.
 *
 * @param columnIndex
 *     Index of the row.
 */
AString
ChartableTwoFileMatrixChart::getColumnNumberAndName(const int32_t columnIndex) const
{
    if ((columnIndex >= 0)
        && (columnIndex < m_numberOfColumns)) {
        if (columnIndex < static_cast<int32_t>(m_columnNumberAndNames.size())) {
            CaretAssertVectorIndex(m_columnNumberAndNames, columnIndex);
            return m_columnNumberAndNames[columnIndex];
        }
        return ("Column: " + AString::number(columnIndex));
    }
    
    return "";
}

/**
 * Save subclass data to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass to which data members should be added.  Will always
 *     be valid (non-NULL).
 */
void
ChartableTwoFileMatrixChart::saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                            SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
}

/**
 * Restore file data from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass for the instance of a class that implements
 *     this interface.  Will NEVER be NULL.
 */
void
ChartableTwoFileMatrixChart::restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                 const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

