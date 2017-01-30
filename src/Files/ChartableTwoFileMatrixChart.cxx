
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
    
    int32_t numRows = 0;
    int32_t numCols = 0;
    
    if (m_matrixContentType != ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED) {
        CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
        CaretAssert(ciftiMapFile);
        ciftiMapFile->helpMapFileGetMatrixDimensions(numRows,
                                                     numCols);
    }
    
    if ((numRows > 0)
        && (numCols > 0)) {
        /* OK */
    }
    else {
        m_matrixContentType = ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED;
    }
    
    
    updateChartTwoCompoundDataTypeAfterFileChanges(ChartTwoCompoundDataType::newInstanceForMatrix(numRows,
                                                                                               numCols));
    
    m_rowColumnDimension = ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW;
    if ( ! m_validRowColumnSelectionDimensions.empty()) {
        CaretAssertVectorIndex(m_validRowColumnSelectionDimensions, 0);
        m_rowColumnDimension = m_validRowColumnSelectionDimensions[0];
    }
    
    m_sceneAssistant->add<ChartTwoMatrixLoadingDimensionEnum, ChartTwoMatrixLoadingDimensionEnum::Enum>("m_rowColumnDimension",
                                                                                                        &m_rowColumnDimension);
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
    
//    bool useMapFileHelperFlag = false;
//    bool useMatrixFileHelperFlag = false;
//    
//    std::vector<int32_t> parcelReorderedRowIndices;
//    
//    switch (ciftiMapFile->getDataFileType()) {
//        case DataFileTypeEnum::CONNECTIVITY_DENSE:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_PARCEL:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_DENSE_TIME_SERIES:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL:
//        {
//            useMatrixFileHelperFlag    = true;
//            
//            const CiftiConnectivityMatrixParcelFile* parcelConnFile = dynamic_cast<const CiftiConnectivityMatrixParcelFile*>(ciftiMapFile);
//            CaretAssert(parcelConnFile);
//            if (parcelConnFile != NULL) {
//                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
//                int32_t parcelLabelFileMapIndex = -1;
//                bool reorderingEnabledFlag = false;
//                
//                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
//                parcelConnFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
//                                                                              parcelLabelReorderingFile,
//                                                                              parcelLabelFileMapIndex,
//                                                                              reorderingEnabledFlag);
//                
//                if (reorderingEnabledFlag) {
//                    const CiftiParcelReordering* parcelReordering = parcelConnFile->getParcelReordering(parcelLabelReorderingFile,
//                                                                                                        parcelLabelFileMapIndex);
//                    if (parcelReordering != NULL) {
//                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
//                    }
//                }
//            }
//        }
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_DENSE:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_LABEL:
//        {
//            useMapFileHelperFlag = true;
//            
//            const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(ciftiMapFile);
//            CaretAssert(parcelLabelFile);
//            if (parcelLabelFile != NULL) {
//                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
//                int32_t parcelLabelFileMapIndex = -1;
//                bool reorderingEnabledFlag = false;
//                
//                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
//                parcelLabelFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
//                                                                               parcelLabelReorderingFile,
//                                                                               parcelLabelFileMapIndex,
//                                                                               reorderingEnabledFlag);
//                
//                if (reorderingEnabledFlag) {
//                    const CiftiParcelReordering* parcelReordering = parcelLabelFile->getParcelReordering(parcelLabelReorderingFile,
//                                                                                                         parcelLabelFileMapIndex);
//                    if (parcelReordering != NULL) {
//                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
//                    }
//                }
//            }
//        }
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SCALAR:
//        {
//            useMapFileHelperFlag = true;
//            
//            const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(ciftiMapFile);
//            CaretAssert(parcelScalarFile);
//            if (parcelScalarFile != NULL) {
//                CiftiParcelLabelFile* parcelLabelReorderingFile = NULL;
//                int32_t parcelLabelFileMapIndex = -1;
//                bool reorderingEnabledFlag = false;
//                
//                std::vector<CiftiParcelLabelFile*> parcelLabelFiles;
//                parcelScalarFile->getSelectedParcelLabelFileAndMapForReordering(parcelLabelFiles,
//                                                                                parcelLabelReorderingFile,
//                                                                                parcelLabelFileMapIndex,
//                                                                                reorderingEnabledFlag);
//                
//                if (reorderingEnabledFlag) {
//                    const CiftiParcelReordering* parcelReordering = parcelScalarFile->getParcelReordering(parcelLabelReorderingFile,
//                                                                                                          parcelLabelFileMapIndex);
//                    if (parcelReordering != NULL) {
//                        parcelReorderedRowIndices = parcelReordering->getReorderedParcelIndices();
//                    }
//                }
//            }
//        }
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_PARCEL_SERIES:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_SCALAR_DATA_SERIES:
//            useMatrixFileHelperFlag = true;
//            break;
//        case DataFileTypeEnum::ANNOTATION:
//            break;
//        case DataFileTypeEnum::BORDER:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_FIBER_ORIENTATIONS_TEMPORARY:
//            break;
//        case DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY:
//            break;
//        case DataFileTypeEnum::FOCI:
//            break;
//        case DataFileTypeEnum::IMAGE:
//            break;
//        case DataFileTypeEnum::LABEL:
//            break;
//        case DataFileTypeEnum::METRIC:
//            break;
//        case DataFileTypeEnum::PALETTE:
//            break;
//        case DataFileTypeEnum::RGBA:
//            break;
//        case DataFileTypeEnum::SCENE:
//            break;
//        case DataFileTypeEnum::SPECIFICATION:
//            break;
//        case DataFileTypeEnum::SURFACE:
//            break;
//        case DataFileTypeEnum::UNKNOWN:
//            break;
//        case DataFileTypeEnum::VOLUME:
//            break;
//    }
//    
//    if (( ! useMapFileHelperFlag)
//        && ( ! useMatrixFileHelperFlag)) {
//        CaretAssertMessage(0, "Trying to get matrix from a file that does not support matrix display");
//        return false;
//    }
//    
//    bool validDataFlag = false;
//    if (useMapFileHelperFlag) {
//        validDataFlag = ciftiMapFile->helpMapFileLoadChartDataMatrixRGBA(numberOfRowsOut,
//                                                                                          numberOfColumnsOut,
//                                                                                          parcelReorderedRowIndices,
//                                                                                          rgbaOut);
//    }
//    else if (useMatrixFileHelperFlag) {
//        validDataFlag = ciftiMapFile->helpMatrixFileLoadChartDataMatrixRGBA(numberOfRowsOut,
//                                                                                             numberOfColumnsOut,
//                                                                                             parcelReorderedRowIndices,
//                                                                                             rgbaOut);
//    }
//    switch (m_matrixContentType) {
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_UNSUPPORTED:
//            break;
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_BRAINORDINATE_MAPPABLE:
//            break;
//        case ChartTwoMatrixContentTypeEnum::MATRIX_CONTENT_SERIES:
//            break;
//    }
//    
//    return validDataFlag;
}

/**
 * @return The selected row/column dimension.
 */
ChartTwoMatrixLoadingDimensionEnum::Enum
ChartableTwoFileMatrixChart::getSelectedRowColumnDimension() const
{
    return m_rowColumnDimension;
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
    if (m_rowColumnDimension != rowColumnDimension) {
        m_rowColumnDimension = rowColumnDimension;
        setModified();
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
 * @param rowColumnDimensionOut
 *     Indices if indices are for rows or columns.
 * @param rowIndicesOut
 *     Output with row indices selected.
 * @param columnIndicesOut
 *     Output with column indices selected.
 */
void
ChartableTwoFileMatrixChart::getSelectedRowColumnIndices(const int32_t tabIndex,
                                                         ChartTwoMatrixLoadingDimensionEnum::Enum& rowColumnDimensionOut,
                                                         std::vector<int32_t>& rowIndicesOut,
                                                         std::vector<int32_t>& columnIndicesOut) const
{
    rowColumnDimensionOut = m_rowColumnDimension;
    rowIndicesOut.clear();
    columnIndicesOut.clear();
    
    std::set<int32_t> rowIndicesSet;
    std::set<int32_t> columnIndicesSet;
    
    const CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
    const CiftiMappableConnectivityMatrixDataFile* connMapFile = dynamic_cast<const CiftiMappableConnectivityMatrixDataFile*>(ciftiMapFile);
    if (connMapFile != NULL) {
        const ConnectivityDataLoaded* connDataLoaded = connMapFile->getConnectivityDataLoaded();
        if (connDataLoaded != NULL) {
            int64_t loadedRowIndex = -1;
            int64_t loadedColumnIndex = -1;
            connDataLoaded->getRowColumnLoading(loadedRowIndex,
                                                loadedColumnIndex);
            switch (m_rowColumnDimension) {
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
    
    const CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<const CiftiParcelScalarFile*>(ciftiMapFile);
    if (parcelScalarFile != NULL) {
        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelScalarFile);
        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
        for (auto indx : mapOverlayEvent.getSelectedMapIndices()) {
            columnIndicesSet.insert(indx);
        }
    }
    
    const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(ciftiMapFile);
    if (parcelLabelFile != NULL) {
        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelLabelFile);
        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
        for (auto indx : mapOverlayEvent.getSelectedMapIndices()) {
            columnIndicesSet.insert(indx);
        }
    }
    
    const CiftiScalarDataSeriesFile* scalarDataSeriesFile = dynamic_cast<const CiftiScalarDataSeriesFile*>(ciftiMapFile);
    if (scalarDataSeriesFile != NULL) {
        const int32_t scalarDataSeriesMapIndex = scalarDataSeriesFile->getSelectedMapIndex(tabIndex);
        if (scalarDataSeriesMapIndex >= 0) {
            rowIndicesSet.insert(scalarDataSeriesMapIndex);
        }
    }
    
    rowIndicesOut.insert(rowIndicesOut.end(),
                         rowIndicesSet.begin(),
                         rowIndicesSet.end());
    columnIndicesOut.insert(columnIndicesOut.end(),
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
    
    CiftiMappableDataFile* ciftiMapFile = getCiftiMappableDataFile();
    CiftiMappableConnectivityMatrixDataFile* connMapFile = dynamic_cast<CiftiMappableConnectivityMatrixDataFile*>(ciftiMapFile);
    if (connMapFile != NULL) {
        /*
         * Load data for the row/column.
         * Invalidating coloring results in the necessary update of brainordinate coloring.
         */
        int32_t numRows = -1;
        int32_t numCols = -1;
        getMatrixDimensions(numRows, numCols);
        switch (m_rowColumnDimension) {
            case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_COLUMN:
                if ((rowColumnIndex >= 0)
                    && (rowColumnIndex < numCols)) {
                    connMapFile->loadDataForColumnIndex(rowColumnIndex);
                    connMapFile->invalidateColoringInAllMaps();
                }
                break;
            case ChartTwoMatrixLoadingDimensionEnum::CHART_MATRIX_LOADING_BY_ROW:
                if ((rowColumnIndex >= 0)
                    && (rowColumnIndex < numRows)) {
                    connMapFile->loadDataForRowIndex(rowColumnIndex);
                    connMapFile->invalidateColoringInAllMaps();
                }
                break;
        }
    }
    
    CiftiParcelScalarFile* parcelScalarFile = dynamic_cast<CiftiParcelScalarFile*>(ciftiMapFile);
    if (parcelScalarFile != NULL) {
        CaretAssertToDoWarning();
//        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelScalarFile);
//        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//        for (auto indx : mapOverlayEvent.getSelectedMapIndices()) {
//            columnIndicesSet.insert(indx);
//        }
    }
    
    const CiftiParcelLabelFile* parcelLabelFile = dynamic_cast<const CiftiParcelLabelFile*>(ciftiMapFile);
    if (parcelLabelFile != NULL) {
//        EventCaretMappableDataFileMapsViewedInOverlays mapOverlayEvent(parcelLabelFile);
//        EventManager::get()->sendEvent(mapOverlayEvent.getPointer());
//        for (auto indx : mapOverlayEvent.getSelectedMapIndices()) {
//            columnIndicesSet.insert(indx);
//        }
    }
    
    CiftiScalarDataSeriesFile* scalarDataSeriesFile = dynamic_cast<CiftiScalarDataSeriesFile*>(ciftiMapFile);
    if (scalarDataSeriesFile != NULL) {
        CaretAssertToDoWarning();
        scalarDataSeriesFile->setSelectedMapIndex(tabIndex,
                                                  rowColumnIndex);
//        const int32_t scalarDataSeriesMapIndex = scalarDataSeriesFile->getSelectedMapIndex(tabIndex);
//        if (scalarDataSeriesMapIndex >= 0) {
//            rowIndicesSet.insert(scalarDataSeriesMapIndex);
//        }
    }
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

